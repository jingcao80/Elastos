
#include "CEngineHelper.h"
#include <CEngine.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Fortress {

CAR_SINGLETON_IMPL(CEngineHelper)

CAR_INTERFACE_IMPL(CEngineHelper, Singleton, IEngineHelper)

ECode CEngineHelper::SetDoor(
    /* [in] */ ISecurityAccess* door)
{
    CEngine::mDoor = door;
    return NOERROR;
}

ECode CEngineHelper::GetDoor(
    /* [out] */ ISecurityAccess** door)
{
    VALIDATE_NOT_NULL(door)
    *door = CEngine::mDoor;
    REFCOUNT_ADD(*door)
    return NOERROR;
}

} // namespace Fortress
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

