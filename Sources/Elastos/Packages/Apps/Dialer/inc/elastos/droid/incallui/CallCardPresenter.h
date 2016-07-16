
#ifndef __ELASTOS_DROID_INCALLUI_CALLCARDPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_CALLCARDPRESENTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/CallTimer.h"
#include "elastos/droid/incallui/ContactInfoCache.h"
#include "elastos/droid/incallui/Presenter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Telecomm::Telecom::ICallDetails;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccount;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class CallCardPresenter
    : public Presenter
    , public IInCallStateListener
    , public IIncomingCallListener
    , public IInCallDetailsListener
    , public IInCallEventListener
{
public:
    class ContactLookupCallback
        : public Object
        , public IContactInfoCacheCallback
    {
    public:
        ContactLookupCallback(
            /* [in] */ CallCardPresenter* callCardPresenter,
            /* [in] */ Boolean isPrimary);

        // @Override
        CARAPI OnContactInfoComplete(
            /* [in] */ const String& callId,
            /* [in] */ IContactCacheEntry* entry);

        CARAPI OnImageLoadComplete(
            /* [in] */ const String& callId,
            /* [in] */ IContactCacheEntry* entry);

    private:
        AutoPtr<IWeakReference> mCallCardPresenter;
        Boolean mIsPrimary;
    };

public:
    CAR_INTERFACE_DECL();

    CallCardPresenter();

    CARAPI_(void) Init(
        /* [in] */ IContext* context,
        /* [in] */ Call* call);

    // @Override
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnIncomingCall(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICall* call);

    // @Override
    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    // @Override
    CARAPI OnDetailsChanged(
        /* [in] */ ICall* call,
        /* [in] */ ICallDetails* details);

    CARAPI_(void) UpdateCallTime();

    CARAPI_(void) OnCallStateButtonTouched();

    CARAPI_(void) SecondaryInfoClicked();

    CARAPI_(void) EndCallClicked();

    /**
     * Handles a change to the full screen video state.
     *
     * @param isFullScreenVideo {@code True} if the application is entering full screen video mode.
     */
    // @Override
    CARAPI OnFullScreenVideoStateChanged(
        /* [in] */ Boolean isFullScreenVideo);

private:
    CARAPI_(String) GetSubscriptionNumber();

    CARAPI_(void) UpdatePrimaryCallState();

    /**
     * Only show the conference call button if we can manage the conference.
     */
    CARAPI_(void) MaybeShowManageConferenceCallButton();

    CARAPI_(void) SetCallbackNumber();

    CARAPI_(void) MaybeStartSearch(
        /* [in] */ Call* call,
        /* [in] */ Boolean isPrimary);

    /**
     * Starts a query for more contact data for the save primary and secondary calls.
     */
    CARAPI_(void) StartContactInfoSearch(
        /* [in] */ Call* call,
        /* [in] */ Boolean isPrimary,
        /* [in] */ Boolean isIncoming);

    CARAPI_(void) OnContactInfoComplete(
        /* [in] */ const String& callId,
        /* [in] */ ContactInfoCache::ContactCacheEntry* entry,
        /* [in] */ Boolean isPrimary);

    CARAPI_(void) OnImageLoadComplete(
        /* [in] */ const String& callId,
        /* [in] */ ContactInfoCache::ContactCacheEntry* entry);

    static CARAPI_(Boolean) IsConference(
        /* [in] */ Call* call);

    static CARAPI_(Boolean) CanManageConference(
        /* [in] */ Call* call);

    CARAPI_(void) UpdateContactEntry(
        /* [in] */ ContactInfoCache::ContactCacheEntry* entry,
        /* [in] */ Boolean isPrimary,
        /* [in] */ Boolean isConference);

    /**
     * Get the highest priority call to display.
     * Goes through the calls and chooses which to return based on priority of which type of call
     * to display to the user. Callers can use the "ignore" feature to get the second best call
     * by passing a previously found primary call as ignore.
     *
     * @param ignore A call to ignore if found.
     */
    CARAPI_(AutoPtr<Call>) GetCallToDisplay(
        /* [in] */ CallList* callList,
        /* [in] */ Call* ignore,
        /* [in] */ Boolean skipDisconnected);

    CARAPI_(void) UpdatePrimaryDisplayInfo(
        /* [in] */ ContactInfoCache::ContactCacheEntry* entry,
        /* [in] */ Boolean isConference);

    CARAPI_(void) UpdateSecondaryDisplayInfo(
        /* [in] */ Boolean isConference);

    /**
     * Gets the phone account to display for a call.
     */
    CARAPI_(AutoPtr<IPhoneAccount>) GetAccountForCall(
        /* [in] */ Call* call);

    /**
     * Returns the gateway number for any existing outgoing call.
     */
    CARAPI_(String) GetGatewayNumber();

    /**
     * Return the Drawable object of the icon to display to the left of the connection label.
     */
    CARAPI_(AutoPtr<IDrawable>) GetCallProviderIcon(
        /* [in] */ Call* call);

    /**
     * Return the string label to represent the call provider
     */
    CARAPI_(String) GetCallProviderLabel(
        /* [in] */ Call* call);

    /**
     * Returns the label (line of text above the number/name) for any given call.
     * For example, "calling via [Account/Google Voice]" for outgoing calls.
     */
    CARAPI_(String) GetConnectionLabel();

    CARAPI_(AutoPtr<IDrawable>) GetConnectionIcon();

    CARAPI_(Boolean) HasOutgoingGatewayCall();

    /**
     * Gets the name to display for the call.
     */
    static CARAPI_(String) GetNameForCall(
        /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo);

    /**
     * Gets the number to display for a call.
     */
    static CARAPI_(String) GetNumberForCall(
        /* [in] */ ContactInfoCache::ContactCacheEntry* contactInfo);

    CARAPI_(String) GetNumberFromHandle(
        /* [in] */ IUri* handle);

    CARAPI_(AutoPtr<ITelecomManager>) GetTelecomManager();

private:
    static const String TAG;
    static const Int64 CALL_TIME_UPDATE_INTERVAL_MS;

    AutoPtr<Call> mPrimary;
    AutoPtr<Call> mSecondary;
    AutoPtr<ContactInfoCache::ContactCacheEntry> mPrimaryContactInfo;
    AutoPtr<ContactInfoCache::ContactCacheEntry> mSecondaryContactInfo;
    AutoPtr<CallTimer> mCallTimer;
    AutoPtr<IContext> mContext;
    AutoPtr<ITelecomManager> mTelecomManager;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CALLCARDPRESENTER_H__
