#pragma once
#include "qlist.h"
#include "qvariant.h"

class Calibrator
{
public:
    /**
    *@brief 获取开始标定命令
    */
    virtual QVariantMap getStartCalibCmd();
    /**
    *@brief 获取结束标定命令
    */
    virtual QVariantMap getStopCalibCmd();
    /**
    *@brief 获取退出标定命令
    */
    virtual QVariantMap getExitCalibCmd();
    /**
    *@brief 获取改变标定类型命令
    */
    virtual QVariantMap getChangeCalibModeCmd(int iMode);
};

