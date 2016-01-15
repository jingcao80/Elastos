
#include "elastos/droid/packages/systemui/Utils.h"

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

const String Utils::sModulePath("......../SystemUI.eco");
AutoPtr<IModuleInfo> Utils::sModuleInfo;
AutoPtr<IClassInfo> Utils::GetClassInfo(
    /* [in] */ const String& className)
{
    if (sModuleInfo == NULL) {
        ASSERT_SUCCEEDED(CReflector::AcquireModuleInfo(sModulePath, (IModuleInfo**)&sModuleInfo));
        assert(sModuleInfo != NULL);
    }
    AutoPtr<IClassInfo> classInfo;
    sModuleInfo->GetClassInfo(className, (IClassInfo**)&classInfo);
    assert(classInfo != NULL);
    return classInfo;
}

} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos