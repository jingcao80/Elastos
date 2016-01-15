#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractSyncState.h"
#include "elastos/droid/provider/ContactsContract.h"
#include "elastos/droid/provider/SyncStateContractHelpers.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractSyncState)

CAR_INTERFACE_IMPL_2(CContactsContractSyncState, Singleton, IContactsContractSyncState, ISyncStateContractColumns)

ECode CContactsContractSyncState::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    return Uri::WithAppendedPath(ContactsContract::AUTHORITY_URI.Get(), IContactsContractSyncState::CONTENT_DIRECTORY, uri);
}

ECode CContactsContractSyncState::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::Get(provider, uri, account, value);
}

/**
 * @see android.provider.SyncStateContract.Helpers#get
 */
ECode CContactsContractSyncState::GetWithUri(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ IPair** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::GetWithUri(provider, uri, account, value);
}

ECode CContactsContractSyncState::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::Set(provider, uri, account, data);
}

ECode CContactsContractSyncState::NewSetOperation(
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    VALIDATE_NOT_NULL(operation);
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::NewSetOperation(uri, account, data, operation);
}

} //Provider
} //Droid
} //Elastos