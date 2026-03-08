// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SDKCWRAPPER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// SDKCWRAPPER_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#ifndef SDKCWRAPPER_H_
#define SDKCWRAPPER_H_
#ifdef SDKCWRAPPER_EXPORTS
#define SDKCWRAPPER_API __declspec(dllexport)
#else
#define SDKCWRAPPER_API __declspec(dllimport)
#endif

#include "sn3dsdkcorepub.h"
#include "Sn3DSDKDataLibpub.h"

typedef void* HSN3DSDKSERVICE;

#define INVALID_HSN3DSDKSERVICE_VALUE 0

extern "C"{

/*!
* @brief 
* **SDK环境初始化， 在使用SDK前必须初始化，只能初始化一次** \n
* **Initializaiton of SDK environment, it must be called once and only once before using the SDK.**
* @details
* ZMQ: 启动FreeScan.exe并建立zmq连接 \n
*	1. 需要建立两条zmq信道 分别是 \n
*		1. 
*		**request-reply  demo** 作为request端FreeScan作为reply端 用于demo像FreeScan发送消息 \n
*		2. 
*		**reply-request demo** 作为reply端FreeScan作为request端 用于demo接收FreeScan的消息 \n
*	2. demo必须对每条FreeScan.exe发来的requst消息进行回复，回复内容为一个字节的0x02\n
*
* @param [out] hService 服务句柄 Service handle
* @param [in] iDeviceType 初始化设备类型 Initialize device type
* @return 0初始化成功，失败参考错误码列表 0:Initialization succeeded, if it fails refer to the error code list.
*/
SDKCWRAPPER_API int Sn3DInitialize(HSN3DSDKSERVICE* hService, int iDeviceType);

/*!
* @brief
* **SDK释放，不再调用SDK，可以调用该函数** \n
* **Release SDK**
* @details
* ZMQ: 关闭FreeScan.exe并断开zmq连接
* @param [in]  hService 服务句柄
* @return 0成功，失败参考错误码列表
*/
SDKCWRAPPER_API int Sn3DRelease(HSN3DSDKSERVICE* pService);

/*!
* @brief
* **连接设备功能** \n
* **Connect device**
* @param [in]  hService 服务句柄
* @return 0成功，失败参考错误码列表
*/
SDKCWRAPPER_API int Sn3DConnectDevice(HSN3DSDKSERVICE pService);

/*!
* @brief
* **重连设备** \n
* **Reconnect device**
* @param [in]  hService 服务句柄
* @return 0成功，失败参考错误码列表
*/
SDKCWRAPPER_API int Sn3DReConnectDevice(HSN3DSDKSERVICE pService);

/*
* @brief 关闭设备  Close device
* @param [in]  hService 服务句柄
* @return 0成功，失败参考错误码列表
*/
SDKCWRAPPER_API int Sn3DCloseDevice(HSN3DSDKSERVICE pService);

/*
* @brief 设备在线	Device is online
* @return 0设备在线，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DGetDeviceIsOnline(HSN3DSDKSERVICE pService);

/*
* @brief 单片扫描数据读取完成	range data finish
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DRangeDataFinish(HSN3DSDKSERVICE pService);

/*!
* @brief 设置设备亮度		Set device brightness
* @param level 档位，不能超过SN3DGetBrightnessRange获取到的最高和最低	The level can't exceed the highest and lowest values of the device
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetBrightness(HSN3DSDKSERVICE pService, int level);

/*!
* @brief 设置背景	Set mask background
* @param level 档位  
* @param enable 是否使能 
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetMaskBackGround(HSN3DSDKSERVICE pService, int level, bool enable);

/*!
*@breif 设置设备亮度（曝光、增益） Set device brightness (exposure、gain)
*@param hasFilter [in] 是否有滤光片
*@param bFilterSwitch [in] 是否强光模式
*/
SDKCWRAPPER_API int Sn3DSetDeviceBrightness(HSN3DSDKSERVICE pService, LPDeviceBrightness devBrightness);

/*!
* @brief 设置设备亮度范围				Set device brightness range
* @param min 最低档位					Reserved, the minimum brightness level is set to 0 (default). By default, this function is disable.
* @param max 最高档位					Maximum level of brightness.
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DGetBrightnessRange(HSN3DSDKSERVICE pService, int& min, int& max);

/*!
* @brief 设置设备亮度					Set device brightness
* @param currentBrightness 当前亮度		current brightness
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DGetCurrentBrightness(HSN3DSDKSERVICE pService, int& currentBrightness);

/*!
* @brief 开始视频处理					start video process
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DStartProcessVideoData(HSN3DSDKSERVICE pService);

/*!
* @brief 结束视频处理					stop video process
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DStopProcessVideoData();

/*!
* @brief 设置LED亮度		Set LED brightness
* @param ledDutyCycle亮度值，范围[0, 100]  LED Duty value range[0, 100]
* @return 0成功，失败参考错误码
* 该接口只支持H1.1设备，H1.2及以后设备不再支持 This interface only supports H1.1 devices, and devices after H1.2 no longer support it
*/
SDKCWRAPPER_API int Sn3DSetCurrentLEDDutyCycle(HSN3DSDKSERVICE pService, int ledDutyCycle);

/*!
*@brief 设置LED亮度
*/
SDKCWRAPPER_API int Sn3DSetLEDBrightness(HSN3DSDKSERVICE pService, int iStep);

/*!
* @brief 获取LED亮度		Set LED brightness
* @param ledDutyCycle亮度值，范围[0, 100]  LED Duty value range[0, 100]
* @return 0成功，失败参考错误码
* 该接口只支持H1.1设备，H1.2及以后设备不再支持 This interface only supports H1.1 devices, and devices after H1.2 no longer support it
*/
SDKCWRAPPER_API int Sn3DGetCurrentLEDDutyCycle(HSN3DSDKSERVICE pService, int& ledDutyCycle);

/*!
* @brief 开启伪彩色						Turn on pseudo color
* @param enable true开启，false关闭		“true”-open, “false”-off
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetEnablePseudoColor(HSN3DSDKSERVICE pService, bool enable);

/*
* @brief 设置扫描距离范围     set work range
* @param min 最小值				set the minimum working distance
* @param max 最大值				set the Maximum working distance
*/
SDKCWRAPPER_API int Sn3DSetWorkRange(HSN3DSDKSERVICE pService, float min, float max);

/*!
* @brief 保存数据		Save Data
* @param absolutePrjName 工程文件的绝对路径					Complete path of project files.
* @param savePath 保存文件的绝对路径不能带后缀				Path and name of the file required to be saved (without a suffix).
* @param saveType 保存格式,目前只有 asc ply stl obj 这四种	The format of the file required to be saved (asc ply stl obj).
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSaveData(HSN3DSDKSERVICE pService, LPSaveData saveData);

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
SDKCWRAPPER_API int Sn3DOpenOrCreateSolution(HSN3DSDKSERVICE pService, LPOpenOrCreateSln openOrCreateSln);

/*!
* @brief
* **新建工程** \n
* **New project**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*      payload  {cmd:new, \n
*                fileDir:slnDirPath #slnDirPath-新建工程存储的文件夹路径(例如D:/desktop/Data) \n
*                mode:0 #0-laser scan 1-inflared scan \n
*               }
* @param slnDirPath 解决方案路径	Solution Path
* @param scanMode 扫描模式, 0 portrait, 1 object, 2 smallobject. Reference: ScanMode
                    Scan mode, 0 portrait, 1 object, 2 smallobject. Reference: ScanMode
* @param hasTexture true纹理扫描, false不带纹理		“true”- texture, “false”-no texture
* @param pointDis 点距									Point distance
* @param alignType 拼接模式. Reference: AlignType		Alignment type. Reference: AlignType  See appendix
* @param globalMarkerPath 框架点路径				Global markers path，no global markers path：set scan mode to empty.
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DNewProject(HSN3DSDKSERVICE pService, LPNewProject newProject);

/*!
* @brief
* **打开工程** \n
* **Open project**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other  \n
*      payload  {"cmd":"openProject", \n
*                "fileDir":openProjectDir,#openProjectDir-打开工程的所在文件夹路径(例如D:/desktop/Data) \n
*                "fileName":openProjectFile,#openProjectFile-打开工程的文件名(例如Data.sln_laser_combo) \n
*                "ScanType":"LASER"#工程类型 LASER-激光
*               }
* @param projFileName 工程路径			Project path
* @param scanType 当前工程的扫描类型	Project scanType
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DOpenProject(HSN3DSDKSERVICE pService, const char* projFileName, ScanType scanType);

/*!
* @brief 打开框架点文件			Open p3 file
* @param p3Path 文件路径			p3 file path
* @param slnPath 解决方案路径			solution path		
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DLoadP3File(HSN3DSDKSERVICE pService, const char* p3Path);

/*!
*@brief Change scan mode
*@param iScanMode[in] SNSDK_SCANMODE_LASER, SNSDK_SCANMODE_SPECKLE or SNSDK_SCANMODE_INFRARED
*@return Error code , like SNSDK_ERR_OK
*/
SDKCWRAPPER_API int Sn3DChangeScanMode(HSN3DSDKSERVICE pService, int iScanMode);

/*!
* @brief 进入扫描			Enter scan
* @param scanType 扫描类型	Scan type
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DEnterScan(HSN3DSDKSERVICE pService, ScanType scanType);

/*!
* @brief 退出扫描		Quit scan mode
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DExitScan(HSN3DSDKSERVICE pService);

/*!
* @brief
* **设置扫描参数**  \n
* **Set Scan Params**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*      payload  {"cmd":"setProjectType2", \n
*                "type":scanType#0:scan framework 1:scan mesh \n
*               }
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetScanPars(HSN3DSDKSERVICE pService, LPScanPars scanPars);

/*!
* @brief
* **扫描, 需要在预扫描之后进行,在FreeScan中开启预扫时是进行预扫再调用一次进行扫描，关闭预扫时是直接开始扫描**  \n
* **scan can been started after preview scan**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*      payload  {"cmd":"scan", \n
*                "method":"scanCtrl", \n
*                "target":"Sn3DLaserScanController" \n
*               }
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DStartScan(HSN3DSDKSERVICE pService);

/*!
* @brief 预扫描		Preview	Scan
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DPreviewScan(HSN3DSDKSERVICE pService);

/*!
* @brief
* **暂停扫描**  \n
* **Pause Scan**
* @details
* **ZMQ**: envelope v2.0/invoke/scanservice/other \n
*      payload  {"cmd":"pause", \n
*                "method":"scanCtrl", \n
*                "target":"Sn3DLaserScanController" \n
*               }
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DPauseScan(HSN3DSDKSERVICE pService);
/*!
* @brief 继续扫描		Continue Scan 主要用于固定模式 转台扫描时暂时再继续 调用该接口
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DContinueScan(HSN3DSDKSERVICE pService);

/*!
* @brief 结束扫描		End scan
* @param globalOptimize true 优化, false 不优化		“true”-optimize , “false” -not optimized
* @param pointDist 点距设置		setting point distance.
* @param callback 设置回调, 可以拿结束扫描后的点云数据		Set up the callback to get the point cloud data after finishing the scan.
* @return 0成功，失败参考错误码
*/
// SDKCWRAPPER_API int Sn3DEndScan(HSN3DSDKSERVICE pService,
//     bool globalOptimize,
//     double pointDist,
//     Sn3DWholePointCloudCallBack wholeCallback = nullptr);

/*!
* @brief 全局优化，仅固定扫描支持
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DGlobalOptimization(HSN3DSDKSERVICE pService);

/*!
* @brief 特征自动拼接, 仅固定扫描支持
* @param pointFeatureAlign 特征拼接参数
* @param rot 旋转矩阵
* @param tran 平移矩阵
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DPointFeatureAlign(HSN3DSDKSERVICE pService, LPPointFeatureAlign pointFeatureAlign, float* rot, float* tran);

/*!
* @brief 特征手动拼接, 仅固定扫描支持
* @param pointManualAlign 特征拼接参数
* @param rot 旋转矩阵
* @param tran 平移矩阵
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DPointManualAlign(HSN3DSDKSERVICE pService, LPPointManualAlign pointManualAlign, float* rot, float* tran);

/*!
* @brief 清空扫描数据		Clear Scan Data
* @param isCancelCurrentProjectFramerMark true 清空框架点信息			"true" - Cancel current project framermark data，固定扫描改参数没有实际意义
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DClearScan(HSN3DSDKSERVICE pService,
    bool isCancelCurrentProjectFramerMark);

/*!
* @brief 网格化数据			Generate mesh data
MeshType
* @param meshType.  Reference: MeshType
* @param filterLevel.  0-3档, 不同的滤波力度				0-3 level, different levels of filtering degree.
* @param SmoothLevel.  1-3档, 在滤波基础上设置平滑力度		1-3 level, Setting up smooth level in filter.
* @param PointDis.  点距									point distance.
* @param fillSmallHole.  true填充小洞						“true”-Fill the small hole
* @param smellHolePerimeter.  填充小洞周长					Fill the circumference of the small hole
* @param fillMarkerHole.  true填充标志点					“true”-filling markers.
* @param spikeSensitivity.									Remove the spikes
* @param faceLimit. 最大面片限制							Maximum number of patch.
* @param callback. 设置回调, 取网格化数据后的点云数据		setting up callback for getting point cloud data of meshed.
* @return 0成功，失败参考错误码
*/
// SDKCWRAPPER_API int Sn3DMesh(HSN3DSDKSERVICE pService, 
//     LPMeshPars meshPars, Sn3DMeshDataCallBack callback = nullptr, void* owner = nullptr);

SDKCWRAPPER_API int Sn3DMeshPostProcess(HSN3DSDKSERVICE pService, bool optimized);

/*!
*@brief Send json message to SDK
*@param pService[in]          SDK service handle
*@param szVersion[in]         Protocol version, \0x00 terminated, eg. "v2.0"
*@param szMsgType[in]         Message type, \0x00 terminated, eg. "invoke", "propGet", "propSet", "propChanged", "event"
*@param szZMQTopic[in]        ZMQ topic, \0x00 terminated, eg. "checkDevice", "scanservice"
*@param pMsg[in]              Json format string to be sent
*@param stLen[in]             pMsg length
*@param pResult[in]           Json format string of result
*@param pstResultLen[in/out]  Result length
*/
SDKCWRAPPER_API int Sn3DSendMessage(HSN3DSDKSERVICE pService,
    const char* szVersion, const char* szMsgType, const char* szZMQTopic,
    const char* pMsg, size_t stLen,
    char* pResult, size_t* pstResultLen);

/*!
* @brief 获取视频流						Get video stream
* @param imageCallback 相机采图回调		Set the callback function.
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetImagasCallBack(HSN3DSDKSERVICE pService, Sn3DImagesCallBack imageCallback, void* owner);

/*!
* @brief 注册回调, 当前帧点云数据刷新触发		Register current point cloud callback
* @param callBackFunc 回调函数					current point cloud callback
* @return 0成功，失败参考错误码
*/

//LSC 新增接口
SDKCWRAPPER_API int Sn3DSetCurrentPointCloudCallBack(HSN3DSDKSERVICE pService, 
    Sn3DCurrentPointCloudCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 增量点云数据刷新触发			Register increase point cloud callback
* @param callBackFunc 回调函数					increase point cloud callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetIncreasePointCloudCallBack(HSN3DSDKSERVICE pService, 
    Sn3DIncreasePointCloudCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 相机视角数据刷新触发		Register Camera	Position callback
* @param callBackFunc 回调函数				Camera	Position callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetCameraPositionCallBack(HSN3DSDKSERVICE pService, 
    Sn3DCameraPositionCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 跟踪丢失状态数据刷新触发		Register track loss status callback
* @param callBackFunc 回调函数					track loss status callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetTrackLostStatusCallBack(HSN3DSDKSERVICE pService, 
    Sn3DTrackLostStatusCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 扫描距离挡位数据刷新触发		Register scan distance gear callback
* @param callBackFunc 回调函数					scan distance gear callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetScanDistCallBack(HSN3DSDKSERVICE pService, 
   Sn3DScanDistCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 设置设备操作摁键触发		Register device event callback
* @param callBackFunc 回调函数				device event callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetDeviceEventCallBack(HSN3DSDKSERVICE pService, 
    Sn3DDeviceEventCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 整体点云刷新触发		Register point count callback
* @param callBackFunc 回调函数				device event callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetWholePointCloudCallBack(HSN3DSDKSERVICE pService,
                                                          Sn3DWholePointCloudCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 当前点云刷新触发		Register point count callback
* @param callBackFunc 回调函数				device event callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetCurrentPointCloudCallBack(HSN3DSDKSERVICE pService,
                                                            Sn3DCurrentPointCloudCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 增量点云刷新触发		Register point count callback
* @param callBackFunc 回调函数				device event callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetIncreasePointCloudCallBack(HSN3DSDKSERVICE pService,
                                                             Sn3DIncreasePointCloudCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 单片扫描数据刷新触发			Range data callback
* @param callBackFunc 回调函数				device event callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetRangeDataCallBack(HSN3DSDKSERVICE pService,
												Sn3DRangeDataCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 工程数据刷新触发			Project info callback
* @param callBackFunc 回调函数				device event callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetProjectInfoCallBack(HSN3DSDKSERVICE pService,
												Sn3DProjectInfoCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 点云数刷新触发		Register point count callback
* @param callBackFunc 回调函数				device event callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetPointCountCallBack(HSN3DSDKSERVICE pService, 
    Sn3DPointCountCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 帧率刷新触发		Register frame rate callback
* @param callBackFunc 回调函数				device event callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetFrameRateCallBack(HSN3DSDKSERVICE pService, 
    Sn3DFrameRateCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 帧数刷新触发		Register frame count callback
* @param callBackFunc 回调函数				device event callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetFrameCountCallBack(HSN3DSDKSERVICE pService, 
    Sn3DFrameCountCallBack callBackFunc, void* owner);

/*!
* @brief 注册回调, 平面提示状态数据刷新触发		Register too flat status callback
* @param callBackFunc 回调函数					too flat status callback
* @param owner. 提供调用对象指针					owner of callback
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DSetTooFlatStatusCallBack(HSN3DSDKSERVICE pService, 
    Sn3DTooFlatStatusCallBack callBackFunc, void* owner);

SDKCWRAPPER_API int Sn3DSetDeviceStatusCallBack(HSN3DSDKSERVICE pService, Sn3DDeviceStatusCallBack pfCallBack, void* owner);
/*!
*@brief 注册回调
*/
//SDKCWRAPPER_API int Sn3DRegisterCallback(SDKCORE_CALLBACK pfCallBack, void* pUserData);

SDKCWRAPPER_API int Sn3DRegisterCallback(HSN3DSDKSERVICE pService, SDKCORE_CALLBACK pfCallBack, void* pUserData);

/*!
*@brief 注册场景数据更新
*/
SDKCWRAPPER_API int Sn3DRegisterSceneDataCallback(Sn3DSceneDataCallBack pfCallBack, void* pUserData);

/*!
*@brief 获取场景数据 调两次 用户分配内存
*/
SDKCWRAPPER_API int Sn3DGetSceneData(LPSN3D_SCENE_DATA sceneData);

/*!
*@brief 进入标定
*/
SDKCWRAPPER_API int Sn3DEnterCali(HSN3DSDKSERVICE pService);
/*!
*@brief 标定采图
*/
SDKCWRAPPER_API int Sn3DCaliSnapImage(HSN3DSDKSERVICE pService);

/*!
*@brief 标定计算
*/
SDKCWRAPPER_API int Sn3DCaliCalculate(HSN3DSDKSERVICE pService);

/*!
* @brief 导入框架点文件
* @param filePath 文件路径
* @return 0成功，失败参考错误码
*/
SDKCWRAPPER_API int Sn3DImproFramePoint(HSN3DSDKSERVICE pService, const char* filePath);

/*!
*@brief
* **设置激光线数** \n
* **set laser grade**
* @details
* **ZMQ**: envelope v2.0/invoke/scanservice/other \n
*     payload  {"cmd":"setLaserGrade2",  \n
*     "index":index \n
*     }
*@param index [in] grade index \n
* **参数-0:交叉 1:平行 2:单线** \n
* **index-0:cross 1:parallel 2:single**
*/
SDKCWRAPPER_API int Sn3DSetLaserGrade(HSN3DSDKSERVICE pService, int index);

/*!
*@brief end scan for FreeScan ONLY
*/
SDKCWRAPPER_API int Sn3DEndScan(HSN3DSDKSERVICE pService);

/*!
*@brief
* **网格处理** \n
* **freescan mesh**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*     payload  {"cmd":"apply", \n
*               "method":"apply", \n
*               "fileter_level":0, \n
*               "remove_small_floating_parts":0, \n
*               "max_trianlges_ischeck":True, \n
*               "max_trianlges_value":20000000,\n
*               "fill_small_hole_ischeck":True, \n
*               "fill_small_hole_value":10, \n
*               "remove_spike_ischeck":True, \n
*               "marker_hole_filling_ischeck":True, \n
*               "isRememberChoice":False \n
*              }
* @param fileter_level 封闭选项
* **0-非封闭模型 1-半封闭模型 2-封闭模型**
* @param remove_small_floating_parts  去孤立面
* @param max_trianlges_ischeck 是否选择最大面片数
* @param max_trianlges_value 最大面片数值
* @param fill_small_hole_ischeck 是否补小洞
* @param fill_small_hole_value  补小洞周长
* @param remove_spike_ischeckrue 是否去尖刺
* @param marker_hole_filling_ischeck 标志点补洞
* @param isRememberChoice 是否使用推荐参数
*              }
*/
SDKCWRAPPER_API int Sn3DMesh(HSN3DSDKSERVICE pService, LPFreeScanMeshPar pParam);

/*!
*@brief
* **保存网格处理结果** \n
* **freescan save mesh**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*     payload  {"fileName":fileName \n
*               ,"folderPath":folderPath \n
*               ,"format":"001000000" \n
*               ,"method":"save" \n
*               ,"postpageVisible":1 \n
*               ,"saveAscFile":0 \n
*               ,"saveStlFile":1 \n
*               ,"saveObjFile":0 \n
*               ,"savePlyFile":0 \n
*               ,"saveP3File":0 \n
*               ,"save3MfFile":0 \n
*               ,"saveTxtFile":0 \n
*               ,"saveCsvFile":0 \n
*               }
* @param format 前置补0，后面7位与saveXXXFile一致
* @param postpageVisible 默认置1
* @param saveXXXFile 是否保存XXX格式的文件
*/
SDKCWRAPPER_API int Sn3DSaveMesh(HSN3DSDKSERVICE pService, LPFreeSaveMeshPar pParam);

/*!
*@brief
* **确认网格参数** \n
* **freescan confirm mesh**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*     payload  {"cmd":"confirmMesh"}
*/
SDKCWRAPPER_API int Sn3DConfirmMesh(HSN3DSDKSERVICE pService);

/*!
*@brief
* **回滚网格参数** \n
* **freescan undo mesh**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*     payload  {"cmd":"undoMesh"}
*/
SDKCWRAPPER_API int Sn3DUndoMesh(HSN3DSDKSERVICE pService);

/*!
*@brief freescan set calibration mode
*/
SDKCWRAPPER_API int Sn3DSetCalibMode(HSN3DSDKSERVICE pService, int iMode);

/*!
*@brief
* **设置是否为框架点增加模式** \n
* **freescan set is increase framework mode**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*     payload  {"cmd":"setIsIncFramework2", \n
*               "increase":isIncrease#true or false  \n
*              }
*@param isIncrease [in]  true or false
*/
SDKCWRAPPER_API int Sn3DSetIsIncFramework(HSN3DSDKSERVICE pService, bool bInc);

/*!
*@brief
* **进入扫描界面** \n
* **freescan enter scan page**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other
*     payload  {"cmd":"enterScanPage2"}
*/
SDKCWRAPPER_API int Sn3DEnterScanPage(HSN3DSDKSERVICE pService);

/*!
*@brief
* **扫面网格时改变亮度** \n
* **freescan change brightness changeBrightnessWhenScanMesh**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*     payload  {"cmd":"changeBrightStep2", "index":index}
*@param iIndex[in] brightness index in the scan panel \n
*       index-int-0<=index<=17
*/
SDKCWRAPPER_API int Sn3DChangeBrightStep(HSN3DSDKSERVICE pService, int iIndex);

/*!
*@brief freescan set is high speed scan
*@brief bIsHigh [in] true means high speed mode
*/
SDKCWRAPPER_API int Sn3DSetIsHighSpeed(HSN3DSDKSERVICE pService, bool bIsHigh);

/*!
*@brief
* **设置扫描物件模式** \n
* **freescan set scan object type**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*     payload  {"cmd":"setScanObject2"
*               , "index":index}
*@param iIndex[in] object index in the scan panel \n
* **index-0:normal 1:reflect** \n
* **index-0:普通 1:反光**
*/
SDKCWRAPPER_API int Sn3DSetScanObject(HSN3DSDKSERVICE pService, int iIndex);

/*!
*@brief freescan enter scan mode page
*/
SDKCWRAPPER_API int Sn3DEnterScanModePage(HSN3DSDKSERVICE pService);

/*!
*@brief
* **设置扫描模式（激光或者红外）** \n
* **freescan set scan mode(Laser or Inflared)**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*     payload  {cmd:setScanMode2, \n
*               ScanMode:0  \n
*              }
*@param
* iMode[in]
* **0-laser scan 1-inflared scan** \n
* **0-激光扫描 1-红外扫描**
*/
SDKCWRAPPER_API int Sn3DSetScanMode(HSN3DSDKSERVICE pService, int iMode);

/*!
*@brief
* **取消扫描，用于清理扫描数据**
* **freescan cancel scan, used for clean scan result**
* @details
* **ZMQ** : envelope v2.0/invoke/scanservice/other \n
*      payload  {"cmd":"delete", \n
*                "method":"scanCtrl", \n
*                "target":"Sn3DLaserScanController" \n
*               }
*/
SDKCWRAPPER_API int Sn3DCancelScan(HSN3DSDKSERVICE pService);

/*!
*@brief freescan stop calib
*/
SDKCWRAPPER_API int Sn3DStopCalib(HSN3DSDKSERVICE pService, int iMode);

/*!
*@brief freescan apply post process
*/
SDKCWRAPPER_API int Sn3DApplyPostProcess(HSN3DSDKSERVICE pService, LPFreePostProcessPar par);

/*!
*@brief freescan confirm post process
*/
SDKCWRAPPER_API int Sn3DConfirmPostProcess(HSN3DSDKSERVICE pService);

/*!
*@brief freescan cancel post process
*/
SDKCWRAPPER_API int Sn3DCancelPostProcess(HSN3DSDKSERVICE pService);

/*!
*@brief freescan rollback post process
*/
SDKCWRAPPER_API int Sn3DRollbackPostProcess(HSN3DSDKSERVICE pService);


}
#endif
