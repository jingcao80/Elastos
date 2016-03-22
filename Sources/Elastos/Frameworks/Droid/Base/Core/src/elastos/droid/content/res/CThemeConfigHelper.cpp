
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
    AutoPtr<IThemeConfig> tc = ThemeConfig::FromJson(json);
    *themeConfig = tc;
    REFCOUNT_ADD(*themeConfig)
    return NOERROR;
}

ECode CThemeConfigHelper::GetBootTheme(
    /* [in] */ IContentResolver* resolver,
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    return ThemeConfig::GetBootTheme(resolver, themeConfig);
}

ECode CThemeConfigHelper::GetBootThemeForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int32 userHandle,
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    return ThemeConfig::GetBootThemeForUser(resolver, userHandle, themeConfig);
}

ECode CThemeConfigHelper::GetSystemTheme(
    /* [out] */ IThemeConfig** themeConfig)
{
    VALIDATE_NOT_NULL(themeConfig)
    AutoPtr<IThemeConfig> tc = ThemeConfig::GetSystemTheme();
    *themeConfig = tc;
    REFCOUNT_ADD(*themeConfig)
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
