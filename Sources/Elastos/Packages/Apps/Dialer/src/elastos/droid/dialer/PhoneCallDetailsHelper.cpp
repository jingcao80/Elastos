
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/contacts/common/util/PhoneNumberHelper.h"
#include "elastos/droid/dialer/PhoneCallDetailsHelper.h"
#include "elastos/droid/dialer/calllog/ContactInfo.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"

using Elastos::Droid::Contacts::Common::Util::PhoneNumberHelper;
using Elastos::Droid::Dialer::CallLog::ContactInfo;
using Elastos::Droid::Dialer::Util::DialerUtils;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {

const Int32 PhoneCallDetailsHelper::MAX_CALL_TYPE_ICONS;

PhoneCallDetailsHelper::PhoneCallDetailsHelper(
    /* [in] */ IResources* resources,
    /* [in] */ CallTypeHelper* callTypeHelper,
    /* [in] */ PhoneNumberUtilsWrapper* phoneUtils)
    : mCurrentTimeMillisForTest(0)
{
    CArrayList::New((IArrayList**)&mDescriptionItems);
    mResources = resources;
    mPhoneNumberUtilsWrapper = phoneUtils;
    mPhoneNumberHelper = new PhoneNumberDisplayHelper(mPhoneNumberUtilsWrapper, resources);
}

void PhoneCallDetailsHelper::SetPhoneCallDetails(
    /* [in] */ PhoneCallDetailsViews* views,
    /* [in] */ PhoneCallDetails* details)
{
    // Display up to a given number of icons.
    views->mCallTypeIcons->Clear();
    Int32 count = details->mCallTypes->GetLength();
    Boolean isVoicemail = FALSE;
    for (Int32 index = 0; index < count && index < MAX_CALL_TYPE_ICONS; ++index) {
        views->mCallTypeIcons->Add((*details->mCallTypes)[index]);
        if (index == 0) {
            isVoicemail = (*details->mCallTypes)[index] == ICalls::VOICEMAIL_TYPE;
        }
    }

    // Show the video icon if the call had video enabled.
    views->mCallTypeIcons->SetShowVideo(
            (details->mFeatures & ICalls::FEATURES_VIDEO) == ICalls::FEATURES_VIDEO);
    AutoPtr<IView> icons = IView::Probe(views->mCallTypeIcons);
    icons->RequestLayout();
    icons->SetVisibility(IView::VISIBLE);

    // Show the total call count only if there are more than the maximum number of icons.
    AutoPtr<IInteger32> callCount;
    if (count > MAX_CALL_TYPE_ICONS) {
        CInteger32::New(count, (IInteger32**)&callCount);
    }

    AutoPtr<ICharSequence> callLocationAndDate = GetCallLocationAndDate(details);

    // Set the call count, location and date.
    SetCallCountAndDate(views, callCount, callLocationAndDate);

    // set the account icon if it exists
    AutoPtr<IView> callAccountIcon = IView::Probe(views->mCallAccountIcon);
    if (details->mAccountIcon != NULL) {
        callAccountIcon->SetVisibility(IView::VISIBLE);
        views->mCallAccountIcon->SetImageDrawable(details->mAccountIcon);
    }
    else {
        callAccountIcon->SetVisibility(IView::GONE);
    }

    AutoPtr<ICharSequence> nameText;
    AutoPtr<ICharSequence> displayNumber = mPhoneNumberHelper->GetDisplayNumber(details->mNumber,
                details->mNumberPresentation, details->mFormattedNumber);
    if (TextUtils::IsEmpty(details->mName)) {
        nameText = displayNumber;
        // We have a real phone number as "nameView" so make it always LTR
        IView::Probe(views->mNameView)->SetTextDirection(IView::TEXT_DIRECTION_LTR);
    }
    else {
        nameText = details->mName;
    }

    views->mNameView->SetText(nameText);

    if (isVoicemail && !TextUtils::IsEmpty(details->mTranscription)) {
        AutoPtr<ICharSequence> cs;
        CString::New(details->mTranscription, (ICharSequence**)&cs);
        views->mVoicemailTranscriptionView->SetText(cs);
        IView::Probe(views->mVoicemailTranscriptionView)->SetVisibility(IView::VISIBLE);
    }
    else {
        views->mVoicemailTranscriptionView->SetText(NULL);
        IView::Probe(views->mVoicemailTranscriptionView)->SetVisibility(IView::GONE);
    }
}

AutoPtr<ICharSequence> PhoneCallDetailsHelper::GetCallLocationAndDate(
    /* [in] */ PhoneCallDetails* details)
{
    mDescriptionItems->Clear();

    // Get type of call (ie mobile, home, etc) if known, or the caller's location.
    AutoPtr<ICharSequence> callTypeOrLocation = GetCallTypeOrLocation(details);

    // Only add the call type or location if its not empty.  It will be empty for unknown
    // callers.
    if (!TextUtils::IsEmpty(callTypeOrLocation)) {
        mDescriptionItems->Add(callTypeOrLocation);
    }
    // The date of this call, relative to the current time.
    AutoPtr<ICharSequence> callDate = GetCallDate(details);
    mDescriptionItems->Add(callDate);

    // Create a comma separated list from the call type or location, and call date.
    return DialerUtils::Join(mResources, IIterable::Probe(mDescriptionItems));
}

AutoPtr<ICharSequence> PhoneCallDetailsHelper::GetCallTypeOrLocation(
    /* [in] */ PhoneCallDetails* details)
{
    AutoPtr<ICharSequence> numberFormattedLabel;
    // Only show a label if the number is shown and it is not a SIP address.
    String number;
    if (!TextUtils::IsEmpty(details->mNumber)
            && !PhoneNumberHelper::IsUriNumber((details->mNumber->ToString(&number), number))
            && mPhoneNumberUtilsWrapper->IsVoicemailNumber(details->mNumber)) {

        String label;
        details->mNumberLabel->ToString(&label);
        if (label.Equals(ContactInfo::GEOCODE_AS_LABEL)) {
            CString::New(details->mGeocode, (ICharSequence**)&numberFormattedLabel);
        }
        else {
            AutoPtr<IContactsContractCommonDataKindsPhone> phone;
            CContactsContractCommonDataKindsPhone::AcquireSingleton((IContactsContractCommonDataKindsPhone**)&phone);
            phone->GetTypeLabel(mResources, details->mNumberType,
                    details->mNumberLabel, (ICharSequence**)&numberFormattedLabel);
        }
    }

    if (!TextUtils::IsEmpty(details->mName) && TextUtils::IsEmpty(numberFormattedLabel)) {
        numberFormattedLabel = mPhoneNumberHelper->GetDisplayNumber(details->mNumber,
                details->mNumberPresentation, details->mFormattedNumber);
    }
    return numberFormattedLabel;
}

AutoPtr<ICharSequence> PhoneCallDetailsHelper::GetCallDate(
    /* [in] */ PhoneCallDetails* details)
{
    AutoPtr<IDateUtils> utils;
    CDateUtils::AcquireSingleton((IDateUtils**)&utils);
    AutoPtr<ICharSequence> cs;
    utils->GetRelativeTimeSpanString(details->mDate,
            GetCurrentTimeMillis(), IDateUtils::MINUTE_IN_MILLIS,
            IDateUtils::FORMAT_ABBREV_RELATIVE, (ICharSequence**)&cs);
    return cs;
}

void PhoneCallDetailsHelper::SetCallDetailsHeader(
    /* [in] */ ITextView* nameView,
    /* [in] */ PhoneCallDetails* details)
{
    AutoPtr<ICharSequence> nameText;
    String str;
    mResources->GetString(Elastos::Droid::Dialer::R::string::recentCalls_addToContact, &str);
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    AutoPtr<ICharSequence> displayNumber = mPhoneNumberHelper->GetDisplayNumber(
            details->mNumber, details->mNumberPresentation, cs);
    if (TextUtils::IsEmpty(details->mName)) {
        nameText = displayNumber;
    }
    else {
        nameText = details->mName;
    }

    nameView->SetText(nameText);
}

void PhoneCallDetailsHelper::SetCurrentTimeForTest(
    /* [in] */ Int64 currentTimeMillis)
{
    mCurrentTimeMillisForTest = currentTimeMillis;
}

Int64 PhoneCallDetailsHelper::GetCurrentTimeMillis()
{
    if (mCurrentTimeMillisForTest == 0) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 value;
        system->GetCurrentTimeMillis(&value);
        return value;
    }
    else {
        return mCurrentTimeMillisForTest;
    }
}

void PhoneCallDetailsHelper::SetCallCountAndDate(
    /* [in] */ PhoneCallDetailsViews* views,
    /* [in] */ IInteger32* callCount,
    /* [in] */ ICharSequence* dateText)
{
    // Combine the count (if present) and the date.
    AutoPtr<ICharSequence> text;
    if (callCount != NULL) {
        Int32 value;
        callCount->GetValue(&value);
        AutoPtr<ArrayOf<IInterface*> > attrs = ArrayOf<IInterface*>::Alloc(2);
        attrs->Set(0, callCount);
        attrs->Set(1, dateText);
        String str;
        mResources->GetString(Elastos::Droid::Dialer::R::string::call_log_item_count_and_date,
                 attrs, &str);
        CString::New(str, (ICharSequence**)&text);
    }
    else {
        text = dateText;
    }

    views->mCallLocationAndDate->SetText(text);
}

} // Dialer
} // Droid
} // Elastos
