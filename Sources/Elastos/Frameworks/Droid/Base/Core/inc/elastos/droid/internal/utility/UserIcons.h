#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_USERICONS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_USERICONS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class UserIcons
{
public:
    /**
     * Converts a given drawable to a bitmap.
     */
    static CARAPI ConvertToBitmap(
        /* [in] */ IDrawable* icon,
        /* [out] */ IBitmap** bitmap);

    /**
     * Returns a default user icon for the given user.
     *
     * Note that for guest users, you should pass in {@code UserHandle.USER_NULL}.
     * @param userId the user id or {@code UserHandle.USER_NULL} for a non-user specific icon
     * @param light whether we want a light icon (suitable for a dark background)
     */
    static CARAPI GetDefaultUserIcon(
        /* [in] */ Int32 userId,
        /* [in] */ Boolean light,
        /* [out] */ IDrawable** drawable);

private:
    UserIcons();

private:
    static const Int32 USER_ICON_COLORS[8];
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_USERICONS_H__
