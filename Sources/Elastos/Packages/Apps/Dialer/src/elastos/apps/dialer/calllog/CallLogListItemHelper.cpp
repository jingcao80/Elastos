
#include "CallLogListItemHelper.h"

namespace Elastos{
namespace Apps{
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
    CCallLogListItemViews* cviews = (CCallLogListItemViews*)views;
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
    CCallLogListItemViews* cviews = (CCallLogListItemViews*)views;
    String str;
    mResources->GetString(
            R::string::description_call_back_action, cviews->mNameOrNumber, &str);
    IView::Probe(cviews->mCallBackButtonView)->SetContentDescription(str);

    mResources->GetString(
            R::string::description_video_call_action, cviews->mNameOrNumber, &str);
    IView::Probe(cviews->mVideoCallButtonView)->SetContentDescription(str);

    mResources->GetString(
            R::string::description_voicemail_action, cviews->mNameOrNumber, &str);
    IView::Probe(cviews->mVoicemailButtonView)->SetContentDescription(str);

    mResources->GetString(
            R::string::description_details_action, cviews->mNameOrNumber, &str);
    IView::Probe(cviews->mDetailsButtonView)->SetContentDescription(str);
}

AutoPtr<ICharSequence> CallLogListItemHelper::GetContactBadgeDescription(
    /* [in] */ IPhoneCallDetails* details)
{
    String str;
    mResources->GetString(R::string::description_contact_details,
            GetNameOrNumber(details), &str);
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

    AutoPtr<IStringBuilder> callDescription;
    CStringBuilder::New((IStringBuilder**)&callDescription);

    // Prepend the voicemail indication.
    if (isVoiceMail) {
        String str;
        mResources->GetString(R::string::description_new_voicemail, &str);
        callDescription->Append(str);
    }

    // Add number of calls if more than one.
    if (cdetails->mCallTypes->GetLength() > 1) {
        String str;
        mResources.getString(R::string.description_num_calls,
                cdetails->mCallTypes->GetLength(), &str);
        callDescription->Append(str);
    }

    // If call had video capabilities, add the "Video Call" string.
    if ((cdetails->mFeatures & ICalls::FEATURES_VIDEO) == ICalls::FEATURES_VIDEO &&
            CallUtil::IsVideoEnabled(context)) {
        String str;
        mResources->GetString(R::string::description_video_call, &str);
        callDescription->Append(str);
    }

    Int32 stringID = GetCallDescriptionStringID(details);

    // Use chosen string resource to build up the message.
    String str;
    mResources->GetString(stringID, nameOrNumber,
            // If no type or location can be determined, sub in empty string.
            typeOrLocation == NULL ? "" : typeOrLocation,
            timeOfCall, &str);
    callDescription->Append(str);

    return ICharSequence::Probe(callDescription);
}

Int32 CallLogListItemHelper::GetCallDescriptionStringID(
    /* [in] */ IPhoneCallDetails* details)
{
    Int32 lastCallType = getLastCallType(details.callTypes);
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
    /* [in] */ const ArrayOf<Int32>& callTypes)
{
    if (callTypes.GetLength() > 0) {
        return callTypes[0];
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
                cdetails->mUumber, cdetails->mNumberPresentation,
                cdetails->mFormattedNumber, (ICharSequence**)&recipient);
    }
    return recipient;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
