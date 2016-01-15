#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTPROVIDERSTATUS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTPROVIDERSTATUS_H__

#include "_Elastos_Droid_Provider_CContactsContractProviderStatus.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsContractProviderStatus)
    , public Singleton
    , public IContactsContractProviderStatus
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URI for this table.  Requests to this URI can be
     * performed on the UI thread because they are always unblocking.
     *
     * @hide
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSCONTRACTPROVIDERSTATUS_H__
