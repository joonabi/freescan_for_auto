#ifndef PGFITEM_H
#define PGFITEM_H
#include "qvariant.h"
#include "qstring.h"

class PGFItem
{
public:
    explicit PGFItem();
    /**
    *@brief Display text in the item
    */
    QString displayText() const;
    /**
    *@brief Real file name
    */
    QString pgfFile() const;
    void setPGFFile(const QString& strPGFFile);
    virtual operator QVariant() const;
private:
    QString m_strDisplayText;
    QString m_strPGFFile;
};
Q_DECLARE_METATYPE(PGFItem)
#endif // PGFITEM_H
