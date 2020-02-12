#include "appstate.h"
#include "mainwindow.h"

using namespace vvf;

ReturnTicketState* ReturnTicketState::getInstance() {
  static ReturnTicketState instance;
  return &instance;
}
