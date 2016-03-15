
#include "elastos/droid/content/res/CThemeConfigBuilder.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CThemeConfigBuilder, Object, IThemeConfigBuilder)

CAR_OBJECT_IMPL(CThemeConfigBuilder)

ECode CThemeConfigBuilder::constructor()
{
    assert(0);
    return NOERROR;
}

ECode CThemeConfigBuilder::constructor(
    /* [in] */ IThemeConfig* theme)
{
    assert(0);
    return NOERROR;
}

ECode CThemeConfigBuilder::DefaultOverlay(
    /* [in] */ const String& themePkgName)
{
    assert(0);
    return NOERROR;
}

ECode CThemeConfigBuilder::DefaultFont(
    /* [in] */ const String& themePkgName)
{
    assert(0);
    return NOERROR;
}

ECode CThemeConfigBuilder::DefaultIcon(
    /* [in] */ const String& themePkgName)
{
    assert(0);
    return NOERROR;
}

ECode CThemeConfigBuilder::Build(
    /* [out] */ IThemeConfig** themeConfig)
{
    assert(0);
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

