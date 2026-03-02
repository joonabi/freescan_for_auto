#include "pgfitem.h"
#include "qfileinfo.h"

PGFItem::PGFItem()
{

}

QString PGFItem::displayText() const
{
    QFileInfo info(m_strPGFFile);
    auto strFileName = info.fileName();
    return info.fileName();
}

QString PGFItem::pgfFile() const
{
    return m_strPGFFile;
}

void PGFItem::setPGFFile(const QString& strPGFFile)
{
    m_strPGFFile = strPGFFile;
}

PGFItem::operator QVariant() const
{
    return QVariant::fromValue(*this);
}