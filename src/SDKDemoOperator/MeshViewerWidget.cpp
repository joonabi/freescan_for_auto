#include "MeshViewerWidget.h"

#include <QFile>
#include <QDataStream>
#include <QFileInfo>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QtMath>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────
// GLSL 330 Core 셰이더 소스
// ─────────────────────────────────────────────────────────────────
static const char* s_vertSrc = R"GLSL(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
uniform mat4 uMVP;
uniform mat3 uNormalMat;
out vec3 vNormal;
void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
    vNormal = normalize(uNormalMat * aNormal);
}
)GLSL";

static const char* s_fragSrc = R"GLSL(
#version 330 core
in vec3 vNormal;
out vec4 fragColor;
void main() {
    vec3 n  = normalize(vNormal);
    vec3 L1 = normalize(vec3( 1.0,  1.5,  2.0));
    vec3 L2 = normalize(vec3(-1.5, -0.8,  0.5));
    float d1 = max(dot(n, L1), 0.0);
    float d2 = max(dot(n, L2), 0.0) * 0.25;
    float amb = 0.18;
    vec3 base = vec3(0.58, 0.68, 0.82);
    vec3 col  = base * (amb + d1 * 0.80 + d2);
    fragColor = vec4(col, 1.0);
}
)GLSL";

// ═════════════════════════════════════════════════════════════════
MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_vbo(QOpenGLBuffer::VertexBuffer)
{
    setMinimumSize(400, 300);
    // 어두운 배경
    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(fmt);
}

MeshViewerWidget::~MeshViewerWidget()
{
    makeCurrent();
    m_vbo.destroy();
    m_vao.destroy();
    delete m_shader;
    doneCurrent();
}

// ─────────────────────────────────────────────────────────────────
// OpenGL 초기화
// ─────────────────────────────────────────────────────────────────
void MeshViewerWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.08f, 0.08f, 0.13f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    m_shader = new QOpenGLShaderProgram(this);
    m_shader->addShaderFromSourceCode(QOpenGLShader::Vertex,   s_vertSrc);
    m_shader->addShaderFromSourceCode(QOpenGLShader::Fragment, s_fragSrc);
    m_shader->link();

    m_vao.create();
    m_vbo.create();
}

void MeshViewerWidget::resizeGL(int w, int h)
{
    float aspect = (h == 0) ? 1.0f : float(w) / float(h);
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, aspect, 0.01f, 200.0f);
}

// ─────────────────────────────────────────────────────────────────
// 렌더링
// ─────────────────────────────────────────────────────────────────
void MeshViewerWidget::paintGL()
{
    glClearColor(0.08f, 0.08f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_hasMesh || m_vertexCount == 0 || !m_shader) return;

    glEnable(GL_DEPTH_TEST);

    QMatrix4x4 view;
    view.lookAt(QVector3D(0, 0, m_camDist), QVector3D(0, 0, 0), QVector3D(0, 1, 0));

    QMatrix4x4 model;
    model.rotate(m_rotX, 1, 0, 0);
    model.rotate(m_rotY, 0, 1, 0);

    QMatrix4x4 mvp = m_projection * view * model;
    QMatrix3x3 normalMat = model.normalMatrix();

    m_shader->bind();
    m_shader->setUniformValue("uMVP",       mvp);
    m_shader->setUniformValue("uNormalMat", normalMat);

    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    m_vao.release();

    m_shader->release();
}

// ─────────────────────────────────────────────────────────────────
// 마우스 이벤트 – 회전 / 확대·축소
// ─────────────────────────────────────────────────────────────────
void MeshViewerWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        m_dragging    = true;
        m_lastMouse   = e->pos();
    }
}

void MeshViewerWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (!m_dragging) return;
    QPoint delta = e->pos() - m_lastMouse;
    m_rotY += delta.x() * 0.5f;
    m_rotX += delta.y() * 0.5f;
    m_lastMouse = e->pos();
    update();
}

void MeshViewerWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) m_dragging = false;
}

void MeshViewerWidget::wheelEvent(QWheelEvent* e)
{
    float delta = e->angleDelta().y() / 120.0f;
    m_camDist -= delta * 0.3f;
    m_camDist = qBound(0.5f, m_camDist, 20.0f);
    update();
}

// ─────────────────────────────────────────────────────────────────
// STL 파일 로드 (Binary STL 전용)
// ─────────────────────────────────────────────────────────────────
bool MeshViewerWidget::loadSTL(const QString& filePath)
{
    QVector<float> posNormals;
    if (!parseBinarySTL(filePath, posNormals)) return false;
    uploadVertexData(posNormals);
    return true;
}

bool MeshViewerWidget::parseBinarySTL(const QString& path, QVector<float>& out)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return false;

    const qint64 fileSize = file.size();
    if (fileSize < 84) return false;  // 80 header + 4 count minimum

    QDataStream ds(&file);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds.setFloatingPointPrecision(QDataStream::SinglePrecision);

    // 80-byte header skip
    file.seek(80);

    quint32 triangleCount = 0;
    ds >> triangleCount;

    if (triangleCount == 0 || triangleCount > 20000000) return false;
    // Validate: 80 header + 4 count + 50*N
    if (fileSize < static_cast<qint64>(80 + 4 + triangleCount * 50LL)) return false;

    // First pass: read raw data + compute bounding box
    QVector<float> raw;
    raw.reserve(static_cast<int>(triangleCount) * 18);

    float minX =  1e9f, minY =  1e9f, minZ =  1e9f;
    float maxX = -1e9f, maxY = -1e9f, maxZ = -1e9f;

    for (quint32 i = 0; i < triangleCount; ++i) {
        float nx, ny, nz;
        float v[3][3];
        quint16 attr;
        ds >> nx >> ny >> nz;
        ds >> v[0][0] >> v[0][1] >> v[0][2];
        ds >> v[1][0] >> v[1][1] >> v[1][2];
        ds >> v[2][0] >> v[2][1] >> v[2][2];
        ds >> attr;

        for (int vi = 0; vi < 3; ++vi) {
            raw << v[vi][0] << v[vi][1] << v[vi][2] << nx << ny << nz;
            minX = std::min(minX, v[vi][0]);
            minY = std::min(minY, v[vi][1]);
            minZ = std::min(minZ, v[vi][2]);
            maxX = std::max(maxX, v[vi][0]);
            maxY = std::max(maxY, v[vi][1]);
            maxZ = std::max(maxZ, v[vi][2]);
        }
    }

    // Center + normalize to fit in [-1, 1]^3
    float cx = (minX + maxX) * 0.5f;
    float cy = (minY + maxY) * 0.5f;
    float cz = (minZ + maxZ) * 0.5f;
    float span = std::max({ maxX - minX, maxY - minY, maxZ - minZ });
    float scale = (span > 1e-6f) ? (2.0f / span) : 1.0f;

    out.reserve(raw.size());
    for (int i = 0; i < raw.size(); i += 6) {
        out << (raw[i]   - cx) * scale;
        out << (raw[i+1] - cy) * scale;
        out << (raw[i+2] - cz) * scale;
        out << raw[i+3];   // normals: not scaled
        out << raw[i+4];
        out << raw[i+5];
    }
    return true;
}

// ─────────────────────────────────────────────────────────────────
// 시뮬레이션용 UV 구면 메시 생성
// ─────────────────────────────────────────────────────────────────
void MeshViewerWidget::loadSimulationMesh()
{
    generateUVSphere();
}

void MeshViewerWidget::generateUVSphere()
{
    const int rings   = 24;
    const int sectors = 24;
    const float PI    = 3.14159265f;

    // Generate grid of (rings+1) x (sectors+1) vertices
    QVector<QVector3D> pos;
    pos.reserve((rings + 1) * (sectors + 1));

    for (int r = 0; r <= rings; ++r) {
        float phi = PI * r / rings;   // 0 → π
        for (int s = 0; s <= sectors; ++s) {
            float theta = 2.0f * PI * s / sectors;
            float x = sinf(phi) * cosf(theta);
            float y = cosf(phi);
            float z = sinf(phi) * sinf(theta);
            pos.append(QVector3D(x, y, z));
        }
    }

    // Build triangle soup (normals == positions for unit sphere)
    QVector<float> data;
    data.reserve(rings * sectors * 2 * 3 * 6);

    auto addV = [&](int r, int s) {
        int idx = r * (sectors + 1) + s;
        auto& p = pos[idx];
        data << p.x() << p.y() << p.z()   // position
             << p.x() << p.y() << p.z();   // normal (same as position on unit sphere)
    };

    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < sectors; ++s) {
            // Triangle 1
            addV(r,   s  );
            addV(r+1, s  );
            addV(r+1, s+1);
            // Triangle 2
            addV(r,   s  );
            addV(r+1, s+1);
            addV(r,   s+1);
        }
    }

    uploadVertexData(data);
}

// ─────────────────────────────────────────────────────────────────
// GPU 업로드 – 정점 레이아웃: [pos.xyz | normal.xyz]  6 floats/vertex
// ─────────────────────────────────────────────────────────────────
void MeshViewerWidget::uploadVertexData(const QVector<float>& posNormals)
{
    makeCurrent();

    m_vao.bind();
    m_vbo.bind();
    m_vbo.allocate(posNormals.constData(),
                   posNormals.size() * static_cast<int>(sizeof(float)));

    constexpr int stride = 6 * sizeof(float);
    // attrib 0: position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    // attrib 1: normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    m_vao.release();
    m_vbo.release();

    m_vertexCount = posNormals.size() / 6;
    m_hasMesh     = (m_vertexCount > 0);

    // 초기 회전 리셋
    m_rotX    =  20.0f;
    m_rotY    = -30.0f;
    m_camDist =  3.0f;

    doneCurrent();
    update();
}

// ─────────────────────────────────────────────────────────────────
// 메시 초기화
// ─────────────────────────────────────────────────────────────────
void MeshViewerWidget::clearMesh()
{
    makeCurrent();
    m_vao.bind();
    m_vbo.bind();
    m_vbo.allocate(nullptr, 0);
    m_vao.release();
    m_vbo.release();
    doneCurrent();

    m_vertexCount = 0;
    m_hasMesh     = false;
    update();
}
