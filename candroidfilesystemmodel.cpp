#include "candroidfilesystemmodel.h"

QString CAndroidFileSystemModel::headerDatas[] = {
    QApplication::tr("privilege"),
    QApplication::tr("group"),
    QApplication::tr("owner"),
    QApplication::tr("size"),
    QApplication::tr("time"),
    QApplication::tr("file name"),
};

CAndroidFileSystemModel::CAndroidFileSystemModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant CAndroidFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {
        if(section >= 0 && section < headerDatas->size()) {
            return headerDatas[section];
        } else {
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

bool CAndroidFileSystemModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        headerDatas[section] = value.toString();
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

QModelIndex CAndroidFileSystemModel::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex CAndroidFileSystemModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int CAndroidFileSystemModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int CAndroidFileSystemModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

bool CAndroidFileSystemModel::hasChildren(const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

bool CAndroidFileSystemModel::canFetchMore(const QModelIndex &parent) const
{
    // FIXME: Implement me!
    return false;
}

void CAndroidFileSystemModel::fetchMore(const QModelIndex &parent)
{
    // FIXME: Implement me!
}

QVariant CAndroidFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

bool CAndroidFileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags CAndroidFileSystemModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool CAndroidFileSystemModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool CAndroidFileSystemModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
}

bool CAndroidFileSystemModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

bool CAndroidFileSystemModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
}
