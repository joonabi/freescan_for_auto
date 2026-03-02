#include "PresetEditDialog.h"
#include "sn3dsdkcorepub.h"     // SNSDK_SCANMODE_*, SNSDK_DEVTYPE_*
#include "sn3dsdkpluginpub.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QVBoxLayout>

// ─────────────────────────────────────────────────────────────────
// 생성자
// ─────────────────────────────────────────────────────────────────
PresetEditDialog::PresetEditDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QString::fromUtf8("프리셋 설정"));
    setMinimumSize(820, 580);
    buildUI();

    refreshList();
    if (m_listWidget->count() > 0) {
        // 현재 프리셋 선택
        const QString& cur = PresetManager::instance().currentPreset().name;
        for (int i = 0; i < m_listWidget->count(); ++i) {
            if (m_listWidget->item(i)->text() == cur) {
                m_listWidget->setCurrentRow(i);
                break;
            }
        }
        if (m_listWidget->currentRow() < 0)
            m_listWidget->setCurrentRow(0);
    }
}

// ─────────────────────────────────────────────────────────────────
// buildUI : 전체 레이아웃 구성
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::buildUI()
{
    setStyleSheet(
        "QDialog { background:#1a1a2e; color:#E0E0E0; }"
        "QGroupBox { border:1px solid #444; border-radius:6px;"
        "            margin-top:8px; color:#AAAAAA; }"
        "QGroupBox::title { subcontrol-origin:margin; left:8px; }"
        "QLabel { color:#CCCCCC; }"
        "QLineEdit, QComboBox, QSpinBox {"
        "  background:#2a2a3e; color:#E0E0E0;"
        "  border:1px solid #555; border-radius:4px; padding:3px; }"
        "QCheckBox { color:#CCCCCC; }"
        "QCheckBox::indicator { width:16px; height:16px; }"
        "QListWidget {"
        "  background:#12121f; color:#CCCCCC;"
        "  border:1px solid #444; border-radius:4px; }"
        "QListWidget::item:selected {"
        "  background:#2E74B5; color:white; }"
        "QPushButton {"
        "  background:#333355; color:#E0E0E0; border-radius:5px;"
        "  padding:5px 14px; }"
        "QPushButton:hover { background:#555577; }"
    );

    auto* mainH = new QHBoxLayout(this);
    mainH->setSpacing(14);
    mainH->setContentsMargins(14, 14, 14, 14);

    // ─── 왼쪽: 프리셋 목록 패널 ──────────────────────────────
    auto* leftW = new QWidget();
    leftW->setFixedWidth(200);
    auto* leftV = new QVBoxLayout(leftW);
    leftV->setContentsMargins(0, 0, 0, 0);
    leftV->setSpacing(6);

    auto* listTitle = new QLabel(QString::fromUtf8("프리셋 목록"));
    listTitle->setStyleSheet("color:#AAAAAA; font-size:12px;");
    leftV->addWidget(listTitle);

    m_listWidget = new QListWidget();
    m_listWidget->setAlternatingRowColors(true);
    leftV->addWidget(m_listWidget, 1);

    auto* listBtnH = new QHBoxLayout();
    m_btnAdd    = new QPushButton("+");
    m_btnCopy   = new QPushButton(QString::fromUtf8("복사"));
    m_btnDelete = new QPushButton(QString::fromUtf8("삭제"));
    m_btnDelete->setStyleSheet(
        "QPushButton { background:#5a1a1a; color:#E0E0E0; border-radius:5px; padding:5px 8px; }"
        "QPushButton:hover { background:#8a2a2a; }");
    listBtnH->addWidget(m_btnAdd);
    listBtnH->addWidget(m_btnCopy);
    listBtnH->addWidget(m_btnDelete);
    leftV->addLayout(listBtnH);

    connect(m_listWidget, &QListWidget::currentRowChanged,
            this, &PresetEditDialog::onPresetSelected);
    connect(m_btnAdd,    &QPushButton::clicked, this, &PresetEditDialog::onAddPreset);
    connect(m_btnCopy,   &QPushButton::clicked, this, &PresetEditDialog::onCopyPreset);
    connect(m_btnDelete, &QPushButton::clicked, this, &PresetEditDialog::onDeletePreset);

    mainH->addWidget(leftW);

    // ─── 오른쪽: 탭 + 설정 패널 ──────────────────────────────
    auto* rightV = new QVBoxLayout();
    rightV->setSpacing(8);

    // 탭 버튼 행
    const char* tabNames[] = {"기본정보", "스캔 설정", "메시 처리", "저장 형식"};
    auto* tabBtnH = new QHBoxLayout();
    tabBtnH->setSpacing(4);
    m_tabStack = new QStackedWidget();

    for (int i = 0; i < 4; ++i) {
        m_tabBtns[i] = new QPushButton(QString::fromUtf8(tabNames[i]));
        m_tabBtns[i]->setCheckable(true);
        m_tabBtns[i]->setMinimumHeight(34);
        tabBtnH->addWidget(m_tabBtns[i]);

        connect(m_tabBtns[i], &QPushButton::clicked, this, [this, i]() {
            m_tabStack->setCurrentIndex(i);
            for (int j = 0; j < 4; ++j)
                m_tabBtns[j]->setChecked(j == i);
        });
    }
    m_tabBtns[0]->setChecked(true);
    rightV->addLayout(tabBtnH);

    // 탭 페이지들
    auto* pageInfo  = new QWidget(); buildInfoPanel(pageInfo);
    auto* pageScan  = new QWidget(); buildScanPanel(pageScan);
    auto* pageMesh  = new QWidget(); buildMeshPanel(pageMesh);
    auto* pageSave  = new QWidget(); buildSavePanel(pageSave);

    m_tabStack->addWidget(pageInfo);
    m_tabStack->addWidget(pageScan);
    m_tabStack->addWidget(pageMesh);
    m_tabStack->addWidget(pageSave);
    m_tabStack->setCurrentIndex(0);
    rightV->addWidget(m_tabStack, 1);

    // 하단 확인/취소 버튼
    auto* botH = new QHBoxLayout();
    botH->addStretch();
    m_btnSave   = new QPushButton(QString::fromUtf8("저장 및 닫기"));
    m_btnSave->setStyleSheet(
        "QPushButton { background:#2E74B5; color:white; border-radius:5px; padding:6px 22px; }"
        "QPushButton:hover { background:#1F497D; }");
    m_btnCancel = new QPushButton(QString::fromUtf8("취소"));
    botH->addWidget(m_btnSave);
    botH->addWidget(m_btnCancel);
    rightV->addLayout(botH);

    connect(m_btnSave,   &QPushButton::clicked, this, &PresetEditDialog::onSaveAndClose);
    connect(m_btnCancel, &QPushButton::clicked, this, &PresetEditDialog::onCancel);

    mainH->addLayout(rightV, 1);
}

// ─────────────────────────────────────────────────────────────────
// 탭 1: 기본 정보
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::buildInfoPanel(QWidget* panel)
{
    auto* form = new QFormLayout(panel);
    form->setContentsMargins(14, 10, 14, 10);
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_edtName    = new QLineEdit();
    m_edtDesc    = new QLineEdit();
    m_edtSaveDir = new QLineEdit();

    auto* dirRow = new QHBoxLayout();
    auto* btnBrowse = new QPushButton("...");
    btnBrowse->setFixedWidth(36);
    dirRow->addWidget(m_edtSaveDir);
    dirRow->addWidget(btnBrowse);

    form->addRow(QString::fromUtf8("프리셋 이름:"), m_edtName);
    form->addRow(QString::fromUtf8("설명:"),         m_edtDesc);
    form->addRow(QString::fromUtf8("저장 폴더:"),    dirRow);

    connect(btnBrowse, &QPushButton::clicked,
            this, &PresetEditDialog::onBrowseDirectory);
}

// ─────────────────────────────────────────────────────────────────
// 탭 2: 스캔 설정
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::buildScanPanel(QWidget* panel)
{
    auto* form = new QFormLayout(panel);
    form->setContentsMargins(14, 10, 14, 10);
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 장치 종류
    m_cmbDeviceType = new QComboBox();
    m_cmbDeviceType->addItem("COMBO",      SNSDK_DEVTYPE_COMBO);
    m_cmbDeviceType->addItem("UE PRO",     SNSDK_DEVTYPE_UEPRO);
    m_cmbDeviceType->addItem("UE PRO 2",   SNSDK_DEVTYPE_UEP2);
    m_cmbDeviceType->addItem("TRIO",       SNSDK_DEVTYPE_TRIO);
    m_cmbDeviceType->addItem("COMBO PLUS", SNSDK_DEVTYPE_COMBOPLUS);

    // 스캔 모드
    m_cmbScanMode = new QComboBox();
    m_cmbScanMode->addItem(QString::fromUtf8("레이저 스캔"),  SNSDK_SCANMODE_LASER);
    m_cmbScanMode->addItem(QString::fromUtf8("적외선 스캔"),  SNSDK_SCANMODE_INFRARED);

    // 밝기 단계
    m_spnBrightStep = new QSpinBox();
    m_spnBrightStep->setRange(1, 10);
    m_spnBrightStep->setSuffix(QString::fromUtf8(" 단계"));

    // 레이저 등급
    m_cmbLaserGrade = new QComboBox();
    m_cmbLaserGrade->addItem(QString::fromUtf8("OFF (0)"),    0);
    m_cmbLaserGrade->addItem(QString::fromUtf8("단선 (1)"),   1);
    m_cmbLaserGrade->addItem(QString::fromUtf8("26선 (2)"),   2);
    m_cmbLaserGrade->addItem(QString::fromUtf8("프리미엄 (3)"), 3);

    // 대상 소재
    m_cmbObjectType = new QComboBox();
    m_cmbObjectType->addItem(QString::fromUtf8("일반 (Normal)"), 0);
    m_cmbObjectType->addItem(QString::fromUtf8("검정 (Black)"),  1);
    m_cmbObjectType->addItem(QString::fromUtf8("반사 (Reflect)"),2);

    // 체크박스 옵션
    m_chkHighSpeed   = new QCheckBox(QString::fromUtf8("고속 모드 (High Speed)"));
    m_chkStrongLight = new QCheckBox(QString::fromUtf8("강광 모드 (Strong Light)"));
    m_chkUseMarker   = new QCheckBox(QString::fromUtf8("마커 사용 (Use Marker)"));
    m_chkUseTexture  = new QCheckBox(QString::fromUtf8("텍스처 사용 (Use Texture)"));

    form->addRow(QString::fromUtf8("장치 종류:"),   m_cmbDeviceType);
    form->addRow(QString::fromUtf8("스캔 모드:"),   m_cmbScanMode);
    form->addRow(QString::fromUtf8("밝기 단계:"),   m_spnBrightStep);
    form->addRow(QString::fromUtf8("레이저 등급:"), m_cmbLaserGrade);
    form->addRow(QString::fromUtf8("대상 소재:"),   m_cmbObjectType);
    form->addRow("", m_chkHighSpeed);
    form->addRow("", m_chkStrongLight);
    form->addRow("", m_chkUseMarker);
    form->addRow("", m_chkUseTexture);
}

// ─────────────────────────────────────────────────────────────────
// 탭 3: 메시 처리
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::buildMeshPanel(QWidget* panel)
{
    auto* form = new QFormLayout(panel);
    form->setContentsMargins(14, 10, 14, 10);
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_cmbWatertight = new QComboBox();
    m_cmbWatertight->addItem(QString::fromUtf8("없음 (None)"),      0);
    m_cmbWatertight->addItem(QString::fromUtf8("완전 밀폐 (Full)"), 1);
    m_cmbWatertight->addItem(QString::fromUtf8("반밀폐 (Half)"),    2);

    m_cmbFilterLevel = new QComboBox();
    m_cmbFilterLevel->addItem(QString::fromUtf8("없음 (0)"),    0);
    m_cmbFilterLevel->addItem(QString::fromUtf8("약함 (1)"),    1);
    m_cmbFilterLevel->addItem(QString::fromUtf8("보통 (2)"),    2);
    m_cmbFilterLevel->addItem(QString::fromUtf8("강함 (3)"),    3);

    m_spnSmoothValue = new QSpinBox();
    m_spnSmoothValue->setRange(0, 10);
    m_spnSmoothValue->setSuffix(QString::fromUtf8(" 단계"));

    m_chkFillHole    = new QCheckBox(QString::fromUtf8("작은 구멍 메우기 (Fill Small Hole)"));
    m_spnFillHoleVal = new QSpinBox();
    m_spnFillHoleVal->setRange(1, 20);
    m_spnFillHoleVal->setSuffix(" mm");

    m_chkRemoveFloat = new QCheckBox(QString::fromUtf8("부유 데이터 제거 (Remove Floating)"));
    m_chkRemoveSpike = new QCheckBox(QString::fromUtf8("스파이크 제거 (Remove Spike)"));
    m_chkMarkerHole  = new QCheckBox(QString::fromUtf8("마커 구멍 메우기 (Marker Hole Filling)"));

    form->addRow(QString::fromUtf8("워터타이트:"),  m_cmbWatertight);
    form->addRow(QString::fromUtf8("필터 레벨:"),   m_cmbFilterLevel);
    form->addRow(QString::fromUtf8("스무스 강도:"),  m_spnSmoothValue);
    form->addRow("", m_chkFillHole);
    form->addRow(QString::fromUtf8("구멍 크기 (mm):"), m_spnFillHoleVal);
    form->addRow("", m_chkRemoveFloat);
    form->addRow("", m_chkRemoveSpike);
    form->addRow("", m_chkMarkerHole);
}

// ─────────────────────────────────────────────────────────────────
// 탭 4: 저장 형식
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::buildSavePanel(QWidget* panel)
{
    auto* vlay = new QVBoxLayout(panel);
    vlay->setContentsMargins(14, 16, 14, 16);
    vlay->setSpacing(16);

    auto* grp = new QGroupBox(QString::fromUtf8("자동 저장 파일 형식 선택"));
    auto* gV  = new QVBoxLayout(grp);
    gV->setSpacing(10);

    m_chkSaveStl = new QCheckBox("STL");
    m_chkSaveObj = new QCheckBox("OBJ");
    m_chkSavePly = new QCheckBox("PLY");

    gV->addWidget(m_chkSaveStl);
    gV->addWidget(m_chkSaveObj);
    gV->addWidget(m_chkSavePly);

    vlay->addWidget(grp);
    vlay->addStretch();
}

// ─────────────────────────────────────────────────────────────────
// 프리셋 목록 새로고침
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::refreshList()
{
    m_listWidget->clear();
    for (const auto& p : PresetManager::instance().presets())
        m_listWidget->addItem(p.name);
}

// ─────────────────────────────────────────────────────────────────
// 프리셋 선택 → UI에 값 로드
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::onPresetSelected(int row)
{
    if (row < 0 || row >= PresetManager::instance().presets().size()) return;
    m_currentRow = row;
    loadPresetToUI(PresetManager::instance().presets().at(row));
}

// ─────────────────────────────────────────────────────────────────
// UI → ScanPreset 읽기
// ─────────────────────────────────────────────────────────────────
ScanPreset PresetEditDialog::readPresetFromUI() const
{
    ScanPreset p;
    p.name          = m_edtName->text();
    p.description   = m_edtDesc->text();
    p.saveDirectory = m_edtSaveDir->text();

    p.deviceType    = m_cmbDeviceType->currentData().toInt();
    p.scanMode      = m_cmbScanMode->currentData().toInt();
    p.brightStep    = m_spnBrightStep->value();
    p.laserGrade    = m_cmbLaserGrade->currentData().toInt();
    p.objectType    = m_cmbObjectType->currentData().toInt();
    p.highSpeed     = m_chkHighSpeed->isChecked();
    p.strongLight   = m_chkStrongLight->isChecked();
    p.useMarker     = m_chkUseMarker->isChecked();
    p.useTexture    = m_chkUseTexture->isChecked();

    p.watertightType    = m_cmbWatertight->currentData().toInt();
    p.filterLevel       = m_cmbFilterLevel->currentData().toInt();
    p.smoothValue       = m_spnSmoothValue->value();
    p.fillSmallHole     = m_chkFillHole->isChecked();
    p.fillSmallHoleValue= m_spnFillHoleVal->value();
    p.removeFloating    = m_chkRemoveFloat->isChecked();
    p.removeSpike       = m_chkRemoveSpike->isChecked();
    p.markerHoleFilling = m_chkMarkerHole->isChecked();

    p.saveStl = m_chkSaveStl->isChecked();
    p.saveObj = m_chkSaveObj->isChecked();
    p.savePly = m_chkSavePly->isChecked();
    return p;
}

// ─────────────────────────────────────────────────────────────────
// ScanPreset → UI에 값 설정
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::loadPresetToUI(const ScanPreset& p)
{
    m_edtName->setText(p.name);
    m_edtDesc->setText(p.description);
    m_edtSaveDir->setText(p.saveDirectory);

    // deviceType
    for (int i = 0; i < m_cmbDeviceType->count(); ++i)
        if (m_cmbDeviceType->itemData(i).toInt() == p.deviceType) {
            m_cmbDeviceType->setCurrentIndex(i); break; }

    // scanMode
    for (int i = 0; i < m_cmbScanMode->count(); ++i)
        if (m_cmbScanMode->itemData(i).toInt() == p.scanMode) {
            m_cmbScanMode->setCurrentIndex(i); break; }

    m_spnBrightStep->setValue(p.brightStep);

    for (int i = 0; i < m_cmbLaserGrade->count(); ++i)
        if (m_cmbLaserGrade->itemData(i).toInt() == p.laserGrade) {
            m_cmbLaserGrade->setCurrentIndex(i); break; }

    for (int i = 0; i < m_cmbObjectType->count(); ++i)
        if (m_cmbObjectType->itemData(i).toInt() == p.objectType) {
            m_cmbObjectType->setCurrentIndex(i); break; }

    m_chkHighSpeed->setChecked(p.highSpeed);
    m_chkStrongLight->setChecked(p.strongLight);
    m_chkUseMarker->setChecked(p.useMarker);
    m_chkUseTexture->setChecked(p.useTexture);

    for (int i = 0; i < m_cmbWatertight->count(); ++i)
        if (m_cmbWatertight->itemData(i).toInt() == p.watertightType) {
            m_cmbWatertight->setCurrentIndex(i); break; }

    for (int i = 0; i < m_cmbFilterLevel->count(); ++i)
        if (m_cmbFilterLevel->itemData(i).toInt() == p.filterLevel) {
            m_cmbFilterLevel->setCurrentIndex(i); break; }

    m_spnSmoothValue->setValue(p.smoothValue);
    m_chkFillHole->setChecked(p.fillSmallHole);
    m_spnFillHoleVal->setValue(p.fillSmallHoleValue);
    m_chkRemoveFloat->setChecked(p.removeFloating);
    m_chkRemoveSpike->setChecked(p.removeSpike);
    m_chkMarkerHole->setChecked(p.markerHoleFilling);

    m_chkSaveStl->setChecked(p.saveStl);
    m_chkSaveObj->setChecked(p.saveObj);
    m_chkSavePly->setChecked(p.savePly);
}

// ─────────────────────────────────────────────────────────────────
// 프리셋 추가
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::onAddPreset()
{
    ScanPreset np;
    np.name = QString::fromUtf8("새 프리셋 %1")
                  .arg(PresetManager::instance().presets().size() + 1);
    PresetManager::instance().addOrUpdate(np);
    refreshList();
    m_listWidget->setCurrentRow(m_listWidget->count() - 1);
}

// ─────────────────────────────────────────────────────────────────
// 프리셋 복사
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::onCopyPreset()
{
    int row = m_listWidget->currentRow();
    if (row < 0) return;
    ScanPreset cp = PresetManager::instance().presets().at(row);
    cp.name += QString::fromUtf8(" (복사)");
    PresetManager::instance().addOrUpdate(cp);
    refreshList();
    m_listWidget->setCurrentRow(m_listWidget->count() - 1);
}

// ─────────────────────────────────────────────────────────────────
// 프리셋 삭제
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::onDeletePreset()
{
    int row = m_listWidget->currentRow();
    if (row < 0) return;
    if (PresetManager::instance().presets().size() <= 1) {
        QMessageBox::warning(this,
            QString::fromUtf8("경고"),
            QString::fromUtf8("마지막 프리셋은 삭제할 수 없습니다."));
        return;
    }
    QString name = m_listWidget->item(row)->text();
    auto ret = QMessageBox::question(this,
        QString::fromUtf8("삭제 확인"),
        QString::fromUtf8("'%1' 프리셋을 삭제하시겠습니까?").arg(name));
    if (ret != QMessageBox::Yes) return;

    PresetManager::instance().remove(name);
    refreshList();
    m_listWidget->setCurrentRow(qMin(row, m_listWidget->count() - 1));
}

// ─────────────────────────────────────────────────────────────────
// 저장 및 닫기
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::onSaveAndClose()
{
    // 현재 편집 중인 프리셋 저장
    if (m_listWidget->currentRow() >= 0 && m_edtName) {
        ScanPreset p = readPresetFromUI();
        if (p.name.isEmpty()) {
            QMessageBox::warning(this,
                QString::fromUtf8("경고"),
                QString::fromUtf8("프리셋 이름을 입력해주세요."));
            return;
        }
        // 이름이 변경된 경우: 기존 이름 삭제 후 추가
        const QString oldName = m_listWidget->currentItem()->text();
        if (oldName != p.name)
            PresetManager::instance().remove(oldName);
        PresetManager::instance().addOrUpdate(p);
        PresetManager::instance().setCurrentPreset(p.name);
    }

    PresetManager::instance().save();
    accept();
}

// ─────────────────────────────────────────────────────────────────
// 취소
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::onCancel()
{
    // 변경 사항을 파일에서 다시 로드해 롤백
    PresetManager::instance().load();
    reject();
}

// ─────────────────────────────────────────────────────────────────
// 폴더 선택 다이얼로그
// ─────────────────────────────────────────────────────────────────
void PresetEditDialog::onBrowseDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        QString::fromUtf8("저장 폴더 선택"),
        m_edtSaveDir->text().isEmpty() ? "C:/" : m_edtSaveDir->text());
    if (!dir.isEmpty())
        m_edtSaveDir->setText(dir);
}
