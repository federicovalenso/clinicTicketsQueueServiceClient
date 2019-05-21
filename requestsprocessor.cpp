#include "requestsprocessor.h"
#include <QDebug>
#include <QJsonDocument>
#include <QNetworkCookieJar>
#include "appsettings.h"
#include "ticketsprocessor.h"

using namespace vvf;

QString fromBool(bool value) { return value == true ? "1" : "0"; }

RequestsProcessor::RequestsProcessor(QObject* parent) : QObject(parent) {
  mNetworkManager = new QNetworkAccessManager(this);
  connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), this,
          SLOT(replyFinished(QNetworkReply*)));
}

RequestsProcessor::~RequestsProcessor() {}

const QString RequestsProcessor::ACTION_TICKETS = "tickets";
const QString RequestsProcessor::ACTION_LOGIN = "login";
const QString RequestsProcessor::NAME_PARAM = "name";
const QString RequestsProcessor::ON_SERVICE_PARAM = "on_service";
const QString RequestsProcessor::PASSWORD_PARAM = "password";
const QString RequestsProcessor::ACTIONS_PARAM = "actions";
const QString RequestsProcessor::TICKET_PARAM = "ticket";
const QByteArray RequestsProcessor::SET_COOKIE_HEADER = "Set-Cookie";
const QByteArray RequestsProcessor::SESSION_ID = "sessionid";

void RequestsProcessor::sendLoginRequest(const QString& name,
                                         const QString& password) const
    noexcept {
  QUrlQuery params;
  params.addQueryItem(NAME_PARAM, name);
  params.addQueryItem(PASSWORD_PARAM, password);
  sendPostRequest(params, ACTION_LOGIN);
}

void RequestsProcessor::sendGetTicketsRequest(SelectModes mode) const noexcept {
  QString action = QString("%1?%2=0&%3=%4")
                       .arg(ACTION_TICKETS)
                       .arg(TicketsProcessor::ON_SERVICE)
                       .arg(TicketsProcessor::IS_MANUAL)
                       .arg(mode == SelectModes::AUTO ? "0" : "1");
  sendGetRequest(std::move(action));
}

void RequestsProcessor::sendUpdateTicketRequest(const Ticket& ticket) const
    noexcept {
  QUrlQuery params;
  AppSettings& settings = AppSettings::getInstance();
  params.addQueryItem(TicketsProcessor::ID, QString::number(ticket.id));
  params.addQueryItem(TicketsProcessor::ON_SERVICE,
                      fromBool(ticket.on_service));
  params.addQueryItem(TicketsProcessor::IS_DONE, fromBool(ticket.is_done));
  params.addQueryItem(TicketsProcessor::IS_VOICED, fromBool(ticket.is_voiced));
  params.addQueryItem(TicketsProcessor::IS_MANUAL, fromBool(ticket.is_manual));
  params.addQueryItem(TicketsProcessor::WINDOW,
                      QString::number(settings.getWindowNumber()));
  sendPutRequest(params, ACTION_TICKETS);
}

void RequestsProcessor::sendGetRequest(QString action) const noexcept {
  AppSettings& settings = AppSettings::getInstance();
  QString address = QString("http://%1:%2/%3")
                        .arg(settings.getServerAddr())
                        .arg(settings.getPort())
                        .arg(action);
  QUrl url(std::move(address));
  QNetworkRequest request(url);
  mNetworkManager->get(request);
}

void RequestsProcessor::sendPostRequest(const QUrlQuery& params,
                                        const QString& action) const noexcept {
  AppSettings& settings = AppSettings::getInstance();
  QString address = QString("http://%1:%2/%3")
                        .arg(settings.getServerAddr())
                        .arg(settings.getPort())
                        .arg(action);
  QUrl url(address);
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");
  mNetworkManager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
}

void RequestsProcessor::sendPutRequest(const QUrlQuery& params,
                                       const QString& action) const noexcept {
  AppSettings& settings = AppSettings::getInstance();
  QString address = QString("http://%1:%2/%3")
                        .arg(settings.getServerAddr())
                        .arg(settings.getPort())
                        .arg(action);
  QUrl url(address);
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    "application/x-www-form-urlencoded");
  mNetworkManager->put(request, params.toString(QUrl::FullyEncoded).toUtf8());
}

QNetworkCookie RequestsProcessor::getCookie(const QNetworkReply& reply) {
  QNetworkCookie result;
  if (reply.hasRawHeader(SET_COOKIE_HEADER)) {
    auto cookie_header = reply.rawHeader(SET_COOKIE_HEADER);
    auto cookie_parts = cookie_header.split(';');
    for (const auto& part : cookie_parts) {
      auto params = part.split('=');
      if (params.size() == 2) {
        if (params.first() == SESSION_ID) {
          result.setName(params.first());
          result.setValue(params.last());
        }
      }
    }
  }
  return result;
}

void RequestsProcessor::replyFinished(QNetworkReply* reply) {
  auto error = reply->error();
  if (error != QNetworkReply::NetworkError::NoError) {
    QString errorMessage;
    if (error == QNetworkReply::NetworkError::AuthenticationRequiredError) {
      errorMessage = tr("Неудачная попытка авторизации");
    } else if (error == QNetworkReply::NetworkError::ContentAccessDenied) {
      errorMessage = tr("Требуется авторизация");
    } else {
      errorMessage = tr("Не удалось получить данные с сервера...");
    }
    emit requestError(errorMessage);
  } else {
    int status =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status == 200) {
      QNetworkAccessManager::Operation operation = reply->operation();
      if (operation == QNetworkAccessManager::GetOperation) {
        if (reply->url().path() == '/' + ACTION_TICKETS) {
          emit receivedTickets(reply->readAll());
        }
      } else if (operation == QNetworkAccessManager::PostOperation) {
        if (reply->url().path() == '/' + ACTION_LOGIN) {
          if (auto cookie = getCookie(*reply); !cookie.value().isEmpty()) {
            reply->manager()->cookieJar()->insertCookie(cookie);
            emit loginFinished();
          } else {
            emit requestError(tr(
                "Авторизация успешна, однако, сервер не вернул данных сессии"));
          }
        }
      } else if (operation == QNetworkAccessManager::PutOperation) {
        if (reply->url().path() == '/' + ACTION_TICKETS) {
          emit ticketUpdated(reply->readAll());
        }
      }
    }
  }
}
