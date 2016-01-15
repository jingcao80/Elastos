#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTSETTINGS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTSETTINGS_H__

#include "_Elastos_Droid_Provider_CContactsContractSettings.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractSettings)
    , public Singleton
    , public IContactsContractSettings
    , public IContactsContractSettingsColumns
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

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTSETTINGS_H__
