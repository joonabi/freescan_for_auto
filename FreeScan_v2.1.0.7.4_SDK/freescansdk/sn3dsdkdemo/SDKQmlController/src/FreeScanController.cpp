#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "FreeScanController.h"
#include "sn3dsdkdemoapp.h"
#include "sn3dsdkpluginpub.h"

#include <QThread>
#include <QProcess>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDateTime>
#include <QVariantMap>
#include <QDebug>

// ── 내부 상수 ────────────────────────────────────────────────────
static const QString CMD_KEY      = "cmd";
static const QString SCANMODE_KEY = "scanMode";

// ─────────────────────────────────────────────────────────────────
FreeScanController::FreeScanController(Sn3DSDKDemoApp* app, QObject* parent)
    : QObject(parent), m_app(app)
{
    m_elapsedTimer = new QTimer(this);
    m_elapsedTimer->setInterval(1000);
    connect(m_elapsedTimer, &QTimer::timeout, this, &FreeScanController::updateElapsedTime);

    if (m_app) {
        connect(m_app, &Sn3DSDKDemoApp::SnMessageReceived,
                this, &FreeScanController::onSnMessageReceived, Qt::QueuedConnection);
        connect(m_app, &Sn3DSDKDemoApp::newProjFinished,
                this, &FreeScanController::onNewProjFinished, Qt::QueuedConnection);
        connect(m_app, &Sn3DSDKDemoApp::meshDataReturn,
                this, &FreeScanController::onMeshDataReturn, Qt::QueuedConnection);
        connect(m_app, &Sn3DSDKDemoApp::saveMeshDataReturn,
                this, &FreeScanController::onSaveMeshDataReturn, Qt::QueuedConnection);
    }
}

// ── Setters ──────────────────────────────────────────────────────
void FreeScanController::setWifiIp(const QString& ip)
{
    if (m_wifiIp != ip) { m_wifiIp = ip; emit wifiIpChanged(); }
}

void FreeScanController::setWifiPort(int port)
{
    if (m_wifiPort != port) { m_wifiPort = port; emit wifiPortChanged(); }
}

// ── QML 호출 메서드 ───────────────────────────────────────────────
void FreeScanController::connectScanner()
{
    if (!m_app) {
        appendLog("[시뮬] SDK 없음 — 시뮬레이션 연결");
        setStatus("connecting");
        setStatusMessage("시뮬레이션 연결 중...");
        QTimer::singleShot(1500, this, [this]() {
            setIsConnected(true);
            setStatus("success");
            setStatusMessage("시뮬레이션 연결 완료");
            m_deviceModel = "Freescan UE Pro2 (Sim)";
            emit deviceModelChanged();
            emit connectionSuccess();
        });
        return;
    }

    // WiFi 연결 (UEP2)
    setStatus("connecting");
    setStatusMessage("WiFi 연결 준비 중...");
    appendLog(QString("[연결] WiFi %1:%2 연결 시도").arg(m_wifiIp).arg(m_wifiPort));

    QString ip   = m_wifiIp;
    int     port = m_wifiPort;
    QThread* worker = QThread::create([this, ip, port]() {
        doConnectWiFi(ip, port);
    });
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void FreeScanController::disconnectScanner()
{
    if (m_app) {
        m_app->Sn3DRecleaseSocket();
    }
    setIsConnected(false);
    setStatus("idle");
    setStatusMessage("연결 해제됨");
    m_waitingForScan = false;
    appendLog("[연결] 스캐너 연결 해제");
}

void FreeScanController::startScan(const QVariantMap& preset)
{
    if (!m_isConnected && m_app) {
        emit errorOccurred("스캐너가 연결되지 않았습니다");
        return;
    }

    m_activePreset = preset;

    // 프로젝트 경로 생성: saveDirectory/yyyyMMdd_HH_mm_ss
    QString saveDir = preset.value("saveDirectory", "C:/ScanResults").toString();
    QString workspace = preset.value("workspace", "").toString();
    QString ts = QDateTime::currentDateTime().toString("yyyyMMdd_HH_mm_ss");
    QString folderName = workspace.isEmpty() ? ts : (workspace + "_" + ts);
    // 파일명에 부적절한 문자 치환
    folderName.replace(QRegExp("[\\\\/:*?\"<>|]"), "_");
    m_projectPath = saveDir + "/" + folderName;

    appendLog(QString("[프로젝트] 생성 경로: %1").arg(m_projectPath));

    setStatus("running");
    setStatusMessage("프로젝트 생성 중...");
    setProgress(15);

    if (!m_app) {
        // 시뮬레이션 모드
        appendLog("[시뮬] 프로젝트 생성 → 스캔 대기 중...");
        m_waitingForScan = true;
        setIsScanning(true);
        setStatus("waiting");
        setStatusMessage("스캐너에서 스캔을 시작하세요");
        setCurrentStep("스캐너 스캔 신호 대기 중...");
        m_scanStartTime = QDateTime::currentDateTime();
        m_startTime = m_scanStartTime.toString("hh:mm:ss");
        emit startTimeChanged();
        m_elapsedSeconds = 0;
        m_elapsedTimer->start();

        // 시뮬: 5초 후 자동으로 SNSDK_SCAN 신호 시뮬레이션
        QTimer::singleShot(5000, this, [this]() {
            if (m_waitingForScan) {
                appendLog("[시뮬] 스캔 시작 신호 수신");
                setCurrentStep("3D 스캔 진행 중...");
                setStatus("scanning");
                setStatusMessage("스캔 중...");
                setProgress(35);
                // 8초 후 스캔 완료 시뮬
                QTimer::singleShot(8000, this, [this]() {
                    appendLog("[시뮬] 스캔 완료 신호 수신 → 메시 생성 중");
                    m_waitingForScan = false;
                    setProgress(65);
                    setCurrentStep("포인트 클라우드 생성 중...");
                    setStatus("meshing");
                    setStatusMessage("메시 변환 중...");
                    QTimer::singleShot(3000, this, [this]() {
                        setProgress(90);
                        setCurrentStep("파일 저장 중...");
                        setStatus("saving");
                        setStatusMessage("저장 중...");
                        QTimer::singleShot(2000, this, [this]() {
                            setProgress(100);
                            setCurrentStep("스캔 완료");
                            setStatus("completed");
                            setStatusMessage("스캔 완료!");
                            setIsScanning(false);
                            m_elapsedTimer->stop();
                            appendLog("[시뮬] 저장 완료");
                            emit scanCompleted();
                        });
                    });
                });
            }
        });
        return;
    }

    // 실제 SDK: Sn3DNewProject
    // lightSource: "Laser" → SNSDK_SCANMODE_LASER(0), "IR" → SNSDK_SCANMODE_INFRARED(2)
    QString lightSrc = preset.value("lightSource", "Laser").toString();
    int scanMode = (lightSrc == "IR") ? SNSDK_SCANMODE_INFRARED : SNSDK_SCANMODE_LASER;
    double ptDist = preset.value("pointDistance", 0.5).toDouble();
    // scanMode(alignment): "marker" → iAlignType=1(Marker_Feature), otherwise 0
    QString alignModeStr = preset.value("scanMode", "mesh").toString();
    int alignType = (alignModeStr == "marker") ? 1 : 0;

    // szSlnDirPath / szGlobalMarkerPath 는 const char* 포인터
    QByteArray pathBytes   = m_projectPath.toUtf8();
    QByteArray markerBytes = QByteArray("");

    NewProject newProj;
    memset(&newProj, 0, sizeof(NewProject));
    newProj.szSlnDirPath       = pathBytes.constData();
    newProj.iScanMode          = scanMode;
    newProj.hasTexture         = false;
    newProj.fPointDis          = static_cast<float>(ptDist);
    newProj.iAlignType         = alignType;
    newProj.szGlobalMarkerPath = markerBytes.constData();

    int ret = m_app->Sn3DNewProject(&newProj);

    if (ret != 0) {
        appendLog(QString("[오류] Sn3DNewProject 실패: %1").arg(ret));
        setStatus("error");
        setStatusMessage("프로젝트 생성 실패");
        emit errorOccurred(QString("프로젝트 생성 실패: %1").arg(ret));
        return;
    }

    appendLog("[프로젝트] Sn3DNewProject 호출 완료 — 완료 콜백 대기 중...");
}

void FreeScanController::pauseScan()
{
    if (!m_isScanning) return;
    if (m_app) m_app->Sn3DPauseScan();
    m_elapsedTimer->stop();
    setStatus("paused");
    setStatusMessage("일시정지");
    appendLog("[제어] 스캔 일시정지");
}

void FreeScanController::resumeScan()
{
    if (!m_isScanning) return;
    if (m_app) m_app->Sn3DContinueScan();
    m_elapsedTimer->start();
    setStatus("scanning");
    setStatusMessage("스캔 재개 중...");
    appendLog("[제어] 스캔 재개");
}

void FreeScanController::stopScan()
{
    m_waitingForScan = false;
    m_elapsedTimer->stop();
    if (m_app && m_isScanning) {
        m_app->Sn3DCancelScan();
    }
    setIsScanning(false);
    setProgress(0);
    setCurrentStep("");
    setStatus("idle");
    setStatusMessage("스캔 취소됨");
    m_startTime = "";
    emit startTimeChanged();
    m_elapsedSeconds = 0;
    emit elapsedSecondsChanged();
    appendLog("[제어] 스캔 중지/취소");
}

QString FreeScanController::formatElapsedTime() const
{
    int h = m_elapsedSeconds / 3600;
    int m = (m_elapsedSeconds % 3600) / 60;
    int s = m_elapsedSeconds % 60;
    if (h > 0) return QString("%1시간 %2분 %3초").arg(h).arg(m).arg(s);
    if (m > 0) return QString("%1분 %2초").arg(m).arg(s);
    return QString("%1초").arg(s);
}

void FreeScanController::clearLog()
{
    m_logMessages.clear();
    emit logMessagesChanged();
}

// ── SDK 신호 핸들러 ──────────────────────────────────────────────
void FreeScanController::onNewProjFinished(QStringList projNameList)
{
    appendLog(QString("[콜백] newProjFinished: %1").arg(projNameList.join(",")));
    setProgress(30);
    setCurrentStep("스캔 페이지 진입 중...");
    setStatusMessage("스캔 페이지 열기...");
    enterScanPage();
}

void FreeScanController::onSnMessageReceived(QVariantMap mapParam)
{
    if (!mapParam.contains(CMD_KEY)) return;
    QString cmd = mapParam[CMD_KEY].toString();

    // ZMQ 메시지 로그
    appendLog(QString("[ZMQ] %1").arg(cmd));

    if (cmd == "ScanModeChange" && mapParam.contains(SCANMODE_KEY)) {
        int scanState = mapParam[SCANMODE_KEY].toInt();
        m_sdkScanState = scanState;
        appendLog(QString("[스캔상태] %1").arg(scanState));

        switch (scanState) {
        case SNSDK_PRE_SCAN:
        case SNSDK_PRE_SCANNING:
            setCurrentStep("프리스캔 중...");
            setStatusMessage("프리스캔 중...");
            setProgress(32);
            break;

        case SNSDK_SCAN:
            // 스캐너에서 스캔 시작 신호 수신
            if (m_waitingForScan) {
                m_waitingForScan = false;
                setIsScanning(true);
                setStatus("scanning");
                setStatusMessage("스캔 진행 중...");
                setCurrentStep("3D 스캔 진행 중...");
                setProgress(35);
                m_scanStartTime = QDateTime::currentDateTime();
                m_startTime = m_scanStartTime.toString("hh:mm:ss");
                emit startTimeChanged();
                m_elapsedSeconds = 0;
                m_elapsedTimer->start();
                appendLog("[스캔] 스캔 시작 확인");
            }
            break;

        case SNSDK_PAUSED:
            setStatus("paused");
            setCurrentStep("일시정지");
            break;

        case SNSDK_SCAN_STOPED:
            // 스캐너에서 스캔 종료 신호 수신
            appendLog("[스캔] 스캔 종료 신호 수신 → EndScan 호출");
            m_elapsedTimer->stop();
            setStatus("meshing");
            setStatusMessage("포인트 클라우드 생성 중...");
            setCurrentStep("포인트 클라우드 생성 중...");
            setProgress(60);
            if (m_app) {
                m_app->Sn3DEndScan();
                // EndScan 후 generatePointCloudResult 콜백 오면
                // meshDataReturn 신호가 발생함
                // 여기서는 바로 doAutoMesh() 호출
                QTimer::singleShot(500, this, [this]() { doAutoMesh(); });
            } else {
                // 시뮬이라면 직접 진행
                QTimer::singleShot(1000, this, [this]() { doAutoMesh(); });
            }
            break;

        default:
            break;
        }
    }
    else if (cmd == "cancelScanFinish") {
        appendLog("[스캔] 취소 완료");
    }
    else if (cmd == "sdkScanPageSwitched") {
        appendLog("[스캔] 스캔 페이지 전환 완료 — 스캔 대기 중");
        m_waitingForScan = true;
        setCurrentStep("스캐너에서 스캔을 시작하세요");
        setStatusMessage("스캔 신호 대기 중...");
        setProgress(32);
    }
}

void FreeScanController::onMeshDataReturn(bool ok)
{
    if (!ok) {
        appendLog("[오류] 메시 생성 실패");
        setStatus("error");
        setStatusMessage("메시 생성 실패");
        setIsScanning(false);
        emit errorOccurred("메시 생성 실패");
        return;
    }
    appendLog("[메시] 메시 생성 완료 → 저장 중...");
    setProgress(90);
    setCurrentStep("파일 저장 중...");
    setStatus("saving");
    setStatusMessage("파일 저장 중...");
    doAutoSave();
}

void FreeScanController::onSaveMeshDataReturn(bool ok)
{
    if (!ok) {
        appendLog("[오류] 저장 실패");
        setStatus("error");
        setStatusMessage("저장 실패");
        setIsScanning(false);
        emit errorOccurred("메시 저장 실패");
        return;
    }
    appendLog("[저장] 저장 완료!");
    setProgress(100);
    setCurrentStep("스캔 완료");
    setStatus("completed");
    setStatusMessage("스캔 완료!");
    setIsScanning(false);
    emit scanCompleted();
}

// ── 내부 메서드 ───────────────────────────────────────────────────
void FreeScanController::doConnectWiFi(const QString& ip, int port)
{
    // 이 함수는 백그라운드 스레드에서 실행됨
    QMetaObject::invokeMethod(this, [this]() {
        setStatusMessage("SDK 초기화 중...");
        appendLog("[WiFi] Sn3DInitializeSocket 호출");
    }, Qt::QueuedConnection);

    int ret = m_app->Sn3DInitializeSocket(SNSDK_DEVTYPE_UEP2);

    QMetaObject::invokeMethod(this, [this, ret]() {
        if (ret != 0) {
            appendLog(QString("[오류] InitializeSocket 실패: %1").arg(ret));
            setStatus("error");
            setStatusMessage("SDK 초기화 실패");
            emit connectionFailed("SDK 초기화 실패");
            return;
        }
        appendLog(QString("[WiFi] ConnectSocket 호출: %1:%2").arg(m_wifiIp).arg(m_wifiPort));
        setStatusMessage("WiFi 연결 중...");
    }, Qt::QueuedConnection);

    if (ret != 0) return;

    ret = m_app->Sn3DConnectSocket(ip, port);

    QMetaObject::invokeMethod(this, [this, ret]() {
        if (ret != 0) {
            appendLog(QString("[오류] ConnectSocket 실패: %1").arg(ret));
            setStatus("error");
            setStatusMessage(QString("WiFi 연결 실패 (오류: %1)").arg(ret));
            emit connectionFailed(QString("WiFi 연결 실패: %1").arg(ret));
        } else {
            appendLog("[WiFi] 연결 성공!");
            m_deviceType  = SNSDK_DEVTYPE_UEP2;
            m_deviceModel = "Freescan UE Pro2";
            m_connType    = "WiFi";
            emit deviceModelChanged();
            emit connTypeChanged();
            setIsConnected(true);
            setStatus("success");
            setStatusMessage("스캐너 연결됨");
            emit connectionSuccess();
        }
    }, Qt::QueuedConnection);
}

void FreeScanController::enterScanPage()
{
    if (!m_app) return;
    appendLog("[스캔] Sn3DEnterScanPage 호출");
    // UEP2는 Sn3DEnterScanPage() 사용
    m_app->Sn3DEnterScanPage();
    // sdkScanPageSwitched 메시지 수신 대기
    // (onSnMessageReceived에서 처리)
    setCurrentStep("스캔 페이지 열기...");
    setProgress(30);
}

void FreeScanController::doAutoMesh()
{
    if (!m_app) {
        // 시뮬
        QTimer::singleShot(2000, this, [this]() {
            onMeshDataReturn(true);
        });
        return;
    }

    appendLog("[메시] Sn3DMesh 호출");
    setProgress(75);
    setCurrentStep("메시 변환 중...");
    setStatus("meshing");
    setStatusMessage("메시 변환 중...");

    FreeScanMeshPar meshPar;
    memset(&meshPar, 0, sizeof(FreeScanMeshPar));

    // 실제 SDK 필드명 (sn3dsdkcorepub.h 기준)
    // filterLevel/smoothLevel은 QML에서 문자열("none","1","2","3")로 옴
    auto filterStr = m_activePreset.value("filterLevel", "2").toString();
    auto smoothStr = m_activePreset.value("smoothLevel", "1").toString();
    meshPar.watertightType           = 0; // 0=open, 1=watertight
    meshPar.fileter_level            = filterStr == "none" ? 0 : filterStr.toInt();
    meshPar.smoothValue              = smoothStr == "none" ? 0 : smoothStr.toInt();
    meshPar.fill_small_hole_ischeck  = m_activePreset.value("fillSmallHoles", false).toBool() ? 1 : 0;
    meshPar.fill_small_hole_value    = m_activePreset.value("fillSmallHoleValue", 3).toInt();
    meshPar.remove_small_floating_parts = m_activePreset.value("removeAnomalies", true).toBool() ? 1 : 0;
    meshPar.remove_spike_ischeck     = m_activePreset.value("removeSpikes", false).toBool();
    meshPar.marker_hole_filling_ischeck = m_activePreset.value("fillMarkerHoles", true).toBool();

    m_app->Sn3DMesh(&meshPar);
    // 완료는 meshDataReturn 신호로 수신
}

void FreeScanController::doAutoSave()
{
    if (!m_app) {
        QTimer::singleShot(1500, this, [this]() {
            onSaveMeshDataReturn(true);
        });
        return;
    }

    appendLog("[저장] Sn3DSaveMesh 호출");

    // folderPath / fileName / format 은 const char* 포인터
    QString meshDir = m_activePreset.value("saveDirectory", "").toString();
    if (meshDir.isEmpty()) meshDir = m_projectPath;
    QByteArray meshDirBytes = meshDir.toUtf8();

    QString fmt = m_activePreset.value("fileFormat", "stl").toString();
    QByteArray fmtBytes  = fmt.toUtf8();
    QByteArray nameBytes = QByteArray("scan_result");
    QByteArray methodBytes = QByteArray("binary");

    FreeSaveMeshPar savePar;
    memset(&savePar, 0, sizeof(FreeSaveMeshPar));
    savePar.folderPath = meshDirBytes.constData();
    savePar.fileName   = nameBytes.constData();
    savePar.format     = fmtBytes.constData();
    savePar.method     = methodBytes.constData();

    // 형식에 따라 저장 플래그 설정
    savePar.saveStlFile = (fmt == "stl") ? 1 : 0;
    savePar.saveObjFile = (fmt == "obj") ? 1 : 0;
    savePar.savePlyFile = (fmt == "ply") ? 1 : 0;

    m_app->Sn3DSaveMesh(&savePar);
    // 완료는 saveMeshDataReturn 신호로 수신
}

// ── 경과 시간 ────────────────────────────────────────────────────
void FreeScanController::updateElapsedTime()
{
    ++m_elapsedSeconds;
    emit elapsedSecondsChanged();
    // 스캔 중 진행률 최솟값 유지 (35~60%)
    if (m_status == "scanning" && m_progress < 60) {
        int p = qMin(58, 35 + m_elapsedSeconds);
        setProgress(p);
    }
}

// ── 내부 setter ───────────────────────────────────────────────────
void FreeScanController::setStatus(const QString& s)
{
    if (m_status != s) { m_status = s; emit statusChanged(); }
}

void FreeScanController::setStatusMessage(const QString& m)
{
    if (m_statusMessage != m) { m_statusMessage = m; emit statusMessageChanged(); }
}

void FreeScanController::setIsConnected(bool c)
{
    if (m_isConnected != c) { m_isConnected = c; emit isConnectedChanged(); }
}

void FreeScanController::setIsScanning(bool s)
{
    if (m_isScanning != s) { m_isScanning = s; emit isScanningChanged(); }
}

void FreeScanController::setProgress(int p)
{
    if (m_progress != p) { m_progress = p; emit progressChanged(); }
}

void FreeScanController::setCurrentStep(const QString& s)
{
    if (m_currentStep != s) { m_currentStep = s; emit currentStepChanged(); }
}

void FreeScanController::appendLog(const QString& msg)
{
    QString ts  = QDateTime::currentDateTime().toString("HH:mm:ss");
    QString line = QString("[%1] %2").arg(ts, msg);
    m_logMessages.append(line);
    // 최대 500줄 유지
    while (m_logMessages.size() > 500)
        m_logMessages.removeFirst();
    emit logMessagesChanged();
    qDebug() << line;
}
