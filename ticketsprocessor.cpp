#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QDebug>
#include <QDateTime>
#include "ticketsprocessor.h"
#include "appsettings.h"

const QString TicketsProcessor::ID = "id";
const QString TicketsProcessor::TICKET_ACTION = "action_name";
const QString TicketsProcessor::TICKET_NUMBER = "ticket_number";
const QString TicketsProcessor::CREATED_AT = "created_at";
const QString TicketsProcessor::USER_NAME = "user_name";
const QString TicketsProcessor::WINDOW = "window";
const QString TicketsProcessor::ON_SERVICE = "on_service";
const QString TicketsProcessor::IS_DONE = "is_done";

Ticket fromQJsonObject(const QJsonObject& object)
{
    Ticket result;
    result.id = object.value(TicketsProcessor::ID).toInt();
    result.ticket_number = object.value(TicketsProcessor::TICKET_NUMBER).toString();
    result.on_service = object.value(TicketsProcessor::ON_SERVICE).toBool();
    result.is_done = object.value(TicketsProcessor::IS_DONE).toBool();
    return result;
}

TicketsProcessor::TicketsProcessor(QObject *parent)
    : QObject(parent)
{
    mRequestsProcessor = new RequestsProcessor(this);
    connect(mRequestsProcessor,
            SIGNAL(ticketUpdated(QByteArray)),
            this,
            SLOT(putTicketRequestFinished(QByteArray)));
    connect(mRequestsProcessor,
            SIGNAL(receivedTickets(QByteArray)),
            this,
            SLOT(getTicketRequestFinished(QByteArray)));
    connect(mRequestsProcessor,
            SIGNAL(requestError(QString)),
            this,
            SLOT(requestFinisedWithError(QString)));
}

void TicketsProcessor::getTickets(const QVector<QString> &actions)
{
    if (currentTicket.isValid() && currentTicket.is_done != true) {
        QMutexLocker locker(&m);
        currentTicket.is_done = true;
        mRequestsProcessor->sendUpdateTicketRequest(currentTicket);
    }
    mActions = actions;
    mRequestsProcessor->sendGetTicketsRequest();
}

bool TicketsProcessor::isValidAction(const QString &action)
{
    bool result = false;
    for (auto it = mActions.begin(); it != mActions.end(); it++) {
        if (*it == action) {
            result = true;
            break;
        }
    }
    return result;
}

bool TicketsProcessor::isValidTicket(const QJsonObject &ticket, ValidModes mode)
{
    if (mode == ValidModes::CONSIDERATION) {
        return ticket.contains(ID) &&
                ticket.contains(TICKET_ACTION) &&
                ticket.contains(CREATED_AT);
    } else {
        return ticket.contains(ID) &&
                ticket.contains(TICKET_NUMBER);
    }

}

void TicketsProcessor::lockTicket(const Ticket &ticket)
{
    currentTicket = ticket;
    currentTicket.on_service = true;
    mRequestsProcessor->sendUpdateTicketRequest(currentTicket);
}

void TicketsProcessor::getTicketRequestFinished(const QByteArray &data)
{
    QJsonDocument jsonInput = QJsonDocument::fromJson(data);
    if (!jsonInput.isEmpty() && jsonInput.isArray()) {
        QVector<QJsonObject> validJsonTickets;
        QJsonArray jsonTicketsArr(jsonInput.array());
        for (const auto& jsonTicketVal : jsonTicketsArr) {
            QJsonObject jsonTicketObj(jsonTicketVal.toObject());
            if (isValidTicket(jsonTicketObj, ValidModes::CONSIDERATION) == true) {
                if (isValidAction(jsonTicketObj.value(TICKET_ACTION).toString())) {
                    validJsonTickets.push_back(std::move(jsonTicketObj));
                }
            } else {
                emit ticketError(tr("Сервер вернул некорректные данные"));
                return;
            }
        }
        if (validJsonTickets.size() > 0) {
            auto oldestJsonTicket = std::min_element(
                        validJsonTickets.begin(),
                        validJsonTickets.end(),
                        [] (const QJsonObject& lhs, const QJsonObject& rhs)
                        { return lhs.value(CREATED_AT).toString().toLongLong() < rhs.value(CREATED_AT).toString().toLongLong(); });
            lockTicket(fromQJsonObject(std::move(*oldestJsonTicket)));
        } else {
            emit ticketError(tr("Нет необслуженных квитанций"));
            emit receivedTicket("");
        }
    } else {
        emit ticketError(tr("Сервер вернул некорректные данные"));
    }
}

void TicketsProcessor::putTicketRequestFinished(const QByteArray &)
{
    emit receivedTicket(currentTicket.ticket_number);
}

void TicketsProcessor::requestFinisedWithError(const QString &message)
{
    emit ticketError(message);
}
