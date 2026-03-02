#ifndef PGFITEMDELEGATE_H
#define PGFITEMDELEGATE_H

#include "qabstractitemdelegate.h"

class PGFItemDelegate : public QAbstractItemDelegate
{
public:
    explicit PGFItemDelegate(QObject *parent = nullptr);
    // painting
    virtual void paint(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const;
};

#endif // PGFITEMDELEGATE_H
