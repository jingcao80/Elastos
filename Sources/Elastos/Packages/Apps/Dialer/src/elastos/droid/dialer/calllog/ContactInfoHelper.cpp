//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/contacts/common/util/PhoneNumberHelper.h"
#include "elastos/droid/contacts/common/util/UriUtils.h"
#include "elastos/droid/contacts/common/util/Constants.h"
#include "elastos/droid/dialer/calllog/ContactInfoHelper.h"
#include "elastos/droid/dialer/calllog/PhoneQuery.h"
#include "elastos/droid/dialerbind/ObjectFactory.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Contacts::Common::Util::PhoneNumberHelper;
using Elastos::Droid::Contacts::Common::Util::UriUtils;
using Elastos::Droid::Contacts::Common::Util::Constants;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Dialer::Service::ICachedContactInfo;
using Elastos::Droid::DialerBind::ObjectFactory;
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
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::IO::ICloseable;
using Org::Json::IJSONObject;
using Org::Json::CJSONObject;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

const AutoPtr<ICachedNumberLookupService> ContactInfoHelper::sCachedNumberLookupService
        = ObjectFactory::NewCachedNumberLookupService();

ContactInfoHelper::ContactInfoHelper(
    /* [in] */ IContext* context,
    /* [in] */ const String& currentCountryIso)
    : mContext(context)
    , mCurrentCountryIso(currentCountryIso)
{}

AutoPtr<ContactInfo> ContactInfoHelper::LookupNumber(
    /* [in] */ const String& number,
    /* [in] */ const String& countryIso)
{
    AutoPtr<ContactInfo> info;

    // Determine the contact info.
    if (PhoneNumberHelper::IsUriNumber(number)) {
        // This "number" is really a SIP address.
        AutoPtr<ContactInfo> sipInfo = QueryContactInfoForSipAddress(number);
        if (sipInfo == NULL || sipInfo == ContactInfo::EMPTY) {
            // Check whether the "username" part of the SIP address is
            // actually the phone number of a contact.
            String username = PhoneNumberHelper::GetUsernameFromUriNumber(number);
            AutoPtr<IPhoneNumberUtils> utils;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
            Boolean isGlobalPhoneNumber;
            if (utils->IsGlobalPhoneNumber(username, &isGlobalPhoneNumber), isGlobalPhoneNumber) {
                sipInfo = QueryContactInfoForPhoneNumber(username, countryIso);
            }
        }
        info = sipInfo;
    }
    else {
        // Look for a contact that has the given phone number.
        AutoPtr<ContactInfo> phoneInfo = QueryContactInfoForPhoneNumber(number, countryIso);

        if (phoneInfo == NULL || phoneInfo == ContactInfo::EMPTY) {
            // Check whether the phone number has been saved as an "Internet call" number.
            phoneInfo = QueryContactInfoForSipAddress(number);
        }
        info = phoneInfo;
    }

    AutoPtr<ContactInfo> updatedInfo;
    if (info == NULL) {
        // The lookup failed.
        updatedInfo = NULL;
    }
    else {
        // If we did not find a matching contact, generate an empty contact info for the number.
        if (info == ContactInfo::EMPTY) {
            // Did not find a matching contact.
            updatedInfo = new ContactInfo();
            updatedInfo->mNumber = number;
            updatedInfo->mFormattedNumber = FormatPhoneNumber(number, String(NULL), countryIso);
            AutoPtr<IPhoneNumberUtils> utils;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
            utils->FormatNumberToE164(
                    number, countryIso, &updatedInfo->mNormalizedNumber);
            updatedInfo->mLookupUri = CreateTemporaryContactUri(updatedInfo->mFormattedNumber);
        }
        else {
            updatedInfo = info;
        }
    }
    return updatedInfo;
}

AutoPtr<IUri> ContactInfoHelper::CreateTemporaryContactUri(
    /* [in] */ const String& number)
{
    AutoPtr<ICharSequence> numberCS;
    CString::New(number, (ICharSequence**)&numberCS);
    AutoPtr<IJSONObject> obj;
    CJSONObject::New((IJSONObject**)&obj);
    if (FAILED(obj->Put(IContactsContractCommonDataKindsPhone::NUMBER, numberCS))) {
        return NULL;
    }
    if (FAILED(obj->Put(IContactsContractCommonDataKindsCommonColumns::TYPE,
            IContactsContractCommonDataKindsBaseTypes::TYPE_CUSTOM))) {
        return NULL;
    }

    AutoPtr<IJSONObject> contactRows;
    CJSONObject::New((IJSONObject**)&contactRows);
    if (FAILED(contactRows->Put(IContactsContractCommonDataKindsPhone::CONTENT_ITEM_TYPE, obj))) {
        return NULL;
    }

    AutoPtr<IJSONObject> json;
    CJSONObject::New((IJSONObject**)&json);
    if (FAILED(json->Put(IContactsContractContactsColumns::DISPLAY_NAME, numberCS))) {
        return NULL;
    }
    if (FAILED(json->Put(IContactsContractContactNameColumns::DISPLAY_NAME_SOURCE,
            IContactsContractDisplayNameSources::PHONE))) {
        return NULL;
    }
    if (FAILED(json->Put(IContactsContractContacts::CONTENT_ITEM_TYPE, contactRows))) {
        return NULL;
    }
    String jsonString;
    IObject::Probe(json)->ToString(&jsonString);

    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> uri;
    contacts->GetCONTENT_LOOKUP_URI((IUri**)&uri);
    AutoPtr<IUriBuilder> builder;
    uri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendPath(Constants::LOOKUP_URI_ENCODED);
    builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
            StringUtils::ToString(Elastos::Core::Math::INT64_MAX_VALUE));
    builder->EncodedFragment(jsonString);
    AutoPtr<IUri> result;
    builder->Build((IUri**)&result);

    return result;
}

AutoPtr<ContactInfo> ContactInfoHelper::LookupContactFromUri(
    /* [in] */ IUri* uri)
{
    AutoPtr<ContactInfo> info;

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> phonesCursor;
    resolver->Query(uri, PhoneQuery::_PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&phonesCursor);

    if (phonesCursor != NULL) {
        // try {
        Boolean result;
        if (phonesCursor->MoveToFirst(&result), result) {
            info = new ContactInfo();
            Int64 contactId;
            phonesCursor->GetInt64(PhoneQuery::PERSON_ID, &contactId);
            String lookupKey;
            phonesCursor->GetString(PhoneQuery::LOOKUP_KEY, &lookupKey);
            info->mLookupKey = lookupKey;
            AutoPtr<IContactsContractContacts> contatcts;
            CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contatcts);
            contatcts->GetLookupUri(contactId, lookupKey, (IUri**)&info->mLookupUri);
            phonesCursor->GetString(PhoneQuery::NAME, &info->mName);
            phonesCursor->GetInt32(PhoneQuery::PHONE_TYPE, &info->mType);
            phonesCursor->GetString(PhoneQuery::LABEL, &info->mLabel);
            phonesCursor->GetString(PhoneQuery::MATCHED_NUMBER, &info->mNumber);
            phonesCursor->GetString(PhoneQuery::NORMALIZED_NUMBER, &info->mNormalizedNumber);
            phonesCursor->GetInt64(PhoneQuery::PHOTO_ID, &info->mPhotoId);
            String photoUri;
            phonesCursor->GetString(PhoneQuery::PHOTO_URI, &photoUri);
            info->mPhotoUri = UriUtils::ParseUriOrNull(photoUri);
            info->mFormattedNumber = NULL;
        }
        else {
            info = ContactInfo::EMPTY;
        }

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

AutoPtr<ContactInfo> ContactInfoHelper::QueryContactInfoForSipAddress(
    /* [in] */ const String& sipAddress)
{
    AutoPtr<ContactInfo> info;

    // "contactNumber" is a SIP address, so use the PhoneLookup table with the SIP parameter.
    AutoPtr<IContactsContractPhoneLookup> phoneLookup;
    CContactsContractPhoneLookup::AcquireSingleton((IContactsContractPhoneLookup**)&phoneLookup);
    AutoPtr<IUri> uri;
    phoneLookup->GetCONTENT_FILTER_URI((IUri**)&uri);
    AutoPtr<IUriBuilder> uriBuilder;
    uri->BuildUpon((IUriBuilder**)&uriBuilder);
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    String encodeAddress;
    helper->Encode(sipAddress, &encodeAddress);
    uriBuilder->AppendPath(encodeAddress);
    uriBuilder->AppendQueryParameter(IContactsContractPhoneLookup::QUERY_PARAMETER_SIP_ADDRESS, String("1"));
    AutoPtr<IUri> result;
    uriBuilder->Build((IUri**)&result);
    return LookupContactFromUri(result);
}

AutoPtr<ContactInfo> ContactInfoHelper::QueryContactInfoForPhoneNumber(
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
    CContactsContractPhoneLookup::AcquireSingleton((IContactsContractPhoneLookup**)&phoneLookup);
    AutoPtr<IUri> filterUri;
    phoneLookup->GetCONTENT_FILTER_URI((IUri**)&filterUri);
    AutoPtr<IUri> uri;
    helper->WithAppendedPath(filterUri, encodeNumber, (IUri**)&uri);

    AutoPtr<ContactInfo> info = LookupContactFromUri(uri);
    if (info != NULL && info != ContactInfo::EMPTY) {
        info->mFormattedNumber = FormatPhoneNumber(number, String(NULL), countryIso);
    }
    else if (sCachedNumberLookupService != NULL) {
        AutoPtr<ICachedContactInfo> cacheInfo;
        sCachedNumberLookupService->LookupCachedContactFromNumber(mContext, number, (ICachedContactInfo**)&cacheInfo);
        if (cacheInfo != NULL) {
            AutoPtr<IContactInfo> contactInfo;
            cacheInfo->GetContactInfo((IContactInfo**)&contactInfo);
            info = ((ContactInfo*)contactInfo.Get())->mIsBadData ? NULL : (ContactInfo*)contactInfo.Get();
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
    /* [in] */ const String& _countryIso)
{
    String countryIso = _countryIso;
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
            AutoPtr<IInterface> value;
            segments->Get(2, (IInterface**)&value);
            String segment;
            ICharSequence::Probe(value)->ToString(&segment);
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            String encode;
            helper->Encode(segment, &encode);
            return encode;
        }
    }
    return String(NULL);
}

Boolean ContactInfoHelper::IsBusiness(
    /* [in] */ Int32 sourceType)
{
    Boolean isBusiness;
    return sCachedNumberLookupService != NULL
            && (sCachedNumberLookupService->IsBusiness(sourceType, &isBusiness), isBusiness);
}

Boolean ContactInfoHelper::ContactInfoHelper::CanReportAsInvalid(
    /* [in] */ Int32 sourceType,
    /* [in] */ const String& objectId)
{
    Boolean canReport;
    return sCachedNumberLookupService != NULL
            && (sCachedNumberLookupService->CanReportAsInvalid(sourceType, objectId, &canReport), canReport);
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
