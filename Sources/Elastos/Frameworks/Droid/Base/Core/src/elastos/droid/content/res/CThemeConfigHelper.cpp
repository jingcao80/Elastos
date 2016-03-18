
#include "elastos/droid/content/res/CThemeConfigHelper.h"
#include "elastos/droid/content/res/ThemeConfig.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CThemeConfigHelper, Singleton, IThemeConfigHelper)

CAR_SINGLETON_IMPL(CThemeConfigHelper)

ECode CThemeConfigHelper::FromJson(
    /* [in] */ const String& json,
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    *themeConfig = ThemeConfig::FromJson(json);
    REFCOUNT_ADD(*themeConfig)
    return NOERROR;
}

ECode CThemeConfigHelper::GetBootTheme(
    /* [in] */ IContentResolver* resolver,
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    *themeConfig = ThemeConfig::GetBootTheme(resolver);
    REFCOUNT_ADD(*themeConfig)
    return NOERROR;
}

ECode CThemeConfigHelper::GetBootThemeForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32 userHandle,
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    *themeConfig = ThemeConfig::GetBootThemeForUser(resolver, userHandle);
    REFCOUNT_ADD(*themeConfig)
    return NOERROR;
}

ECode CThemeConfigHelper::GetSystemTheme(
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    *themeConfig = ThemeConfig::GetSystemTheme();
    REFCOUNT_ADD(*themeConfig)
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
