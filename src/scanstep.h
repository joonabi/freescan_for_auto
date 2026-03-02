#pragma once
#include "windows.h"
#include "sn3dsdkcorepub.h"
#include "sn3dsdkpluginpub.h"

class ScanStep
{
public:
	int deviceType();
    void setDeviceType(int iDeviceType);
    bool hasEnterScan();
    /**
    *@brief Laser or Infared
    */
    int scanMode();
    void setScanMode(int iMode);
    /**
    *@brief enable prescan
    */
    bool enablePrescan();
    void setEnablePrescan(bool bEnable);
    /**
    *@brief Scan state
    */
    int scanState();
    void setScanState(int iState);
private:
    int m_iDeviceType = SNSDK_DEVTYPE_UEPRO;
    int m_iScanMode = SNSDK_SCANMODE_LASER;
    int m_iScanState = SNSDK_SCAN_STOPED;
    bool m_bEnablePrescan = false;
};

