#include "sn3dsdkfreescanmainview.h"
#include "ui_sn3dsdkfreescanmainview.h"
#include "qstring.h"
#include "qthread.h"
#include "qstringlist.h"
#include "qdialog.h"
#include "qfiledialog.h"
#include "qcombobox.h"
#include "glog/logging.h"
#include "sn3dsdkmesh.h"
#include "calibrationframe.h"
#include "sn3dsdkdemoapp.h"
#include "freescanproppanel.h"
#include "choosemodepage.h"
#include "calibrator.h"
#include "QFile"
#include "QTextStream"
#include<QVector3D>
#include "sdkmodbuswrapper.h"

static QString CMD_STR = "cmd";
static QString SCANMODE_STR = "scanMode";
static int isPackaged = false;
void CALLBACK SDKWrapperVideoCallback(int camId, const unsigned char * data, int width, int height, int channel, void* pUserData)
{
	Sn3DSDKFreeScanMainView* pWrapper = (Sn3DSDKFreeScanMainView*)pUserData;
	if (pWrapper)
		pWrapper->videoCallBack(camId, data, width, height, channel);
}

void CALLBACK SDKWrapperSceneDataCallBack(void* pUserData) {
	Sn3DSDKFreeScanMainView* pWrapper = (Sn3DSDKFreeScanMainView*)pUserData;
	if (pWrapper) {
		pWrapper->sceneDataCallBack();
	}
}


Sn3DSDKFreeScanMainView::Sn3DSDKFreeScanMainView(QWidget *parent)
    : Sn3DSDKMainViewBase(parent)
    , ui(new Ui::Sn3DSDKFreeScanMainView)
	, m_demoApp(static_cast<Sn3DSDKDemoApp *>qApp)
    , m_pPostProcessForm(new FreeScanPostProcessForm(this))
{
    ui->setupUi(this);
    ui->stackPage->setCurrentIndex(0);
    ui->chkRecvMsg->setChecked(true);
	this->ui->lblProgressPrompt->setVisible(false);
	this->ui->progressPercent->setVisible(false);
    loadScanSoftSetting();
	save_config = new QSettings("./scansoftwaresdk.ini", QSettings::IniFormat);
	m_bShowCurrentRT = save_config->value("showCurrentRT", false).toBool();
    connect(ui->btnScannerTab, &QPushButton::clicked, [this]() {
        this->ui->stackPage->setCurrentIndex(0);
    });
    connect(ui->btnCalibrateTab, &QPushButton::clicked, [this]() {
        ui->stackPage->setCurrentIndex(1);
    });
    connect(ui->btnScanTab, &QPushButton::clicked, [this]() {
        ui->stackPage->setCurrentIndex(2);
        auto strInfo = QStringLiteral("switch to scan");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        if (m_scanStep.hasEnterScan())
            m_demoApp->Sn3DEnterScanModePage();
        else
            m_demoApp->Sn3DEnterScanPage();
        // original cmd interface
        /*QString strResult;
        QString strCmd = "enterScanPage2";
        if(m_scanStep.hasEnterScan())
            strCmd = "enterScanModePage2";
        QVariantMap mapParam = { {"cmd", strCmd}};
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);*/
    });

    connect(m_demoApp, &Sn3DSDKDemoApp::SnMessageReceived, this, &Sn3DSDKFreeScanMainView::onSnMessageReceived, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::deviceStatusChanged, this, [this](LPDeviceStatus status) { // sn3dsdk_c_wrapper::DeviceStatus
        auto strInfo = QStringLiteral("device online");
        if(SNSDK_DEVSTAT_OFFLINE == status->offline)
            strInfo = QStringLiteral("device offline");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::newProjFinished, this, [this](QStringList projNameList){
        auto strInfo = QStringLiteral("End create new projects group,name:").trimmed() + projNameList.join(QStringLiteral(","));
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        showScannerPropPanel(true);
    }, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::saveScanDataReturn, this, [this]() {
        auto strInfo = QStringLiteral("End save scan");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        this->ui->lblProgressPrompt->setText(QStringLiteral("Loading..."));
        this->ui->progressPercent->setValue(0);
    }, Qt::QueuedConnection);
	connect(m_demoApp, &Sn3DSDKDemoApp::meshDataReturn, this, [this](bool success) {
		auto strInfo = QStringLiteral("End mesh %1").arg(success? QStringLiteral("success"): QStringLiteral("failed"));
		this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
		this->ui->lblProgressPrompt->setText(QStringLiteral("Loading..."));
		this->ui->progressPercent->setValue(0);
	}, Qt::QueuedConnection);
	connect(m_demoApp, &Sn3DSDKDemoApp::saveMeshDataReturn, this, [this](bool success) {
		auto strInfo = QStringLiteral("End save mesh%1").arg(success ? QStringLiteral("sucess") : QStringLiteral("failed"));
		this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
		this->ui->lblProgressPrompt->setText(QStringLiteral("Loading..."));
		this->ui->progressPercent->setValue(0);
	}, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::updateStateWhenStopFusion, this, [this]() {
        auto strInfo = QStringLiteral("End pause scan");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::showProgress, this, [this](QString strTitle, double dValue, bool bVisible) {
		this->ui->lblProgressPrompt->setVisible(bVisible);
		this->ui->progressPercent->setVisible(bVisible);
        this->ui->lblProgressPrompt->setText(strTitle);
        int iValue = dValue * 100;
        this->ui->progressPercent->setValue(iValue);
		if (dValue == 1.0f) {
			if (strTitle=="Project Loading") {
				auto strInfo = QStringLiteral("End load projects group");
				this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
				showScannerPropPanel(true);
			}
		}
    }, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::initCalibParamFinished, this, [this]() {
		//m_demoApp->SnEnterCalib();
        // m_pCalibFrame->show(); DON'T restore this
        auto strInfo = QStringLiteral("End switch to calibration");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::calibHeightOKChanged, this, [this](int iOKHeight) {
        auto strInfo = QStringLiteral("height %1 sucess");
        strInfo = strInfo.arg(iOKHeight);
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::groupIndexChanged, this, [this](int index) {
        auto strInfo = QStringLiteral("switch to group %1");
        strInfo = strInfo.arg(index);
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::showMaximized, this, [this]() {
        loadScanSoftSetting();
        auto strInfo = QStringLiteral("SDK environment initialized ");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }, Qt::QueuedConnection);

	connect(this, &Sn3DSDKFreeScanMainView::sigGetVideoData, this, [this](int camId, const unsigned char * data, int width, int height, int channel) {
		QImage image(data, width, height, QImage::Format_BGR888);
		int widgetWidth;
		int widgetHeight;
		switch (camId)
		{
		case 0:
			widgetWidth = ui->leftCameraWidget->width();
			widgetHeight = ui->leftCameraWidget->height();
			ui->leftCameraWidget->updateImage(QPixmap::fromImage(image).scaled(widgetWidth, widgetHeight));
			break;
		case 1:
			widgetWidth = ui->rightCameraWidget->width();
			widgetHeight = ui->rightCameraWidget->height();
			ui->rightCameraWidget->updateImage(QPixmap::fromImage(image).scaled(widgetWidth, widgetHeight));
			break;
        case 2:
			widgetWidth = ui->textureCameraWidget->width();
			widgetHeight = ui->textureCameraWidget->height();
			ui->textureCameraWidget->updateImage(QPixmap::fromImage(image).scaled(widgetWidth, widgetHeight));
			break;
		default:
			break;
		}
	},Qt::AutoConnection);

    // Post process
    connect(m_pPostProcessForm, &FreeScanPostProcessForm::applied, this, &Sn3DSDKFreeScanMainView::applyPostProcess);
    connect(m_pPostProcessForm, &FreeScanPostProcessForm::confirmed, this, &Sn3DSDKFreeScanMainView::confirmPostProcess);
    connect(m_pPostProcessForm, &FreeScanPostProcessForm::canceled, this, &Sn3DSDKFreeScanMainView::cancelPostProcess);
    connect(m_pPostProcessForm, &FreeScanPostProcessForm::rollbacked, this, &Sn3DSDKFreeScanMainView::rollbackPostProcess);
    connect(m_demoApp, &Sn3DSDKDemoApp::postProcessApplied, this, &Sn3DSDKFreeScanMainView::onPostProcessApplied, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::postProcessConfirmed, this, &Sn3DSDKFreeScanMainView::onPostProcessConfirmed, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::postProcessCanceled, this, &Sn3DSDKFreeScanMainView::onPostProcessCanceled, Qt::QueuedConnection);
    connect(m_demoApp, &Sn3DSDKDemoApp::postProcessRollbacked, this, &Sn3DSDKFreeScanMainView::onPostProcessRollbacked, Qt::QueuedConnection);
    // End post process

}

Sn3DSDKFreeScanMainView::~Sn3DSDKFreeScanMainView()
{
    delete ui;
}

void Sn3DSDKFreeScanMainView::on_btnCheckDevice_clicked(bool bChecked)
{
}

void Sn3DSDKFreeScanMainView::closeEvent(QCloseEvent *event)
{
    on_btnCloseService_clicked();
    event->accept();
}

void Sn3DSDKFreeScanMainView::videoCallBack(int camId, const unsigned char * data, int width, int height, int channel)
{
	emit sigGetVideoData(camId, data, width, height, channel);
}
static int cnt = 0;
static void saveSceneData(const SN3D_SCENE_DATA& sceneData,QSettings* qset)
{
	QString filename= qset->value("freescan_save/save_path").toString();
	QDir folderDir(QDir::currentPath());
	if (!folderDir.exists(filename))folderDir.mkdir(filename);
	filename = QString(QDir::currentPath()+ "/" +filename + "/");
	bool save_CurrentMarkers = qset->value("freescan_save/save_CurrentMarkers").toBool();
	bool save_WholeMarkers = qset->value("freescan_save/save_WholeMarkers").toBool();
	bool save_CurrentCloud = qset->value("freescan_save/save_CurrentCloud").toBool();
	bool save_WholeCloud = qset->value("freescan_save/save_WholeCloud").toBool();
	bool save_rt = qset->value("freescan_save/save_rt").toBool();
	bool save_stl = qset->value("freescan_save/save_mesh_stl").toBool();
	bool save_increase_stl = qset->value("freescan_save/save_increase_stl").toBool();
	//filename=
	//
	//if()
	//保存当前标志点
	if (save_CurrentMarkers&&sceneData.nCurrentMarkersCount)
	{
		QFile file(QString(filename + "CurrentMarkers.asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		for (int i = 0; i < sceneData.nCurrentMarkersCount; ++i)
		{
			out << sceneData.pCurrentMarkers[i].mCoords.nX << "\t"
				<< sceneData.pCurrentMarkers[i].mCoords.nY << "\t"
				<< sceneData.pCurrentMarkers[i].mCoords.nZ << "\t"
				<< sceneData.pCurrentMarkers[i].mNormals.nX << "\t"
				<< sceneData.pCurrentMarkers[i].mNormals.nY << "\t"
				<< sceneData.pCurrentMarkers[i].mNormals.nZ << "\t"
				<< "\n";
		}
		file.close();
	}
	//保存整体标志点
	
	if (save_WholeMarkers&&sceneData.nWholeMarkersCount)
	{
		QFile file(QString(filename + "WholeMarkers.asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		for (int i = 0; i < sceneData.nWholeMarkersCount; ++i)
		{
			out << sceneData.pWholeMarkers[i].mCoords.nX << "\t"
				<< sceneData.pWholeMarkers[i].mCoords.nY << "\t"
				<< sceneData.pWholeMarkers[i].mCoords.nZ << "\t"
				<< sceneData.pWholeMarkers[i].mNormals.nX << "\t"
				<< sceneData.pWholeMarkers[i].mNormals.nY << "\t"
				<< sceneData.pWholeMarkers[i].mNormals.nZ << "\t"
				<< "\n";
		}

	}
	//保存当前点云
	
	if (save_CurrentCloud&&sceneData.nCurrentCloudCount)
	{
		QFile file(QString(filename + "CurrentCloud.asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream out(&file);
		for (int i = 0; i < sceneData.nCurrentCloudCount; ++i)
		{
			out << sceneData.pCurrentCloud.pPointCoords[i].nX << "\t"
				<< sceneData.pCurrentCloud.pPointCoords[i].nY << "\t"
				<< sceneData.pCurrentCloud.pPointCoords[i].nZ << "\t"
				<< sceneData.pCurrentCloud.pPointNormals[i].nX << "\t"
				<< sceneData.pCurrentCloud.pPointNormals[i].nY << "\t"
				<< sceneData.pCurrentCloud.pPointNormals[i].nZ << "\t"
				<< sceneData.pCurrentCloud.pPointColors[i].nX << "\t"
				<< sceneData.pCurrentCloud.pPointColors[i].nY << "\t"
				<< sceneData.pCurrentCloud.pPointColors[i].nZ << "\t"
				<< "\n";
		}
		file.close();
	}
	//保存整体点云
	
	if (save_WholeCloud&&sceneData.nWholeCloudCount)
	{
		QFile file(QString(filename + "WholeCloud.asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream out(&file);
		for (int i = 0; i < sceneData.nWholeCloudCount; ++i)
		{
			out << sceneData.pWholeCloud.pPointCoords[i].nX << "\t"
				<< sceneData.pWholeCloud.pPointCoords[i].nY << "\t"
				<< sceneData.pWholeCloud.pPointCoords[i].nZ << "\t"
				<< sceneData.pWholeCloud.pPointNormals[i].nX << "\t"
				<< sceneData.pWholeCloud.pPointNormals[i].nY << "\t"
				<< sceneData.pWholeCloud.pPointNormals[i].nZ << "\t"
				<< sceneData.pWholeCloud.pPointColors[i].nX << "\t"
				<< sceneData.pWholeCloud.pPointColors[i].nY << "\t"
				<< sceneData.pWholeCloud.pPointColors[i].nZ << "\t"
				<< "\n";
		}
		file.close();
	}
	//保存全部RT,需要把之前的RT.asc删掉
	

	if (save_rt)
	{
		QFile file(QString(filename + "RT.asc"));
		if (file.exists())
		{
			if (!file.open(QIODevice::Append | QIODevice::Text))
				return;
		}
		else
		{
			if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
				return;
		}
		QTextStream out(&file);
		auto rt = sceneData.aCurrentCloudRt;
		out << rt.aRot[0] << "\t"
			<< rt.aRot[1] << "\t"
			<< rt.aRot[2] << "\t"
			<< rt.aRot[3] << "\t"
			<< rt.aRot[4] << "\t"
			<< rt.aRot[5] << "\t"
			<< rt.aRot[6] << "\t"
			<< rt.aRot[7] << "\t"
			<< rt.aRot[8] << "\t"
			<< rt.aTrans[0] << "\t"
			<< rt.aTrans[1] << "\t"
			<< rt.aTrans[2] << "\t"
			<< "\n";

		file.close();
	}
	//保存整体点云
	if (save_WholeCloud&&sceneData.nWholeCloudCount)
	{
		QFile file(QString(filename + "pWholeCloud.asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;

		QTextStream out(&file);
		for (int i = 0; i < sceneData.nWholeCloudCount; ++i)
		{
			out << sceneData.pWholeCloud.pPointCoords[i].nX << "\t"
				<< sceneData.pWholeCloud.pPointCoords[i].nY << "\t"
				<< sceneData.pWholeCloud.pPointCoords[i].nZ << "\t"
				<< sceneData.pWholeCloud.pPointNormals[i].nX << "\t"
				<< sceneData.pWholeCloud.pPointNormals[i].nY << "\t"
				<< sceneData.pWholeCloud.pPointNormals[i].nZ << "\t"
				<< sceneData.pWholeCloud.pPointColors[i].nX << "\t"
				<< sceneData.pWholeCloud.pPointColors[i].nY << "\t"
				<< sceneData.pWholeCloud.pPointColors[i].nZ << "\t"
				<< "\n";
		}
		file.close();
	}
	//保存网格数据
	
	if (save_stl&&sceneData.mMesh.nMeshpointCount&&sceneData.mMesh.nMeshNormalCount&&sceneData.mMesh.nMeshTriFaceIdCount)
	{
		QString fn;
		if (isPackaged)fn = filename + "PackagedMesh.stl";
		else fn = filename + "Mesh.stl";
		QFile file(fn);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		out << "solid object\n";
		for (int i = 0; i < sceneData.mMesh.nMeshTriFaceIdCount; i++)
		{
			QVector3D normalA(sceneData.mMesh.pMeshNormal[sceneData.mMesh.pMeshTriFaceId[i].x].nX,
				sceneData.mMesh.pMeshNormal[sceneData.mMesh.pMeshTriFaceId[i].x].nY,
				sceneData.mMesh.pMeshNormal[sceneData.mMesh.pMeshTriFaceId[i].x].nZ);
			QVector3D normalB(sceneData.mMesh.pMeshNormal[sceneData.mMesh.pMeshTriFaceId[i].y].nX,
				sceneData.mMesh.pMeshNormal[sceneData.mMesh.pMeshTriFaceId[i].y].nY,
				sceneData.mMesh.pMeshNormal[sceneData.mMesh.pMeshTriFaceId[i].y].nZ);
			QVector3D normalC(sceneData.mMesh.pMeshNormal[sceneData.mMesh.pMeshTriFaceId[i].z].nX,
				sceneData.mMesh.pMeshNormal[sceneData.mMesh.pMeshTriFaceId[i].z].nY,
				sceneData.mMesh.pMeshNormal[sceneData.mMesh.pMeshTriFaceId[i].z].nZ);
			QVector3D normalAB = normalA - normalB;
			QVector3D normalAC = normalA - normalC;
			QVector3D normaln = QVector3D::crossProduct(normalAB, normalAC);
			normaln.normalize();
			out << "facet normal"<<"\t"
				<< normaln[0] << "\t"
				<< normaln[1] << "\t"
				<< normaln[2] << "\t"
				<< "\n";
			out << "\t\touter loop\n";
			out << "\t\t\t\t" << "vertex" << "\t"
				<< sceneData.mMesh.pMeshPoint[sceneData.mMesh.pMeshTriFaceId[i].x].nX << "\t"
				<< sceneData.mMesh.pMeshPoint[sceneData.mMesh.pMeshTriFaceId[i].x].nY << "\t"
				<< sceneData.mMesh.pMeshPoint[sceneData.mMesh.pMeshTriFaceId[i].x].nZ << "\t"
				<< "\n"
				<< "\t\t\t\t" << "vertex" << "\t"
				<< sceneData.mMesh.pMeshPoint[sceneData.mMesh.pMeshTriFaceId[i].y].nX << "\t"
				<< sceneData.mMesh.pMeshPoint[sceneData.mMesh.pMeshTriFaceId[i].y].nY << "\t"
				<< sceneData.mMesh.pMeshPoint[sceneData.mMesh.pMeshTriFaceId[i].y].nZ << "\t"
				<< "\n"
				<< "\t\t\t\t" << "vertex" << "\t"
				<< sceneData.mMesh.pMeshPoint[sceneData.mMesh.pMeshTriFaceId[i].z].nX << "\t"
				<< sceneData.mMesh.pMeshPoint[sceneData.mMesh.pMeshTriFaceId[i].z].nY << "\t"
				<< sceneData.mMesh.pMeshPoint[sceneData.mMesh.pMeshTriFaceId[i].z].nZ << "\t"
				<< "\n";
			out << "\t\tendloop\n";
			out << "endfacet\n";
		}
		out << "endsolid object";
	}

	bool save_increase_asc = false;
	cnt++;
	if (cnt % 10!=0 )return;
	if (cnt > 99999999)cnt = 0;
	if (save_increase_stl&&sceneData.mIncreaseMesh.nVertexCount&& sceneData.mIncreaseMesh.nMeshTriFaceIdCount) {
		QFile file(QString(filename +QString::number(cnt/10) +"increaseMesh.stl"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		out << "solid object\n";
		for (int i = 0; i < sceneData.mIncreaseMesh.nVertexCount; i=i+3)
		{
			//int face1 = sceneData.mIncreaseMesh.pMeshTriFaceId[i].x-1;
			//int face2= sceneData.mIncreaseMesh.pMeshTriFaceId[i].y-1;
			//int face3= sceneData.mIncreaseMesh.pMeshTriFaceId[i].z-1;
			//if (face1 >= sceneData.mIncreaseMesh.nVertexCount ||
			//	face2 >= sceneData.mIncreaseMesh.nVertexCount ||
			//	face3 >= sceneData.mIncreaseMesh.nVertexCount)continue;
			auto& vect = sceneData.mIncreaseMesh.pVertex;

			QVector3D vectA(vect[i].nX, vect[i].nY, vect[i].nZ);
			QVector3D vectB(vect[i+1].nX, vect[i + 1].nY, vect[i + 1].nZ);
			QVector3D vectC(vect[i+2].nX, vect[i + 2].nY, vect[i + 2].nZ);
			QVector3D vectAB = vectA - vectB;
			QVector3D vectAC = vectA - vectC;
			QVector3D vectn = QVector3D::crossProduct(vectAB, vectAC);
			vectn.normalize();
			out << "facet normal" << "\t"
				<< vectn[0] << "\t"
				<< vectn[1] << "\t"
				<< vectn[2] << "\t"
				<< "\n";
			out << "\t\touter loop\n";
			out << "\t\t\t\t" << "vertex" << "\t" << vectA[0] << "\t" << vectA[1] << "\t" << vectA[2] << "\t" << "\n"
				<< "\t\t\t\t" << "vertex" << "\t" << vectB[0] << "\t" << vectB[1] << "\t" << vectB[2] << "\t" << "\n"
				<< "\t\t\t\t" << "vertex" << "\t" << vectC[0] << "\t" << vectC[1] << "\t" << vectC[2] << "\t" << "\n";
			out << "\t\tendloop\n";
			out << "endfacet\n";
		}
		out << "endsolid object";
	}
	/*if (save_increase_asc&&sceneData.mIncreaseMesh.nVertexCount&& sceneData.mIncreaseMesh.nMeshTriFaceIdCount)
	{
		QFile file(QString(filename + QString::number(cnt)+ "increaseAsc.asc"));
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
			return;
		QTextStream out(&file);
		for (int i = 0; i < sceneData.mIncreaseMesh.nVertexCount; ++i)
		{
			auto& vect = sceneData.mIncreaseMesh.pVertex;
			out << vect[i].nX << "\t"
				<< vect[i].nY << "\t"
				<< vect[i].nZ << "\t"
				<< "\n";
		}
		file.close();
	}*/
}
void Sn3DSDKFreeScanMainView::sceneDataCallBack()
{
	VLOG(google::GLOG_INFO) << __FUNCTION__ << " get SceneData start";
	//if (!m_sceneDataProcessFlag.load()) return;
	SN3D_SCENE_DATA sceneData;
	memset(&sceneData, 0, sizeof(SN3D_SCENE_DATA));
	m_demoApp->getSceneData(&sceneData);

	std::shared_ptr<SN3D_MARKER> wholeMarker;
	if (sceneData.nWholeMarkersCount != 0) {
		wholeMarker = std::shared_ptr<SN3D_MARKER>(new SN3D_MARKER[sceneData.nWholeMarkersCount], std::default_delete<SN3D_MARKER[]>());
	}
	sceneData.pWholeMarkers = wholeMarker.get();

	std::shared_ptr<SN3D_MARKER> currentMarker;
	if (sceneData.nCurrentMarkersCount != 0) {
		currentMarker = std::shared_ptr<SN3D_MARKER>(new SN3D_MARKER[sceneData.nCurrentMarkersCount], std::default_delete<SN3D_MARKER[]>());
	}
	sceneData.pCurrentMarkers = currentMarker.get();

	std::shared_ptr<SN3D_POINT_DATA> curCloudCoords;
	std::shared_ptr<SN3D_POINT_DATA> curCloudNormals;
	std::shared_ptr<SN3D_POINT_DATA> curCloudColors;
	if (sceneData.nCurrentCloudCount != 0) {
		curCloudCoords = std::shared_ptr<SN3D_POINT_DATA>(new SN3D_POINT_DATA[sceneData.nCurrentCloudCount], std::default_delete<SN3D_POINT_DATA[]>());
		curCloudNormals = std::shared_ptr<SN3D_POINT_DATA>(new SN3D_POINT_DATA[sceneData.nCurrentCloudCount], std::default_delete<SN3D_POINT_DATA[]>());
		curCloudColors = std::shared_ptr<SN3D_POINT_DATA>(new SN3D_POINT_DATA[sceneData.nCurrentCloudCount], std::default_delete<SN3D_POINT_DATA[]>());
	}
	sceneData.pCurrentCloud.pPointCoords = curCloudCoords.get();
	sceneData.pCurrentCloud.pPointNormals = curCloudNormals.get();
	sceneData.pCurrentCloud.pPointColors = curCloudColors.get();

	std::shared_ptr<SN3D_POINT_DATA> wholeCloudCoords;
	std::shared_ptr<SN3D_POINT_DATA> wholeCloudNormals;
	std::shared_ptr<SN3D_POINT_DATA> wholeCloudColors;
	if (sceneData.nWholeCloudCount != 0) {
		wholeCloudCoords = std::shared_ptr<SN3D_POINT_DATA>(new SN3D_POINT_DATA[sceneData.nWholeCloudCount], std::default_delete<SN3D_POINT_DATA[]>());
		wholeCloudNormals = std::shared_ptr<SN3D_POINT_DATA>(new SN3D_POINT_DATA[sceneData.nWholeCloudCount], std::default_delete<SN3D_POINT_DATA[]>());
		wholeCloudColors = std::shared_ptr<SN3D_POINT_DATA>(new SN3D_POINT_DATA[sceneData.nWholeCloudCount], std::default_delete<SN3D_POINT_DATA[]>());
	}
	sceneData.pWholeCloud.pPointCoords = wholeCloudCoords.get();
	sceneData.pWholeCloud.pPointNormals = wholeCloudNormals.get();
	sceneData.pWholeCloud.pPointColors = wholeCloudColors.get();


	std::shared_ptr<SN3D_POINT_DATA>pMeshPoint;
	if (sceneData.mMesh.nMeshpointCount != 0) {
		pMeshPoint = std::shared_ptr <SN3D_POINT_DATA>(new SN3D_POINT_DATA[sceneData.mMesh.nMeshpointCount], std::default_delete<SN3D_POINT_DATA[]>());
	}
	sceneData.mMesh.pMeshPoint = pMeshPoint.get();

	std::shared_ptr<SN3D_POINT_DATA>pMeshNormal;
	if (sceneData.mMesh.nMeshNormalCount != 0) {
		pMeshNormal = std::shared_ptr <SN3D_POINT_DATA>(new SN3D_POINT_DATA[sceneData.mMesh.nMeshNormalCount], std::default_delete<SN3D_POINT_DATA[]>());
	}
	sceneData.mMesh.pMeshNormal = pMeshNormal.get();

	std::shared_ptr<SN3D_TRI_FACE>pMeshTriFaceId;
	if (sceneData.mMesh.nMeshTriFaceIdCount != 0) {
		pMeshTriFaceId = std::shared_ptr <SN3D_TRI_FACE>(new SN3D_TRI_FACE[sceneData.mMesh.nMeshTriFaceIdCount], std::default_delete<SN3D_TRI_FACE[]>());
	}
	sceneData.mMesh.pMeshTriFaceId = pMeshTriFaceId.get();

	std::shared_ptr<SN3D_TRI_FACE>pMeshTextureId;
	if (sceneData.mMesh.nMeshTextureIdCount != 0)
	{
		pMeshTextureId = std::shared_ptr <SN3D_TRI_FACE>(new SN3D_TRI_FACE[sceneData.mMesh.nMeshTextureIdCount], std::default_delete<SN3D_TRI_FACE[]>());
	}
	sceneData.mMesh.pMeshTextureId = pMeshTextureId.get();

	std::shared_ptr<SN3D_UVCOORD>pTextureUV;
	if (sceneData.mMesh.nTextureUVCount != 0)
	{
		pTextureUV = std::shared_ptr <SN3D_UVCOORD>(new SN3D_UVCOORD[sceneData.mMesh.nTextureUVCount], std::default_delete<SN3D_UVCOORD[]>());
	}
	sceneData.mMesh.pTextureUV = pTextureUV.get();

	std::shared_ptr<unsigned char>mImage;
	size_t imageSize = sceneData.mMesh.mImage.width*sceneData.mMesh.mImage.height*sceneData.mMesh.mImage.channel;
	if (imageSize)
	{
		mImage = std::shared_ptr <unsigned char>(new unsigned char[imageSize], std::default_delete<unsigned char[]>());
	}
	sceneData.mMesh.mImage.data = mImage.get();

	std::shared_ptr<SN3D_POINT_DATA> increaseMeshVertex;
	if (sceneData.mIncreaseMesh.nVertexCount != 0) {
		increaseMeshVertex = std::shared_ptr<SN3D_POINT_DATA>(new SN3D_POINT_DATA[sceneData.mIncreaseMesh.nVertexCount], std::default_delete<SN3D_POINT_DATA[]>());
	}
	sceneData.mIncreaseMesh.pVertex = increaseMeshVertex.get();
	std::shared_ptr <SN3D_TRI_FACE> increaseMeshTriFaceId;
	if (sceneData.mIncreaseMesh.nMeshTriFaceIdCount != 0) {
		increaseMeshTriFaceId = std::shared_ptr <SN3D_TRI_FACE>(new SN3D_TRI_FACE[sceneData.mIncreaseMesh.nMeshTriFaceIdCount], std::default_delete<SN3D_TRI_FACE[]>());
	}
	sceneData.mIncreaseMesh.pMeshTriFaceId = increaseMeshTriFaceId.get();
	std::shared_ptr <float> increaseMeshWeight;
	if (sceneData.mIncreaseMesh.nVertexCount != 0) {
		increaseMeshWeight = std::shared_ptr <float>(new float[sceneData.mIncreaseMesh.nVertexCount], std::default_delete<float[]>());
	}
	sceneData.mIncreaseMesh.pWeightSet = increaseMeshWeight.get();

	m_demoApp->getSceneData(&sceneData);
	saveSceneData(sceneData,save_config);


}

void Sn3DSDKFreeScanMainView::on_btnOpenService_clicked(bool bChecked)
{
    m_scanStep.setDeviceType(deviceType());
    updateScanWidget();
	m_demoApp->Sn3DInitialize(deviceType());
	m_demoApp->registerSceneDataCallback(SDKWrapperSceneDataCallBack, this);
	//if (m_pScannerPropPanel)
		//delete m_pScannerPropPanel;
	m_pScannerPropPanel = std::make_unique<FreeScanPropPanel>();//new FreeScanPropPanel();
	dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get())->setDeviceType(deviceType());

    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::castLightTypeChaned, this, [this](int iType) {
        m_demoApp->Sn3DSetLaserGrade(iType);
        // origin cmd interface
        /*QString strResult;
        QVariantMap mapParam = { {"cmd", "setLaserGrade2"}, {"index", iType} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);*/
    });
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::objectTypeChanged, this, [this](int iType) {
        m_demoApp->Sn3DSetScanObject(iType);
        // original cmd interface
        /*QString strResult;
        QVariantMap mapParam = { {"cmd", "setScanObject2"}, {"index", iType} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);*/
    });
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::brightStepChanged, this, [this](int iType) {
        m_demoApp->Sn3DChangeBrightStep(iType);
        // original cmd interface
        /*QString strResult;
        QVariantMap mapParam = { {"cmd", "changeBrightStep2"}, {"index", iType} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);*/
    });
	connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::brightRadioChanged, this, [this](int id) {
		QString strResult;
		QVariantMap mapParam = { {"cmd", "changeBrightRadio2"}, {"index", id} };
		m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
	});
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::alignModeChanged, this, [this](int id) {
		QString strResult;
		QVariantMap mapParam = { {"cmd", "changeAlignMode2"}, {"index", id} };
		m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
	});
	m_demoApp->installGetImagasCallBack(SDKWrapperVideoCallback, this);
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::frameworkFileOpened, this, [this](const QString& strFileName) {
        QFileDialog dlg;
        auto strP3File = dlg.getOpenFileName(this, tr("Select *.p3 file"), qApp->applicationDirPath(), "framework file(*.p3);;all files(*.*)");
        if (!strP3File.isEmpty()) {
            QString strResult;
            // QVariantMap mapParam = { {"cmd", "loadFramework"}, {"FileName", "F:/frame/sdk/testframe_Project1.p3"} };
            QVariantMap mapParam = { {"cmd", "loadFramework"}, {"FileName", strP3File} };
            m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
        }
    });
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::projectTypeChanged, this, [this](int iType) {
        ScanPars scanPars;
        scanPars.scanMarkers = false;
        scanPars.scanPhotographic = false;
        scanPars.scanPointCloud = false;
        if (iType == SNSDK_PROJTYPE_FRAME)
            scanPars.scanMarkers = true;
        else if (iType == SNSDK_PROJTYPE_PHOTOGRAPHIC)
            scanPars.scanPhotographic = true;
        else if (iType == SNSDK_PROJTYPE_POINTCLOUD)
            scanPars.scanPointCloud = true;
        m_demoApp->Sn3DSetScanPars(&scanPars);
        // original cmd interface
        /*QString strResult;
        QVariantMap mapParam = { {"cmd", "setProjectType2"}, {"type", iType} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);*/
    });
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), QOverload<int>::of(&FreeScanPropPanel::pointDistChanged),
        this, [this](int iIndex) {
        QString strResult;
        QVariantMap mapParam = { {"cmd", "setPointDist2"}, {"index", iIndex} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
        VLOG(google::GLOG_INFO) << "setPointDist2 index="<<iIndex;
        
    });
    // Is heigh speed
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::isHighSpeedChanged,
        this, [this](bool bIsHigh) {
        m_demoApp->Sn3DSetIsHighSpeed(bIsHigh);
        // original cmd interface
        /*QString strResult;
        QVariantMap mapParam = { {"cmd", "setIsHighSpeed2"}, {"high", bIsHigh} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);*/
    });
    // Is strong light
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::isStrongLightChanged,
        this, [this](bool bIsStrong) {
        QString strResult;
        QVariantMap mapParam = { {"cmd", "setIsStrongLight2"}, {"strong", bIsStrong} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
    });
    // Is quality color
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::isQualityColor,
        this, [this](bool bIsColor) {
        QString strResult;
        QVariantMap mapParam = { {"cmd", "setIsQualityColor2"}, {"color", bIsColor} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
    });
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::deepHoleModeChanged,
        this, [this](bool enable) {
        QString strResult;
        QVariantMap mapParam = { {"cmd", "setDeepHoleMode2"}, {"enable", enable} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
    });
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::pointCloudModeChanged,
        this, [this](bool enable) {
        QString strResult;
        QVariantMap mapParam = { {"cmd", "setPointCloudMode2"}, {"enable", enable} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
    });
    // Is increase framework
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::isIncFrameChanged,
        this, [this](bool bIsIncrease) {
        m_demoApp->Sn3DSetIsIncFramework(bIsIncrease);
        // origin cmd interface
        /*QString strResult;
        QVariantMap mapParam = { {"cmd", "setIsIncFramework2"}, {"increase", bIsIncrease} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);*/
    });
    // Ruler file list changed
    connect(dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get()), &FreeScanPropPanel::rulerFileListChanged,
        this, [this](const QStringList rulerFileList) {
        QString strResult;
        QVariantMap mapParam = { {"cmd", "setRulerFileList2"}, {"list", rulerFileList} };
        m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
    });
}

void Sn3DSDKFreeScanMainView::on_btnCloseService_clicked(bool bChecked)
{
	m_demoApp->videoProcessControl(false);
	on_btnReleaseModbus_clicked();
    auto* pScannerPropPanel = dynamic_cast<FreeScanPropPanel*>(m_pScannerPropPanel.get());
    if (pScannerPropPanel)
        pScannerPropPanel->close();
    if (m_pChooseModePage)
        m_pChooseModePage->close();
	m_demoApp->Sn3DRelease();
}

void Sn3DSDKFreeScanMainView::slot_printInfo(QString info, LogType type)
{
    QColor color = Qt::white;
    switch (type)
    {
    case LogType::SEND_MESSAGE:
        if (!ui->chkSendMsg->isChecked())
        {
            return;
        }
        color = QColor(62, 162, 255);// Blue
        break;
    case LogType::RECEIVE_MESSAGE:
        if (!ui->chkRecvMsg->isChecked())
        {
            return;
        }
        color = QColor(73, 170, 25);// Green
        break;
    case LogType::ERROR_MESSAGE:
        if (!ui->chkErrorMsg->isChecked())
        {
            return;
        }
        color = QColor(166, 29, 36);// Red
        break;
    case LogType::INTERAL_LOG:
        if (!ui->chkInternalLog->isChecked())
        {
            return;
        }
        color = Qt::white;
        break;
    default:
        break;
    }
    ui->textBrowser->setTextColor(color);
    ui->textBrowser->append(info);
}

void Sn3DSDKFreeScanMainView::on_btnNewProject_clicked(bool bChecked)
{
    QFileDialog dlg;
    auto strDir = dlg.getExistingDirectory(this, tr("Select project folder"), qApp->applicationDirPath());
    if (!strDir.isEmpty()) {
        auto strInfo = QStringLiteral("Begin create new projects group");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);

        NewProject struNew;
        auto strDir2 = strDir.toStdString();
        std::shared_ptr<char> buffer(new char[strDir2.size() + 1]);
        if (buffer) {
            ::memcpy(buffer.get(), strDir2.c_str(), strDir2.size());
            *(buffer.get() + strDir2.size()) = 0;
            struNew.szSlnDirPath = buffer.get();
            struNew.iScanMode = m_scanStep.scanMode();
            auto errCode = m_demoApp->Sn3DNewProject(&struNew);
        }
        
        // original cmd interface
        /*QVariantMap mapParam = {
            {"cmd", "new"},
            {"fileDir", strDir},
            {"mode", m_scanStep.scanMode()}
        };
        QString strResult;
        auto errCode = m_demoApp->SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);*/
    }
}

void Sn3DSDKFreeScanMainView::on_btnScan_clicked(bool bChecked)
{
	m_demoApp->videoProcessControl(true);

    int iState = m_scanStep.scanState();
    int iEnablePre = m_scanStep.enablePrescan();
    if (iState == SNSDK_PRE_SCAN) {
        m_demoApp->Sn3DStartScan();
        auto strInfo = QStringLiteral("start preview scan");
        strInfo = strInfo.trimmed();
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        showScannerPropPanel(true);
    }
    else if (iState == SNSDK_PRE_SCANNING) {
        m_demoApp->Sn3DStartScan();
        auto strInfo = QStringLiteral("start scan");
        strInfo = strInfo.trimmed();
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        showScannerPropPanel(true);
		isPackaged = false;
    }
    else if (iState == SNSDK_SCAN) {
        m_demoApp->Sn3DPauseScan();
        auto strInfo = QStringLiteral("Begin pause scan");
        strInfo = strInfo.trimmed();
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }
    else if (iState == SNSDK_PAUSED) {
        m_demoApp->Sn3DStartScan();
        auto strInfo = QStringLiteral("start scan");
        strInfo = strInfo.trimmed();
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        showScannerPropPanel(true);
    }
    else if (iState == SNSDK_SCAN_STOPED) {
        m_demoApp->Sn3DStartScan();
        auto strInfo = QStringLiteral("start scan");
        strInfo = strInfo.trimmed();
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        showScannerPropPanel(true);
    }
}

void Sn3DSDKFreeScanMainView::on_btnSavePointCloud_clicked(bool bChecked)
{
    QFileDialog dlg;
    auto strDir = dlg.getExistingDirectory(this, tr("Select project folder"), qApp->applicationDirPath());
    if (!strDir.isEmpty()) {
        if (strDir.right(1) != "/")
            strDir += "/";
        auto strInfo = QStringLiteral("Begin save scan");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        m_demoApp->saveASC(strDir);
    }
}

// open new dialog for mesh setting [11/14/2023 sbc]
void Sn3DSDKFreeScanMainView::on_btnMesh_clicked(bool bChecked)
{
	m_demoApp->Sn3DMeshPostProcess(false);
	switchToPostProcessPage();
}

void Sn3DSDKFreeScanMainView::on_btnOptimizedMesh_clicked(bool bChecked)
{
	m_demoApp->Sn3DMeshPostProcess(true);
	switchToPostProcessPage();
}

void Sn3DSDKFreeScanMainView::on_btnOpenProject_clicked(bool bChecked)
{
	QFileDialog dlg;
	//根据设备类型和扫描模式过滤文件后缀名
	//		  UEP					Combo
	//激光   .sln_laser_uep	        .sln_laser_combo
	//红外	 无						.sln_ir_combo

	int scanType = SNSDK_SCANMODE_LASER;
	if(m_pChooseModePage)
		scanType = m_pChooseModePage->scanmode();

	QString scanTypeStr = scanType== SNSDK_SCANMODE_LASER ? "laser" : "ir";
	QString deviceStr;
	switch (deviceType())
	{
	case SNSDK_DEVTYPE_UEPRO:
		deviceStr = "uep";
		break;
	case SNSDK_DEVTYPE_COMBO:
		deviceStr = "combo";
		break;
	case SNSDK_DEVTYPE_TRIO:
		deviceStr = "trio";
		break;
	default:
        deviceStr = "*";
		break;
	}
	QString slnFilter = QString("*.sln_%1_%2").arg(scanTypeStr).arg(deviceStr);
	auto strDir = dlg.getOpenFileName(this, tr("Select project file"), qApp->applicationDirPath(),slnFilter);
	if (!strDir.isEmpty()) {
		auto strInfo = QStringLiteral("Begin load projects group");
		this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
		auto errCode = m_demoApp->openProject(strDir.toStdString().c_str(),(ScanType)scanType);
	}
}

void Sn3DSDKFreeScanMainView::on_btnEnterCalib_clicked(bool bChecked)
{
    m_demoApp->SnChangeCalibMode(SNSDK_CALIBMODE_QUICK);
}

void Sn3DSDKFreeScanMainView::on_btnStartCalib_clicked(bool bChecked)
{
    m_demoApp->Sn3DCaliSnapImage();
}

void Sn3DSDKFreeScanMainView::showScannerPropPanel(bool bShow)
{
    FreeScanPropPanel* pPanel = (FreeScanPropPanel*)m_pScannerPropPanel.get();
    if (bShow) {
        pPanel->move(this->pos().x() - pPanel->width(), this->pos().y());
        pPanel->show();
    }else
        pPanel->hide();
}

void Sn3DSDKFreeScanMainView::updateScanWidget()
{
    ui->btnEnterScan->setEnabled(m_scanStep.hasEnterScan());
}

void Sn3DSDKFreeScanMainView::on_btnEnterScan_clicked(bool bChecked)
{
    if (!m_pChooseModePage) {
        m_pChooseModePage = new ChooseModePage();
        connect(m_pChooseModePage, &ChooseModePage::scanModeChanged, [this](int iScanMode) {
            m_scanStep.setScanMode(iScanMode);
            m_demoApp->Sn3DSetScanMode(iScanMode);
            // original cmd interface
            /*QVariantMap mapParam = { {"cmd", "setScanMode2"}, {"ScanMode", iScanMode} };
            QString strResult;
            m_demoApp->SnSendMessage("2.0", "request", "other", mapParam, strResult);*/
        });
    }
    if (m_pChooseModePage)
        m_pChooseModePage->show();
}

static auto rtMap2Str(const QVariantMap& rtMap)
{
	QString ret = "Current RT: ";
	for (auto iRot = 0; iRot < 9; iRot++) {
		auto strRotKey = QString("rot%1").arg(iRot);
		auto strRot = rtMap[strRotKey].toString();
		ret += strRotKey + "=" + strRot + ", ";
	}
	for (auto iTrans = 0; iTrans < 3; iTrans++) {
		auto strTransKey = QString("trans%1").arg(iTrans);
		auto strTrans = rtMap[strTransKey].toString();
		if (iTrans < 2)
			ret += strTransKey + "=" + strTrans + ", ";
		else
			ret += strTransKey + "=" + strTrans;
	}
	return ret;

}

void Sn3DSDKFreeScanMainView::onSnMessageReceived(QVariantMap mapParam)
{
    if (!mapParam.contains(CMD_STR))
        return;
    QString strCmd = mapParam[CMD_STR].toString();
    if (strCmd == "calibCalculateResult") {
        QString strCalibResult;
        if (!mapParam.contains("calibResult"))
            return;
        if (mapParam["calibResult"].toBool() == true) {
            if (!mapParam.contains("camErr"))
                return;
            auto dCamErr = mapParam["camErr"].toDouble();
            strCalibResult = QString(QStringLiteral("calibration success,error value=%1")).arg(dCamErr);
        }
        else {
            auto strErrCode = tr("unknown");
            if (mapParam.contains("errCodeJson")) {
                QVariantMap mapErrorCode;
                auto mapErrJson = mapParam["errCodeJson"].toMap();
                if (mapErrJson.contains("cameraCalib")) {
                    auto listCamera = mapErrJson["cameraCalib"].toList();
                    for (int i = 0; i < listCamera.length(); i++) {
                        auto mapCamera = listCamera[i].toMap();
                        if (mapCamera.contains("errorCode")) {
                            mapErrorCode[QString("camera%1").arg(i)] = mapCamera["errorCode"].toString();
                        }
                    }
                }
                if (mapErrJson.contains("laserCalib")) {
                    auto listLaser = mapErrJson["laserCalib"].toList();
                    for (int i = 0; i < listLaser.length(); i++) {
                        auto mapCamera = listLaser[i].toMap();
                        if (mapCamera.contains("errorCode")) {
                            mapErrorCode[QString("laser%1").arg(i)] = mapCamera["errorCode"].toString();
                        }
                    }
                }
                strErrCode = "";
                for (auto it = mapErrorCode.begin(); it != mapErrorCode.end(); ++it) {
                    strErrCode += (it.key() + ":" + it.value().toString() + ",");
                }
            }
            strCalibResult = QString(QStringLiteral("calibration failed,error code=%1")).arg(strErrCode);
        }
        this->slot_printInfo(strCalibResult, LogType::RECEIVE_MESSAGE);
    }
    else if (strCmd == "sdkScanPageSwitched") {
        auto strInfo = QStringLiteral("switch to scan");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }
    else if (strCmd == "ScanModeChange") {
        // Change scan mode(status) to prescan/scan/pause etc.
        if (mapParam.contains(SCANMODE_STR)) {
            int iScanState = mapParam[SCANMODE_STR].toInt();
            m_scanStep.setScanState(iScanState);
            this->onScanStateChange();
            auto strInfo = QStringLiteral("switch to state %1").arg(iScanState);
            this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        }
    }
    else if (strCmd == "returnScanSoftSetting") {
        if (mapParam.contains("enablePreScanFlg")) {
            bool bEnable = mapParam["enablePreScanFlg"].toBool();
            m_scanStep.setEnablePrescan(bEnable);
            this->onScanStateChange();
        }
        if (mapParam.contains("uep_changepoint_arrayvalue")) {
            m_listDist.clear();
            auto strList = mapParam["uep_changepoint_arrayvalue"].toString();
            auto list = strList.split(',');
            foreach(const auto& v, list) {
                m_listDist.push_back(v.toDouble());
            }
            updateScanProp();
        }
    }
    else if (strCmd == "generatePointCloudResult") {
        if (mapParam.contains("result")) {
            int iResult = mapParam["result"].toInt();
            QString strInfo = tr("Generate data error: %1").arg(iResult);
            if (SNSDK_ERR_OK == iResult)
                strInfo = tr("Generate data success");
            this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        }
    }
    else if (strCmd == "cancelScanFinish") {
        auto strInfo = tr("End cancel scan");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }
    else if (strCmd == "showProgress") {
        if (mapParam.contains("Change Point Distance") && mapParam.contains("value")
            && mapParam["value"] == 1) {
            auto strInfo = tr("Change point distance OK");
            this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        }
    }
    else if (strCmd == "createOrLoadCurProjectFinished") {
        if (mapParam.contains("PROJECT_TYPE")){
            auto strType = mapParam["PROJECT_TYPE"].toString();
            m_pScannerPropPanel->setProjectType(strType);
        }
        if (mapParam.contains("DIS")) {
            auto strDist = mapParam["DIS"].toString();
            m_pScannerPropPanel->setPointDist(strDist);
        }
        if (mapParam.contains("LASER_GRADE")) {
            auto strGrade = mapParam["LASER_GRADE"].toString();
            m_pScannerPropPanel->setLaserGrade(strGrade);
        }
        if (mapParam.contains("MATERIAL_TYPE")) {
            auto strMaterial = mapParam["MATERIAL_TYPE"].toString();
            m_pScannerPropPanel->setMaterial(strMaterial);
        }
        if (mapParam.contains("HIGH_SPEED_MODE")) {
            auto strHigh = mapParam["HIGH_SPEED_MODE"].toString();
            m_pScannerPropPanel->setHighSeed(strHigh);
        }
        if (mapParam.contains("STRONG_LIGHT_MODE")) {
            auto strStrong = mapParam["STRONG_LIGHT_MODE"].toString();
            m_pScannerPropPanel->setStrongLight(strStrong);
        }
        if (mapParam.contains("BRIGHTNESS_LEVEL")) {
            auto strLevel = mapParam["BRIGHTNESS_LEVEL"].toString();
            m_pScannerPropPanel->setBrightnessLevel(strLevel);
        } 
    }
	else if (strCmd == "currentRTcmd" && m_bShowCurrentRT) {
		auto strRT = rtMap2Str(mapParam);
		this->slot_printInfo(strRT, LogType::RECEIVE_MESSAGE);
	}
    else if (strCmd == "setRulerFileListReply") {
        auto iCode = mapParam["errorcode"].toInt();
        auto strInfo = tr("set land mark file list fail, errorcode = %s").arg(iCode);
        if (0 == iCode)
            strInfo = tr("set land mark file list successfully");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    }
}

void Sn3DSDKFreeScanMainView::loadScanSoftSetting()
{
    QString strResult;
    QString strCmd = "loadScanSoftSetting";
    QVariantMap mapParam = { {"cmd", strCmd} };
    m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
}

void Sn3DSDKFreeScanMainView::switchToPostProcessPage()
{
	Sn3DSDKMesh* meshDlg = new Sn3DSDKMesh();
	meshDlg->setWindowModality(Qt::ApplicationModal);
	
    connect(meshDlg, &Sn3DSDKMesh::messageReady, [this](QVariantMap mapParam) {
        auto strInfo = QStringLiteral("Begin mesh");
        this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
        if (mapParam.contains("method") && mapParam["method"].toString() == "apply") {
            FreeScanMeshPar meshParam;
            meshParam.watertightType=mapParam["watertightType"].toInt();
            meshParam.fileter_level = mapParam["fileter_level"].toInt();
            meshParam.smoothValue=mapParam["smooth_level"].toInt();
			VLOG(google::GLOG_INFO) << "filterType = " << mapParam["filterType_"].toInt();
			VLOG(google::GLOG_INFO) << "filterType1 = " << mapParam["fileter_level"].toInt();
            meshParam.remove_small_floating_parts = mapParam["remove_small_floating_parts"].toInt();
            meshParam.max_trianlges_ischeck = mapParam["max_trianlges_ischeck"].toBool();
            meshParam.max_trianlges_value = mapParam["max_trianlges_value"].toInt();
            meshParam.border_optimize=mapParam["use_border_opt"].toBool();
            meshParam.fill_small_hole_ischeck = mapParam["fill_small_hole_ischeck"].toBool();
            meshParam.fill_small_hole_value = mapParam["fill_small_hole_value"].toInt();
            meshParam.neighborhood_perimeter=mapParam["neighborhood_perimeter"].toInt();
            meshParam.remove_spike_ischeck = mapParam["remove_spike_ischeck"].toBool();
            meshParam.marker_hole_filling_ischeck = mapParam["marker_hole_filling_ischeck"].toBool();
            m_demoApp->Sn3DMesh(&meshParam);
        }
        else if (mapParam.contains("method") && mapParam["method"].toString() == "save") {
            FreeSaveMeshPar meshSave;
            auto strFileName = mapParam["fileName"].toString().toStdString();
            meshSave.fileName = strFileName.c_str();
            auto strFolderPath = mapParam["folderPath"].toString().toStdString();
            meshSave.folderPath = strFolderPath.c_str();
            auto strFormat = mapParam["format"].toString().toStdString();
            meshSave.format = strFormat.c_str();
            auto strMethod = mapParam["method"].toString().toStdString();
            meshSave.method = strMethod.c_str();
            meshSave.postpageVisible = mapParam["postpageVisible"].toInt();
            meshSave.saveAscFile = mapParam["saveAscFile"].toInt();
            meshSave.saveStlFile = mapParam["saveStlFile"].toInt();
            meshSave.saveObjFile = mapParam["saveObjFile"].toInt();
            meshSave.savePlyFile = mapParam["savePlyFile"].toInt();
            meshSave.saveP3File = mapParam["saveP3File"].toInt();
            meshSave.save3MfFile = mapParam["save3MfFile"].toInt();
            meshSave.saveTxtFile = mapParam["saveTxtFile"].toInt();
            meshSave.saveCsvFile = mapParam["saveCsvFile"].toInt();
            m_demoApp->Sn3DSaveMesh(&meshSave);
        }
        else if (mapParam.contains("cmd") && mapParam["cmd"].toString() == "undoMesh") {
            m_demoApp->Sn3DUndoMesh();
        }
        else if (mapParam.contains("cmd") && mapParam["cmd"].toString() == "confirmMesh") {
            m_demoApp->Sn3DConfirmMesh();
        }
        // Following code reserved for test
        /*else {
            QString strResult;
            m_demoApp->SnSendMessage("v2.0", "request", "other", mapParam, strResult);
        }*/
		isPackaged = true;
	});
	connect(meshDlg, &Sn3DSDKMesh::sigPrintInfo, [this](QString strInfo) {
		this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
	});
	connect(meshDlg, &Sn3DSDKMesh::destroyed, [this]() {
		//切换回扫描界面
        // DON'T switch to scan form, we may need post process
		//ui->btnScanTab->click();
		m_demoApp->Sn3DEnterScanPage();
	});
	meshDlg->exec();
}

void Sn3DSDKFreeScanMainView::onScanStateChange()
{
    int iState = m_scanStep.scanState();
    int iEnablePre = m_scanStep.enablePrescan();
    if (iState == SNSDK_PRE_SCAN) {
        VLOG(google::GLOG_INFO) << "debug prescan1";
        ui->btnScan->setText(tr("PreScan"));
    }
    else if (iState == SNSDK_PRE_SCANNING) {
        ui->btnScan->setText(tr("Scan"));
    }
    else if (iState == SNSDK_SCAN) {
        ui->btnScan->setText(tr("Pause"));
    }
    else if (iState == SNSDK_PAUSED) {
        ui->btnScan->setText(tr("Scan"));
    }
    else if (iState == SNSDK_SCAN_STOPED) {
        if (iEnablePre) {
            VLOG(google::GLOG_INFO) << "debug prescan2";
            ui->btnScan->setText(tr("PreScan"));
        }
        else
            ui->btnScan->setText(tr("Scan"));
    }
}

void Sn3DSDKFreeScanMainView::on_btnEndScan_clicked(bool bChecked)
{
    m_demoApp->Sn3DEndScan();
    // original cmd interface
    /*QVariantMap mapParam = { {"method", "scanCtrl"},{"cmd", "stop"},{"target", "Sn3DLaserScanController"} };
    QString strResult;
    m_demoApp->SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);*/
}

void Sn3DSDKFreeScanMainView::on_btnCancelScan_clicked(bool bChecked)
{
    auto strInfo = tr("Begin cancel scan");
    this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
    m_demoApp->Sn3DCancelScan();
    // original cmd interface
    /*QVariantMap mapParam = { {"method", "scanCtrl"},{"cmd", "delete"},{"target", "Sn3DLaserScanController"} };
    QString strResult;
    m_demoApp->SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);*/

}

void Sn3DSDKFreeScanMainView::on_socketInitialBtn_pressed()
{
	auto strInfo = tr("Begin Initial");
	this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);

	if (m_demoApp) {
		m_demoApp->Sn3DInitializeSocket(deviceType());
	}
	strInfo = tr("End Initial");
	this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
}

void Sn3DSDKFreeScanMainView::on_socketReleaseBtn_pressed()
{
	auto strInfo = tr("Begin Release");
	this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);

	if (m_demoApp)
		m_demoApp->Sn3DRecleaseSocket();

	strInfo = tr("End Release");
	this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
}

void Sn3DSDKFreeScanMainView::on_socketConnectBtn_pressed()
{
	auto strInfo = tr("Begin Connect");
	this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);

	if (m_demoApp) {
		m_demoApp->Sn3DConnectSocket(ui->sockeIPLEdit->text(), ui->socketPortLEdit->text().toInt());
	}

	strInfo = tr("End Connect");
	this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
}

void Sn3DSDKFreeScanMainView::on_disconnectBtn_pressed()
{
}

void Sn3DSDKFreeScanMainView::on_sendBtn_pressed()
{
	auto strInfo = tr("Begin Send");
	this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);

	if (m_demoApp) {
		m_demoApp->Sn3DSendSocket(ui->socketSendTEdit->toPlainText());
	}

	strInfo = tr("End Send");
	this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
}

void Sn3DSDKFreeScanMainView::updateScanProp()
{
    if (m_pScannerPropPanel) {
        m_pScannerPropPanel->setPointDistList(m_listDist);
    }
}


void Sn3DSDKFreeScanMainView::on_btnInitializeModbus_clicked(bool checked)
{
	if (!m_bModbusInitialized) {
		m_demoApp->Sn3DInitializeModbus(m_scanStep.deviceType());
		m_bModbusInitialized = true;
	}
}

void Sn3DSDKFreeScanMainView::on_btnReleaseModbus_clicked(bool checked)
{
	if (m_bModbusInitialized) {
		m_demoApp->Sn3DReleaseModbus();
		m_bModbusInitialized = false;
	}
}

void Sn3DSDKFreeScanMainView::on_btnPostProcess_clicked(bool checked)
{
    m_pPostProcessForm->show();
}

void Sn3DSDKFreeScanMainView::onPostProcessApplied()
{
    auto strInfo = QStringLiteral("Apply post process finished");
    this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
}

void Sn3DSDKFreeScanMainView::onPostProcessConfirmed()
{
    auto strInfo = QStringLiteral("Confirm post process finished");
    this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
}

void Sn3DSDKFreeScanMainView::onPostProcessCanceled()
{
    auto strInfo = QStringLiteral("Cancel post process finished");
    this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
}

void Sn3DSDKFreeScanMainView::onPostProcessRollbacked()
{
    auto strInfo = QStringLiteral("Rollback post process finished");
    this->slot_printInfo(strInfo, LogType::RECEIVE_MESSAGE);
}

void Sn3DSDKFreeScanMainView::applyPostProcess(FreeScanPostProcessForm::AutoFillType fillType, float fPerimeter)
{
    QVariantMap param = { {"autoFillType", (int)fillType},{"markerPerimeter", fPerimeter} };
    m_demoApp->Sn3DApplyPostProcess(param);
}

void Sn3DSDKFreeScanMainView::confirmPostProcess(FreeScanPostProcessForm::AutoFillType fillType, float fPerimeter)
{
    m_demoApp->Sn3DConfirmPostProcess();
}

void Sn3DSDKFreeScanMainView::cancelPostProcess()
{
    m_demoApp->Sn3DCancelPostProcess();
}

void Sn3DSDKFreeScanMainView::rollbackPostProcess()
{
    m_demoApp->Sn3DRollbackPostProcess();
}