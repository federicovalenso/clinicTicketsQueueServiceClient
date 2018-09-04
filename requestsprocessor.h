#ifndef REQUESTSPROCESSOR_H
#define REQUESTSPROCESSOR_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include "ticket.h"

class RequestsProcessor : public QObject
{
    Q_OBJECT
public:
    explicit RequestsProcessor(QObject *parent = nullptr);
    ~RequestsProcessor();
    void sendLoginRequest(const QString& name, const QString& password) const noexcept;
    void sendGetTicketsRequest() const noexcept;
    void sendUpdateTicketRequest(const Ticket& ticket) const noexcept;

signals:
    void requestError(const QString& message);
    void loginFinished();
    void receivedTickets(const QByteArray& data);
    void ticketUpdated(const QByteArray& data);

private slots:
    void replyFinished(QNetworkReply *reply);

private:
    static const QString ACTION_LOGIN;
    static const QString ACTION_TICKETS;
    static const QString NAME_PARAM;
    static const QString ON_SERVICE_PARAM;
    static const QString PASSWORD_PARAM;
    static const QString ACTIONS_PARAM;
    static const QString TICKET_PARAM;

    void sendGetRequest(QString action) const noexcept;
    void sendPostRequest(const QUrlQuery& params, const QString& action) const noexcept;
    void sendPutRequest(const QUrlQuery& params, const QString& action) const noexcept;
    QNetworkAccessManager *mNetworkManager;
};

#endif // REQUESTSPROCESSOR_H
