
#include "elastos/droid/app/CFragmentManagerHelper.h"
#include "elastos/droid/app/FragmentManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CFragmentManagerHelper, Singleton, IFragmentManagerHelper)

CAR_SINGLETON_IMPL(CFragmentManagerHelper)

ECode CFragmentManagerHelper::EnableDebugLogging(
    /* [in] */ Boolean enabled)
{
    return FragmentManager::EnableDebugLogging(enabled);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
