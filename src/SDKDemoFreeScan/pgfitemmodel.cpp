#include "pgfitemmodel.h"

PGFItemModel::PGFItemModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant PGFItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (0 == section)
        return "PGF file";
    if (1 == section)
        return "Operation";
    return QVariant();
}

QModelIndex PGFItemModel::index(int row, int column, const QModelIndex &parent) const
{
    /*return !parent.isValid()
        && row >= 0 && row < m_listItem.size()
        && column >= 0 && column < 2
        ? createIndex(row, column)
        : QModelIndex();*/
    return createIndex(row, column);
}

QModelIndex PGFItemModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int PGFItemModel::rowCount(const QModelIndex &parent) const
{
    return m_listItem.size();
}

int PGFItemModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant PGFItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int iRow = index.row();
    if (iRow < m_listItem.count()) {
        // return m_listItem[iRow];
        if (index.column() == 0) {
            return m_listItem[iRow].displayText();
        }
        else if (index.column() == 1) {
            return m_listItem[iRow].pgfFile();
        }
    }
    return QVariant();
}

bool PGFItemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; i++) {
         PGFItem item;
         m_listItem.append(item);
    }
    endInsertRows();
    return true;
}

bool PGFItemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; i++) {
        m_listItem.removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool PGFItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    int iRow = index.row();
    if (iRow < m_listItem.count()) {
        // PGFItem item = value.value<PGFItem>();
        // m_listItem[iRow] = item;
        // m_listItem[iRow].setPGFFile(item.pgfFile());
        /*if (index.column() == 0) {
            m_listItem[iRow].setPGFFile(item.pgfFile());
        }*/
        // else if (index.column() == 1) {
        if (index.column() == 1) {
            auto strPGFFile = value.toString();
            m_listItem[iRow].setPGFFile(strPGFFile);
        }
    }
    return true;
}