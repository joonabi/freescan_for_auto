#pragma once
#include "qlist.h"
#include "qvariant.h"

class ScanResultFile
{
public:
    /**
    *@brief 工程名称列表
    */
    virtual QList<QString> prjNameList() const;
    virtual void setPrjNameList(const QList<QString>& list);
    /**
    *@brief 数据文件保存文件夹
    */
    virtual QString folder() const;
    virtual void setFolder(const QString& strFolder);
    /**
    *@brief 保存数据文件的命令
    */
    virtual QVariantMap cmdForSave(bool bSaveASC, bool bSaveP3, bool bSaveTb);
    /**
    *@brief 保存ASC数据文件的命令
    */
    virtual QVariantMap cmdForSaveASC();
private:
    QList<QString> m_listPrjName;
    QString m_strSaveFolder;
};

