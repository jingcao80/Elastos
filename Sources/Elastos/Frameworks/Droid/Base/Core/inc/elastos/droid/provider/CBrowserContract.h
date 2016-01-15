#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACT_H__

#include "_Elastos_Droid_Provider_CBrowserContract.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContract)
    , public Singleton
    , public IBrowserContract
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetAUTHORITY_URI(
        /* [out] */ IUri** uri);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACT_H__
