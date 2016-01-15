
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_UTILS_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_UTILS_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

class Utils
{
public:
    static AutoPtr<IClassInfo> GetClassInfo(
        /* [in] */ const String& className);

private:
    static const String sModulePath;
    static AutoPtr<IModuleInfo> sModuleInfo;
};

} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_UTILS_H__
