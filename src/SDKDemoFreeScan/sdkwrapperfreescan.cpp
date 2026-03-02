#include "sdkwrapperfreescan.h"
#include "glog/logging.h"
SDKWrapperFreeScan::SDKWrapperFreeScan(QObject* parent)
    : SDKWrapperBase(parent)
{

}

SDKWrapperFreeScan::~SDKWrapperFreeScan()
{
    
}

int SDKWrapperFreeScan::Sn3DInitialize(int iDeviceType)
{
    return SDKWrapperBase::Sn3DInitialize(iDeviceType);
}

int SDKWrapperFreeScan::Sn3DRelease()
{
    return SDKWrapperBase::Sn3DRelease();
}

bool SDKWrapperFreeScan::SnNewProject(const QString& strFileDir)
{
    return SDKWrapperBase::SnNewProject(strFileDir);
}

int SDKWrapperFreeScan::Sn3DNewProject(LPNewProject newProject)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DNewProject(m_hService, newProject);
}

int SDKWrapperFreeScan::Sn3DOpenOrCreateSolution(
    const QString& strSlnDirPath,
    bool isCreate,
    int iScanMode,
    bool hasTexture,
    float fPointDis)
{
    return SDKWrapperBase::Sn3DOpenOrCreateSolution(strSlnDirPath, isCreate, iScanMode, hasTexture, fPointDis);
}

int SDKWrapperFreeScan::Sn3DNewProject(
    const QString& strSlnDirPath,
    int iScanMode,
    bool hasTexture,
    float fPointDis,
    int iAlignType,
    const QString& strGlobalMarkerPath)
{
    return SDKWrapperBase::Sn3DNewProject(strSlnDirPath, iScanMode, hasTexture, fPointDis, iAlignType,
        strGlobalMarkerPath);
}

bool SDKWrapperFreeScan::SnScanModeChanged(int scanMode) // sn3dsdk_c_wrapper::ScanMode
{
    return SDKWrapperBase::SnScanModeChanged(scanMode);
}

bool SDKWrapperFreeScan::SnSetLEDBrightness(int iStep)
{
    return SDKWrapperBase::SnSetLEDBrightness(iStep);
}

void SDKWrapperFreeScan::CallBack(int iEventType, int iExtendData, void* pData, size_t stDataLen)
{
    return SDKWrapperBase::CallBack(iEventType, iExtendData, pData, stDataLen);
}

bool SDKWrapperFreeScan::SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
    const QString& strMsg,
    QString& strResult)
{
    return SDKWrapperBase::SnSendMessage(strVersion, strMsgType, strZMQTopic, strMsg,
        strResult);
}

bool SDKWrapperFreeScan::SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
    const QVariantMap& mapMsg,
    QString& strResult)
{
    return SDKWrapperBase::SnSendMessage(strVersion, strMsgType, strZMQTopic, mapMsg,
        strResult);
}

bool SDKWrapperFreeScan::sendLaserNum(int iLaserNum)
{
    return SDKWrapperBase::sendLaserNum(iLaserNum);
}

int SDKWrapperFreeScan::Sn3DPauseScan()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DPauseScan(m_hService);
    // original cmd interface
    /*QString strResult;
    QVariantMap mapEndCode = { {"method", "scanCtrl"}, {"cmd", "pause"}, {"target", "Sn3DLaserScanController"} };
    SnSendMessage("v2.0", "invoke", "other", mapEndCode, strResult);*/
}

bool SDKWrapperFreeScan::changeLight(int iStep, int iLaserNum)
{
    return SDKWrapperBase::changeLight(iStep, iLaserNum);
}

bool SDKWrapperFreeScan::enterToPreScan(bool bGeneratedCloud)
{
    return SDKWrapperBase::enterToPreScan(bGeneratedCloud);
}

bool SDKWrapperFreeScan::switchScanMode(int iScanMode)
{
    return SDKWrapperBase::switchScanMode(iScanMode);
}

// bool
int SDKWrapperFreeScan::Sn3DStartScan()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DStartScan(m_hService);
    // original cmd interface
    /*QVariantMap mapParam = { {"method", "scanCtrl"},{"cmd", "scan"},{"target", "Sn3DLaserScanController"} };
    QString strResult;
    return SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);*/
}

bool SDKWrapperFreeScan::saveASC(const QString& strFolder)
{
    return SDKWrapperBase::saveASC(strFolder);
}

int SDKWrapperFreeScan::Sn3DSaveData(const QString& strAbsolutePrjName, const QString& strSavePath, const QString& strSaveType)
{
    return SDKWrapperBase::Sn3DSaveData(strAbsolutePrjName, strSavePath, strSaveType);
}

bool SDKWrapperFreeScan::changeObject(ObjectType objType)
{
    return SDKWrapperBase::changeObject(objType);
}

bool SDKWrapperFreeScan::enbableVideo(bool bEnabled)
{
    return SDKWrapperBase::enbableVideo(bEnabled);
}

bool SDKWrapperFreeScan::SnChangeCalibMode(int iMode)
{
    return SDKWrapperBase::SnChangeCalibMode(iMode);
}
int SDKWrapperFreeScan::Sn3DMeshPostProcess(const bool & optimized)
{
	return SDKWrapperBase::Sn3DMeshPostProcess(optimized);
}

int SDKWrapperFreeScan::videoProcessControl(const bool&start)
{
	return SDKWrapperBase::videoProcessControl(start);
}

void SDKWrapperFreeScan::installGetImagasCallBack(Sn3DGetImagesCallBack imageCallback, void * owner)
{
	SDKWrapperBase::installGetImagasCallBack(imageCallback, owner);
}

int SDKWrapperFreeScan::registerSceneDataCallback(Sn3DSceneDataCallBack pfCallBack, void * pUserData)
{
	return SDKWrapperBase::registerSceneDataCallback(pfCallBack,pUserData);
}

int SDKWrapperFreeScan::getSceneData(LPSN3D_SCENE_DATA sceneData)
{
	return SDKWrapperBase::getSceneData(sceneData);
}

int SDKWrapperFreeScan::Sn3DOpenProject(const char * projFileName, ScanType scanType)
{
	QString scanTypeStr = scanType == SNSDK_SCANMODE_LASER ? "LASER" : "IR";

	QString wholePath(projFileName);
	QString fileName("");
	QString fileDir("");
	
	int lastIndex = wholePath.lastIndexOf("/");

	if (lastIndex != -1) {
		fileDir = wholePath.left(lastIndex);
		fileName = wholePath.right(wholePath.length() - lastIndex - 1);
	}

	if (fileName.isEmpty() || fileDir.isEmpty()) return -1;

	QVariantMap mapParam = {
		{"cmd", "openProject"},
		{"fileDir", fileDir},
		{"fileName",fileName},
		{"ScanType",scanTypeStr}
	};

	QString strResult;
	auto errCode = SnSendMessage("v2.0", "invoke", "other", mapParam, strResult);
	VLOG(google::GLOG_INFO) << "SnSendMessage return " << errCode;
	if (errCode == SNSDK_ERR_OK)
		return 0;
	return -1;
}

bool SDKWrapperFreeScan::SnEnterCalib()
{
    return SDKWrapperBase::SnEnterCalib();
}

bool SDKWrapperFreeScan::SnExitCalib()
{
    return SDKWrapperBase::SnExitCalib();
}

int SDKWrapperFreeScan::Sn3DCaliSnapImage()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DCaliSnapImage(m_hService);
}

int SDKWrapperFreeScan::Sn3DSetLaserGrade(int index)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetLaserGrade(m_hService, index);
}

int SDKWrapperFreeScan::Sn3DEndScan()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DEndScan(m_hService);
}

int SDKWrapperFreeScan::Sn3DMesh(LPFreeScanMeshPar pParam)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DMesh(m_hService, pParam);
}

int SDKWrapperFreeScan::Sn3DSaveMesh(LPFreeSaveMeshPar pParam)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSaveMesh(m_hService, pParam);
}

int SDKWrapperFreeScan::Sn3DConfirmMesh()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DConfirmMesh(m_hService);
}

int SDKWrapperFreeScan::Sn3DUndoMesh()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DUndoMesh(m_hService);
}

int SDKWrapperFreeScan::Sn3DSetCalibMode(int iMode)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetCalibMode(m_hService, iMode);
}

int SDKWrapperFreeScan::Sn3DSetIsIncFramework(bool bInc)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetIsIncFramework(m_hService, bInc);
}

int SDKWrapperFreeScan::Sn3DEnterScanPage()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DEnterScanPage(m_hService);
}

int SDKWrapperFreeScan::Sn3DChangeBrightStep(int iIndex)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DChangeBrightStep(m_hService, iIndex);
}

int SDKWrapperFreeScan::Sn3DSetIsHighSpeed(bool bIsHigh)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetIsHighSpeed(m_hService, bIsHigh);
}

int SDKWrapperFreeScan::Sn3DSetScanObject(int iIndex)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetScanObject(m_hService, iIndex);
}

int SDKWrapperFreeScan::Sn3DEnterScan(ScanType scanType)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DEnterScan(m_hService, scanType);
}

int SDKWrapperFreeScan::Sn3DEnterScanModePage()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DEnterScanModePage(m_hService);
}

int SDKWrapperFreeScan::Sn3DSetScanMode(int iMode)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DSetScanMode(m_hService, iMode);
}

int SDKWrapperFreeScan::Sn3DCancelScan()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DCancelScan(m_hService);
}

int SDKWrapperFreeScan::Sn3DStopCalib(int iMode)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DStopCalib(m_hService, iMode);
}

int SDKWrapperFreeScan::Sn3DApplyPostProcess(const QVariantMap param)
{
    VLOG(google::GLOG_INFO) << __FUNCTION__ << param.count();
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperFreeScan::Sn3DConfirmPostProcess()
{
    VLOG(google::GLOG_INFO) << __FUNCTION__;
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperFreeScan::Sn3DCancelPostProcess()
{
    VLOG(google::GLOG_INFO) << __FUNCTION__;
    return INVALID_HSN3DSDKSERVICE_VALUE;
}

int SDKWrapperFreeScan::Sn3DRollbackPostProcess()
{
    VLOG(google::GLOG_INFO) << __FUNCTION__;
    return INVALID_HSN3DSDKSERVICE_VALUE;
}