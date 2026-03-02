#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QString>
#include <QVector>
#include <QPoint>

// ─────────────────────────────────────────────────────────────────
// MeshViewerWidget
//   - Binary STL 파일을 로드하여 OpenGL 3.3 Core 로 렌더링
//   - 마우스 드래그: 회전  /  휠: 확대·축소
//   - loadSimulationMesh(): 스캐너 없는 테스트용 구면 메시 생성
// ─────────────────────────────────────────────────────────────────
class MeshViewerWidget
    : public QOpenGLWidget
    , protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit MeshViewerWidget(QWidget* parent = nullptr);
    ~MeshViewerWidget() override;

    bool loadSTL(const QString& filePath);   // 실제 STL 파일 로드
    void loadSimulationMesh();               // 시뮬레이션용 구면 메시
    void clearMesh();                        // 메시 초기화

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

private:
    bool parseBinarySTL(const QString& path, QVector<float>& posNormals);
    void generateUVSphere();
    void uploadVertexData(const QVector<float>& posNormals);  // 6 floats/vertex

    QOpenGLShaderProgram*    m_shader      = nullptr;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer            m_vbo{ QOpenGLBuffer::VertexBuffer };
    int                      m_vertexCount = 0;
    bool                     m_hasMesh     = false;

    QMatrix4x4 m_projection;
    float      m_rotX    = 20.0f;
    float      m_rotY    = -30.0f;
    float      m_camDist = 3.0f;
    QPoint     m_lastMouse;
    bool       m_dragging = false;
};
