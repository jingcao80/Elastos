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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_WAKEGESTURELISTENER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_WAKEGESTURELISTENER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Hardware::ITriggerEvent;
using Elastos::Droid::Hardware::ITriggerEventListener;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::IRunnable;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
  * Watches for wake gesture sensor events then invokes the listener.
  */
class WakeGestureListener
    : public Object
    , public IWakeGestureListener
{
private:
    class InnerTriggerEventListener
        : public Object
        , public ITriggerEventListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerTriggerEventListener(
            /* [in] */ WakeGestureListener* owner);

        // @Override
        CARAPI OnTrigger(
            /* [in] */ ITriggerEvent* event);

    private:
        WakeGestureListener* mOwner;
    };

    class InnerRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerRunnable(
            /* [in] */ WakeGestureListener* owner);

        // @Override
        CARAPI Run();

    private:
        WakeGestureListener* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    WakeGestureListener();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler);

    virtual CARAPI OnWakeUp() = 0;

    virtual CARAPI IsSupported(
        /* [out] */ Boolean* result);

    virtual CARAPI RequestWakeUpTrigger();

    virtual CARAPI CancelWakeUpTrigger();

    virtual CARAPI Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

private:
    static const String TAG;
    AutoPtr<ISensorManager> mSensorManager;
    AutoPtr<IHandler> mHandler;
    AutoPtr<Object> mLock;
    Boolean mTriggerRequested;
    AutoPtr<ISensor> mSensor;
    AutoPtr<ITriggerEventListener> mListener;
    AutoPtr<IRunnable> mWakeUpRunnable;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_WAKEGESTURELISTENER_H__

