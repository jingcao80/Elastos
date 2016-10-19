
#include "elastos/droid/contacts/common/CallUtil.h"
#include "elastos/droid/dialer/calllog/CallLogListItemHelper.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "R.h"

using Elastos::Droid::Contacts::Common::CallUtil;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CallLogListItemHelper::CallLogListItemHelper(
    /* [in] */ PhoneCallDetailsHelper* phoneCallDetailsHelper,
    /* [in] */ PhoneNumberDisplayHelper* phoneNumberHelper,
    /* [in] */ IResources* resources)
    : mPhoneCallDetailsHelper(phoneCallDetailsHelper)
    , mPhoneNumberHelper(phoneNumberHelper)
    , mResources(resources)
{}

void CallLogListItemHelper::SetPhoneCallDetails(
    /* [in] */ IContext* context,
    /* [in] */ CallLogListItemViews* views,
    /* [in] */ PhoneCallDetails* details)
{
    mPhoneCallDetailsHelper->SetPhoneCallDetails(views->mPhoneCallDetailsViews, details);

    // Set the accessibility text for the contact badge
    IView::Probe(views->mQuickContactView)->SetContentDescription(
            GetContactBadgeDescription(details));

    // Set the primary action accessibility description
    views->mPrimaryActionView->SetContentDescription(
            GetCallDescription(context, details));

    // Cache name or number of caller.  Used when setting the content descriptions of buttons
    // when the actions ViewStub is inflated.
    views->mNameOrNumber = GetNameOrNumber(details);
}

void CallLogListItemHelper::SetActionContentDescriptions(
    /* [in] */ CallLogListItemViews* views)
{
    String str;
    AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    formatArgs->Set(0, views->mNameOrNumber);
    mResources->GetString(
            Elastos::Droid::Dialer::R::string::description_call_back_action, formatArgs, &str);
    IView::Probe(views->mCallBackButtonView)->SetContentDescription(CoreUtils::Convert(str));

    mResources->GetString(
            Elastos::Droid::Dialer::R::string::description_video_call_action, formatArgs, &str);
    IView::Probe(views->mVideoCallButtonView)->SetContentDescription(CoreUtils::Convert(str));

    mResources->GetString(
            Elastos::Droid::Dialer::R::string::description_voicemail_action, formatArgs, &str);
    IView::Probe(views->mVoicemailButtonView)->SetContentDescription(CoreUtils::Convert(str));

    mResources->GetString(
            Elastos::Droid::Dialer::R::string::description_details_action, formatArgs, &str);
    IView::Probe(views->mDetailsButtonView)->SetContentDescription(CoreUtils::Convert(str));
}

AutoPtr<ICharSequence> CallLogListItemHelper::GetContactBadgeDescription(
    /* [in] */ PhoneCallDetails* details)
{
    String str;
    AutoPtr<ICharSequence> nameOrNumber = GetNameOrNumber(details);
    AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
    formatArgs->Set(0, nameOrNumber);
    mResources->GetString(Elastos::Droid::Dialer::R::string::description_contact_details, formatArgs, &str);
    return CoreUtils::Convert(str);
}

AutoPtr<ICharSequence> CallLogListItemHelper::GetCallDescription(
    /* [in] */ IContext* context,
    /* [in] */ PhoneCallDetails* details)
{
    Int32 lastCallType = GetLastCallType(details->mCallTypes);
    Boolean isVoiceMail = lastCallType == ICalls::VOICEMAIL_TYPE;

    // Get the name or number of the caller.
    AutoPtr<ICharSequence> nameOrNumber = GetNameOrNumber(details);

    // Get the call type or location of the caller; null if not applicable
    AutoPtr<ICharSequence> typeOrLocation = mPhoneCallDetailsHelper->GetCallTypeOrLocation(details);

    // Get the time/date of the call
    AutoPtr<ICharSequence> timeOfCall = mPhoneCallDetailsHelper->GetCallDate(details);

    AutoPtr<StringBuilder> callDescription = new StringBuilder();

    // Prepend the voicemail indication.
    if (isVoiceMail) {
        String str;
        mResources->GetString(Elastos::Droid::Dialer::R::string::description_new_voicemail, &str);
        callDescription->Append(str);
    }

    // Add number of calls if more than one.
    if (details->mCallTypes->GetLength() > 1) {
        String str;
        AutoPtr<ArrayOf<IInterface*> > formatArgs = ArrayOf<IInterface*>::Alloc(1);
        formatArgs->Set(0, CoreUtils::Convert(details->mCallTypes->GetLength()));
        mResources->GetString(
                Elastos::Droid::Dialer::R::string::description_num_calls, formatArgs, &str);
        callDescription->Append(str);
    }

    // If call had video capabilities, add the "Video Call" string.
    if ((details->mFeatures & ICalls::FEATURES_VIDEO) == ICalls::FEATURES_VIDEO &&
            CallUtil::IsVideoEnabled(context)) {
        String str;
        mResources->GetString(Elastos::Droid::Dialer::R::string::description_video_call, &str);
        callDescription->Append(str);
    }

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
    /* [in] */ PhoneCallDetails* details)
{
    Int32 lastCallType = GetLastCallType(details->mCallTypes);
    Int32 stringID;

    if (lastCallType == ICalls::VOICEMAIL_TYPE || lastCallType == ICalls::MISSED_TYPE) {
        //Message: Missed call from <NameOrNumber>, <TypeOrLocation>, <TimeOfCall>.
        stringID = Elastos::Droid::Dialer::R::string::description_incoming_missed_call;
    }
    else if (lastCallType == ICalls::INCOMING_TYPE) {
        //Message: Answered call from <NameOrNumber>, <TypeOrLocation>, <TimeOfCall>.
        stringID = Elastos::Droid::Dialer::R::string::description_incoming_answered_call;
    }
    else {
        //Message: Call to <NameOrNumber>, <TypeOrLocation>, <TimeOfCall>.
        stringID = Elastos::Droid::Dialer::R::string::description_outgoing_call;
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
    /* [in] */ PhoneCallDetails* details)
{
    AutoPtr<ICharSequence> recipient;
    if (!TextUtils::IsEmpty(details->mName)) {
        recipient = details->mName;
    }
    else {
        return mPhoneNumberHelper->GetDisplayNumber(
                details->mNumber, details->mNumberPresentation,
                details->mFormattedNumber);
    }
    return recipient;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
