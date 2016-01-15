#include "elastos/droid/provider/CSyncStateContractHelpers.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CSyncStateContractHelpers)

CAR_INTERFACE_IMPL(CSyncStateContractHelpers, Singleton, ISyncStateContractHelpers)

ECode CSyncStateContractHelpers::Get(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    return SyncStateContractHelpers::Get(provider, uri, account, value);
}

ECode CSyncStateContractHelpers::Set(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data)
{
    return SyncStateContractHelpers::Set(provider, uri, account, data);
}

ECode CSyncStateContractHelpers::Insert(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IUri** retUri)
{
    return SyncStateContractHelpers::Insert(provider, uri, account, data, retUri);
}

ECode CSyncStateContractHelpers::Update(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<Byte>* data)
{
    return SyncStateContractHelpers::Update(provider, uri, data);
}

ECode CSyncStateContractHelpers::NewSetOperation(
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    return SyncStateContractHelpers::NewSetOperation(uri, account, data, operation);
}

ECode CSyncStateContractHelpers::NewUpdateOperation(
    /* [in] */ IUri* uri,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IContentProviderOperation** operation)
{
    return SyncStateContractHelpers::NewUpdateOperation(uri, data, operation);
}

ECode CSyncStateContractHelpers::GetWithUri(
    /* [in] */ IContentProviderClient* provider,
    /* [in] */ IUri* uri,
    /* [in] */ IAccount* account,
    /* [out] */ IPair** value)
{
    return SyncStateContractHelpers::GetWithUri(provider, uri, account, value);
}

} //Provider
} //Droid
} //Elastos