
#include "elastos/droid/launcher2/CLauncherProviderHelper.h"
#include "elastos/droid/launcher2/LauncherProvider.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CLauncherProviderHelper)

CAR_INTERFACE_IMPL(CLauncherProviderHelper, Singleton, ILauncherProviderHelper)

ECode CLauncherProviderHelper::BuildOrWhereString(
    /* [in] */ const String& column,
    /* [in] */ ArrayOf<Int32>* values,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    return LauncherProvider::BuildOrWhereString(column, values, result);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos