#ifndef __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTSETTINGS_H__
#define __ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTSETTINGS_H__

#include "_Elastos_Droid_Provider_CBrowserContractSettings.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CBrowserContractSettings)
    , public Singleton
    , public IBrowserContractSettings
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Returns true if bookmark sync is enabled
     */
    CARAPI IsSyncEnabled(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* res);

    /**
     * Sets the bookmark sync enabled setting.
     */
    CARAPI SetSyncEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDER_CBROWSERCONTRACTSETTINGS_H__
