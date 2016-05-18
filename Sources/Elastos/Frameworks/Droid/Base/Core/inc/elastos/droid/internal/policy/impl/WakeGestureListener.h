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

