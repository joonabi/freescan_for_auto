#include "OperatorMainView.h"
#include "PresetEditDialog.h"
#include "sn3dsdkdemoapp.h"
#include "sn3dsdkcorepub.h"   // ScanType, SNSDK_PRE_SCAN, ...
#include "sn3dsdkpluginpub.h" // SNSDK_DEVTYPE_*
#include "SDKCWrapper.h"      // NewProject struct

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSettings>
#include <QSizePolicy>
#include <QFont>
#include <QImage>
#include <QPixmap>
#include <QTimer>

// ─────────────────────────────────────────────────────────────────
// 전역 C-Style 콜백 (SDK가 C 함수 포인터만 허용하기 때문에 필요)
// ─────────────────────────────────────────────────────────────────
static void CALLBACK OperatorVideoCallback(
    int camId, const unsigned char* data,
    int width, int height, int channel, void* pUserData)
{
    auto* v = reinterpret_cast<OperatorMainView*>(pUserData);
    if (v) v->videoCallBack(camId, data, width, height, channel);
}

// ═════════════════════════════════════════════════════════════════
// 생성자
// ═════════════════════════════════════════════════════════════════
OperatorMainView::OperatorMainView(QWidget* parent)
    : Sn3DSDKMainViewBase(parent)
    , m_demoApp(static_cast<Sn3DSDKDemoApp*>(qApp))
{
    setWindowTitle(QString::fromUtf8("SNK 3D 스캐너 운영 시스템"));
    resize(1200, 800);

    m_settings = new QSettings("./scansoftwaresdk.ini", QSettings::IniFormat);

    // ── 스택 위젯 생성 ─────────────────────────────────────────
    m_stack = new QStackedWidget(this);
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(m_stack);

    buildStartPage();
    buildScanPage();

    m_stack->addWidget(m_startPage);
    m_stack->addWidget(m_scanPage);
    m_stack->setCurrentIndex(0);

    applyGlobalStyle();

    // ── Sn3DSDKDemoApp 시그널 연결 ─────────────────────────────
    connect(m_demoApp, &Sn3DSDKDemoApp::SnMessageReceived,
            this, &OperatorMainView::onSnMessageReceived, Qt::QueuedConnection);

    connect(m_demoApp, &Sn3DSDKDemoApp::newProjFinished,
            this, &OperatorMainView::onNewProjFinished, Qt::QueuedConnection);

    connect(m_demoApp, &Sn3DSDKDemoApp::showProgress,
            this, &OperatorMainView::onShowProgress, Qt::QueuedConnection);

    connect(m_demoApp, &Sn3DSDKDemoApp::pointCountChanged,
            this, &OperatorMainView::onPointCountChanged, Qt::QueuedConnection);

    connect(m_demoApp, &Sn3DSDKDemoApp::frameRateChanged,
            this, &OperatorMainView::onFrameRateChanged, Qt::QueuedConnection);

    connect(m_demoApp, &Sn3DSDKDemoApp::trackLostStatusChanged,
            this, &OperatorMainView::onTrackLostStatusChanged, Qt::QueuedConnection);

    connect(m_demoApp, &Sn3DSDKDemoApp::scanDistChanged,
            this, &OperatorMainView::onScanDistChanged, Qt::QueuedConnection);

    // ── 비디오 프레임 시그널 (스레드 경계) ─────────────────────
    connect(this, &OperatorMainView::sigGetVideoData,
            this, [this](int camId, const unsigned char* data,
                         int width, int height, int /*channel*/)
    {
        QImage img(data, width, height, QImage::Format_BGR888);
        QPixmap px = QPixmap::fromImage(img);
        if (camId == 0 && m_leftCamLbl)
            m_leftCamLbl->setPixmap(
                px.scaled(m_leftCamLbl->size(), Qt::KeepAspectRatio));
        else if (camId == 1 && m_rightCamLbl)
            m_rightCamLbl->setPixmap(
                px.scaled(m_rightCamLbl->size(), Qt::KeepAspectRatio));
    }, Qt::AutoConnection);
}

OperatorMainView::~OperatorMainView()
{
    releaseSDK();
}

// ═════════════════════════════════════════════════════════════════
// 창 닫기 이벤트
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::closeEvent(QCloseEvent* event)
{
    releaseSDK();
    event->accept();
}

// ─────────────────────────────────────────────────────────────────
// SDK 전역 콜백 → Qt 시그널 전달
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::videoCallBack(int camId, const unsigned char* data,
                                     int width, int height, int channel)
{
    emit sigGetVideoData(camId, data, width, height, channel);
}

// ═════════════════════════════════════════════════════════════════
// UI 초기화 – Page 0 (시작 화면)
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::buildStartPage()
{
    m_startPage = new QWidget();
    auto* vlay  = new QVBoxLayout(m_startPage);
    vlay->setContentsMargins(60, 40, 60, 40);
    vlay->setSpacing(20);

    // ── 상단 로고 / 회사명 영역 ────────────────────────────────
    m_logoLabel = new QLabel();
    m_logoLabel->setAlignment(Qt::AlignCenter);
    m_logoLabel->setMinimumHeight(140);
    // 기본: 텍스트로 표시 (실제 운영 시 이미지 파일 교체 가능)
    QPixmap logoImg(":/images/company_logo.png");
    if (!logoImg.isNull()) {
        m_logoLabel->setPixmap(
            logoImg.scaled(420, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_logoLabel->setText(
            QString::fromUtf8("◈  SNK 3D 스캐너 운영 시스템  ◈"));
        m_logoLabel->setStyleSheet(
            "color:#FFFFFF; font-size:32px; font-weight:bold;"
            "background:#1F497D; border-radius:10px; padding:20px;");
    }
    vlay->addWidget(m_logoLabel);
    vlay->addSpacing(30);

    // ── 버튼 3개 ───────────────────────────────────────────────
    auto makeMainBtn = [](const QString& text, const QString& color) {
        auto* btn = new QPushButton(text);
        btn->setMinimumHeight(80);
        btn->setStyleSheet(
            QString("QPushButton {"
                    "  background:%1; color:white; border-radius:12px;"
                    "  font-size:22px; font-weight:bold; }"
                    "QPushButton:hover { background:%2; }"
                    "QPushButton:pressed { background:#333; }")
            .arg(color)
            .arg(color == "#2E74B5" ? "#1F497D" : color == "#217346" ? "#185030" : "#5a0000"));
        return btn;
    };

    m_btnNewProj  = makeMainBtn(QString::fromUtf8("▶  프로젝트 생성"), "#2E74B5");
    m_btnOpenProj = makeMainBtn(QString::fromUtf8("▶  프로젝트 열기"), "#217346");
    m_btnPreset   = new QPushButton(QString::fromUtf8("⚙  프리셋 설정"));
    m_btnPreset->setMinimumHeight(50);
    m_btnPreset->setStyleSheet(
        "QPushButton { background:#555; color:white; border-radius:8px;"
        "  font-size:16px; }"
        "QPushButton:hover { background:#777; }");

    vlay->addWidget(m_btnNewProj);
    vlay->addWidget(m_btnOpenProj);
    vlay->addStretch();
    vlay->addWidget(m_btnPreset, 0, Qt::AlignRight);

    // ── 하단 상태 표시 ─────────────────────────────────────────
    m_startStatus = new QLabel(QString::fromUtf8("준비"));
    m_startStatus->setAlignment(Qt::AlignCenter);
    m_startStatus->setStyleSheet("color:#AAAAAA; font-size:13px;");
    vlay->addWidget(m_startStatus);

    // ── 시그널 연결 ────────────────────────────────────────────
    connect(m_btnNewProj,  &QPushButton::clicked,
            this, &OperatorMainView::onBtnNewProjectClicked);
    connect(m_btnOpenProj, &QPushButton::clicked,
            this, &OperatorMainView::onBtnOpenProjectClicked);
    connect(m_btnPreset,   &QPushButton::clicked,
            this, &OperatorMainView::onBtnPresetClicked);
}

// ═════════════════════════════════════════════════════════════════
// UI 초기화 – Page 1 (스캔 화면)
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::buildScanPage()
{
    m_scanPage = new QWidget();
    auto* rootV = new QVBoxLayout(m_scanPage);
    rootV->setContentsMargins(12, 8, 12, 8);
    rootV->setSpacing(8);

    // ── 상단 제목 바 ───────────────────────────────────────────
    auto* topBar = new QLabel(QString::fromUtf8("SNK 3D 스캐너  |  스캔 진행"));
    topBar->setAlignment(Qt::AlignCenter);
    topBar->setFixedHeight(36);
    topBar->setStyleSheet(
        "background:#1F497D; color:white; font-size:16px; font-weight:bold;"
        "border-radius:6px;");
    rootV->addWidget(topBar);

    // ── 중단: 카메라 + 상태 패널 ──────────────────────────────
    auto* midH = new QHBoxLayout();
    midH->setSpacing(12);

    // 카메라 영역 (왼쪽 / 오른쪽)
    auto makeCamLbl = [](const QString& title) {
        auto* lbl = new QLabel();
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setText(title);
        lbl->setMinimumSize(440, 330);
        lbl->setStyleSheet(
            "background:#111; color:#555; font-size:13px;"
            "border:1px solid #333; border-radius:4px;");
        return lbl;
    };
    m_leftCamLbl  = makeCamLbl(QString::fromUtf8("왼쪽 카메라"));
    m_rightCamLbl = makeCamLbl(QString::fromUtf8("오른쪽 카메라"));

    auto* camV = new QVBoxLayout();
    auto* camH = new QHBoxLayout();
    camH->addWidget(m_leftCamLbl);
    camH->addWidget(m_rightCamLbl);
    camV->addLayout(camH);
    midH->addLayout(camV, 3);

    // 상태 패널 (오른쪽)
    auto* statusPanel = new QWidget();
    statusPanel->setMinimumWidth(220);
    statusPanel->setStyleSheet("background:#1a1a2e; border-radius:8px;");
    auto* sV = new QVBoxLayout(statusPanel);
    sV->setContentsMargins(14, 14, 14, 14);
    sV->setSpacing(12);

    auto makeStatLbl = [](const QString& init) {
        auto* lbl = new QLabel(init);
        lbl->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        lbl->setStyleSheet(
            "color:#CCCCCC; font-size:15px;"
            "background:transparent; padding:4px;");
        lbl->setWordWrap(true);
        return lbl;
    };

    m_stateLbl = makeStatLbl(QString::fromUtf8("상태: 대기 중"));
    m_trackLbl = makeStatLbl(QString::fromUtf8("추적: ●"));
    m_pointLbl = makeStatLbl(QString::fromUtf8("포인트: 0"));
    m_fpsLbl   = makeStatLbl(QString::fromUtf8("FPS: --"));
    m_distLbl  = makeStatLbl(QString::fromUtf8("거리: --"));

    sV->addWidget(m_stateLbl);
    sV->addWidget(m_trackLbl);
    sV->addWidget(m_pointLbl);
    sV->addWidget(m_fpsLbl);
    sV->addWidget(m_distLbl);
    sV->addStretch();

    midH->addWidget(statusPanel, 1);
    rootV->addLayout(midH, 1);

    // ── 진행률 ─────────────────────────────────────────────────
    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    m_progressBar->setFixedHeight(18);
    m_progressBar->setStyleSheet(
        "QProgressBar { background:#222; border-radius:4px; }"
        "QProgressBar::chunk { background:#2E74B5; border-radius:4px; }");
    rootV->addWidget(m_progressBar);

    // ── 하단 제어 버튼 ─────────────────────────────────────────
    auto* btnH = new QHBoxLayout();
    btnH->setSpacing(14);

    m_btnScanCtrl = new QPushButton(QString::fromUtf8("▶  프리스캔"));
    m_btnScanCtrl->setMinimumHeight(60);
    m_btnScanCtrl->setStyleSheet(
        "QPushButton { background:#2E74B5; color:white; border-radius:10px;"
        "  font-size:18px; font-weight:bold; }"
        "QPushButton:hover { background:#1F497D; }");

    m_btnEndScan = new QPushButton(QString::fromUtf8("■  스캔 완료"));
    m_btnEndScan->setMinimumHeight(60);
    m_btnEndScan->setStyleSheet(
        "QPushButton { background:#217346; color:white; border-radius:10px;"
        "  font-size:18px; font-weight:bold; }"
        "QPushButton:hover { background:#185030; }");

    m_btnBack = new QPushButton(QString::fromUtf8("← 처음으로"));
    m_btnBack->setMinimumHeight(60);
    m_btnBack->setFixedWidth(140);
    m_btnBack->setStyleSheet(
        "QPushButton { background:#555; color:white; border-radius:10px;"
        "  font-size:15px; }"
        "QPushButton:hover { background:#777; }");

    btnH->addWidget(m_btnBack);
    btnH->addWidget(m_btnScanCtrl, 2);
    btnH->addWidget(m_btnEndScan,  1);
    rootV->addLayout(btnH);

    // ── 시그널 연결 ────────────────────────────────────────────
    connect(m_btnScanCtrl, &QPushButton::clicked,
            this, &OperatorMainView::onBtnScanCtrlClicked);
    connect(m_btnEndScan,  &QPushButton::clicked,
            this, &OperatorMainView::onBtnEndScanClicked);
    connect(m_btnBack,     &QPushButton::clicked,
            this, &OperatorMainView::onBtnBackClicked);
}

// ─────────────────────────────────────────────────────────────────
// 전체 스타일시트 (다크 테마)
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::applyGlobalStyle()
{
    setStyleSheet(
        "QWidget { background:#0D1117; color:#E0E0E0; }"
        "QLabel  { color:#E0E0E0; }");
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – 프로젝트 생성
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onBtnNewProjectClicked()
{
    m_activePreset = PresetManager::instance().currentPreset();

    // SDK 초기화 (이미 초기화 됐으면 건너뜀)
    if (!m_sdkInitialized) {
        updateStatusArea(QString::fromUtf8("SDK 초기화 중..."), "#FFCC00");
        if (!initSDK(m_activePreset)) {
            updateStatusArea(QString::fromUtf8("SDK 초기화 실패"), "#FF4444");
            return;
        }
    }

    // 스캔 모드 설정 (레이저 / 적외선)
    m_scanStep.setDeviceType(m_activePreset.deviceType);
    m_scanStep.setScanMode(m_activePreset.scanMode);
    m_demoApp->Sn3DSetScanMode(m_activePreset.scanMode);

    // 자동 프로젝트 경로 생성: saveDirectory/YYYYMMDD_HHMMSS
    QString projPath = generateProjectPath(m_activePreset);
    QDir().mkpath(projPath);

    updateStatusArea(
        QString::fromUtf8("프로젝트 생성 중: ") + projPath, "#FFCC00");

    // Sn3DNewProject 호출
    NewProject struNew;
    memset(&struNew, 0, sizeof(NewProject));
    auto projPathStd = projPath.toStdString();
    struNew.szSlnDirPath = projPathStd.c_str();
    struNew.iScanMode    = m_activePreset.scanMode;

    auto err = m_demoApp->Sn3DNewProject(&struNew);
    if (err != 0) {
        updateStatusArea(
            QString::fromUtf8("프로젝트 생성 오류: %1").arg(err), "#FF4444");
    }
    // → 완료는 onNewProjFinished() 에서 처리
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – 프로젝트 열기
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onBtnOpenProjectClicked()
{
    m_activePreset = PresetManager::instance().currentPreset();

    if (!m_sdkInitialized) {
        if (!initSDK(m_activePreset)) {
            QMessageBox::warning(this,
                QString::fromUtf8("오류"),
                QString::fromUtf8("SDK 초기화에 실패했습니다."));
            return;
        }
    }

    // 스캔 모드에 따라 파일 필터 설정
    int  scanMode = m_activePreset.scanMode;
    bool isLaser  = (scanMode == SNSDK_SCANMODE_LASER);
    QString modeStr   = isLaser ? "laser" : "ir";
    QString deviceStr;
    switch (m_activePreset.deviceType) {
    case SNSDK_DEVTYPE_UEPRO:    deviceStr = "uep";       break;
    case SNSDK_DEVTYPE_COMBO:    deviceStr = "combo";     break;
    case SNSDK_DEVTYPE_TRIO:     deviceStr = "trio";      break;
    case SNSDK_DEVTYPE_UEP2:     deviceStr = "uep2";      break;
    case SNSDK_DEVTYPE_COMBOPLUS:deviceStr = "comboplus";  break;
    default:                     deviceStr = "*";         break;
    }
    QString filter = QString("*.sln_%1_%2").arg(modeStr, deviceStr);

    QFileDialog dlg;
    QString slnPath = dlg.getOpenFileName(
        this,
        QString::fromUtf8("프로젝트 파일 선택"),
        m_activePreset.saveDirectory,
        filter);

    if (slnPath.isEmpty()) return;

    updateStatusArea(QString::fromUtf8("프로젝트 로딩 중..."), "#FFCC00");

    ScanType scanType = isLaser ? ST_FreeScan_LASER : ST_FreeScan_Infrared;
    m_scanStep.setScanMode(scanMode);
    m_demoApp->Sn3DSetScanMode(scanMode);
    m_demoApp->openProject(slnPath.toStdString().c_str(), scanType);

    // 로딩 완료는 onSnMessageReceived의 'createOrLoadCurProjectFinished' 에서 처리
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – 프리셋 설정
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onBtnPresetClicked()
{
    PresetEditDialog dlg(this);
    dlg.exec();
    // 다이얼로그 내에서 PresetManager가 갱신됨
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – 스캔 제어 버튼 (상태에 따라 동작이 달라짐)
//  SNSDK_PRE_SCAN      → Sn3DStartScan() : 프리스캔 시작
//  SNSDK_PRE_SCANNING  → Sn3DStartScan() : 실제 스캔 시작
//  SNSDK_SCAN          → Sn3DPauseScan() : 일시정지
//  SNSDK_PAUSED        → Sn3DStartScan() : 재개
//  SNSDK_SCAN_STOPED   → Sn3DStartScan() : 재시작
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onBtnScanCtrlClicked()
{
    m_demoApp->videoProcessControl(true);

    int state = m_scanStep.scanState();
    if (state == SNSDK_SCAN) {
        m_demoApp->Sn3DPauseScan();
    } else {
        m_demoApp->Sn3DStartScan();
    }
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – 스캔 완료
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onBtnEndScanClicked()
{
    m_demoApp->Sn3DEndScan();
    m_stateLbl->setText(QString::fromUtf8("상태: 스캔 완료 처리 중"));
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – 처음 화면으로
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onBtnBackClicked()
{
    auto reply = QMessageBox::question(
        this,
        QString::fromUtf8("확인"),
        QString::fromUtf8("스캔을 중단하고 처음 화면으로 돌아가시겠습니까?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_demoApp->Sn3DCancelScan();
        m_demoApp->videoProcessControl(false);
        m_stack->setCurrentIndex(0);
        updateStatusArea(QString::fromUtf8("준비"), "#AAAAAA");
    }
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – SDK 메시지 수신 (onSnMessageReceived)
//  ScanModeChange              → 스캔 상태 업데이트
//  createOrLoadCurProjectFinished → 프로젝트 로드 완료 → 스캔 진입
//  cancelScanFinish            → 취소 완료
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onSnMessageReceived(QVariantMap mapParam)
{
    if (!mapParam.contains("cmd")) return;
    const QString cmd = mapParam["cmd"].toString();

    if (cmd == "ScanModeChange") {
        if (!mapParam.contains("scanMode")) return;
        int state = mapParam["scanMode"].toInt();
        m_scanStep.setScanState(state);
        updateScanCtrlButton();

        // 상태 레이블 텍스트
        QString stateText;
        switch (state) {
        case SNSDK_PRE_SCAN:
            stateText = QString::fromUtf8("상태: 프리스캔 대기");  break;
        case SNSDK_PRE_SCANNING:
            stateText = QString::fromUtf8("상태: 프리스캔 중");    break;
        case SNSDK_SCAN:
            stateText = QString::fromUtf8("상태: 스캔 중");        break;
        case SNSDK_PAUSED:
            stateText = QString::fromUtf8("상태: 일시정지");        break;
        case SNSDK_SCAN_STOPED:
            stateText = QString::fromUtf8("상태: 스캔 정지");       break;
        default:
            stateText = QString::fromUtf8("상태: 알 수 없음");
        }
        m_stateLbl->setText(stateText);

    } else if (cmd == "createOrLoadCurProjectFinished") {
        // 프로젝트 생성/로드 완료 → 스캔 진입
        enterScanPage();

    } else if (cmd == "cancelScanFinish") {
        m_stateLbl->setText(QString::fromUtf8("상태: 취소 완료"));

    } else if (cmd == "returnScanSoftSetting") {
        if (mapParam.contains("enablePreScanFlg")) {
            m_scanStep.setEnablePrescan(mapParam["enablePreScanFlg"].toBool());
            updateScanCtrlButton();
        }

    } else if (cmd == "generatePointCloudResult") {
        int result = mapParam["result"].toInt();
        if (result == 0)
            m_stateLbl->setText(QString::fromUtf8("상태: 점군 생성 완료"));
        else
            m_stateLbl->setText(
                QString::fromUtf8("상태: 점군 생성 실패 (코드:%1)").arg(result));
    }
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – 새 프로젝트 생성 완료
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onNewProjFinished(QStringList projNameList)
{
    updateStatusArea(
        QString::fromUtf8("프로젝트 생성 완료: ") + projNameList.join(", "),
        "#00CC44");
    // 스캔 진입은 enterScanPage()에서 처리
    enterScanPage();
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – 진행률 업데이트
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onShowProgress(QString title, double value, bool visible)
{
    m_progressBar->setVisible(visible);
    m_progressBar->setValue(static_cast<int>(value * 100));
    if (visible && !title.isEmpty())
        m_stateLbl->setText(QString::fromUtf8("상태: ") + title);
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – 포인트 수, FPS, 추적, 거리
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onPointCountChanged(int count)
{
    m_pointLbl->setText(
        QString::fromUtf8("포인트: %L1").arg(count));
}

void OperatorMainView::onFrameRateChanged(int rate)
{
    m_fpsLbl->setText(QString("FPS: %1").arg(rate));
}

void OperatorMainView::onTrackLostStatusChanged(bool lost)
{
    if (lost) {
        m_trackLbl->setText(
            QString::fromUtf8("추적: ● 실패 (위치 이동)"));
        m_trackLbl->setStyleSheet(
            "color:#FF4444; font-size:15px; font-weight:bold;"
            "background:transparent; padding:4px;");
    } else {
        m_trackLbl->setText(QString::fromUtf8("추적: ● 정상"));
        m_trackLbl->setStyleSheet(
            "color:#00FF88; font-size:15px;"
            "background:transparent; padding:4px;");
    }
}

void OperatorMainView::onScanDistChanged(double dist)
{
    // dist > 0: 적정, 0 이하: 너무 가깝거나 멀리
    QString distStr = dist > 0
        ? QString::fromUtf8("거리: ● 적정 (%1mm)").arg(dist, 0, 'f', 0)
        : QString::fromUtf8("거리: ● 범위 벗어남");
    QString color = dist > 0 ? "#00FF88" : "#FFCC00";
    m_distLbl->setText(distStr);
    m_distLbl->setStyleSheet(
        QString("color:%1; font-size:15px; background:transparent; padding:4px;")
        .arg(color));
}

// ═════════════════════════════════════════════════════════════════
// 내부: SDK 초기화 + 콜백 등록 + 프리셋 설정 적용
// ═════════════════════════════════════════════════════════════════
bool OperatorMainView::initSDK(const ScanPreset& preset)
{
    int err = m_demoApp->Sn3DInitialize(preset.deviceType);
    if (err != 0) return false;

    // 비디오 콜백 등록
    m_demoApp->installGetImagasCallBack(OperatorVideoCallback, this);

    // 프리셋 설정 적용 (장치에 전달)
    m_demoApp->Sn3DSetLaserGrade(preset.laserGrade);
    m_demoApp->Sn3DSetIsHighSpeed(preset.highSpeed);
    m_demoApp->Sn3DSetScanObject(preset.objectType);
    m_demoApp->Sn3DChangeBrightStep(preset.brightStep);

    // 강광 모드
    if (preset.strongLight) {
        QString strResult;
        QVariantMap mp = {{"cmd","setIsStrongLight2"},{"strong", true}};
        m_demoApp->SnSendMessage("v2.0","request","other", mp, strResult);
    }

    // 소프트웨어 설정 로드 요청 (prescan 활성화 여부 등)
    QString strResult;
    QVariantMap mp = {{"cmd","loadScanSoftSetting"}};
    m_demoApp->SnSendMessage("v2.0","request","other", mp, strResult);

    m_sdkInitialized = true;
    m_scanStep.setDeviceType(preset.deviceType);
    return true;
}

// ═════════════════════════════════════════════════════════════════
// 내부: 스캔 화면으로 전환 + 스캔 진입
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::enterScanPage()
{
    // 비디오 스트림 시작
    m_demoApp->videoProcessControl(true);

    // 스캔 모드 진입 (Sn3DEnterScan)
    ScanType scanType = (m_activePreset.scanMode == SNSDK_SCANMODE_LASER)
        ? ST_FreeScan_LASER
        : ST_FreeScan_Infrared;
    m_demoApp->Sn3DEnterScan(scanType);

    // UI 초기화
    m_scanStep.setScanState(SNSDK_PRE_SCAN);
    updateScanCtrlButton();
    m_stateLbl->setText(QString::fromUtf8("상태: 프리스캔 대기"));
    m_trackLbl->setText(QString::fromUtf8("추적: ●"));
    m_pointLbl->setText(QString::fromUtf8("포인트: 0"));
    m_fpsLbl->setText(QString("FPS: --"));
    m_distLbl->setText(QString::fromUtf8("거리: --"));
    m_progressBar->setVisible(false);

    // 스캔 페이지로 전환
    m_stack->setCurrentIndex(1);
}

// ═════════════════════════════════════════════════════════════════
// 내부: 스캔 제어 버튼 텍스트/색상 업데이트
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::updateScanCtrlButton()
{
    int state = m_scanStep.scanState();
    QString text;
    QString bg;

    switch (state) {
    case SNSDK_PRE_SCAN:
        text = QString::fromUtf8("▶  프리스캔");
        bg   = "#2E74B5"; break;
    case SNSDK_PRE_SCANNING:
        text = QString::fromUtf8("▶  스캔 시작");
        bg   = "#217346"; break;
    case SNSDK_SCAN:
        text = QString::fromUtf8("⏸  일시정지");
        bg   = "#8B4513"; break;
    case SNSDK_PAUSED:
        text = QString::fromUtf8("▶  계속");
        bg   = "#217346"; break;
    default:
        int enablePre = m_scanStep.enablePrescan();
        text = enablePre
            ? QString::fromUtf8("▶  프리스캔")
            : QString::fromUtf8("▶  스캔 시작");
        bg   = "#2E74B5";
    }

    m_btnScanCtrl->setText(text);
    m_btnScanCtrl->setStyleSheet(
        QString("QPushButton { background:%1; color:white; border-radius:10px;"
                "  font-size:18px; font-weight:bold; }"
                "QPushButton:hover { background:#444; }").arg(bg));
}

// ─────────────────────────────────────────────────────────────────
// 내부: 하단 상태 레이블 업데이트
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::updateStatusArea(const QString& msg, const QString& color)
{
    if (m_startStatus)
        m_startStatus->setText(msg);
    m_startStatus->setStyleSheet(
        QString("color:%1; font-size:13px;").arg(color));
}

// ─────────────────────────────────────────────────────────────────
// 내부: 자동 프로젝트 경로 생성
//   예) C:/ScanResults/20260302_143025
// ─────────────────────────────────────────────────────────────────
QString OperatorMainView::generateProjectPath(const ScanPreset& preset) const
{
    QString stamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    return preset.saveDirectory + "/" + stamp;
}

// ─────────────────────────────────────────────────────────────────
// 내부: SDK 해제
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::releaseSDK()
{
    if (m_sdkInitialized) {
        m_demoApp->videoProcessControl(false);
        m_demoApp->Sn3DRelease();
        m_sdkInitialized = false;
    }
}
