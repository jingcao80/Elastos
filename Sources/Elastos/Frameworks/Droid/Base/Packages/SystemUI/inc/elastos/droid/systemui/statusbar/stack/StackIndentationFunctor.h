//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKINDENTATIONFUNCTOR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKINDENTATIONFUNCTOR_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

/**
 * A functor which can be queried for offset given the number of items before it.
 */
class StackIndentationFunctor
    : public Object
    , public IStackIndentationFunctor
{
public:
    CAR_INTERFACE_DECL()

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
     */
    StackIndentationFunctor(
        /* [in] */ Int32 maxItemsInStack,
        /* [in] */ Int32 peekSize,
        /* [in] */ Int32 distanceToPeekStart);

    CARAPI SetPeekSize(
        /* [in] */ Int32 size);

    CARAPI SetDistanceToPeekStart(
        /* [in] */ Int32 distanceToPeekStart);

    /**
     * Gets the offset of this Functor given a the quantity of items before it
     *
     * @param itemsBefore how many items are already in the stack before this element
     * @return the offset
     */
    virtual CARAPI GetValue(
        /* [in] */ Float itemsBefore,
        /* [out] */ Float* value) = 0;

private:
    CARAPI_(void) UpdateTotalTransitionDistance();

protected:
    Int32 mTotalTransitionDistance;
    Int32 mDistanceToPeekStart;
    Int32 mMaxItemsInStack;
    Int32 mPeekSize;
    Boolean mStackStartsAtPeek;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKINDENTATIONFUNCTOR_H__
