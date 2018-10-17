#include <QJsonDocument>
#include <QDebug>
#include "requestsprocessor.h"
#include "ticketsprocessor.h"
#include "appsettings.h"

using namespace vvf;

QString fromBool(bool value)
{
    return value == true ? "1" : "0";
}

RequestsProcessor::RequestsProcessor(QObject *parent):
    QObject(parent)
{
    mNetworkManager = new QNetworkAccessManager(this);
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

RequestsProcessor::~RequestsProcessor() {}

const QString RequestsProcessor::ACTION_TICKETS = "tickets";
const QString RequestsProcessor::ACTION_LOGIN = "login";
const QString RequestsProcessor::NAME_PARAM = "name";
const QString RequestsProcessor::ON_SERVICE_PARAM = "on_service";
const QString RequestsProcessor::PASSWORD_PARAM = "password";
const QString RequestsProcessor::ACTIONS_PARAM = "actions";
const QString RequestsProcessor::TICKET_PARAM = "ticket";

void RequestsProcessor::sendLoginRequest(const QString& name, const QString& password) const noexcept
{
    QUrlQuery params;
    params.addQueryItem(NAME_PARAM, name);
    params.addQueryItem(PASSWORD_PARAM, password);
    sendPostRequest(params, ACTION_LOGIN);
}

void RequestsProcessor::sendGetTicketsRequest(SelectModes mode) const noexcept
{
    QString action = QString("%1?%2=0&%3=%4")
            .arg(ACTION_TICKETS)
            .arg(TicketsProcessor::ON_SERVICE)
            .arg(TicketsProcessor::IS_MANUAL)
            .arg(mode == SelectModes::AUTO ? "0" : "1");
    sendGetRequest(std::move(action));
}

void RequestsProcessor::sendUpdateTicketRequest(const Ticket& ticket) const noexcept
{
    QUrlQuery params;
    AppSettings& settings = AppSettings::getInstance();
    params.addQueryItem(TicketsProcessor::USER_NAME, settings.getUserName());
    params.addQueryItem(TicketsProcessor::ID, QString::number(ticket.id));
    params.addQueryItem(TicketsProcessor::ON_SERVICE, fromBool(ticket.on_service));
    params.addQueryItem(TicketsProcessor::IS_DONE, fromBool(ticket.is_done));
    params.addQueryItem(TicketsProcessor::IS_VOICED, fromBool(ticket.is_voiced));
    params.addQueryItem(TicketsProcessor::IS_MANUAL, fromBool(ticket.is_manual));
    params.addQueryItem(TicketsProcessor::WINDOW, QString::number(settings.getWindowNumber()));
    sendPutRequest(params, ACTION_TICKETS);
}

void RequestsProcessor::sendGetRequest(QString action) const noexcept
{
    AppSettings& settings = AppSettings::getInstance();
    QString address = QString("http://%1:%2/%3")
            .arg(settings.getServerAddr())
            .arg(settings.getPort())
            .arg(action);
    QUrl url(std::move(address));
    QNetworkRequest request(url);
    mNetworkManager->get(request);
}

void RequestsProcessor::sendPostRequest(const QUrlQuery& params, const QString& action) const noexcept
{
    AppSettings& settings = AppSettings::getInstance();
    QString address = QString("http://%1:%2/%3")
            .arg(settings.getServerAddr())
            .arg(settings.getPort())
            .arg(action);
    QUrl url(address);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    mNetworkManager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
}

void RequestsProcessor::sendPutRequest(const QUrlQuery &params, const QString &action) const noexcept
{
    AppSettings& settings = AppSettings::getInstance();
    QString address = QString("http://%1:%2/%3")
            .arg(settings.getServerAddr())
            .arg(settings.getPort())
            .arg(action);
    QUrl url(address);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    mNetworkManager->put(request, params.toString(QUrl::FullyEncoded).toUtf8());
}

void RequestsProcessor::replyFinished(QNetworkReply *reply)
{
    auto error = reply->error();
    if (error != QNetworkReply::NetworkError::NoError) {
        QString errorMessage;
        if (error == QNetworkReply::NetworkError::AuthenticationRequiredError) {
            errorMessage = tr("Неудачная попытка авторизации");
        } else {
            errorMessage = tr("Не удалось получить данные с сервера...");
        }
        emit requestError(errorMessage);
    } else {
        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (status == 200) {
            QNetworkAccessManager::Operation operation = reply->operation();
            if (operation == QNetworkAccessManager::GetOperation) {
                emit receivedTickets(reply->readAll());
            } else if (operation == QNetworkAccessManager::PostOperation) {
                emit loginFinished();
            } else if (operation == QNetworkAccessManager::PutOperation) {
                emit ticketUpdated(reply->readAll());
            }
        }
    }
}
