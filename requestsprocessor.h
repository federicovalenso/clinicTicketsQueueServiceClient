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
  void login(const QString& name, const QString& password) const noexcept;
  void getTickets(SelectModes mode = SelectModes::AUTO) const noexcept;
  void updateTicket(int id, const QString& action, QUrlQuery&& query) const
      noexcept;
  void takeTicket(int id) const noexcept;
  void returnTicket(int id) const noexcept;
  void voiceTicket(int id) const noexcept;
  void finishTicket(int id) const noexcept;

 signals:
  void requestError(const QString& message);
  void loginError();
  void loginFinished();
  void receivedTickets(const QByteArray& data);
  void ticketUpdated(const QByteArray& data);

 private slots:
  void replyFinished(QNetworkReply* reply);

 private:
  static const QString kActionLogin;
  static const QString kActionTickets;
  static const QString kActionTakeTicket;
  static const QString kActionReturnTicket;
  static const QString kActionVoiceTicket;
  static const QString kActionFinishTicket;
  static const QString kActionsParam;
  static const QString kTicketParam;
  static const QByteArray kSetCookieHeader;
  static const QByteArray kSessionId;
  static const QMap<QNetworkReply::NetworkError, QString> kErrorMessages;

  void sendGetRequest(QString action) const noexcept;
  void sendPostRequest(const QUrlQuery& params, const QString& action) const
      noexcept;
  void sendPutRequest(const QUrlQuery& params, const QString& action) const
      noexcept;
  QNetworkCookie getCookie(const QNetworkReply& reply);
  QNetworkAccessManager* network_manager_;
};

}  // namespace vvf

#endif  // REQUESTSPROCESSOR_H
