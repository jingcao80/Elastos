
#ifndef __ELASTOS_DROID_INCALLUI_CALLLIST_H__
#define __ELASTOS_DROID_INCALLUI_CALLLIST_H__

#include "Elastos.Droid.Telecomm.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/telecomm/telecom/Phone.h"
#include "elastos/droid/os/Handler.h"
#define HASH_FOR_TELECOMM_TELECOM
#include "elastos/droid/ext/frameworkhash.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/Set.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Telecomm::Telecom::IPhone;
using Elastos::Droid::Telecomm::Telecom::IPhoneListener;
using Elastos::Core::Object;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::Set;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class CallList
    : public Object
    , public ICallList
    , public IInCallPhoneListener
{
private:
    class PhoneListener
        : public Phone::Listener
    {
    public:
        PhoneListener(
            /* [in] */ CallList* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnCallAdded(
            /* [in] */ IPhone* phone,
            /* [in] */ Elastos::Droid::Telecomm::Telecom::ICall* call);

        // @Override
        CARAPI OnCallRemoved(
            /* [in] */ IPhone* phone,
            /* [in] */ Elastos::Droid::Telecomm::Telecom::ICall* call);

    private:
        CallList* mHost;
    };

    class MyHandler : public Handler
    {
    public:
        MyHandler(
            /* [in] */ CallList* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CallList* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Static singleton accessor method.
     */
    static CARAPI_(AutoPtr<CallList>) GetInstance();

    // @Override
    CARAPI SetPhone(
        /* [in] */ IPhone* phone);

    // @Override
    CARAPI ClearPhone();

    /**
     * Called when a single call disconnects.
     */
    CARAPI_(void) OnDisconnect(
        /* [in] */ Call* call);

    /**
     * Called when a single call has changed.
     */
    CARAPI OnIncoming(
        /* [in] */ Call* call,
        /* [in] */ IList* textMessages);

    /**
     * Called when a single call has changed.
     */
    CARAPI_(void) OnUpdate(
        /* [in] */ Call* call);

    CARAPI_(void) NotifyCallUpdateListeners(
        /* [in] */ Call* call);

    /**
     * Add a call update listener for a call id.
     *
     * @param callId The call id to get updates for.
     * @param listener The listener to add.
     */
    CARAPI_(void) AddCallUpdateListener(
        /* [in] */ const String& callId,
        /* [in] */ ICallUpdateListener* listener);

    /**
     * Remove a call update listener for a call id.
     *
     * @param callId The call id to remove the listener for.
     * @param listener The listener to remove.
     */
    CARAPI_(void) RemoveCallUpdateListener(
        /* [in] */ const String& callId,
        /* [in] */ ICallUpdateListener* listener);

    CARAPI AddListener(
        /* [in] */ ICallListListener* listener);

    CARAPI_(void) RemoveListener(
        /* [in] */ ICallListListener* listener);

    /**
     * TODO: Change so that this function is not needed. Instead of assuming there is an active
     * call, the code should rely on the status of a specific Call and allow the presenters to
     * update the Call object when the active call changes.
     */
    CARAPI_(AutoPtr<Call>) GetIncomingOrActive();

    CARAPI_(AutoPtr<Call>) GetOutgoingOrActive();

    /**
     * A call that is waiting for {@link PhoneAccount} selection
     */
    CARAPI_(AutoPtr<Call>) GetWaitingForAccountCall();

    CARAPI_(AutoPtr<Call>) GetPendingOutgoingCall();

    CARAPI_(AutoPtr<Call>) GetOutgoingCall();

    CARAPI_(AutoPtr<Call>) GetActiveCall();

    CARAPI_(AutoPtr<Call>) GetBackgroundCall();

    CARAPI_(AutoPtr<Call>) GetDisconnectedCall();

    CARAPI_(AutoPtr<Call>) GetDisconnectingCall();

    CARAPI_(AutoPtr<Call>) GetSecondBackgroundCall();

    CARAPI_(AutoPtr<Call>) GetActiveOrBackgroundCall();

    CARAPI_(AutoPtr<Call>) GetIncomingCall();

    CARAPI_(AutoPtr<Call>) GetFirstCall();

    CARAPI_(Boolean) HasLiveCall();

    /**
     * Returns the first call found in the call map with the specified call modification state.
     * @param state The session modification state to search for.
     * @return The first call with the specified state.
     */
    CARAPI_(AutoPtr<Call>) GetVideoUpgradeRequestCall();

    CARAPI_(AutoPtr<Call>) GetCallById(
        /* [in] */ const String& callId);

    CARAPI_(AutoPtr<Call>) GetCallByTelecommCall(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::ICall* telecommCall);

    CARAPI_(AutoPtr<IList>) GetTextResponses(
        /* [in] */ const String& callId);

    /**
     * Returns first call found in the call map with the specified state.
     */
    CARAPI_(AutoPtr<Call>) GetFirstCallWithState(
        /* [in] */ Int32 state);

    /**
     * Returns the [position]th call found in the call map with the specified state.
     * TODO: Improve this logic to sort by call time.
     */
    CARAPI_(AutoPtr<Call>) GetCallWithState(
        /* [in] */ Int32 state,
        /* [in] */ Int32 positionToFind);

    /**
     * This is called when the service disconnects, either expectedly or unexpectedly.
     * For the expected case, it's because we have no calls left.  For the unexpected case,
     * it is likely a crash of phone and we need to clean up our calls manually.  Without phone,
     * there can be no active calls, so this is relatively safe thing to do.
     */
    CARAPI_(void) ClearOnDisconnect();

    /**
     * Notifies all video calls of a change in device orientation.
     *
     * @param rotation The new rotation angle (in degrees).
     */
    CARAPI_(void) NotifyCallsOfDeviceRotation(
        /* [in] */ Int32 rotation);

private:
    /**
     * Private constructor.  Instance should only be acquired through getInstance().
     */
    CallList();

    static CARAPI_(AutoPtr<CallList>) InitInstance();

    /**
     * Processes an update for a single call.
     *
     * @param call The call to update.
     */
    CARAPI OnUpdateCall(
        /* [in] */ Call* call);

    /**
     * Sends a generic notification to all listeners that something has changed.
     * It is up to the listeners to call back to determine what changed.
     */
    CARAPI_(void) NotifyGenericListeners();

    CARAPI_(void) NotifyListenersOfDisconnect(
        /* [in] */ Call* call);

    /**
     * Updates the call entry in the local map.
     * @return false if no call previously existed and no call was added, otherwise true.
     */
    CARAPI_(Boolean) UpdateCallInMap(
        /* [in] */ Call* call);

    CARAPI_(Int32) GetDelayForDisconnect(
        /* [in] */ Call* call);

    CARAPI UpdateCallTextMap(
        /* [in] */ Call* call,
        /* [in] */ IList* textResponses);

    CARAPI_(Boolean) IsCallDead(
        /* [in] */ Call* call);

    /**
     * Sets up a call for deletion and notifies listeners of change.
     */
    CARAPI_(void) FinishDisconnectedCall(
        /* [in] */ Call* call);

private:
    static const Int32 DISCONNECTED_CALL_SHORT_TIMEOUT_MS = 200;
    static const Int32 DISCONNECTED_CALL_MEDIUM_TIMEOUT_MS = 2000;
    static const Int32 DISCONNECTED_CALL_LONG_TIMEOUT_MS = 5000;

    static const Int32 EVENT_DISCONNECTED_TIMEOUT = 1;

    static AutoPtr<CallList> sInstance;

    HashMap<String, AutoPtr<Call> > mCallById;
    HashMap<AutoPtr<Elastos::Droid::Telecomm::Telecom::ICall>, AutoPtr<Call> > mCallByTelecommCall;
    HashMap<String, AutoPtr<IList> > mCallTextReponsesMap;
    /**
     * ConcurrentHashMap constructor params: 8 is initial table size, 0.9f is
     * load factor before resizing, 1 means we only expect a single thread to
     * access the map so make only a single shard
     */
    Set<AutoPtr<ICallListListener> > mListeners;
    HashMap<String, AutoPtr<List<AutoPtr<ICallUpdateListener> > > > mCallUpdateListenerMap;

    AutoPtr<IPhone> mPhone;

    AutoPtr<IPhoneListener> mPhoneListener;

    /**
     * Handles the timeout for destroying disconnected calls.
     */
    AutoPtr<IHandler> mHandler;

    friend class PhoneListener;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CALLLIST_H__
