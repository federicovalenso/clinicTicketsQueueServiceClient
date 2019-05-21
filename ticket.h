#ifndef TICKET_H
#define TICKET_H

#include <QString>

struct Ticket
{
    int id = -1;
    QString ticket_number = "";
    QString action = "";
    bool on_service = false;
    bool is_done = false;
    bool is_voiced = false;
    bool is_manual = false;
    double created_at = 0;

    inline bool isValid() const
    {
        return id != -1 ? true : false;
    }
    inline void makeInvalid()
    {
        id = -1;
    }
    inline operator QString() const
    {
        QString result = QString("{%1 : %2, %3 : %4, %5 : %6, %7 : %8, %9 : %10, %11 : %12, %13 : %14, %15 : %16}")
                .arg("id").arg(id)
                .arg("number").arg(ticket_number)
                .arg("action").arg(action)
                .arg("on_service").arg(on_service)
                .arg("is_done").arg(is_done)
                .arg("is_voiced").arg(is_voiced)
                .arg("is_manual").arg(is_manual)
                .arg("created_at").arg(created_at);
        return result;
    }
};

inline bool operator < (const Ticket& lhs, const Ticket& rhs)
{
    return lhs.created_at < rhs.created_at;
}

#endif // TICKET_H
