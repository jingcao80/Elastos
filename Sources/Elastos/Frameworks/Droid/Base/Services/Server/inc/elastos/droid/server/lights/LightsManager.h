
#ifndef __ELASTOS_DROID_SERVER_LIGHTS_LIGHTSMANAGER_H__
#define __ELASTOS_DROID_SERVER_LIGHTS_LIGHTSMANAGER_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include "elastos/droid/server/lights/Light.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Server::Lights::ILightsManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Lights {

class LightsManager
    : public Object
    , public ILightsManager
{
public:
    CAR_INTERFACE_DECL()

    virtual ~LightsManager();

    virtual CARAPI_(AutoPtr<Light>) GetLight(
        /* [in] */ Int32 id) = 0;

public:
    static const Int32 LIGHT_ID_BACKLIGHT;
    static const Int32 LIGHT_ID_KEYBOARD;
    static const Int32 LIGHT_ID_BUTTONS;
    static const Int32 LIGHT_ID_BATTERY;
    static const Int32 LIGHT_ID_NOTIFICATIONS;
    static const Int32 LIGHT_ID_ATTENTION;
    static const Int32 LIGHT_ID_BLUETOOTH;
    static const Int32 LIGHT_ID_WIFI;
    static const Int32 LIGHT_ID_CAPS;
    static const Int32 LIGHT_ID_FUNC;
    static const Int32 LIGHT_ID_COUNT;
};

} // Lights
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_LIGHTS_LIGHTSMANAGER_H__
