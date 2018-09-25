#ifndef TICKET_H
#define TICKET_H

#include <QString>

struct Ticket
{
    int id = -1;
    QString ticket_number;
    bool on_service = false;
    bool is_done = false;
    bool is_voiced = false;
    qint64 created_at = 0;

    inline bool isValid()
    {
        return id != -1 ? true : false;
    }
};

inline bool operator < (const Ticket& lhs, const Ticket& rhs)
{
    return lhs.created_at < rhs.created_at;
}

#endif // TICKET_H
