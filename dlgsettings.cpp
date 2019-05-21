#include "dlgsettings.h"
#include "ui_dlgsettings.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgSettings)
{
    ui->setupUi(this);
    ui->leServer->setText(mSettings.getServerAddr());
    ui->sbPort->setValue(mSettings.getPort());
    ui->sbWindow->setValue(mSettings.getWindowNumber());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttonBox_accepted()
{
    mSettings.setConnectionSettings(ui->leServer->text(),
                                    ui->sbPort->value());
    mSettings.setWindowNumber(ui->sbWindow->value());
}
