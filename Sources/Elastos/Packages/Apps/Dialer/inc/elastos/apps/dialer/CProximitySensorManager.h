#ifndef __ELASTOS_APPS_DIALER_CPROXIMITYSENSERMANAGER_H__
#define __ELASTOS_APPS_DIALER_CPROXIMITYSENSERMANAGER_H__

#include "_Elastos_Apps_Dialer_CProximitySensorManager.h"

using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Apps::Dialer::IProximitySensorManager;
using Elastos::Apps::Dialer::IProximitySensorManagerListener;

namespace Elastos{
namespace Apps{
namespace Dialer {

CarClass(CProximitySensorManager)
    , public Object
    , public IProximitySensorManager
{
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
        CARAPI_(IProximitySensorManager::State) GetStateFromValue(
            /* [in] */ Float value);

        // @GuardedBy("this")
        CARAPI_(void) UnregisterWithoutNotification();

    private:
        static const Float FAR_THRESHOLD; // = 5.0f;
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
        IProximitySensorManager::State mLastState;

        /**
         * If this flag is set to true, we are waiting to reach the {@link State#FAR} state and
         * should notify the listener and unregister when that happens.
         */
        /* @GuardedBy("this") */
         Boolean mWaitingForFarState;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CProximitySensorManager();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IProximitySensorManagerListener* listener);

    /**
     * Enables the proximity manager.
     * <p>
     * The listener will start getting notifications of events.
     * <p>
     * This method is idempotent.
     */
    CARAPI Enable();

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
    CARAPI Disable(
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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CPROXIMITYSENSERMANAGER_H__
