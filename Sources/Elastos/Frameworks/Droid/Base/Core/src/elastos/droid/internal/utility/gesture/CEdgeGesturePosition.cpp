#include "elastos/droid/internal/utility/gesture/CEdgeGesturePosition.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Gesture {

static AutoPtr<CEdgeGesturePosition> Init(Int32 index, Int32 factor)
{
    AutoPtr<CEdgeGesturePosition> obj;
    CEdgeGesturePosition::NewByFriend(index, factor, (CEdgeGesturePosition**)&obj);
    return obj;
}

const AutoPtr<CEdgeGesturePosition> CEdgeGesturePosition::LEFT = Init(0, 0);
const AutoPtr<CEdgeGesturePosition> CEdgeGesturePosition::BOTTOM = Init(1, 1);
const AutoPtr<CEdgeGesturePosition> CEdgeGesturePosition::RIGHT = Init(2, 2);
const AutoPtr<CEdgeGesturePosition> CEdgeGesturePosition::TOP = Init(3, 3);

CAR_INTERFACE_IMPL(CEdgeGesturePosition, Object, IEdgeGesturePosition)

CAR_OBJECT_IMPL(CEdgeGesturePosition)

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

} // namespace Gesture
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
