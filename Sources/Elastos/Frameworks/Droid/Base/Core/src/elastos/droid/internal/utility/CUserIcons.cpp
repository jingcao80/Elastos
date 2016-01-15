
#include "elastos/droid/internal/utility/CUserIcons.h"
#include "elastos/droid/internal/utility/UserIcons.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CUserIcons, Singleton, IUserIcons)

CAR_SINGLETON_IMPL(CUserIcons)

ECode CUserIcons::ConvertToBitmap(
    /* [in] */ IDrawable* icon,
    /* [out] */ IBitmap** bitmap)
{
    return ConvertToBitmap(icon, bitmap);
}

ECode CUserIcons::GetDefaultUserIcon(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean light,
    /* [out] */ IDrawable** drawable)
{
    return UserIcons::GetDefaultUserIcon(userId, light, drawable);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
