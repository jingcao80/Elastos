#include "elastos/droid/internal/telephony/CCallerInfo.h"
// TODO: Need CCallerInfoAsyncQuery
// #include "elastos/droid/internal/telephony/CCallerInfoAsyncQuery.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractCommonDataKindsPhone.h"
#include "elastos/droid/provider/ContactsContractContacts.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::ICountryDetector;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::ContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String CCallerInfo::TAG("CCallerInfo");
const Boolean CCallerInfo::VDBG = FALSE;

CAR_INTERFACE_IMPL(CCallerInfo, Object, ICallerInfo)

CAR_OBJECT_IMPL(CCallerInfo)

CCallerInfo::CCallerInfo()
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
{
}

CCallerInfo::~CCallerInfo()
{
}

ECode CCallerInfo::constructor()
{
    return NOERROR;
}

ECode CCallerInfo::IsEmergencyNumber(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsEmergency;
    return NOERROR;
}

ECode CCallerInfo::IsVoiceMailNumber(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsVoiceMail;
    return NOERROR;
}

ECode CCallerInfo::MarkAsEmergency(
    /* [in] */ IContext* context,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);
    context->GetString(R::string::emergency_call_dialog_number_for_display, &mPhoneNumber);
    mPhotoResource = R::drawable::picture_emergency;
    mIsEmergency = TRUE;
    *callerInfo = this;
    REFCOUNT_ADD(*callerInfo);
    return NOERROR;
}

ECode CCallerInfo::MarkAsVoiceMail(
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);
    Int64 subId;
    SubscriptionManager::GetDefaultSubId(&subId);
    return MarkAsVoiceMail(subId, callerInfo);
}

ECode CCallerInfo::MarkAsVoiceMail(
    /* [in] */ Int64 subId,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);
    mIsVoiceMail = TRUE;

    //try {
    String voiceMailLabel;
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    if(FAILED(tm->GetVoiceMailAlphaTag(subId, &voiceMailLabel))) {
        Logger::E(TAG, "Cannot access VoiceMail.");
    }

    mPhoneNumber = voiceMailLabel;
    /*} catch (SecurityException se) {
        // Should never happen: if this process does not have
        // permission to retrieve VM tag, it should not have
        // permission to retrieve VM number and would not call
        // this method.
        // Leave phoneNumber untouched.
        Log.e(TAG, "Cannot access VoiceMail.", se);
    }*/
    // TODO: There is no voicemail picture?
    // FIXME: FIND ANOTHER ICON
    // photoResource = android.R.drawable.badge_voicemail;
    *callerInfo = this;
    REFCOUNT_ADD(*callerInfo);
    return NOERROR;
}

ECode CCallerInfo::UpdateGeoDescription(
    /* [in] */ IContext* context,
    /* [in] */ const String& fallbackNumber)
{
    String number = mPhoneNumber.IsEmpty() ? fallbackNumber : mPhoneNumber;
    mGeoDescription = GetGeoDescription(context, number);
    return NOERROR;
}

ECode CCallerInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // Warning: never check in this file with VERBOSE_DEBUG = true
    // because that will result in PII in the system log.
    const Boolean VERBOSE_DEBUG = FALSE;

    if (VERBOSE_DEBUG) {
        StringBuilder sb;
        sb.Append("super.toString() +  { ");
        sb.Append("\nname: ");
        sb.Append(mName);
        sb.Append("\nphoneNumber: ");
        sb.Append(mPhoneNumber);
        sb.Append("\nnormalizedNumber: ");
        sb.Append(mNormalizedNumber);
        sb.Append("\ngeoDescription: ");
        sb.Append(mGeoDescription);
        sb.Append("\ncnapName: ");
        sb.Append(mCnapName);
        sb.Append("\nnumberPresentation: ");
        sb.Append(mNumberPresentation);
        sb.Append("\nnamePresentation: ");
        sb.Append(mNamePresentation);
        sb.Append("\ncontactExits: ");
        sb.Append(mContactExists);
        sb.Append("\nphoneLabel: ");
        sb.Append(mPhoneLabel);
        sb.Append("\nnumberType: ");
        sb.Append(mNumberType);
        sb.Append("\nnumberLabel: ");
        sb.Append(mNumberLabel);
        sb.Append("\nphotoResource: ");
        sb.Append(mPhotoResource);
        sb.Append("\ncontactIdOrZero: ");
        sb.Append(mContactIdOrZero);
        sb.Append("\nneedUpdate: ");
        sb.Append(mNeedUpdate);
        sb.Append("\ncontactRingtoneUri: ");
        String uri;
        IObject::Probe(mContactRingtoneUri)->ToString(&uri);
        sb.Append(uri);
        sb.Append("\ncontactDisplayPhotoUri: ");
        IObject::Probe(mContactDisplayPhotoUri)->ToString(&uri);
        sb.Append(uri);
        sb.Append("\nshouldSendToVoicemail: ");
        sb.Append(mShouldSendToVoicemail);
        sb.Append("\ncachedPhoto: ");
        sb.Append("mCachedPhoto");
        sb.Append("\nisCachedPhotoCurrent: ");
        sb.Append(mIsCachedPhotoCurrent);
        sb.Append("\nemergency: ");
        sb.Append(mIsEmergency);
        sb.Append("\nvoicemail ");
        sb.Append(mIsVoiceMail);
        sb.Append("\ncontactExists ");
        sb.Append(mContactExists);
        sb.Append(" }");
        *str = sb.ToString();
    }
    else {
        StringBuilder sb;
        sb.Append("super.toString() +  { ");
        sb.Append("name ");
        sb.Append((mName.IsNull()) ? "null" : "non-null");
        sb.Append(", phoneNumber ");
        sb.Append((mPhoneNumber.IsNull()) ? "null" : "non-null");
        sb.Append(" }");
        *str = sb.ToString();
    }

    return NOERROR;
}

ECode CCallerInfo::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef,
    /* [in] */ ICursor* cursor,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);

    AutoPtr<CCallerInfo> info;
    CCallerInfo::NewByFriend((CCallerInfo**)&info);

    if (VDBG) Logger::V(TAG, "getCallerInfo() based on cursor...");
    Boolean tempRes;
    String tempStr;
    if (cursor != NULL) {
        if (cursor->MoveToFirst(&tempRes), tempRes) {
            // TODO: photo_id is always available but not taken
            // care of here. Maybe we should store it in the
            // CallerInfo object as well.

            Int32 columnIndex;

            // Look for the name
            cursor->GetColumnIndex(String("display_name"/*PhoneLookup.DISPLAY_NAME*/), &columnIndex);
            if (columnIndex != -1) {
                cursor->GetString(columnIndex, &tempStr);
                info->mName = tempStr;
            }

            // Look for the number
            cursor->GetColumnIndex(String("number")/*PhoneLookup.NUMBER*/, &columnIndex);
            if (columnIndex != -1) {
                cursor->GetString(columnIndex, &tempStr);
                info->mPhoneNumber = tempStr;
            }

            // Look for the normalized number
            cursor->GetColumnIndex(String("normalized_number")/*PhoneLookup.NORMALIZED_NUMBER*/, &columnIndex);
            if (columnIndex != -1) {
                cursor->GetString(columnIndex, &tempStr);
                info->mNormalizedNumber = tempStr;
            }

            // Look for the label/type combo
            cursor->GetColumnIndex(String("label")/*PhoneLookup.LABEL*/, &columnIndex);
            if (columnIndex != -1) {
                Int32 typeColumnIndex;
                cursor->GetColumnIndex(String("type")/*PhoneLookup.TYPE*/, &typeColumnIndex);
                if (typeColumnIndex != -1) {
                    Int32 numberType;
                    cursor->GetInt32(typeColumnIndex, &numberType);
                    info->mNumberType = numberType;
                    cursor->GetString(columnIndex, &tempStr);
                    info->mNumberLabel = tempStr;

                    AutoPtr<IContactsContractCommonDataKindsPhone> phone;
                    CContactsContractCommonDataKindsPhone::AcquireSingleton((IContactsContractCommonDataKindsPhone**)&phone);
                    AutoPtr<ICharSequence> cs, result;
                    CString::New(info->mNumberLabel, (ICharSequence**)&cs);
                    phone->GetDisplayLabel(context, info->mNumberType, cs, (ICharSequence**)&result);
                    result->ToString(&info->mPhoneLabel);
                }
            }

            // Look for the person_id.
            GetColumnIndexForPersonId(contactRef, cursor, &columnIndex);
            if (columnIndex != -1) {
                Int64 contactId;
                cursor->GetInt64(columnIndex, &contactId);
                Boolean b;
                if (contactId != 0 && (ContactsContractContacts::IsEnterpriseContactId(contactId, &b), !b)) {
                    info->mContactIdOrZero = contactId;
                    if (VDBG) {
                        Logger::V(TAG, "==> got info->mContactIdOrZero: %ld", info->mContactIdOrZero);
                    }
                }
            }
            else {
                // No valid columnIndex, so we can't look up person_id.
                String str;
                IObject::Probe(contactRef)->ToString(&str);
                Logger::W(TAG, "Couldn't find contact_id column for %s", str.string());
                // Watch out: this means that anything that depends on
                // person_id will be broken (like contact photo lookups in
                // the in-call UI, for example.)
            }

            // Contact lookupKey
            cursor->GetColumnIndex(String("lookup")/*PhoneLookup.LOOKUP_KEY*/, &columnIndex);
            if (columnIndex != -1) {
                cursor->GetString(columnIndex, &info->mLookupKey);
            }

            // Display photo URI.
            cursor->GetColumnIndex(String("photo_uri")/*PhoneLookup.PHOTO_URI*/, &columnIndex);
            String str;
            if ((columnIndex != -1) && ((cursor->GetString(columnIndex, &str), str) != NULL)) {
                Uri::Parse((cursor->GetString(columnIndex, &str), str),
                        (IUri**)&info->mContactDisplayPhotoUri);
            }
            else {
                info->mContactDisplayPhotoUri = NULL;
            }

            // look for the send to voicemail flag, set it to true only
            // under certain circumstances.
            cursor->GetColumnIndex(String("send_to_voicemail")/*PhoneLookup.SEND_TO_VOICEMAIL*/, &columnIndex);
            Int32 tempInt32;
            tempRes = (columnIndex != -1) && ((cursor->GetInt32(columnIndex, &tempInt32), tempInt32) == 1);
            info->mShouldSendToVoicemail = tempRes;
            info->mContactExists = TRUE;
        }
        ICloseable::Probe(cursor)->Close();
        cursor = NULL;
    }

    info->mNeedUpdate = FALSE;
    String name;
    Normalize(info->mName, &name);
    info->mName = name;
    info->mContactRefUri = contactRef;

    *callerInfo = info;
    REFCOUNT_ADD(*callerInfo);
    return NOERROR;
}

ECode CCallerInfo::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);
    AutoPtr<IContentResolver> resolver;
// TODO: Need CCallerInfoAsyncQuery
    // CCallerInfoAsyncQuery::GetCurrentProfileContentResolver(context, (IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    resolver->Query(contactRef, (ArrayOf<String>*)NULL, String(NULL), (ArrayOf<String>*)NULL, String(NULL), (ICursor**)&cursor);
    return GetCallerInfo(context, contactRef, cursor, callerInfo);
}

ECode CCallerInfo::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);

    if (VDBG) Logger::D(TAG, "getCallerInfo() based on number...");

    Int64 subId;
    SubscriptionManager::GetDefaultSubId(&subId);
    return GetCallerInfo(context, number, subId, callerInfo);
}

ECode CCallerInfo::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ Int64 subId,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo)

    if (number.IsEmpty()) {
        *callerInfo = NULL;
        return NOERROR;
    }

    // Change the callerInfo number ONLY if it is an emergency number
    // or if it is the voicemail number.  If it is either, take a
    // shortcut and skip the query.
    Boolean b;
    if (PhoneNumberUtils::IsLocalEmergencyNumber(context, number, &b), b) {
        AutoPtr<ICallerInfo> ci;
        CCallerInfo::New((ICallerInfo**)&ci);
        return ci->MarkAsEmergency(context, callerInfo);
    }
    else if (PhoneNumberUtils::IsVoiceMailNumber(subId, number, &b), b) {
        AutoPtr<ICallerInfo> ci;
        CCallerInfo::New((ICallerInfo**)&ci);
        return ci->MarkAsVoiceMail(callerInfo);
    }

    String str;
    Uri::Encode(number, &str);
    AutoPtr<IUri> baseUri;
    Uri::Parse(String("content://phone_lookup_enterprise"), (IUri**)&baseUri);
    AutoPtr<IUri> contactUri;
    Uri::WithAppendedPath(baseUri, str, (IUri**)&contactUri);

    AutoPtr<ICallerInfo> ci;
    GetCallerInfo(context, contactUri, (ICallerInfo**)&ci);
    AutoPtr<ICallerInfo> info;
    DoSecondaryLookupIfNecessary(context, number, ci, (ICallerInfo**)&info);

    // if no query results were returned with a viable number,
    // fill in the original number value we used to query with.
    AutoPtr<CCallerInfo> cInfo = (CCallerInfo*)info.Get();
    if (cInfo->mPhoneNumber.IsEmpty()) {
        cInfo->mPhoneNumber = number;
    }

    *callerInfo = ICallerInfo::Probe(info);
    REFCOUNT_ADD(*callerInfo)
    return NOERROR;
}

ECode CCallerInfo::DoSecondaryLookupIfNecessary(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ ICallerInfo* previousResult,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);

    AutoPtr<ICallerInfo> ci;
    Boolean contactExists, tempRes;
    AutoPtr<CCallerInfo> cInfo = (CCallerInfo*)previousResult;
    if (!cInfo->mContactExists && (PhoneNumberUtils::IsUriNumber(number, &tempRes), tempRes)) {
        String username;
        PhoneNumberUtils::GetUsernameFromUriNumber(number, &username);
        if (PhoneNumberUtils::IsGlobalPhoneNumber(username, &tempRes), tempRes) {
            AutoPtr<IUri> contactUri, baseUri;
            String encodedStr;
            Uri::Encode(username, &encodedStr);
            Uri::Parse(String("content://phone_lookup_enterprise"), (IUri**)&baseUri);
            Uri::WithAppendedPath(baseUri, encodedStr, (IUri**)&contactUri);
            GetCallerInfo(context, contactUri, (ICallerInfo**)&ci);
        }
    }

    *callerInfo = ci;
    REFCOUNT_ADD(*callerInfo)
    return NOERROR;
}

String CCallerInfo::GetCurrentCountryIso(
    /* [in] */ IContext* context)
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    return GetCurrentCountryIso(context, locale);
}

ECode CCallerInfo::Normalize(
    /* [in] */ const String& s,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    if (s.IsNull() || s.GetLength() > 0) {
        *res = s;
    } else {
        *res = String(NULL);
    }
    return NOERROR;
}

ECode CCallerInfo::GetColumnIndexForPersonId(
    /* [in] */ IUri* contactRef,
    /* [in] */ ICursor* cursor,
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);
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
    String url;
    IObject::Probe(contactRef)->ToString(&url);
    if (VDBG) Logger::V(TAG, "- getColumnIndexForPersonId: contactRef URI = '%s '...", url.string());
    // Warning: Do not enable the following logging (due to ANR risk.)
    // if (VDBG) Logger::V(TAG, "- MIME type: "
    //                 + context.getContentResolver().getType(contactRef));

    String columnName;
    if (url.StartWith("content://com.android.contacts/data/phones")) {
        // Direct lookup in the Phone table.
        // MIME type: Phone.CONTENT_ITEM_TYPE (= "vnd.android.cursor.item/phone_v2")
        if (VDBG) Logger::V(TAG, "'data/phones' URI; using RawContacts.CONTACT_ID");
        columnName = String("contact_id")/*RawContacts::CONTACT_ID*/;
    } else if (url.StartWith("content://com.android.contacts/data")) {
        // Direct lookup in the Data table.
        // MIME type: Data.CONTENT_TYPE (= "vnd.android.cursor.dir/data")
        if (VDBG) Logger::V(TAG, "'data' URI; using Data.CONTACT_ID");
        // (Note Data.CONTACT_ID and RawContacts.CONTACT_ID are equivalent.)
        columnName = String("contact_id")/*DATA::CONTACT_ID*/;
    } else if (url.StartWith("content://com.android.contacts/phone_lookup")) {
        // Lookup in the PhoneLookup table, which provides "fuzzy matching"
        // for phone numbers.
        // MIME type: PhoneLookup.CONTENT_TYPE (= "vnd.android.cursor.dir/phone_lookup")
        if (VDBG) Logger::V(TAG, "'phone_lookup' URI; using PhoneLookup._ID");
        columnName = String("_id")/*PhoneLookup._ID*/;
    } else {
        Logger::W(TAG, "Unexpected prefix for contactRef ' %s '", url.string());
    }
    Int32 columnIndex;
    columnIndex = (!columnName.IsNull()) ? (cursor->GetColumnIndex(columnName, &columnIndex), columnIndex) : -1;
    if (VDBG) Logger::V(TAG, "==> Using column '%s' (columnIndex = %d) for person_id lookup...", columnName.string(), columnIndex);
    *res = columnIndex;
    return NOERROR;
}

String CCallerInfo::GetGeoDescription(
    /* [in] */ IContext* context,
    /* [in] */ const String& number)
{
    if (VDBG) Logger::V(TAG, "getGeoDescription(' %s ')...", number.string());

    if (number.IsEmpty()) {
        return String(NULL);
    }

// TODO: Need i18n.phonenumbers.PhoneNumberUtil
    // PhoneNumberUtil util = PhoneNumberUtil.getInstance();
    // PhoneNumberOfflineGeocoder geocoder = PhoneNumberOfflineGeocoder.getInstance();

    // Locale locale = context.getResources().getConfiguration().locale;
    // String countryIso;
    // GetCurrentCountryIso(context, locale, &countryIso);
// TODO: Need i18n.phonenumbers.Phonenumber.PhoneNumber
    // PhoneNumber pn = null;
    // try {
    //     if (VDBG) Rlog.v(TAG, "parsing '" + number
    //                     + "' for countryIso '" + countryIso + "'...");
    //     pn = util.parse(number, countryIso);
    //     if (VDBG) Rlog.v(TAG, "- parsed number: " + pn);
    // } catch (NumberParseException e) {
    //     Rlog.w(TAG, "getGeoDescription: NumberParseException for incoming number '" + number + "'");
    // }

    // if (pn != null) {
    //     String description = geocoder.getDescriptionForNumber(pn, locale);
    //     if (VDBG) Rlog.v(TAG, "- got description: '" + description + "'");
    //     return description;
    // } else {
    //     return null;
    // }
    assert(0);
    return String(NULL);
}

/**
 * @return The ISO 3166-1 two letters country code of the country the user
 *         is in.
 */
String CCallerInfo::GetCurrentCountryIso(
    /* [in] */ IContext* context,
    /* [in] */ ILocale* locale)
{
    String countryIso;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::COUNTRY_DETECTOR, (IInterface**)&obj);
    AutoPtr<ICountryDetector> detector = ICountryDetector::Probe(obj);

    if (detector != NULL) {
        AutoPtr<ICountry> country;
        detector->DetectCountry((ICountry**)&country);
        if (country != NULL) {
            country->GetCountryIso(&countryIso);
        }
        else {
            Logger::E(TAG, "CountryDetector.detectCountry() returned null.");
        }
    }
    if (countryIso == NULL) {
        locale->GetCountry(&countryIso);
        Logger::W(TAG, "No CountryDetector; falling back to countryIso based on locale: %s"
                , countryIso.string());
    }
    return countryIso;
}

ECode CCallerInfo::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CCallerInfo::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CCallerInfo::SetPhoneNumber(
    /* [in] */ const String& phoneNumber)
{
    mPhoneNumber = phoneNumber;
    return NOERROR;
}

ECode CCallerInfo::GetPhoneNumber(
    /* [out] */ String* phoneNumber)
{
    VALIDATE_NOT_NULL(phoneNumber);
    *phoneNumber = mPhoneNumber;
    return NOERROR;
}

ECode CCallerInfo::SetNormalizedNumber(
    /* [in] */ const String& normalizedNumber)
{
    mNormalizedNumber = normalizedNumber;
    return NOERROR;
}

ECode CCallerInfo::GetNormalizedNumber(
    /* [out] */ String* normalizedNumber)
{
    VALIDATE_NOT_NULL(normalizedNumber);
    *normalizedNumber = mNormalizedNumber;
    return NOERROR;
}

ECode CCallerInfo::SetGeoDescription(
    /* [in] */ const String& geoDescription)
{
    mGeoDescription = geoDescription;
    return NOERROR;
}

ECode CCallerInfo::GetGeoDescription(
    /* [out] */ String* geoDescription)
{
    VALIDATE_NOT_NULL(geoDescription);
    *geoDescription = mGeoDescription;
    return NOERROR;
}

ECode CCallerInfo::SetCnapName(
    /* [in] */ const String& cnapName)
{
    mCnapName = cnapName;
    return NOERROR;
}

ECode CCallerInfo::GetCnapName(
    /* [out] */ String* cnapName)
{
    VALIDATE_NOT_NULL(cnapName);
    *cnapName = mCnapName;
    return NOERROR;
}

ECode CCallerInfo::SetNumberPresentation(
    /* [in] */ Int32 numberPresentation)
{
    mNumberPresentation = numberPresentation;
    return NOERROR;
}

ECode CCallerInfo::GetNumberPresentation(
    /* [out] */ Int32* numberPresentation)
{
    VALIDATE_NOT_NULL(numberPresentation);
    *numberPresentation = mNumberPresentation;
    return NOERROR;
}

ECode CCallerInfo::SetNamePresentation(
    /* [in] */ Int32 namePresentation)
{
    mNamePresentation = namePresentation;
    return NOERROR;
}

ECode CCallerInfo::GetNamePresentation(
    /* [out] */ Int32* namePresentation)
{
    VALIDATE_NOT_NULL(namePresentation);
    *namePresentation = mNamePresentation;
    return NOERROR;
}

ECode CCallerInfo::GetContactExists(
    /* [out] */ Boolean* contactExists)
{
    VALIDATE_NOT_NULL(contactExists);
    *contactExists = mContactExists;
    return NOERROR;
}

ECode CCallerInfo::SetContactExists(
    /* [in] */ Boolean contactExists)
{
    mContactExists = contactExists;
    return NOERROR;
}

ECode CCallerInfo::SetPhoneLabel(
    /* [in] */ const String& phoneLabel)
{
    mPhoneLabel = phoneLabel;
    return NOERROR;
}

ECode CCallerInfo::GetPhoneLabel(
    /* [out] */ String* phoneLabel)
{
    VALIDATE_NOT_NULL(phoneLabel);
    *phoneLabel = mPhoneLabel;
    return NOERROR;
}

ECode CCallerInfo::SetNumberType(
    /* [in] */ Int32 numberType)
{
    mNumberType = numberType;
    return NOERROR;
}

ECode CCallerInfo::GetNumberType(
    /* [out] */ Int32* numberType)
{
    VALIDATE_NOT_NULL(numberType);
    *numberType = mNumberType;
    return NOERROR;
}

ECode CCallerInfo::SetNumberLabel(
    /* [in] */ const String& numberLabel)
{
    mNumberLabel = numberLabel;
    return NOERROR;
}

ECode CCallerInfo::GetNumberLabel(
    /* [out] */ String* numberLabel)
{
    VALIDATE_NOT_NULL(numberLabel);
    *numberLabel = mNumberLabel;
    return NOERROR;
}

ECode CCallerInfo::SetPhotoResource(
    /* [in] */ Int32 photoResource)
{
    mPhotoResource = photoResource;
    return NOERROR;
}

ECode CCallerInfo::GetPhotoResource(
    /* [out] */ Int32* photoResource)
{
    VALIDATE_NOT_NULL(photoResource);
    *photoResource = mPhotoResource;
    return NOERROR;
}

ECode CCallerInfo::SetContactIdOrZero(
    /* [in] */ Int64 contactIdOrZero)
{
    mContactIdOrZero = contactIdOrZero;
    return NOERROR;
}

ECode CCallerInfo::GetContactIdOrZero(
    /* [out] */ Int64* contactIdOrZero)
{
    VALIDATE_NOT_NULL(contactIdOrZero);
    *contactIdOrZero = mContactIdOrZero;
    return NOERROR;
}

ECode CCallerInfo::GetNeedUpdate(
    /* [out] */ Boolean* needUpdate)
{
    VALIDATE_NOT_NULL(needUpdate);
    *needUpdate = mNeedUpdate;
    return NOERROR;
}

ECode CCallerInfo::SetNeedUpdate(
    /* [in] */ Boolean needUpdate)
{
    mNeedUpdate = needUpdate;
    return NOERROR;
}

ECode CCallerInfo::SetContactRefUri(
    /* [in] */ IUri* contactRefUri)
{
    mContactRefUri = contactRefUri;
    return NOERROR;
}

ECode CCallerInfo::GetContactRefUri(
    /* [out] */ IUri** contactRefUri)
{
    VALIDATE_NOT_NULL(contactRefUri);
    *contactRefUri = mContactRefUri;
    REFCOUNT_ADD(*contactRefUri);
    return NOERROR;
}

ECode CCallerInfo::SetLookupKey(
    /* [in] */ const String& lookupKey)
{
    mLookupKey = lookupKey;
    return NOERROR;
}

ECode CCallerInfo::GetLookupKey(
    /* [out] */ String* lookupKey)
{
    VALIDATE_NOT_NULL(lookupKey);
    *lookupKey = mLookupKey;
    return NOERROR;
}

ECode CCallerInfo::SetContactDisplayPhotoUri(
    /* [in] */ IUri* contactDisplayPhotoUri)
{
    mContactDisplayPhotoUri = contactDisplayPhotoUri;
    return NOERROR;
}

ECode CCallerInfo::GetContactDisplayPhotoUri(
    /* [out] */ IUri** contactDisplayPhotoUri)
{
    VALIDATE_NOT_NULL(contactDisplayPhotoUri);
    *contactDisplayPhotoUri = mContactDisplayPhotoUri;
    REFCOUNT_ADD(*contactDisplayPhotoUri);
    return NOERROR;
}

ECode CCallerInfo::SetContactRingtoneUri(
    /* [in] */ IUri* contactRingtoneUri)
{
    mContactRingtoneUri = contactRingtoneUri;
    return NOERROR;
}

ECode CCallerInfo::GetContactRingtoneUri(
    /* [out] */ IUri** contactRingtoneUri)
{
    VALIDATE_NOT_NULL(contactRingtoneUri);
    *contactRingtoneUri = mContactRingtoneUri;
    REFCOUNT_ADD(*contactRingtoneUri);
    return NOERROR;
}

ECode CCallerInfo::SetShouldSendToVoicemail(
    /* [in] */ Boolean shouldSendToVoicemail)
{
    mShouldSendToVoicemail = shouldSendToVoicemail;
    return NOERROR;
}

ECode CCallerInfo::GetShouldSendToVoicemail(
    /* [out] */ Boolean* shouldSendToVoicemail)
{
    VALIDATE_NOT_NULL(shouldSendToVoicemail);
    *shouldSendToVoicemail = mShouldSendToVoicemail;
    return NOERROR;
}

ECode CCallerInfo::SetCachedPhoto(
    /* [in] */ IDrawable* cachedPhoto)
{
    mCachedPhoto = cachedPhoto;
    return NOERROR;
}

ECode CCallerInfo::GetCachedPhoto(
    /* [out] */ IDrawable** cachedPhoto)
{
    VALIDATE_NOT_NULL(cachedPhoto);
    *cachedPhoto = mCachedPhoto;
    REFCOUNT_ADD(*cachedPhoto);
    return NOERROR;
}

ECode CCallerInfo::SetCachedPhotoIcon(
    /* [in] */ IBitmap* cachedPhotoIcon)
{
    mCachedPhotoIcon = cachedPhotoIcon;
    return NOERROR;
}

ECode CCallerInfo::GetCachedPhotoIcon(
    /* [out] */ IBitmap** cachedPhotoIcon)
{
    VALIDATE_NOT_NULL(cachedPhotoIcon);
    *cachedPhotoIcon = mCachedPhotoIcon;
    REFCOUNT_ADD(*cachedPhotoIcon);
    return NOERROR;
}

ECode CCallerInfo::SetIsCachedPhotoCurrent(
    /* [in] */ Boolean isCachedPhotoCurrent)
{
    mIsCachedPhotoCurrent = isCachedPhotoCurrent;
    return NOERROR;
}

ECode CCallerInfo::GetIsCachedPhotoCurrent(
    /* [out] */ Boolean* isCachedPhotoCurrent)
{
    VALIDATE_NOT_NULL(isCachedPhotoCurrent);
    *isCachedPhotoCurrent = mIsCachedPhotoCurrent;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos