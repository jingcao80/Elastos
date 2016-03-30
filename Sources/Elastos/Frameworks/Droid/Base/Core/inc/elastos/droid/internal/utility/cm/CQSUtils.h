#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSUTILS_H__

#include "_Elastos_Droid_Internal_Utility_Cm_CQSUtils.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CQSUtils)
    , public Singleton
    , public IQSUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAvailableTiles(
        /* [in] */ IContext* context,
        /* [out] */ IList** result);

    CARAPI GetDefaultTiles(
        /* [in] */ IContext* context,
        /* [out] */ IList** result);

    CARAPI GetDefaultTilesAsString(
        /* [in] */ IContext* context,
        /* [out] */ String* result);

    CARAPI DeviceSupportsLte(
        /* [in] */ IContext* ctx,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsDdsSupported(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsMobileData(
        /* [in] */ IContext* ctx,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsBluetooth(
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsNfc(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsFlashLight(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI DeviceSupportsCompass(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CQSUTILS_H__
