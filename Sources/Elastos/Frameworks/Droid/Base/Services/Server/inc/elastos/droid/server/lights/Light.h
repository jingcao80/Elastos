
#ifndef __ELASTOS_DROID_SERVER_LIGHTS_LIGHTS_H__
#define __ELASTOS_DROID_SERVER_LIGHTS_LIGHTS_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Lights {

class Light
    : public Object
    , public ILight
{
public:
    CAR_INTERFACE_DECL();

    virtual ~Light();

    virtual CARAPI_(void) SetBrightness(
        /* [in] */ Int32 brightness) = 0;

    virtual CARAPI_(void) SetBrightness(
        /* [in] */ Int32 brightness,
        /* [in] */ Int32 brightnessMode) = 0;

    virtual CARAPI_(void) SetColor(
        /* [in] */ Int32 color) = 0;

    virtual CARAPI_(void) SetFlashing(
        /* [in] */ Int32 color,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 onMS,
        /* [in] */ Int32 offMS) = 0;

    virtual CARAPI_(void) Pulse() = 0;

    virtual CARAPI_(void) Pulse(
        /* [in] */ Int32 color,
        /* [in] */ Int32 onMS) = 0;

    virtual CARAPI_(void) TurnOff() = 0;

public:
    static const Int32 LIGHT_FLASH_NONE;
    static const Int32 LIGHT_FLASH_TIMED;
    static const Int32 LIGHT_FLASH_HARDWARE;

    /**
     * Light brightness is managed by a user setting.
     */
    static const Int32 BRIGHTNESS_MODE_USER;

    /**
     * Light brightness is managed by a light sensor.
     */
    static const Int32 BRIGHTNESS_MODE_SENSOR;
};

} // Lights
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_LIGHTS_LIGHTS_H__
