
#include "elastos/droid/incallui/CallerInfo.h"
#include "elastos/droid/contacts/common/util/PhoneNumberHelper.h"
#include "elastos/droid/contacts/common/util/TelephonyManagerUtils.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Contacts::Common::Util::PhoneNumberHelper;
using Elastos::Droid::Contacts::Common::Util::TelephonyManagerUtils;
using Elastos::Droid::InCallUI::EIID_ICallerInfo;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractPhoneLookupColumns;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractContactOptionsColumns;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::CContactsContractPhoneLookup;
using Elastos::Droid::Provider::IContactsContractRawContactsColumns;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

const String CallerInfo::TAG("CallerInfo");

CAR_INTERFACE_IMPL(CallerInfo, Object, ICallerInfo)

CallerInfo::CallerInfo()
    : mNumberPresentation(0)
    , mNamePresentation(0)
    , mContactExists(FALSE)
    , mNumberType(0)
    , mPhotoResource(0)
    , mContactIdOrZero(0)
    , mNeedUpdate(FALSE)
    , mShouldSendToVoicemail(FALSE)
    , mIsCachedPhotoCurrent(FALSE)
    , mIsEmergency(FALSE)
    , mIsVoiceMail(FALSE)
{}

AutoPtr<CallerInfo> CallerInfo::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<CallerInfo> info = new CallerInfo();
    info->mPhotoResource = 0;
    info->mPhoneLabel = String(NULL);
    info->mNumberType = 0;
    info->mNumberLabel = String(NULL);
    info->mCachedPhoto = NULL;
    info->mIsCachedPhotoCurrent = FALSE;
    info->mContactExists = FALSE;

    Logger::V(TAG, "getCallerInfo() based on cursor...");

    if (cursor != NULL) {
        Boolean result;
        if (cursor->MoveToFirst(&result), result) {
            // TODO: photo_id is always available but not taken
            // care of here. Maybe we should store it in the
            // CallerInfo object as well.

            Int32 columnIndex;

            // Look for the name
            cursor->GetColumnIndex(IContactsContractContactsColumns::DISPLAY_NAME, &columnIndex);
            if (columnIndex != -1) {
                cursor->GetString(columnIndex, &info->mName);
            }

            // Look for the number
            cursor->GetColumnIndex(IContactsContractPhoneLookupColumns::NUMBER, &columnIndex);
            if (columnIndex != -1) {
                cursor->GetString(columnIndex, &info->mPhoneNumber);
            }

            // Look for the normalized number
            cursor->GetColumnIndex(IContactsContractPhoneLookupColumns::NORMALIZED_NUMBER, &columnIndex);
            if (columnIndex != -1) {
                cursor->GetString(columnIndex, &info->mNormalizedNumber);
            }

            // Look for the label/type combo
            cursor->GetColumnIndex(IContactsContractPhoneLookupColumns::LABEL, &columnIndex);
            if (columnIndex != -1) {
                Int32 typeColumnIndex;
                cursor->GetColumnIndex(IContactsContractPhoneLookupColumns::TYPE, &typeColumnIndex);
                if (typeColumnIndex != -1) {
                    cursor->GetInt32(typeColumnIndex, &info->mNumberType);
                    cursor->GetString(columnIndex, &info->mNumberLabel);
                    AutoPtr<IContactsContractCommonDataKindsPhone> phone;
                    CContactsContractCommonDataKindsPhone::AcquireSingleton((IContactsContractCommonDataKindsPhone**)&phone);
                    AutoPtr<IResources> res;
                    context->GetResources((IResources**)&res);
                    AutoPtr<ICharSequence> label;
                    CString::New(info->mNumberLabel, (ICharSequence**)&label);
                    AutoPtr<ICharSequence> cs;
                    phone->GetTypeLabel(res, info->mNumberType, label, (ICharSequence**)&cs);
                    cs->ToString(&info->mPhoneLabel);
                }
            }

            // Look for the person_id.
            columnIndex = GetColumnIndexForPersonId(contactRef, cursor);
            if (columnIndex != -1) {
                Int64 contactId;
                cursor->GetInt64(columnIndex, &contactId);
                if (contactId != 0) {
                    AutoPtr<IContactsContractContacts> contacts;
                    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
                    Boolean result;
                    if (contacts->IsEnterpriseContactId(contactId, &result), !result) {
                        info->mContactIdOrZero = contactId;
                        Logger::V(TAG, "==> got info.contactIdOrZero: %d", info->mContactIdOrZero);

                        // cache the lookup key for later use with person_id to create lookup URIs
                        cursor->GetColumnIndex(IContactsContractContactsColumns::LOOKUP_KEY, &columnIndex);
                        if (columnIndex != -1) {
                            cursor->GetString(columnIndex, &info->mLookupKeyOrNull);
                        }
                    }
                }
            }
            else {
                // No valid columnIndex, so we can't look up person_id.
                Logger::V(TAG, "Couldn't find contactId column for %s", TO_CSTR(contactRef));
                // Watch out: this means that anything that depends on
                // person_id will be broken (like contact photo lookups in
                // the in-call UI, for example.)
            }

            // Display photo URI.
            cursor->GetColumnIndex(IContactsContractContactsColumns::PHOTO_URI, &columnIndex);
            String str;
            if ((columnIndex != -1) && (cursor->GetString(columnIndex, &str), !str.IsNull())) {
                AutoPtr<IUriHelper> helper;
                CUriHelper::AcquireSingleton((IUriHelper**)&helper);
                AutoPtr<IUri> temp;
                helper->Parse(str, (IUri**)&temp);
                info->mContactDisplayPhotoUri = temp;
            }
            else {
                info->mContactDisplayPhotoUri = NULL;
            }

            // look for the custom ringtone, create from the string stored
            // in the database.
            cursor->GetColumnIndex(IContactsContractContactOptionsColumns::CUSTOM_RINGTONE, &columnIndex);
            if ((columnIndex != -1) && (cursor->GetString(columnIndex, &str), !str.IsNull())) {
                AutoPtr<IUriHelper> helper;
                CUriHelper::AcquireSingleton((IUriHelper**)&helper);
                AutoPtr<IUri> temp;
                helper->Parse(str, (IUri**)&temp);
                info->mContactRingtoneUri = temp;
            }
            else {
                info->mContactRingtoneUri = NULL;
            }

            // look for the send to voicemail flag, set it to true only
            // under certain circumstances.
            cursor->GetColumnIndex(IContactsContractContactOptionsColumns::SEND_TO_VOICEMAIL, &columnIndex);
            Int32 value;
            info->mShouldSendToVoicemail = (columnIndex != -1) &&
                    ((cursor->GetInt32(columnIndex, &value), value == 1));
            info->mContactExists = TRUE;
        }
        ICloseable::Probe(cursor)->Close();
    }

    info->mNeedUpdate = FALSE;
    info->mName = Normalize(info->mName);
    info->mContactRefUri = contactRef;

    return info;
}

AutoPtr<CallerInfo> CallerInfo::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef)
{
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> cursor;
    cr->Query(contactRef, NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    return GetCallerInfo(context, contactRef, cursor);
}

AutoPtr<CallerInfo> CallerInfo::DoSecondaryLookupIfNecessary(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ CallerInfo* previousResult)
{
    if (!previousResult->mContactExists
            && PhoneNumberHelper::IsUriNumber(number)) {
        String username = PhoneNumberHelper::GetUsernameFromUriNumber(number);
        AutoPtr<IPhoneNumberUtils> utils;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
        Boolean result;
        if (utils->IsGlobalPhoneNumber(username, &result), result) {
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            String encode;
            helper->Encode(username, &encode);
            AutoPtr<IContactsContractPhoneLookup> phoneLookup;
            CContactsContractPhoneLookup::AcquireSingleton((IContactsContractPhoneLookup**)&phoneLookup);
            AutoPtr<IUri> enterpriseUri;
            phoneLookup->GetENTERPRISE_CONTENT_FILTER_URI((IUri**)&enterpriseUri);
            AutoPtr<IUri> uri;
            helper->WithAppendedPath(enterpriseUri, encode, (IUri**)&uri);
            previousResult = GetCallerInfo(context, uri);
        }
    }
    return previousResult;
}

Boolean CallerInfo::IsEmergencyNumber()
{
    return mIsEmergency;
}

Boolean CallerInfo::IsVoiceMailNumber()
{
    return mIsVoiceMail;
}

AutoPtr<CallerInfo> CallerInfo::MarkAsEmergency(
    /* [in] */ IContext* context,
    /* [in] */ const String& number)
{
    context->GetString(Elastos::Droid::Dialer::R::string::emergency_call_dialog_number_for_display, &mPhoneNumber);
    mPhoneNumber += String(" ");
    mPhoneNumber += number;
    mPhotoResource = Elastos::Droid::Dialer::R::drawable::img_phone;
    mIsEmergency = TRUE;
    return this;
}

AutoPtr<CallerInfo> CallerInfo::MarkAsVoiceMail(
    /* [in] */ IContext* context)
{
    mIsVoiceMail = TRUE;

    // try {
    // For voicemail calls, we display the voice mail tag
    // instead of the real phone number in the "number"
    // field.
    ECode ec = TelephonyManagerUtils::GetVoiceMailAlphaTag(context, &mPhoneNumber);
    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        Logger::E(TAG, "Cannot access VoiceMail.");
    }
    // } catch (SecurityException se) {
    //     // Should never happen: if this process does not have
    //     // permission to retrieve VM tag, it should not have
    //     // permission to retrieve VM number and would not call
    //     // this method.
    //     // Leave phoneNumber untouched.
    //     Log.e(TAG, "Cannot access VoiceMail.", se);
    // }
    // TODO: There is no voicemail picture?
    // FIXME: FIND ANOTHER ICON
    // photoResource = android.R.drawable.badge_voicemail;
    return this;
}

String CallerInfo::Normalize(
    /* [in] */ const String& s)
{
    if (s.IsNull() || s.GetLength() > 0) {
        return s;
    }
    else {
        return String(NULL);
    }
}

Int32 CallerInfo::GetColumnIndexForPersonId(
    /* [in] */ IUri* contactRef,
    /* [in] */ ICursor* cursor)
{
    // TODO: This is pretty ugly now, see bug 2269240 for
    // more details. The column to use depends upon the type of URL:
    // - content://com.android.contacts/data/phones ==> use the "contact_id" column
    // - content://com.android.contacts/phone_lookup ==> use the "_ID" column
    // - content://com.android.contacts/data ==> use the "contact_id" column
    // If it's none of the above, we leave columnIndex=-1 which means
    // that the person_id field will be left unset.
    //
    // The logic here *used* to be based on the mime type of contactRef
    // (for example Phone.CONTENT_ITEM_TYPE would tell us to use the
    // RawContacts.CONTACT_ID column).  But looking up the mime type requires
    // a call to context.getContentResolver().getType(contactRef), which
    // isn't safe to do from the UI thread since it can cause an ANR if
    // the contacts provider is slow or blocked (like during a sync.)
    //
    // So instead, figure out the column to use for person_id by just
    // looking at the URI itself.

    Logger::V(TAG, "- getColumnIndexForPersonId: contactRef URI = '%s'...", TO_CSTR(contactRef));
    // Warning: Do not enable the following logging (due to ANR risk.)
    // if (VDBG) RLogger::V(TAG, "- MIME type: "
    //                 + context.getContentResolver().getType(contactRef));

    String url;
    IObject::Probe(contactRef)->ToString(&url);
    String columnName;
    if (url.StartWith("content://com.android.contacts/data/phones")) {
        // Direct lookup in the Phone table.
        // MIME type: Phone.CONTENT_ITEM_TYPE (= "vnd.android.cursor.item/phone_v2")
        Logger::V(TAG, "'data/phones' URI; using RawContacts.CONTACT_ID");
        columnName = IContactsContractRawContactsColumns::CONTACT_ID;
    }
    else if (url.StartWith("content://com.android.contacts/data")) {
        // Direct lookup in the Data table.
        // MIME type: Data.CONTENT_TYPE (= "vnd.android.cursor.dir/data")
        Logger::V(TAG, "'data' URI; using Data.CONTACT_ID");
        // (Note Data.CONTACT_ID and RawContacts.CONTACT_ID are equivalent.)
        columnName = IContactsContractRawContactsColumns::CONTACT_ID;
    }
    else if (url.StartWith("content://com.android.contacts/phone_lookup")) {
        // Lookup in the PhoneLookup table, which provides "fuzzy matching"
        // for phone numbers.
        // MIME type: PhoneLookup.CONTENT_TYPE (= "vnd.android.cursor.dir/phone_lookup")
        Logger::V(TAG, "'phone_lookup' URI; using PhoneLookup._ID");
        columnName = IBaseColumns::ID;
    }
    else {
        Logger::V(TAG, "Unexpected prefix for contactRef '%s'", url.string());
    }
    Int32 columnIndex = -1;
    if (!columnName.IsNull()) {
        cursor->GetColumnIndex(columnName, &columnIndex);
    }
    Logger::V(TAG, "==> Using column '%s' (columnIndex = %d) for person_id lookup...", columnName.string(), columnIndex);
    return columnIndex;
}

void CallerInfo::UpdateGeoDescription(
    /* [in] */ IContext* context,
    /* [in] */ const String& fallbackNumber)
{
    String number = TextUtils::IsEmpty(mPhoneNumber) ? fallbackNumber : mPhoneNumber;
    mGeoDescription = GetGeoDescription(context, number);
}

String CallerInfo::GetGeoDescription(
    /* [in] */ IContext* context,
    /* [in] */ const String& number)
{
    Logger::V(TAG, "getGeoDescription('%s')...", number.string());

    if (TextUtils::IsEmpty(number)) {
        return String(NULL);
    }
assert(0);
    // PhoneNumberUtil util = PhoneNumberUtil.getInstance();
    // PhoneNumberOfflineGeocoder geocoder = PhoneNumberOfflineGeocoder.getInstance();

    // Locale locale = context.getResources().getConfiguration().locale;
    // String countryIso = TelephonyManagerUtils.getCurrentCountryIso(context, locale);
    // PhoneNumber pn = null;
    // try {
    //     Logger::V(TAG, "parsing '" + number
    //             + "' for countryIso '" + countryIso + "'...");
    //     pn = util.parse(number, countryIso);
    //     Logger::V(TAG, "- parsed number: " + pn);
    // } catch (NumberParseException e) {
    //     Logger::V(TAG, "getGeoDescription: NumberParseException for incoming number '" +
    //             number + "'");
    // }

    // if (pn != null) {
    //     String description = geocoder.getDescriptionForNumber(pn, locale);
    //     Logger::V(TAG, "- got description: '" + description + "'");
    //     return description;
    // }

    return String(NULL);
}

ECode CallerInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    // Warning: never check in this file with VERBOSE_DEBUG = true
    // because that will result in PII in the system log.
    Boolean VERBOSE_DEBUG = FALSE;

    if (VERBOSE_DEBUG) {
        AutoPtr<StringBuilder> sb = new StringBuilder(384);
        sb->Append(" { ");
        sb->Append("\nname: " );
        sb->Append(mName);
        sb->Append("\nphoneNumber: ");
        sb->Append(mPhoneNumber);
        sb->Append("\nnormalizedNumber: ");
        sb->Append(mNormalizedNumber);
        sb->Append("\forwardingNumber: ");
        sb->Append(mForwardingNumber);
        sb->Append("\ngeoDescription: ");
        sb->Append(mGeoDescription);
        sb->Append("\ncnapName: ");
        sb->Append(mCnapName);
        sb->Append("\nnumberPresentation: ");
        sb->Append(mNumberPresentation);
        sb->Append("\nnamePresentation: ");
        sb->Append(mNamePresentation);
        sb->Append("\ncontactExists: ");
        sb->Append(mContactExists);
        sb->Append("\nphoneLabel: ");
        sb->Append(mPhoneLabel);
        sb->Append("\nnumberType: ");
        sb->Append(mNumberType);
        sb->Append("\nnumberLabel: ");
        sb->Append(mNumberLabel);
        sb->Append("\nphotoResource: ");
        sb->Append(mPhotoResource);
        sb->Append("\ncontactIdOrZero: ");
        sb->Append(mContactIdOrZero);
        sb->Append("\nneedUpdate: ");
        sb->Append(mNeedUpdate);
        sb->Append("\ncontactRefUri: ");
        sb->Append(mContactRefUri);
        sb->Append("\ncontactRingtoneUri: ");
        sb->Append(mContactRingtoneUri);
        sb->Append("\ncontactDisplayPhotoUri: ");
        sb->Append(mContactDisplayPhotoUri);
        sb->Append("\nshouldSendToVoicemail: ");
        sb->Append(mShouldSendToVoicemail);
        sb->Append("\ncachedPhoto: ");
        sb->Append(mCachedPhoto);
        sb->Append("\nisCachedPhotoCurrent: ");
        sb->Append(mIsCachedPhotoCurrent);
        sb->Append("\nemergency: ");
        sb->Append(mIsEmergency);
        sb->Append("\nvoicemail ");
        sb->Append(mIsVoiceMail);
        sb->Append(" }");
        *str = sb->ToString();
    }
    else {
        AutoPtr<StringBuilder> sb = new StringBuilder(128);
        sb->Append(" { ");
        sb->Append("name ");
        sb->Append((mName.IsNull()) ? "null" : "non-null");
        sb->Append(", phoneNumber ");
        sb->Append((mPhoneNumber.IsNull()) ? "null" : "non-null");
        sb->Append(" }");
        *str = sb->ToString();
    }
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos