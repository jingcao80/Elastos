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

#ifndef __ELASTOS_DROID_SERVER_LIGHTS_LIGHTSSERVICE_H__
#define __ELASTOS_DROID_SERVER_LIGHTS_LIGHTSSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/server/lights/Light.h"
#include "elastos/droid/server/lights/LightsManager.h"
#include "elastos/droid/server/SystemService.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IIHardwareService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Lights {

class LightsService;

class LightImpl
    : public Light
{
    friend class LightsService;

public:
    virtual ~LightImpl();

    // @Override
    virtual CARAPI_(void) SetBrightness(
        /* [in] */ Int32 brightness);

    // @Override
    virtual CARAPI_(void) SetBrightness(
        /* [in] */ Int32 brightness,
        /* [in] */ Int32 brightnessMode);

    // @Override
    virtual CARAPI_(void) SetColor(
        /* [in] */ Int32 color);

    // @Override
    virtual CARAPI_(void) SetFlashing(
        /* [in] */ Int32 color,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 onMS,
        /* [in] */ Int32 offMS);

    // @Override
    virtual CARAPI_(void) Pulse();

    // @Override
    virtual CARAPI_(void) Pulse(
        /* [in] */ Int32 color,
        /* [in] */ Int32 onMS);

    // @Override
    virtual CARAPI_(void) TurnOff();

private:
    LightImpl(
        /* [in] */ Int32 id,
        /* [in] */ LightsService* host);

    CARAPI_(void) StopFlashing();

    CARAPI_(void) SetLightLocked(
        /* [in] */ Int32 color,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 onMS,
        /* [in] */ Int32 offMS,
        /* [in] */ Int32 brightnessMode);

private:
    Int32 mId;
    Int32 mColor;
    Int32 mMode;
    Int32 mOnMS;
    Int32 mOffMS;
    Boolean mFlashing;

    LightsService* mHost;
};

} // Lights
} // Server
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Lights::LightImpl, IInterface)

namespace Elastos {
namespace Droid {
namespace Server {
namespace Lights {

class LightsService
    : public SystemService
{
    friend class LightImpl;

public:
    class LegacyFlashlightHack
        : public Object
        , public IIHardwareService
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        LegacyFlashlightHack();

        ~LegacyFlashlightHack();

        CARAPI constructor(
            /* [in] */ ISystemService* service);

        CARAPI GetFlashlightEnabled(
            /* [out] */ Boolean* isEnabled);

        CARAPI SetFlashlightEnabled(
            /* [in] */ Boolean on);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        LightsService* mHost;
        static const String FLASHLIGHT_FILE;
    };

private:
    class MyLightsManager
        : public LightsManager
    {
    public:
        MyLightsManager(
            /* [in] */ LightsService* host);

        virtual ~MyLightsManager();

        virtual CARAPI_(AutoPtr<Light>) GetLight(
            /* [in] */ Int32 id);

    private:
        LightsService* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("LightsService::MyHandler")

        MyHandler(
            /* [in] */ LightsService* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        LightsService* mHost;
    };

public:
    LightsService();

    virtual ~LightsService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

private:
    CARAPI_(void) Init_native();

    CARAPI_(void) Finalize_native();

    CARAPI_(void) SetLight_native(
        /* [in] */ Int32 light,
        /* [in] */ Int32 color,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 onMS,
        /* [in] */ Int32 offMS,
        /* [in] */ Int32 brightnessMode);

protected:
    static const Boolean DEBUG;

private:
    AutoPtr<ArrayOf<LightImpl*> > mLights;
    AutoPtr<MyLightsManager> mService;

    AutoPtr<MyHandler> mH;
    Int64 mNativePointer;
    AutoPtr<IIHardwareService> mLegacyFlashlightHack;
};

} // Lights
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_LIGHTS_LIGHTSSERVICE_H__
