#include "optimscantabwidget.h"
#include "ui_optimscantabwidget.h"
#include <QFileDialog>

OptimScanTabWidget::OptimScanTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptimScanTabWidget)
{
    ui->setupUi(this);
    initUI();
}

OptimScanTabWidget::~OptimScanTabWidget()
{
    delete ui;
}

void OptimScanTabWidget::initUI()
{
    ui->widget->setVisible(false);
    ui->widget_2->setVisible(false);
}

void OptimScanTabWidget::getScanSettingData(LPScanPars& param)
{
	param->scanPointCloud = ui->scanPointCloudRadio->isChecked();
	param->scanMarkers = ui->scanMarkersRadio->isChecked();
	param->addGlobalMarkers = ui->addGlobalMarkersRadio->isChecked();
	param->alignMod = new char[ui->alignMode->currentText().size() + 1];
	strcpy(const_cast<char*>(param->alignMod), ui->alignMode->currentText().toUtf8().data());
	//param->alignMod = ui->alignMode->currentText().toUtf8().constData();
	param->HDR = ui->hdrRadio->isChecked();
	param->turtableChecked = ui->turntablemodeRadio->isChecked();
	param->turntableAlignMod = "";
	param->turntableSteps = ui->turntableStepsEdit->text().toInt();
	param->turntableSpeed = ui->turntableSpeedNumEdit->text().toInt();
	param->isRoundChecked = ui->roundRadio->isChecked();
	param->useExistVerifyData = ui->verifyDataCheckbox->isChecked();
	param->openScanEdit = false;
}

//选择拼接模式
void OptimScanTabWidget::on_alignMode_currentTextChanged(const QString &alignModText)
{
    if("framePoint" == alignModText)
        ui->widget->setVisible(true);
    else
        ui->widget->setVisible(false);
}

//选择使用转台
void OptimScanTabWidget::on_turntablemodeRadio_toggled(bool checked)
{
    ui->widget_2->setVisible(checked);
}


void OptimScanTabWidget::on_openGmarkerBtn_pressed()
{
    QString slnPath = QFileDialog::getOpenFileName(this, "open", QDir::homePath(), "*.p3 *.txt *.asc");
    if (!slnPath.isEmpty()) {
        emit importFrameworkMakers(slnPath);
    }
}

