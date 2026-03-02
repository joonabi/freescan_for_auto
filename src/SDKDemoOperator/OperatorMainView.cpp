#include "OperatorMainView.h"
#include "MeshViewerWidget.h"
#include "PresetEditDialog.h"
#include "sn3dsdkdemoapp.h"
#include "sn3dsdkcorepub.h"
#include "sn3dsdkpluginpub.h"
#include "SDKCWrapper.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSettings>
#include <QFont>
#include <QImage>

// ─────────────────────────────────────────────────────────────────
// 전역 C-Style 콜백 (SDK가 C 함수 포인터만 허용)
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
    resize(1280, 800);

    m_settings  = new QSettings("./scansoftwaresdk.ini", QSettings::IniFormat);
    m_simTimer  = new QTimer(this);
    m_simTimer->setInterval(300);
    connect(m_simTimer, &QTimer::timeout, this, &OperatorMainView::onSimTimerTick);

    // ── 스택 위젯 ─────────────────────────────────────────────
    m_stack = new QStackedWidget(this);
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->addWidget(m_stack);

    buildPresetPage();
    buildScanPage();
    buildPreviewPage();

    m_stack->addWidget(m_presetPage);   // index 0
    m_stack->addWidget(m_scanPage);     // index 1
    m_stack->addWidget(m_previewPage);  // index 2
    m_stack->setCurrentIndex(0);

    applyGlobalStyle();

    // ── Sn3DSDKDemoApp 시그널 연결 ────────────────────────────
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

    connect(m_demoApp, &Sn3DSDKDemoApp::meshDataReturn,
            this, &OperatorMainView::onMeshDataReturn, Qt::QueuedConnection);

    connect(m_demoApp, &Sn3DSDKDemoApp::saveMeshDataReturn,
            this, &OperatorMainView::onSaveMeshDataReturn, Qt::QueuedConnection);

    // ── 비디오 프레임 시그널 (스레드 경계) ──────────────────
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

void OperatorMainView::closeEvent(QCloseEvent* event)
{
    releaseSDK();
    event->accept();
}

void OperatorMainView::videoCallBack(int camId, const unsigned char* data,
                                     int width, int height, int channel)
{
    emit sigGetVideoData(camId, data, width, height, channel);
}

// ═════════════════════════════════════════════════════════════════
// UI 구성 – Page 0 : 프리셋 선택
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::buildPresetPage()
{
    m_presetPage = new QWidget();
    auto* rootV  = new QVBoxLayout(m_presetPage);
    rootV->setContentsMargins(20, 16, 20, 16);
    rootV->setSpacing(12);

    // ── 상단 타이틀 바 ─────────────────────────────────────
    auto* topBar = new QWidget();
    topBar->setStyleSheet("background:#0D1B2A; border-radius:8px;");
    auto* topH = new QHBoxLayout(topBar);
    topH->setContentsMargins(18, 10, 18, 10);

    auto* title = new QLabel(QString::fromUtf8("◈  SNK 3D 스캐너 운영 시스템"));
    title->setStyleSheet("color:#4FC3F7; font-size:22px; font-weight:bold;"
                         "background:transparent;");

    m_simBadge0 = new QLabel(QString::fromUtf8("[ 시뮬레이션 모드 ]"));
    m_simBadge0->setStyleSheet(
        "color:#FF9800; font-size:13px; font-weight:bold;"
        "background:#2A1800; border:1px solid #FF9800;"
        "border-radius:4px; padding:2px 10px;");
    m_simBadge0->setVisible(false);

    auto* btnManage = new QPushButton(QString::fromUtf8("⚙  프리셋 관리"));
    btnManage->setFixedHeight(34);
    btnManage->setStyleSheet(
        "QPushButton { background:#2E3A4A; color:#CCC; border-radius:6px;"
        "  font-size:13px; padding:0 14px; }"
        "QPushButton:hover { background:#4A5A6A; }");

    topH->addWidget(title);
    topH->addSpacing(14);
    topH->addWidget(m_simBadge0);
    topH->addStretch();
    topH->addWidget(btnManage);
    rootV->addWidget(topBar);

    // ── 스크롤 영역 + 프리셋 그리드 ──────────────────────
    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(
        "QScrollArea { border:none; background:transparent; }"
        "QScrollBar:vertical { background:#1A1A2A; width:8px; }"
        "QScrollBar::handle:vertical { background:#444; border-radius:4px; }");

    m_presetGrid = new QWidget();
    m_presetGrid->setStyleSheet("background:transparent;");
    scroll->setWidget(m_presetGrid);
    rootV->addWidget(scroll, 1);

    // ── 하단 상태 표시 ─────────────────────────────────────
    m_presetStatus = new QLabel(QString::fromUtf8("스캔할 부품을 선택하세요"));
    m_presetStatus->setAlignment(Qt::AlignCenter);
    m_presetStatus->setStyleSheet("color:#888; font-size:13px;");
    rootV->addWidget(m_presetStatus);

    connect(btnManage, &QPushButton::clicked,
            this, &OperatorMainView::onBtnManagePresetClicked);

    refreshPresetButtons();
}

// ─────────────────────────────────────────────────────────────────
// 프리셋 그리드 재구성 (PresetManager → 버튼)
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::refreshPresetButtons()
{
    // 기존 레이아웃 제거
    QLayout* old = m_presetGrid->layout();
    if (old) {
        QLayoutItem* item;
        while ((item = old->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        delete old;
    }

    const auto& presets = PresetManager::instance().presets();

    auto* grid = new QGridLayout(m_presetGrid);
    grid->setContentsMargins(12, 12, 12, 12);
    grid->setSpacing(16);

    // 버튼 색상 팔레트
    static const char* colors[] = {
        "#1565C0", "#1B5E20", "#4A148C", "#E65100",
        "#006064", "#880E4F", "#1A237E", "#33691E"
    };
    static const char* hovers[] = {
        "#1976D2", "#2E7D32", "#6A1B9A", "#F57C00",
        "#00838F", "#AD1457", "#283593", "#558B2F"
    };
    static const int PALETTE = 8;
    const int COLS = 3;
    int col = 0, row = 0;

    for (int i = 0; i < presets.size(); ++i) {
        const auto& p = presets.at(i);
        const char* c = colors[i % PALETTE];
        const char* h = hovers[i % PALETTE];

        auto* btn = new QPushButton();
        btn->setMinimumSize(190, 140);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QString devStr  = deviceTypeStr(p.deviceType);
        QString modeStr = (p.scanMode == SNSDK_SCANMODE_LASER)
                          ? QString::fromUtf8("레이저")
                          : QString::fromUtf8("적외선");
        btn->setText(p.name + "\n\n" + devStr + "  │  " + modeStr);

        btn->setStyleSheet(
            QString(
                "QPushButton {"
                "  background:%1; color:white; border-radius:14px;"
                "  font-size:17px; font-weight:bold; padding:14px;"
                "  text-align:center; }"
                "QPushButton:hover   { background:%2; }"
                "QPushButton:pressed { background:#1A1A1A; }")
            .arg(c).arg(h));

        int idx = i;
        connect(btn, &QPushButton::clicked, this, [this, idx]() {
            onPresetSelected(idx);
        });

        grid->addWidget(btn, row, col);
        if (++col >= COLS) { col = 0; ++row; }
    }

    if (presets.isEmpty()) {
        auto* lbl = new QLabel(
            QString::fromUtf8(
                "등록된 프리셋이 없습니다.\n"
                "⚙ 프리셋 관리에서 추가하세요."));
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("color:#666; font-size:15px;");
        grid->addWidget(lbl, 0, 0, 1, COLS);
    }
}

// ═════════════════════════════════════════════════════════════════
// UI 구성 – Page 1 : 스캔 화면
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::buildScanPage()
{
    m_scanPage = new QWidget();
    auto* rootV = new QVBoxLayout(m_scanPage);
    rootV->setContentsMargins(12, 8, 12, 8);
    rootV->setSpacing(8);

    // ── 상단 타이틀 바 ─────────────────────────────────────
    auto* topBarW = new QWidget();
    topBarW->setStyleSheet("background:#1F497D; border-radius:6px;");
    auto* topH = new QHBoxLayout(topBarW);
    topH->setContentsMargins(14, 6, 14, 6);

    auto* topTitle = new QLabel(QString::fromUtf8("SNK 3D 스캐너  │  스캔 진행"));
    topTitle->setStyleSheet(
        "color:white; font-size:16px; font-weight:bold; background:transparent;");

    m_simBadge1 = new QLabel(QString::fromUtf8("[ 시뮬레이션 모드 ]"));
    m_simBadge1->setStyleSheet(
        "color:#FF9800; font-size:13px; font-weight:bold;"
        "background:transparent; border:1px solid #FF9800;"
        "border-radius:4px; padding:1px 8px;");
    m_simBadge1->setVisible(false);

    topH->addWidget(topTitle);
    topH->addSpacing(12);
    topH->addWidget(m_simBadge1);
    topH->addStretch();
    topBarW->setFixedHeight(40);
    rootV->addWidget(topBarW);

    // ── 중단: 카메라 + 상태 패널 ──────────────────────────
    auto* midH = new QHBoxLayout();
    midH->setSpacing(12);

    // 카메라 레이블 생성 헬퍼
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

    auto* camH = new QHBoxLayout();
    camH->addWidget(m_leftCamLbl);
    camH->addWidget(m_rightCamLbl);
    midH->addLayout(camH, 3);

    // 상태 패널 (오른쪽)
    auto* statusPanel = new QWidget();
    statusPanel->setMinimumWidth(220);
    statusPanel->setStyleSheet("background:#1A1A2E; border-radius:8px;");
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

    // 스캐너 물리 버튼 안내
    auto* guideLbl = new QLabel(
        QString::fromUtf8("ℹ 스캐너 버튼을 누르면\n스캔이 시작됩니다."));
    guideLbl->setAlignment(Qt::AlignCenter);
    guideLbl->setStyleSheet(
        "color:#888; font-size:12px; background:transparent;"
        "border:1px solid #333; border-radius:6px; padding:8px;");

    sV->addWidget(m_stateLbl);
    sV->addWidget(m_trackLbl);
    sV->addWidget(m_pointLbl);
    sV->addWidget(m_fpsLbl);
    sV->addWidget(m_distLbl);
    sV->addStretch();
    sV->addWidget(guideLbl);

    midH->addWidget(statusPanel, 1);
    rootV->addLayout(midH, 1);

    // ── 진행률 바 ─────────────────────────────────────────
    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    m_progressBar->setFixedHeight(18);
    m_progressBar->setStyleSheet(
        "QProgressBar { background:#222; border-radius:4px; }"
        "QProgressBar::chunk { background:#2E74B5; border-radius:4px; }");
    rootV->addWidget(m_progressBar);

    // ── 하단 제어 버튼 ─────────────────────────────────────
    auto* btnH = new QHBoxLayout();
    btnH->setSpacing(14);

    m_btnCancel = new QPushButton(QString::fromUtf8("← 취소"));
    m_btnCancel->setMinimumHeight(62);
    m_btnCancel->setFixedWidth(120);
    m_btnCancel->setStyleSheet(
        "QPushButton { background:#3A3A3A; color:#CCC; border-radius:10px;"
        "  font-size:14px; }"
        "QPushButton:hover { background:#555; }");

    m_btnScanCtrl = new QPushButton(QString::fromUtf8("▶  프리스캔"));
    m_btnScanCtrl->setMinimumHeight(62);
    m_btnScanCtrl->setStyleSheet(
        "QPushButton { background:#2E74B5; color:white; border-radius:10px;"
        "  font-size:19px; font-weight:bold; }"
        "QPushButton:hover { background:#1F497D; }"
        "QPushButton:disabled { background:#333; color:#666; }");

    m_btnEndScan = new QPushButton(QString::fromUtf8("■  스캔 종료"));
    m_btnEndScan->setMinimumHeight(62);
    m_btnEndScan->setStyleSheet(
        "QPushButton { background:#217346; color:white; border-radius:10px;"
        "  font-size:19px; font-weight:bold; }"
        "QPushButton:hover { background:#185030; }"
        "QPushButton:disabled { background:#333; color:#666; }");

    btnH->addWidget(m_btnCancel);
    btnH->addWidget(m_btnScanCtrl, 2);
    btnH->addWidget(m_btnEndScan,  1);
    rootV->addLayout(btnH);

    connect(m_btnScanCtrl, &QPushButton::clicked,
            this, &OperatorMainView::onBtnScanCtrlClicked);
    connect(m_btnEndScan,  &QPushButton::clicked,
            this, &OperatorMainView::onBtnEndScanClicked);
    connect(m_btnCancel,   &QPushButton::clicked,
            this, &OperatorMainView::onBtnCancelScanClicked);
}

// ═════════════════════════════════════════════════════════════════
// UI 구성 – Page 2 : 결과 미리보기
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::buildPreviewPage()
{
    m_previewPage = new QWidget();
    auto* rootV   = new QVBoxLayout(m_previewPage);
    rootV->setContentsMargins(12, 8, 12, 8);
    rootV->setSpacing(8);

    // ── 상단 타이틀 ────────────────────────────────────────
    auto* topBar = new QLabel(
        QString::fromUtf8("SNK 3D 스캐너  │  결과 미리보기"));
    topBar->setAlignment(Qt::AlignCenter);
    topBar->setFixedHeight(40);
    topBar->setStyleSheet(
        "background:#217346; color:white; font-size:16px; font-weight:bold;"
        "border-radius:6px;");
    rootV->addWidget(topBar);

    // ── 중단: 3D 뷰어 + 정보 패널 ─────────────────────────
    auto* midH = new QHBoxLayout();
    midH->setSpacing(12);

    // 3D 메시 뷰어
    m_meshViewer = new MeshViewerWidget();
    m_meshViewer->setMinimumSize(600, 380);
    m_meshViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    midH->addWidget(m_meshViewer, 3);

    // 정보 패널
    auto* infoPanel = new QWidget();
    infoPanel->setMinimumWidth(240);
    infoPanel->setStyleSheet("background:#1A2A1A; border-radius:8px;");
    auto* iV = new QVBoxLayout(infoPanel);
    iV->setContentsMargins(16, 16, 16, 16);
    iV->setSpacing(8);

    auto* infoTitle = new QLabel(QString::fromUtf8("✔  스캔 완료"));
    infoTitle->setStyleSheet(
        "color:#00C853; font-size:18px; font-weight:bold;"
        "background:transparent;");

    m_previewInfo = new QLabel();
    m_previewInfo->setWordWrap(true);
    m_previewInfo->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_previewInfo->setStyleSheet(
        "color:#CCCCCC; font-size:13px; background:transparent;"
        "line-height:160%;");

    auto* opHint = new QLabel(
        QString::fromUtf8(
            "🖱 드래그: 회전\n"
            "🖱 휠: 확대·축소"));
    opHint->setStyleSheet(
        "color:#666; font-size:12px; background:transparent;");

    iV->addWidget(infoTitle);
    iV->addSpacing(8);
    iV->addWidget(m_previewInfo);
    iV->addStretch();
    iV->addWidget(opHint);

    midH->addWidget(infoPanel, 1);
    rootV->addLayout(midH, 1);

    // ── 하단 버튼 ─────────────────────────────────────────
    auto* btnH = new QHBoxLayout();
    btnH->setSpacing(16);

    m_btnRescan = new QPushButton(QString::fromUtf8("↺  재스캔"));
    m_btnRescan->setMinimumHeight(62);
    m_btnRescan->setStyleSheet(
        "QPushButton { background:#E65100; color:white; border-radius:10px;"
        "  font-size:18px; font-weight:bold; }"
        "QPushButton:hover { background:#FF6D00; }");

    m_btnSaveOk = new QPushButton(QString::fromUtf8("✔  저장 완료"));
    m_btnSaveOk->setMinimumHeight(62);
    m_btnSaveOk->setStyleSheet(
        "QPushButton { background:#1565C0; color:white; border-radius:10px;"
        "  font-size:18px; font-weight:bold; }"
        "QPushButton:hover { background:#1976D2; }");

    btnH->addWidget(m_btnRescan, 1);
    btnH->addWidget(m_btnSaveOk, 2);
    rootV->addLayout(btnH);

    connect(m_btnRescan, &QPushButton::clicked,
            this, &OperatorMainView::onBtnRescanClicked);
    connect(m_btnSaveOk, &QPushButton::clicked,
            this, &OperatorMainView::onBtnSaveOkClicked);
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
// 슬롯 – Page 0 : 프리셋 선택
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onPresetSelected(int index)
{
    const auto& presets = PresetManager::instance().presets();
    if (index < 0 || index >= presets.size()) return;

    m_activePreset       = presets.at(index);
    m_currentProjectPath = generateProjectPath(m_activePreset);
    QDir().mkpath(m_currentProjectPath);

    updatePresetStatus(
        QString::fromUtf8("초기화 중: ") + m_activePreset.name, "#FFCC00");

    // SDK 초기화 (최초 1회)
    if (!m_sdkInitialized && !m_simMode) {
        if (!initSDK(m_activePreset)) {
            // 초기화 실패 → 시뮬레이션 모드
            m_simMode = true;
            m_simBadge0->setVisible(true);
            m_simBadge1->setVisible(true);
            updatePresetStatus(
                QString::fromUtf8("스캐너 없음 → 시뮬레이션 모드"), "#FF9800");
        }
    }

    m_scanStep.setDeviceType(m_activePreset.deviceType);
    m_scanStep.setScanMode(m_activePreset.scanMode);

    if (!m_simMode) {
        m_demoApp->Sn3DSetScanMode(m_activePreset.scanMode);
        createProject();          // → onNewProjFinished → enterScanPage()
    } else {
        enterScanPage();          // 시뮬레이션: 바로 스캔 화면으로
    }
}

void OperatorMainView::onBtnManagePresetClicked()
{
    PresetEditDialog dlg(this);
    dlg.exec();
    refreshPresetButtons();
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – Page 1 : 스캔 제어
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onBtnScanCtrlClicked()
{
    int state = m_scanStep.scanState();

    // ── 시뮬레이션 모드 ────────────────────────────────────
    if (m_simMode) {
        if (state == SNSDK_PRE_SCAN) {
            m_stateLbl->setText(QString::fromUtf8("상태: 프리스캔 중..."));
            m_btnScanCtrl->setEnabled(false);
            QTimer::singleShot(1500, this, [this]() {
                m_scanStep.setScanState(SNSDK_PRE_SCANNING);
                updateScanCtrlButton();
                m_stateLbl->setText(
                    QString::fromUtf8("상태: 스캔 준비 완료"));
                m_btnScanCtrl->setEnabled(true);
            });
        } else if (state == SNSDK_PRE_SCANNING) {
            m_scanStep.setScanState(SNSDK_SCAN);
            updateScanCtrlButton();
            m_stateLbl->setText(QString::fromUtf8("상태: 스캔 중"));
            m_simTickCount = 0;
            m_simTimer->start();
        } else if (state == SNSDK_SCAN) {
            m_simTimer->stop();
            m_scanStep.setScanState(SNSDK_PAUSED);
            updateScanCtrlButton();
            m_stateLbl->setText(QString::fromUtf8("상태: 일시정지"));
        } else if (state == SNSDK_PAUSED) {
            m_scanStep.setScanState(SNSDK_SCAN);
            updateScanCtrlButton();
            m_stateLbl->setText(QString::fromUtf8("상태: 스캔 중"));
            m_simTimer->start();
        }
        return;
    }

    // ── 실제 SDK ───────────────────────────────────────────
    m_demoApp->videoProcessControl(true);
    if (state == SNSDK_SCAN) {
        m_demoApp->Sn3DPauseScan();
    } else {
        m_demoApp->Sn3DStartScan();
    }
}

void OperatorMainView::onBtnEndScanClicked()
{
    m_btnEndScan->setEnabled(false);
    m_btnScanCtrl->setEnabled(false);

    if (m_simMode) {
        runSimEndScan();
        return;
    }

    // 실제 SDK: EndScan → onSnMessageReceived("generatePointCloudResult")
    //           → doAutoMesh → onMeshDataReturn
    //           → doAutoSave → onSaveMeshDataReturn → showPreviewPage
    m_stateLbl->setText(QString::fromUtf8("상태: 스캔 완료 처리 중..."));
    m_demoApp->Sn3DEndScan();
}

void OperatorMainView::onBtnCancelScanClicked()
{
    auto reply = QMessageBox::question(
        this,
        QString::fromUtf8("확인"),
        QString::fromUtf8("스캔을 취소하고 부품 선택으로 돌아가시겠습니까?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    m_simTimer->stop();
    m_simTickCount    = 0;
    m_lastPointCount  = 0;

    if (!m_simMode) {
        m_demoApp->Sn3DCancelScan();
        m_demoApp->videoProcessControl(false);
    }

    m_btnEndScan->setEnabled(true);
    m_btnScanCtrl->setEnabled(true);
    m_scanStep.setScanState(0);
    m_progressBar->setVisible(false);
    m_stack->setCurrentIndex(0);
    updatePresetStatus(QString::fromUtf8("스캔할 부품을 선택하세요"), "#888888");
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – Page 2 : 저장 완료 / 재스캔
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onBtnSaveOkClicked()
{
    // 파일은 이미 저장 완료 → 프리셋 선택으로 복귀
    m_lastPointCount = 0;
    m_meshViewer->clearMesh();
    m_scanStep.setScanState(0);
    m_btnEndScan->setEnabled(true);
    m_btnScanCtrl->setEnabled(true);
    m_stack->setCurrentIndex(0);
    updatePresetStatus(QString::fromUtf8("스캔할 부품을 선택하세요"), "#888888");
}

void OperatorMainView::onBtnRescanClicked()
{
    // 새 타임스탬프 폴더 생성 후 스캔 화면으로
    m_currentProjectPath = generateProjectPath(m_activePreset);
    QDir().mkpath(m_currentProjectPath);

    m_lastPointCount = 0;
    m_meshViewer->clearMesh();
    m_btnEndScan->setEnabled(true);
    m_btnScanCtrl->setEnabled(true);

    if (m_simMode) {
        enterScanPage();
    } else {
        createProject();   // → onNewProjFinished → enterScanPage()
    }
}

// ═════════════════════════════════════════════════════════════════
// 슬롯 – SDK 메시지 수신
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

        switch (state) {
        case SNSDK_PRE_SCAN:
            m_stateLbl->setText(QString::fromUtf8("상태: 프리스캔 대기")); break;
        case SNSDK_PRE_SCANNING:
            m_stateLbl->setText(QString::fromUtf8("상태: 프리스캔 중"));   break;
        case SNSDK_SCAN:
            m_stateLbl->setText(QString::fromUtf8("상태: 스캔 중"));       break;
        case SNSDK_PAUSED:
            m_stateLbl->setText(QString::fromUtf8("상태: 일시정지"));       break;
        case SNSDK_SCAN_STOPED:
            m_stateLbl->setText(QString::fromUtf8("상태: 스캔 정지"));      break;
        default:
            m_stateLbl->setText(QString::fromUtf8("상태: 알 수 없음"));
        }

    } else if (cmd == "createOrLoadCurProjectFinished") {
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
        if (result == 0) {
            m_stateLbl->setText(QString::fromUtf8("상태: 점군 생성 완료 → 메시 처리 중"));
            doAutoMesh();
        } else {
            m_stateLbl->setText(
                QString::fromUtf8("상태: 점군 생성 실패 (코드:%1)").arg(result));
            m_btnEndScan->setEnabled(true);
            m_btnScanCtrl->setEnabled(true);
        }
    }
}

void OperatorMainView::onNewProjFinished(QStringList /*projNameList*/)
{
    enterScanPage();
}

void OperatorMainView::onShowProgress(QString title, double value, bool visible)
{
    m_progressBar->setVisible(visible);
    m_progressBar->setValue(static_cast<int>(value * 100));
    if (visible && !title.isEmpty())
        m_stateLbl->setText(QString::fromUtf8("상태: ") + title);
}

void OperatorMainView::onPointCountChanged(int count)
{
    m_lastPointCount = count;
    m_pointLbl->setText(QString::fromUtf8("포인트: %L1").arg(count));
}

void OperatorMainView::onFrameRateChanged(int rate)
{
    m_fpsLbl->setText(QString("FPS: %1").arg(rate));
}

void OperatorMainView::onTrackLostStatusChanged(bool lost)
{
    if (lost) {
        m_trackLbl->setText(QString::fromUtf8("추적: ● 실패"));
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
    QString distStr = dist > 0
        ? QString::fromUtf8("거리: ● 적정 (%1mm)").arg(dist, 0, 'f', 0)
        : QString::fromUtf8("거리: ● 범위 벗어남");
    QString color = dist > 0 ? "#00FF88" : "#FFCC00";
    m_distLbl->setText(distStr);
    m_distLbl->setStyleSheet(
        QString("color:%1; font-size:15px; background:transparent; padding:4px;")
        .arg(color));
}

// ─────────────────────────────────────────────────────────────────
// 슬롯 – 메시 완료 (SDK 신호)
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::onMeshDataReturn(bool success)
{
    if (!success) {
        m_stateLbl->setText(QString::fromUtf8("상태: 메시 처리 실패"));
        m_progressBar->setVisible(false);
        m_btnEndScan->setEnabled(true);
        m_btnScanCtrl->setEnabled(true);
        return;
    }
    m_stateLbl->setText(QString::fromUtf8("상태: 메시 완료 → 저장 중..."));
    doAutoSave();
}

// ─────────────────────────────────────────────────────────────────
// 슬롯 – 메시 저장 완료 (SDK 신호)
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::onSaveMeshDataReturn(bool success)
{
    m_progressBar->setVisible(false);

    if (!success) {
        m_stateLbl->setText(QString::fromUtf8("상태: 저장 실패"));
        m_btnEndScan->setEnabled(true);
        m_btnScanCtrl->setEnabled(true);
        return;
    }

    // 저장된 STL 파일 로드하여 미리보기
    QString folderName = QFileInfo(m_currentProjectPath).fileName();
    QString stlPath = m_currentProjectPath + "/" + folderName + ".stl";
    m_meshViewer->loadSTL(stlPath);
    showPreviewPage();
}

// ═════════════════════════════════════════════════════════════════
// 시뮬레이션 타이머 틱 (스캔 중 포인트 수 증가)
// ═════════════════════════════════════════════════════════════════
void OperatorMainView::onSimTimerTick()
{
    if (m_scanStep.scanState() != SNSDK_SCAN) return;

    ++m_simTickCount;
    m_lastPointCount += 7000 + (qrand() % 4000);

    m_pointLbl->setText(
        QString::fromUtf8("포인트: %L1").arg(m_lastPointCount));
    m_fpsLbl->setText("FPS: 30");

    if (m_simTickCount == 1) {
        // 첫 틱: 거리·추적 초기화
        m_distLbl->setText(QString::fromUtf8("거리: ● 적정 (300mm)"));
        m_distLbl->setStyleSheet(
            "color:#00FF88; font-size:15px; background:transparent; padding:4px;");
        m_trackLbl->setText(QString::fromUtf8("추적: ● 정상"));
        m_trackLbl->setStyleSheet(
            "color:#00FF88; font-size:15px; background:transparent; padding:4px;");
    }
}

// ═════════════════════════════════════════════════════════════════
// 내부: SDK 초기화
// ═════════════════════════════════════════════════════════════════
bool OperatorMainView::initSDK(const ScanPreset& preset)
{
    int err = m_demoApp->Sn3DInitialize(preset.deviceType);
    if (err != 0) return false;

    m_demoApp->installGetImagasCallBack(OperatorVideoCallback, this);
    m_demoApp->Sn3DSetLaserGrade(preset.laserGrade);
    m_demoApp->Sn3DSetIsHighSpeed(preset.highSpeed);
    m_demoApp->Sn3DSetScanObject(preset.objectType);
    m_demoApp->Sn3DChangeBrightStep(preset.brightStep);

    if (preset.strongLight) {
        QString strResult;
        QVariantMap mp = {{"cmd","setIsStrongLight2"}, {"strong", true}};
        m_demoApp->SnSendMessage("v2.0", "request", "other", mp, strResult);
    }

    QString strResult;
    QVariantMap mp = {{"cmd","loadScanSoftSetting"}};
    m_demoApp->SnSendMessage("v2.0", "request", "other", mp, strResult);

    m_sdkInitialized = true;
    m_scanStep.setDeviceType(preset.deviceType);
    return true;
}

// ─────────────────────────────────────────────────────────────────
// 내부: 프로젝트 생성 (Sn3DNewProject 호출)
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::createProject()
{
    updatePresetStatus(QString::fromUtf8("프로젝트 생성 중..."), "#FFCC00");

    NewProject struNew;
    memset(&struNew, 0, sizeof(NewProject));
    auto pathStd = m_currentProjectPath.toStdString();
    struNew.szSlnDirPath = pathStd.c_str();
    struNew.iScanMode    = m_activePreset.scanMode;

    int err = m_demoApp->Sn3DNewProject(&struNew);
    if (err != 0) {
        updatePresetStatus(
            QString::fromUtf8("프로젝트 생성 오류: %1").arg(err), "#FF4444");
    }
    // → 완료는 onNewProjFinished()
}

// ─────────────────────────────────────────────────────────────────
// 내부: 스캔 화면 전환 (공통)
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::enterScanPage()
{
    if (!m_simMode) {
        m_demoApp->videoProcessControl(true);
        ScanType scanType = (m_activePreset.scanMode == SNSDK_SCANMODE_LASER)
                            ? ST_FreeScan_LASER : ST_FreeScan_Infrared;
        m_demoApp->Sn3DEnterScan(scanType);
    }

    m_scanStep.setScanState(SNSDK_PRE_SCAN);
    updateScanCtrlButton();
    m_stateLbl->setText(m_simMode
        ? QString::fromUtf8("상태: [시뮬] 프리스캔 버튼을 누르세요")
        : QString::fromUtf8("상태: 프리스캔 대기"));
    m_trackLbl->setText(QString::fromUtf8("추적: ●"));
    m_pointLbl->setText(QString::fromUtf8("포인트: 0"));
    m_fpsLbl->setText("FPS: --");
    m_distLbl->setText(QString::fromUtf8("거리: --"));
    m_progressBar->setVisible(false);
    m_btnEndScan->setEnabled(true);
    m_btnScanCtrl->setEnabled(true);

    m_stack->setCurrentIndex(1);
}

// ─────────────────────────────────────────────────────────────────
// 내부: 자동 메시 처리
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::doAutoMesh()
{
    m_progressBar->setVisible(true);
    m_progressBar->setValue(20);

    FreeScanMeshPar p;
    memset(&p, 0, sizeof(p));
    p.watertightType           = m_activePreset.watertightType;
    p.fileter_level            = m_activePreset.filterLevel;
    p.smoothValue              = m_activePreset.smoothValue;
    p.fill_small_hole_ischeck  = m_activePreset.fillSmallHole  ? 1 : 0;
    p.fill_small_hole_value    = m_activePreset.fillSmallHoleValue;
    p.remove_small_floating_parts = m_activePreset.removeFloating ? 1 : 0;
    p.remove_spike_ischeck     = m_activePreset.removeSpike;
    p.marker_hole_filling_ischeck = m_activePreset.markerHoleFilling;
    p.border_optimize          = false;
    p.max_trianlges_ischeck    = false;
    p.max_trianlges_value      = 0;
    p.neighborhood_perimeter   = 0;

    m_demoApp->Sn3DMesh(&p);
}

// ─────────────────────────────────────────────────────────────────
// 내부: 자동 저장 (STL 항상 + 프리셋 설정 형식)
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::doAutoSave()
{
    m_progressBar->setValue(70);
    m_stateLbl->setText(QString::fromUtf8("상태: 저장 중..."));

    QString folderName = QFileInfo(m_currentProjectPath).fileName();
    std::string fileNameStd = folderName.toStdString();
    std::string folderStd   = m_currentProjectPath.toStdString();

    FreeSaveMeshPar s;
    memset(&s, 0, sizeof(s));
    s.fileName        = fileNameStd.c_str();
    s.folderPath      = folderStd.c_str();
    s.format          = "010000000";   // STL 기준
    s.method          = "save";
    s.postpageVisible = 0;
    s.saveAscFile     = 0;
    s.saveStlFile     = 1;   // 미리보기용 항상 저장
    s.saveObjFile     = m_activePreset.saveObj ? 1 : 0;
    s.savePlyFile     = m_activePreset.savePly ? 1 : 0;
    s.saveP3File      = 0;
    s.save3MfFile     = 0;
    s.saveTxtFile     = 0;
    s.saveCsvFile     = 0;

    m_demoApp->Sn3DSaveMesh(&s);
}

// ─────────────────────────────────────────────────────────────────
// 내부: 미리보기 페이지 전환
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::showPreviewPage()
{
    m_progressBar->setVisible(false);

    QString folderName = QFileInfo(m_currentProjectPath).fileName();
    QStringList formats;
    formats << "STL";
    if (m_activePreset.saveObj) formats << "OBJ";
    if (m_activePreset.savePly) formats << "PLY";

    QString info = QString::fromUtf8(
        "프리셋:  %1\n"
        "장치:    %2\n\n"
        "저장 위치:\n%3\n\n"
        "포인트 수:\n%L4\n\n"
        "저장 형식:\n%5")
        .arg(m_activePreset.name)
        .arg(deviceTypeStr(m_activePreset.deviceType))
        .arg(m_currentProjectPath)
        .arg(m_lastPointCount)
        .arg(formats.join(", "));

    m_previewInfo->setText(info);
    m_stack->setCurrentIndex(2);
}

// ─────────────────────────────────────────────────────────────────
// 내부: 시뮬레이션 종료 (메시 시뮬 → 미리보기)
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::runSimEndScan()
{
    m_simTimer->stop();
    m_stateLbl->setText(QString::fromUtf8("상태: [시뮬] 메시 처리 중..."));
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);

    int* prog = new int(0);
    auto* t   = new QTimer(this);
    connect(t, &QTimer::timeout, this, [this, t, prog]() {
        *prog += 3;
        m_progressBar->setValue(*prog);
        if (*prog >= 100) {
            t->stop();
            t->deleteLater();
            delete prog;
            m_meshViewer->loadSimulationMesh();
            showPreviewPage();
        }
    });
    t->start(90);   // 90ms × ~34회 ≈ 3초
}

// ─────────────────────────────────────────────────────────────────
// 내부: 스캔 제어 버튼 텍스트/색상 업데이트
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::updateScanCtrlButton()
{
    int state = m_scanStep.scanState();
    QString text;
    QString bg;

    switch (state) {
    case SNSDK_PRE_SCAN:
        text = QString::fromUtf8("▶  프리스캔");  bg = "#2E74B5"; break;
    case SNSDK_PRE_SCANNING:
        text = QString::fromUtf8("▶  스캔 시작"); bg = "#217346"; break;
    case SNSDK_SCAN:
        text = QString::fromUtf8("⏸  일시정지"); bg = "#8B4513"; break;
    case SNSDK_PAUSED:
        text = QString::fromUtf8("▶  계속");     bg = "#217346"; break;
    default:
        text = m_scanStep.enablePrescan()
               ? QString::fromUtf8("▶  프리스캔")
               : QString::fromUtf8("▶  스캔 시작");
        bg = "#2E74B5";
    }

    m_btnScanCtrl->setText(text);
    m_btnScanCtrl->setStyleSheet(
        QString(
            "QPushButton { background:%1; color:white; border-radius:10px;"
            "  font-size:19px; font-weight:bold; }"
            "QPushButton:hover { background:#444; }"
            "QPushButton:disabled { background:#333; color:#666; }").arg(bg));
}

// ─────────────────────────────────────────────────────────────────
// 내부: 프리셋 선택 페이지 상태 메시지
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::updatePresetStatus(const QString& msg, const QString& color)
{
    if (m_presetStatus) {
        m_presetStatus->setText(msg);
        m_presetStatus->setStyleSheet(
            QString("color:%1; font-size:13px;").arg(color));
    }
}

// ─────────────────────────────────────────────────────────────────
// 내부: 타임스탬프 기반 프로젝트 경로
// ─────────────────────────────────────────────────────────────────
QString OperatorMainView::generateProjectPath(const ScanPreset& preset) const
{
    QString stamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    return preset.saveDirectory + "/" + stamp;
}

// ─────────────────────────────────────────────────────────────────
// 내부: 장치 타입 → 표시 문자열
// ─────────────────────────────────────────────────────────────────
QString OperatorMainView::deviceTypeStr(int type) const
{
    switch (type) {
    case SNSDK_DEVTYPE_UEPRO:    return "UE PRO";
    case SNSDK_DEVTYPE_COMBO:    return "COMBO";
    case SNSDK_DEVTYPE_TRIO:     return "TRIO";
    case SNSDK_DEVTYPE_UEP2:     return "UE PRO 2";
    case SNSDK_DEVTYPE_COMBOPLUS:return "COMBO PLUS";
    default:                     return "Unknown";
    }
}

// ─────────────────────────────────────────────────────────────────
// 내부: SDK 해제
// ─────────────────────────────────────────────────────────────────
void OperatorMainView::releaseSDK()
{
    m_simTimer->stop();
    if (m_sdkInitialized) {
        m_demoApp->videoProcessControl(false);
        m_demoApp->Sn3DRelease();
        m_sdkInitialized = false;
    }
}
