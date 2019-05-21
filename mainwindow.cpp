#include "mainwindow.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QTreeWidgetItemIterator>
#include <QVector>
#include "dlgsettings.h"
#include "ui_mainwindow.h"

using namespace vvf;

const TicketActions MainWindow::TICKET_ACTIONS = {
    {"book", "Плановая запись"},
    {"disp", "Диспансеризация"},
    {"receipt", "Льготные рецепты"},
    {"emergency", "Острое состояние"},
    {"report", "Справки, выписки"},
    {"vaccine", "Вакцинация"},
    {"other", "Прочее"}};

MainWindow::MainWindow(QWidget* parent, RequestsProcessor* rp)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  for (auto it = TICKET_ACTIONS.begin(); it != TICKET_ACTIONS.end(); it++) {
    QTreeWidgetItem* twItem = new QTreeWidgetItem(ui->twTicketActions);
    twItem->setText(ACTION_COL_NUMBER, it.key());
    twItem->setText(TRANSLATED_ACTION_COL_NUMBER, it.value());
    twItem->setFlags(twItem->flags() | Qt::ItemIsUserCheckable);
    twItem->setCheckState(TRANSLATED_ACTION_COL_NUMBER, Qt::Checked);
    ui->twTicketActions->addTopLevelItem(twItem);
  }
  ui->twTicketActions->hideColumn(ACTION_COL_NUMBER);

  AppSettings& settings = AppSettings::getInstance();
  restoreGeometry(settings.getMainWindowGeometry());
  restoreState(settings.getMainWindowState());
  setUserName(settings.getUserName());

  ticketsProcessor = new TicketsProcessor(this, rp);
  timer = new QTimer(this);
  connect(ticketsProcessor, &TicketsProcessor::ticketError, this,
          [=](const QString& error) { state_->error(this, error); });
  connect(ticketsProcessor, &TicketsProcessor::receivedTicket, this,
          [=](const Ticket& ticket) { state_->processTicket(this, ticket); });
  connect(ticketsProcessor, &TicketsProcessor::receivedTickets, this,
          [=](const QVector<Ticket>& tickets) {
            state_->processTickets(this, tickets);
          });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setUserName(const QString& userName) {
  ui->lblUserName->setText(userName);
}

void MainWindow::closeEvent(QCloseEvent* event) {
  state_->close(this, event);
  AppSettings& settings = AppSettings::getInstance();
  settings.setSizeSettings(saveGeometry(), saveState());
  QMainWindow::closeEvent(event);
}

void MainWindow::on_btnNext_clicked() { state_->nextTicket(this); }

void MainWindow::on_actionSettings_triggered() {
  SettingsDialog dlg;
  dlg.exec();
}

void MainWindow::on_btnVoiceTicket_clicked() { state_->voiceTicket(this); }

void MainWindow::on_btnSelect_clicked() { state_->selectTicket(this); }

void MainWindow::on_btnReturn_clicked() { state_->returnTicket(this); }

void MainWindow::getTickets(SelectModes mode) {
  QVector<QString> actions;
  QTreeWidgetItemIterator actionsIt(ui->twTicketActions);
  while (*actionsIt) {
    if ((*actionsIt)->checkState(TRANSLATED_ACTION_COL_NUMBER) == Qt::Checked) {
      actions.push_back((*actionsIt)->text(ACTION_COL_NUMBER));
    }
    actionsIt++;
  }
  ticketsProcessor->getTickets(actions, mode);
}

void MainWindow::changeButtonsStates() {
  ui->btnNext->setEnabled(isWorking);
  ui->btnSelect->setEnabled(isWorking);
  ui->btnVoiceTicket->setEnabled(isWorking);
  ui->btnReturn->setEnabled(isWorking);
  if (isWorking == true) {
    ui->btnSwitchWorkingState->setText(tr("Закончить работу"));
    QIcon icon;
    icon.addFile(QStringLiteral(":/icons/icons/switch-off.svg"), QSize(),
                 QIcon::Normal, QIcon::Off);
    ui->btnSwitchWorkingState->setIcon(icon);
    connect(timer, &QTimer::timeout, this, [=]() { state_->background(this); });
    timer->start(5000);
  } else {
    ui->btnSwitchWorkingState->setText(tr("Начать работу"));
    QIcon icon;
    icon.addFile(QStringLiteral(":/icons/icons/switch-on.svg"), QSize(),
                 QIcon::Normal, QIcon::Off);
    ui->btnSwitchWorkingState->setIcon(icon);
    if (ticketsProcessor->hasActiveTicket() == true) {
      ticketsProcessor->finishCurrentTicket();
    }
    timer->disconnect();
  }
}

void MainWindow::setButtonsEnabled(bool mode) {
  ui->btnNext->setEnabled(mode);
  ui->btnReturn->setEnabled(mode);
  ui->btnSelect->setEnabled(mode);
  ui->btnSwitchWorkingState->setEnabled(mode);
  ui->btnVoiceTicket->setEnabled(mode);
}

void MainWindow::on_btnSwitchWorkingState_clicked() {
  isWorking = !isWorking;
  changeButtonsStates();
}

void MainWindow::changeState(AppState* state) { state_ = state; }
