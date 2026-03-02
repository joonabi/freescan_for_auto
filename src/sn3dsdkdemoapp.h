#pragma once
#include <qapplication.h>
#include "qobject.h"
#include "sdkwrapperbase.h"

class Sn3DSDKDemoApp :
    public QApplication
{
    Q_OBJECT
public:
#ifdef Q_QDOC
    Sn3DSDKDemoApp(int &argc, char **argv);
#else
    Sn3DSDKDemoApp(int &argc, char **argv, int = ApplicationFlags);
#endif
    virtual ~Sn3DSDKDemoApp();
    // virtual bool createService(int deviceType); // sn3dsdk_c_wrapper::DeviceType
    virtual int Sn3DInitialize(int iDeviceType);
    virtual int Sn3DInitializeSocket(int iDeviceType);
	/**
	*@brief Initialize modbus business environment
	*/
	virtual int Sn3DInitializeModbus(int iDeviceType);
    virtual int Sn3DInitialLog(const QString& strExeName, const QString& strPath);
    // virtual bool destroyService();
    virtual int Sn3DRecleaseSocket();
    virtual int Sn3DRelease();
	/**
	*@brief Release modbus business environment
	*/
	virtual int Sn3DReleaseModbus();
    virtual int Sn3DConnectSocket(const QString& ip, int port);
    virtual int Sn3DDisconnectSocket();
    virtual int Sn3DSendSocket(const QString& str);

    // virtual bool closeService();
    // virtual bool newProject(const QString& strFileDir);
    virtual int Sn3DOpenOrCreateSolution(
        const QString& strSlnDirPath,
        bool isCreate,
        int iScanMode,
        bool hasTexture,
        float fPointDis);
	virtual int Sn3DOpenOrCreateSolution(const QString& strSlnDirPath,
		bool isCreate,
		int iScanMode,
		bool hasTexture,
		float fPointDis,
		int iAlignType, 
		bool useNonrigid,
		int	iNeedLimit);

    virtual int Sn3DNewProject(
        const QString& strSlnDirPath,
        int iScanMode,
        bool hasTexture,
        float fPointDis,
        int iAlignType,
        const QString& strGlobalMarkerPath);
	virtual bool SnNewProject(const QString& strFileDir);
    virtual int Sn3DNewProject(LPNewProject newProject);
	virtual int Sn3DLoadP3File(const QString& p3Path);
    // virtual bool scanModeChanged(int scanMode); // sn3dsdk_c_wrapper::ScanMode
    virtual int SnChangeScanMode(int iScanMode); // sn3dsdk_c_wrapper::ScanMode
    // virtual bool scan(); .. bool->int
    virtual int Sn3DStartScan();
    // virtual int endScan();
    virtual int Sn3DPauseScan();
	virtual int Sn3DContinueScan();

    virtual int Sn3DGlobalOptimization();
	virtual int Sn3DPointFeatureAlign(LPPointFeatureAlign pointFeatureAlign, float* rot, float* tran);
	virtual int Sn3DPointManualAlign(LPPointManualAlign pointManualAlign, float* rot, float* tran);
	bool changeLight(int iStep, int iLaserNum);
	// 发送激光线数
	bool sendLaserNum(int iLaserNum);
	/**
	*@brief 开视频流
	*/
	bool enbableVideo(bool bEnabled);
	/**
	*@brief
	*@param iScanMode[in] 点云扫描0，框架点扫描1，摄影测量模式3
	*/
	bool switchScanMode(int iScanMode);
	/**
	*@brief 设置扫描物体类型
	*@param objType[in] 0:正常 1:黑色 2:反光
	*/
	bool changeObject(ObjectType objType);
	bool enterToPreScan(bool bGeneratedCloud);
    /**
    *@brief save point cloud(*.asc)
    */
    virtual bool saveASC(const QString& strFolder);
    int Sn3DSaveData(const QString& strAbsolutePrjName, const QString& strSavePath, const QString& strSaveType);
	/*
		*@brief 网格处理
		*@note  对应客户端扫描暂停时点击右侧进入封装(是否优化)
		*/
	virtual int Sn3DMeshPostProcess(const bool&optimized);
    /**
    *@brief end scan(pause scan)
    */
    // virtual bool enterToScanPause();
    /**
    *@brief 设置扫描物体类型
    *@param objType[in] 0:正常 1:黑色 2:反光
    */
    // virtual bool changeObject(ObjectType objType);
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
	/**
    *@brief 开启/关闭视频获取
    */
	int videoProcessControl(bool start);
	/**
	*@brief 注册视频回调
	*/
	void installGetImagasCallBack(Sn3DGetImagesCallBack imageCallback, void* owner);
    /**
    *@brief Send Message
    */
    bool SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
        const QString& strMsg,
        QString& strResult);
    /**
    *@brief Send Message
    */
    bool SnSendMessage(const QString& strVersion, const QString strMsgType, const QString& strZMQTopic,
        const QVariantMap& mapMsg,
        QString& strResult);

	/**
	*@brief 连接设备
	*/
	int connectDevice();
	/**
	*@brief 重连设备
	*/

	int reconnectDevice();

    /**
    *@brief 设备在线
    */

    int getDeviceIsOnline();
    /**
    *@brief 打开工程
    */
    int openProject(const char* projFileName, ScanType scanType);
	/**
	*@brief 单片扫描数据读取完成
	*/
	int rangeDataFinish();
    /**
    *@brief 设置设备亮度
    */
    int setBrightness(int level);
	/**
	*@brief 设置设备亮度
	*/
	int setMaskBackGround(int level, bool enable);
    /**
    *@brief 设置设备亮度（曝光、增益）
    */
    int setDeviceBrightness(LPDeviceBrightness devBrightness);
    /**
    *@brief 设置LED亮度
    */
    int setLEDBrightness(int iStep);
    /**
	*@brief 设置扫描参数
	*/
	int Sn3DSetScanPars(LPScanPars param);
    /**
    *@brief 获取设备亮度范围
    */
    int getBrightnessRange(int& min, int& max);
    /**
    *@brief 进入扫描
    */
    int enterScan(ScanType scanType);
    /**
    *@brief 退出扫描
    */
    int exitScan();
    /**
    *@brief 预扫描
    */
    int previewScan();
    /**
    *@brief 暂停扫描
    */
    int pauseScan();
    /**
    *@brief 结束扫描
    */
    int endScan(bool globalOptimize,double pointDist,Sn3DWholePointCloudCallBack wholeCallback);

    /**
    *@brief 清空扫描数据
    */
    int clearScan(bool isCancelCurrentProjectFramerMark);

    /**
    *@brief 获取设备当前亮度
    */
    int getCurrentBrightness(int& currentBrightness);
    /**
    *@brief 开启伪彩色
    */
    int setEnablePseudoColor(bool enable);
    /**
    *@brief 设置扫描工作范围
    */
    int setWorkRange(float min, float max);
    /**
    *@brief 网格化封装数据
    */
    int Sn3DMesh(LPMeshPars meshPars, Sn3DMeshDataCallBack callback = nullptr, void* owner = nullptr);
	
	/**
	*@brief 注册场景数据更新回调
	*/
	virtual int registerSceneDataCallback(Sn3DSceneDataCallBack pfCallBack, void* pUserData);

	/**
	*@brief 获取场景数据 调两次 用户分配内存
	*/
	virtual int getSceneData(LPSN3D_SCENE_DATA sceneData);

    /**
    *@brief 注册整体点云回调
    */
    int setWholePointCloudCallBack(Sn3DWholePointCloudCallBack callBackFunc, void* owner);
    /**
    *@brief 注册当前帧点云回调
    */
    int setCurrentPointCloudCallBack(Sn3DCurrentPointCloudCallBack callBackFunc, void* owner);
    /**
    *@brief 注册增量点云回调
    */
    int setIncreasePointCloudCallBack(Sn3DIncreasePointCloudCallBack callBackFunc, void* owner);
    /**
    *@brief 注册相机视角回调
    */
    int setCameraPositionCallBack(Sn3DCameraPositionCallBack callBackFunc, void* owner);
    /**
    *@brief 注册跟踪丢失回调
    */
    int setTrackLostStatusCallBack(Sn3DTrackLostStatusCallBack callBackFunc, void* owner);
    /**
    *@brief 注册扫描距离回调
    */
    int setScanDistCallBack(Sn3DScanDistCallBack callBackFunc, void* owner);
    /**
    *@brief 注册设备按键操作事件回调
    */
    int setDeviceEventCallBack(Sn3DDeviceEventCallBack callBackFunc, void* owner);
    /**
    *@brief 注册点云数量回调
    */
    int getPointCountCallBack(Sn3DPointCountCallBack callBackFunc, void* owner);
    /**
    *@brief 注册帧率回调
    */
    int getFrameRateCallBack(Sn3DFrameRateCallBack callBackFunc, void* owner);
    /**
    *@brief 注册帧数回调
    */
    int getFrameCountCallBack(Sn3DFrameCountCallBack callBackFunc, void* owner);
    /**
    *@brief 注册平面检测回调
    */
    int setTooFlatStatusCallBack(Sn3DTooFlatStatusCallBack callBackFunc, void* owner);
	/**
	*@brief 进入标定
	*/
	int Sn3DEnterCali();

	/**
	*@brief 标定采图
	*/
	int Sn3DCaliSnapImage();

	/**
	*@brief 标定计算
	*/
	int Sn3DCaliCalculate();

	/**
	*@brief 导入框架点
	*/
	int Sn3DImproFramePoint(QString filePath);
    /**
    *@brief set laser grade
    *@param index [in] grade index
    */
    int Sn3DSetLaserGrade(int index);
    /**
    *@brief end scan for FreeScan ONLY
    */
    int Sn3DEndScan();
    /**
    *@brief freescan mesh
    */
    int Sn3DMesh(LPFreeScanMeshPar pParam);
    /**
    *brief freescan save mesh
    */
    int Sn3DSaveMesh(LPFreeSaveMeshPar pParam);
    /**
    *@brief freescan confirm mesh
    */
    int Sn3DConfirmMesh();
    /**
    *brief freescan undo mesh
    */
    int Sn3DUndoMesh();
    /**
    *@brief freescan set calibration mode
    */
    int Sn3DSetCalibMode(int iMode);
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
    *@brief 收到消息
    */
    void SnMessageReceived(QVariantMap mapParam);
    /**
    *@brief 设备状态发生改变信号
    */
    void deviceStatusChanged(LPDeviceStatus deviceStatus); // sn3dsdk_c_wrapper::DeviceStatus
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

    void pointCountChanged(int pointCount);

    void frameCountChanged(int frameCount);

    void frameRateChanged(int frameRate);

    void cameraPositionChanged(LPSn3dCameraPosition cameraPosition);
    
    void trackLostStatusChanged(bool trackLost);
    void scanDistChanged(double dis);
    void deviceEventChanged(DeviceEvent event);
    void tooFlatStatusChanged(bool tooFlat);
    void showMsg(QString msg, int type);

    void postProcessApplied();
    void postProcessConfirmed();
    void postProcessCanceled();
    void postProcessRollbacked();
	void flushRangeData(LPSn3dRangeData rangeData);
	void revealProjectInfo(LPSn3dProjectInfo projectInfo);
private:
    SDKWrapperBase* m_pSDKWrapper = nullptr;
};

