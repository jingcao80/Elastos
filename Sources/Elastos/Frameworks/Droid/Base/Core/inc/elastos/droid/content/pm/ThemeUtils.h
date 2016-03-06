
#ifndef __ELASTOS_DROID_CONTENT_PM_THEMEUTILS_H__
#define __ELASTOS_DROID_CONTENT_PM_THEMEUTILS_H__

#include "_Elastos.Droid.Content.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

class ThemeUtils
{
public:
    static CARAPI_(String) GetIconPackPkgPath(
        /* [in] */ const String& pkgName);

    static CARAPI_(String) GetIconPackResPath(
        /* [in] */ const String& pkgName);
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_PM_THEMEUTILS_H__
