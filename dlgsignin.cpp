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
      ui_(new Ui::DlgSignIn),
      requests_processor_(rp) {
  ui_->setupUi(this);
  ui_->leUser->setText(settings_.getUserName());
  connect(requests_processor_, &RequestsProcessor::loginError, this, [this] {
    if (!isVisible()) show();
    QMessageBox::critical(this, tr("Ошибка"), tr("Ошибка авторизации"),
                          QMessageBox::Close);
  });
  connect(requests_processor_, SIGNAL(loginFinished()), this,
          SLOT(loginRequestFinished()));
  disconnect(ui_->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

DlgSignIn::~DlgSignIn() { delete ui_; }

void DlgSignIn::on_buttonBox_clicked(QAbstractButton* button) {
  if (qobject_cast<QPushButton*>(button) ==
      ui_->buttonBox->button(QDialogButtonBox::Ok)) {
    requests_processor_->login(ui_->leUser->text(), ui_->lePassword->text());
  }
}

void DlgSignIn::loginRequestFinished() {
  QString userName = ui_->leUser->text();
  settings_.setUserName(userName);
  disconnect(requests_processor_, &RequestsProcessor::requestError, this,
             nullptr);
  emit accept();
}

void DlgSignIn::showEvent(QShowEvent*) {
  connect(requests_processor_, &RequestsProcessor::requestError, this,
          [this](const QString& error) {
            QMessageBox::critical(this, tr("Ошибка"), error,
                                  QMessageBox::Close);
            SettingsDialog dlg(this);
            dlg.exec();
          });
}
