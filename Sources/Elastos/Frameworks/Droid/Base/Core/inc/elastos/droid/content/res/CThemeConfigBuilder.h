
#ifndef __ELASTOS_DROID_CONTENT_RES_CTHEMECONFIGBUILDER_H__
#define __ELASTOS_DROID_CONTENT_RES_CTHEMECONFIGBUILDER_H__

#include "_Elastos_Droid_Content_Res_CThemeConfigBuilder.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

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

    CARAPI Build(
        /* [out] */ IThemeConfig** themeConfig);
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_RES_CTHEMECONFIGBUILDER_H__
