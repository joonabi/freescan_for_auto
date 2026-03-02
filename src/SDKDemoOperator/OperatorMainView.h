#pragma once
#include "sn3dsdkmainviewbase.h"
#include "scanstep.h"
#include "PresetManager.h"

#include <QCloseEvent>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QStackedWidget>
#include <QSettings>
#include <QTimer>
#include <atomic>

class Sn3DSDKDemoApp;
class MeshViewerWidget;

// ─────────────────────────────────────────────────────────────────
// OperatorMainView  –  3페이지 운영자 메인 창
//
//  Page 0 : 프리셋 선택  (부품별 큰 버튼 그리드)
//  Page 1 : 스캔 화면    (카메라 + 상태 + 제어)
//  Page 2 : 결과 미리보기 (3D MeshViewer + 저장/재스캔)
//
//  시뮬레이션 모드
//    SDK 초기화가 실패하면 자동으로 활성화.
//    실제 스캐너 없이 실행 파일만으로 UI 흐름 테스트 가능.
// ─────────────────────────────────────────────────────────────────
class OperatorMainView : public Sn3DSDKMainViewBase
{
    Q_OBJECT
public:
    explicit OperatorMainView(QWidget* parent = nullptr);
    ~OperatorMainView();

    void closeEvent(QCloseEvent* event) override;

    // SDK 전역 콜백 → 멤버 함수로 전달
    void videoCallBack(int camId, const unsigned char* data,
                       int width, int height, int channel);

Q_SIGNALS:
    void sigGetVideoData(int camId, const unsigned char* data,
                         int width, int height, int channel);

private Q_SLOTS:
    // ── Page 0 : 프리셋 선택 ──────────────────────────────────────
    void onPresetSelected(int index);       // 프리셋 버튼 클릭
    void onBtnManagePresetClicked();        // 프리셋 관리 다이얼로그
    void refreshPresetButtons();            // PresetManager → 버튼 재구성

    // ── Page 1 : 스캔 화면 ────────────────────────────────────────
    void onBtnScanCtrlClicked();            // 프리스캔/스캔/일시정지/계속
    void onBtnEndScanClicked();             // 스캔 종료 → 자동 메시·저장
    void onBtnCancelScanClicked();          // 취소 → Page 0

    // ── Page 2 : 결과 미리보기 ────────────────────────────────────
    void onBtnSaveOkClicked();              // 저장 완료 → Page 0
    void onBtnRescanClicked();              // 재스캔 → 새 폴더 → Page 1

    // ── Sn3DSDKDemoApp 시그널 ──────────────────────────────────────
    void onSnMessageReceived(QVariantMap mapParam);
    void onNewProjFinished(QStringList projNameList);
    void onShowProgress(QString title, double value, bool visible);
    void onPointCountChanged(int count);
    void onFrameRateChanged(int rate);
    void onTrackLostStatusChanged(bool lost);
    void onScanDistChanged(double dist);
    void onMeshDataReturn(bool success);
    void onSaveMeshDataReturn(bool success);

    // ── 시뮬레이션 타이머 ─────────────────────────────────────────
    void onSimTimerTick();

private:
    // ── UI 구성 ───────────────────────────────────────────────────
    void buildPresetPage();
    void buildScanPage();
    void buildPreviewPage();
    void applyGlobalStyle();

    // ── 스캔 흐름 헬퍼 ───────────────────────────────────────────
    bool initSDK(const ScanPreset& preset);
    void createProject();
    void enterScanPage();
    void doAutoMesh();
    void doAutoSave();
    void showPreviewPage();
    void updateScanCtrlButton();
    void updatePresetStatus(const QString& msg, const QString& color = "#888888");
    void releaseSDK();

    QString generateProjectPath(const ScanPreset& preset) const;
    QString deviceTypeStr(int type) const;

    // ── 시뮬레이션 헬퍼 ──────────────────────────────────────────
    void runSimEndScan();   // 종료 시뮬레이션 (진행바 → 미리보기)

    // ── 앱 / 상태 ────────────────────────────────────────────────
    Sn3DSDKDemoApp* m_demoApp        = nullptr;
    ScanStep        m_scanStep;
    ScanPreset      m_activePreset;
    QSettings*      m_settings       = nullptr;
    bool            m_sdkInitialized = false;
    bool            m_simMode        = false;
    QString         m_currentProjectPath;
    int             m_lastPointCount = 0;

    // ── 스택 위젯 ─────────────────────────────────────────────────
    QStackedWidget* m_stack = nullptr;

    // ── Page 0 : 프리셋 선택 ──────────────────────────────────────
    QWidget* m_presetPage   = nullptr;
    QWidget* m_presetGrid   = nullptr;   // 버튼 그리드 컨테이너
    QLabel*  m_presetStatus = nullptr;   // 하단 상태 메시지
    QLabel*  m_simBadge0    = nullptr;   // "[시뮬레이션 모드]" 배지

    // ── Page 1 : 스캔 화면 ────────────────────────────────────────
    QWidget*      m_scanPage    = nullptr;
    QLabel*       m_leftCamLbl  = nullptr;
    QLabel*       m_rightCamLbl = nullptr;
    QLabel*       m_stateLbl    = nullptr;
    QLabel*       m_trackLbl    = nullptr;
    QLabel*       m_pointLbl    = nullptr;
    QLabel*       m_fpsLbl      = nullptr;
    QLabel*       m_distLbl     = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QPushButton*  m_btnScanCtrl = nullptr;
    QPushButton*  m_btnEndScan  = nullptr;
    QPushButton*  m_btnCancel   = nullptr;
    QLabel*       m_simBadge1   = nullptr;   // "[시뮬레이션 모드]" 배지

    // ── Page 2 : 결과 미리보기 ────────────────────────────────────
    QWidget*          m_previewPage = nullptr;
    MeshViewerWidget* m_meshViewer  = nullptr;
    QLabel*           m_previewInfo = nullptr;
    QPushButton*      m_btnSaveOk   = nullptr;
    QPushButton*      m_btnRescan   = nullptr;

    // ── 시뮬레이션 타이머 ─────────────────────────────────────────
    QTimer* m_simTimer     = nullptr;
    int     m_simTickCount = 0;
};
