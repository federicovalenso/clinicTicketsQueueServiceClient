#include "appstate.h"

using namespace vvf;

VoiceTicketState *VoiceTicketState::getInstance()
{
    static VoiceTicketState instance;
    return &instance;
}
