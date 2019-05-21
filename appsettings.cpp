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
    server_addr_ = serverAddr;
    port_ = port;

    settings_.beginGroup(CONNECTION_GROUP);
    settings_.setValue(SERVER_ADDR, serverAddr);
    settings_.setValue(PORT, port);
    settings_.endGroup();
}

void AppSettings::setWindowNumber(const int windowNumber)
{

    window_number_ = windowNumber;
    settings_.beginGroup(USER_GROUP);
    settings_.setValue(WINDOW_NUMBER, window_number_);
    settings_.endGroup();
}

void AppSettings::setSizeSettings(const QByteArray &mainWindowGeometry, const QByteArray &mainWindowState)
{
    main_window_geometry_ = mainWindowGeometry;
    main_window_state_ = mainWindowState;
    settings_.beginGroup(SIZE_GROUP);
    settings_.setValue(MAIN_WINDOW_GEOMETRY, mainWindowGeometry);
    settings_.setValue(MAIN_WINDOW_STATE, mainWindowState);
    settings_.endGroup();
}

QString AppSettings::getServerAddr() const
{
    return server_addr_;
}

int AppSettings::getPort() const
{
    return port_;
}

int AppSettings::getWindowNumber() const
{
    return  window_number_;
}

QByteArray AppSettings::getMainWindowState() const
{
    return main_window_state_;
}

QByteArray AppSettings::getMainWindowGeometry() const
{
    return main_window_geometry_;
}

QByteArray AppSettings::getSelectTicketsDialogGeometry() const
{
    return select_tickets_dialog_geometry_;
}

QString AppSettings::getUserName() const
{
    return user_name_;
}

void AppSettings::setUserName(const QString &userName)
{
    user_name_ = userName;
    settings_.beginGroup(USER_GROUP);
    settings_.setValue(USER_NAME, userName);
    settings_.endGroup();
}

void AppSettings::setMainWindowSettings(const QByteArray& geometry, const QByteArray& state)
{
    main_window_geometry_ = geometry;
    main_window_state_ = state;
    settings_.beginGroup(SIZE_GROUP);
    settings_.setValue(MAIN_WINDOW_GEOMETRY, geometry);
    settings_.setValue(MAIN_WINDOW_STATE, state);
    settings_.endGroup();
}

void AppSettings::setSelectTicketsDialogSettings(const QByteArray& geometry)
{
    select_tickets_dialog_geometry_ = geometry;
    settings_.beginGroup(SIZE_GROUP);
    settings_.setValue(SELECT_TICKETS_DIALOG_GEOMETRY, geometry);
    settings_.endGroup();
}

void AppSettings::ReadSettings()
{
    settings_.beginGroup(CONNECTION_GROUP);
    server_addr_ = settings_.value(SERVER_ADDR).toString();
    port_ = settings_.value(PORT).toInt();
    settings_.endGroup();

    settings_.beginGroup(SIZE_GROUP);
    main_window_geometry_ = settings_.value(MAIN_WINDOW_GEOMETRY).toByteArray();
    main_window_state_ = settings_.value(MAIN_WINDOW_STATE).toByteArray();
    select_tickets_dialog_geometry_ = settings_.value(SELECT_TICKETS_DIALOG_GEOMETRY).toByteArray();
    settings_.endGroup();

    settings_.beginGroup(USER_GROUP);
    user_name_ = settings_.value(USER_NAME).toString();
    window_number_ = settings_.value(WINDOW_NUMBER).toInt();
    settings_.endGroup();
}
