#include <QDateTime>
#include "ticketsmodel.h"

TicketsModel::TicketsModel(QObject *parent, const QVector<Ticket> &data)
    : QAbstractTableModel (parent)
    , data_(data)
{}

int TicketsModel::rowCount(const QModelIndex &) const
{
    return data_.size();
}

int TicketsModel::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant TicketsModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return data_.at(index.row()).ticket_number;
        } else if (index.column() == 1) {
            return QDateTime::fromSecsSinceEpoch(data_.at(index.row()).created_at).time();
        };
    }
    return QVariant();
}

QVariant TicketsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section == 0) {
                return QString(tr("Талон"));
            } else if (section == 1) {
                return QString(tr("Время создания"));
            }
        }
    }
    return QVariant();
}

Qt::ItemFlags TicketsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

Ticket TicketsModel::getTicket(int index)
{
    return data_.at(index);
}
