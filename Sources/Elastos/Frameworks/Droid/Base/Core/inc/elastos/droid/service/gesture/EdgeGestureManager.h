
#ifndef __ELASTOS_DROID_SERVICE_GESTURE_EDGE_GESTURE_MANAGER_H__
#define __ELASTOS_DROID_SERVICE_GESTURE_EDGE_GESTURE_MANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Internal::Utility::Gesture::IEdgeGesturePosition;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Gesture {

/**
 * This is a simple Manager class for edge gesture service on the application side. The application need
 * {@code INJECT_EVENTS} permission to register {@code EdgeGestureActivationListener}s.<br>
 * See {@link android.service.gesture.IEdgeGestureService} for more information.
 *
 * @see android.service.gesture.IEdgeGestureService
 * @hide
 */
class EdgeGestureManager
    : public Object
    , public IEdgeGestureManager
{
public:
    class EdgeGestureActivationListener
        : public Object
        , public IEdgeGestureActivationListener
    {
    private:
        class OnEdgeGestureActivationRunnable
            : public Runnable
        {
        public:
            OnEdgeGestureActivationRunnable(
                /* [in] */ EdgeGestureActivationListener* host,
                /* [in] */ Int32 touchX,
                /* [in] */ Int32 touchY,
                /* [in] */ Int32 positionIndex,
                /* [in] */ Int32 flags);

            CARAPI Run();

        private:
            AutoPtr<EdgeGestureActivationListener> mHost;
            Int32 mTouchX, mTouchY, mPositionIndex, mFlags;
        };
    public:
        CAR_INTERFACE_DECL()

        EdgeGestureActivationListener();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ ILooper* looper);

        CARAPI SetHostCallback(
            /* [in] */ IIEdgeGestureHostCallback* hostCallback);

        /**
         * Override this to receive activations from the edge gesture service.
         *
         * @param touchX the last X position a touch event was registered.
         * @param touchY the last Y position a touch event was registered.
         * @param position the position of the activation.
         * @param flags currently 0.
         * @see IEdgeGestureActivationListener#onEdgeGestureActivation(int, int, int, int)
         */
        virtual CARAPI OnEdgeGestureActivation(
            /* [in] */ Int32 touchX,
            /* [in] */ Int32 touchY,
            /* [in] */ IEdgeGesturePosition* position,
            /* [in] */ Int32 flags) = 0;

        /**
         * After being activated, this allows the edge gesture control to steal focus from the current
         * window.
         *
         * @see IEdgeGestureHostCallback#gainTouchFocus(IBinder)
         */
        CARAPI GainTouchFocus(
            /* [in] */ IBinder* applicationWindowToken,
            /* [out] */ Boolean* result);

        CARAPI DropEventsUntilLift(
            /* [out] */ Boolean* result);

        /**
         * Turns listening for edge gesture activation gestures on again, after it was disabled during
         * the call to the listener.
         *
         * @see IEdgeGestureHostCallback#restoreListenerState()
         */
        CARAPI RestoreListenerState();

        CARAPI OnEdgeGestureActivationInner(
            /* [in] */ Int32 touchX,
            /* [in] */ Int32 touchY,
            /* [in] */ Int32 positionIndex,
            /* [in] */ Int32 flags);

    private:
        friend class EdgeGestureManager;
        AutoPtr<IHandler> mHandler;
        AutoPtr<IIEdgeGestureHostCallback> mCallback;
        AutoPtr<IIEdgeGestureActivationListener> mDelegator;
    };

public:
    CAR_INTERFACE_DECL()

    EdgeGestureManager();

    CARAPI constructor();

    /**
     * Gets an instance of the edge gesture manager.
     *
     * @return The edge gesture manager instance.
     * @hide
     */
    static AutoPtr<IEdgeGestureManager> GetInstance();

    /**
     * Checks if the edge gesture service is present.
     * <p>
     * Since the service is only started at boot time and is bound to the system server, this
     * is constant for the devices up time.
     *
     * @return {@code true} when the edge gesture service is running on this device.
     * @hide
     */
    CARAPI IsPresent(
        /* [out] */ Boolean* result);

    /**
     * Register a listener for edge gesture activation gestures. Initially the listener
     * is set to listen for no position. Use updateedge gestureActivationListener() to
     * bind the listener to positions.
     *
     * @param listener is the activation listener.
     * @return {@code true} if the registration was successful.
     * @hide
     */
    CARAPI SetEdgeGestureActivationListener(
        /* [in] */ IEdgeGestureActivationListener* listener,
        /* [out] */ Boolean* result);

    /**
     * Update the listener to react on gestures in the given positions.
     *
     * @param listener is a already registered listener.
     * @param positions is a bit mask describing the positions to listen to.
     * @hide
     */
    CARAPI UpdateEdgeGestureActivationListener(
        /* [in] */ IEdgeGestureActivationListener* listener,
        /* [in] */ Int32 positions);

private:
    CARAPI constructor(
        /* [in] */ IIEdgeGestureService* ps);

private:
    static const String TAG;// = "EdgeGestureManager";
    static const Boolean DEBUG;// = false;

    static AutoPtr<IEdgeGestureManager> sInstance;
    static Object sLock;

    AutoPtr<IIEdgeGestureService> mPs;
};

} // namespace Gesture
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_GESTURE_EDGE_GESTURE_MANAGER_H__