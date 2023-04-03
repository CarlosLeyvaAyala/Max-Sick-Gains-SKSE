#include "logger.h"
using namespace SKSE;
using namespace RE;

SKSEPluginLoad(const LoadInterface *skse) {
    Init(skse);

    SetupLog();
    logger::debug("Max Sick Gains has been installed");

    return true;
}