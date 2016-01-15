
#include "Elastos.Droid.Net.h"
#include "elastos/droid/provider/CBrowserContractSyncState.h"
#include "elastos/droid/provider/BrowserContract.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CBrowserContractSyncState)

CAR_INTERFACE_IMPL(CBrowserContractSyncState, Object, IBrowserContractSyncState)

ECode CBrowserContractSyncState::GetCONTENT_DIRECTORY(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = BrowserContract::SyncState::CONTENT_DIRECTORY;
    return NOERROR;
}

ECode CBrowserContractSyncState::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = BrowserContract::SyncState::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CBrowserContractSyncState::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    return BrowserContract::SyncState::Get(provider, account, value);
}

ECode CBrowserContractSyncState::GetWithUri(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [out] */ IPair** value)
{
    return BrowserContract::SyncState::GetWithUri(provider, account, value);
}

ECode CBrowserContractSyncState::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    return BrowserContract::SyncState::Set(provider, account, data);
}

ECode CBrowserContractSyncState::NewSetOperation(
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    return BrowserContract::SyncState::NewSetOperation(account, data, operation);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
