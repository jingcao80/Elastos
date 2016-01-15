
#ifndef __ELASTOS_DROID_APP_TRUST_CTRUSTMANAGER_TRUST_LISTENER_H__
#define __ELASTOS_DROID_APP_TRUST_CTRUSTMANAGER_TRUST_LISTENER_H__

#include "_Elastos_Droid_App_Trust_CTrustManagerTrustListener.h"
#include "elastos/droid/app/trust/TrustManager.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Trust {

CarClass(CTrustManagerTrustListener)
    , public TrustManager::TrustManagerTrustListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Trust
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_TRUST_CTRUSTMANAGER_TRUST_LISTENER_H__