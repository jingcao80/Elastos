#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CM_CSCREENTYPE_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CM_CSCREENTYPE_H__

#include "_Elastos_Droid_Internal_Utility_Cm_CScreenType.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Cm {

CarClass(CScreenType)
    , public Singleton
    , public IScreenType
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsPhone(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI IsHybrid(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI IsTablet(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);
};

} // namespace Cm
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CM_CSCREENTYPE_H__
