#include "mainwindow.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QTreeWidgetItemIterator>
#include <QVector>
#include "dlgsettings.h"
#include "ui_mainwindow.h"

using namespace vvf;

const TicketActions MainWindow::kTicketActions = {
    {"book", "Плановая запись"},
    {"disp", "Диспансеризация"},
    {"receipt", "Льготные рецепты"},
    {"emergency", "Острое состояние"},
    {"report", "Справки, выписки"},
    {"vaccine", "Вакцинация"},
    {"other", "Прочее"}};

MainWindow::MainWindow(QWidget* parent, RequestsProcessor* rp)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  for (auto it = kTicketActions.begin(); it != kTicketActions.end(); ++it) {
    QTreeWidgetItem* twItem = new QTreeWidgetItem(ui_->twTicketActions);
    twItem->setText(kActionColNumber, it.key());
    twItem->setText(kTranslatedActionColNumber, it.value());
    twItem->setFlags(twItem->flags() | Qt::ItemIsUserCheckable);
    twItem->setCheckState(kTranslatedActionColNumber, Qt::Checked);
    ui_->twTicketActions->addTopLevelItem(twItem);
  }
  ui_->twTicketActions->hideColumn(kActionColNumber);

  AppSettings& settings = AppSettings::getInstance();
  restoreGeometry(settings.getMainWindowGeometry());
  restoreState(settings.getMainWindowState());
  setUserName(settings.getUserName());

  tickets_processor_ = new TicketsProcessor(this, rp);
  timer_ = new QTimer(this);
  connect(tickets_processor_, &TicketsProcessor::ticketError, this,
          [=](const QString& error) { state_->error(this, error); });
  connect(tickets_processor_, &TicketsProcessor::receivedTicket, this,
          [=](const Ticket& ticket) { state_->processTicket(this, ticket); });
  connect(tickets_processor_, &TicketsProcessor::receivedTickets, this,
          [=](const QVector<Ticket>& tickets) {
            state_->processTickets(this, tickets);
          });
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::setUserName(const QString& userName) {
  ui_->lblUserName->setText(userName);
}

void MainWindow::closeEvent(QCloseEvent* event) {
  state_->close(this, event);
  AppSettings& settings = AppSettings::getInstance();
  settings.setSizeSettings(saveGeometry(), saveState());
  QMainWindow::closeEvent(event);
}

void MainWindow::on_btnNext_clicked() { state_->nextTicket(this); }

void MainWindow::on_actionSettings_triggered() {
  SettingsDialog dlg(this);
  dlg.exec();
}

void MainWindow::on_btnVoiceTicket_clicked() { state_->voiceTicket(this); }

void MainWindow::on_btnSelect_clicked() { state_->selectTicket(this); }

void MainWindow::on_btnReturn_clicked() { state_->returnTicket(this); }

void MainWindow::getTickets(SelectModes mode) {
  QVector<QString> actions;
  QTreeWidgetItemIterator actionsIt(ui_->twTicketActions);
  while (*actionsIt) {
    if ((*actionsIt)->checkState(kTranslatedActionColNumber) == Qt::Checked) {
      actions.push_back((*actionsIt)->text(kActionColNumber));
    }
    ++actionsIt;
  }
  tickets_processor_->getTickets(actions, mode);
}

void MainWindow::switchWorkingState() {
  ui_->btnNext->setEnabled(is_working_);
  ui_->btnSelect->setEnabled(is_working_);
  ui_->btnVoiceTicket->setEnabled(is_working_);
  ui_->btnReturn->setEnabled(is_working_);
  if (is_working_) {
    ui_->btnSwitchWorkingState->setText(tr("Закончить работу"));
    QIcon icon;
    icon.addFile(QStringLiteral(":/icons/icons/switch-off.svg"), QSize(),
                 QIcon::Normal, QIcon::Off);
    ui_->btnSwitchWorkingState->setIcon(icon);
    connect(timer_, &QTimer::timeout, this,
            [=]() { state_->background(this); });
    timer_->start(5000);
  } else {
    ui_->btnSwitchWorkingState->setText(tr("Начать работу"));
    QIcon icon;
    icon.addFile(QStringLiteral(":/icons/icons/switch-on.svg"), QSize(),
                 QIcon::Normal, QIcon::Off);
    ui_->btnSwitchWorkingState->setIcon(icon);
    if (tickets_processor_->hasActiveTicket()) {
      tickets_processor_->finishCurrentTicket();
    }
    timer_->disconnect();
  }
}

void MainWindow::setButtonsEnabled(bool mode) {
  ui_->btnNext->setEnabled(mode);
  ui_->btnReturn->setEnabled(mode);
  ui_->btnSelect->setEnabled(mode);
  ui_->btnSwitchWorkingState->setEnabled(mode);
  ui_->btnVoiceTicket->setEnabled(mode);
}

void MainWindow::on_btnSwitchWorkingState_clicked() {
  is_working_ = !is_working_;
  switchWorkingState();
}

void MainWindow::changeState(AppState* state) { state_ = state; }
