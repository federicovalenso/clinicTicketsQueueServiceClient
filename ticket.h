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
};

inline bool operator < (const Ticket& lhs, const Ticket& rhs)
{
    return lhs.created_at < rhs.created_at;
}

#endif // TICKET_H
