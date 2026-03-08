#ifdef SDKSOCKETWRAPPER_EXPORTS
#define SDKSOCKETWRAPPER_API __declspec(dllexport)
#else
#define SDKSOCKETWRAPPER_API __declspec(dllimport)
#endif
#include "sn3dsdkcorepub.h"
#include "Sn3DSDKDataLibpub.h"
#define INVALID_HSN3DSDKSERVICE_VALUE 0

extern "C"{
typedef void* HSN3DSDKSERVICE;
/*!
* @brief SDK环境初始化， 在使用SDK前必须初始化，只能初始化一次
* @param [out] hService 服务句柄
* @param [in] iDeviceType 初始化设备类型
* @return 0初始化成功，失败参考错误码列表
*/
SDKSOCKETWRAPPER_API int Sn3DInitializeSocket(HSN3DSDKSERVICE* hService, int iDeviceType);

/*!
* @brief SDK释放，不在调用SDK，可以调用该函数
* @param [in]  hService 服务句柄
* @return 0成功，失败参考错误码列表
*/
SDKSOCKETWRAPPER_API int Sn3DReleaseSocket(HSN3DSDKSERVICE* pService);
}

