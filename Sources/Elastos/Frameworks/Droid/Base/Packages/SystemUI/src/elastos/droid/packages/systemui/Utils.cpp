
#include "elastos/droid/packages/systemui/Utils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

static const String TAG("Elastos.Droid.Packages.SystemUI.Utils");

const String Utils::sModulePath("......../SystemUI.eco");
AutoPtr<IModuleInfo> Utils::sModuleInfo;
AutoPtr<IClassInfo> Utils::GetClassInfo(
    /* [in] */ const String& className)
{
    if (sModuleInfo == NULL) {
        ASSERT_SUCCEEDED(CReflector::AcquireModuleInfo(sModulePath, (IModuleInfo**)&sModuleInfo));
        assert(sModuleInfo != NULL);
    }

    String name = String("C") + className.Substring(className.LastIndexOf('.'));
    Logger::D(TAG, "the class name is [%s]", name.string());
    AutoPtr<IClassInfo> classInfo;
    sModuleInfo->GetClassInfo(className, (IClassInfo**)&classInfo);
    assert(classInfo != NULL);
    return classInfo;
}

} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos