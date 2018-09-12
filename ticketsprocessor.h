#ifndef TICKETSPROCESSOR_H
#define TICKETSPROCESSOR_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QJsonObject>
#include <QMutex>
#include "requestsprocessor.h"
#include "ticket.h"

class TicketsProcessor : public QObject
{
    Q_OBJECT
public:
    explicit TicketsProcessor(QObject *parent = nullptr);
    void getTickets(const QVector<QString>& actions);
    void voiceTicket();
    bool hasActiveTicket();
    void finishCurrentTicket();

    enum class ValidModes {
        SERVICE, CONSIDERATION
    };

    static const QString ID;
    static const QString TICKET_ACTION;
    static const QString TICKET_NUMBER;
    static const QString CREATED_AT;
    static const QString USER_NAME;
    static const QString WINDOW;
    static const QString ON_SERVICE;
    static const QString IS_DONE;
    static const QString IS_VOICED;
signals:
    void ticketError(const QString& message);
    void receivedTicket(const QString& ticket_number);

private:
    RequestsProcessor* mRequestsProcessor;
    QVector<QString> mActions;
    Ticket currentTicket;
    QMutex m;

    bool isValidAction(const QString& action);
    bool isValidTicket(const QJsonObject& ticket, ValidModes);
    void lockTicket(const Ticket& ticket);

private slots:
    void getTicketRequestFinished(const QByteArray &data);
    void putTicketRequestFinished(const QByteArray &);
    void requestFinisedWithError(const QString& message);
};

#endif // TICKETSPROCESSOR_H
