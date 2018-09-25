#ifndef TICKETSMODEL_H
#define TICKETSMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "ticket.h"

class TicketsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TicketsModel(QObject* parent = nullptr, const QVector<Ticket> &data = {});
    virtual int rowCount(const QModelIndex &) const override;
    virtual int columnCount(const QModelIndex &) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    Ticket getTicket(int index);

private:
    QVector<Ticket> data_;
};

#endif // TICKETSMODEL_H
