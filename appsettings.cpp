#include "appsettings.h"

AppSettings& AppSettings::getInstance()
{
    static AppSettings instance;
    return instance;
}

AppSettings::AppSettings()
{
    ReadSettings();
}

void AppSettings::setConnectionSettings(const QString& serverAddr, const int port)
{
    mServerAddr = serverAddr;
    mPort = port;

    mSettings.beginGroup(CONNECTION_GROUP);
    mSettings.setValue(SERVER_ADDR, serverAddr);
    mSettings.setValue(PORT, port);
    mSettings.endGroup();
}

void AppSettings::setWindowNumber(const int windowNumber)
{

    mWindowNumber = windowNumber;
    mSettings.beginGroup(USER_GROUP);
    mSettings.setValue(WINDOW_NUMBER, mWindowNumber);
    mSettings.endGroup();
}

void AppSettings::setSizeSettings(const QByteArray &mainWindowGeometry, const QByteArray &mainWindowState)
{
    mMainWindowGeometry = mainWindowGeometry;
    mMainWindowState = mainWindowState;
    mSettings.beginGroup(SIZE_GROUP);
    mSettings.setValue(MAIN_WINDOW_GEOMETRY, mainWindowGeometry);
    mSettings.setValue(MAIN_WINDOW_STATE, mainWindowState);
    mSettings.endGroup();
}

QString AppSettings::getServerAddr() const
{
    return mServerAddr;
}

int AppSettings::getPort() const
{
    return mPort;
}

int AppSettings::getWindowNumber() const
{
    return  mWindowNumber;
}

QByteArray AppSettings::getMainWindowState() const
{
    return mMainWindowState;
}

QByteArray AppSettings::getMainWindowGeometry() const
{
    return mMainWindowGeometry;
}


QString AppSettings::getUserName() const
{
    return mUserName;
}

void AppSettings::setUserName(const QString &userName)
{
    mUserName = userName;
    mSettings.beginGroup(USER_GROUP);
    mSettings.setValue(USER_NAME, userName);
    mSettings.endGroup();
}

void AppSettings::ReadSettings()
{
    mSettings.beginGroup(CONNECTION_GROUP);
    mServerAddr = mSettings.value(SERVER_ADDR).toString();
    mPort = mSettings.value(PORT).toInt();
    mSettings.endGroup();

    mSettings.beginGroup(SIZE_GROUP);
    mMainWindowGeometry = mSettings.value(MAIN_WINDOW_GEOMETRY).toByteArray();
    mMainWindowState = mSettings.value(MAIN_WINDOW_STATE).toByteArray();
    mSettings.endGroup();

    mSettings.beginGroup(USER_GROUP);
    mUserName = mSettings.value(USER_NAME).toString();
    mWindowNumber = mSettings.value(WINDOW_NUMBER).toInt();
    mSettings.endGroup();
}
