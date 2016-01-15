#ifndef __ELASTOS_DROID_PROVIDER_CVOICEMAILCONTRACTSTATUS_H__
#define __ELASTOS_DROID_PROVIDER_CVOICEMAILCONTRACTSTATUS_H__

#include "_Elastos_Droid_Provider_CVoicemailContractStatus.h"
#include "elastos/droid/provider/VoicemailContract.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CVoicemailContractStatus)
    , public Singleton
    , public IVoicemailContractStatus
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI BuildSourceUri(
        /* [in] */ const String& packageName,
        /* [out] */ IUri** uri);

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PROVIDER_CVOICEMAILCONTRACTSTATUS_H__