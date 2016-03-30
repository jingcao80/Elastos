#include "elastos/droid/internal/utility/gesture/CEdgeGesturePosition.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Gesture {

CAR_INTERFACE_IMPL(CEdgeGesturePosition, Object, IEdgeGesturePosition)

CAR_OBJECT_IMPL(CEdgeGesturePosition)

const AutoPtr<CEdgeGesturePosition> CEdgeGesturePosition::LEFT;
const AutoPtr<CEdgeGesturePosition> CEdgeGesturePosition::BOTTOM;
const AutoPtr<CEdgeGesturePosition> CEdgeGesturePosition::RIGHT;
const AutoPtr<CEdgeGesturePosition> CEdgeGesturePosition::TOP;
Boolean CEdgeGesturePosition::sInit = InitStatic();

CEdgeGesturePosition::CEdgeGesturePosition()
    : INDEX(0)
    , FLAG(0)
    , FACTOR(0)
{
}

ECode CEdgeGesturePosition::constructor()
{
    return NOERROR;
}

ECode CEdgeGesturePosition::constructor(
    /* [in] */ Int32 index,
    /* [in] */ Int32 factor)
{
    INDEX = index;
    FLAG = (0x01 << index);
    FACTOR = factor;
    return NOERROR;
}

Boolean CEdgeGesturePosition::InitStatic()
{
    CEdgeGesturePosition::NewByFriend(0, 0, (CEdgeGesturePosition**)&LEFT);
    CEdgeGesturePosition::NewByFriend(1, 1, (CEdgeGesturePosition**)&BOTTOM);
    CEdgeGesturePosition::NewByFriend(2, 1, (CEdgeGesturePosition**)&RIGHT);
    CEdgeGesturePosition::NewByFriend(3, 0, (CEdgeGesturePosition**)&TOP);
    return TRUE;
}

} // namespace Gesture
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
