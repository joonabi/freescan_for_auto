#include "sdkwrappereinscan.h"

SDKWrapperEinScan::SDKWrapperEinScan(QObject *parent)
    : SDKWrapperBase{parent}
{

}

SDKWrapperEinScan::~SDKWrapperEinScan()
{
}

int SDKWrapperEinScan::Sn3DInitialize(int iDeviceType)
{
	return SDKWrapperBase::Sn3DInitialize(iDeviceType);
}



int SDKWrapperEinScan::Sn3DRelease()
{
    return SDKWrapperBase::Sn3DRelease();
}

//新建或打开解决方案
int SDKWrapperEinScan::Sn3DOpenOrCreateSolution(const QString & strSlnDirPath, bool isCreate, int iScanMode, bool hasTexture, float fPointDis, int iAlignType, bool useNonrigid, int iNeedLimit)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
	LPOpenOrCreateSln param = new OpenOrCreateSln();
	auto path = strSlnDirPath.toUtf8();
	param->szSlnDirPath = path.constData();
	param->isCreate = isCreate;
	param->iScanMode = iScanMode;
	param->hasTexture = hasTexture;
	param->fPointDis = fPointDis;
	param->iAlignType = iAlignType;
	param->useNonrigid = useNonrigid;
	param->iNeedLimit = iNeedLimit;
	//return ::Sn3DOpenOrCreateSolution(m_hService, param);
	int ret = ::Sn3DOpenOrCreateSolution(m_hService, param);
	delete param;
	param = nullptr;
	return ret;
}

int SDKWrapperEinScan::Sn3DNewProject(const QString & strSlnDirPath, int iScanMode, bool hasTexture, float fPointDis, int iAlignType, const QString & strGlobalMarkerPath)
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	NewProject param;
	param.szSlnDirPath = strSlnDirPath.toUtf8().constData();
	param.iScanMode = iScanMode;
	param.hasTexture = hasTexture;
	param.fPointDis = fPointDis;
	param.iAlignType = iAlignType;
	param.szGlobalMarkerPath = strGlobalMarkerPath.toUtf8().constData();
	return ::Sn3DNewProject(m_hService, &param);
}
//打开工程
int SDKWrapperEinScan::Sn3DOpenProject(const char* projFileName, ScanType scanType)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    return ::Sn3DOpenProject(m_hService, projFileName, scanType);
}
//开始扫描
int SDKWrapperEinScan::Sn3DStartScan()
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return INVALID_HSN3DSDKSERVICE_VALUE;
    int ret = ::Sn3DStartScan(m_hService);
    return ret;
}
int SDKWrapperEinScan::Sn3DPauseScan()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	int ret = ::Sn3DPauseScan(m_hService);
	return ret;
}

int SDKWrapperEinScan::Sn3DContinueScan()
{
	if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
		return INVALID_HSN3DSDKSERVICE_VALUE;
	int ret = ::Sn3DContinueScan(m_hService);
	return ret;
}

//保存数据
int SDKWrapperEinScan::Sn3DSaveData(const QString &strAbsolutePrjName, const QString &strSavePath, const QString &strSaveType)
{
    if (m_hService == INVALID_HSN3DSDKSERVICE_VALUE)
        return SNSDK_ERR_NOTINITIALIZED;
    SaveData param;
    auto strAbsolutePrjName_ = strAbsolutePrjName.toUtf8();
    auto strSavePath_ = strSavePath.toUtf8();
    auto strSaveType_ = strSaveType.toUtf8();
    param.absolutePrjName = strAbsolutePrjName_.data();
    param.savePath = strSavePath_.data();
    param.saveType = strSaveType_.data();

    return ::Sn3DSaveData(m_hService, &param);
}

