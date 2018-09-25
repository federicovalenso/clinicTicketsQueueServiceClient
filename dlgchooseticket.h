#ifndef DLGCHOOSETICKET_H
#define DLGCHOOSETICKET_H

#include <QVector>
#include <QDialog>
#include "ticket.h"
#include "models/ticketsmodel.h"

namespace Ui {
class DlgChooseTicket;
}

class DlgChooseTicket : public QDialog
{
    Q_OBJECT

public:
    explicit DlgChooseTicket(QWidget *parent = nullptr, const QVector<Ticket> &data = {});
    ~DlgChooseTicket();
    Ticket currentTicket();
    static Ticket getTicket(QWidget *parent = nullptr, const QVector<Ticket> &data = {});

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_tvTickets_doubleClicked(const QModelIndex &);

private:
    Ui::DlgChooseTicket *ui;
    TicketsModel* model;
};

#endif // DLGCHOOSETICKET_H
