#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CAUDIOSTATEHELPER_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CAUDIOSTATEHELPER_H__

#include "_Elastos_Droid_Telecomm_Telecom_CAudioStateHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CAudioStateHelper)
    , public Singleton
    , public IAudioStateHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI AudioRouteToString(
        /* [in] */ Int32 route,
        /* [out] */ String* result);
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CAUDIOSTATEHELPER_H__