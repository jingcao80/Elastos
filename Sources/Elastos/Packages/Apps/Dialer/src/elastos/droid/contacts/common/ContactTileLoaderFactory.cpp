
#include "elastos/droid/contacts/common/ContactTileLoaderFactory.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Content::CCursorLoader;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContactOptionsColumns;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsCommonColumns;
using Elastos::Droid::Provider::IContactsContractDataColumns;
using Elastos::Droid::Provider::IContactsContractRawContactsColumns;
using Elastos::Droid::Provider::IContactsContractContactStatusColumns;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContract;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

const Int32 ContactTileLoaderFactory::CONTACT_ID;
const Int32 ContactTileLoaderFactory::DISPLAY_NAME;
const Int32 ContactTileLoaderFactory::STARRED;
const Int32 ContactTileLoaderFactory::PHOTO_URI;
const Int32 ContactTileLoaderFactory::LOOKUP_KEY;
const Int32 ContactTileLoaderFactory::CONTACT_PRESENCE;
const Int32 ContactTileLoaderFactory::CONTACT_STATUS;
const Int32 ContactTileLoaderFactory::PHONE_NUMBER;
const Int32 ContactTileLoaderFactory::PHONE_NUMBER_TYPE;
const Int32 ContactTileLoaderFactory::PHONE_NUMBER_LABEL;
const Int32 ContactTileLoaderFactory::IS_DEFAULT_NUMBER;
const Int32 ContactTileLoaderFactory::PINNED;
const Int32 ContactTileLoaderFactory::CONTACT_ID_FOR_DATA;

static AutoPtr<ArrayOf<String> > InitColumnsPhoneOnly()
{
    AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(11);
    (*columns)[0] = IBaseColumns::ID;
    (*columns)[1] = IContactsContractContactsColumns::DISPLAY_NAME;
    (*columns)[2] = IContactsContractContactOptionsColumns::STARRED;
    (*columns)[3] = IContactsContractContactsColumns::PHOTO_URI;
    (*columns)[4] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*columns)[5] = IContactsContractCommonDataKindsPhone::NUMBER;
    (*columns)[6] = IContactsContractCommonDataKindsCommonColumns::TYPE;
    (*columns)[7] = IContactsContractCommonDataKindsCommonColumns::LABEL;
    (*columns)[8] = IContactsContractDataColumns::IS_SUPER_PRIMARY;
    (*columns)[9] = IContactsContractContactOptionsColumns::PINNED;
    (*columns)[10] = IContactsContractRawContactsColumns::CONTACT_ID;
}
const AutoPtr<ArrayOf<String> > ContactTileLoaderFactory::COLUMNS_PHONE_ONLY = InitColumnsPhoneOnly();

static AutoPtr<ArrayOf<String> > InitColumns()
{
    AutoPtr<ArrayOf<String> > columns = ArrayOf<String>::Alloc(7);
    (*columns)[0] = IBaseColumns::ID;
    (*columns)[1] = IContactsContractContactsColumns::DISPLAY_NAME;
    (*columns)[2] = IContactsContractContactOptionsColumns::STARRED;
    (*columns)[3] = IContactsContractContactsColumns::PHOTO_URI;
    (*columns)[4] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*columns)[5] = IContactsContractContactStatusColumns::CONTACT_PRESENCE;
    (*columns)[6] = IContactsContractContactStatusColumns::CONTACT_STATUS;
}
const AutoPtr<ArrayOf<String> > ContactTileLoaderFactory::COLUMNS = InitColumns();

const String ContactTileLoaderFactory::STARRED_ORDER
        = IContactsContractContactsColumns::DISPLAY_NAME + " COLLATE NOCASE ASC";

AutoPtr<ICursorLoader> ContactTileLoaderFactory::CreateStrequentLoader(
    /* [in] */ IContext* context)
{
    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> uri;
    Contacts->GetCONTENT_STREQUENT_URI((IUri**)&uri);
    AutoPtr<ICursorLoader> cl;
    CCursorLoader::New(context, uri, COLUMNS, String(NULL), NULL, STARRED_ORDER, (ICursorLoader**)&cl);
    return cl;
}

AutoPtr<ICursorLoader> ContactTileLoaderFactory::CreateStrequentPhoneOnlyLoader(
    /* [in] */ IContext* context)
{
    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> contentStrequentUri;
    contacts->GetCONTENT_STREQUENT_URI((IUri**)&contentStrequentUri);
    AutoPtr<IUriBuilder> builder;
    contentStrequentUri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendQueryParameter(IContactsContract::STREQUENT_PHONE_ONLY, String("true"));
    AutoPtr<IUri> uri;
    builder->Build((IUri**)&uri);
    AutoPtr<ICursorLoader> cl;
    CCursorLoader::New(context, uri, COLUMNS_PHONE_ONLY, String(NULL), NULL, String(NULL), (ICursorLoader**)&cl);
    return cl;
}

AutoPtr<ICursorLoader> ContactTileLoaderFactory::CreateStarredLoader(
    /* [in] */ IContext* context)
{
    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> uri;
    Contacts->GetCONTENT_URI((IUri**)&uri);
    AutoPtr<ArrayOf<String> > attrs = ArrayOf<String>::Alloc(1);
    (*attrs)[0] = String("1");
    AutoPtr<ICursorLoader> cl;
    CCursorLoader::New(context, uri, COLUMNS_PHONE_ONLY,
            IContactsContractContactOptionsColumns::STARRED + "=?", attrs, STARRED_ORDER, (ICursorLoader**)&cl);
    return cl;
}

AutoPtr<ICursorLoader> ContactTileLoaderFactory::CreateFrequentLoader(
    /* [in] */ IContext* context)
{
    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> uri;
    Contacts->GetCONTENT_FREQUENT_URI((IUri**)&uri);
    AutoPtr<ArrayOf<String> > attrs = ArrayOf<String>::Alloc(1);
    (*attrs)[0] = String("0");
    AutoPtr<ICursorLoader> cl;
    CCursorLoader::New(context, uri, COLUMNS,
            IContactsContractContactOptionsColumns::STARRED + "=?", attrs, String(NULL), (ICursorLoader**)&cl);
    return cl;
}

} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
