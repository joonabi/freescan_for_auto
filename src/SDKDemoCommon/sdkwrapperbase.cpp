#include "sdkwrapperbase.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qsettings.h"
#include "qexception.h"
#include "util.h"
#include "qbytearray.h"
#include "calibrator.h"
#include "snjsondocument.h"
#include "sn3dsdkpluginpub.h"
#include "glog/logging.h"
#include "sdkmodbuswrapper.h"

#define ERR_STR_MSG(msg, ec) QString("%1 error code: 0x%2").arg(msg).arg(QString::number(ec, 16).toUpper(), 8, QChar('0'))
enum ProjectFlags { Solution = 0x00, Project, Solution_Project };
enum ScanAlignMode { Marker };
enum ProjectType { Human, Thing, SmallThing, Frame = 0x00, PointCloud, FrameNoInc, PhotoGraphic };
enum ScanTypes {
    SCAN_Grid,     // ����
    SCAN_HD,        // ��ϸɨ��
    SCAN_Markers,   // ��־��
    SCAN_Photo     // ��Ӱ����
   // SCAN_Grid       // ����ɨ��
};

void CALLBACK SDKWrapperCallback(int iEventType, int iExtendData, void* pData, size_t stDataLen, void* pUserData)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)pUserData;
    if (pWrapper)
        pWrapper->CallBack(iEventType, iExtendData, pData, stDataLen);
}

void CALLBACK SDKWrapperDeviceStatusCallback(LPDeviceStatus deviceStatus, void* pUserData)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)pUserData;
    if (pWrapper)
        pWrapper->deviceStatusChanged(deviceStatus);
}

void CALLBACK SDKWrapperSn3DCameraPositionCallBack(LPSn3dCameraPosition cameraPosition, void* pUserData)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)pUserData;
    if (pWrapper)
        pWrapper->setCameraPosition(cameraPosition);
}

void CALLBACK SDKWrapperSn3DTrackLostStatusCallBack(bool trackLostStatus, void* owner)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
    if (pWrapper)
        pWrapper->setTrackLost(trackLostStatus);
}

void CALLBACK SDKWrapperSn3DScanDistCallBack(double scanDist, void* owner)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
    if (pWrapper)
        pWrapper->setScanDis(scanDist);
}

void CALLBACK SDKWrapperSn3DDeviceEventCallBack(DeviceEvent event, void* owner)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
    if (pWrapper)
        pWrapper->setDeviceEvent(event);
}

void CALLBACK SDKWrapperSn3DWholePointCloudCallBack(LPSn3dPointCloud wholePointCloud)
{
    VLOG(google::GLOG_INFO) << __FUNCTION__ << " wholePointCloud->vertex_count = " << wholePointCloud->vertex_count;
}

void CALLBACK SDKWrapperSn3DCurrentPointCloudCallBack(LPSn3dPointCloud currentPointCloud, void* owner)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
    if (pWrapper)
    {
        VLOG(google::GLOG_INFO) << __FUNCTION__ << " currentPointCloud->vertex_count = " << currentPointCloud->vertex_count;
    }
}

void CALLBACK SDKWrapperSn3DIncreasePointCloudCallBack(LPSn3dIncreasePointCloud increasePointCloud, void* owner)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
    if (pWrapper)
    {
        VLOG(google::GLOG_INFO) << __FUNCTION__ << " increasePointCloud->vertex_count = " << increasePointCloud->vertex_count;
    }
}

void CALLBACK SDKWrapperSn3DPointCountCallBack(int pointCount, void* owner)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
    if (pWrapper)
        pWrapper->setPointCount(pointCount);
}

void CALLBACK SDKWrapperSn3DRangeDataCallBack(LPSn3dRangeData rangeData, void* owner)
{
	SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
	if (pWrapper)
	{
		VLOG(google::GLOG_INFO) << __FUNCTION__ << " range data flush ";
		pWrapper->flushRangeData(rangeData);
	}
}

void CALLBACK SDKWrapperSn3DProjectInfoCallBack(LPSn3dProjectInfo projectInfo, void *owner)
{
	SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
	if (pWrapper)
	{
		VLOG(google::GLOG_INFO) << __FUNCTION__ << " range data flush ";
		pWrapper->flushProjectInfo(projectInfo);
	}
}

void CALLBACK SDKWrapperSn3DFrameRateCallBack(int frameRate, void* owner)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
    if (pWrapper)
        pWrapper->setFrameRate(frameRate);
}

void CALLBACK SDKWrapperSn3DFrameCountCallBack(int frameCount, void* owner)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
    if (pWrapper)
        pWrapper->setFrameCount(frameCount);
}

void CALLBACK SDKWrapperSn3DTooFlatStatusCallBack(bool tooFlatStatus, void* owner)
{
    SDKWrapperBase* pWrapper = (SDKWrapperBase*)owner;
    if (pWrapper)
        pWrapper->setTooFlatStatus(tooFlatStatus);
}

SDKWrapperBase::SDKWrapperBase(QObject* parent)
{
	QSettings settings("./scansoftwaresdk.ini", QSettings::IniFormat);
	m_bShowCurrentRT = settings.value("showCurrentRT").toBool();
}
SDKWrapperBase::~SDKWrapperBase()
{
    Sn3DRegisterCallback(m_hService, NULL, NULL);
    Sn3DRelease();
}

int SDKWrapperBase::Sn3DInitialize(int iDeviceType)
{
    VLOG(google::GLOG_INFO) << __FUNCTION__ << " m_hService = " << m_hService;
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE) {
        auto err = ::Sn3DInitialize(&m_hService, iDeviceType);
        VLOG(google::GLOG_INFO) << "Sn3DSDK_CreateService return m_hService = " << m_hService;
        if (m_hService != INVALID_HSN3DSDKSERVICE_VALUE) {
            Sn3DRegisterCallback(m_hService, SDKWrapperCallback, (void*)this);

            Sn3DSetWholePointCloudCallBack(SDKWrapperSn3DWholePointCloudCallBack, (void*)this);
            Sn3DSetCurrentPointCloudCallBack(SDKWrapperSn3DCurrentPointCloudCallBack, (void*)this);
            Sn3DSetIncreasePointCloudCallBack(SDKWrapperSn3DIncreasePointCloudCallBack, (void*)this);

			Sn3DSetRangeDataCallBack(SDKWrapperSn3DRangeDataCallBack, (void*)this);
			Sn3DSetProjectInfoCallBack(SDKWrapperSn3DProjectInfoCallBack, (void*)this);

            Sn3DSetPointCountCallBack(SDKWrapperSn3DPointCountCallBack, (void*)this);
            Sn3DSetCameraPositionCallBack(SDKWrapperSn3DCameraPositionCallBack, (void*)this);
            Sn3DSetTrackLostStatusCallBack(SDKWrapperSn3DTrackLostStatusCallBack, (void*)this);
            Sn3DSetScanDistCallBack(SDKWrapperSn3DScanDistCallBack, (void*)this);
            Sn3DSetDeviceEventCallBack(SDKWrapperSn3DDeviceEventCallBack, (void*)this);
            Sn3DSetFrameRateCallBack(SDKWrapperSn3DFrameRateCallBack, (void*)this);
            Sn3DSetFrameCountCallBack(SDKWrapperSn3DFrameCountCallBack, (void*)this);
            Sn3DSetTooFlatStatusCallBack(SDKWrapperSn3DTooFlatStatusCallBack, (void*)this);
            Sn3DSetDeviceStatusCallBack(m_hService,SDKWrapperDeviceStatusCallback, (void*)this);
            /*::Sn3DInitModbus(&m_hModBus, iDeviceType);
            ::Sn3DModbusStart(m_hModBus, m_hService);*/
            // m_modBus.connect(m_hService, iDeviceType);
        }

    }
    return m_hService != INVALID_HSN3DSDKSERVICE_VALUE ? SNSDK_ERR_OK : SNSDK_ERR_INITIALIZEFIAILZED;
}

int SDKWrapperBase::Sn3DInitializeSocket(int iDeviceType)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE) {
        auto ret = ::Sn3DInitializeSocket(&m_hService, iDeviceType);
        if (SNSDK_ERR_OK != ret){
            emit showMsg(ERR_STR_MSG("initialize sdk failed!", ret), LogType::INTERAL_LOG);
        }
        emit showMsg("initialize sdk success.", LogType::INTERAL_LOG);
        VLOG(google::GLOG_INFO) << "Sn3DInitializeSocket return m_hService = " << m_hService;
    }
    return m_hService == INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DRelease()
{
    VLOG(google::GLOG_INFO) << __FUNCTION__ << " m_hService = " << m_hService;
    // m_modBus.disconnect();
    // ::Sn3DModbusStop(m_hModBus, m_hService);
    // ::Sn3DReleaseModbus(&m_hModBus);
    
    if (m_hService  == INVALID_HSN3DSDKSERVICE_VALUE)
        return SNSDK_ERR_NULLSERVICE;
    else {
        auto ret = ::Sn3DRelease(&m_hService);
        VLOG(google::GLOG_INFO) << "Sn3DSDK_DestroyService return " << ret;
        if (SNSDK_ERR_OK != ret) {
            emit showMsg(ERR_STR_MSG("release sdk failed!", ret), LogType::INTERAL_LOG);
            return ret;
        }
        emit showMsg("release sdk success.", LogType::INTERAL_LOG);
        return ret;
    }
}

int SDKWrapperBase::Sn3DReleaseSocket()
{
    VLOG(google::GLOG_INFO) << __FUNCTION__ << " m_hService = " << m_hService;
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return SNSDK_ERR_NULLSERVICE;
    else {
		Sn3DDisconnectSocket();
        auto ret = ::Sn3DReleaseSocket(&m_hService);
        VLOG(google::GLOG_INFO) << "Sn3DRecleaseSocket return " << ret;
        if (ret == SNSDK_ERR_OK)
            return ret;
        return ret;
    }
}

int SDKWrapperBase::Sn3DConnectSocket(const QString& ip, int port)
{
    auto ret = m_socketHelper.initSocket(ip.toStdString(), port);
    if (ret!=0)
    {
        emit showMsg(ERR_STR_MSG("connect failed!", ret), LogType::INTERAL_LOG);
        return ret;
    }
    emit showMsg("connect success!", LogType::INTERAL_LOG);
    if (nullptr == m_pThread)
    {
        m_pThread.reset(new std::thread([this]() {
            this->Sn3DReceiveSocket();
        }));
    }

    return ret;
}

int SDKWrapperBase::Sn3DDisconnectSocket()
{
    if (m_pThread && m_pThread->joinable()) {
        m_bQuit = true;
        m_pThread->join();
        m_pThread.reset();
    }
    int ret = m_socketHelper.releaseSocket();
    m_bQuit = false;
    return ret;
}

int SDKWrapperBase::Sn3DSendSocket(const QString& str)
{
    std::string result;
    m_socketHelper.sendMsg(str.toStdString());
    return true;
}

int SDKWrapperBase::Sn3DReceiveSocket()
{
    while (!m_bQuit)
    {
        std::string result = "";
        m_socketHelper.receiveMsg(result);
        if (!result.empty())
        {
            emit showMsg(QString::fromStdString(result), LogType::RECEIVE_MESSAGE);
        }
    }
    return SNSDK_ERR_OK;
}

bool SDKWrapperBase::SnNewProject(const QString& strFileDir)
{
    VLOG(google::GLOG_INFO) << __FUNCTION__ << " m_hService = " << m_hService;
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return false;
    else {
        QVariantMap mapParam = { {"cmd", "new"}, {"theme", "new"},
            {"type", Solution_Project},{"success",true},
        {"fileDir", strFileDir}, {"scanType", "handleScan"}, {"modName", "ChooseProjectPage"},
        {"mode", SNSDK_SCANMODE_LASER}, {"DIS", 0.5}, {"TEX", false},
        {"ALIGN_MODE", Marker}, {"PROJECT_TYPE", PointCloud}, {"DEF_OPTIMIAL", 0},
        {"LASER_GRADE", 26}
        };
        QString strResult;
        auto errCode = SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);
        // ��ʱ��ʹ�����з���������������װ��
        // auto errCode = Sn3DSDK_NewProject(m_hService, strFileDir.toStdString().c_str());
        VLOG(google::GLOG_INFO) << "SnSendMessage return " << errCode;
        if (errCode == SNSDK_ERR_OK)
            return true;
        return false;
    }
}

bool SDKWrapperBase::SnScanModeChanged(int scanMode)
{
    VLOG(google::GLOG_INFO) << __FUNCTION__ << " m_hService = " << m_hService;
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return false;
    else {
        auto errCode = ::Sn3DChangeScanMode(m_hService, scanMode);
        VLOG(google::GLOG_INFO) << "Sn3DSDK_ScanModeChanged return " << errCode;
        if (errCode == SNSDK_ERR_OK)
            return true;
        return false;
    }
}

int SDKWrapperBase::Sn3DOpenOrCreateSolution(
    const QString& strSlnDirPath,
    bool isCreate,
    int iScanMode,
    bool hasTexture,
    float fPointDis)
{
    return SNSDK_ERR_NULLFUNCTION;
}

int SDKWrapperBase::Sn3DOpenOrCreateSolution(const QString & strSlnDirPath, bool isCreate, int iScanMode, bool hasTexture, float fPointDis, int iAlignType, bool useNonrigid, int iNeedLimit)
{
	return SNSDK_ERR_NULLFUNCTION;
}

int SDKWrapperBase::Sn3DNewProject(
    const QString& strSlnDirPath,
    int iScanMode,
    bool hasTexture,
    float fPointDis,
    int iAlignType,
    const QString& strGlobalMarkerPath)
{
    return SNSDK_ERR_NULLFUNCTION;
}

int SDKWrapperBase::Sn3DLoadP3File(const QString& p3Path)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DLoadP3File(m_hService, p3Path.toStdString().c_str());
}

int SDKWrapperBase::Sn3DEnterScan(ScanType scanType)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DEnterScan(m_hService,scanType);
}

int SDKWrapperBase::Sn3DExitScan()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DExitScan(m_hService);
}
// (bool)
int SDKWrapperBase::Sn3DStartScan()
{
    return SNSDK_ERR_UNIMPLEMENTED;
}

int SDKWrapperBase::Sn3DSetScanPars(LPScanPars param)
{
    if (nullptr == m_hService)
        return SNSDK_ERR_NULLDATA;
    return ::Sn3DSetScanPars(m_hService, param);
}

int SDKWrapperBase::Sn3DPreviewScan()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DPreviewScan(m_hService);
}

int SDKWrapperBase::Sn3DEndScan(bool globalOptimize, double pointDist, Sn3DWholePointCloudCallBack wholeCallback)
{
    return SNSDK_ERR_UNIMPLEMENTED;
}

int SDKWrapperBase::Sn3DClearScan(bool isCancelCurrentProjectFramerMark)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DClearScan(m_hService,isCancelCurrentProjectFramerMark);
}

int SDKWrapperBase::Sn3DGlobalOptimization()
{
    if (nullptr == m_hService)
        return SNSDK_ERR_NULLDATA;
    return ::Sn3DGlobalOptimization(m_hService);
}

int SDKWrapperBase::Sn3DPointFeatureAlign(LPPointFeatureAlign pointFeatureAlign, float* rot, float* tran)
{
	if (nullptr == m_hService)
		return SNSDK_ERR_NULLDATA;
	return ::Sn3DPointFeatureAlign(m_hService, pointFeatureAlign, rot, tran);
}

int SDKWrapperBase::Sn3DPointManualAlign(LPPointManualAlign pointManualAlign, float* rot, float* tran)
{
	if (nullptr == m_hService)
		return SNSDK_ERR_NULLDATA;
	return ::Sn3DPointManualAlign(m_hService, pointManualAlign, rot, tran);
}

int SDKWrapperBase::Sn3DMesh(LPMeshPars meshPars, Sn3DMeshDataCallBack callback, void *owner)
{
    return SNSDK_ERR_UNIMPLEMENTED;
}

bool SDKWrapperBase::SnSetLEDBrightness(int iStep)
{
    VLOG(google::GLOG_INFO) << __FUNCTION__ << " m_hService = " << m_hService;
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return false;
    else {
        auto errCode = ::Sn3DSetLEDBrightness(m_hService, iStep);
        VLOG(google::GLOG_INFO) << "Sn3DSDK_Scan return " << errCode;
        if (errCode == SNSDK_ERR_OK)
            return true;
        return false;
    }
}

void SDKWrapperBase::CallBack(int iEventType, int iExtendData, void* pData, size_t stDataLen)
{
    std::string str((char*)pData, stDataLen);
    QString strJson(str.c_str());
    // VLOG(google::GLOG_INFO) << __FUNCTION__ << " " << strJson.toStdString();
    QJsonObject root;
    QJsonParseError parseError;
    try {
        QJsonDocument doc;
        auto buffer = SnJsonDocument::getLegalChar(strJson);
        doc = QJsonDocument::fromJson(buffer, &parseError);
        root = doc.object();
    }
    catch (QException exp) {
		VLOG(google::GLOG_INFO) << __FUNCTION__ << " " << strJson.toStdString();
        VLOG(google::GLOG_INFO) << __FUNCTION__ << " " << exp.what();
        return;
    }
	// There're too many currentRTcmd
	if (root.contains("cmd") && root["cmd"].toString() == "currentRTcmd") {
		if (m_bShowCurrentRT)
			VLOG(google::GLOG_INFO) << __FUNCTION__ << " " << strJson.toStdString();
	}
	else
		VLOG(google::GLOG_INFO) << __FUNCTION__ << " " << strJson.toStdString();
    // �豸״̬����
    if (root.contains("cmd") && root["cmd"].toString() == "deviceStatus"
        && root.contains("offline")) {
        //int devStatus = SNSDK_DEVSTAT_OFFLINE; // sn3dsdk_c_wrapper::DeviceStatus
        //if (!root["offline"].toBool())
        //    devStatus = SNSDK_DEVSTAT_ONLINE;
		DeviceStatus devStatus;
		devStatus.offline = SNSDK_DEVSTAT_OFFLINE; // sn3dsdk_c_wrapper::DeviceStatus
		if (!root["offline"].toBool())
			devStatus.offline = SNSDK_DEVSTAT_ONLINE;

        emit deviceStatusChanged(&devStatus);

    }
    // �����½����̳ɹ�����
    else if (root.contains("cmd") && root["cmd"].toString() == "createOrLoadSolutionFinished") {

    }
    else if (root.contains("cmd") && root["cmd"].toString() == "addPrj" && root.contains("prjInfo")) {
        auto prjInfo = root["prjInfo"].toObject();
        if (prjInfo.contains("prjPath")) {
            auto strPrjName = prjInfo["prjPath"].toString();
            if (strPrjName.isEmpty())
                return;
            // @note �������б�����������.laser_prj_uep���Ҳ����ظ�
            auto listPrjName = strPrjName.split('.');
            if (listPrjName.size() <= 1)
                return;
            strPrjName = listPrjName[0];
            auto list = m_scanResultFile.prjNameList();
            // �����ظ�
            if (!list.contains(strPrjName)) {
                list.append(strPrjName);
                m_scanResultFile.setPrjNameList(list);
            }
        }
    }
    else if (root.contains("cmd") && root["cmd"].toString() == "newProjectFinish") {
        emit newProjFinished(m_scanResultFile.prjNameList());
    }
    else if (root.contains("cmd") && root["cmd"].toString() == "saveScanDataReturn") {
        emit saveScanDataReturn();
    }
    else if (root.contains("cmd") && root["cmd"].toString() == "updateStateWhenStopFusion") {
        emit updateStateWhenStopFusion();
    }
    else if (root.contains("cmd") && root["cmd"].toString() == "initalCalibParam") {
        // changeCaliMode
        emit initCalibParamFinished();
    }
    else if (root.contains("cmd") && root["cmd"].toString() == "showProgress"
        && root.contains("data")) {
        QString strTitle;
        double dValue;
        bool bVisible = false;

        QJsonObject objData = root["data"].toObject();
        if (objData.contains("title"))
            strTitle = objData["title"].toString();
        if (objData.contains("value"))
            dValue = objData["value"].toDouble();
        if (objData.contains("visible"))
            bVisible = objData["visible"].toBool();
        emit showProgress(strTitle, dValue, bVisible);
    }
	else if (root.contains("cmd") && root["cmd"].toString() == "hideProgress") {
		emit showProgress("", 0,false);
	}
    else if (root.contains("cmd") && root["cmd"].toString() == "CalibHeightOKChanged"
        && root.contains("okheight")) {
        auto iOKHeight = root["okheight"].toInt();
        emit calibHeightOKChanged(iOKHeight);
    }
    else if (root.contains("cmd") && root["cmd"].toString() == "setGroupIndex"
        && root.contains("index")) {
        auto iGroupIndex = root["index"].toInt();
        emit groupIndexChanged(iGroupIndex);
    }
    else if (root.contains("method") && root["method"].toString() == "meshResult"&&root.contains("success")) {
        auto success = root["success"].toBool();
        emit meshDataReturn(success);
    }
    else if (root.contains("method") && root["method"].toString() == "saveMeshFinished"&&root.contains("finish")) {
        auto success = root["finish"].toBool();
        emit saveMeshDataReturn(success);
    }
    else if (root.contains("cmd") && root["cmd"].toString() == "showMaximized") {
        emit showMaximized();
    }
    else {
        
        if (parseError.error == QJsonParseError::NoError) {
            auto mapParam = root.toVariantMap();
            emit SnMessageReceived(mapParam);
        }
        else {
            VLOG(google::GLOG_INFO) << "error = " << parseError.error;
        }
    }
}


void SDKWrapperBase::deviceStatusCallBack(LPDeviceStatus deviceStatus)
{
	emit deviceStatusChanged(deviceStatus);
}

void SDKWrapperBase::flushRangeData(LPSn3dRangeData rangeData)
{
	emit rangeDataChanged(rangeData);
}

void SDKWrapperBase::flushProjectInfo(LPSn3dProjectInfo projectInfo)
{
	emit revealProjectInfo(projectInfo);
}

void SDKWrapperBase::setPointCount(int pointCount)
{
    emit pointCountChanged(pointCount);
}

void SDKWrapperBase::setFrameRate(int frameRate)
{
    emit frameRateChanged(frameRate);
}

void SDKWrapperBase::setFrameCount(int frameCount)
{
    emit frameCountChanged(frameCount);
}

void SDKWrapperBase::setCameraPosition(LPSn3dCameraPosition cameraPosition)
{
    emit cameraPositionChanged(cameraPosition);
}

void SDKWrapperBase::setTrackLost(bool trackLost)
{
    emit trackLostStatusChanged(trackLost);
}

void SDKWrapperBase::setScanDis(double dis)
{
    emit scanDistChanged(dis);
}

void SDKWrapperBase::setDeviceEvent(DeviceEvent event)
{
    emit deviceEventChanged(event);
}

void SDKWrapperBase::setTooFlatStatus(bool tooFlat)
{
    emit tooFlatStatusChanged(tooFlat);
}

bool SDKWrapperBase::SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
    const QString& strMsg,
    QString& strResult)
{
    VLOG(google::GLOG_INFO) << __FUNCTION__ << " m_hService = " << m_hService << ", strMsg = " << strMsg.toStdString();
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return false;
    else {
        QByteArray buffer(1024, 0);
        size_t stResultLen = buffer.size();
        if (!buffer.isEmpty()) {
            auto errCode = ::Sn3DSendMessage(m_hService,
                strVersion.toStdString().c_str(), strMsgType.toStdString().c_str(), strZMQTopic.toStdString().c_str(),
                strMsg.toStdString().c_str(), strMsg.size(), buffer.data(), &stResultLen);
            std::string strBuffer(buffer, stResultLen);
            VLOG(google::GLOG_INFO) << "Sn3DSDK_SendMessage return " << errCode << ", buffer = " << strBuffer;
            if (errCode == SNSDK_ERR_OK) {
                return true;
            }
        }
        else
            VLOG(google::GLOG_INFO) << " out of memory.";
        return false;
    }
}

bool SDKWrapperBase::SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
    const QVariantMap& mapMsg,
    QString& strResult)
{
    QString strMsg = demo_util::map2qstr(mapMsg);
    return SnSendMessage(strVersion, strMsgType, strZMQTopic, strMsg, strResult);
}

// ���ͼ�������
bool SDKWrapperBase::sendLaserNum(int iLaserNum)
{
    QString strResult;
    QVariantMap mapLaserNum = { {"method", "scanCtrl"}, {"cmd", "setConfig"},{"changeLaserGrade", iLaserNum},
    {"isUEP7", false},{"target","Sn3DLaserScanController"} };
    auto bChangeStep1 = SnSendMessage("v2.0", "invoke", "other", mapLaserNum, strResult);
    return true;
}

// bool endScan();
bool SDKWrapperBase::changeLight(int iStep, int iLaserNum)
{
    QVariantMap mapLight;
    QVariantMap mapSubValue = { {"deviceType", "upe"}, {"alignMode", 0}, {"isRightNow", true},
    {"scanObj", 0}, {"laserGrade",iLaserNum}, {"hasFilter", false}, {"filterSwitch", false}, {"step", iStep} };
    QVariantMap mapParam = { {"cmd", "setConfig"},{"setDeviceBrightness", mapSubValue}, {"target","Sn3DLaserScanController"} };
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);
}

bool SDKWrapperBase::enterToPreScan(bool bGeneratedCloud)
{
    QVariantMap mapParam = { {"method", "scanCtrl"},{"cmd", "preScan"},{"target", "Sn3DLaserScanController"},
    {"isGeneratedData", bGeneratedCloud} };
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);
}

bool SDKWrapperBase::switchScanMode(int iScanMode)
{
    int iLastScanMode = 0;
    if (iScanMode == 0)
        iLastScanMode = 1;
    QVariantMap mapParam = { {"method", "switchScanMode"},{"cmd", "switchScanMode"},{"defOptimal", 1},
    {"lastScanMode", iLastScanMode},{"currentScanMode", iScanMode},{"target", "Sn3DLaserScanController"} };
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);
}

bool SDKWrapperBase::saveASC(const QString& strFolder)
{
    m_scanResultFile.setFolder(strFolder);
    auto mapParam = m_scanResultFile.cmdForSaveASC();
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);
}

int SDKWrapperBase::Sn3DSaveData(const QString& strAbsolutePrjName, const QString& strSavePath, const QString& strSaveType)
{
    m_scanResultFile.setFolder(strSavePath);
    auto mapParam = m_scanResultFile.cmdForSaveASC();
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);
}

int SDKWrapperBase::Sn3DPauseScan()
{
	return SNSDK_ERR_UNIMPLEMENTED;
}

int SDKWrapperBase::Sn3DContinueScan()
{
	return SNSDK_ERR_UNIMPLEMENTED;
}

bool SDKWrapperBase::changeObject(ObjectType objType)
{
    QVariantMap mapParam = { {"method","scanCtrl"}, {"cmd","setConfig"},{"changeMaterialType",objType},
    {"target","Sn3DLaserScanController"} };
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);
}

bool SDKWrapperBase::enbableVideo(bool bEnabled)
{
    QString strResult;
    QVariantMap mapStream = { {"cmd", "enableImageStream"}, {"enable", bEnabled} };
    return SnSendMessage("v2.0", "invoke", "other", mapStream, strResult);
}

bool SDKWrapperBase::SnChangeCalibMode(int iMode)
{
    Calibrator calibrator;
    auto mapStream = calibrator.getChangeCalibModeCmd(iMode);
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapStream, strResult);
}

bool SDKWrapperBase::SnEnterCalib()
{
    Calibrator calibrator;
    auto mapStream = calibrator.getChangeCalibModeCmd(SNSDK_CALIBMODE_QUICK);
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapStream, strResult);
}


bool SDKWrapperBase::SnExitCalib()
{
    Calibrator calibrator;
    auto mapStream = calibrator.getExitCalibCmd();
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapStream, strResult);
}

int SDKWrapperBase::Sn3DMeshPostProcess(const bool&optimized)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DMeshPostProcess(m_hService, optimized);
}

int SDKWrapperBase::videoProcessControl(const bool & start)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	if (start)
		return ::Sn3DStartProcessVideoData(m_hService);
	else
		return ::Sn3DStopProcessVideoData();
}

void SDKWrapperBase::installGetImagasCallBack(Sn3DGetImagesCallBack imageCallback, void * owner)
{
	::Sn3DSetImagasCallBack(m_hService, imageCallback, owner);
}

int SDKWrapperBase::connectDevice()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DConnectDevice(m_hService);
}

int SDKWrapperBase::reconnectDevice()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DReConnectDevice(m_hService);
}

int SDKWrapperBase::Sn3DGetDeviceIsOnline()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DGetDeviceIsOnline(m_hService);
}

int SDKWrapperBase::Sn3DOpenProject(const char* projFileName, ScanType scanType)
{
    return SNSDK_ERR_NULLFUNCTION;
}

int SDKWrapperBase::Sn3DRangeDataFinish()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DRangeDataFinish(m_hService);
}

int SDKWrapperBase::Sn3DSetDeviceBrightness(LPDeviceBrightness devBrightness)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetDeviceBrightness(m_hService,devBrightness);
}

int SDKWrapperBase::Sn3DSetLEDBrightness(int iStep)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetLEDBrightness(m_hService,iStep);
}

int SDKWrapperBase::Sn3DGetBrightnessRange(int &min, int &max)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DGetBrightnessRange(m_hService,min,max);
}

int SDKWrapperBase::Sn3DGetCurrentBrightness(int &currentBrightness)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DGetCurrentBrightness(m_hService,currentBrightness);
}

int SDKWrapperBase::Sn3DSetEnablePseudoColor(bool enable)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetEnablePseudoColor(m_hService,enable);
}

int SDKWrapperBase::Sn3DSetWorkRange(float min, float max)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetWorkRange(m_hService,min,max);
}

int SDKWrapperBase::registerSceneDataCallback(Sn3DSceneDataCallBack pfCallBack, void * pUserData)
{
	return ::Sn3DRegisterSceneDataCallback(pfCallBack,pUserData);
}

int SDKWrapperBase::getSceneData(LPSN3D_SCENE_DATA sceneData)
{
    return ::Sn3DGetSceneData(sceneData);
}

int SDKWrapperBase::Sn3DSetWholePointCloudCallBack(Sn3DWholePointCloudCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetWholePointCloudCallBack(m_hService, callBackFunc, owner);
}

int SDKWrapperBase::Sn3DSetCurrentPointCloudCallBack(Sn3DCurrentPointCloudCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetCurrentPointCloudCallBack(m_hService, callBackFunc, owner);
}

int SDKWrapperBase::Sn3DSetIncreasePointCloudCallBack(Sn3DIncreasePointCloudCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetIncreasePointCloudCallBack(m_hService, callBackFunc, owner);
}

int SDKWrapperBase::Sn3DSetRangeDataCallBack(Sn3DRangeDataCallBack callBackFunc, void* owner)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DSetRangeDataCallBack(m_hService, callBackFunc, owner);
}

int SDKWrapperBase::Sn3DSetProjectInfoCallBack(Sn3DProjectInfoCallBack callBackFunc, void* owner)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DSetProjectInfoCallBack(m_hService, callBackFunc, owner);
}

int SDKWrapperBase::Sn3DSetCameraPositionCallBack(Sn3DCameraPositionCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetCameraPositionCallBack(m_hService,callBackFunc,owner);
}

int SDKWrapperBase::Sn3DSetTrackLostStatusCallBack(Sn3DTrackLostStatusCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetTrackLostStatusCallBack(m_hService,callBackFunc,owner);
}

int SDKWrapperBase::Sn3DSetScanDistCallBack(Sn3DScanDistCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetScanDistCallBack(m_hService,callBackFunc,owner);
}

int SDKWrapperBase::Sn3DSetDeviceEventCallBack(Sn3DDeviceEventCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetDeviceEventCallBack(m_hService,callBackFunc,owner);
}

 int SDKWrapperBase::Sn3DSetPointCountCallBack(Sn3DPointCountCallBack callBackFunc, void *owner)
 {
     if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
         return INVALID_HSN3DSDKSERVICE_VALUE;
     return ::Sn3DSetPointCountCallBack(m_hService,callBackFunc,owner);
 }

int SDKWrapperBase::Sn3DSetFrameRateCallBack(Sn3DFrameRateCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetFrameRateCallBack(m_hService,callBackFunc,owner);
}

int SDKWrapperBase::Sn3DSetFrameCountCallBack(Sn3DFrameCountCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetFrameCountCallBack(m_hService,callBackFunc,owner);
}

int SDKWrapperBase::Sn3DSetTooFlatStatusCallBack(Sn3DTooFlatStatusCallBack callBackFunc, void *owner)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetTooFlatStatusCallBack(m_hService,callBackFunc,owner);
}

int SDKWrapperBase::Sn3DSetBrightness(int level)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetBrightness(m_hService,level);
}

int SDKWrapperBase::setMaskBackGround(int level,bool enable)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DSetMaskBackGround(m_hService, level, enable);
}

int SDKWrapperBase::Sn3DCaliSnapImage()
{
	return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DCaliCalculate()
{
	return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DEnterCali()
{
	return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DImproFramePoint(QString filePath)
{
	return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DNewProject(LPNewProject newProject)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DSetLaserGrade(int index)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DEndScan()
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DMesh(LPFreeScanMeshPar pParam)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DSaveMesh(LPFreeSaveMeshPar pParam)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DConfirmMesh()
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DUndoMesh()
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DSetCalibMode(int iMode)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DSetIsIncFramework(bool bInc)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DEnterScanPage()
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DChangeBrightStep(int iIndex)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DSetIsHighSpeed(bool bIsHigh)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DSetScanObject(int iIndex)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DEnterScanModePage()
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DSetScanMode(int iMode)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DCancelScan()
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DStopCalib(int iMode)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DApplyPostProcess(const QVariantMap param)
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DConfirmPostProcess()
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DCancelPostProcess()
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DRollbackPostProcess()
{
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DInitializeModbus(int iDeviceType)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE) {
		auto ret = ::Sn3DInitializeModbus(&m_hService, iDeviceType);
		if (SNSDK_ERR_OK != ret) {
			emit showMsg(ERR_STR_MSG("initialize sdk failed!", ret), LogType::INTERAL_LOG);
		}
		emit showMsg("initialize sdk success.", LogType::INTERAL_LOG);
		VLOG(google::GLOG_INFO) << "Sn3DInitializeModbus return m_hService = " << m_hService;
	}
	return m_hService == INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperBase::Sn3DReleaseModbus()
{
	VLOG(google::GLOG_INFO) << __FUNCTION__ << " m_hService = " << m_hService;
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return SNSDK_ERR_NULLSERVICE;
	else {
		// Sn3DDisconnectSocket();
		auto ret = ::Sn3DReleaseModbus(&m_hService);
		VLOG(google::GLOG_INFO) << "Sn3DReleaseModbus return " << ret;
		if (ret == SNSDK_ERR_OK)
			return ret;
		return ret;
	}
}