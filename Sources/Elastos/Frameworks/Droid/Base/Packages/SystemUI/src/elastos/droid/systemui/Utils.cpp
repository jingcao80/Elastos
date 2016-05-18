
#include "elastos/droid/systemui/Utils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/ClassLoader.h>

using Elastos::Core::CPathClassLoader;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

static const String TAG("Elastos.Droid.SystemUI.Utils");
const String Utils::sModulePath("Elastos.Droid.SystemUI.eco");

AutoPtr<IClassLoader> Utils::sClassLoader;

AutoPtr<IClassInfo> Utils::GetClassInfo(
    /* [in] */ const String& className)
{
    if (sClassLoader == NULL) {
        ASSERT_SUCCEEDED(CPathClassLoader::New(sModulePath, NULL, (IClassLoader**)&sClassLoader))
        assert(sClassLoader != NULL);
    }

    AutoPtr<IClassInfo> classInfo;
    sClassLoader->LoadClass(className, (IClassInfo**)&classInfo);
    assert(classInfo != NULL);
    return classInfo;
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos