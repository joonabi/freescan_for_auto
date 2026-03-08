/**
*@file sn3dsdkcorepub.h
*@brief define const value or enum for Shining3D SDK
*@author xuzhen
*@data 2023/11/21
*/
#if !defined(SNSDK_CORE_PUBLIC)
#define SNSDK_CORE_PUBLIC
#include "sn3dsdkpluginpub.h"
#include <string>
#include <map>
enum ScanType {
    ST_FIXED = 0,           // fix scan
    ST_HD = 1,				// HD
    ST_RAPID = 2,			// rapid scan         
    ST_EinScan_HX_NORMAL = 3,	// E3_HX rapid
    ST_EinScan_HX_LASER = 4,		// E3_HX laser
    ST_EinScan_H_NORMAL = 5,		// E3_H normal
    ST_EinScan_H_BODY = 6,		// E3_H body
    ST_EinScan_H_PORTRAIT = 7,	// E3_H portrait
    ST_EinScan_CALIB = 8,	     // calib
    ST_EinStar_CALIB = 9,
    ST_EinStar_NORMAL = 10,
    ST_FreeScan_LASER = 11,       // UE
    ST_FreeScan_Infared = 12,
};

enum DeviceEvent {
    DE_NULL = -1,				//NULL
    DE_DOUBLECLICK = 0,			//Double Click
    DE_CLICK = 1,				//Click
    DE_UP_CLICK = 4,			//Up button click
    DE_DOWN_CLICK = 5,			//Down button click
    DE_RIGHT_CLICK = 6,			//Right button long press
    DE_LEFT_CLICK = 7,			//Left button long press
    DE_SMALL_CLICK = 8,			//SMALL button click
    DE_LARGE_CLICK = 9,			//LARGE button click
};

/// 数据类型，当前序列化的数据类型
enum DATA_TYPE
{
	DT_POINT_VERTEX, /// 点坐标(顶点）
	DT_POINT_NORMAL, /// 点法向
	DT_POINT_ID, /// 点ID
	DT_POINT_WEIGHT, /// 点权重
	DT_GRAY_UV, /// 扫描图uv坐标值
	DT_MARKER_VERTEX, /// 标志点坐标
	DT_MARKER_NORMAL, /// 标志点法向
	DT_MARKER_ID, /// 标志点ID
	DT_MARKER_RADIUS, /// 标志点半径
	DT_MARKER_WEIGTHT, /// 标志点权重
	DT_COLOR_0_255, /// 颜色值 RGB [0,255]
	DT_COLOR_0_1, /// 颜色值 RGB [0,1]
	DT_TEX_IMG, /// 纹理图数据
	DT_TEX_UV, /// 纹理图坐标 uv
	DT_RT, /// RT数据
	DT_MESH_TOP, /// 网格拓扑数据
	DT_FACE_NORMAL, /// 面片法向数据
	DT_FACE_ID, /// 面片ID
	DT_FACE_TEX_ID, /// 面片中每个点的纹理id
	DT_END, /// 数据结束标志位
};

/// 序列化数据的具体数据类型
enum RAW_DATA_TYPE
{
	RDT_CHAR, ///< char类型
	RDT_INT, ///< int类型
	RDT_FLOAT, ///< float类型
	RDT_DOUBLE ///< double类型
};

/// 反序列化错误码
enum DeserializeErrorCode
{
	EM_Deserialize = 0x29000000, /// 反序列化模块错误码

	EC_DeserNoError = 0x00000000,
	EC_DeserInputParamNotValid = 0x00000001 | EM_Deserialize, /// 传入无效参数 
	EC_DeserFinish = 0x00000002 | EM_Deserialize, /// 反序列化数据传输完成
	EC_DeserLast = 0x00000003 | EM_Deserialize, /// 最后一次传输
	EC_DeserHaveMore = 0x00000004 | EM_Deserialize, /// 反序列化数据未传输完成
	EC_DeserEnd = 0x00000005 | EM_Deserialize, /// 反序列化数据传输完成
	EC_DeserFail = 0x00000006 | EM_Deserialize, /// 反序列化发生错误
};

typedef struct
{
    float              x;						//X coordinate value
    float              y;						//Y coordinate value
    float              z;						//Z coordinate value
}Sn3dPointData, *LPSn3dPointData;

typedef struct
{
    int               vertex_count;			//The number of vertex .
    LPSn3dPointData   vertex_data;			//The data of vertex.
    int               norma_count;			//The number of vertex normal.
    LPSn3dPointData   norma_data;			//The data of vertex normal.
    int               vertex_color_count;	//The number of vertex color.
    LPSn3dPointData   vertex_color_data;	//The data of vertex color.
}Sn3DPointCloud, *LPSn3dPointCloud;

typedef struct
{
    int               vertex_count;			//The number of vertex .
    LPSn3dPointData   vertex_data;			//The data of vertex.
    int               norma_count;			//The number of vertex normal.
    LPSn3dPointData   norma_data;			//The data of vertex normal.
    int               vertex_color_count;	//The number of vertex color.
    LPSn3dPointData   vertex_color_data;	//The data of vertex color.
    int				  index_count;			//The number of index
    unsigned int*	  index;				//The data of index
}Sn3DIncreasePointCloud, *LPSn3dIncreasePointCloud;

typedef struct {
    LPSn3dPointData position;				//Camera Position
    LPSn3dPointData center;				//Center of view 
    LPSn3dPointData up;					//Normal of up
}Sn3DCameraPosition, *LPSn3dCameraPosition;

//LSC 数据结构设计
// x y z : index of face 
typedef struct {
    int x;
    int y;
    int z;
}Sn3DFaceId, *LPSn3dFaceId;

//x, y : coordinate of texture
typedef struct {
    float x;
    float y;
}Sn3DVec2F, *LPSn3dVec2F;

// QJW added
// 标志点id
typedef struct {
	int64_t x;
}Sn3DMarkerId, *LPSn3dMarkerId;

// 标志点半径
typedef struct {
	float x;
}Sn3DMarkerRadius, *LPSn3dMarkerRadius;

// 旋转矩阵
typedef struct {
	float x;
}Sn3DRot, *LPSn3dRot;

// 平移向量
typedef struct {
	float x;
}Sn3DTran, *LPSn3dTran;

typedef struct {
    int width;				//width of image
    int height;				//height of image
    int channel;			//channel of image 
    unsigned char* data;	//data of image
}Sn3DImage;


typedef struct {
    //点云数量和数据   
    int				meshpoint_count;	//num of point cloud
    LPSn3dPointData meshpoint;			//data of point cloud
    //法向数量和数据
    int				meshnormal_count;	//num of normal
    LPSn3dPointData	meshnormal;			//data of normal
    //点云索引
    int				meshtrifaceid_count;//num of point index
    LPSn3dFaceId	meshtrifaceid;		//data of point index
    //纹理索引
    int				meshtextureid_count;//num of textureimage
    LPSn3dFaceId	meshtextureid;		//data of texture image
    //纹理UV坐标
    int				textureUV_count;	//num of uv
    LPSn3dVec2F		textureUV;			//data of uv
    //纹理图
    Sn3DImage		image;				//texture image
}Sn3DMeshData, *LPSn3dMeshData;


/// 序列化数据头
struct DATA_HEAD
{
	DATA_TYPE dataType; ///< 序列化的数据类型，如：坐标，法向，颜色等
	RAW_DATA_TYPE rawDateType; ///< 数据的具体类型，如：char，int，float等
	int totalCount; ///< 当前需要传输的数据总个数
	int currentPackDataCount; ///<当前能传输的数据个数 
};


typedef struct
{
	// 缓冲区指针
	char* m_pBuffer;
	// 缓冲区长度
	int m_bufferLen;
	// 数据指针
	char* m_pData;
	// 接收数据缓冲区指针,暂存接收到的所有数据
	char* m_tmpBuffer;
	// 已经接收的数据长度
	int m_recDataLen;
	// 当前接收包中的有效数据长度，即：总包长-数据头  单位：字节
	int m_recValiDataLen;
	int m_recPos;
	// 当前接受到包的数据长度
	int m_curRecPackDataLen;
	DATA_HEAD		dataHead;
	// 点云数量和数据
	int				rangepoint_count;
	LPSn3dPointData rangepoint;
	// 法向数量和数据
	int				rangepointnormal_count;
	LPSn3dPointData rangepointnormal;
	// 标志点数量和数据
	int				rangemarker_count;
	LPSn3dPointData rangemarker;
	// 标志点法向数量和数据
	int				rangemarkernormal_count;
	LPSn3dPointData rangemarkernormal;
	// 标志点ID数量和数据
	int				rangemarkerid_count;
	LPSn3dMarkerId  rangemarkerid;
	// 标志点半径数量和数据
	int				rangemarkerradius_count;
	LPSn3dMarkerRadius rangemarkerradius;
	// 纹理颜色值数量和数据
	int				rangetexvertex_count;
	LPSn3dPointData rangetexvertex;
	/// 旋转矩阵数组个数
	const int ROT_NUM = 9;
	/// 平移向量数组个数
	const int TRAN_NUM = 3;
	// 旋转矩阵
	LPSn3dRot		rot;
	// 平移向量
	LPSn3dTran		tran;

}Sn3DRangeData, *LPSn3dRangeData;

typedef struct
{
	// 点云数量和数据
	int				pointdata_count;
	LPSn3dPointData pointdata;
	// 法向数量和数据
	int				pointnormaldata_count;
	LPSn3dPointData pointnormaldata;
	// 标志点数量和数据
	int				marker_count;
	LPSn3dPointData marker;
	// 标志点法向数量和数据
	int				markernormal_count;
	LPSn3dPointData markernormal;
	// 标志点ID数量和数据
	int				markerid_count;
	LPSn3dMarkerId  markerid;
	// 标志点半径数量和数据
	int				   markerradius_count;
	LPSn3dMarkerRadius markerradius;
	// 纹理颜色值数量和数据
	int				texvertex_count;
	LPSn3dPointData texvertex;
	/// 旋转矩阵数组个数
	const int ROT_NUM = 9;
	/// 平移向量数组个数
	const int TRAN_NUM = 3;
	// 旋转矩阵
	LPSn3dRot		rot;
	// 平移向量
	LPSn3dTran		tran;

}Sn3DParsedRangeData, *LPSn3dParsedRangeData;


// 工程组信息
typedef struct  
{
	std::string dataID;
	std::string name;
	std::string prjName;
}Sn3DProjectInfo, *LPSn3dProjectInfo;

//手动拼接模式
enum AlignType : int
{
	Point_Feature = 0, //点云特征手动
	Marker_Feature,     //标志点手动拼接
	Point_Manual,              //手选点云手动拼接
	Marker_Manual              //手选标点手动拼接
};

// 特征拼接参数
typedef struct {
	AlignType		alignType;
	std::string		fixKey;
	std::string		floatKey;
	bool			isTurnTableScan;
}PointFeatureAlign, *LPPointFeatureAlign;

// 手动拼接参数
typedef struct
{
	AlignType		alignType;
	std::string		fixKey;
	std::string		floatKey;
	bool			isTurnTableScan;
	int				v_fixPosCount;
	LPSn3dPointData v_fixPos;
	int				v_floatPosCount;
	LPSn3dPointData v_floatPos;
}PointManualAlign, *LPPointManualAlign;

// 设备亮度参数
typedef struct {
    int         iStep = 0;
    const char* szDeviceType = "";
    int         brightAlignMode = SNSDK_BRIGHT_A_POINTCLOUD;
    int         scanObject = SNSDK_SCANOBJECT_NORMAL;
    int         laserGrade = SNSDK_LASERGRADE_OFF;
    bool        hasFilter = false;
    bool        bFilterSwitch = false;
}DeviceBrightness, *LPDeviceBrightness;

// 设备状态参数
typedef struct {
	bool hasTexture = true;
	bool isCalibrate = false; 
	int mode = -1; 
	bool offline = false; 
	bool offlineEnter = false;
}DeviceStatus, *LPDeviceStatus;

// 保存数据参数
typedef struct {
    const char* absolutePrjName;
    const char* savePath;
    const char* saveType;
}SaveData, *LPSaveData;

// 打开或创建解决方案参数
typedef struct {
    const char* szSlnDirPath;
    bool        isCreate;
    int         iScanMode;
    bool        hasTexture;
    float       fPointDis;
	int			iAlignType;
	bool		useNonrigid;
	int			iNeedLimit; // 0:新建空解决方案; 1:新建单个工程; 2: 新建解决方案
}OpenOrCreateSln, *LPOpenOrCreateSln;

// 新建工程参数
typedef struct {
    const char* szSlnDirPath;
    int         iScanMode;
    bool        hasTexture;
    float       fPointDis;
    int         iAlignType;
    const char* szGlobalMarkerPath;
}NewProject, *LPNewProject;

// 扫描参数
typedef struct {
    bool scanPointCloud;
    bool scanMarkers;
    bool addGlobalMarkers;
    const char* alignMod;
    bool HDR;
    bool turtableChecked;
    const char* turntableAlignMod;
    int turntableSteps;
    int turntableSpeed;
    bool isRoundChecked;
    bool useExistVerifyData;
    bool openScanEdit;
    bool scanPhotographic;
}ScanPars, *LPScanPars;

// 封装参数
typedef struct {
    int meshType;               // 0: Unwartertight 1: Watertight
    int filterLevel;            // 0: None 1: Low 2:Med 3:High
    int smoothLevel;            // 0: None 1: Low 2:Med 3:High
    float pointDis;             // na
    int removeSmall;            // 0-100
    bool maxFace;               // true false
    int faceLimit;              // 0-50000000
    bool fillSmallHole;         // true false
    double smallHolePerimeter;  // 10-100
    bool spikeSensitivity;      // true false
    bool fillMarkerHole;        // true false
}MeshPars, *LPMeshPars;

// Mesh parameter for FreeScan
typedef struct
{
	int watertightType;//0 unclose 1 halfclose 2 close
    int fileter_level;
	int smoothValue;//1 low 2 med 3 high
    int remove_small_floating_parts;
    bool max_trianlges_ischeck;
    int max_trianlges_value;
	bool border_optimize;
    int fill_small_hole_ischeck;
    int fill_small_hole_value;
	int neighborhood_perimeter;
    bool remove_spike_ischeck;
    bool marker_hole_filling_ischeck;
}FreeScanMeshPar, *LPFreeScanMeshPar;

// Save mesh param for FreeScan
typedef struct
{
    const char* fileName;
    const char* folderPath;
    const char* format;
    const char* method;
    int postpageVisible;
    int saveAscFile;
    int saveStlFile;
    int saveObjFile;
    int savePlyFile;
    int saveP3File;
    int save3MfFile;
    int saveTxtFile;
    int saveCsvFile;
}FreeSaveMeshPar, *LPFreeSaveMeshPar;

typedef struct
{
    int autoFillMarkerType;
    float markerPerimeter;
}FreePostProcessPar, *LPFreePostProcessPar;

struct Async {};


/*!
* @brief 定义获图片的回调
* param camId 相机索引					Index of Camera
* param data 图像数据					Data of image
* param width 图像宽度					Width of image
* param height 图像高度					height of image
* param channel 通道数					num of channel
* param owner 回调的函数的拥有者指针	Pointer to the owner of the callback function
* @return 0初始化成功，失败参考错误		Initialization success.
*/
typedef void(CALLBACK *Sn3DImagesCallBack)(int camId, const unsigned char* data, int  width, int height, int channel, void* owner);

/**
*@brief SDK call back function
*@param[out] iEventType Events Type. Reference: The Notification Events of Calibration.
*@param[out] iExtendData Events extend data.
*@param[out] pData The data structure pointer matched with event
*@param[out] stDataLen The length of data.
*@param[out] pUserData The parameter needs to be imported into sn3d_regist_callback.
*NOTE: 1. callback function run in a workthread
*/
typedef void (CALLBACK *SDKCORE_CALLBACK)(int iEventType, int iExtendData, void* pData, size_t stDataLen, void* pUserData);

//Initialize the device setup callback
typedef  void (CALLBACK *Sn3DScanServiceWatcherCallBack)(int watcher, void* owner);

//LSC新增回调函数  Add callback function
//Get whole point cloud callback
typedef void(CALLBACK *Sn3DWholePointCloudCallBack)(LPSn3dPointCloud wholePointCloud);

//Get current point cloud callback
typedef void(CALLBACK *Sn3DCurrentPointCloudCallBack)(LPSn3dPointCloud currentPointCloud, void* owner);

//Get increase point cloud callback function
typedef void(CALLBACK *Sn3DIncreasePointCloudCallBack)(LPSn3dIncreasePointCloud increasePointCloud, void* owner);

//Get Camera Position callback function
typedef void(CALLBACK *Sn3DCameraPositionCallBack)(LPSn3dCameraPosition cameraPosition, void* owner);

//Get track lost status callback function
typedef void(CALLBACK *Sn3DTrackLostStatusCallBack)(bool trackLostStatus, void* owner);

//Get scan distance gear callback function
typedef void(CALLBACK *Sn3DScanDistCallBack)(double scanDist, void* owner);

//Get mesh data callback function
typedef void(CALLBACK *Sn3DMeshDataCallBack)(LPSn3dMeshData meshData, void* owner);

//Get range data callback function
typedef void(CALLBACK *Sn3DRangeDataCallBack)(LPSn3dRangeData rangeData, void *owner);

//Get project info callback function
typedef void(CALLBACK *Sn3DProjectInfoCallBack)(LPSn3dProjectInfo projectInfo, void *owner);

//Get device event callback function
typedef void(CALLBACK *Sn3DDeviceEventCallBack)(DeviceEvent event, void* owner);

//Get Point Count callback function
typedef void(CALLBACK *Sn3DPointCountCallBack)(int pointCount, void* owner);

//Get Frame Rate callback function
typedef void(CALLBACK *Sn3DFrameRateCallBack)(int frameRate, void* owner);

//Get Frame Count callback function
typedef void(CALLBACK *Sn3DFrameCountCallBack)(int frameCount, void* owner);

//Get too flat callback function
typedef void(CALLBACK *Sn3DTooFlatStatusCallBack)(bool tooFlatStatus, void* owner);

typedef void(CALLBACK *Sn3DDeviceStatusCallBack)(LPDeviceStatus deviceStatus, void* owner);

//Get SharedMemory Notified callback function
typedef void (CALLBACK *Sn3DShmNotifiedCallBack)(int shmID);
#endif // SNSDK_CORE_PUBLIC