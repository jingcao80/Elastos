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

#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/CContactsContractContacts.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractContacts)

CAR_INTERFACE_IMPL_6(CContactsContractContacts, Singleton
    , IContactsContractContacts
    , IBaseColumns
    , IContactsContractContactsColumns
    , IContactsContractContactOptionsColumns
    , IContactsContractContactNameColumns
    , IContactsContractContactStatusColumns)

ECode CContactsContractContacts::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetCONTENT_LOOKUP_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CONTENT_LOOKUP_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetCONTENT_VCARD_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CONTENT_VCARD_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetCONTENT_MULTI_VCARD_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CONTENT_MULTI_VCARD_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetCONTENT_FILTER_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CONTENT_FILTER_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetCONTENT_STREQUENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CONTENT_STREQUENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetCONTENT_FREQUENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CONTENT_FREQUENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetCONTENT_STREQUENT_FILTER_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CONTENT_STREQUENT_FILTER_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetCONTENT_GROUP_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CONTENT_GROUP_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetCORP_CONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = ContactsContractContacts::CORP_CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CContactsContractContacts::GetLookupUri(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* contactUri,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return ContactsContractContacts::GetLookupUri(resolver, contactUri, uri);
}

ECode CContactsContractContacts::GetLookupUri(
    /* [in] */ Int64 contactId,
    /* [in] */ const String& lookupKey,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return ContactsContractContacts::GetLookupUri(contactId, lookupKey, uri);
}

ECode CContactsContractContacts::LookupContact(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* lookupUri,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    return ContactsContractContacts::LookupContact(resolver, lookupUri, uri);
}

ECode CContactsContractContacts::MarkAsContacted(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int64 contactId)
{
    return ContactsContractContacts::MarkAsContacted(resolver, contactId);
}

ECode CContactsContractContacts::OpenContactPhotoInputStream(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* contactUri,
    /* [in] */ Boolean preferHighres,
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);

    return ContactsContractContacts::OpenContactPhotoInputStream(cr, contactUri, preferHighres, stream);
}

ECode CContactsContractContacts::OpenContactPhotoInputStream(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IUri* contactUri,
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);

    return ContactsContractContacts::OpenContactPhotoInputStream(cr, contactUri, stream);
}

ECode CContactsContractContacts::IsEnterpriseContactId(
    /* [in] */ Int64 contactId,
    /* [out] */ Boolean* result)
{
    return ContactsContractContacts::IsEnterpriseContactId(contactId, result);
}

} //Provider
} //Droid
} //Elastos