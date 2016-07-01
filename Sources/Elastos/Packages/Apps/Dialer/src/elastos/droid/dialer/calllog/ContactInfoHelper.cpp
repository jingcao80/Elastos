
#include "elastos/droid/dialer/calllog/ContactInfoHelper.h"
#include "elastos/droid/dialer/calllog/CContactInfo.h"
#include "elastos/droid/dialer/calllog/CPhoneQuery.h"
#include "elastos/droid/dialerbind/CObjectFactory.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringUtils.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsCommonColumns;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsBaseTypes;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContactNameColumns;
using Elastos::Droid::Provider::IContactsContractDisplayNameSources;
using Elastos::Droid::Provider::IContactsContractPhoneLookup;
using Elastos::Droid::Provider::CContactsContractPhoneLookup;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::IO::ICloseable;
using Org::Json::IJSONObject;
using Org::Json::CJSONObject;
using Elastos::Apps::Dialer::Service::ICachedContactInfo;
using Elastos::Apps::DialerBind::CObjectFactory;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

const AutoPtr<ICachedNumberLookupService> ContactInfoHelper::mCachedNumberLookupService
        = CObjectFactory::NewCachedNumberLookupService();

CAR_INTERFACE_IMPL(ContactInfoHelper, Object, IContactInfoHelper);

ECode ContactInfoHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& currentCountryIso)
{
    mContext = context;
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
    Boolean isUriNumber;
    assert(0 && "TODO");
    // isUriNumber = PhoneNumberHelper::IsUriNumber(number)
    if (isUriNumber) {
        // This "number" is really a SIP address.
        AutoPtr<IContactInfo> sipInfo = QueryContactInfoForSipAddress(number);
        if (sipInfo == NULL || sipInfo.Get() == CContactInfo::EMPTY.Get()) {
            // Check whether the "username" part of the SIP address is
            // actually the phone number of a contact.
            assert(0 && "TODO");
            // String username = PhoneNumberHelper::GetUsernameFromUriNumber(number);
            // if (PhoneNumberUtils::IsGlobalPhoneNumber(username)) {
            //     sipInfo = QueryContactInfoForPhoneNumber(username, countryIso);
            // }
        }
        info = sipInfo;
    }
    else {
        // Look for a contact that has the given phone number.
        AutoPtr<IContactInfo> phoneInfo = QueryContactInfoForPhoneNumber(number, countryIso);

        Boolean equals;
        if (phoneInfo == NULL || IObject::Probe(phoneInfo)->Equals(
                IObject::Probe(CContactInfo::EMPTY.Get()), &equals), equals) {
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
        Boolean equals;
        // If we did not find a matching contact, generate an empty contact info for the number.
        if (IObject::Probe(info)->Equals(IObject::Probe(CContactInfo::EMPTY.Get()), &equals), equals) {
            // Did not find a matching contact.
            AutoPtr<CContactInfo> contactInfo;
            CContactInfo::NewByFriend((CContactInfo**)&contactInfo);
            contactInfo->mNumber = number;
            contactInfo->mFormattedNumber = FormatPhoneNumber(number, String(NULL), countryIso);

            AutoPtr<IPhoneNumberUtils> utils;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
            utils->FormatNumberToE164(number, countryIso, &contactInfo->mNormalizedNumber);
            contactInfo->mLookupUri = CreateTemporaryContactUri(contactInfo->mFormattedNumber);
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
    AutoPtr<IJSONObject> contactRows;
    AutoPtr<IJSONObject> json;
    AutoPtr<IContactsContractContacts> contacts;
    AutoPtr<IUri> uri;
    AutoPtr<IUriBuilder> builder;
    AutoPtr<IUri> result;
    String jsonString;

    CJSONObject::New((IJSONObject**)&obj);
    ECode ec = obj->Put(IContactsContractCommonDataKindsCommonColumns::TYPE,
            IContactsContractCommonDataKindsBaseTypes::TYPE_CUSTOM);
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;
    CJSONObject::New((IJSONObject**)&contactRows);
    ec = contactRows->Put(IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE, obj);
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;

    CJSONObject::New((IJSONObject**)&json);
    ec = json->Put(IContactsContractContactsColumns::DISPLAY_NAME, CoreUtils::Convert(number));
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;
    ec = json->Put(IContactsContractContactNameColumns::DISPLAY_NAME_SOURCE,
            IContactsContractDisplayNameSources::PHONE);
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;
    ec = json->Put(IContactsContractContacts::CONTENT_ITEM_TYPE, contactRows);
    if (ec == (ECode)E_JSON_EXCEPTION) goto exit;
    IObject::Probe(json)->ToString(&jsonString);

    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    contacts->GetCONTENT_LOOKUP_URI((IUri**)&uri);
    uri->BuildUpon((IUriBuilder**)&builder);
    assert(0 && "TODO");
    // builder->AppendPath(IConstants::LOOKUP_URI_ENCODED);
    builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
            StringUtils::ToString(0x7FFFFFFFFFFFFFFFL) /*String.valueOf(Long.MAX_VALUE)*/);
    builder->EncodedFragment(jsonString);

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
    AutoPtr<IPhoneQuery> query;
    CPhoneQuery::AcquireSingleton((IPhoneQuery**)&query);
    AutoPtr<ArrayOf<String> > args;
    query->Get_PROJECTION((ArrayOf<String>**)&args);
    AutoPtr<ICursor> phonesCursor;
    resolver->Query(uri, args,
            String(NULL), NULL, String(NULL), (ICursor**)&phonesCursor);

    if (phonesCursor != NULL) {
        // try {
        Boolean result;
        if (phonesCursor->MoveToFirst(&result), result) {
            AutoPtr<CContactInfo> cInfo;
            AutoPtr<IContactsContractContacts> contatcts;
            Int64 contactId;
            String lookupKey;
            String photoUri;

            FAIL_GOTO(CContactInfo::NewByFriend((CContactInfo**)&cInfo), next);

            FAIL_GOTO(phonesCursor->GetInt64(
                    IPhoneQuery::PERSON_ID, &contactId), next);
            FAIL_GOTO(phonesCursor->GetString(
                    IPhoneQuery::LOOKUP_KEY, &lookupKey), next);
            cInfo->mLookupKey = lookupKey;
            CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contatcts);
            FAIL_GOTO(contatcts->GetLookupUri(
                    contactId, lookupKey, (IUri**)&cInfo->mLookupUri), next);
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

            FAIL_GOTO(phonesCursor->GetString(
                    IPhoneQuery::PHOTO_URI, &photoUri), next);
            assert(0 && "TODO");
            // cInfo->mPhotoUri = UriUtils::ParseUriOrNull(photoUri);
            cInfo->mFormattedNumber = NULL;
            info = IContactInfo::Probe(cInfo);
        }
        else {
            info = CContactInfo::EMPTY;
        }
next:
        ICloseable::Probe(phonesCursor)->Close();
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
            IContactsContractPhoneLookup::QUERY_PARAMETER_SIP_ADDRESS, String("1"));

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
        AutoPtr<IPhoneNumberUtils> utils;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
        String numberE164;
        utils->FormatNumberToE164(number, countryIso, &numberE164);
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
    Boolean equals;
    if (info != NULL && !IObject::Probe(info)->Equals(CContactInfo::EMPTY, &equals), equals) {
        ((CContactInfo*)IObject::Probe(info))->mFormattedNumber = FormatPhoneNumber(
                number, String(NULL), countryIso);
    }
    else if (mCachedNumberLookupService != NULL) {
        AutoPtr<ICachedContactInfo> cacheInfo;
        mCachedNumberLookupService->LookupCachedContactFromNumber(
                mContext, number, (ICachedContactInfo**)&cacheInfo);
        if (cacheInfo != NULL) {
            AutoPtr<IContactInfo> contactInfo;
            cacheInfo->GetContactInfo((IContactInfo**)&contactInfo);
            info = ((CContactInfo*)IObject::Probe(contactInfo))->mIsBadData ? NULL : contactInfo;
        }
        else {
            info = NULL;
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
    assert(0 && "TODO");
    // if (PhoneNumberHelper::IsUriNumber(number)) {
    //     return number;
    // }
    if (TextUtils::IsEmpty(countryIso)) {
        (String)countryIso = mCurrentCountryIso;
    }

    AutoPtr<IPhoneNumberUtils> utils;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
    String result;
    utils->FormatNumber(number, normalizedNumber, countryIso, &result);
    return result;
}

String ContactInfoHelper::GetLookupKeyFromUri(
    /* [in] */ IUri* lookupUri)
{
    // Would be nice to be able to persist the lookup key somehow to avoid having to parse
    // the uri entirely just to retrieve the lookup key, but every uri is already parsed
    // once anyway to check if it is an encoded JSON uri, so this has negligible effect
    // on performance.
    assert(0 && "TODO");
    // if (lookupUri != NULL && !UriUtils::IsEncodedContactUri(lookupUri)) {
    //     AutoPtr<IList> segments;
    //     lookupUri->GetPathSegments((IList**)&segments);
    //     // This returns the third path segment of the uri, where the lookup key is located.
    //     // See {@link android.provider.ContactsContract.Contacts#CONTENT_LOOKUP_URI}.
    //     Int32 size;
    //     segments->GetSize(&size);
    //     if (size < 3) {
    //         return String(NULL);
    //     }
    //     else {
    //         AutoPtr<IUriHelper> helper;
    //         CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    //         AutoPtr<IInterface> value;
    //         segments->Get(2, (IInterface**)&value);
    //         String encode;
    //         helper->Encode(CoreUtils::Unbox(ICharSequence::Probe(value)), &encode);
    //         return encode;
    //     }
    // }
    // else {
    //     return String(NULL);
    // }
    return String(NULL);
}

ECode ContactInfoHelper::IsBusiness(
    /* [in] */ Int32 sourceType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Boolean isBusiness = FALSE;
    *result = mCachedNumberLookupService != NULL
            && (mCachedNumberLookupService->IsBusiness(
            sourceType, &isBusiness), isBusiness);
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
            && (mCachedNumberLookupService->CanReportAsInvalid(
            sourceType, objectId, &canReport), canReport);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
