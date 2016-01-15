#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTSYNCSTATE_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTSYNCSTATE_H__

#include "_Elastos_Droid_Provider_CBrowserContractSyncState.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IPair;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractSyncState)
    , public Singleton
    , public IBrowserContractSyncState
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_DIRECTORY(
        /* [out] */ String* result);

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI Get(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IAccount* account,
        /* [out, callee] */ ArrayOf<Byte>** value);

    CARAPI GetWithUri(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IAccount* account,
        /* [out] */ IPair** value);

    CARAPI Set(
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI NewSetOperation(
        /* [in] */ IAccount* account,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IContentProviderOperation** operation);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTSYNCSTATE_H__
