#include "logger.h"
using namespace SKSE;
using namespace RE;
using namespace std;

void OnMessage(MessagingInterface::Message* msg) {
    auto mt{msg->type};

    if (mt == MessagingInterface::kNewGame)
        log::info("New game. Configuring Max Sick Gains for first use.");
    else if (mt == MessagingInterface::kPostLoadGame)
        log::info("Game was loaded. Applying appearance to nearby NPCs");
}

SKSEPluginLoad(const LoadInterface* skse) {
    Init(skse);

    SetupLog();
    log::debug("Max Sick Gains is ready to work");

    GetMessagingInterface()->RegisterListener(OnMessage);

    return true;
}
