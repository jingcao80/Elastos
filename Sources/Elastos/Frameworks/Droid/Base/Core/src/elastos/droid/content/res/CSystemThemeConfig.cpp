
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/res/CSystemThemeConfig.h"

using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {


CAR_OBJECT_IMPL(CSystemThemeConfig)

ECode CSystemThemeConfig::constructor()
{
    AutoPtr<IMap> themes;
    CHashMap::New((IMap**)&themes);
    return ThemeConfig::constructor(themes);
}

}
}
}
}
