
#ifndef __ELASTOS_DROID_CONTENT_RES_CTHEMECONFIGBUILDER_H__
#define __ELASTOS_DROID_CONTENT_RES_CTHEMECONFIGBUILDER_H__

#include "_Elastos_Droid_Content_Res_CThemeConfigBuilder.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CThemeConfigBuilder)
    , public Object
    , public IThemeConfigBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IThemeConfig* theme);

    /**
     * For uniquely theming a specific app. ex. "Dialer gets red theme,
     * Calculator gets blue theme"
     */
    CARAPI DefaultOverlay(
        /* [in] */ const String& themePkgName);

    CARAPI DefaultFont(
        /* [in] */ const String& themePkgName);

    CARAPI DefaultIcon(
        /* [in] */ const String& themePkgName);

    CARAPI Icon(
        /* [in] */ const String&appPkgName,
        /* [in] */ const String&themePkgName);

    CARAPI Overlay(
        /* [in] */ const String&appPkgName,
        /* [in] */ const String&themePkgName);

    CARAPI Font(
        /* [in] */ const String&appPkgName,
        /* [in] */ const String&themePkgName);

    CARAPI SetLastThemeChangeRequestType(
        /* [in] */ RequestType requestType);

    CARAPI Build(
        /* [out] */ IThemeConfig** themeConfig);

private:
    HashMap<String, String> mOverlays;
    HashMap<String, String> mIcons;
    HashMap<String, String> mFonts;
    RequestType mLastThemeChangeRequestType;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_RES_CTHEMECONFIGBUILDER_H__
