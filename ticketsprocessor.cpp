#include "ticketsprocessor.h"
#include <QDateTime>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QMutexLocker>
#include "appsettings.h"

using namespace vvf;

const QString TicketsProcessor::kId = "id";
const QString TicketsProcessor::kTicketAction = "action_name";
const QString TicketsProcessor::kTicketNumber = "ticket_number";
const QString TicketsProcessor::kCreatedAt = "created_at";
const QString TicketsProcessor::kWindow = "window";
const QString TicketsProcessor::kOnService = "on_service";
const QString TicketsProcessor::kIsDone = "is_done";
const QString TicketsProcessor::kIsVoiced = "is_voiced";
const QString TicketsProcessor::kIsManual = "is_manual";

Ticket ExtractTicketFromJson(const QJsonObject &object) {
  Ticket result;
  result.id = object.value(TicketsProcessor::kId).toInt();
  result.ticket_number =
      object.value(TicketsProcessor::kTicketNumber).toString();
  result.action = object.value(TicketsProcessor::kTicketAction).toString();
  result.on_service = object.value(TicketsProcessor::kOnService).toBool();
  result.is_done = object.value(TicketsProcessor::kIsDone).toBool();
  result.is_voiced = object.value(TicketsProcessor::kIsVoiced).toBool();
  result.is_manual = object.value(TicketsProcessor::kIsManual).toBool();
  result.created_at = object.value(TicketsProcessor::kCreatedAt).toInt();
  return result;
}

TicketsProcessor::TicketsProcessor(QObject *parent, RequestsProcessor *rp)
    : QObject(parent), requests_processor_(rp) {
  connect(requests_processor_, SIGNAL(ticketUpdated(QByteArray)), this,
          SLOT(putTicketRequestFinished(QByteArray)));
  connect(requests_processor_, SIGNAL(receivedTickets(QByteArray)), this,
          SLOT(getTicketRequestFinished(QByteArray)));
  connect(requests_processor_, SIGNAL(requestError(QString)), this,
          SLOT(requestFinisedWithError(QString)));
}

void TicketsProcessor::getTickets(const QVector<QString> &actions,
                                  SelectModes mode) noexcept {
  QMutexLocker locker(&mutex_);
  finishCurrentTicket();
  actions_ = actions;
  mode_ = mode;
  requests_processor_->getTickets(mode);
}

void TicketsProcessor::lockTicket(int id) noexcept {
  QMutexLocker locker(&mutex_);
  current_ticket_->id = id;
  requests_processor_->takeTicket(current_ticket_->id);
}

void TicketsProcessor::voiceTicket() noexcept {
  QMutexLocker locker(&mutex_);
  if (current_ticket_) {
    requests_processor_->voiceTicket(current_ticket_->id);
  } else {
    emit ticketError(tr("Нет текущего талона"));
  }
}

bool TicketsProcessor::hasActiveTicket() const noexcept {
  QMutexLocker locker(&mutex_);
  return current_ticket_.operator bool();
}

void TicketsProcessor::returnCurrentTicket() noexcept {
  QMutexLocker locker(&mutex_);
  if (current_ticket_) {
    requests_processor_->returnTicket(current_ticket_->id);
  }
}

void TicketsProcessor::finishCurrentTicket() noexcept {
  if (current_ticket_) {
    current_ticket_->is_done = true;
    requests_processor_->finishTicket(current_ticket_->id);
  }
}

bool TicketsProcessor::isValidAction(const QString &action) const noexcept {
  bool result = false;
  for (auto it = actions_.begin(); it != actions_.end(); it++) {
    if (*it == action) {
      result = true;
      break;
    }
  }
  return result;
}

bool TicketsProcessor::isValidTicket(const QJsonObject &ticket) const noexcept {
  return ticket.contains(kId) && ticket.contains(kTicketAction) &&
         ticket.contains(kCreatedAt);
}

void TicketsProcessor::getTicketRequestFinished(
    const QByteArray &data) noexcept {
  QJsonDocument jsonInput = QJsonDocument::fromJson(data);
  if (!jsonInput.isEmpty() && jsonInput.isArray()) {
    QVector<Ticket> validJsonTickets;
    QJsonArray jsonTicketsArr(jsonInput.array());
    SelectModes mode;
    mode = mode_;
    for (const auto jsonTicketVal : jsonTicketsArr) {
      QJsonObject jsonTicketObj(jsonTicketVal.toObject());
      if (isValidTicket(jsonTicketObj) == true) {
        Ticket ticket = ExtractTicketFromJson(jsonTicketObj);
        if (mode == SelectModes::MANUAL ||
            (isValidAction(ticket.action) && !ticket.is_manual)) {
          validJsonTickets.push_back(std::move(ticket));
        }
      } else {
        emit ticketError(tr("Сервер вернул некорректные данные"));
        return;
      }
    }
    if (validJsonTickets.size() > 0) {
      emit receivedTickets(validJsonTickets);
    } else {
      emit ticketError(tr("Нет необслуженных квитанций"));
      emit receivedTicket({});
    }
  } else {
    emit ticketError(tr("Сервер вернул некорректные данные"));
  }
}

void TicketsProcessor::putTicketRequestFinished(
    const QByteArray &data) noexcept {
  QMutexLocker locker(&mutex_);
  current_ticket_ =
      ExtractTicketFromJson(QJsonDocument::fromJson(data).object());
  if (!current_ticket_->is_done && current_ticket_->on_service) {
    emit receivedTicket(*current_ticket_);
  } else {
    current_ticket_ = std::experimental::nullopt;
    emit receivedTicket({});
  }
}

void TicketsProcessor::requestFinisedWithError(const QString &message) const
    noexcept {
  emit ticketError(message);
}
