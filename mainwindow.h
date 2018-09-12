#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include "ticketsprocessor.h"

using TicketActions = QMap<QString, QString>;

namespace Ui {
class MainWindow;
}

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

private:
    Ui::MainWindow *ui;
    TicketsProcessor* ticketsProcessor;
    bool isClosingState = false;
    static const int ACTION_COL_NUMBER = 0;
    static const int TRANSLATED_ACTION_COL_NUMBER = 1;
    static const TicketActions mTicketActions;
};

#endif // MAINWINDOW_H
