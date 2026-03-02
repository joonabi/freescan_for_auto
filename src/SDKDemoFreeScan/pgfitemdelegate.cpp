#include "pgfitemdelegate.h"
#include "qpainter.h"
#include <QStyleOptionViewItem>
#include "pgfitem.h"

PGFItemDelegate::PGFItemDelegate(QObject *parent)
    :QAbstractItemDelegate(parent)
{

}

// painting
void PGFItemDelegate::paint(QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    if (index.column() == 0) {
        auto strText = index.model()->data(index).toString();
        painter->drawText(QPoint(0, index.row() * 33), strText);
    }
    /*PGFItem item = index.model()->data(index).value<PGFItem>();
    auto strText = item.displayText();
    
    painter->drawText(QPoint(0, index.row() * 33), item.displayText());*/
}

QSize PGFItemDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    return QSize(100, 33);
}
