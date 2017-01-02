//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
