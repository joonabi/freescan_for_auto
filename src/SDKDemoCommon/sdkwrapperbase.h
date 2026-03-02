#pragma once
#include "sockethelper.h"
#include <qobject.h>
#include "windows.h"
#include "SDKCWrapper.h"
#include "sdksocketwrapper.h"
#include "scanresultfile.h"
#include <QMetaType>
#include <atomic>

enum ObjectType { Normal, Black, Reflect };
Q_DECLARE_METATYPE(LPSn3dCameraPosition)
Q_DECLARE_METATYPE(DeviceEvent)
Q_DECLARE_METATYPE(LPSn3dPointData)
Q_DECLARE_METATYPE(LPSn3dRangeData)
Q_DECLARE_METATYPE(LPSn3dProjectInfo)


class SDKWrapperBase : public QObject
{
    Q_OBJECT
    enum LogType
    {
        SEND_MESSAGE,				//send to service
        RECEIVE_MESSAGE,			//receive message from service,only correct result
        ERROR_MESSAGE,				//receive message from service,only incorrect result
        INTERAL_LOG					//interal log
    };
public:
    SDKWrapperBase(QObject* parent = nullptr);
    virtual ~SDKWrapperBase();
    // bool SnCreateService(int deviceType); // sn3dsdk_c_wrapper::DeviceType
    virtual int Sn3DInitialize(int iDeviceType);
    virtual int Sn3DInitializeSocket(int iDeviceType);
	/**
	*@brief Initialize modbus business environment
	*/
	virtual int Sn3DInitializeModbus(int iDeviceType);
    virtual int Sn3DRelease();
    virtual int Sn3DReleaseSocket();
	/**
	*@brief Release modbus business environment
	*/
	virtual int Sn3DReleaseModbus();
    virtual int Sn3DConnectSocket(const QString& ip, int port);
    virtual int Sn3DDisconnectSocket();
    virtual int Sn3DSendSocket(const QString& str);
    virtual int Sn3DReceiveSocket();


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
	/**
	*@brief 加载框架点文件 (qjw 新增)
	*/
	virtual int Sn3DLoadP3File(const QString& p3Path);
    /**
    *@brief 进入扫描
    */
    virtual int Sn3DEnterScan(ScanType scanType);
    /**
    *@brief 退出扫描
    */
    virtual int Sn3DExitScan();
	/**
	*@brief 退出扫描 bool->int
	*@return 0成功，失败参考错误码
	*/
    virtual int Sn3DStartScan();


    /*!
    * @brief 设置扫描参数，仅固定扫描支持
    * @return 0成功，失败参考错误码
    */
    virtual int Sn3DSetScanPars(LPScanPars param);
    /**
    *@brief 预扫描
    */
    virtual int Sn3DPreviewScan();
    /**
    *@brief 结束扫描
    */
    virtual int Sn3DEndScan(bool globalOptimize,double pointDist,Sn3DWholePointCloudCallBack wholeCallback = nullptr);


    /**
    *@brief 清空扫描数据
    */
    virtual int Sn3DClearScan(bool isCancelCurrentProjectFramerMark);
    /*!
    * @brief 全局优化，仅固定扫描支持
    * @return 0成功，失败参考错误码
    */
    virtual int Sn3DGlobalOptimization();

	virtual int Sn3DPointFeatureAlign(LPPointFeatureAlign pointFeatureAlign, float* rot, float* tran);

	virtual int Sn3DPointManualAlign(LPPointManualAlign pointManualAlign, float* rot, float* tran);

    virtual int Sn3DMesh(LPMeshPars meshPars, Sn3DMeshDataCallBack callback = nullptr, void* owner = nullptr);
    virtual bool SnSetLEDBrightness(int iStep);
    virtual void CallBack(int iEventType, int iExtendData, void* pData, size_t stDataLen);
    //virtual void deviceStatusCallBack(bool offline);
	virtual void deviceStatusCallBack(LPDeviceStatus deviceStatus);
    virtual void setPointCount(int pointCount);
	virtual void flushRangeData(LPSn3dRangeData rangeData);
	virtual void flushProjectInfo(LPSn3dProjectInfo projectInfo);
    virtual void setFrameRate(int frameRate);
    virtual void setFrameCount(int frameCount);
    virtual void setCameraPosition(LPSn3dCameraPosition cameraPosition);
    virtual void setTrackLost(bool trackLost);
    virtual void setScanDis(double dis);
    virtual void setDeviceEvent(DeviceEvent event);
    virtual void setTooFlatStatus(bool tooFlat);
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
    /**
    *@brief 暂停扫描
    */
    virtual int Sn3DPauseScan();
	/**
	*@brief 继续扫描
	*/
	virtual int Sn3DContinueScan();

	
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
	/*
	*@brief 开启/关闭视频获取
	*/
	virtual int videoProcessControl(const bool&start);
	/**
	*@brief 注册视频回调
	*/
	virtual void installGetImagasCallBack(Sn3DGetImagesCallBack imageCallback, void* owner);
	/**
	*@brief 连接设备
	*/
	virtual int connectDevice();
	/**
	*@brief 重连设备
	*/
	virtual int reconnectDevice();
	/**
	*@brief 关闭设备
	*/
	virtual void closeDevice() {};
	/**
	*@brief 获取设备是否在线
	*/
	virtual int Sn3DGetDeviceIsOnline();
	/**
    *@brief 打开工程
    */
    virtual int Sn3DOpenProject(const char* projFileName, ScanType scanType);
	/**
	*@brief 单片扫描数据读取完成
	*/
	virtual int Sn3DRangeDataFinish();
    /**
    *@brief 设置设备亮度
    */
    virtual int Sn3DSetBrightness(int level);
	/**
	*@brief 设置背景屏蔽
	*/
	virtual int setMaskBackGround(int level, bool enable);
    /**
    *@brief 设置设备亮度（曝光、增益）
    */
    virtual int Sn3DSetDeviceBrightness(LPDeviceBrightness devBrightness);
    /**
    *@brief 设置LED亮度
    */
    virtual int Sn3DSetLEDBrightness(int iStep);
    /**
    *@brief 获取设备亮度范围
    */
    virtual int Sn3DGetBrightnessRange(int& min, int& max);
    /**
    *@brief 获取设备当前亮度
    */
    virtual int Sn3DGetCurrentBrightness(int& currentBrightness);
    /**
    *@brief 开启伪彩色
    */
    virtual int Sn3DSetEnablePseudoColor(bool enable);
    /**
    *@brief 设置扫描工作范围
    */
    virtual int Sn3DSetWorkRange(float min, float max);
    
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
    virtual int Sn3DSetWholePointCloudCallBack(Sn3DWholePointCloudCallBack callBackFunc, void *owner);
    /**
    *@brief 注册当前帧点云回调
    */
    virtual  int Sn3DSetCurrentPointCloudCallBack(Sn3DCurrentPointCloudCallBack callBackFunc, void* owner);
    /**
    *@brief 注册增量点云回调
    */
    virtual int Sn3DSetIncreasePointCloudCallBack(Sn3DIncreasePointCloudCallBack callBackFunc, void* owner);
	/**
	*@brief 注册单片数据回调
	*/
	virtual int Sn3DSetRangeDataCallBack(Sn3DRangeDataCallBack callBackFunc, void* owner);
	/**
	*@brief 注册工程数据回调
	*/
	virtual int Sn3DSetProjectInfoCallBack(Sn3DProjectInfoCallBack callBackFunc, void* owner);
    /**
    *@brief 注册相机视角回调
    */
    virtual int Sn3DSetCameraPositionCallBack(Sn3DCameraPositionCallBack callBackFunc, void* owner);
    /**
    *@brief 注册跟踪丢失回调
    */
    virtual int Sn3DSetTrackLostStatusCallBack(Sn3DTrackLostStatusCallBack callBackFunc, void* owner);
    /**
    *@brief 注册扫描距离回调
    */
    virtual int Sn3DSetScanDistCallBack(Sn3DScanDistCallBack callBackFunc, void* owner);
    /**
    *@brief 注册设备按键操作事件回调
    */
    virtual int Sn3DSetDeviceEventCallBack(Sn3DDeviceEventCallBack callBackFunc, void* owner);
    /**
    *@brief 注册点云数量回调
    */
    virtual int Sn3DSetPointCountCallBack(Sn3DPointCountCallBack callBackFunc, void* owner);
    /**
    *@brief 注册帧率回调
    */
    virtual int Sn3DSetFrameRateCallBack(Sn3DFrameRateCallBack callBackFunc, void* owner);
    /**
    *@brief 注册帧数回调
    */
    virtual int Sn3DSetFrameCountCallBack(Sn3DFrameCountCallBack callBackFunc, void* owner);
    /**
    *@brief 注册平面检测回调
    */
    virtual int Sn3DSetTooFlatStatusCallBack(Sn3DTooFlatStatusCallBack callBackFunc, void* owner);
    
	/**
	*@brief 标定采图
	*/
	virtual int Sn3DCaliSnapImage();

	/**
	*@brief 标定采图
	*/
	virtual int Sn3DCaliCalculate();

	/**
	*@brief 标定采图
	*/
	virtual int Sn3DEnterCali();

	/**
	*@brief 标定采图
	*/
	virtual int Sn3DImproFramePoint(QString filePath);

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

    void pointCountChanged(int pointCount);

    void frameRateChanged(int frameRate);

    void frameCountChanged(int frameCount);

    void cameraPositionChanged(LPSn3dCameraPosition cameraPosition);
    void trackLostStatusChanged(bool trackLost);
    void scanDistChanged(double dis);
    void deviceEventChanged(DeviceEvent event);
    void tooFlatStatusChanged(bool tooFlat);
    void showMsg(QString msg, int type);
	void deviceStatusChanged(LPDeviceStatus deviceStatus);
	void rangeDataChanged(LPSn3dRangeData rangeData);
	void revealProjectInfo(LPSn3dProjectInfo projectInfo);

    void postProcessApplied();
    void postProcessConfirmed();
    void postProcessCanceled();
    void postProcessRollbacked();
protected:
    HSN3DSDKSERVICE m_hService = INVALID_HSN3DSDKSERVICE_VALUE;
    ScanResultFile m_scanResultFile;
    SocketHelper m_socketHelper;
    std::shared_ptr<std::thread> m_pThread;
    std::atomic_bool m_bQuit = { false };
    HSN3DSDKSERVICE m_hModBus = INVALID_HSN3DSDKSERVICE_VALUE;
	bool m_bShowCurrentRT = false;
};

