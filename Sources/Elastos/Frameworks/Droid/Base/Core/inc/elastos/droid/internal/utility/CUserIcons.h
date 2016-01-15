#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CFASTMATH_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CFASTMATH_H__

#include "_Elastos_Droid_Internal_Utility_CUserIcons.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * Fast and loose math routines.
 */
CarClass(CUserIcons)
    , public Singleton
    , public IUserIcons
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Converts a given drawable to a bitmap.
     */
    CARAPI ConvertToBitmap(
        /* [in] */ IDrawable* icon,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns a default user icon for the given user.
     *
     * Note that for guest users, you should pass in {@code UserHandle.USER_NULL}.
     * @param userId the user id or {@code UserHandle.USER_NULL} for a non-user specific icon
     * @param light whether we want a light icon (suitable for a dark background)
     */
    CARAPI GetDefaultUserIcon(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean light,
        /* [out] */ IDrawable** drawable);

};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CFASTMATH_H__
