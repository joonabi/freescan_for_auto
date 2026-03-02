#include "util.h"
#include "glog/logging.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qexception.h"

std::string demo_util::map2str(const QVariantMap map1)
{
    QJsonDocument doc;
    doc = doc.fromVariant(map1);
    QString str = doc.toJson();
    str.replace("\n", "");
    return str.toStdString();
}

QString demo_util::map2qstr(const QVariantMap map1)
{
    QJsonDocument doc;
    doc = doc.fromVariant(map1);
    QString str = doc.toJson();
    str.replace("\n", "");
    return str;
}

QVariantMap demo_util::str2map(const QString str1)
{
    QVariantMap ret;
    try {
        QJsonDocument doc;
        doc = doc.fromJson(str1.toLocal8Bit());
        auto obj = doc.object();
        ret = obj.toVariantMap();
    }
    catch (QException exp) {
        VLOG(google::GLOG_INFO) << __FUNCTION__ << " " << exp.what();
    }
    return ret;
}