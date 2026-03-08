#pragma once
// ─────────────────────────────────────────────────────────────────
// FreeScanController.h
//   FreeScan SDK → QML 인터페이스 컨트롤러
//   - WiFi (UEP2): Sn3DInitializeSocket + Sn3DConnectSocket
//   - USB (COMBO): FreeScan.exe 실행 + Sn3DInitialize
//   - 스캔 흐름: NewProject → EnterScanPage → ScanModeChange 대기
//               → SNSDK_SCAN_STOPED → EndScan → Mesh → SaveMesh
// ─────────────────────────────────────────────────────────────────
#include <QObject>
#include <QString>
#include <QTimer>
#include <QDateTime>
#include <QStringList>
#include <QVariantMap>

class Sn3DSDKDemoApp;

class FreeScanController : public QObject
{
    Q_OBJECT

    // ── QML 바인딩 프로퍼티 ────────────────────────────────────
    Q_PROPERTY(QString  status          READ status          NOTIFY statusChanged)
    Q_PROPERTY(QString  statusMessage   READ statusMessage   NOTIFY statusMessageChanged)
    Q_PROPERTY(bool     isConnected     READ isConnected     NOTIFY isConnectedChanged)
    Q_PROPERTY(bool     isScanning      READ isScanning      NOTIFY isScanningChanged)
    Q_PROPERTY(int      progress        READ progress        NOTIFY progressChanged)
    Q_PROPERTY(QString  currentStep     READ currentStep     NOTIFY currentStepChanged)
    Q_PROPERTY(QString  startTime       READ startTime       NOTIFY startTimeChanged)
    Q_PROPERTY(int      elapsedSeconds  READ elapsedSeconds  NOTIFY elapsedSecondsChanged)
    Q_PROPERTY(QString  deviceModel     READ deviceModel     NOTIFY deviceModelChanged)
    Q_PROPERTY(QString  connType        READ connType        NOTIFY connTypeChanged)
    Q_PROPERTY(QString  wifiIp          READ wifiIp          WRITE setWifiIp  NOTIFY wifiIpChanged)
    Q_PROPERTY(int      wifiPort        READ wifiPort        WRITE setWifiPort NOTIFY wifiPortChanged)
    Q_PROPERTY(QStringList logMessages  READ logMessages     NOTIFY logMessagesChanged)

public:
    explicit FreeScanController(Sn3DSDKDemoApp* app, QObject* parent = nullptr);
    ~FreeScanController() override = default;

    // ── Getters ────────────────────────────────────────────────
    QString     status()         const { return m_status; }
    QString     statusMessage()  const { return m_statusMessage; }
    bool        isConnected()    const { return m_isConnected; }
    bool        isScanning()     const { return m_isScanning; }
    int         progress()       const { return m_progress; }
    QString     currentStep()    const { return m_currentStep; }
    QString     startTime()      const { return m_startTime; }
    int         elapsedSeconds() const { return m_elapsedSeconds; }
    QString     deviceModel()    const { return m_deviceModel; }
    QString     connType()       const { return m_connType; }
    QString     wifiIp()         const { return m_wifiIp; }
    int         wifiPort()       const { return m_wifiPort; }
    QStringList logMessages()    const { return m_logMessages; }

    // ── Setters (QML에서도 쓸 수 있게) ────────────────────────
    void setWifiIp(const QString& ip);
    void setWifiPort(int port);

    // ── QML 호출 가능 함수 ────────────────────────────────────
    Q_INVOKABLE void connectScanner();       // WiFi(UEP2) 또는 USB 자동 판별
    Q_INVOKABLE void disconnectScanner();
    Q_INVOKABLE void startScan(const QVariantMap& preset);  // 프로젝트 생성 → 스캔 대기
    Q_INVOKABLE void pauseScan();
    Q_INVOKABLE void resumeScan();
    Q_INVOKABLE void stopScan();
    Q_INVOKABLE QString formatElapsedTime() const;
    Q_INVOKABLE void clearLog();

signals:
    void statusChanged();
    void statusMessageChanged();
    void isConnectedChanged();
    void isScanningChanged();
    void progressChanged();
    void currentStepChanged();
    void startTimeChanged();
    void elapsedSecondsChanged();
    void deviceModelChanged();
    void connTypeChanged();
    void wifiIpChanged();
    void wifiPortChanged();
    void logMessagesChanged();

    void scanCompleted();
    void errorOccurred(const QString& error);
    void connectionSuccess();
    void connectionFailed(const QString& error);

private slots:
    void updateElapsedTime();
    // SDK 신호 핸들러
    void onSnMessageReceived(QVariantMap mapParam);
    void onNewProjFinished(QStringList projNameList);
    void onMeshDataReturn(bool ok);
    void onSaveMeshDataReturn(bool ok);

private:
    void setStatus(const QString& s);
    void setStatusMessage(const QString& m);
    void setIsConnected(bool c);
    void setIsScanning(bool s);
    void setProgress(int p);
    void setCurrentStep(const QString& s);
    void appendLog(const QString& msg);

    // WiFi 연결 (백그라운드 스레드)
    void doConnectWiFi(const QString& ip, int port);
    // USB 연결 (FreeScan.exe 실행 후 Sn3DInitialize)
    void doConnectUsb();
    // 스캔 진입
    void enterScanPage();
    // 메시 처리
    void doAutoMesh();
    // 메시 저장
    void doAutoSave();

    Sn3DSDKDemoApp* m_app = nullptr;

    // 상태
    QString  m_status        = "idle";
    QString  m_statusMessage = "대기중";
    bool     m_isConnected   = false;
    bool     m_isScanning    = false;
    int      m_progress      = 0;
    QString  m_currentStep;
    QString  m_startTime;
    int      m_elapsedSeconds = 0;
    QDateTime m_scanStartTime;
    QString  m_deviceModel   = "Freescan UE Pro2";
    QString  m_connType      = "WiFi";

    // WiFi 설정
    QString  m_wifiIp   = "192.168.1.10";
    int      m_wifiPort = 11999;

    // 내부 상태
    int     m_deviceType     = 0;   // SNSDK_DEVTYPE_UEP2 등
    int     m_sdkScanState   = -1;  // 마지막 ScanModeChange 상태
    bool    m_waitingForScan = false; // EnterScanPage 후 스캔 신호 대기 중
    QString m_projectPath;
    QVariantMap m_activePreset;

    QTimer*     m_elapsedTimer = nullptr;
    QStringList m_logMessages;
};
