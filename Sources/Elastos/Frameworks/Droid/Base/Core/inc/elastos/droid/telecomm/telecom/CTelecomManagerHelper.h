#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CTELECOMMANAGERHELPER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CTELECOMMANAGERHELPER_H__

#include "_Elastos_Droid_Telecomm_Telecom_CTelecomManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CTelecomManagerHelper)
    , public Singleton
    , public ITelecomManagerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI From(
        /* [in] */ IContext* context,
        /* [out] */ ITelecomManager** result);
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CTELECOMMANAGERHELPER_H__