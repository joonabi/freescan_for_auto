#include "snjsondocument.h"

QString SnJsonDocument::findStrFromJson(const QString& strTag, const QString& strJson)
{
    QString ret;
    auto strTagWithQuot = "\"" + strTag + "\"";
    auto iTagIdx = strJson.indexOf(strTagWithQuot);
    if (iTagIdx == -1)
        return ret;
    iTagIdx += strTagWithQuot.length();
    auto iColonIdx = strJson.indexOf(":", iTagIdx);
    iColonIdx += 1;
    auto iCommaIdx = strJson.indexOf(",", iColonIdx);
    if (iCommaIdx == -1)
        return ret;
    ret = strJson.mid(iColonIdx, iCommaIdx - iColonIdx);
    ret = ret.trimmed();
    // 如果前后是引号，则移除
    if (ret.length() > 2) {
        if (ret[0] == "\"")
            ret = ret.right(ret.length() - 1);
        if (ret[ret.length() - 1] == "\"")
            ret = ret.left(ret.length() - 1);
    }
    return ret;
}

bool SnJsonDocument::findDblFromJson(const QString& strTag, const QString& strJson, double& dValue)
{
    bool ret = false;
    auto str = findStrFromJson(strTag, strJson);
    if (!str.isEmpty()) {
        dValue = str.toDouble(&ret);
    }
    return ret;
}

bool SnJsonDocument::findBoolFromJson(const QString& strTag, const QString& strJson)
{
    bool ret = false;
    auto str = findStrFromJson(strTag, strJson);
    if (!str.isEmpty()) {
        str = str.trimmed();
        ret = str.compare("true", Qt::CaseInsensitive) == 0;
    }
    return ret;
}

QByteArray SnJsonDocument::getLegalChar(const QString& strJson)
{
    QByteArray ret;
    auto buffer = strJson.toLocal8Bit();
    for (int i = 0; i < buffer.length(); i++) {
        int iCode = buffer[i];
        if (iCode >= 0)
            ret += (char)iCode;
    }
    return ret;
}