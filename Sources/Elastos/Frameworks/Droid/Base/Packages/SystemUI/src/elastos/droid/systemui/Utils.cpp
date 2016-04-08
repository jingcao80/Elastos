
#include "elastos/droid/systemui/Utils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

static const String TAG("Elastos.Droid.SystemUI.Utils");

const String Utils::sModulePath("SystemUI.eco");
const Int32 Utils::ELASTOS_DROID_CORE_ECO_FALG = 1;
AutoPtr<IModuleInfo> Utils::sModuleInfo;
AutoPtr<IClassInfo> Utils::GetClassInfo(
    /* [in] */ const String& className,
    /* [in] */ Int32 flag)
{
    if (flag == ELASTOS_DROID_CORE_ECO_FALG) {
        AutoPtr<IModuleInfo> m;
        ASSERT_SUCCEEDED(CReflector::AcquireModuleInfo(String("Elastos.Droid.Core.eco"), (IModuleInfo**)&m));
        AutoPtr<IClassInfo> classInfo;
        m->GetClassInfo(className, (IClassInfo**)&classInfo);
        assert(classInfo != NULL);
        return classInfo;
    }

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
} // namespace Droid
} // namespace Elastos