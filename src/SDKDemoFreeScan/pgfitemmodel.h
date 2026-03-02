#ifndef PGFITEMMODEL_H
#define PGFITEMMODEL_H

#include <QAbstractListModel>
#include <qlist.h>
#include "pgfitem.h"
class PGFItemModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PGFItemModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    QList<PGFItem> m_listItem;
};

#endif // PGFITEMMODEL_H
