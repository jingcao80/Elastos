#ifndef __ELASTOS_DROID_DIALER_PROXIMITYSENSERMANAGER_H__
#define __ELASTOS_DROID_DIALER_PROXIMITYSENSERMANAGER_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Hardware::ISensorManager;

namespace Elastos {
namespace Droid {
namespace Dialer {

class ProximitySensorManager : public Object
{
public:
    enum State {
        NEAR,
        FAR,
    };

private:
    /**
     * The listener to the state of the sensor.
     * <p>
     * Contains most of the logic concerning tracking of the sensor.
     * <p>
     * After creating an instance of this object, one should call {@link #register()} and
     * {@link #unregister()} to enable and disable the notifications.
     * <p>
     * Instead of calling unregister, one can call {@link #unregisterWhenFar()} to unregister the
     * listener the next time the sensor reaches the {@link State#FAR} state if currently in the
     * {@link State#NEAR} state.
     */
    class ProximitySensorEventListener
        : public Object
        , public ISensorEventListener
    {
    public:
        CAR_INTERFACE_DECL()

        ProximitySensorEventListener(
            /* [in] */ ISensorManager* sensorManager,
            /* [in] */ ISensor* proximitySensor,
            /* [in] */ IProximitySensorManagerListener* listener);

        // @Override
        CARAPI OnSensorChanged(
            /* [in] */ ISensorEvent* event);

        // @Override
        CARAPI OnAccuracyChanged(
            /* [in] */ ISensor* sensor,
            /* [in] */ Int32 accuracy);

        /**
         * Unregister the next time the sensor reaches the {@link State#FAR} state.
         */
        CARAPI_(void) UnregisterWhenFar();

        /** Register the listener and call the listener as necessary. */
        CARAPI_(void) Register();

        CARAPI_(void) Unregister();

    private:
        /** Returns the state of the sensor given its current value. */
        CARAPI_(State) GetStateFromValue(
            /* [in] */ Float value);

        // @GuardedBy("this")
        CARAPI_(void) UnregisterWithoutNotification();

    private:
        static const Float FAR_THRESHOLD = 5.0f;

        AutoPtr<ISensorManager> mSensorManager;
        AutoPtr<ISensor> mProximitySensor;
        Float mMaxValue;
        AutoPtr<IProximitySensorManagerListener> mListener;

        /**
         * The last state of the sensor.
         * <p>
         * Before registering and after unregistering we are always in the {@link State#FAR} state.
         */
        /* @GuardedBy("this") */
        State mLastState;

        /**
         * If this flag is set to true, we are waiting to reach the {@link State#FAR} state and
         * should notify the listener and unregister when that happens.
         */
        /* @GuardedBy("this") */
         Boolean mWaitingForFarState;
    };

public:
    ProximitySensorManager(
        /* [in] */ IContext* context,
        /* [in] */ IProximitySensorManagerListener* listener);

    /**
     * Enables the proximity manager.
     * <p>
     * The listener will start getting notifications of events.
     * <p>
     * This method is idempotent.
     */
    CARAPI_(void) Enable();

    /**
     * Disables the proximity manager.
     * <p>
     * The listener will stop receiving notifications of events, possibly after receiving a last
     * {@link Listener#onFar()} callback.
     * <p>
     * If {@code waitForFarState} is true, if the sensor is not currently in the {@link State#FAR}
     * state, the listener will receive a {@link Listener#onFar()} callback the next time the sensor
     * actually reaches the {@link State#FAR} state.
     * <p>
     * If {@code waitForFarState} is false, the listener will receive a {@link Listener#onFar()}
     * callback immediately if the sensor is currently not in the {@link State#FAR} state.
     * <p>
     * This method is idempotent.
     */
    CARAPI_(void) Disable(
        /* [in] */ Boolean waitForFarState);

private:
    AutoPtr<ProximitySensorEventListener> mProximitySensorListener;

    /**
     * The current state of the manager, i.e., whether it is currently tracking the state of the
     * sensor.
     */
    Boolean mManagerEnabled;

};

} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_PROXIMITYSENSERMANAGER_H__
