#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CFASTMATH_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CFASTMATH_H__

#include "_Elastos_Droid_Internal_Utility_CNotificationColorUtilHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * Fast and loose math routines.
 */
CarClass(CNotificationColorUtilHelper)
    , public Singleton
    , public INotificationColorUtilHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ INotificationColorUtil** instance);
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CFASTMATH_H__
