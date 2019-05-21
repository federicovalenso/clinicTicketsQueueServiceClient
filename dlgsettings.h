#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "appsettings.h"

namespace Ui {
class dlgSettings;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::dlgSettings *ui;
    AppSettings& mSettings = AppSettings::getInstance();
};

#endif // DLGSETTINGS_H
