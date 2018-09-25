#include <QTreeWidgetItemIterator>
#include <QVector>
#include <QMessageBox>
#include <QCloseEvent>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dlgsettings.h"
#include "dlgchooseticket.h"

const TicketActions MainWindow::mTicketActions = {
    {"book", "Плановая запись"},
    {"disp", "Диспансеризация"},
    {"receipt", "Льготные рецепты"},
    {"emergency", "Острое состояние"},
    {"report", "Справки, выписки"},
    {"vaccine", "Вакцинация"},
    {"other", "Прочее"}
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for (auto it = mTicketActions.begin(); it != mTicketActions.end(); it++) {
        QTreeWidgetItem* twItem = new QTreeWidgetItem(ui->twTicketActions);
        twItem->setText(ACTION_COL_NUMBER, it.key());
        twItem->setText(TRANSLATED_ACTION_COL_NUMBER, it.value());
        twItem->setFlags(twItem->flags() | Qt::ItemIsUserCheckable);
        twItem->setCheckState(TRANSLATED_ACTION_COL_NUMBER, Qt::Checked);
        ui->twTicketActions->addTopLevelItem(twItem);
    }
    ui->twTicketActions->hideColumn(ACTION_COL_NUMBER);
    ticketsProcessor = new TicketsProcessor(this);
    connect(ticketsProcessor,
            &TicketsProcessor::ticketError,
            this,
            [=] (const QString& error)
            {
                QMessageBox::critical(this, tr("Ошибка при получении талона"), error, QMessageBox::Close);
            });
    connect(ticketsProcessor,
            &TicketsProcessor::receivedTicket,
            this,
            [=] (const QString& ticketNumber)
            {
                ui->lblTicketNumber->setText(ticketNumber);
                if (current_state_ == STATES::CLOSE) QMainWindow::close();
            });
    connect(ticketsProcessor,
            &TicketsProcessor::receivedTickets,
            this,
            [=] (const QVector<Ticket>& tickets)
            {
                if (current_state_ == STATES::NEXT_TICKET) {
                    auto oldestTicket = std::min_element(tickets.begin(), tickets.end());
                    ticketsProcessor->lockTicket(std::move(*oldestTicket));
                } else {
                    Ticket ticket = DlgChooseTicket::getTicket(this, tickets);
                    if (ticket.isValid()) {
                        ticketsProcessor->lockTicket(std::move(ticket));
                    }
                }
            });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUserName(const QString &userName)
{
    ui->lblUserName->setText(userName);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (ticketsProcessor->hasActiveTicket() == true) {
        event->ignore();
        if (QMessageBox::warning(
                    this,
                    tr("Есть незавершенный талон!"),
                    tr("Закончить работу?"),
                    QMessageBox::Yes,
                    QMessageBox::No) == QMessageBox::Yes) {
            ticketsProcessor->finishCurrentTicket();
            current_state_ = STATES::CLOSE;
        }
    }
}

void MainWindow::on_btnNext_clicked()
{
    getTickets();
    current_state_ = STATES::NEXT_TICKET;
}

void MainWindow::on_actionSettings_triggered()
{
    dlgSettings dlg;
    dlg.exec();
}

void MainWindow::on_btnVoiceTicket_clicked()
{
    ticketsProcessor->voiceTicket();
}

void MainWindow::on_btnSelect_clicked()
{
    getTickets();
    current_state_ = STATES::CHOOSE_TICKET;
}

void MainWindow::getTickets()
{
    QVector<QString> actions;
    QTreeWidgetItemIterator actionsIt(ui->twTicketActions);
    while (*actionsIt) {
        if ((*actionsIt)->checkState(TRANSLATED_ACTION_COL_NUMBER) == Qt::Checked) {
            actions.push_back((*actionsIt)->text(ACTION_COL_NUMBER));
        }
        actionsIt++;
    }
    ticketsProcessor->getTickets(std::move(actions));
}
