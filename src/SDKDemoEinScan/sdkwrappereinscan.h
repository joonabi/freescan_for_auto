#ifndef SDKWRAPPEREINSCAN_H
#define SDKWRAPPEREINSCAN_H

#include <sdkwrapperbase.h>

class SDKWrapperEinScan : public SDKWrapperBase
{
public:
    explicit SDKWrapperEinScan(QObject *parent = nullptr);
	virtual ~SDKWrapperEinScan();

    virtual int Sn3DInitialize(int iDeviceType);
    virtual int Sn3DRelease();
    //新建或打开解决方案
	virtual int Sn3DOpenOrCreateSolution(const QString& strSlnDirPath,
		bool isCreate,
		int iScanMode,
		bool hasTexture,
		float fPointDis,
		int iAlignType,
		bool useNonrigid,
		int	iNeedLimit) override;

	//新建工程
	virtual int Sn3DNewProject(
		const QString& strSlnDirPath,
		int iScanMode,
		bool hasTexture,
		float fPointDis,
		int iAlignType,
		const QString& strGlobalMarkerPath);
	virtual int Sn3DOpenProject(const char* projFileName, ScanType scanType) override;
    //开始扫描
    virtual int Sn3DStartScan() override;
	//暂停扫描
	virtual int Sn3DPauseScan() override;
	//继续扫描
	virtual int Sn3DContinueScan() override;
    //保存数据
    virtual int Sn3DSaveData(const QString& strAbsolutePrjName, const QString& strSavePath, const QString& strSaveType) override;
};

#endif // SDKWRAPPEREINSCAN_H
