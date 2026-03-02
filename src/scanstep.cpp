#include "scanstep.h"

bool ScanStep::hasEnterScan()
{
    if (SNSDK_DEVTYPE_COMBO == m_iDeviceType||SNSDK_DEVTYPE_COMBOPLUS == m_iDeviceType)
        return true;
    return false;
}

int ScanStep::deviceType()
{
	return m_iDeviceType;
}

void ScanStep::setDeviceType(int iDeviceType)
{
    m_iDeviceType = iDeviceType;
}

int ScanStep::scanMode()
{
    return m_iScanMode;
}

void ScanStep::setScanMode(int iMode)
{
    m_iScanMode = iMode;
}

bool ScanStep::enablePrescan()
{
    return m_bEnablePrescan;
}

void ScanStep::setEnablePrescan(bool bEnable)
{
    m_bEnablePrescan = bEnable;
}

int ScanStep::scanState()
{
    return m_iScanState;
}

void ScanStep::setScanState(int iState)
{
    m_iScanState = iState;
}