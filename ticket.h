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

    inline bool isValid()
    {
        return id != -1 ? true : false;
    }
};

#endif // TICKET_H
