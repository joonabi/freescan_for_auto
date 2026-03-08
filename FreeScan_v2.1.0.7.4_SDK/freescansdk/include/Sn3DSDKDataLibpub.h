#if !defined(SNSDK_DATALIB_PUBLIC)

#define SNSDK_ERR_OK    									0x00000000  // 成功	
#define SNSDK_ERR_INITIALIZEFIAILZED						0x00000002  // 初始化失败	
#define SNSDK_ERR_AlREADYINITIALIZED						0x00000003  // 重复初始化视频流共享内存	
#define SNSDK_ERR_INITSHAREDMEMORY							0x00000004	// 共享内存初始化失败	



#define SNSDK_DATALIB_PUBLIC
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
typedef void (CALLBACK *Sn3DGetImagesCallBack)(int camId, const unsigned char* data, int  width, int height, int channel, void* owner);



/*!
* @brief 场景数据更新回调
*/
typedef void (CALLBACK *Sn3DSceneDataCallBack)(void* pUserData);

typedef struct SN3D_POINT_DATA
{
	float              nX;
	float              nY;
	float              nZ;
} SN3D_POINT_DATA, *LPSN3D_POINT_DATA;

typedef struct SN3D_SCANNER_RT
{
	float		aRot[9];
	float		aTrans[3];
} SN3D_SCANNER_RT, *LPSN3D_SCANNER_RT;

typedef struct SN3D_MARKER
{
	SN3D_POINT_DATA	mCoords;           //坐标
	SN3D_POINT_DATA	mNormals;          //法向
	int     nId = 0;             //编号
	float	nDiameter = 6.0;     //直径
	int     nCodeId = 0;         //编码点ID(<=0即为普通的标志点)
	bool    bIsDelete = false;   //是否删除 
}SN3D_MARKER, *LPSN3D_MARKER;

typedef  struct SN3D_CLOUDPOINT
{
	LPSN3D_POINT_DATA pPointCoords = nullptr;       //点云坐标
	LPSN3D_POINT_DATA pPointNormals = nullptr;      //点云法向
	LPSN3D_POINT_DATA  pPointColors = nullptr;//std::vector<std::array<float, 3>> colors;       //点云纹理颜色		
}SN3D_CLOUDPOINT, *LPSN3D_CLOUDPOINT;

typedef struct {
	int x;
	int y;
	int z;
} SN3D_TRI_FACE, *LPSN3D_TRI_FACE;

typedef struct SN3D_UVCOORD
{
	int               uu;
	int               vv;
} SN3D_UVCOORD, *LPSN3D_UVCOORD;

typedef struct {
	int width;				//width of image
	int height;				//height of image
	int channel;			//channel of image 
	unsigned char* data;			//data of image
}SN3D_IMAGE_DATA,*LPSN3D_IMAGE_DATA;

typedef struct {
	//点云数量和数据   
	int				nMeshpointCount;	//num of point cloud
	LPSN3D_POINT_DATA pMeshPoint;			//data of point cloud
	//法向数量和数据
	int				nMeshNormalCount;	//num of normal
	LPSN3D_POINT_DATA	pMeshNormal;			//data of normal
	//点云索引
	int				nMeshTriFaceIdCount;//num of point index
	LPSN3D_TRI_FACE	pMeshTriFaceId;		//data of point index
	//纹理索引
	int				nMeshTextureIdCount;//num of textureimage
	LPSN3D_TRI_FACE	pMeshTextureId;		//data of texture image
	//纹理UV坐标
	int				nTextureUVCount;	//num of uv
	LPSN3D_UVCOORD		pTextureUV;			//data of uv
	//纹理图
	SN3D_IMAGE_DATA		mImage;				//texture image
}SN3D_MESH_DATA, *LPSN3D_MESH_DATA;

typedef struct {
	int nVertexCount;
	LPSN3D_POINT_DATA pVertex;
	int	nMeshTriFaceIdCount;	//num of point index
	LPSN3D_TRI_FACE	pMeshTriFaceId;			//data of point index
	float* pWeightSet;						//每个顶点的权重 个数等于nVertexCount
}SN3D_INCREASE_MESH_DATA, *LPSN3D_INCREASE_MESH_DATA;
typedef struct SN3D_SCENE_DATA
{
	int	nCurrentMarkersCount = 0;
	LPSN3D_MARKER pCurrentMarkers = nullptr;

	int	nWholeMarkersCount = 0;
	LPSN3D_MARKER pWholeMarkers = nullptr;

	int nCurrentCloudCount = 0;
	SN3D_CLOUDPOINT pCurrentCloud;//命名错了待改

	SN3D_SCANNER_RT aCurrentCloudRt;

	int nWholeCloudCount = 0;
	SN3D_CLOUDPOINT pWholeCloud;

	SN3D_MESH_DATA mMesh;//整体网格(UE激光)

	SN3D_INCREASE_MESH_DATA mIncreaseMesh;//增量网格(UE激光)
	//增量点云

}SN3D_SCENE_DATA, *LPSN3D_SCENE_DATA;
#endif
