#include "scanresultfile.h"

QList<QString> ScanResultFile::prjNameList() const
{
    return m_listPrjName;
}

void ScanResultFile::setPrjNameList(const QList<QString>& list)
{
    m_listPrjName = list;
}

QString ScanResultFile::folder() const
{
    return m_strSaveFolder;
}

void ScanResultFile::setFolder(const QString& strFolder)
{
    m_strSaveFolder = strFolder;
}

QVariantMap ScanResultFile::cmdForSave(bool bSaveASC, bool bSaveP3, bool bSaveTb)
{
    QVariantList listPrjName;
    listPrjName.append("Project1");
    return { {"cmd", "saveScanDataToSrvCmd"},{"prjNameList", listPrjName},{"saveFolder", m_strSaveFolder},
    {"saveAscFile",bSaveASC},{"saveP3File", bSaveP3},{"saveTbFile", bSaveTb} };
}

QVariantMap ScanResultFile::cmdForSaveASC()
{
    return cmdForSave(true, true, false);
}
