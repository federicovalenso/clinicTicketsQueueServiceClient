#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QString>
#include <QByteArray>

class AppSettings
{
public:
    static AppSettings& getInstance();
    void setConnectionSettings(const QString& serverAddr,
                               const int port);
    void setWindowNumber(const int windowNumber);
    void setSizeSettings(const QByteArray& mainWindowGeometry,
                         const QByteArray& mainWindowState);

    QString getServerAddr() const;
    int getPort() const;
    int getWindowNumber() const;
    QByteArray getMainWindowGeometry() const;
    QByteArray getMainWindowState() const;
    QByteArray getSelectTicketsDialogGeometry() const;
    QString getUserName() const;
    void setUserName(const QString &userName);
    void setMainWindowSettings(const QByteArray& geometry, const QByteArray& state);
    void setSelectTicketsDialogSettings(const QByteArray& geometry);


private:
    AppSettings();
    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(AppSettings&) = delete;
    void ReadSettings();

    const QString CONNECTION_GROUP = "/connection";
    const QString SERVER_ADDR = "server_addr";
    const QString PORT = "port";
    const QString SIZE_GROUP = "/size";
    const QString MAIN_WINDOW_GEOMETRY = "main_window_geometry";
    const QString MAIN_WINDOW_STATE = "main_window_state";
    const QString SELECT_TICKETS_DIALOG_GEOMETRY = "select_tickets_dialog_geometry";
    const QString USER_GROUP = "/user";
    const QString USER_NAME = "user_name";
    const QString WINDOW_NUMBER = "window_number";

    QSettings settings_;
    QString server_addr_;
    int port_;
    int window_number_;
    QByteArray main_window_geometry_;
    QByteArray main_window_state_;
    QByteArray select_tickets_dialog_geometry_;
    QString user_name_;
};

#endif // SETTINGS_H
