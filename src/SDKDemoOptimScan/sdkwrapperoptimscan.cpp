#include "sdkwrapperoptimscan.h"

SDKWrapperOptimScan::SDKWrapperOptimScan(QObject* parent)
	: SDKWrapperBase(parent)
{

}
SDKWrapperOptimScan::~SDKWrapperOptimScan()
{

}

int SDKWrapperOptimScan::Sn3DInitialize(int iDeviceType)
{
	return SDKWrapperBase::Sn3DInitialize(iDeviceType);
}

int SDKWrapperOptimScan::Sn3DInitializeSocket(int iDeviceType)
{
    return SDKWrapperBase::Sn3DInitializeSocket(iDeviceType);
}

int SDKWrapperOptimScan::Sn3DRelease()
{
	return SDKWrapperBase::Sn3DRelease();
}

int SDKWrapperOptimScan::Sn3DReleaseSocket()
{
    return SDKWrapperBase::Sn3DReleaseSocket();
}

void SDKWrapperOptimScan::CallBack(int iEventType, int iExtendData, void* pData, size_t stDataLen)
{
	SDKWrapperBase::CallBack(iEventType, iExtendData, pData, stDataLen);
}

int SDKWrapperOptimScan::connectDevice()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DConnectDevice(m_hService);//Sn3DReConnectDevice
}

void SDKWrapperOptimScan::closeDevice()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return;
	::Sn3DCloseDevice(m_hService);
}


int SDKWrapperOptimScan::reconnectDevice()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	return ::Sn3DReConnectDevice(m_hService);
}

int SDKWrapperOptimScan::Sn3DOpenOrCreateSolution(
	const QString& strSlnDirPath,
	bool isCreate,
	int iScanMode,
	bool hasTexture,
	float fPointDis)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;

    LPOpenOrCreateSln param = new OpenOrCreateSln();
    char* path = new char[strSlnDirPath.length() + 1];
	memcpy(path, strSlnDirPath.toUtf8().data(), strSlnDirPath.length() + 1);
    param->szSlnDirPath = path;
    param->isCreate = isCreate;
    param->iScanMode = iScanMode;
    param->hasTexture = hasTexture;
    param->fPointDis = fPointDis;
    int ret = ::Sn3DOpenOrCreateSolution(m_hService, param);

    delete param; 
    param = nullptr;
    delete[] path;
    path = nullptr;
    return ret;
}
// bool
int SDKWrapperOptimScan::Sn3DStartScan()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return SNSDK_ERR_NOTINITIALIZED;
  
	int ret = ::Sn3DStartScan(m_hService);
    return ret;
}

int SDKWrapperOptimScan::Sn3DPauseScan()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return SNSDK_ERR_NOTINITIALIZED;
	return ::Sn3DPauseScan(m_hService);
}

int SDKWrapperOptimScan::Sn3DSaveData(const QString& strAbsolutePrjName, const QString& strSavePath, const QString& strSaveType)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return SNSDK_ERR_NOTINITIALIZED;
    LPSaveData param = new SaveData();

    auto strAbsolutePrjName_ = strAbsolutePrjName.toUtf8();
    auto strSavePath_ = strSavePath.toUtf8();
    auto strSaveType_ = strSaveType.toUtf8();
    
    param->absolutePrjName = strAbsolutePrjName_.data();
    param->savePath = strSavePath_.data();
    param->saveType = strSaveType_.data();
    int ret = ::Sn3DSaveData(m_hService, param);
    delete param;
    param = nullptr;
    return ret;
}

int SDKWrapperOptimScan::Sn3DCaliSnapImage()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return SNSDK_ERR_NOTINITIALIZED;
	return ::Sn3DCaliSnapImage(m_hService);
}

int SDKWrapperOptimScan::Sn3DCaliCalculate()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return SNSDK_ERR_NOTINITIALIZED;
	return ::Sn3DCaliCalculate(m_hService);
}

int SDKWrapperOptimScan::Sn3DEnterCali()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return SNSDK_ERR_NOTINITIALIZED;
	return ::Sn3DEnterCali(m_hService);
}

int SDKWrapperOptimScan::Sn3DImproFramePoint(QString filePath)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return SNSDK_ERR_NOTINITIALIZED;
	return ::Sn3DImproFramePoint(m_hService,filePath.toUtf8().constData());
}