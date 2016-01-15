
#include "elastos/droid/server/lights/LightsManager.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Lights {

CAR_INTERFACE_IMPL(LightsManager, Object, ILightsManager);

const Int32 LightsManager::LIGHT_ID_BACKLIGHT = 0;
const Int32 LightsManager::LIGHT_ID_KEYBOARD = 1;
const Int32 LightsManager::LIGHT_ID_BUTTONS = 2;
const Int32 LightsManager::LIGHT_ID_BATTERY = 3;
const Int32 LightsManager::LIGHT_ID_NOTIFICATIONS = 4;
const Int32 LightsManager::LIGHT_ID_ATTENTION = 5;
const Int32 LightsManager::LIGHT_ID_BLUETOOTH = 6;
const Int32 LightsManager::LIGHT_ID_WIFI = 7;
const Int32 LightsManager::LIGHT_ID_COUNT = 8;

LightsManager::~LightsManager()
{
}

} // Lights
} // Server
} // Droid
} // Elastos
