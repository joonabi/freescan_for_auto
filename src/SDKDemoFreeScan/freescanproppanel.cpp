#include "freescanproppanel.h"
#include "ui_freescanproppanel.h"
#include "qabstractbutton.h"
#include "qabstractslider.h"
#include "sn3dsdkpluginpub.h"
#include "qabstractslider.h"

FreeScanPropPanel::FreeScanPropPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FreeScanPropPanel)
{
    ui->setupUi(this);
    ui->widRulerFile->setVisible(false);
    // Change scan type
    connect(ui->radFramework, &QAbstractButton::clicked, [this](bool bChecked) {
        ui->widRulerFile->setVisible(false);
        if (bChecked)
            emit projectTypeChanged(SNSDK_PROJTYPE_FRAME);
    });
    connect(ui->radPhotographic, &QAbstractButton::clicked, [this](bool bChecked) {
        ui->widRulerFile->setVisible(true);
        if (bChecked)
            emit projectTypeChanged(SNSDK_PROJTYPE_PHOTOGRAPHIC);
    });
    connect(ui->radMesh, &QAbstractButton::clicked, [this](bool bChecked) {
        ui->widRulerFile->setVisible(false);
        if(bChecked)
            emit projectTypeChanged(SNSDK_PROJTYPE_POINTCLOUD);
    });
    // Change laser grade
    connect(ui->radGrade26, &QAbstractButton::clicked, [this](bool bChecked) {
        emit castLightTypeChaned(0);
    });
	connect(ui->radGrade7, &QAbstractButton::clicked, [this](bool bChecked) {
		emit castLightTypeChaned(1);
	});
    connect(ui->radGrade5, &QAbstractButton::clicked, [this](bool bChecked) {
        emit castLightTypeChaned(1);
    });
    connect(ui->radGrade1, &QAbstractButton::clicked, [this](bool bChecked) {
        emit castLightTypeChaned(2);
    });
    connect(ui->radGrade98, &QAbstractButton::clicked, [this](bool bChecked) {
        emit castLightTypeChaned(3);
    });
    // Scan object type changed
    connect(ui->radNormal, &QAbstractButton::clicked, [this](bool bChecked) {
        emit objectTypeChanged(0);
    });
    connect(ui->radReflect, &QAbstractButton::clicked, [this](bool bChecked) {
        emit objectTypeChanged(1);
    });
    connect(ui->radBlack, &QAbstractButton::clicked, [this](bool bChecked) {
        emit objectTypeChanged(2);
    });
    // Lightbrightness step changed
    connect(ui->sliderBright, &QAbstractSlider::valueChanged, [this](int iStep) {
        emit brightStepChanged(iStep);
    });
	//Light radio changed
	connect(ui->radLow, &QAbstractButton::clicked, [this]() {
		emit brightRadioChanged(0);
	});
	connect(ui->radMedium, &QAbstractButton::clicked, [this]() {
		emit brightRadioChanged(1);
	});
	connect(ui->radHight, &QAbstractButton::clicked, [this]() {
		emit brightRadioChanged(2);
	});
    connect(ui->btnOpenFramework, &QAbstractButton::clicked, [this](bool bChecked) {
        emit frameworkFileOpened("");
    });
    // Point distance step changed
    connect(ui->btnSetPointDist, &QAbstractButton::clicked, [this]() {
        auto iIndex = ui->sliderPointDist->value();
        emit pointDistChanged(iIndex);
    });
    connect(ui->chkHighSpeed, &QAbstractButton::clicked, [this](bool bChecked) {
        emit isHighSpeedChanged(bChecked);
    });
    connect(ui->chkStrongLight, &QAbstractButton::clicked, [this](bool bChecked) {
        emit isStrongLightChanged(bChecked);
    });
    connect(ui->chkQualityColor, &QAbstractButton::clicked, [this](bool bChecked) {
        emit isQualityColor(bChecked);
    });
    connect(ui->checkBox_deepholeMode, &QAbstractButton::clicked, [this](bool bChecked) {
        emit deepHoleModeChanged(bChecked);
    });
    connect(ui->checkBox_PointCoudMode, &QAbstractButton::clicked, [this](bool bChecked) {
        emit pointCloudModeChanged(bChecked);
    });
    connect(ui->chkIncFramework, &QAbstractButton::clicked, [this](bool bChecked) {
        emit isIncFrameChanged(bChecked);
    });
    connect(ui->radioButton_markers, &QAbstractButton::clicked, [this]() {
		emit alignModeChanged(0);
	});
    connect(ui->radioButton_features, &QAbstractButton::clicked, [this]() {
		emit alignModeChanged(1);
	});
    connect(ui->radioButton_hybrid, &QAbstractButton::clicked, [this]() {
		emit alignModeChanged(2);
	});
}

FreeScanPropPanel::~FreeScanPropPanel()
{
    delete ui;
}

void FreeScanPropPanel::on_widRulerFile_rulerFileListChanged(const QStringList& rulerFileList)
{
    emit rulerFileListChanged(rulerFileList);
}

void FreeScanPropPanel::setDeviceType(int deviceType)
{
	switch (deviceType)
	{
	case SNSDK_DEVTYPE_UEPRO:
		ui->radGrade5->show();
		ui->radGrade7->hide();
		ui->radBlack->show();
		break;
	case SNSDK_DEVTYPE_COMBO:
		ui->radPhotographic->hide();
		ui->radGrade5->hide();
		ui->radGrade7->show();
		ui->radBlack->hide();
		break;
	default:
		break;
	}
}


void FreeScanPropPanel::setProjectType(const QString& strType)
{
    bool bOK = false;
    int iType = strType.toInt(&bOK);
    // According to sn3DFileCommonDefine.h
    // int projectType = 1; /// 0 is pure marker mode, 1 is grid mode, 2 is append marker mode
    if (bOK) {
        if (0 == iType)
            ui->radMesh->setChecked(true);
        else if (1 == iType)
            ui->radFramework->setChecked(true);
        else if (2 == iType)
            ui->radMesh->setChecked(true);
    }
}

void FreeScanPropPanel::setPointDist(const QString& strDist)
{
    bool bOK = false;
    auto dDist = strDist.toDouble(&bOK);
    if (bOK) {
        for (int i = 0; i < m_listDist.size(); i++) {
            if (std::abs(m_listDist[i] - dDist) < .000001) {
                ui->sliderPointDist->setValue(i);
                ui->lblCurPointDist->setText(strDist);
                break;
            }
        }
    }
}

void FreeScanPropPanel::setLaserGrade(const QString& strGrade)
{
    bool bOK = false;
    int iGrade = strGrade.toInt(&bOK);
    if (bOK) {
        if (26 == iGrade)
            ui->radGrade26->setChecked(true);
        else if (7 == iGrade)
            ui->radGrade7->setChecked(true);
        else if (5 == iGrade)
            ui->radGrade5->setChecked(true);
        else if (1 == iGrade)
            ui->radGrade1->setChecked(true);
    }
}

void FreeScanPropPanel::setMaterial(const QString& strMaterial)
{
    bool bOK = false;
    int iMaterial = strMaterial.toInt(&bOK);
    if (bOK) {
        if (0 == iMaterial)
            ui->radNormal->setChecked(true);
        // 1 is NOT used by application
        else if (1 == iMaterial)
            ui->radReflect->setChecked(true);
        else if (2 == iMaterial)
            ui->radReflect->setChecked(true);
    }
}

void FreeScanPropPanel::setHighSeed(const QString& strHigh)
{
    bool bOK = false;
    int iHigh = strHigh.toInt(&bOK);
    if (bOK) {
        if (0 == iHigh)
            ui->chkHighSpeed->setChecked(false);
        else if (1 == iHigh)
            ui->chkHighSpeed->setChecked(true);
    }
}

void FreeScanPropPanel::setStrongLight(const QString& strStrong)
{
    bool bOK = false;
    int iStrong = strStrong.toInt(&bOK);
    if (bOK) {
        if (0 == iStrong)
            ui->chkStrongLight->setChecked(false);
        else if (1 == iStrong)
            ui->chkStrongLight->setChecked(true);
    }
}

void FreeScanPropPanel::setPointDistList(const QList<double>& listDist)
{
    m_listDist = listDist;
    std::sort(m_listDist.begin(), m_listDist.end());
    int iLength = m_listDist.length();
    if (iLength > 1) {
        int iMaxIdx = iLength - 1;
        auto dMin = m_listDist[0];
        ui->lblMinDist->setText(QString("%1").arg(dMin));
        auto dMax = m_listDist[iMaxIdx];
        ui->lblMaxDist->setText(QString("%1").arg(dMax));
        ui->sliderPointDist->setMinimum(0);
        ui->sliderPointDist->setMaximum(iMaxIdx);
    }
}

void FreeScanPropPanel::on_sliderPointDist_valueChanged(int iValue)
{
    if (m_listDist.length() <= 0)
        return;
    if (iValue >= m_listDist.length() || iValue < 0)
        return;
    auto dDist = m_listDist[iValue];
    ui->lblCurPointDist->setText(QString("%1").arg(dDist));
}

void FreeScanPropPanel::setBrightnessLevel(const QString& strLevel)
{
    bool bOK = false;
    int iLevel = strLevel.toInt(&bOK);
    if (bOK) {
        ui->sliderBright->setValue(iLevel);
    }
}

void FreeScanSlider::mousePressEvent(QMouseEvent *ev)
{
	int CurrentX = ev->pos().x();
	double per = CurrentX * 1.0 / this->width();
	int value = per * (this->maximum() - this->minimum())+this->minimum();
	this->setValue(value);
	QSlider::mousePressEvent(ev);
}
