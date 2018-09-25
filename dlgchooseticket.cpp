#include <memory>
#include "dlgchooseticket.h"
#include "ui_dlgchooseticket.h"

DlgChooseTicket::DlgChooseTicket(QWidget *parent, const QVector<Ticket>& data) :
    QDialog(parent),
    ui(new Ui::DlgChooseTicket)
{
    ui->setupUi(this);
    model = new TicketsModel(this, data);
    ui->tvTickets->setModel(model);
}

DlgChooseTicket::~DlgChooseTicket()
{
    delete ui;
}

Ticket DlgChooseTicket::currentTicket()
{
    Ticket result;
    int current_row = ui->tvTickets->currentIndex().row();
    if (current_row >= 0) {
        result = model->getTicket(current_row);
    }
    return result;
}

Ticket DlgChooseTicket::getTicket(QWidget *parent, const QVector<Ticket> &data)
{
    Ticket result;
    std::unique_ptr<DlgChooseTicket> dlg = std::make_unique<DlgChooseTicket>(parent, data);
    if (dlg->exec() == QDialog::Accepted) {
        result = dlg->currentTicket();
    }
    return result;
}

void DlgChooseTicket::resizeEvent(QResizeEvent *event)
{
    int column_count = ui->tvTickets->model()->columnCount();
    for (int i = 0; i < column_count; i++) {
        ui->tvTickets->setColumnWidth(i, ui->tvTickets->width() / column_count - 5);
    }
    QDialog::resizeEvent(event);
}

void DlgChooseTicket::on_tvTickets_doubleClicked(const QModelIndex &)
{
    emit accept();
}
