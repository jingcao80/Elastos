
#include "elastos/apps/dialer/calllog/ContactInfoHelper.h"

using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Org::Json::IJSONObject;
using Org::Json::CJSONObject;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

const AutoPtr<ICachedNumberLookupService> ContactInfoHelper::mCachedNumberLookupService
        = ObjectFactory::NewCachedNumberLookupService();

CAR_INTERFACE_IMPL(ContactInfoHelper, Object, IContactInfoHelper);

ECode ContactInfoHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& currentCountryIso)
    : mContext(context)
{
    mCurrentCountryIso = currentCountryIso;
    return NOERROR;
}

ECode ContactInfoHelper::LookupNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso,
    /* [out] */ IContactInfo** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IContactInfo> info;

    // Determine the contact info.
    if (PhoneNumberHelper::IsUriNumber(number)) {
        // This "number" is really a SIP address.
        AutoPtr<IContactInfo> sipInfo = QueryContactInfoForSipAddress(number);
        if (sipInfo == NULL || sipInfo == IContactInfo::EMPTY) {
            // Check whether the "username" part of the SIP address is
            // actually the phone number of a contact.
            String username = PhoneNumberHelper::GetUsernameFromUriNumber(number);
            if (PhoneNumberUtils::IsGlobalPhoneNumber(username)) {
                sipInfo = QueryContactInfoForPhoneNumber(username, countryIso);
            }
        }
        info = sipInfo;
    }
    else {
        // Look for a contact that has the given phone number.
        AutoPtr<IContactInfo> phoneInfo = QueryContactInfoForPhoneNumber(number, countryIso);

        if (phoneInfo == NULL || IObject::Probe(phoneInfo)->Equals(CContactInfo::EMPTY)) {
            // Check whether the phone number has been saved as an "Internet call" number.
            phoneInfo = QueryContactInfoForSipAddress(number);
        }
        info = phoneInfo;
    }

    AutoPtr<IContactInfo> updatedInfo;
    if (info == NULL) {
        // The lookup failed.
        updatedInfo = NULL;
    }
    else {
        // If we did not find a matching contact, generate an empty contact info for the number.
        if (IObject::Probe(info)->Equals(CContactInfo::EMPTY)) {
            // Did not find a matching contact.
            AutoPtr<CContactInfo> contactInfo;
            CContactInfo::NewByFriend((CContactInfo**)&contactInfo);
            contactInfo->mNumber = number;
            contactInfo->mFormattedNumber = FormatPhoneNumber(number, String(NULL), countryIso);
            contactInfo->mNormalizedNumber = PhoneNumberUtils::FormatNumberToE164(
                    number, countryIso);
            contactInfo->mLookupUri = CreateTemporaryContactUri(updatedInfo.formattedNumber);
            updatedInfo = IContactInfo::Probe(contactInfo);
        }
        else {
            updatedInfo = info;
        }
    }
    *result = updatedInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IUri> ContactInfoHelper::CreateTemporaryContactUri(
    /* [in] */ const String& number)
{
    // try {
    AutoPtr<IJSONObject> obj;
    CJSONObject::New((IJSONObject**)&obj);
    ECode ec = obj->Put(IPhone::TYPE, IPhone::TYPE_CUSTOM);
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;
    AutoPtr<IJSONObject> contactRows;
    CJSONObject::New((IJSONObject**)&contactRows);
    ec = contactRows->Put(IPhone::CONTENT_ITEM_TYPE, obj);
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;

    AutoPtr<IJSONObject> json;
    CJSONObject::New((IJSONObject**)&json);
    ec = json->Put(IContacts::DISPLAY_NAME, number);
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;
    ec = json->Put(IContacts::DISPLAY_NAME_SOURCE, IDisplayNameSources::PHONE);
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;
    ec = json->Put(IContacts::CONTENT_ITEM_TYPE, contactRows);
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;
    String jsonString;
    json->ToString(&jsonString);

    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> uri;
    contacts->GetCONTENT_LOOKUP_URI((IUri**)&uri);
    AutoPtr<IUriBuilder> builder;
    builder->BuildUpon((IUriBuilder**)&builder);
    builder->AppendPath(IConstants::LOOKUP_URI_ENCODED);
    builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
            StringUtils::ToString(0x7FFFFFFFFFFFFFFFL) /*String.valueOf(Long.MAX_VALUE)*/);
    builder->EncodedFragment(jsonString);

    AutoPtr<IUri> result;
    builder->Build((IUri**)&result);

    return result;
    // } catch (JSONException e) {
    //     return null;
    // }
exit:
    return NULL;
}

AutoPtr<IContactInfo> ContactInfoHelper::LookupContactFromUri(
    /* [in] */ IUri* uri)
{
    AutoPtr<IContactInfo> info;

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> phonesCursor;
    resolver->Query(uri, PhoneQuery::Get_PROJECTION(),
            NULL, NULL, NULL, (ICursor**)&phonesCursor);

    if (phonesCursor != NULL) {
        // try {
        Boolean result;
        if (phonesCursor->MoveToFirst(&result), result) {
            AutoPtr<CContactInfo> cInfo;

            FAIL_GOTO(CContactInfo::NewByFriend((CContactInfo**)&cInfo), next);

            Int64 contactId;
            FAIL_GOTO(phonesCursor->GetInt64(
                    IPhoneQuery::PERSON_ID, &contactId), next);
            String lookupKey;
            FAIL_GOTO(phonesCursor->GetString(
                    IPhoneQuery::LOOKUP_KEY, &lookupKey), next);
            CContactInfo->mLookupKey = lookupKey;
            FAIL_GOTO(phonesCursor->GetLookupUri(
                    contactId, lookupKey, &cInfo->mlookupUri), next);
            FAIL_GOTO(phonesCursor->GetString(
                    IPhoneQuery::NAME, &cInfo->mName), next);
            FAIL_GOTO(phonesCursor->GetInt32(
                    IPhoneQuery::PHONE_TYPE, &cInfo->mType), next);
            FAIL_GOTO(phonesCursor->GetString(
                    IPhoneQuery::LABEL, &cInfo->mLabel), next);
            FAIL_GOTO(phonesCursor->GetString(
                    IPhoneQuery::MATCHED_NUMBER, &cInfo->mNumber), next);
            FAIL_GOTO(phonesCursor->GetString(
                    IPhoneQuery::NORMALIZED_NUMBER, &cInfo->mNormalizedNumber), next);
            FAIL_GOTO(phonesCursor->GetInt64(
                    IPhoneQuery::PHOTO_ID, &cInfo->mPhotoId), next);

            String photoUri;
            FAIL_GOTO(phonesCursor->GetString(
                    IPhoneQuery::PHOTO_URI, photoUri), next);
            cInfo->mPhotoUri = UriUtils::ParseUriOrNull(photoUri);
            cInfo->mFormattedNumber = NULL;
            info = IContactInfo::Probe(cInfo);
        }
        else {
            info = ContactInfo::EMPTY;
        }
next:
        phonesCursor->Close();
        // } finally {
        //     phonesCursor.close();
        // }
    }
    else {
        // Failed to fetch the data, ignore this request.
        info = NULL;
    }
    return info;
}

AutoPtr<IContactInfo> ContactInfoHelper::QueryContactInfoForSipAddress(
    /* [in] */ const String& sipAddress)
{
    AutoPtr<IContactInfo> info;

    // "contactNumber" is a SIP address, so use the PhoneLookup table with the SIP parameter.
    AutoPtr<IContactsContractPhoneLookup> phoneLookup;
    CContactsContractPhoneLookup::AcquireSingleton(
            (IContactsContractPhoneLookup**)&phoneLookup);
    AutoPtr<IUri> uri;
    phoneLookup->GetCONTENT_FILTER_URI((IUri**)&uri);
    AutoPtr<IUriBuilder> uriBuilder;
    uri->BuildUpon((IUriBuilder**)&uriBuilder);
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    String encodeAddress;
    helper->Encode(sipAddress, &encodeAddress);
    uriBuilder->AppendPath(encodeAddress);
    uriBuilder->AppendQueryParameter(
            IPhoneLookup::QUERY_PARAMETER_SIP_ADDRESS, String("1"));

    AutoPtr<IUri> obj;
    uriBuilder->Build((IUri**)&obj);
    return LookupContactFromUri(obj);
}

AutoPtr<IContactInfo> ContactInfoHelper::QueryContactInfoForPhoneNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso)
{
    String contactNumber = number;
    if (!TextUtils::IsEmpty(countryIso)) {
        // Normalize the number: this is needed because the PhoneLookup query below does not
        // accept a country code as an input.
        String numberE164 = PhoneNumberUtils.FormatNumberToE164(number, countryIso);
        if (!TextUtils::IsEmpty(numberE164)) {
            // Only use it if the number could be formatted to E164.
            contactNumber = numberE164;
        }
    }

    // The "contactNumber" is a regular phone number, so use the PhoneLookup table.
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    String encodeNumber;
    helper->Encode(contactNumber, &encodeNumber);
    AutoPtr<IContactsContractPhoneLookup> phoneLookup;
    CContactsContractPhoneLookup::AcquireSingleton(
            (IContactsContractPhoneLookup**)&phoneLookup);
    AutoPtr<IUri> filterUri;
    phoneLookup->GetCONTENT_FILTER_URI((IUri**)&filterUri);
    AutoPtr<IUri> uri;
    helper->WithAppendedPath(filterUri, encodeNumber, (IUri**)&uri);

    AutoPtr<IContactInfo> info = LookupContactFromUri(uri);
    if (info != NULL && !IObject::Probe(info).Equals(CContactInfo::EMPTY)) {
        ((CContactInfo*)info)->mFormattedNumber = FormatPhoneNumber(
                number, String(NULL), countryIso);
    }
    else if (mCachedNumberLookupService != NULL) {
        AutoPtr<ICachedContactInfo> cacheInfo;
        mCachedNumberLookupService->LookupCachedContactFromNumber(
                mContext, number, (ICachedContactInfo**)&cacheInfo);
        if (cacheInfo != NULL) {
            AutoPtr<IContactInfo> contactInfo;
            cacheInfo->GetContactInfo(&contactInfo);
            info = ((CContactInfo*)contactInfo)->mIsBadData ? NULL : contactInfo;
        }
        else {
            info = null;
        }
    }
    return info;
}

String ContactInfoHelper::FormatPhoneNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& normalizedNumber,
    /* [in] */ const String& countryIso)
{
    if (TextUtils::IsEmpty(number)) {
        return String("");
    }
    // If "number" is really a SIP address, don't try to do any formatting at all.
    if (PhoneNumberHelper::IsUriNumber(number)) {
        return number;
    }
    if (TextUtils::IsEmpty(countryIso)) {
        countryIso = mCurrentCountryIso;
    }
    return PhoneNumberUtils::FormatNumber(number, normalizedNumber, countryIso);
}

String ContactInfoHelper::GetLookupKeyFromUri(
    /* [in] */ IUri* lookupUri)
{
    // Would be nice to be able to persist the lookup key somehow to avoid having to parse
    // the uri entirely just to retrieve the lookup key, but every uri is already parsed
    // once anyway to check if it is an encoded JSON uri, so this has negligible effect
    // on performance.
    if (lookupUri != NULL && !UriUtils::IsEncodedContactUri(lookupUri)) {
        AutoPtr<IList> segments;
        lookupUri->GetPathSegments((IList**)&segments);
        // This returns the third path segment of the uri, where the lookup key is located.
        // See {@link android.provider.ContactsContract.Contacts#CONTENT_LOOKUP_URI}.
        Int32 size;
        segments->GetSize(&size);
        if (size < 3) {
            return String(NULL);
        }
        else {
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            AutoPtr<IInterface> value;
            segments->Get(2, (IInterface**)&value);
            String encode;
            helper->Encode(CoreUtils::Unbox(value), &encode);
            return encode;
        }
    }
    else {
        return String(NULL);
    }
}

ECode ContactInfoHelper::IsBusiness(
    /* [in] */ Int32 sourceType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isBusiness = FALSE;
    *result = mCachedNumberLookupService != NULL
            && mCachedNumberLookupService->IsBusiness(
            sourceType, &isBusiness), isBusiness;
    return NOERROR;
}

ECode ContactInfoHelper::ContactInfoHelper::CanReportAsInvalid(
    /* [in] */ Int32 sourceType,
    /* [in] */ const String& objectId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean canReport = FALSE;
    *result = mCachedNumberLookupService != NULL
            && mCachedNumberLookupService->CanReportAsInvalid(
            sourceType, objectId, &canReport), canReport;
    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
