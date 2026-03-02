#pragma once
#include "PresetManager.h"
#include <QDialog>

class QComboBox;
class QCheckBox;
class QSpinBox;
class QLineEdit;
class QListWidget;
class QPushButton;
class QLabel;
class QStackedWidget;

// ─────────────────────────────────────────────────────────────────
// PresetEditDialog
//   - 프리셋 목록 관리 + 선택된 프리셋의 전체 파라미터 편집
//   - 왼쪽: 프리셋 목록 (추가/복사/삭제)
//   - 오른쪽: 탭 형태 설정 패널 (기본정보 / 스캔 설정 / 메시 처리 / 저장)
// ─────────────────────────────────────────────────────────────────
class PresetEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PresetEditDialog(QWidget* parent = nullptr);

private Q_SLOTS:
    void onPresetSelected(int row);
    void onAddPreset();
    void onCopyPreset();
    void onDeletePreset();
    void onSaveAndClose();
    void onCancel();
    void onBrowseDirectory();

private:
    void buildUI();
    void buildInfoPanel(QWidget* panel);
    void buildScanPanel(QWidget* panel);
    void buildMeshPanel(QWidget* panel);
    void buildSavePanel(QWidget* panel);

    void loadPresetToUI(const ScanPreset& p);
    ScanPreset readPresetFromUI() const;
    void refreshList();

    // ── 왼쪽: 프리셋 목록 ────────────────────────────────────
    QListWidget*  m_listWidget    = nullptr;
    QPushButton*  m_btnAdd        = nullptr;
    QPushButton*  m_btnCopy       = nullptr;
    QPushButton*  m_btnDelete     = nullptr;

    // ── 오른쪽 탭 ────────────────────────────────────────────
    QStackedWidget* m_tabStack    = nullptr;
    QPushButton*  m_tabBtns[4]   = {};     // 기본정보/스캔/메시/저장

    // ── [기본정보] 탭 ─────────────────────────────────────────
    QLineEdit*    m_edtName       = nullptr;
    QLineEdit*    m_edtDesc       = nullptr;
    QLineEdit*    m_edtSaveDir    = nullptr;

    // ── [스캔 설정] 탭 ───────────────────────────────────────
    QComboBox*    m_cmbDeviceType = nullptr;
    QComboBox*    m_cmbScanMode   = nullptr;
    QSpinBox*     m_spnBrightStep = nullptr;
    QComboBox*    m_cmbLaserGrade = nullptr;
    QComboBox*    m_cmbObjectType = nullptr;
    QCheckBox*    m_chkHighSpeed  = nullptr;
    QCheckBox*    m_chkStrongLight= nullptr;
    QCheckBox*    m_chkUseMarker  = nullptr;
    QCheckBox*    m_chkUseTexture = nullptr;

    // ── [메시 처리] 탭 ───────────────────────────────────────
    QComboBox*    m_cmbWatertight    = nullptr;
    QComboBox*    m_cmbFilterLevel   = nullptr;
    QSpinBox*     m_spnSmoothValue   = nullptr;
    QCheckBox*    m_chkFillHole      = nullptr;
    QSpinBox*     m_spnFillHoleVal   = nullptr;
    QCheckBox*    m_chkRemoveFloat   = nullptr;
    QCheckBox*    m_chkRemoveSpike   = nullptr;
    QCheckBox*    m_chkMarkerHole    = nullptr;

    // ── [저장 형식] 탭 ───────────────────────────────────────
    QCheckBox*    m_chkSaveStl    = nullptr;
    QCheckBox*    m_chkSaveObj    = nullptr;
    QCheckBox*    m_chkSavePly    = nullptr;

    // ── 하단 버튼 ────────────────────────────────────────────
    QPushButton*  m_btnSave       = nullptr;
    QPushButton*  m_btnCancel     = nullptr;

    int           m_currentRow    = 0;
};
