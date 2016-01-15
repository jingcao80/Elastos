#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTACCOUNTS_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTACCOUNTS_H__

#include "_Elastos_Droid_Provider_CBrowserContractAccounts.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractAccounts)
    , public Singleton
    , public IBrowserContractAccounts
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTACCOUNTS_H__
