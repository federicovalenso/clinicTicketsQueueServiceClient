#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QTimer>
#include "ticketsprocessor.h"
#include "states/appstate.h"

namespace Ui {
class MainWindow;
}

namespace vvf {

using TicketActions = QMap<QString, QString>;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setUserName(const QString& userName);

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnNext_clicked();
    void on_actionSettings_triggered();
    void on_btnVoiceTicket_clicked();
    void on_btnSelect_clicked();
    void on_btnSwitchWorkingState_clicked();
    void on_btnReturn_clicked();

private:
    friend class AppState;
    Ui::MainWindow *ui;
    TicketsProcessor* ticketsProcessor;
    QTimer* timer;
    static const int ACTION_COL_NUMBER = 0;
    static const int TRANSLATED_ACTION_COL_NUMBER = 1;
    static const TicketActions mTicketActions;
    AppState* state_ = BackgroundState::getInstance();
    bool isWorking = false;

    void changeState(AppState* state);
    void getTickets(SelectModes mode = SelectModes::AUTO);
    void changeButtonsStates();
};

} // namespace vvf

#endif // MAINWINDOW_H
