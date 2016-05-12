#include "elastos/droid/service/gesture/CEdgeGestureManagerHelper.h"
#include "elastos/droid/service/gesture/EdgeGestureManager.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Gesture {

CAR_INTERFACE_IMPL(CEdgeGestureManagerHelper, Singleton, IEdgeGestureManagerHelper)

CAR_SINGLETON_IMPL(CEdgeGestureManagerHelper)

ECode CEdgeGestureManagerHelper::GetInstance(
    /* [out] */ IEdgeGestureManager** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IEdgeGestureManager> egm = EdgeGestureManager::GetInstance();
    *result = egm;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


} // namespace Gesture
} // namespace Service
} // namepsace Droid
} // namespace Elastos