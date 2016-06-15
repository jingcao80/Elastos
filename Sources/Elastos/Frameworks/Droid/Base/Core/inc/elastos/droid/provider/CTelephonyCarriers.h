#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYCARRIERS_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYCARRIERS_H__

#include "_Elastos_Droid_Provider_CTelephonyCarriers.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonyCarriers)
    , public Singleton
    , public ITelephonyCarriers
    , public IBaseColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYCARRIERS_H__
