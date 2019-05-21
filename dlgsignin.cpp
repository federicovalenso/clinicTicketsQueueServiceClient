#include "dlgsignin.h"
#include <QDebug>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QPushButton>
#include <QUrl>
#include <QUrlQuery>
#include "dlgsettings.h"
#include "ui_dlgsignin.h"

using namespace vvf;

DlgSignIn::DlgSignIn(QWidget* parent, RequestsProcessor* rp)
    : QDialog(parent, Qt::WindowCloseButtonHint),
      ui(new Ui::DlgSignIn),
      mRequestsProcessor(rp) {
  ui->setupUi(this);
  ui->leUser->setText(mSettings.getUserName());
  connect(mRequestsProcessor, &RequestsProcessor::requestError, this,
          [=](const QString& error) {
            QMessageBox::critical(this, tr("Ошибка"), error,
                                  QMessageBox::Close);
            SettingsDialog dlg;
            dlg.exec();
          });
  connect(mRequestsProcessor, SIGNAL(loginFinished()), this,
          SLOT(loginRequestFinished()));
  disconnect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

DlgSignIn::~DlgSignIn() { delete ui; }

void DlgSignIn::on_buttonBox_clicked(QAbstractButton* button) {
  if (qobject_cast<QPushButton*>(button) ==
      ui->buttonBox->button(QDialogButtonBox::Ok)) {
    mRequestsProcessor->sendLoginRequest(ui->leUser->text(),
                                         ui->lePassword->text());
  }
}

void DlgSignIn::loginRequestFinished() {
  QString userName = ui->leUser->text();
  mSettings.setUserName(userName);
  emit accept();
}
