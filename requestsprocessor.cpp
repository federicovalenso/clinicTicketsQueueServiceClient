#include "requestsprocessor.h"
#include <QDebug>
#include <QJsonDocument>
#include <QNetworkCookieJar>
#include "appsettings.h"
#include "ticketsprocessor.h"

using namespace vvf;
using NetworkError = QNetworkReply::NetworkError;

QString fromBool(bool value) { return value == true ? "1" : "0"; }

RequestsProcessor::RequestsProcessor(QObject* parent) : QObject(parent) {
  network_manager_ = new QNetworkAccessManager(this);
  connect(network_manager_, SIGNAL(finished(QNetworkReply*)), this,
          SLOT(replyFinished(QNetworkReply*)));
}

RequestsProcessor::~RequestsProcessor() {}

const QString RequestsProcessor::kActionLogin = "login";
const QString RequestsProcessor::kActionTickets = "api/tickets";
const QString RequestsProcessor::kActionTakeTicket = "api/ticket/take";
const QString RequestsProcessor::kActionReturnTicket = "api/ticket/return";
const QString RequestsProcessor::kActionVoiceTicket = "api/ticket/voice";
const QString RequestsProcessor::kActionFinishTicket = "api/ticket/finish";
const QString RequestsProcessor::kActionsParam = "actions";
const QString RequestsProcessor::kTicketParam = "ticket";
const QByteArray RequestsProcessor::kSetCookieHeader = "Set-Cookie";
const QByteArray RequestsProcessor::kSessionId = "sessionid";
const QMap<NetworkError, QString> RequestsProcessor::kErrorMessages = {
    {NetworkError::AuthenticationRequiredError,
     QObject::tr("Неудачная попытка авторизации")},
    {NetworkError::ContentAccessDenied, QObject::tr("Требуется авторизация")},
    {NetworkError::ContentOperationNotPermittedError,
     QObject::tr("Требуется авторизация")},
    {NetworkError::ContentConflictError,
     QObject::tr("Конфликт. Талон занят другим регистратором")}};

void RequestsProcessor::login(const QString& name,
                              const QString& password) const noexcept {
  QUrlQuery params;
  params.addQueryItem("name", name);
  params.addQueryItem("password", password);
  sendPostRequest(params, kActionLogin);
}

void RequestsProcessor::getTickets(SelectModes mode) const noexcept {
  QString action = QString("%1?%2=0&%3=%4")
                       .arg(kActionTickets)
                       .arg(TicketsProcessor::kOnService)
                       .arg(TicketsProcessor::kIsManual)
                       .arg(mode == SelectModes::AUTO ? "0" : "1");
  sendGetRequest(std::move(action));
}

void RequestsProcessor::updateTicket(int id, const QString& action,
                                     QUrlQuery&& query) const noexcept {
  query.addQueryItem(TicketsProcessor::kId, QString::number(id));
  query.addQueryItem(
      TicketsProcessor::kWindow,
      QString::number(AppSettings::getInstance().getWindowNumber()));
  sendPutRequest(query, action);
}

void RequestsProcessor::takeTicket(int id) const noexcept {
  QUrlQuery query;
  query.addQueryItem(TicketsProcessor::kOnService, "1");
  updateTicket(id, kActionTakeTicket, std::move(query));
}

void RequestsProcessor::returnTicket(int id) const noexcept {
  QUrlQuery query;
  query.addQueryItem(TicketsProcessor::kOnService, "0");
  updateTicket(id, kActionReturnTicket, std::move(query));
}

void RequestsProcessor::voiceTicket(int id) const noexcept {
  QUrlQuery query;
  query.addQueryItem(TicketsProcessor::kIsVoiced, "0");
  updateTicket(id, kActionVoiceTicket, std::move(query));
}

void RequestsProcessor::finishTicket(int id) const noexcept {
  QUrlQuery query;
  query.addQueryItem(TicketsProcessor::kIsDone, "1");
  updateTicket(id, kActionFinishTicket, std::move(query));
}

void RequestsProcessor::sendGetRequest(QString action) const noexcept {
  AppSettings& settings = AppSettings::getInstance();
  QString address = QString("http://%1:%2/%3")
                        .arg(settings.getServerAddr())
                        .arg(settings.getPort())
                        .arg(action);
  QUrl url(std::move(address));
  QNetworkRequest request(url);
  network_manager_->get(request);
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
  network_manager_->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
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
  network_manager_->put(request, params.toString(QUrl::FullyEncoded).toUtf8());
}

QNetworkCookie RequestsProcessor::getCookie(const QNetworkReply& reply) {
  QNetworkCookie result;
  if (reply.hasRawHeader(kSetCookieHeader)) {
    const auto cookie_header = reply.rawHeader(kSetCookieHeader);
    const auto cookie_parts = cookie_header.split(';');
    for (const auto& part : cookie_parts) {
      const auto params = part.split('=');
      if (params.size() == 2 && params.first() == kSessionId) {
        result.setName(params.first());
        result.setValue(params.last());
        break;
      }
    }
  }
  return result;
}

void RequestsProcessor::replyFinished(QNetworkReply* reply) {
  qDebug() << reply->url();
  const auto error = reply->error();
  if (error != NetworkError::NoError) {
    QString errorMessage = kErrorMessages.value(error);
    if (errorMessage.isEmpty()) errorMessage = "Произошла ошибка";
    if (error == NetworkError::AuthenticationRequiredError ||
        error == NetworkError::ContentAccessDenied ||
        error == NetworkError::ContentOperationNotPermittedError) {
      emit loginError();
    } else {
      emit requestError(errorMessage);
    }
  } else {
    int status =
        reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status == 200) {
      QNetworkAccessManager::Operation operation = reply->operation();
      if (operation == QNetworkAccessManager::GetOperation) {
        if (reply->url().path() == '/' + kActionTickets) {
          emit receivedTickets(reply->readAll());
        }
      } else if (operation == QNetworkAccessManager::PostOperation) {
        if (reply->url().path() == '/' + kActionLogin) {
          const auto cookie = getCookie(*reply);
          if (!cookie.value().isEmpty()) {
            reply->manager()->cookieJar()->insertCookie(cookie);
            emit loginFinished();
          } else {
            emit requestError(tr(
                "Авторизация успешна, однако, сервер не вернул данных сессии"));
          }
        }
      } else if (operation == QNetworkAccessManager::PutOperation) {
        if (reply->url().path().startsWith("/api/ticket")) {
          emit ticketUpdated(reply->readAll());
        }
      }
    }
  }
}
