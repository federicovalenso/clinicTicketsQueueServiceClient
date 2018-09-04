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
    QString getUserName() const;
    void setUserName(const QString &userName);

private:
    AppSettings();
    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(AppSettings&) = delete;
    void ReadSettings();

    QSettings mSettings;
    const QString CONNECTION_GROUP = "/connection";
    const QString SERVER_ADDR = "server_addr";
    const QString PORT = "port";
    const QString SIZE_GROUP = "/size";
    const QString MAIN_WINDOW_GEOMETRY = "main_window_geometry";
    const QString MAIN_WINDOW_STATE = "main_window_state";
    const QString USER_GROUP = "/user";
    const QString USER_NAME = "user_name";
    const QString WINDOW_NUMBER = "window_number";

    QString mServerAddr;
    int mPort;
    int mWindowNumber;
    QByteArray mMainWindowGeometry;
    QByteArray mMainWindowState;
    QString mUserName;
};

#endif // SETTINGS_H
