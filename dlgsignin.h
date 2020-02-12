#ifndef DLGSIGNIN_H
#define DLGSIGNIN_H

#include "appsettings.h"
#include "requestsprocessor.h"

#include <QAbstractButton>
#include <QDialog>
#include <QString>

namespace Ui {
class DlgSignIn;
}

namespace vvf {

class DlgSignIn : public QDialog {
  Q_OBJECT

 public:
  DlgSignIn(QWidget *parent, RequestsProcessor *rp);
  ~DlgSignIn();

 private slots:
  void on_buttonBox_clicked(QAbstractButton *button);
  void loginRequestFinished();

 private:
  Ui::DlgSignIn *ui_;
  AppSettings &settings_ = AppSettings::getInstance();
  RequestsProcessor *requests_processor_;

  // QWidget interface
 protected:
  void showEvent(QShowEvent *) override;
};

}  // namespace vvf

#endif  // DLGSIGNIN_H
