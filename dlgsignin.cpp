#include "dlgsignin.h"
#include "ui_dlgsignin.h"

#include <QMessageBox>
#include <QPushButton>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QDebug>

DlgSignIn::DlgSignIn(QWidget *parent) :
    QDialog(parent, Qt::WindowCloseButtonHint),
    ui(new Ui::DlgSignIn)
{
    ui->setupUi(this);
    ui->leUser->setText(mSettings.getUserName());
    mRequestsProcessor = new RequestsProcessor(this);
    connect(mRequestsProcessor,
            &RequestsProcessor::requestError,
            this,
            [=] (const QString& error)
            {
                QMessageBox::critical(this, tr("Ошибка"), error, QMessageBox::Close);
            });
    connect(mRequestsProcessor,
            SIGNAL(loginFinished()),
            this,
            SLOT(loginRequestFinished()));
    disconnect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

DlgSignIn::~DlgSignIn() {
    delete ui;
}

void DlgSignIn::on_buttonBox_clicked(QAbstractButton *button) {
    if (qobject_cast<QPushButton*>(button) == ui->buttonBox->button(QDialogButtonBox::Ok)) {
        mRequestsProcessor->sendLoginRequest(ui->leUser->text(), ui->lePassword->text());
    }
}

void DlgSignIn::loginRequestFinished()
{
    QString userName = ui->leUser->text();
    mSettings.setUserName(userName);
    emit loginAccepted(userName);
    emit accept();
}
