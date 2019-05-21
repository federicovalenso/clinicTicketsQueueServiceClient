#ifndef TICKETSPROCESSOR_H
#define TICKETSPROCESSOR_H

#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QVector>
#include "requestsprocessor.h"
#include "ticket.h"

namespace vvf {

class TicketsProcessor : public QObject {
  Q_OBJECT
 public:
  TicketsProcessor() = delete;
  TicketsProcessor(QObject* parent, RequestsProcessor* rp);
  void getTickets(const QVector<QString>& actions = {},
                  SelectModes mode = SelectModes::AUTO) noexcept;
  void lockTicket(const Ticket& ticket) noexcept;
  void voiceTicket() noexcept;
  bool hasActiveTicket() const noexcept;
  void returnCurrentTicket() noexcept;
  void finishCurrentTicket() noexcept;

  static const QString ID;
  static const QString TICKET_ACTION;
  static const QString TICKET_NUMBER;
  static const QString CREATED_AT;
  static const QString WINDOW;
  static const QString ON_SERVICE;
  static const QString IS_DONE;
  static const QString IS_VOICED;
  static const QString IS_MANUAL;

 signals:
  void ticketError(const QString& message) const;
  void receivedTicket(const Ticket& ticket) const;
  void receivedTickets(const QVector<Ticket>& tickets) const;

 private:
  RequestsProcessor* requests_processor_;
  mutable QVector<QString> actions_;
  mutable Ticket current_ticket_;
  mutable SelectModes mode_ = SelectModes::AUTO;
  QMutex m_;

  bool isValidAction(const QString& action) const noexcept;
  bool isValidTicket(const QJsonObject& ticket) const noexcept;

 private slots:
  void getTicketRequestFinished(const QByteArray& data) noexcept;
  void putTicketRequestFinished(const QByteArray&) noexcept;
  void requestFinisedWithError(const QString& message) const noexcept;
};

}  // namespace vvf

#endif  // TICKETSPROCESSOR_H
