#pragma once
#include "windows.h"
#include "sdkwrapperbase.h"
#include "scanresultfile.h"

// enum CalibMode{QuickCalib, FactoryCalib};
class SDKWrapperFreeScan : public SDKWrapperBase
{
    Q_OBJECT
public:
    SDKWrapperFreeScan(QObject* parent = nullptr);
    virtual ~SDKWrapperFreeScan();
    // bool SnCreateService(int deviceType); // sn3dsdk_c_wrapper::DeviceType
    virtual int Sn3DInitialize(int iDeviceType);
    virtual int Sn3DRelease();
    /*bool SnOpenService();
    bool SnCloseService();
    bool SnOpenDevice();
    bool SnCloseDevice();*/
    virtual bool SnNewProject(const QString& strFileDir);
    virtual int Sn3DNewProject(LPNewProject newProject);
    virtual bool SnScanModeChanged(int scanMode); // sn3dsdk_c_wrapper::ScanMode
    virtual int Sn3DOpenOrCreateSolution(
        const QString& strSlnDirPath,
        bool isCreate,
        int iScanMode,
        bool hasTexture,
        float fPointDis);
    virtual int Sn3DNewProject(
        const QString& strSlnDirPath,
        int iScanMode,
        bool hasTexture,
        float fPointDis,
        int iAlignType,
        const QString& strGlobalMarkerPath);
	// bool
    virtual int Sn3DStartScan();
    virtual bool SnSetLEDBrightness(int iStep);
    virtual void CallBack(int iEventType, int iExtendData, void* pData, size_t stDataLen);
    virtual bool SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
        const QString& strMsg,
        QString& strResult);
    virtual bool SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
        const QVariantMap& mapMsg,
        QString& strResult);
    // 发送激光线数
    virtual bool sendLaserNum(int iLaserNum);
    
    // bool endScan();
    virtual bool changeLight(int iStep, int iLaserNum);
    virtual bool enterToPreScan(bool bGeneratedCloud);
    /**
    *@brief
    *@param iScanMode[in] 点云扫描0，框架点扫描1，摄影测量模式3
    */
    virtual bool switchScanMode(int iScanMode);
    // bool enterToScan();
    /**
    *@brief save point cloud(*.asc)
    */
    virtual bool saveASC(const QString& strFolder);
    /**
    *@brief save point cloud(*.asc)
    */
    virtual int Sn3DSaveData(const QString& strAbsolutePrjName, const QString& strSavePath, const QString& strSaveType);
    /**
    *@brief end scan(pause scan)
    */
    // bool enterToScanPause();
    virtual int Sn3DPauseScan();
    /**
    *@brief 设置扫描物体类型
    *@param objType[in] 0:正常 1:黑色 2:反光
    */
    virtual bool changeObject(ObjectType objType);
    /**
    *@brief 开视频流
    */
    virtual bool enbableVideo(bool bEnabled);
    /**
    *@brief 进入标定
    */
    virtual bool SnChangeCalibMode(int iMode);
    /**
    *@brief 进入标定
    */
    virtual bool SnEnterCalib();
    /**
    *@brief 退出标定
    */
    virtual bool SnExitCalib();
	/*
	*@brief 网格处理
	*@note  对应客户端扫描暂停时点击右侧进入封装(是否优化)
	*/
	virtual int Sn3DMeshPostProcess(const bool&optimized);
	/**
    *@brief 开启/关闭视频获取
    */
	virtual int videoProcessControl(const bool&start)override;
	/**
	*@brief 注册视频回调
	*/
	virtual void installGetImagasCallBack(Sn3DGetImagesCallBack imageCallback, void* owner)override;

	/**
	*@brief 注册场景数据更新回调
	*/
	virtual int registerSceneDataCallback(Sn3DSceneDataCallBack pfCallBack, void* pUserData)override;

	/**
	*@brief 获取场景数据 调两次 用户分配内存
	*/
	virtual int getSceneData(LPSN3D_SCENE_DATA sceneData)override;
	/**
	*@brief 打开工程
	*/
	virtual int Sn3DOpenProject(const char* projFileName, ScanType scanType)override;
    /**
    *@brief Start capture image for calibration
    */
    virtual int Sn3DCaliSnapImage() override;
    /**
    *@brief set laser grade
    *@param index [in] grade index
    */
    virtual int Sn3DSetLaserGrade(int index);
    /**
    *@brief end scan for FreeScan ONLY
    */
    virtual int Sn3DEndScan();
    /**
    *@brief freescan mesh
    */
    virtual int Sn3DMesh(LPFreeScanMeshPar pParam);
    /**
    *brief freescan save mesh
    */
    virtual int Sn3DSaveMesh(LPFreeSaveMeshPar pParam);
    /**
    *@brief freescan confirm mesh
    */
    virtual int Sn3DConfirmMesh();
    /**
    *brief freescan undo mesh
    */
    virtual int Sn3DUndoMesh();
    /**
    *@brief freescan set calibration mode
    */
    virtual int Sn3DSetCalibMode(int iMode);
    /**
    *@brief freescan set is increase framework mode
    */
    virtual int Sn3DSetIsIncFramework(bool bInc);
    /**
    *@brief freescan enter scan page
    */
    virtual int Sn3DEnterScanPage();
    /**
    *@brief freescan change brightness
    *@param iIndex[in] brightness index in the scan panel
    */
    virtual int Sn3DChangeBrightStep(int iIndex);
    /**
    *@brief freescan set is high speed scan
    *@brief bIsHigh [in] true means high speed mode
    */
    virtual int Sn3DSetIsHighSpeed(bool bIsHigh);
    /**
    *@brief freescan set scan object type
    *@param iIndex[in] object index in the scan panel
    */
    virtual int Sn3DSetScanObject(int iIndex);
    /**
    *@brief 进入扫描
    */
    virtual int Sn3DEnterScan(ScanType scanType);
    /**
    *@brief freescan enter scan mode page
    */
    virtual int Sn3DEnterScanModePage();
    /**
    *@brief freescan set scan mode(Laser or Inflared)
    */
    virtual int Sn3DSetScanMode(int iMode);
    /**
    *@brief freescan cancel scan, used for clean scan result
    */
    virtual int Sn3DCancelScan();
    /**
    *@brief freescan stop calib
    */
    virtual int Sn3DStopCalib(int iMode);
    /**
    *@brief post process
    */
    virtual int Sn3DApplyPostProcess(const QVariantMap param);
    virtual int Sn3DConfirmPostProcess();
    virtual int Sn3DCancelPostProcess();
    virtual int Sn3DRollbackPostProcess();
Q_SIGNALS:
    /**
    *@brief 设备状态发生改变信号
    */
    void deviceStatusChanged(int deviceStatus); // sn3dsdk_c_wrapper::DeviceStatus
    /**
    *@brief 新建工程完成信号
    */
    void newProjFinished(QStringList projNameList);
    /**
    *@brief 保存扫描数据完成信号
    */
    void saveScanDataReturn();
	/**
	*@brief 封装完成信号
	*/
    void meshDataReturn(bool);
    /**
	/**
	*@brief 保存封装数据完成信号
	*/
    void saveMeshDataReturn(bool);
	/**
    *@brief 暂停扫描完成信号
    */
    void updateStateWhenStopFusion();
    /**
    *@brief 进度条信号
    */
    void showProgress(QString strTitle, double dValue, bool bVisible);
    /**
    *@brief 初始化标定完成
    */
    void initCalibParamFinished();
    /**
    *@brief 标定成功1个格子
    */
    void calibHeightOKChanged(int iOKHeight);
    /**
    *@brief 标定组切换
    */
    void groupIndexChanged(int index);
    /**
    *@brief 主窗口初始化完成
    */
    void showMaximized();
private:
    ScanResultFile m_scanResultFile;
};

