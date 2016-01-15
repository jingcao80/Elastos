#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTIMAGES_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTIMAGES_H__

#include "_Elastos_Droid_Provider_CBrowserContractImages.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractImages)
    , public Singleton
    , public IBrowserContractImages
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

#endif //__ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTIMAGES_H__
