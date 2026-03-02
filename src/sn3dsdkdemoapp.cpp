#include "sn3dsdkdemoapp.h"
#include "SDKDemoFreeScan/sdkwrapperfreescan.h"
#include "SDKDemoOptimScan/sdkwrapperoptimscan.h"
#include "SDKDemoEinScan/sdkwrappereinscan.h"

#ifdef Q_QDOC
Sn3DSDKDemoApp::Sn3DSDKDemoApp(int &argc, char **argv)
    : QGuiApplication(argc, argv)
{

}
#else
Sn3DSDKDemoApp::Sn3DSDKDemoApp(int &argc, char **argv, int flags)
    : QApplication(argc, argv, flags)
{

}
#endif

Sn3DSDKDemoApp::~Sn3DSDKDemoApp()
{

}

int Sn3DSDKDemoApp::Sn3DInitialize(int iDeviceType)	// breakpoint add here
{
	if (m_pSDKWrapper)
		return SNSDK_ERR_AlREADYINITIALIZED;
    if (SNSDK_DEVTYPE_UEPRO == iDeviceType || SNSDK_DEVTYPE_COMBO == iDeviceType || SNSDK_DEVTYPE_TRIO == iDeviceType || SNSDK_DEVTYPE_UEP2 == iDeviceType || SNSDK_DEVTYPE_COMBOPLUS == iDeviceType) {
        m_pSDKWrapper = new SDKWrapperFreeScan();
        if (!m_pSDKWrapper)
            return SNSDK_ERR_OUTOFMEMORY;
    }
    else if (SNSDK_DEVTYPE_5M == iDeviceType || SNSDK_DEVTYPE_9M == iDeviceType
        || SNSDK_DEVTYPE_E10 == iDeviceType) {
        m_pSDKWrapper = new SDKWrapperOptimScan();
        if (!m_pSDKWrapper)
            return SNSDK_ERR_OUTOFMEMORY;
    }
	else if (SNSDK_DEVTYPE_2X == iDeviceType ) {
		m_pSDKWrapper = new SDKWrapperEinScan();
		if (!m_pSDKWrapper)
			return SNSDK_ERR_OUTOFMEMORY;
	}
    connect(m_pSDKWrapper, &SDKWrapperBase::newProjFinished, this, &Sn3DSDKDemoApp::newProjFinished);
    connect(m_pSDKWrapper, &SDKWrapperBase::deviceStatusChanged, this, &Sn3DSDKDemoApp::deviceStatusChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::saveScanDataReturn, this, &Sn3DSDKDemoApp::saveScanDataReturn);
    connect(m_pSDKWrapper, &SDKWrapperBase::meshDataReturn, this, &Sn3DSDKDemoApp::meshDataReturn);
    connect(m_pSDKWrapper, &SDKWrapperBase::saveMeshDataReturn, this, &Sn3DSDKDemoApp::saveMeshDataReturn);
    connect(m_pSDKWrapper, &SDKWrapperBase::updateStateWhenStopFusion, this, &Sn3DSDKDemoApp::updateStateWhenStopFusion);
    connect(m_pSDKWrapper, &SDKWrapperBase::showProgress, this, &Sn3DSDKDemoApp::showProgress);
    connect(m_pSDKWrapper, &SDKWrapperBase::initCalibParamFinished, this, &Sn3DSDKDemoApp::initCalibParamFinished);
    connect(m_pSDKWrapper, &SDKWrapperBase::calibHeightOKChanged, this, &Sn3DSDKDemoApp::calibHeightOKChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::groupIndexChanged, this, &Sn3DSDKDemoApp::groupIndexChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::showMaximized, this, &Sn3DSDKDemoApp::showMaximized);
    connect(m_pSDKWrapper, &SDKWrapperBase::SnMessageReceived, this, &Sn3DSDKDemoApp::SnMessageReceived);
    connect(m_pSDKWrapper, &SDKWrapperBase::pointCountChanged, this, &Sn3DSDKDemoApp::pointCountChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::frameCountChanged, this, &Sn3DSDKDemoApp::frameCountChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::frameRateChanged, this, &Sn3DSDKDemoApp::frameRateChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::cameraPositionChanged, this, &Sn3DSDKDemoApp::cameraPositionChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::trackLostStatusChanged, this, &Sn3DSDKDemoApp::trackLostStatusChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::scanDistChanged, this, &Sn3DSDKDemoApp::scanDistChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::deviceEventChanged, this, &Sn3DSDKDemoApp::deviceEventChanged);
    connect(m_pSDKWrapper, &SDKWrapperBase::tooFlatStatusChanged, this, &Sn3DSDKDemoApp::tooFlatStatusChanged);
	connect(m_pSDKWrapper, &SDKWrapperBase::rangeDataChanged, this, &Sn3DSDKDemoApp::flushRangeData);
	connect(m_pSDKWrapper, &SDKWrapperBase::revealProjectInfo, this, &Sn3DSDKDemoApp::revealProjectInfo);
    // Post process
    connect(m_pSDKWrapper, &SDKWrapperBase::postProcessApplied, this, &Sn3DSDKDemoApp::postProcessApplied);
    connect(m_pSDKWrapper, &SDKWrapperBase::postProcessConfirmed, this, &Sn3DSDKDemoApp::postProcessConfirmed);
    connect(m_pSDKWrapper, &SDKWrapperBase::postProcessCanceled, this, &Sn3DSDKDemoApp::postProcessCanceled);
    connect(m_pSDKWrapper, &SDKWrapperBase::postProcessRollbacked, this, &Sn3DSDKDemoApp::postProcessRollbacked);
    // End post process
    return m_pSDKWrapper->Sn3DInitialize(iDeviceType);
}

int Sn3DSDKDemoApp::Sn3DInitializeSocket(int iDeviceType)
{
    if (SNSDK_DEVTYPE_5M == iDeviceType || SNSDK_DEVTYPE_9M == iDeviceType
        || SNSDK_DEVTYPE_E10 == iDeviceType) {
		m_pSDKWrapper = new SDKWrapperOptimScan();
	}
	else if (SNSDK_DEVTYPE_UE == iDeviceType || SNSDK_DEVTYPE_UEPRO == iDeviceType || SNSDK_DEVTYPE_UEP2 == iDeviceType
		|| SNSDK_DEVTYPE_COMBO == iDeviceType || SNSDK_DEVTYPE_COMBOPLUS == iDeviceType || SNSDK_DEVTYPE_TRIO == iDeviceType) {
		m_pSDKWrapper = new SDKWrapperFreeScan();
	}

    connect(m_pSDKWrapper, &SDKWrapperBase::showMsg, this, &Sn3DSDKDemoApp::showMsg);

    return m_pSDKWrapper->Sn3DInitializeSocket(iDeviceType);
}

int Sn3DSDKDemoApp::Sn3DInitialLog(const QString& strExeName, const QString& strPath)
{
    return SNSDK_ERR_NULLFUNCTION;
}

int Sn3DSDKDemoApp::Sn3DRecleaseSocket()
{
	if (!m_pSDKWrapper) return 0;
	disconnect(m_pSDKWrapper, &SDKWrapperBase::showMsg, this, &Sn3DSDKDemoApp::showMsg);
	auto ret = m_pSDKWrapper->Sn3DReleaseSocket();
	delete m_pSDKWrapper;
	m_pSDKWrapper = nullptr;
	return ret;
}

int Sn3DSDKDemoApp::Sn3DRelease()
{
	if (!m_pSDKWrapper) return 0;
    auto ret = m_pSDKWrapper->Sn3DRelease();
    delete m_pSDKWrapper;
    m_pSDKWrapper = nullptr;
    return ret;
}

int Sn3DSDKDemoApp::Sn3DConnectSocket(const QString& ip, int port)
{
    return m_pSDKWrapper->Sn3DConnectSocket(ip, port);
}

int Sn3DSDKDemoApp::Sn3DDisconnectSocket()
{
    return m_pSDKWrapper->Sn3DDisconnectSocket();
}

int Sn3DSDKDemoApp::Sn3DSendSocket(const QString& str)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSendSocket(str);
}

int Sn3DSDKDemoApp::Sn3DOpenOrCreateSolution(
    const QString& strSlnDirPath,
    bool isCreate,
    int iScanMode,
    bool hasTexture,
    float fPointDis)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DOpenOrCreateSolution(strSlnDirPath, isCreate, iScanMode, hasTexture, fPointDis);
}

int Sn3DSDKDemoApp::Sn3DOpenOrCreateSolution(const QString & strSlnDirPath, bool isCreate, int iScanMode, bool hasTexture, float fPointDis, int iAlignType, bool useNonrigid, int iNeedLimit)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DOpenOrCreateSolution(strSlnDirPath, isCreate, iScanMode, hasTexture, fPointDis, iAlignType, useNonrigid, iNeedLimit);
}

int Sn3DSDKDemoApp::Sn3DNewProject(
    const QString& strSlnDirPath,
    int iScanMode,
    bool hasTexture,
    float fPointDis,
    int iAlignType,
    const QString& strGlobalMarkerPath)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DNewProject(strSlnDirPath,iScanMode,hasTexture,fPointDis,iAlignType,strGlobalMarkerPath);
}

bool Sn3DSDKDemoApp::SnNewProject(const QString & strFileDir)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->SnNewProject(strFileDir);
}

int Sn3DSDKDemoApp::Sn3DLoadP3File(const QString & p3Path)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DLoadP3File(p3Path);
}

int Sn3DSDKDemoApp::SnChangeScanMode(int iScanMode) // sn3dsdk_c_wrapper::ScanMode
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->SnScanModeChanged(iScanMode);
}

int Sn3DSDKDemoApp::Sn3DStartScan()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DStartScan();
}

int Sn3DSDKDemoApp::Sn3DPauseScan()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DPauseScan();
}

int Sn3DSDKDemoApp::Sn3DContinueScan()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DContinueScan();
}



int Sn3DSDKDemoApp::Sn3DGlobalOptimization()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DGlobalOptimization();
}

int Sn3DSDKDemoApp::Sn3DPointFeatureAlign(LPPointFeatureAlign pointFeatureAlign, float* rot, float* tran)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DPointFeatureAlign(pointFeatureAlign, rot, tran);
}

int Sn3DSDKDemoApp::Sn3DPointManualAlign(LPPointManualAlign pointManualAlign, float* rot, float* tran)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DPointManualAlign(pointManualAlign, rot, tran);
}

bool Sn3DSDKDemoApp::changeLight(int iStep, int iLaserNum)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->changeLight(iStep,iLaserNum);
}

bool Sn3DSDKDemoApp::sendLaserNum(int iLaserNum)
{
	if (m_pSDKWrapper == nullptr)
		return false;
	return m_pSDKWrapper->sendLaserNum(iLaserNum);
}

bool Sn3DSDKDemoApp::saveASC(const QString & strFolder)
{
	if (m_pSDKWrapper == nullptr)
		return false;
	return m_pSDKWrapper->saveASC(strFolder);
}

int Sn3DSDKDemoApp::Sn3DSaveData(const QString& strAbsolutePrjName, const QString& strSavePath, const QString& strSaveType)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSaveData(strAbsolutePrjName, strSavePath, strSaveType);
}

int Sn3DSDKDemoApp::Sn3DMeshPostProcess(const bool & optimized)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DMeshPostProcess(optimized);
}

bool Sn3DSDKDemoApp::SnChangeCalibMode(int iMode)
{
	if (m_pSDKWrapper == nullptr)
		return false;
    return m_pSDKWrapper->SnChangeCalibMode(iMode);
}

bool Sn3DSDKDemoApp::SnEnterCalib()
{
	if (m_pSDKWrapper == nullptr)
		return false;
    return m_pSDKWrapper->SnEnterCalib();
}

bool Sn3DSDKDemoApp::SnExitCalib()
{
	if (m_pSDKWrapper == nullptr)
		return false;
    return m_pSDKWrapper->SnExitCalib();
}

int Sn3DSDKDemoApp::videoProcessControl(bool start)
{
	if (m_pSDKWrapper == nullptr)
		return false;
	if(m_pSDKWrapper)
		m_pSDKWrapper->videoProcessControl(start);
}

void Sn3DSDKDemoApp::installGetImagasCallBack(Sn3DGetImagesCallBack imageCallback, void * owner)
{
	if (m_pSDKWrapper == nullptr)
		return;
	m_pSDKWrapper->installGetImagasCallBack(imageCallback, owner);
}

bool Sn3DSDKDemoApp::enbableVideo(bool bEnabled)
{
	if (m_pSDKWrapper == nullptr)
		return false;
    return m_pSDKWrapper->enbableVideo(bEnabled);
}

bool Sn3DSDKDemoApp::switchScanMode(int iScanMode)
{
	if (m_pSDKWrapper == nullptr)
		return false;
	return m_pSDKWrapper->switchScanMode(iScanMode);
}

bool Sn3DSDKDemoApp::changeObject(ObjectType objType)
{
	if (m_pSDKWrapper == nullptr)
		return false;
	return m_pSDKWrapper->changeObject(objType);
}

bool Sn3DSDKDemoApp::enterToPreScan(bool bGeneratedCloud)
{
	if (m_pSDKWrapper == nullptr)
		return false;
	return m_pSDKWrapper->enterToPreScan(bGeneratedCloud);
}

bool Sn3DSDKDemoApp::SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
    const QString& strMsg,
    QString& strResult)
{
	if (m_pSDKWrapper == nullptr)
		return false;
    return m_pSDKWrapper->SnSendMessage(strVersion, strMsgType, strZMQTopic, strMsg, strResult);
}

bool Sn3DSDKDemoApp::SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
    const QVariantMap& mapMsg,
    QString& strResult)
{
	if (m_pSDKWrapper == nullptr)
		return false;
    return m_pSDKWrapper->SnSendMessage(strVersion, strMsgType, strZMQTopic, mapMsg, strResult);
}

int Sn3DSDKDemoApp::connectDevice()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->connectDevice();
	
}

int Sn3DSDKDemoApp::reconnectDevice()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->reconnectDevice();
}

int Sn3DSDKDemoApp::getDeviceIsOnline()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DGetDeviceIsOnline();

}

int Sn3DSDKDemoApp::openProject(const char* projFileName, ScanType scanType)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DOpenProject(projFileName, scanType);
}

int Sn3DSDKDemoApp::rangeDataFinish()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DRangeDataFinish();
}

int Sn3DSDKDemoApp::setBrightness(int level)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetBrightness(level);
}

int Sn3DSDKDemoApp::setMaskBackGround(int level,bool enable)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->setMaskBackGround(level, enable);
}


int Sn3DSDKDemoApp::setDeviceBrightness(LPDeviceBrightness devBrightness)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetDeviceBrightness(devBrightness);

}

int Sn3DSDKDemoApp::setLEDBrightness(int iStep)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetLEDBrightness(iStep);
}

int Sn3DSDKDemoApp::Sn3DSetScanPars(LPScanPars param)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DSetScanPars(param);
}

int Sn3DSDKDemoApp::getBrightnessRange(int &min, int &max)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DGetBrightnessRange(min, max);
}

int Sn3DSDKDemoApp::getCurrentBrightness(int &currentBrightness)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DGetCurrentBrightness(currentBrightness);
}

int Sn3DSDKDemoApp::setEnablePseudoColor(bool enable)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetEnablePseudoColor(enable);
}

int Sn3DSDKDemoApp::setWorkRange(float min, float max)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetWorkRange(min, max);
}

int Sn3DSDKDemoApp::Sn3DCaliSnapImage()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DCaliSnapImage();
}

int Sn3DSDKDemoApp::Sn3DEnterCali()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DEnterCali();
}

int Sn3DSDKDemoApp::Sn3DCaliCalculate()
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DCaliCalculate();
}

int Sn3DSDKDemoApp::enterScan(ScanType scanType)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DEnterScan(scanType);
}

int Sn3DSDKDemoApp::exitScan()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DExitScan();
}

int Sn3DSDKDemoApp::previewScan()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DPreviewScan();
}

int Sn3DSDKDemoApp::pauseScan()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DPauseScan();
}

int Sn3DSDKDemoApp::endScan(bool globalOptimize, double pointDist, Sn3DWholePointCloudCallBack wholeCallback)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DEndScan(globalOptimize,pointDist,wholeCallback);
}

int Sn3DSDKDemoApp::clearScan(bool isCancelCurrentProjectFramerMark)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DClearScan(isCancelCurrentProjectFramerMark);
}

int Sn3DSDKDemoApp::Sn3DMesh(LPMeshPars meshPars, Sn3DMeshDataCallBack callback /*= nullptr*/, void* owner /*= nullptr*/)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DMesh(meshPars, callback,owner);
}

int Sn3DSDKDemoApp::registerSceneDataCallback(Sn3DSceneDataCallBack pfCallBack, void * pUserData)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->registerSceneDataCallback(pfCallBack, pUserData);
}

int Sn3DSDKDemoApp::getSceneData(LPSN3D_SCENE_DATA sceneData)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->getSceneData(sceneData);
}

int Sn3DSDKDemoApp::setWholePointCloudCallBack(Sn3DWholePointCloudCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetWholePointCloudCallBack(callBackFunc, owner);
}

int Sn3DSDKDemoApp::setCurrentPointCloudCallBack(Sn3DCurrentPointCloudCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetCurrentPointCloudCallBack(callBackFunc,owner);
}

int Sn3DSDKDemoApp::setIncreasePointCloudCallBack(Sn3DIncreasePointCloudCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetIncreasePointCloudCallBack(callBackFunc,owner);
}

int Sn3DSDKDemoApp::setCameraPositionCallBack(Sn3DCameraPositionCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetCameraPositionCallBack(callBackFunc,owner);
}

int Sn3DSDKDemoApp::setTrackLostStatusCallBack(Sn3DTrackLostStatusCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetTrackLostStatusCallBack(callBackFunc,owner);
}

int Sn3DSDKDemoApp::setScanDistCallBack(Sn3DScanDistCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetScanDistCallBack(callBackFunc,owner);
}

int Sn3DSDKDemoApp::setDeviceEventCallBack(Sn3DDeviceEventCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetDeviceEventCallBack(callBackFunc,owner);
}

int Sn3DSDKDemoApp::getPointCountCallBack(Sn3DPointCountCallBack callBackFunc, void *owner)
{
     if (m_pSDKWrapper == nullptr)
         return SNSDK_ERR_NOTINITIALIZED;
     return m_pSDKWrapper->Sn3DSetPointCountCallBack(callBackFunc,owner);
    return SNSDK_ERR_NOTINITIALIZED;
}

int Sn3DSDKDemoApp::getFrameRateCallBack(Sn3DFrameRateCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetFrameRateCallBack(callBackFunc,owner);
}

int Sn3DSDKDemoApp::getFrameCountCallBack(Sn3DFrameCountCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetFrameCountCallBack(callBackFunc,owner);
}

int Sn3DSDKDemoApp::setTooFlatStatusCallBack(Sn3DTooFlatStatusCallBack callBackFunc, void *owner)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetTooFlatStatusCallBack(callBackFunc,owner);
}

int Sn3DSDKDemoApp::Sn3DImproFramePoint(QString filePath)
{
	if (m_pSDKWrapper == nullptr)
		return SNSDK_ERR_NOTINITIALIZED;
	return m_pSDKWrapper->Sn3DImproFramePoint(filePath);
}

int Sn3DSDKDemoApp::Sn3DNewProject(LPNewProject newProject)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DNewProject(newProject);
}

int Sn3DSDKDemoApp::Sn3DSetLaserGrade(int index)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetLaserGrade(index);
}

int Sn3DSDKDemoApp::Sn3DEndScan()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DEndScan();
}

int Sn3DSDKDemoApp::Sn3DMesh(LPFreeScanMeshPar pParam)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DMesh(pParam);
}

int Sn3DSDKDemoApp::Sn3DSaveMesh(LPFreeSaveMeshPar pParam)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSaveMesh(pParam);
}

int Sn3DSDKDemoApp::Sn3DConfirmMesh()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DConfirmMesh();
}

int Sn3DSDKDemoApp::Sn3DUndoMesh()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DUndoMesh();
}

int Sn3DSDKDemoApp::Sn3DSetCalibMode(int iMode)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetCalibMode(iMode);
}

int Sn3DSDKDemoApp::Sn3DSetIsIncFramework(bool bInc)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetIsIncFramework(bInc);
}

int Sn3DSDKDemoApp::Sn3DEnterScanPage()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DEnterScanPage();
}

int Sn3DSDKDemoApp::Sn3DChangeBrightStep(int iIndex)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DChangeBrightStep(iIndex);
}

int Sn3DSDKDemoApp::Sn3DSetIsHighSpeed(bool bIsHigh)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetIsHighSpeed(bIsHigh);
}

int Sn3DSDKDemoApp::Sn3DSetScanObject(int iIndex)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetScanObject(iIndex);
}

int Sn3DSDKDemoApp::Sn3DEnterScan(ScanType scanType)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DEnterScan(scanType);
}

int Sn3DSDKDemoApp::Sn3DEnterScanModePage()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DEnterScanModePage();
}

int Sn3DSDKDemoApp::Sn3DSetScanMode(int iMode)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DSetScanMode(iMode);
}

int Sn3DSDKDemoApp::Sn3DCancelScan()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DCancelScan();
}

int Sn3DSDKDemoApp::Sn3DStopCalib(int iMode)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DStopCalib(iMode);
}

int Sn3DSDKDemoApp::Sn3DApplyPostProcess(const QVariantMap param)
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DApplyPostProcess(param);
}

int Sn3DSDKDemoApp::Sn3DConfirmPostProcess()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DConfirmPostProcess();
}

int Sn3DSDKDemoApp::Sn3DCancelPostProcess()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DCancelPostProcess();
}

int Sn3DSDKDemoApp::Sn3DRollbackPostProcess()
{
    if (m_pSDKWrapper == nullptr)
        return SNSDK_ERR_NOTINITIALIZED;
    return m_pSDKWrapper->Sn3DRollbackPostProcess();
}

int Sn3DSDKDemoApp::Sn3DInitializeModbus(int iDeviceType)
{
	if (SNSDK_DEVTYPE_5M == iDeviceType || SNSDK_DEVTYPE_9M == iDeviceType
		|| SNSDK_DEVTYPE_E10 == iDeviceType) {
		m_pSDKWrapper = new SDKWrapperOptimScan();
	}
	else if (SNSDK_DEVTYPE_UE == iDeviceType || SNSDK_DEVTYPE_UEPRO == iDeviceType || SNSDK_DEVTYPE_UEP2 == iDeviceType
		|| SNSDK_DEVTYPE_COMBO == iDeviceType || SNSDK_DEVTYPE_COMBOPLUS == iDeviceType || SNSDK_DEVTYPE_TRIO == iDeviceType) {
		m_pSDKWrapper = new SDKWrapperFreeScan();
	}
	connect(m_pSDKWrapper, &SDKWrapperBase::showMsg, this, &Sn3DSDKDemoApp::showMsg);

	return m_pSDKWrapper->Sn3DInitializeModbus(iDeviceType);
}

int Sn3DSDKDemoApp::Sn3DReleaseModbus()
{
	if (!m_pSDKWrapper)
		return 0;
	disconnect(m_pSDKWrapper, &SDKWrapperBase::showMsg, this, &Sn3DSDKDemoApp::showMsg);
	auto ret = m_pSDKWrapper->Sn3DReleaseModbus();
	delete m_pSDKWrapper;
	m_pSDKWrapper = nullptr;
	return ret;
}