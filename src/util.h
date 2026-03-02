#pragma once
#include <string>
#include "qvariant.h"

namespace demo_util
{
    std::string map2str(const QVariantMap map1);
    QString map2qstr(const QVariantMap map1);
    QVariantMap str2map(const QString str1);
}

