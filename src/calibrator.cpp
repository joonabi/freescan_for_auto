#include "calibrator.h"

QVariantMap Calibrator::getStartCalibCmd()
{
    QVariantMap mapParam = { {"cmd", "clickCalibBtn"},{"capture", true} };
    return mapParam;
}

QVariantMap Calibrator::getStopCalibCmd()
{
    QVariantMap mapParam = { {"cmd", "clickCalibBtn"},{"capture", true} };
    return mapParam;
}

QVariantMap Calibrator::getExitCalibCmd()
{
    QVariantMap mapParam = { {"cmd", "test"} };
    return mapParam;
}

QVariantMap Calibrator::getChangeCalibModeCmd(int iMode)
{
    QVariantMap mapParam = { {"cmd", "setCalibMode2"},{"mode", iMode} };
    return mapParam;
}