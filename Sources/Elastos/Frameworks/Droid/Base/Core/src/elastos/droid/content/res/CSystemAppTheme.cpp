
#include "elastos/droid/content/res/CSystemAppTheme.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {


CAR_OBJECT_IMPL(CSystemAppTheme)

ECode CSystemAppTheme::constructor()
{
    return ThemeConfig::AppTheme::constructor(
            IThemeConfig::SYSTEM_DEFAULT, IThemeConfig::SYSTEM_DEFAULT, IThemeConfig::SYSTEM_DEFAULT);
}

ECode CSystemAppTheme::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("No Theme Applied (Holo)");
    return NOERROR;
}

}
}
}
}
