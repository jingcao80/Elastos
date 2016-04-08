
#include "elastos/droid/launcher2/CLauncherApplicationHelper.h"
#include "elastos/droid/launcher2/LauncherApplication.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CLauncherApplicationHelper)

CAR_INTERFACE_IMPL(CLauncherApplicationHelper, Singleton, ILauncherApplicationHelper)

ECode CLauncherApplicationHelper::GetSharedPreferencesKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);

    return LauncherApplication::GetSharedPreferencesKey(key);
}

ECode CLauncherApplicationHelper::IsScreenLarge(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return LauncherApplication::IsScreenLarge(result);
}

ECode CLauncherApplicationHelper::IsScreenLandscape(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return LauncherApplication::IsScreenLandscape(context, result);
}

ECode CLauncherApplicationHelper::GetScreenDensity(
    /* [out] */ Float* density)
{
    VALIDATE_NOT_NULL(density);

    return LauncherApplication::GetScreenDensity(density);
}

ECode CLauncherApplicationHelper::GetLongPressTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout);

    return LauncherApplication::GetLongPressTimeout(timeout);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos