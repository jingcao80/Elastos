#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_PHONE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_PHONE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * A unified virtual device providing a means of voice (and other) communication on a device.
 *
 * {@hide}
 */
class Phone
    : public Object
    , public IPhone
{
public:
    class Listener
        : public Object
        , public IPhoneListener
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Called when the audio state changes.
         *
         * @param phone The {@code Phone} calling this method.
         * @param audioState The new {@link AudioState}.
         */
        CARAPI OnAudioStateChanged(
            /* [in] */ IPhone* phone,
            /* [in] */ IAudioState* audioState);

        /**
         * Called to bring the in-call screen to the foreground. The in-call experience should
         * respond immediately by coming to the foreground to inform the user of the state of
         * ongoing {@code Call}s.
         *
         * @param phone The {@code Phone} calling this method.
         * @param showDialpad If true, put up the dialpad when the screen is shown.
         */
        CARAPI OnBringToForeground(
            /* [in] */ IPhone* phone,
            /* [in] */ Boolean showDialpad);

        /**
         * Called when a {@code Call} has been added to this in-call session. The in-call user
         * experience should add necessary state listeners to the specified {@code Call} and
         * immediately start to show the user information about the existence
         * and nature of this {@code Call}. Subsequent invocations of {@link #getCalls()} will
         * include this {@code Call}.
         *
         * @param phone The {@code Phone} calling this method.
         * @param call A newly added {@code Call}.
         */
        CARAPI OnCallAdded(
            /* [in] */ IPhone* phone,
            /* [in] */ ICall* call);

        /**
         * Called when a {@code Call} has been removed from this in-call session. The in-call user
         * experience should remove any state listeners from the specified {@code Call} and
         * immediately stop displaying any information about this {@code Call}.
         * Subsequent invocations of {@link #getCalls()} will no longer include this {@code Call}.
         *
         * @param phone The {@code Phone} calling this method.
         * @param call A newly removed {@code Call}.
         */
        CARAPI OnCallRemoved(
            /* [in] */ IPhone* phone,
            /* [in] */ ICall* call);
    };

public:
    CAR_INTERFACE_DECL()

    Phone();

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IInCallAdapter* adapter);

    /** {@hide} */
    CARAPI InternalAddCall(
        /* [in] */ IParcelableCall* parcelableCall);

    /** {@hide} */
    CARAPI InternalRemoveCall(
        /* [in] */ ICall* call);

    /** {@hide} */
    CARAPI InternalUpdateCall(
        /* [in] */ IParcelableCall* parcelableCall);

    /** {@hide} */
    CARAPI InternalSetPostDialWait(
        /* [in] */ String telecomId,
        /* [in] */ String remaining);

    /** {@hide} */
    CARAPI InternalAudioStateChanged(
        /* [in] */ IAudioState* audioState);

    /** {@hide} */
    CARAPI InternalGetCallByTelecomId(
        /* [in] */ String telecomId,
        /* [out] */ ICall** result);

    /** {@hide} */
    CARAPI InternalBringToForeground(
        /* [in] */ Boolean showDialpad);

    /**
     * Called to destroy the phone and cleanup any lingering calls.
     * @hide
     */
    CARAPI Destroy();

    /**
     * Adds a listener to this {@code Phone}.
     *
     * @param listener A {@code Listener} object.
     */
    CARAPI AddListener(
        /* [in] */ IPhoneListener* listener);

    /**
     * Removes a listener from this {@code Phone}.
     *
     * @param listener A {@code Listener} object.
     */
    CARAPI RemoveListener(
        /* [in] */ IPhoneListener* listener);

    /**
     * Obtains the current list of {@code Call}s to be displayed by this in-call experience.
     *
     * @return A list of the relevant {@code Call}s.
     */
    CARAPI GetCalls(
        /* [out] */ IList** result);

    /**
     * Sets the microphone mute state. When this request is honored, there will be change to
     * the {@link #getAudioState()}.
     *
     * @param state {@code true} if the microphone should be muted; {@code false} otherwise.
     */
    CARAPI SetMuted(
        /* [in] */ Boolean state);

    /**
     * Sets the audio route (speaker, bluetooth, etc...).  When this request is honored, there will
     * be change to the {@link #getAudioState()}.
     *
     * @param route The audio route to use.
     */
    CARAPI SetAudioRoute(
        /* [in] */ Int32 route);

    /**
     * Turns the proximity sensor on. When this request is made, the proximity sensor will
     * become active, and the touch screen and display will be turned off when the user's face
     * is detected to be in close proximity to the screen. This operation is a no-op on devices
     * that do not have a proximity sensor.
     */
    CARAPI SetProximitySensorOn();

    /**
     * Turns the proximity sensor off. When this request is made, the proximity sensor will
     * become inactive, and no longer affect the touch screen and display. This operation is a
     * no-op on devices that do not have a proximity sensor.
     *
     * @param screenOnImmediately If true, the screen will be turned on immediately if it was
     * previously off. Otherwise, the screen will only be turned on after the proximity sensor
     * is no longer triggered.
     */
    CARAPI SetProximitySensorOff(
        /* [in] */ Boolean screenOnImmediately);

    /**
     * Obtains the current phone call audio state of the {@code Phone}.
     *
     * @return An object encapsulating the audio state.
     */
    CARAPI GetAudioState(
        /* [out] */ IAudioState** result);

private:
    CARAPI_(void) FireCallAdded(
        /* [in] */ ICall* call);

    CARAPI_(void) FireCallRemoved(
        /* [in] */ ICall* call);

    CARAPI_(void) FireAudioStateChanged(
        /* [in] */ IAudioState* audioState);

    CARAPI_(void) FireBringToForeground(
        /* [in] */ Boolean showDialpad);

    CARAPI_(void) CheckCallTree(
        /* [in] */ IParcelableCall* parcelableCall);

private:
    // A Map allows us to track each Call by its Telecom-specified call ID
    AutoPtr<IMap> mCallByTelecomCallId;

    // A List allows us to keep the Calls in a stable iteration order so that casually developed
    // user interface components do not incur any spurious jank
    AutoPtr<IList> mCalls;

    // An unmodifiable view of the above List can be safely shared with subclass implementations
    AutoPtr<IList> mUnmodifiableCalls;

    AutoPtr<IInCallAdapter> mInCallAdapter;

    AutoPtr<IAudioState> mAudioState;

    AutoPtr<IList> mListeners;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_PHONE_H__