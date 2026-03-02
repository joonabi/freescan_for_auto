#pragma once
#include "qstring.h"

class SnJsonDocument
{
public:
    static QString findStrFromJson(const QString& strTag, const QString& strJson);
    static bool findDblFromJson(const QString& strTag, const QString& strJson, double& dValue);
    static bool findBoolFromJson(const QString& strTag, const QString& strJson);
    /**
    *@brief 获取合法字符
    */
    static QByteArray getLegalChar(const QString& strJson);
};

