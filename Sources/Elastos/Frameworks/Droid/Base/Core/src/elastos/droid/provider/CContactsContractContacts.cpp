
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