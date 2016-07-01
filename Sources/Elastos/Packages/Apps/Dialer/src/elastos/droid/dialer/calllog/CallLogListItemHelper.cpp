
#include "elastos/droid/dialer/calllog/CallLogListItemHelper.h"
#include "elastos/droid/dialer/calllog/CallLogListItemViews.h"
#include "elastos/droid/dialer/CPhoneCallDetails.h"
#include "R.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Apps::Dialer::CPhoneCallDetails;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CallLogListItemHelper::CallLogListItemHelper(
    /* [in] */ IPhoneCallDetailsHelper* phoneCallDetailsHelper,
    /* [in] */ IPhoneNumberDisplayHelper* phoneNumberHelper,
    /* [in] */ IResources* resources)
    : mPhoneCallDetailsHelper(phoneCallDetailsHelper)
    , mPhoneNumberHelper(phoneNumberHelper)
    , mResources(resources)
{}

void CallLogListItemHelper::SetPhoneCallDetails(
    /* [in] */ IContext* context,
    /* [in] */ ICallLogListItemViews* views,
    /* [in] */ IPhoneCallDetails* details)
{
    CallLogListItemViews* cviews = (CallLogListItemViews*)views;
    mPhoneCallDetailsHelper->SetPhoneCallDetails(
            cviews->mPhoneCallDetailsViews, details);

    // Set the accessibility text for the contact badge
    IView::Probe(cviews->mQuickContactView)->SetContentDescription(
            GetContactBadgeDescription(details));

    // Set the primary action accessibility description
    cviews->mPrimaryActionView->SetContentDescription(
            GetCallDescription(context, details));

    // Cache name or number of caller.  Used when setting the content descriptions of buttons
    // when the actions ViewStub is inflated.
    cviews->mNameOrNumber = GetNameOrNumber(details);
}

/**
 * Sets the accessibility descriptions for the action buttons in the action button ViewStub.
 *
 * @param views The views associated with the current call log entry.
 */
void CallLogListItemHelper::SetActionContentDescriptions(
    /* [in] */ ICallLogListItemViews* views)
{
    CallLogListItemViews* cviews = (CallLogListItemViews*)views;
    String str;
    AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    formatArgs->Set(0, cviews->mNameOrNumber);
    mResources->GetString(
            R::string::description_call_back_action, formatArgs, &str);
    IView::Probe(cviews->mCallBackButtonView)->SetContentDescription(CoreUtils::Convert(str));

    mResources->GetString(
            R::string::description_video_call_action, formatArgs, &str);
    IView::Probe(cviews->mVideoCallButtonView)->SetContentDescription(CoreUtils::Convert(str));

    mResources->GetString(
            R::string::description_voicemail_action, formatArgs, &str);
    IView::Probe(cviews->mVoicemailButtonView)->SetContentDescription(CoreUtils::Convert(str));

    mResources->GetString(
            R::string::description_details_action, formatArgs, &str);
    IView::Probe(cviews->mDetailsButtonView)->SetContentDescription(CoreUtils::Convert(str));
}

AutoPtr<ICharSequence> CallLogListItemHelper::GetContactBadgeDescription(
    /* [in] */ IPhoneCallDetails* details)
{
    String str;
    AutoPtr<ICharSequence> nameOrNumber = GetNameOrNumber(details);
    AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    formatArgs->Set(0, nameOrNumber);
    mResources->GetString(
            R::string::description_contact_details, formatArgs, &str);
    return CoreUtils::Convert(str);
}

AutoPtr<ICharSequence> CallLogListItemHelper::GetCallDescription(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneCallDetails* details)
{
    CPhoneCallDetails* cdetails = (CPhoneCallDetails*)details;
    Int32 lastCallType = GetLastCallType(cdetails->mCallTypes);
    Boolean isVoiceMail = lastCallType == ICalls::VOICEMAIL_TYPE;

    // Get the name or number of the caller.
    AutoPtr<ICharSequence> nameOrNumber = GetNameOrNumber(details);

    // Get the call type or location of the caller; null if not applicable
    AutoPtr<ICharSequence> typeOrLocation;
    mPhoneCallDetailsHelper->GetCallTypeOrLocation(
            details, (ICharSequence**)&typeOrLocation);

    // Get the time/date of the call
    AutoPtr<ICharSequence> timeOfCall;
    mPhoneCallDetailsHelper->GetCallDate(details, (ICharSequence**)&timeOfCall);

    AutoPtr<StringBuilder> callDescription = new StringBuilder();

    // Prepend the voicemail indication.
    if (isVoiceMail) {
        String str;
        mResources->GetString(R::string::description_new_voicemail, &str);
        callDescription->Append(str);
    }

    // Add number of calls if more than one.
    if (cdetails->mCallTypes->GetLength() > 1) {
        String str;
        AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
        formatArgs->Set(0, CoreUtils::Convert(cdetails->mCallTypes->GetLength()));
        mResources->GetString(
                R::string::description_num_calls, formatArgs, &str);
        callDescription->Append(str);
    }

    // If call had video capabilities, add the "Video Call" string.
    assert(0 && "TODO");
    // if ((cdetails->mFeatures & ICalls::FEATURES_VIDEO) == ICalls::FEATURES_VIDEO &&
    //         CallUtil::IsVideoEnabled(context)) {
    //     String str;
    //     mResources->GetString(R::string::description_video_call, &str);
    //     callDescription->Append(str);
    // }

    Int32 stringID = GetCallDescriptionStringID(details);

    // Use chosen string resource to build up the message.
    String str;
    AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(3);
    formatArgs->Set(0, nameOrNumber);
    // If no type or location can be determined, sub in empty string.
    if (typeOrLocation == NULL) {
        formatArgs->Set(1, CoreUtils::Convert(String("")));
    }
    else {
        formatArgs->Set(1, typeOrLocation);
    }
    formatArgs->Set(2, timeOfCall);
    mResources->GetString(stringID, formatArgs, &str);
    callDescription->Append(str);

    return ICharSequence::Probe(callDescription);
}

Int32 CallLogListItemHelper::GetCallDescriptionStringID(
    /* [in] */ IPhoneCallDetails* details)
{
    Int32 lastCallType = GetLastCallType(((CPhoneCallDetails*)details)->mCallTypes);
    Int32 stringID;

    if (lastCallType == ICalls::VOICEMAIL_TYPE || lastCallType == ICalls::MISSED_TYPE) {
        //Message: Missed call from <NameOrNumber>, <TypeOrLocation>, <TimeOfCall>.
        stringID = R::string::description_incoming_missed_call;
    }
    else if (lastCallType == ICalls::INCOMING_TYPE) {
        //Message: Answered call from <NameOrNumber>, <TypeOrLocation>, <TimeOfCall>.
        stringID = R::string::description_incoming_answered_call;
    }
    else {
        //Message: Call to <NameOrNumber>, <TypeOrLocation>, <TimeOfCall>.
        stringID = R::string::description_outgoing_call;
    }
    return stringID;
}

Int32 CallLogListItemHelper::GetLastCallType(
    /* [in] */ ArrayOf<Int32>* callTypes)
{
    if (callTypes->GetLength() > 0) {
        return (*callTypes)[0];
    }
    else {
        return ICalls::MISSED_TYPE;
    }
}

AutoPtr<ICharSequence> CallLogListItemHelper::GetNameOrNumber(
    /* [in] */ IPhoneCallDetails* details)
{
    CPhoneCallDetails* cdetails = (CPhoneCallDetails*)details;
    AutoPtr<ICharSequence> recipient;
    if (!TextUtils::IsEmpty(cdetails->mName)) {
        recipient = cdetails->mName;
    }
    else {
         mPhoneNumberHelper->GetDisplayNumber(
                cdetails->mNumber, cdetails->mNumberPresentation,
                cdetails->mFormattedNumber, (ICharSequence**)&recipient);
    }
    return recipient;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
