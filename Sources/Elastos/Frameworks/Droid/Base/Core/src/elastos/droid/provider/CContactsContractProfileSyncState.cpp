#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/CContactsContractProfileSyncState.h"
#include "elastos/droid/provider/ContactsContractProfile.h"
#include "elastos/droid/provider/SyncStateContractHelpers.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Provider::ISyncStateContractHelpers;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CContactsContractProfileSyncState)

CAR_INTERFACE_IMPL_2(CContactsContractProfileSyncState, Singleton
    , IContactsContractProfileSyncState
    , ISyncStateContractColumns)

ECode CContactsContractProfileSyncState::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IUri> auUri;
    FAIL_RETURN(ContactsContractProfile::GetCONTENT_URI((IUri**)&auUri))
    return Uri::WithAppendedPath(auUri, IContactsContractProfileSyncState::CONTENT_DIRECTORY, uri);
}

ECode CContactsContractProfileSyncState::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IUri> uri;
    FAIL_RETURN(GetCONTENT_URI((IUri**)&uri))
    return SyncStateContractHelpers::Get(provider, uri, account, value);
}

/**
 * @see android.provider.SyncStateContract.Helpers#get
 */
ECode CContactsContractProfileSyncState::GetWithUri(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ IPair** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::GetWithUri(provider, uri, account, value);
}

/**
 * @see android.provider.SyncStateContract.Helpers#set
 */
ECode CContactsContractProfileSyncState::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<IUri> uri;
    GetCONTENT_URI((IUri**)&uri);
    return SyncStateContractHelpers::Set(provider, uri, account, data);
}

/**
 * @see android.provider.SyncStateContract.Helpers#newSetOperation
 */
ECode CContactsContractProfileSyncState::NewSetOperation(
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