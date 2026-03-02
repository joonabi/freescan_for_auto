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
#include <atomic>

class Sn3DSDKDemoApp;

// ─────────────────────────────────────────────────────────────────
// OperatorMainView
//   - 운영자 전용 메인 창 (최소화된 UI)
//   - 기존 Sn3DSDKFreeScanMainView 를 대체
//   - Page 0 : 시작 화면 (로고 + 3개 버튼)
//   - Page 1 : 스캔 화면 (카메라 미리보기 + 상태 + 제어)
// ─────────────────────────────────────────────────────────────────
class OperatorMainView : public Sn3DSDKMainViewBase
{
    Q_OBJECT
public:
    explicit OperatorMainView(QWidget* parent = nullptr);
    ~OperatorMainView();

    void closeEvent(QCloseEvent* event) override;

    // SDK 전역 콜백에서 호출 (외부 C 콜백 → 멤버 함수)
    void videoCallBack(int camId, const unsigned char* data,
                       int width, int height, int channel);

Q_SIGNALS:
    // 카메라 영상 전달용 (스레드 경계 넘기기)
    void sigGetVideoData(int camId, const unsigned char* data,
                         int width, int height, int channel);

private Q_SLOTS:
    // ── 시작 화면 버튼 ──────────────────────────────────────────
    void onBtnNewProjectClicked();   // 프로젝트 생성
    void onBtnOpenProjectClicked();  // 프로젝트 열기
    void onBtnPresetClicked();       // 프리셋 설정

    // ── 스캔 화면 버튼 ──────────────────────────────────────────
    void onBtnScanCtrlClicked();     // 프리스캔/스캔시작/일시정지/계속
    void onBtnEndScanClicked();      // 스캔 완료
    void onBtnBackClicked();         // 처음 화면으로

    // ── Sn3DSDKDemoApp 시그널 수신 ──────────────────────────────
    void onSnMessageReceived(QVariantMap mapParam);
    void onNewProjFinished(QStringList projNameList);
    void onShowProgress(QString title, double value, bool visible);
    void onPointCountChanged(int count);
    void onFrameRateChanged(int rate);
    void onTrackLostStatusChanged(bool lost);
    void onScanDistChanged(double dist);

private:
    // ── UI 초기화 ───────────────────────────────────────────────
    void buildStartPage();
    void buildScanPage();
    void applyGlobalStyle();

    // ── 동작 헬퍼 ───────────────────────────────────────────────
    bool  initSDK(const ScanPreset& preset);       // Sn3DInitialize + 콜백 등록 + 설정 적용
    bool  createAndEnterProject(const ScanPreset& preset);  // NewProject + EnterScan
    void  enterScanPage();                         // 스캔 화면으로 전환
    void  updateScanCtrlButton();                  // 상태에 따라 버튼 텍스트 변경
    void  updateStatusArea(const QString& msg, const QString& color = "#00FF00");
    void  releaseSDK();

    QString generateProjectPath(const ScanPreset& preset) const;

    // ── 앱 / 상태 ───────────────────────────────────────────────
    Sn3DSDKDemoApp* m_demoApp        = nullptr;
    ScanStep        m_scanStep;
    ScanPreset      m_activePreset;
    QSettings*      m_settings       = nullptr;
    bool            m_sdkInitialized = false;

    // ── 전체 스택 ───────────────────────────────────────────────
    QStackedWidget* m_stack = nullptr;

    // ── Page 0 : 시작 화면 ──────────────────────────────────────
    QWidget*     m_startPage  = nullptr;
    QLabel*      m_logoLabel  = nullptr;   // 회사명 / 로고
    QPushButton* m_btnNewProj = nullptr;
    QPushButton* m_btnOpenProj= nullptr;
    QPushButton* m_btnPreset  = nullptr;
    QLabel*      m_startStatus= nullptr;   // 하단 상태 메시지

    // ── Page 1 : 스캔 화면 ──────────────────────────────────────
    QWidget*     m_scanPage   = nullptr;
    QLabel*      m_leftCamLbl = nullptr;   // 왼쪽 카메라 영상
    QLabel*      m_rightCamLbl= nullptr;   // 오른쪽 카메라 영상
    QLabel*      m_stateLbl   = nullptr;   // 현재 스캔 상태
    QLabel*      m_trackLbl   = nullptr;   // 추적 상태
    QLabel*      m_pointLbl   = nullptr;   // 포인트 수
    QLabel*      m_fpsLbl     = nullptr;   // 프레임레이트
    QLabel*      m_distLbl    = nullptr;   // 스캔 거리
    QProgressBar*m_progressBar= nullptr;  // 작업 진행률
    QPushButton* m_btnScanCtrl= nullptr;  // 프리스캔/스캔/일시정지/계속
    QPushButton* m_btnEndScan = nullptr;  // 스캔 완료
    QPushButton* m_btnBack    = nullptr;  // 시작 화면으로
};
