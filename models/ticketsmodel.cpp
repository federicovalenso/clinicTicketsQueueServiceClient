#include "ticketsmodel.h"
#include <QColor>
#include <QDateTime>
#include <QDebug>

TicketsModel::TicketsModel(QObject *parent, const QVector<Ticket> &data)
    : QAbstractTableModel(parent), data_(data) {}

int TicketsModel::rowCount(const QModelIndex &) const { return data_.size(); }

int TicketsModel::columnCount(const QModelIndex &) const { return 2; }

QVariant TicketsModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole) {
    if (index.column() == 0) {
      return data_.at(index.row()).ticket_number;
    } else if (index.column() == 1) {
      return QDateTime::fromMSecsSinceEpoch(data_.at(index.row()).created_at *
                                            1000)
          .time();
    }
  } else if (role == Qt::TextAlignmentRole) {
    return Qt::AlignCenter;
  } else if (role == Qt::BackgroundRole) {
    QColor color;
    if (data_.at(index.row()).is_manual == 1) {
      color.setRgb(255, 0, 0);
    } else if (index.row() % 2 == 0) {
      color.setRgb(72, 219, 251);
    } else {
      color.setRgb(0, 210, 211);
    }
    color.setAlpha(70);
    return color;
  }
  return QVariant();
}

QVariant TicketsModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const {
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

Qt::ItemFlags TicketsModel::flags(const QModelIndex &index) const {
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

Ticket TicketsModel::getTicket(int index) { return data_.at(index); }
