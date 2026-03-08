#ifdef SDKMODBUSWRAPPER_EXPORTS
#define SDKMODBUSWRAPPER_API __declspec(dllexport)
#else
#define SDKMODBUSWRAPPER_API __declspec(dllimport)
#endif
#include "SDKCWrapper.h"

/*!
* @brief Initialize sn3d modbus
* @param hModbus[out] modbus handle
* @param iDeviceType[in] device type
* @return SNSDK_ERR_OK successful, others fail
*/
SDKMODBUSWRAPPER_API int WINAPI Sn3DInitializeModbus(HSN3DSDKSERVICE* phModbus, int iDeviceType);

/*!
* @brief Release sn3d modbus
* @param hModbus[in and out] modbus handle
* @return SNSDK_ERR_OK successful, others fail
*/
SDKMODBUSWRAPPER_API int WINAPI Sn3DReleaseModbus(HSN3DSDKSERVICE* phModbus);

/*!
* @brief start modbus service
* @param hModbus[in] modbus handle
* @param hService[in] service handle
* @return SNSDK_ERR_OK successful, others fail
* @note Modbus is just use hService, but don't allocate and release it
*/
// SDKMODBUSWRAPPER_API int WINAPI Sn3DModbusStart(HSN3DSDKSERVICE hModbus, HSN3DSDKSERVICE hService);


/*!
* @brief start modbus service
* @param hModbus[in] modbus handle
* @param hService[in] service handle
* @return SNSDK_ERR_OK successful, others fail
* @note Modbus is just use hService, but don't allocate and release it
*/
// SDKMODBUSWRAPPER_API int WINAPI Sn3DModbusStop(HSN3DSDKSERVICE hModbus, HSN3DSDKSERVICE hService);

