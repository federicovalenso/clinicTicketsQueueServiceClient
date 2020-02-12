#ifndef APPSTATE_H
#define APPSTATE_H

#include <QCloseEvent>
#include <QString>
#include <QVector>
#include "requestsprocessor.h"
#include "ticket.h"

namespace vvf {

class MainWindow;

class AppState {
 public:
  virtual void processTicket(MainWindow* w, const Ticket& ticket);
  virtual void processTickets(MainWindow*, const QVector<Ticket>&);
  virtual void background(MainWindow*);
  virtual void nextTicket(MainWindow* w);
  virtual void returnTicket(MainWindow* w);
  virtual void selectTicket(MainWindow* w);
  virtual void voiceTicket(MainWindow* w);
  virtual void close(MainWindow* w, QCloseEvent* event);
  virtual void error(MainWindow* w, const QString& error);

 protected:
  AppState() = default;
  virtual ~AppState() = default;
  AppState(const AppState&) = delete;
  AppState& operator=(AppState&) = delete;
  void changeState(MainWindow* w, AppState* state);
  void lockTicket(MainWindow* w, const Ticket& ticket);
  void getTickets(MainWindow* w, SelectModes mode = SelectModes::AUTO);
};

class BackgroundState : public AppState {
 public:
  static BackgroundState* getInstance();
  virtual void processTickets(MainWindow* w,
                              const QVector<Ticket>& tickets) override;
  virtual void background(MainWindow* w) override;
  virtual void error(MainWindow*, const QString& error) override;
};

class SelectTicketState : public AppState {
 public:
  static SelectTicketState* getInstance();
  virtual void processTicket(MainWindow* w, const Ticket& ticket) override;
  virtual void processTickets(MainWindow* w,
                              const QVector<Ticket>& tickets) override;

 private:
  bool isSelected = false;
};

class CloseState : public AppState {
 public:
  static CloseState* getInstance();
  virtual void processTicket(MainWindow* w, const Ticket&) override;
};

class NextTicketState : public AppState {
 public:
  static NextTicketState* getInstance();
  virtual void processTickets(MainWindow* w,
                              const QVector<Ticket>& tickets) override;
  virtual void error(MainWindow* w, const QString& error) override;
};

class ReturnTicketState : public AppState {
 public:
  static ReturnTicketState* getInstance();
};

class VoiceTicketState : public AppState {
 public:
  static VoiceTicketState* getInstance();
};

}  // namespace vvf

#endif  // APPSTATE_H
