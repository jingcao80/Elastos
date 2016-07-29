
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/ConferenceManagerPresenter.h"
#include "elastos/droid/incallui/ContactInfoCache.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

const Int32 ConferenceManagerPresenter::MAX_CALLERS_IN_CONFERENCE = 5;

CAR_INTERFACE_IMPL(ConferenceManagerPresenter, Presenter, IInCallStateListener)

ECode ConferenceManagerPresenter::OnUiReady(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiReady(ui));

    // register for call state changes last
    InCallPresenter::GetInstance()->AddListener(this);
    return NOERROR;
}

ECode ConferenceManagerPresenter::OnUiUnready(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(Presenter::OnUiUnready(ui));

    InCallPresenter::GetInstance()->RemoveListener(this);
    return NOERROR;
}

ECode ConferenceManagerPresenter::OnStateChange(
    /* [in] */ InCallState oldState,
    /* [in] */ InCallState newState,
    /* [in] */ ICallList* callList)
{
    AutoPtr<IUi> _ui;
    GetUi((IUi**)&_ui);
    IConferenceManagerUi* ui = IConferenceManagerUi::Probe(_ui);
    Boolean visible;
    if (ui->IsFragmentVisible(&visible), visible) {
        Logger::V("ConferenceManagerPresenter", "onStateChange %d", newState);
        if (newState == InCallState_INCALL) {
            AutoPtr<Call> call = ((CallList*)callList)->GetActiveOrBackgroundCall();
            if (call != NULL && call->IsConferenceCall()) {
                Logger::V("ConferenceManagerPresenter", "Number of existing calls is %d",
                        call->GetChildCallIds().GetSize());
                Update(callList);
            }
            else {
                ui->SetVisible(FALSE);
            }
        }
        else {
            ui->SetVisible(FALSE);
        }
    }
    return NOERROR;
}

void ConferenceManagerPresenter::Init(
    /* [in] */ IContext* context,
    /* [in] */ ICallList* callList)
{
    assert(context != NULL);
    mContext = context;
    Update(callList);
}

void ConferenceManagerPresenter::Update(
    /* [in] */ ICallList* _callList)
{
    CallList* callList = (CallList*)_callList;
    // callList is non null, but getActiveOrBackgroundCall() may return null
    AutoPtr<Call> currentCall = callList->GetActiveOrBackgroundCall();
    if (currentCall != NULL) {
        // getChildCallIds() always returns a valid Set
        List<String>& callIds = currentCall->GetChildCallIds();
        mCallerIds = ArrayOf<String>::Alloc(callIds.GetSize());
        List<String>::Iterator it;
        Int32 i = 0;
        for (it = callIds.Begin(); it != callIds.End(); ++it, ++i) {
            (*mCallerIds)[i] = *it;
        }
    }
    else {
        mCallerIds = ArrayOf<String>::Alloc(0);
    }
    Logger::D("ConferenceManagerPresenter", "Number of calls is %d", mCallerIds->GetLength());

    // Users can split out a call from the conference call if there either the active call
    // or the holding call is empty. If both are filled at the moment, users can not split out
    // another call.
    Boolean hasActiveCall = (callList->GetActiveCall() != NULL);
    Boolean hasHoldingCall = (callList->GetBackgroundCall() != NULL);
    Boolean canSeparate = !(hasActiveCall && hasHoldingCall);

    for (Int32 i = 0; i < MAX_CALLERS_IN_CONFERENCE; i++) {
        if (i < mCallerIds->GetLength()) {
            AutoPtr<ICallDetails> details;
            callList->GetCallById(
                    currentCall->GetChildCallIds()[i])->GetTelecommCall()->GetDetails((ICallDetails**)&details);
            Int32 callCapabilities;
            details->GetCallCapabilities(&callCapabilities);
            Boolean thisRowCanSeparate = canSeparate &&
                    ((callCapabilities & IPhoneCapabilities::SEPARATE_FROM_CONFERENCE) != 0);
            Boolean thisRowCanDisconnect =
                    ((callCapabilities & IPhoneCapabilities::DISCONNECT_FROM_CONFERENCE) != 0);
            // Fill in the row in the UI for this caller.
            AutoPtr<ContactInfoCache::ContactCacheEntry> contactCache =
                    ContactInfoCache::GetInstance(mContext)->GetInfo((*mCallerIds)[i]);
            UpdateManageConferenceRow(
                    i,
                    contactCache,
                    thisRowCanSeparate,
                    thisRowCanDisconnect);
        }
        else {
            // Blank out this row in the UI
            UpdateManageConferenceRow(i, NULL, FALSE, FALSE);
        }
    }
}

void ConferenceManagerPresenter::UpdateManageConferenceRow(
    /* [in] */ Int32 i,
    /* [in] */ ContactInfoCache::ContactCacheEntry* contactCacheEntry,
    /* [in] */ Boolean canSeparate,
    /* [in] */ Boolean canDisconnect)
{
    AutoPtr<IUi> _ui;
    GetUi((IUi**)&_ui);
    IConferenceManagerUi* ui = IConferenceManagerUi::Probe(_ui);
    if (contactCacheEntry != NULL) {
        // Activate this row of the Manage conference panel:
        ui->SetRowVisible(i, TRUE);

        String name = contactCacheEntry->mName;
        String number = contactCacheEntry->mNumber;

        if (TextUtils::IsEmpty(name)) {
            name = number;
            number = NULL;
        }

        ui->SetupSeparateButtonForRow(i, canSeparate);
        ui->SetupEndButtonForRow(i, canDisconnect);
        ui->DisplayCallerInfoForConferenceRow(i, name, number, contactCacheEntry->mLabel,
                contactCacheEntry->mLookupKey, contactCacheEntry->mDisplayPhotoUri);
    }
    else {
        // Disable this row of the Manage conference panel:
        ui->SetRowVisible(i, FALSE);
    }
}

Int32 ConferenceManagerPresenter::GetMaxCallersInConference()
{
    return MAX_CALLERS_IN_CONFERENCE;
}

void ConferenceManagerPresenter::SeparateConferenceConnection(
    /* [in] */ Int32 rowId)
{
    if (rowId < mCallerIds->GetLength()) {
        TelecomAdapter::GetInstance()->SeparateCall((*mCallerIds)[rowId]);
    }
}

void ConferenceManagerPresenter::EndConferenceConnection(
    /* [in] */ Int32 rowId)
{
    if (rowId < mCallerIds->GetLength()) {
        TelecomAdapter::GetInstance()->DisconnectCall((*mCallerIds)[rowId]);
    }
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
