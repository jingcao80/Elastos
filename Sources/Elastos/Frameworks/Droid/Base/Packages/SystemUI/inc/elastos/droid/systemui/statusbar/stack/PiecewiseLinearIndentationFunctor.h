
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_PIECEWISELINEARINDETATIONFUNCTOR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_PIECEWISELINEARINDETATIONFUNCTOR_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/statusbar/stack/StackIndentationFunctor.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

/**
 * A Functor which interpolates the stack distance linearly based on base values.
 * The base values are based on an interpolation between a linear function and a
 * quadratic function
 */
class PiecewiseLinearIndentationFunctor: public StackIndentationFunctor
{
public:
    /**
     * @param maxItemsInStack The maximum number of items which should be visible at the same time,
     *                        i.e the function returns totalTransitionDistance for the element with
     *                        index maxItemsInStack
     * @param peekSize The visual appearance of this is how far the cards in the stack peek
     *                 out below the top card and it is measured in real pixels.
     *                 Note that the visual appearance does not necessarily always correspond to
     *                 the actual visual distance below the top card but is a maximum,
     *                 achieved when the next card just starts transitioning into the stack and
     *                 the stack is full.
     *                 If distanceToPeekStart is 0, we directly start at the peek, otherwise the
     *                 first element transitions between 0 and distanceToPeekStart.
     *                 Visualization:
     *           ---------------------------------------------------   ---
     *          |                                                   |   |
     *          |                  FIRST ITEM                       |   | <- distanceToPeekStart
     *          |                                                   |   |
     *          |---------------------------------------------------|  ---  ---
     *          |__________________SECOND ITEM______________________|        |  <- peekSize
     *          |===================================================|       _|_
     *
     * @param distanceToPeekStart The distance to the start of the peak.
     * @param linearPart The interpolation factor between the linear and the quadratic amount taken.
     *                   This factor must be somewhere in [0 , 1]
     */
    PiecewiseLinearIndentationFunctor(
        /* [in] */ Int32 maxItemsInStack,
        /* [in] */ Int32 peekSize,
        /* [in] */ Int32 distanceToPeekStart,
        /* [in] */ Float linearPart);

    // @Override
    CARAPI GetValue(
        /* [in] */ Float itemsBefore,
        /* [out] */ Float* value);

private:
    CARAPI_(void) InitBaseValues();

    /**
     * Get the sum of squares up to and including n, i.e sum(i * i, 1, n)
     *
     * @param n the maximum square to include
     * @return
     */
    CARAPI_(Int32) GetSumOfSquares(
        /* [in] */ Int32 n);

private:
    AutoPtr<IArrayList> mBaseValues;  /*<Float*/
    Float mLinearPart;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_PIECEWISELINEARINDETATIONFUNCTOR_H__
