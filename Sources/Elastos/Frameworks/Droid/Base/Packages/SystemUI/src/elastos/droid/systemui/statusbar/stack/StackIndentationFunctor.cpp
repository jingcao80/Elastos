
#include "elastos/droid/systemui/statusbar/stack/StackIndentationFunctor.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

CAR_INTERFACE_IMPL(StackIndentationFunctor, Object, IStackIndentationFunctor)
StackIndentationFunctor::StackIndentationFunctor(
    /* [in] */ Int32 maxItemsInStack,
    /* [in] */ Int32 peekSize,
    /* [in] */ Int32 distanceToPeekStart)
{
    mDistanceToPeekStart = distanceToPeekStart;
    mStackStartsAtPeek = mDistanceToPeekStart == 0;
    mMaxItemsInStack = maxItemsInStack;
    mPeekSize = peekSize;
    UpdateTotalTransitionDistance();
}

void StackIndentationFunctor::UpdateTotalTransitionDistance()
{
    mTotalTransitionDistance = mDistanceToPeekStart + mPeekSize;
}

ECode StackIndentationFunctor::SetPeekSize(
    /* [in] */ Int32 size)
{
    mPeekSize = size;
    UpdateTotalTransitionDistance();
    return NOERROR;
}

ECode StackIndentationFunctor::SetDistanceToPeekStart(
    /* [in] */ Int32 distanceToPeekStart)
{
    mDistanceToPeekStart = distanceToPeekStart;
    mStackStartsAtPeek = mDistanceToPeekStart == 0;
    UpdateTotalTransitionDistance();
    return NOERROR;
}

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
