
#ifndef __ELASTOS_DROID_INCALLUI_INCALLPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_INCALLPRESENTER_H__

#include "Elastos.Droid.Telecom.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/AudioModeProvider.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/CInCallActivity.h"
#include "elastos/droid/incallui/ContactInfoCache.h"
#include "elastos/droid/incallui/InCallCameraManager.h"
#include "elastos/droid/incallui/ProximitySensor.h"
#include "elastos/droid/incallui/StatusBarNotifier.h"
#include <elastos/droid/telecom/Call.h>
#include <elastos/droid/telecom/Phone.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Telecom::ICall;
using Elastos::Droid::Telecom::ICallDetails;
using Elastos::Droid::Telecom::ICallListener;
using Elastos::Droid::Telecom::IPhone;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::IPhoneListener;
using Elastos::Core::Object;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class InCallPresenter
    : public Object
    , public ICallListListener
    , public IInCallPhoneListener
{
private:
    class PhoneListener
        : public Elastos::Droid::Telecom::Phone::Listener
    {
    public:
        PhoneListener(
            /* [in] */ InCallPresenter* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnBringToForeground(
            /* [in] */ IPhone* phone,
            /* [in] */ Boolean showDialpad);

        // @Override
        CARAPI OnCallAdded(
            /* [in] */ IPhone* phone,
            /* [in] */ Elastos::Droid::Telecom::ICall* call);

        // @Override
        CARAPI OnCallRemoved(
            /* [in] */ IPhone* phone,
            /* [in] */ Elastos::Droid::Telecom::ICall* call);

    private:
        InCallPresenter* mHost;
    };

    class CallListener
        : public Elastos::Droid::Telecom::Call::Listener
    {
    public:
        CallListener(
            /* [in] */ InCallPresenter* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnPostDialWait(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ const String& remainingPostDialSequence);

        // @Override
        CARAPI OnDetailsChanged(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ ICallDetails* details);

        // @Override
        CARAPI OnConferenceableCallsChanged(
            /* [in] */ Elastos::Droid::Telecom::ICall* call,
            /* [in] */ IList* conferenceableCalls);

    private:
        InCallPresenter* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    static CARAPI_(AutoPtr<InCallPresenter>) GetInstance();

    // @Override
    CARAPI SetPhone(
        /* [in] */ IPhone* phone);

    // @Override
    CARAPI ClearPhone();

    CARAPI_(InCallState) GetInCallState();

    CARAPI_(AutoPtr<CallList>) GetCallList();

    CARAPI_(void) SetUp(
        /* [in] */ IContext* context,
        /* [in] */ CallList* callList,
        /* [in] */ AudioModeProvider* audioModeProvider);

    /**
     * Called when the telephony service has disconnected from us.  This will happen when there are
     * no more active calls. However, we may still want to continue showing the UI for
     * certain cases like showing "Call Ended".
     * What we really want is to wait for the activity and the service to both disconnect before we
     * tear things down. This method sets a serviceConnected boolean and calls a secondary method
     * that performs the aforementioned logic.
     */
    CARAPI_(void) TearDown();

    /**
     * Called when the UI begins or ends. Starts the callstate callbacks if the UI just began.
     * Attempts to tear down everything if the UI just ended. See #tearDown for more insight on
     * the tear-down process.
     */
    CARAPI_(void) SetActivity(
        /* [in] */ CInCallActivity* inCallActivity);

    /**
     * Called when there is a change to the call list.
     * Sets the In-Call state for the entire in-call app based on the information it gets from
     * CallList. Dispatches the in-call state to all listeners. Can trigger the creation or
     * destruction of the UI based on the states that is calculates.
     */
    // @Override
    CARAPI OnCallListChange(
        /* [in] */ ICallList* callList);

    /**
     * Called when there is a new incoming call.
     *
     * @param call
     */
    // @Override
    CARAPI OnIncomingCall(
        /* [in] */ ICall* call);

    /**
     * Called when a call becomes disconnected. Called everytime an existing call
     * changes from being connected (incoming/outgoing/active) to disconnected.
     */
    // @Override
    CARAPI OnDisconnect(
        /* [in] */ ICall* call);

    /**
     * Given the call list, return the state in which the in-call screen should be.
     */
    static CARAPI_(InCallState) GetPotentialStateFromCallList(
        /* [in] */ CallList* callList);

    CARAPI_(void) AddIncomingCallListener(
        /* [in] */ IIncomingCallListener* listener);

    CARAPI_(void) RemoveIncomingCallListener(
        /* [in] */ IIncomingCallListener* listener);

    CARAPI_(void) AddListener(
        /* [in] */ IInCallStateListener* listener);

    CARAPI_(void) RemoveListener(
        /* [in] */ IInCallStateListener* listener);

    CARAPI_(void) AddDetailsListener(
        /* [in] */ IInCallDetailsListener* listener);

    CARAPI_(void) RemoveDetailsListener(
        /* [in] */ IInCallDetailsListener* listener);

    CARAPI_(void) AddOrientationListener(
        /* [in] */ IInCallOrientationListener* listener);

    CARAPI_(void) RemoveOrientationListener(
        /* [in] */ IInCallOrientationListener* listener);

    CARAPI_(void) AddInCallEventListener(
        /* [in] */ IInCallEventListener* listener);

    CARAPI_(void) RemoveInCallEventListener(
        /* [in] */ IInCallEventListener* listener);

    CARAPI_(AutoPtr<ProximitySensor>) GetProximitySensor();

    CARAPI_(void) HandleAccountSelection(
        /* [in] */ IPhoneAccountHandle* accountHandle);

    CARAPI_(void) CancelAccountSelection();

    /**
     * Hangs up any active or outgoing calls.
     */
    CARAPI_(void) HangUpOngoingCall(
        /* [in] */ IContext* context);

    /**
     * Answers any incoming call.
     */
    CARAPI_(void) AnswerIncomingCall(
        /* [in] */ IContext* context,
        /* [in] */ Int32 videoState);

    /**
     * Declines any incoming call.
     */
    CARAPI_(void) DeclineIncomingCall(
        /* [in] */ IContext* context);

    CARAPI_(void) AcceptUpgradeRequest(
        /* [in] */ IContext* context);

    CARAPI_(void) DeclineUpgradeRequest(
        /* [in] */ IContext* context);

    /**
     * Returns true if the incall app is the foreground application.
     */
    CARAPI_(Boolean) IsShowingInCallUi();

    /**
     * Returns true if the activity has been created and is running.
     * Returns true as long as activity is not destroyed or finishing.  This ensures that we return
     * true even if the activity is paused (not in foreground).
     */
    CARAPI_(Boolean) IsActivityStarted();

    CARAPI_(Boolean) IsActivityPreviouslyStarted();

    /**
     * Called when the activity goes in/out of the foreground.
     */
    CARAPI_(void) OnUiShowing(
        /* [in]*/ Boolean showing);

    /**
     * Brings the app into the foreground if possible.
     */
    CARAPI_(void) BringToForeground(
        /* [in] */ Boolean showDialpad);

    CARAPI_(void) OnPostDialCharWait(
        /* [in] */ const String& callId,
        /* [in] */ const String& chars);

    /**
     * Handles the green CALL key while in-call.
     * @return true if we consumed the event.
     */
    CARAPI_(Boolean) HandleCallKey();

    /**
     * A dialog could have prevented in-call screen from being previously finished.
     * This function checks to see if there should be any UI left and if not attempts
     * to tear down the UI.
     */
    CARAPI_(void) OnDismissDialog();

    /**
     * Called by the {@link VideoCallPresenter} to inform of a change in full screen video status.
     *
     * @param isFullScreenVideo {@code True} if entering full screen video mode.
     */
    CARAPI_(void) SetFullScreenVideoState(
        /* [in] */ Boolean isFullScreenVideo);

    CARAPI_(AutoPtr<IIntent>) GetInCallIntent(
        /* [in] */ Boolean showDialpad,
        /* [in] */ Boolean newOutgoingCall);

    /**
     * Retrieves the current in-call camera manager instance, creating if necessary.
     *
     * @return The {@link InCallCameraManager}.
     */
    CARAPI_(AutoPtr<InCallCameraManager>) GetInCallCameraManager();

    /**
     * Handles changes to the device rotation.
     *
     * @param rotation The device rotation.
     */
    CARAPI_(void) OnDeviceRotationChange(
        /* [in] */ Int32 rotation);

    /**
     * Notifies listeners of changes in orientation (e.g. portrait/landscape).
     *
     * @param orientation The orientation of the device.
     */
    CARAPI_(void) OnDeviceOrientationChange(
        /* [in] */ Int32 orientation);

    /**
     * Configures the in-call UI activity so it can change orientations or not.
     *
     * @param allowOrientationChange {@code True} if the in-call UI can change between portrait
     *      and landscape.  {@Code False} if the in-call UI should be locked in portrait.
     */
    CARAPI_(void) SetInCallAllowsOrientationChange(
        /* [in] */ Boolean allowOrientationChange);

    /**
     * Returns the space available beside the call card.
     *
     * @return The space beside the call card.
     */
    CARAPI_(Float) GetSpaceBesideCallCard();

    /**
     * Returns whether the call card fragment is currently visible.
     *
     * @return True if the call card fragment is visible.
     */
    CARAPI_(Boolean) GetCallCardFragmentVisible();

    /**
     * @return True if the application is currently running in a right-to-left locale.
     */
    static CARAPI_(Boolean) IsRtl();

private:
    CARAPI_(void) AttemptFinishActivity();

    /**
     * For some disconnected causes, we show a dialog.  This calls into the activity to show
     * the dialog if appropriate for the call.
     */
    CARAPI_(void) MaybeShowErrorDialogOnDisconnect(
        /* [in] */ Call* call);

    /**
     * Hides the dialpad.  Called when a call is disconnected (Requires hiding dialpad).
     */
    CARAPI_(void) HideDialpadForDisconnect();

    /**
     * When the state of in-call changes, this is the first method to get called. It determines if
     * the UI needs to be started or finished depending on the new state and does it.
     */
    CARAPI_(InCallState) StartOrFinishUi(
        /* [in] */ InCallState newState);

    /**
     * Sets the DisconnectCause for a call that was disconnected because it was missing a
     * PhoneAccount or PhoneAccounts to select from.
     * @param call
     */
    CARAPI_(void) SetDisconnectCauseForMissingAccounts(
        /* [in] */ Call* call);

    CARAPI_(Boolean) StartUi(
        /* [in] */ InCallState inCallState);

    /**
     * Checks to see if both the UI is gone and the service is disconnected. If so, tear it all
     * down.
     */
    CARAPI_(void) AttemptCleanup();

    CARAPI_(void) ShowInCall(
        /* [in] */ Boolean showDialpad,
        /* [in] */ Boolean newOutgoingCall);

    /**
     * Private constructor. Must use getInstance() to get this singleton.
     */
    InCallPresenter() {}

private:
    static const String EXTRA_FIRST_TIME_SHOWN;

    static AutoPtr<InCallPresenter> sInCallPresenter;

    /**
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    AutoPtr<ISet> mListeners;
    AutoPtr<IList> mIncomingCallListeners;
    AutoPtr<ISet> mDetailsListeners;
    AutoPtr<ISet> mOrientationListeners;
    AutoPtr<ISet> mInCallEventListeners;

    AutoPtr<AudioModeProvider> mAudioModeProvider;
    AutoPtr<StatusBarNotifier> mStatusBarNotifier;
    AutoPtr<ContactInfoCache> mContactInfoCache;
    AutoPtr<IContext> mContext;
    AutoPtr<CallList> mCallList;
    AutoPtr<CInCallActivity> mInCallActivity;
    InCallState mInCallState;
    AutoPtr<ProximitySensor> mProximitySensor;
    Boolean mServiceConnected;
    Boolean mAccountSelectionCancelled;
    AutoPtr<InCallCameraManager> mInCallCameraManager;

    AutoPtr<IPhoneListener> mPhoneListener;

    AutoPtr<ICallListener> mCallListener;

    /**
     * Is true when the activity has been previously started. Some code needs to know not just if
     * the activity is currently up, but if it had been previously shown in foreground for this
     * in-call session (e.g., StatusBarNotifier). This gets reset when the session ends in the
     * tear-down method.
     */
    Boolean mIsActivityPreviouslyStarted;

    AutoPtr<IPhone> mPhone;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_INCALLPRESENTER_H__
