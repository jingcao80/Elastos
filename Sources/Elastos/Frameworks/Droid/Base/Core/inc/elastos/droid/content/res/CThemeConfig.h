
#ifndef __ELASTOS_DROID_CONTENT_RES_CThemeConfig_H__
#define __ELASTOS_DROID_CONTENT_RES_CThemeConfig_H__

#include "_Elastos_Droid_Content_Res_CThemeConfig.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloneable;
using Elastos::Core::IComparable;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * The Theme Configuration allows lookup of a theme element (fonts, icon, overlay) for a given
 * application. If there isn't a particular theme designated to an app, it will fallback on the
 * default theme. If there isn't a default theme then it will simply fallback to holo.
 *
 * @hide
 */
CarClass(CThemeConfig)
    , public Object
    , public IThemeConfig
    , public ICloneable
    , public IParcelable
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetOverlayPkgNameForApp(
        /* [in] */ const String& appPkgName,
        /* [out] */ String* overlayPkgName);

    CARAPI GetIconPackPkgName(
        /* [out] */ String* icPkgName);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI CompareTo(
        /* [in] */ IInterface* another,
        /* [out] */ Int32* result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Represents the theme that the device booted into. This is used to
     * simulate a "default" configuration based on the user's last known
     * preference until the theme is switched at runtime.
     */
    static CARAPI GetBootTheme(
        /* [in] */ IContentResolver* resolver,
        /* [out] */ IThemeConfig** themeConfig);

    static CARAPI_(AutoPtr<IThemeConfig>) FromJson(
        /* [in] */ const String& json);

    /**
     * Represents the system framework theme, perceived by the system as there
     * being no theme applied.
     */
    static CARAPI GetSystemTheme(
        /* [out] */ IThemeConfig** themeConfig);
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CThemeConfig_H__
