#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASUBSCRIPTIONSOURCEMANAGERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASUBSCRIPTIONSOURCEMANAGERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CCdmaSubscriptionSourceManagerHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaSubscriptionSourceManagerHelper)
    , public Singleton
    , public ICdmaSubscriptionSourceManagerHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [in] */ ICommandsInterface* ci,
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [out] */ ICdmaSubscriptionSourceManager** result);

    CARAPI GetDefault(
        /* [in] */ IContext* context,
        /* [out] */ Int32* result);

};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASUBSCRIPTIONSOURCEMANAGERHELPER_H__
