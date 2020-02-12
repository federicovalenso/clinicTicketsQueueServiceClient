#ifndef TICKETSPROCESSOR_H
#define TICKETSPROCESSOR_H

#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QString>
#include <QVector>
#include <atomic>
#include <experimental/optional>
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
  void lockTicket(int id) noexcept;
  void voiceTicket() noexcept;
  bool hasActiveTicket() const noexcept;
  void returnCurrentTicket() noexcept;
  void finishCurrentTicket() noexcept;

  static const QString kId;
  static const QString kTicketAction;
  static const QString kTicketNumber;
  static const QString kCreatedAt;
  static const QString kWindow;
  static const QString kOnService;
  static const QString kIsDone;
  static const QString kIsVoiced;
  static const QString kIsManual;

 signals:
  void ticketError(const QString& message) const;
  void receivedTicket(const Ticket& ticket) const;
  void receivedTickets(const QVector<Ticket>& tickets) const;

 private:
  RequestsProcessor* requests_processor_;
  mutable QVector<QString> actions_;
  mutable std::experimental::optional<Ticket> current_ticket_;
  mutable std::atomic<SelectModes> mode_{SelectModes::AUTO};
  mutable QMutex mutex_;

  bool isValidAction(const QString& action) const noexcept;
  bool isValidTicket(const QJsonObject& ticket) const noexcept;

 private slots:
  void getTicketRequestFinished(const QByteArray& data) noexcept;
  void putTicketRequestFinished(const QByteArray&) noexcept;
  void requestFinisedWithError(const QString& message) const noexcept;
};

}  // namespace vvf

#endif  // TICKETSPROCESSOR_H
