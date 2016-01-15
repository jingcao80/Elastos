#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTIMAGEMAPPINGS_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTIMAGEMAPPINGS_H__

#include "_Elastos_Droid_Provider_CBrowserContractImageMappings.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractImageMappings)
    , public Singleton
    , public IBrowserContractImageMappings
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__CBROWSERCONTRACTIMAGES_H__
