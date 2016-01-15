
#include "CCallerInfoHelper.h"
#include "CPhoneNumberUtils.h"
#include "CCallerInfo.h"
#include "elastos/droid/net/CUriHelper.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::IICountryDetector;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String CCallerInfoHelper::TAG("CCallerInfoHelper");

ECode CCallerInfoHelper::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);

    if (VDBG) Slogger::V(TAG, "getCallerInfo() based on number...");

    if (number.IsEmpty()) {
        *callerInfo = NULL;
        return NOERROR;
    }

    // Change the callerInfo number ONLY if it is an emergency number
    // or if it is the voicemail number.  If it is either, take a
    // shortcut and skip the query.
    Boolean tempRes;
    AutoPtr<ICallerInfo> tmpCallerInfo;
    if (CPhoneNumberUtils::IsLocalEmergencyNumber(number, context, &tempRes), tempRes) {
        CCallerInfo::New((ICallerInfo**)&tmpCallerInfo);
        return tmpCallerInfo->MarkAsEmergency(context, callerInfo);
    } else if (CPhoneNumberUtils::IsVoiceMailNumber(number, &tempRes), tempRes) {
        CCallerInfo::New((ICallerInfo**)&tmpCallerInfo);
        return tmpCallerInfo->MarkAsVoiceMail(callerInfo);
    }

    AutoPtr<IUri> contactUri, baseUri, plContentFilterUri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    String encodedStr;
    helper->Encode(number, &encodedStr);
    helper->Parse(String("content://com.android.contacts"), (IUri**)&baseUri);
    helper->WithAppendedPath(baseUri, String("phone_lookup"), (IUri**)&plContentFilterUri);
    helper->WithAppendedPath(plContentFilterUri, encodedStr, (IUri**)&contactUri);

    AutoPtr<ICallerInfo> info;
    GetCallerInfo(context, contactUri, (ICallerInfo**)&info);
    DoSecondaryLookupIfNecessary(context, number, info, (ICallerInfo**)&tmpCallerInfo);
    info = tmpCallerInfo;
    // if no query results were returned with a viable number,
    // fill in the original number value we used to query with.
    String phoneNumber;
    info->GetPhoneNumber(&phoneNumber);
    if (phoneNumber.IsEmpty()) {
        info->SetPhoneNumber(number);
    }

    *callerInfo = info;
    REFCOUNT_ADD(*callerInfo);

    return NOERROR;
}

ECode CCallerInfoHelper::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    resolver->Query(contactRef, (ArrayOf<String>*)NULL, String(NULL), (ArrayOf<String>*)NULL, String(NULL), (ICursor**)&cursor);
    return GetCallerInfo(context, contactRef, cursor, callerInfo);
}

ECode CCallerInfoHelper::GetCallerInfo(
    /* [in] */ IContext* context,
    /* [in] */ IUri* contactRef,
    /* [in] */ ICursor* cursor,
    /* [out] */ ICallerInfo** callerInfo)
{
    VALIDATE_NOT_NULL(callerInfo);

    AutoPtr<ICallerInfo> info;
    CCallerInfo::New((ICallerInfo**)&info);
    info->SetPhotoResource(0);
    info->SetPhoneLabel(String(NULL));
    info->SetNumberType(0);
    info->SetNumberLabel(String(NULL));
    info->SetCachedPhoto((IDrawable*)NULL);
    info->SetIsCachedPhotoCurrent(FALSE);
    info->SetContactExists(FALSE);

    if (VDBG) Slogger::V(TAG, "getCallerInfo() based on cursor...");
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
                info->SetName(tempStr);
            }

            // Look for the number
            cursor->GetColumnIndex(String("number")/*PhoneLookup.NUMBER*/, &columnIndex);
            if (columnIndex != -1) {
                cursor->GetString(columnIndex, &tempStr);
                info->SetPhoneNumber(tempStr);
            }

            // Look for the normalized number
            cursor->GetColumnIndex(String("normalized_number")/*PhoneLookup.NORMALIZED_NUMBER*/, &columnIndex);
            if (columnIndex != -1) {
                cursor->GetString(columnIndex, &tempStr);
                info->SetNormalizedNumber(tempStr);
            }

            // Look for the label/type combo
            cursor->GetColumnIndex(String("label")/*PhoneLookup.LABEL*/, &columnIndex);
            if (columnIndex != -1) {
                Int32 typeColumnIndex;
                cursor->GetColumnIndex(String("type")/*PhoneLookup.TYPE*/, &typeColumnIndex);
                if (typeColumnIndex != -1) {
                    Int32 numberType;
                    cursor->GetInt32(typeColumnIndex, &numberType);
                    info->SetNumberType(numberType);
                    cursor->GetString(columnIndex, &tempStr);
                    info->SetNumberLabel(tempStr);
                    /*info.phoneLabel = Phone.getDisplayLabel(context,
                            info.numberType, info.numberLabel)
                            .toString();*/
                }
            }

            // Look for the person_id.
            GetColumnIndexForPersonId(contactRef, cursor, &columnIndex);
            if (columnIndex != -1) {
                Int64 person_id;
                cursor->GetInt64(columnIndex, &person_id);
                info->SetPerson_id(person_id);
                if (VDBG) Slogger::V(TAG, "==> got info.person_id: %ld", person_id);
            } else {
                // No valid columnIndex, so we can't look up person_id.
                String uriStr;
                Slogger::W(TAG, "Couldn't find person_id column for %s", (contactRef->ToString(&uriStr), uriStr.string()));
                // Watch out: this means that anything that depends on
                // person_id will be broken (like contact photo lookups in
                // the in-call UI, for example.)
            }

            // look for the custom ringtone, create from the string stored
            // in the database.
            cursor->GetColumnIndex(String("custom_ringtone")/*PhoneLookup.CUSTOM_RINGTONE*/, &columnIndex);
            if ((columnIndex != -1) && (cursor->GetString(columnIndex, &tempStr), !tempStr.IsNull())) {
                AutoPtr<IUri> contactRingtoneUri;
                AutoPtr<IUriHelper> helper;
                CUriHelper::AcquireSingleton((IUriHelper**)&helper);
                cursor->GetString(columnIndex, &tempStr);
                helper->Parse(tempStr, (IUri**)&contactRingtoneUri);
                info->SetContactRingtoneUri(contactRingtoneUri);
            } else {
                info->SetContactRingtoneUri((IUri*)NULL);
            }

            // look for the send to voicemail flag, set it to true only
            // under certain circumstances.
            cursor->GetColumnIndex(String("send_to_voicemail")/*PhoneLookup.SEND_TO_VOICEMAIL*/, &columnIndex);
            Int32 tempInt32;
            tempRes = (columnIndex != -1) && ((cursor->GetInt32(columnIndex, &tempInt32), tempInt32) == 1);
            info->SetShouldSendToVoicemail(tempRes);
            info->SetContactExists(TRUE);
        }
        cursor->Close();
    }

    info->SetNeedUpdate(FALSE);
    String name;
    info->GetName(&tempStr);
    Normalize(tempStr, &name);
    info->SetName(name);
    info->SetContactRefUri(contactRef);

    *callerInfo = info;
    REFCOUNT_ADD(*callerInfo);
    return NOERROR;
}

ECode CCallerInfoHelper::GetCallerId(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<ICallerInfo> info;
    GetCallerInfo(context, number, (ICallerInfo**)&info);
    String callerID;

    if (info != NULL) {
        String name;
        info->GetName(&name);

        if (!name.IsEmpty()) {
            callerID = name;
        } else {
            callerID = number;
        }
    }

    *res = callerID;
    return NOERROR;
}

ECode CCallerInfoHelper::DoSecondaryLookupIfNecessary(
    /* [in] */ IContext* context,
    /* [in] */ const String& number,
    /* [in] */ ICallerInfo* _previousResult,
    /* [out] */ ICallerInfo** res)
{
    VALIDATE_NOT_NULL(res);
    Boolean contactExists, tempRes;
    AutoPtr<ICallerInfo> previousResult = _previousResult;
    previousResult->GetContactExists(&contactExists);
    if (!contactExists && (CPhoneNumberUtils::IsUriNumber(number, &tempRes), tempRes)) {
        String username;
        CPhoneNumberUtils::GetUsernameFromUriNumber(number, &username);
        if (CPhoneNumberUtils::IsGlobalPhoneNumber(username, &tempRes), tempRes) {
            AutoPtr<IUri> contactUri, baseUri;
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            String encodedStr;
            helper->Encode(username, &encodedStr);
            helper->Parse(String("content://com.android.contacts"), (IUri**)&baseUri);
            helper->WithAppendedPath(baseUri, encodedStr, (IUri**)&contactUri);
            previousResult = NULL;
            GetCallerInfo(context, contactUri, (ICallerInfo**)&previousResult);
        }
    }
    *res = previousResult;
    return NOERROR;
}

ECode CCallerInfoHelper::GettCurrentCountryIso(
    /* [in] */ IContext* context,
    /* [in] */ ILocale* locale,
    /* [out] */ String* res)
{
    String countryIso;
    AutoPtr<IICountryDetector> detector;
    context->GetSystemService(IContext::COUNTRY_DETECTOR, (IInterface**)&detector);
    if (detector != NULL) {
        AutoPtr<ICountry> country;
        detector->DetectCountry((ICountry**)&country);
        country->GetCountryIso(&countryIso);
    } else {
        locale->GetCountry(&countryIso);
        Slogger::W(TAG, "No CountryDetector; falling back to countryIso based on locale: %s", countryIso.string());
    }

    *res = countryIso;
    return NOERROR;
}

ECode CCallerInfoHelper::GetColumnIndexForPersonId(
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
    contactRef->ToString(&url);
    if (VDBG) Slogger::V(TAG, "- getColumnIndexForPersonId: contactRef URI = '%s '...", url.string());
    // Warning: Do not enable the following logging (due to ANR risk.)
    // if (VDBG) Slogger::V(TAG, "- MIME type: "
    //                 + context.getContentResolver().getType(contactRef));

    String columnName;
    if (url.StartWith("content://com.android.contacts/data/phones")) {
        // Direct lookup in the Phone table.
        // MIME type: Phone.CONTENT_ITEM_TYPE (= "vnd.android.cursor.item/phone_v2")
        if (VDBG) Slogger::V(TAG, "'data/phones' URI; using RawContacts.CONTACT_ID");
        columnName = String("contact_id")/*RawContacts::CONTACT_ID*/;
    } else if (url.StartWith("content://com.android.contacts/data")) {
        // Direct lookup in the Data table.
        // MIME type: Data.CONTENT_TYPE (= "vnd.android.cursor.dir/data")
        if (VDBG) Slogger::V(TAG, "'data' URI; using Data.CONTACT_ID");
        // (Note Data.CONTACT_ID and RawContacts.CONTACT_ID are equivalent.)
        columnName = String("contact_id")/*DATA::CONTACT_ID*/;
    } else if (url.StartWith("content://com.android.contacts/phone_lookup")) {
        // Lookup in the PhoneLookup table, which provides "fuzzy matching"
        // for phone numbers.
        // MIME type: PhoneLookup.CONTENT_TYPE (= "vnd.android.cursor.dir/phone_lookup")
        if (VDBG) Slogger::V(TAG, "'phone_lookup' URI; using PhoneLookup._ID");
        columnName = String("_id")/*PhoneLookup._ID*/;
    } else {
        Slogger::W(TAG, "Unexpected prefix for contactRef ' %s '", url.string());
    }
    Int32 columnIndex;
    columnIndex = (!columnName.IsNull()) ? (cursor->GetColumnIndex(columnName, &columnIndex), columnIndex) : -1;
    if (VDBG) Slogger::V(TAG, "==> Using column '%s' (columnIndex = %d) for person_id lookup...", columnName.string(), columnIndex);
    *res = columnIndex;
    return NOERROR;
}

ECode CCallerInfoHelper::Normalize(
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

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony
