#ifndef REQUESTSPROCESSOR_H
#define REQUESTSPROCESSOR_H

#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QUrlQuery>
#include "ticket.h"

namespace vvf {

enum class SelectModes { MANUAL, AUTO };

class RequestsProcessor : public QObject {
  Q_OBJECT
 public:
  explicit RequestsProcessor(QObject* parent = nullptr);
  ~RequestsProcessor();
  void sendLoginRequest(const QString& name, const QString& password) const
      noexcept;
  void sendGetTicketsRequest(SelectModes mode = SelectModes::AUTO) const
      noexcept;
  void sendUpdateTicketRequest(const Ticket& ticket) const noexcept;

 signals:
  void requestError(const QString& message);
  void loginFinished();
  void receivedTickets(const QByteArray& data);
  void ticketUpdated(const QByteArray& data);

 private slots:
  void replyFinished(QNetworkReply* reply);

 private:
  static const QString ACTION_LOGIN;
  static const QString ACTION_TICKETS;
  static const QString NAME_PARAM;
  static const QString ON_SERVICE_PARAM;
  static const QString PASSWORD_PARAM;
  static const QString ACTIONS_PARAM;
  static const QString TICKET_PARAM;
  static const QByteArray SET_COOKIE_HEADER;
  static const QByteArray SESSION_ID;

  void sendGetRequest(QString action) const noexcept;
  void sendPostRequest(const QUrlQuery& params, const QString& action) const
      noexcept;
  void sendPutRequest(const QUrlQuery& params, const QString& action) const
      noexcept;
  QNetworkCookie getCookie(const QNetworkReply& reply);
  QNetworkAccessManager* mNetworkManager;
};

}  // namespace vvf

#endif  // REQUESTSPROCESSOR_H
