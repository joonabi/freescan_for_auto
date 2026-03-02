
#include "sn3dsdkmesh.h"
#include "ui_sn3dsdkmesh.h"
#include "SDKWrapperBase.h"
#include "QFileDialog"
Sn3DSDKMesh::Sn3DSDKMesh(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Sn3DSDKMesh)
	, sdkWrapper_(new SDKWrapperBase)
{
    ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	connect(ui->rbtnNone, &QRadioButton::clicked, [this]() {this->m_filterType = FILTER_TYPE::NONE; });
	connect(ui->rbtnLow, &QRadioButton::clicked, [this]() {this->m_filterType = FILTER_TYPE::LOW; });
	connect(ui->rbtnMed, &QRadioButton::clicked, [this]() {this->m_filterType = FILTER_TYPE::MED; });
	connect(ui->rbtnHigh, &QRadioButton::clicked, [this]() {this->m_filterType = FILTER_TYPE::HIGH; });
}

Sn3DSDKMesh::~Sn3DSDKMesh()
{
    delete ui;
}

void Sn3DSDKMesh::on_btnUndo_clicked(bool bChecked)
{
	QVariantMap mapParam = {
		{"cmd", "undoMesh"},
	};
	emit messageReady(mapParam);
}

void Sn3DSDKMesh::on_btnConfirm_clicked(bool bChecked)
{
	QVariantMap mapParam = {
		{"cmd", "confirmMesh"},
	};
	emit messageReady(mapParam);
}

void Sn3DSDKMesh::on_btnSaveMesh_clicked(bool bChecked)
{
	QFileDialog dlg;
	auto strDir = dlg.getSaveFileName(this, tr("Select save  folder"), qApp->applicationDirPath(), tr("Model (*.stl)"));
	if (strDir.isEmpty()) return;

	QString fileName("");
	QString fileDir("");
	int lastIndex = strDir.lastIndexOf("/");
	if (lastIndex != -1) {
		fileDir = strDir.left(lastIndex);
		fileName = strDir.right(strDir.length() - lastIndex - 1);
	}
	fileName.remove(".stl");
	if (!fileDir.endsWith("/")) fileDir += "/";
	if (fileName.isEmpty() || fileDir.isEmpty()) return;

	auto strInfo = QStringLiteral("Begin 保存封装数据");
	//this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
	sigPrintInfo(strInfo);
	QString strResult;
	QVariantMap mapParam = {
		//{"controller", "$$LaserMeshPost"},
		{"fileName",fileName},//保存文件名
		{"folderPath",fileDir},//保存文件所在文件夹路径
		{"format","001000000"},//文件类型(可自定义，需和下面asc等标志位一致)
		{"method", "save"},
		{"postpageVisible", 1},
		{"saveAscFile", 0},//format[0]
		{"saveStlFile", 1},//format[2]
		{"saveObjFile", 0},//format[3]
		{"savePlyFile", 0},//format[4]
		{"saveP3File", 0}, //format[5]
		{"save3MfFile", 0},//format[6]
		{"saveTxtFile", 0},//format[7]
		{"saveCsvFile", 0}//,//format[8]
	};
	//m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
	emit messageReady(mapParam);
}

void Sn3DSDKMesh::on_btnApply_clicked(bool bChecked /*= false*/)
{
	QVariantMap mapParam = {
		{"cmd", "apply"},
		{"method", "apply"},
        {"watertightType",(int)this->m_watertightType},
		{"fileter_level", (int)this->m_filterType},
        {"smooth_level",(int)this->m_smoothType},
		{"remove_small_floating_parts", ui->sBoxRSFParts->value()},
		{"max_trianlges_ischeck", ui->cBoxMTriangle->isChecked() ? true : false},
		{"max_trianlges_value", ui->cBoxMTriangle->isChecked() ? ui->sBoxMTriangle->value() : -1},
        {"use_border_opt",ui->checkBox_edgeOptimzation->isChecked()},
		{"fill_small_hole_ischeck", ui->groupBox_fillSmallHole->isChecked()},
		{"fill_small_hole_value", ui->groupBox_fillSmallHole->isChecked() ? ui->sBoxSHole->value() : -1},
		{"neighborhood_perimeter",ui->spinBox_neighboringAreas->value()},
        {"remove_spike_ischeck", ui->cBoxRSpike->isChecked() ? true : false},
		{"marker_hole_filling_ischeck", ui->cBoxMHFilling->isChecked() ? true : false}
	};
    emit messageReady(mapParam);
	/*QString strResult;
	sdkWrapper_->SnSendMessage("v2.0", "invoke", "MeshGenerate", mapParam, strResult);*/
}


void Sn3DSDKMesh::on_radioButton_unwatertight_clicked()
{
    m_watertightType=WATERTIGHT_TYPE::UNWATERTIGHT;
}


void Sn3DSDKMesh::on_radioButton_halfwatertight_clicked()
{
    m_watertightType=WATERTIGHT_TYPE::HALFWATERTIGHT;
}


void Sn3DSDKMesh::on_radioButton_watertight_clicked()
{
    m_watertightType=WATERTIGHT_TYPE::WATERTIGHT;
}


void Sn3DSDKMesh::on_radioButton_smoothLow_clicked()
{
    m_smoothType=SMOOTH_TYPE::LOW;
}


void Sn3DSDKMesh::on_radioButton_smoothMed_clicked()
{
    m_smoothType=SMOOTH_TYPE::MED;
}


void Sn3DSDKMesh::on_radioButton_smoothHigh_clicked()
{
    m_smoothType=SMOOTH_TYPE::HIGH;
}



