
#ifndef __ELASTOS_DROID_CONTENT_RES_CTHEMECONFIGHELPER_H__
#define __ELASTOS_DROID_CONTENT_RES_CTHEMECONFIGHELPER_H__

#include "_Elastos_Droid_Content_Res_CThemeConfigHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CThemeConfigHelper)
    , public Singleton
    , public IThemeConfigHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FromJson(
        /* [in] */ const String& json,
        /* [out] */ IThemeConfig** themeConfig);

    /**
     * Represents the theme that the device booted into. This is used to
     * simulate a "default" configuration based on the user's last known
     * preference until the theme is switched at runtime.
     */
    CARAPI GetBootTheme(
        /* [in] */ IContentResolver* resolver,
        /* [out] */ IThemeConfig** themeConfig);

    CARAPI GetBootThemeForUser(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int32 userHandle,
        /* [out] */ IThemeConfig** themeConfig);

    /**
     * Represents the system framework theme, perceived by the system as there
     * being no theme applied.
     */
    CARAPI GetSystemTheme(
        /* [out] */ IThemeConfig** themeConfig);
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CTHEMECONFIGHELPER_H__
