#pragma once
#include "sdkwrapperbase.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QException>
#include "glog/logging.h"

class SDKWrapperOptimScan :
    public SDKWrapperBase
{
public:
    SDKWrapperOptimScan(QObject* parent = nullptr);
    virtual ~SDKWrapperOptimScan() override;
    virtual int Sn3DInitialize(int iDeviceType) override;
    virtual int Sn3DInitializeSocket(int iDeviceType);
    virtual int Sn3DRelease() override;
    virtual int Sn3DReleaseSocket();

	/*!
	* @brief 新建或打开解决方案			New or open solutions
	* @param slnDirPath 解决方案路径	Solution path
	* @param isCreate true新建, false打开	“true”- new, “false”-open
	* @param scanMode 扫描模式, 0 portrait, 1 object, 2 smallobject. Reference: ScanMode
								Scan mode, 0 portrait, 1 object, 2 smallobject. Reference: ScanMode
	* @param hasTexture true纹理扫描, false不带纹理		“true”- texture, “false”-no texture
	* @param pointDis 点距		Point distance
	* @return 0成功，失败参考错误码
	*/
	virtual int Sn3DOpenOrCreateSolution(
		const QString& strSlnDirPath, 
		bool isCreate, 
		int iScanMode, 
		bool hasTexture, 
		float fPointDis) 
		override;

	void CallBack(int iEventType, int iExtendData, void* pData, size_t stDataLen) override;
	//(bool)
	int	Sn3DStartScan() override;
	int Sn3DPauseScan() override;
	int Sn3DSaveData(const QString& strAbsolutePrjName, const QString& strSavePath, const QString& strSaveType) override;
	int connectDevice() override;
	int reconnectDevice() override;
	void closeDevice() override;

	/**
	*@brief 标定采图
	*/
	virtual int Sn3DCaliSnapImage();

	/**
	*@brief 标定采图
	*/
	virtual int Sn3DCaliCalculate();

	/**
	*@brief 进入标定
	*/
	virtual int Sn3DEnterCali();

	/**
	*@brief 导入框架点文件
	*/
	virtual int Sn3DImproFramePoint(QString filePath);
};

