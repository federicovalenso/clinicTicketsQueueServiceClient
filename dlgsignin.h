#ifndef DLGSIGNIN_H
#define DLGSIGNIN_H

#include "appsettings.h"
#include "requestsprocessor.h"

#include <QDialog>
#include <QAbstractButton>
#include <QString>

namespace Ui {
class DlgSignIn;
}

class DlgSignIn : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSignIn(QWidget *parent = 0);
    ~DlgSignIn();

signals:
    void loginAccepted(const QString& userName);

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void loginRequestFinished();

private:
    Ui::DlgSignIn *ui;
    AppSettings& mSettings = AppSettings::getInstance();
    RequestsProcessor *mRequestsProcessor;
    QNetworkAccessManager *mNetworkManager;
};

#endif // DLGSIGNIN_H
