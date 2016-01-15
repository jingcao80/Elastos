#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTCHROMESYNCCOLUMNS_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTCHROMESYNCCOLUMNS_H__

#include "_Elastos_Droid_Provider_CBrowserContractChromeSyncColumns.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractChromeSyncColumns)
    , public Singleton
    , public IBrowserContractChromeSyncColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSERVER_UNIQUE(
        /* [out] */ String* result);

    CARAPI GetCLIENT_UNIQUE(
        /* [out] */ String* result);

public:
    static const String SERVER_UNIQUE;
    static const String CLIENT_UNIQUE;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTCHROMESYNCCOLUMNS_H__
