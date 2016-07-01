
#include "elastos/droid/dialer/PhoneCallDetailsHelper.h"

namespace Elastos {
namespace Droid {
namespace Dialer {

const Int32 PhoneCallDetailsHelper::MAX_CALL_TYPE_ICONS = 3;

CAR_INTERFACE_IMPL(PhoneCallDetailsHelper, Object, IPhoneCallDetailsHelper);

PhoneCallDetailsHelper::PhoneCallDetailsHelper()
{
    CArrayList::New((ArrayList**)&mDescriptionItems);
}

ECode PhoneCallDetailsHelper::construtor(
    /* [in] */ IResources* resources,
    /* [in] */ ICallTypeHelper* callTypeHelper,
    /* [in] */ IPhoneNumberUtilsWrapper* phoneUtils)
{
    mResources = resources;
    mPhoneNumberUtilsWrapper = phoneUtils;
    CPhoneNumberDisplayHelper::New(mPhoneNumberUtilsWrapper, resources,
            (IPhoneNumberDisplayHelper**)&mPhoneNumberHelper);
    return NOERROR;
}

ECode PhoneCallDetailsHelper::SetPhoneCallDetails(
    /* [in] */ IPhoneCallDetailsViews* views,
    /* [in] */ IPhoneCallDetails* details)
{
    PhoneCallDetailsViews* pcViews = (PhoneCallDetailsViews*)views;
    CPhoneCallDetails* cpcDetails = (CPhoneCallDetails*)details;

    // Display up to a given number of icons.
    pcViews->mCallTypeIcons->Clear();
    Int32 count = details.callTypes.length;
    Boolean isVoicemail = FALSE;
    for (Int32 index = 0; index < count && index < MAX_CALL_TYPE_ICONS; ++index) {
        pcViews->mCallTypeIcons->Add(cpcDetails->mCallTypes[index]);
        if (index == 0) {
            isVoicemail = cpcDetails->mCallTypes[index] == ICalls::VOICEMAIL_TYPE;
        }
    }

    // Show the video icon if the call had video enabled.
    pcViews->mCallTypeIcons->SetShowVideo(
            (cpcDetails->mFeatures & ICalls::FEATURES_VIDEO) == ICalls::FEATURES_VIDEO);
    pcViews->mCallTypeIcons->RequestLayout();
    pcViews->mCallTypeIcons->SetVisibility(IView::VISIBLE);

    // Show the total call count only if there are more than the maximum number of icons.
    AutoPtr<IInteger32> callCount;
    if (count > MAX_CALL_TYPE_ICONS) {
        CInteger32::New(count, (IInteger32**)&callCount);
    }
    else {
        callCount = NULL;
    }

    AutoPtr<ICharSequence> callLocationAndDate = GetCallLocationAndDate(details);

    // Set the call count, location and date.
    SetCallCountAndDate(views, callCount, callLocationAndDate);

    // set the account icon if it exists
    if (cpcDetails->mAccountIcon != null) {
        pcViews->mCallAccountIcon->SetVisibility(IView::VISIBLE);
        pcViews->mCallAccountIcon->SetImageDrawable(cpcDetails->mAccountIcon);
    }
    else {
        pcViews->mCallAccountIcon->SetVisibility(IView::GONE);
    }

    AutoPtr<ICharSequence> nameText;
    AutoPtr<ICharSequence> displayNumber;
    mPhoneNumberHelper->GetDisplayNumber(cpcDetails->mNumber,
                cpcDetails->mNumberPresentation, cpcDetails->mFormattedNumber,
                (ICharSequence**)&displayNumber);
    if (TextUtils::IsEmpty(cpcDetails->mName)) {
        nameText = displayNumber;
        // We have a real phone number as "nameView" so make it always LTR
        pcViews->mNameView->SetTextDirection(IView::TEXT_DIRECTION_LTR);
    }
    else {
        nameText = cpcDetails->mName;
    }

    pcViews->mNameView->SetText(nameText);

    if (isVoicemail && !TextUtils::isEmpty(cpcDetails->mTranscription)) {
        pcViews->mVoicemailTranscriptionView->SetText(cpcDetails->mTranscription);
        pcViews->mVoicemailTranscriptionView->SetVisibility(IView::VISIBLE);
    }
    else {
        pcViews->mVoicemailTranscriptionView->SetText(NULL);
        pcViews->mVoicemailTranscriptionView->SetVisibility(IView::GONE);
    }

    return NOERROR;
}

AutoPtr<ICharSequence> PhoneCallDetailsHelper::GetCallLocationAndDate(
    /* [in] */ IPhoneCallDetails* details)
{
    mDescriptionItems->Clear();

    // Get type of call (ie mobile, home, etc) if known, or the caller's location.
    AutoPtr<ICharSequence> callTypeOrLocation;
    GetCallTypeOrLocation(details, (ICharSequence**)&callTypeOrLocation);

    // Only add the call type or location if its not empty.  It will be empty for unknown
    // callers.
    if (!TextUtils::IsEmpty(callTypeOrLocation)) {
        mDescriptionItems->Add(callTypeOrLocation);
    }
    // The date of this call, relative to the current time.
    AutoPtr<ICharSequence> callDate;
    GetCallDate(details, (ICharSequence**)&callDate);
    mDescriptionItems->Add(callDate);

    // Create a comma separated list from the call type or location, and call date.
    return DialerUtils::Join(mResources, mDescriptionItems);
}

ECode PhoneCallDetailsHelper::GetCallTypeOrLocation(
    /* [in] */ IPhoneCallDetails* details,
    /* [out] */ ICharSequence* result)
{
    VALIDATE_NOT_NULL(result);

    CPhoneCallDetails* cpcDetails = (CPhoneCallDetails*)details;
    AutoPtr<ICharSequence> numberFormattedLabel;
    // Only show a label if the number is shown and it is not a SIP address.
    String number;
    Boolean isVoicemailNumber;
    if (!TextUtils::IsEmpty(cpcDetails->mNumber)
            && !PhoneNumberHelper::IsUriNumber(cpcDetails->mNumber->ToString(&number), number)
            && mPhoneNumberUtilsWrapper->IsVoicemailNumber(
                    cpcDetails->mNumber, &isVoicemailNumber), !isVoicemailNumber) {

        if (cpcDetails->mNumberLabel == IContactInfo::GEOCODE_AS_LABEL) {
            numberFormattedLabel = cpcDetails->mGeocode;
        }
        else {
            AutoPtr<IContactsContractCommonDataKindsPhone> phone;
            CContactsContractCommonDataKindsPhone::AcquireSingleton(
                    (IContactsContractCommonDataKindsPhone**)&phone);
            phone->GetTypeLabel(mResources, cpcDetails->mNumberType,
                    cpcDetails->mNumberLabel, (ICharSequence**)&numberFormattedLabel);
        }
    }

    if (!TextUtils::IsEmpty(cpcDetails->mName) && TextUtils::IsEmpty(numberFormattedLabel)) {
        mPhoneNumberHelper->GetDisplayNumber(cpcDetails->mNumber,
                cpcDetails->mNumberPresentation, cpcDetails->mFormattedNumber,
                (ICharSequence**)&numberFormattedLabel);
    }
    *result = numberFormattedLabel;
    if (numberFormattedLabel != NULL) {
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode PhoneCallDetailsHelper::GetCallDate(
    /* [in] */ IPhoneCallDetails* details,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IDateUtils> utils;
    CDateUtils::AcquireSingleton((IDateUtils**)&utils);
    utils->GetRelativeTimeSpanString(((CPhoneCallDetails*)details)->mDate,
            GetCurrentTimeMillis(), IDateUtils::MINUTE_IN_MILLIS,
            IDateUtils::FORMAT_ABBREV_RELATIVE, result);
    return NOERROR;
}

ECode PhoneCallDetailsHelper::SetCallDetailsHeader(
    /* [in] */ ITextView* nameView,
    /* [in] */ IPhoneCallDetails* details)
{
    AutoPtr<ICharSequence> nameText;
    AutoPtr<ICharSequence> displayNumber;
    String str;
    mResources->GetString(R::string::recentCalls_addToContact, &str)
    mPhoneNumberHelper->GetDisplayNumber(((CPhoneCallDetails*)details)->mNumber,
            ((CPhoneCallDetails*)details)->mNumberPresentation,
            str, (ICharSequence**)&displayNumber);
    if (TextUtils::IsEmpty(((CPhoneCallDetails*)details)->mName)) {
        nameText = displayNumber;
    }
    else {
        nameText = ((CPhoneCallDetails*)details)->mName;
    }

    nameView->SetText(nameText);
    return NOERROR;
}

ECode PhoneCallDetailsHelper::SetCurrentTimeForTest(
    /* [in] */ Int64 currentTimeMillis)
{
    CInteger64::New(currentTimeMillis, (IInteger64**)&mCurrentTimeMillisForTest);
    return NOERROR;
}

Int64 PhoneCallDetailsHelper::GetCurrentTimeMillis()
{
    Int64 value;
    if (mCurrentTimeMillisForTest == NULL) {
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetCurrentTimeMillis(&value);
    }
    else {
        mCurrentTimeMillisForTest->GetValue(&value);
    }
    return value;
}

void PhoneCallDetailsHelper::SetCallCountAndDate(
    /* [in] */ IPhoneCallDetailsViews* views,
    /* [in] */ IInteger32* callCount,
    /* [in] */ ICharSequence* dateText)
{
    // Combine the count (if present) and the date.
    AutoPtr<ICharSequence> text;
    if (callCount != NULL) {
        Int32 value;
        callCount->GetValue(&value);
        mResources->getString(R::string::call_log_item_count_and_date,
                 value, dateText, (ICharSequence**)&text);
    }
    else {
        text = dateText;
    }

    ((PhoneCallDetailsViews*)views)->mCallLocationAndDate->SetText(text);
}

} // Dialer
} // Droid
} // Elastos
