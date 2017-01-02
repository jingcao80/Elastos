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
