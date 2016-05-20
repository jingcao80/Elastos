
#include "elastos/droid/server/lights/Light.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Lights {

const Int32 Light::LIGHT_FLASH_NONE = 0;
const Int32 Light::LIGHT_FLASH_TIMED = 1;
const Int32 Light::LIGHT_FLASH_HARDWARE = 2;
const Int32 Light::BRIGHTNESS_MODE_USER = 0;
const Int32 Light::BRIGHTNESS_MODE_SENSOR = 1;

CAR_INTERFACE_IMPL(Light, Object, ILight);

Light::~Light()
{
}

} // Lights
} // Server
} // Droid
} // Elastos
