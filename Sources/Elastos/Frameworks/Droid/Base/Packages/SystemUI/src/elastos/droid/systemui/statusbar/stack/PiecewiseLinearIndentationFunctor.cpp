
#include "elastos/droid/systemui/statusbar/stack/PiecewiseLinearIndentationFunctor.h"

using Elastos::Core::CFloat;
using Elastos::Core::IFloat;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

PiecewiseLinearIndentationFunctor::PiecewiseLinearIndentationFunctor(
    /* [in] */ Int32 maxItemsInStack,
    /* [in] */ Int32 peekSize,
    /* [in] */ Int32 distanceToPeekStart,
    /* [in] */ Float linearPart)
    : StackIndentationFunctor(maxItemsInStack, peekSize, distanceToPeekStart)
{
    CArrayList::New(maxItemsInStack + 1, (IArrayList**)&mBaseValues);
    InitBaseValues();
    mLinearPart = linearPart;
}

void PiecewiseLinearIndentationFunctor::InitBaseValues()
{
    Int32 sumOfSquares = GetSumOfSquares(mMaxItemsInStack - 1);
    Int32 totalWeight = 0;
    AutoPtr<IFloat> f;
    CFloat::New(0.0f, (IFloat**)&f);
    mBaseValues->Add(f);
    for (Int32 i = 0; i < mMaxItemsInStack - 1; i++) {
        totalWeight += (mMaxItemsInStack - i - 1) * (mMaxItemsInStack - i - 1);
        f = NULL;
        CFloat::New((Float) totalWeight / sumOfSquares, (IFloat**)&f);
        mBaseValues->Add(f);
    }
}

Int32 PiecewiseLinearIndentationFunctor::GetSumOfSquares(
    /* [in] */ Int32 n)
{
    return n * (n + 1) * (2 * n + 1) / 6;
}

ECode PiecewiseLinearIndentationFunctor::GetValue(
    /* [in] */ Float itemsBefore,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    if (mStackStartsAtPeek) {
        // We directly start at the stack, so no initial interpolation.
        itemsBefore++;
    }
    if (itemsBefore < 0) {
        *value = 0;
        return NOERROR;
    }
    else if (itemsBefore >= mMaxItemsInStack) {
        *value = mTotalTransitionDistance;
        return NOERROR;
    }
    Int32 below = (Int32) itemsBefore;
    Float partialIn = itemsBefore - below;

    if (below == 0) {
        *value = mDistanceToPeekStart * partialIn;
        return NOERROR;
    }

    Float result = mDistanceToPeekStart;
    AutoPtr<IInterface> obj1;
    mBaseValues->Get(below - 1, (IInterface**)&obj1);
    AutoPtr<IInterface> obj2;
    mBaseValues->Get(below, (IInterface**)&obj2);
    Float f1 = 0, f2 = 0;
    IFloat::Probe(obj1)->GetValue(&f1);
    IFloat::Probe(obj2)->GetValue(&f2);
    Float progress = f1 * (1 - partialIn) + f2 * partialIn;
    result += (progress * (1 - mLinearPart)
            + (itemsBefore - 1) / (mMaxItemsInStack - 1)  * mLinearPart) * mPeekSize;
    *value = result;
    return NOERROR;
}

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
