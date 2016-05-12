#include "elastos/droid/internal/utility/gesture/CEdgeGesturePosition.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Gesture {

static AutoPtr<IEdgeGesturePosition> Init(Int32 index, Int32 factor)
{
    AutoPtr<IEdgeGesturePosition> obj;
    CEdgeGesturePosition::New(index, factor, (IEdgeGesturePosition**)&obj);
    return obj;
}

const AutoPtr<IEdgeGesturePosition> CEdgeGesturePosition::LEFT = Init(0, 0);
const AutoPtr<IEdgeGesturePosition> CEdgeGesturePosition::BOTTOM = Init(1, 1);
const AutoPtr<IEdgeGesturePosition> CEdgeGesturePosition::RIGHT = Init(2, 1);
const AutoPtr<IEdgeGesturePosition> CEdgeGesturePosition::TOP = Init(3, 0);

CAR_INTERFACE_IMPL(CEdgeGesturePosition, Object, IEdgeGesturePosition)

CAR_OBJECT_IMPL(CEdgeGesturePosition)

CEdgeGesturePosition::CEdgeGesturePosition()
    : mIndex(0)
    , mFlag(0)
    , mFactor(0)
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
    mIndex = index;
    mFlag = (0x01 << index);
    mFactor = factor;
    return NOERROR;
}

ECode CEdgeGesturePosition::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    *index = mIndex;
    return NOERROR;
}

ECode CEdgeGesturePosition::GetFlag(
    /* [out] */ Int32* flag)
{
    VALIDATE_NOT_NULL(flag)
    *flag = mFlag;
    return NOERROR;
}

ECode CEdgeGesturePosition::GetFactor(
    /* [out] */ Int32* factor)
{
    VALIDATE_NOT_NULL(factor)
    *factor = mFactor;
    return NOERROR;
}

AutoPtr<ArrayOf<IEdgeGesturePosition*> > CEdgeGesturePosition::Values()
{
    AutoPtr<ArrayOf<IEdgeGesturePosition*> > array = ArrayOf<IEdgeGesturePosition*>::Alloc(4);
    array->Set(0, LEFT.Get());
    array->Set(0, BOTTOM.Get());
    array->Set(0, RIGHT.Get());
    array->Set(0, TOP.Get());
    return array;
}

} // namespace Gesture
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
