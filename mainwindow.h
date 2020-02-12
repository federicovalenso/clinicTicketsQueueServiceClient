#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QTimer>
#include "states/appstate.h"
#include "ticketsprocessor.h"

namespace Ui {
class MainWindow;
}

namespace vvf {

using TicketActions = QMap<QString, QString>;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent, RequestsProcessor* rp);
  ~MainWindow() override;

 public slots:
  void setUserName(const QString& userName);

 protected:
  virtual void closeEvent(QCloseEvent* event) override;

 private slots:
  void on_btnNext_clicked();
  void on_actionSettings_triggered();
  void on_btnVoiceTicket_clicked();
  void on_btnSelect_clicked();
  void on_btnSwitchWorkingState_clicked();
  void on_btnReturn_clicked();

 private:
  static const int kActionColNumber = 0;
  static const int kTranslatedActionColNumber = 1;
  static const TicketActions kTicketActions;
  friend class AppState;
  Ui::MainWindow* ui_;
  TicketsProcessor* tickets_processor_;
  QTimer* timer_;

  AppState* state_ = BackgroundState::getInstance();
  bool is_working_ = false;

  void changeState(AppState* state);
  void getTickets(SelectModes mode = SelectModes::AUTO);
  void switchWorkingState();
  void setButtonsEnabled(bool mode);
};

}  // namespace vvf

#endif  // MAINWINDOW_H
