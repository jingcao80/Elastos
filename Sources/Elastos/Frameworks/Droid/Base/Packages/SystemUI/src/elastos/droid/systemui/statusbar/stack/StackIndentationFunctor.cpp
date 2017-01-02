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
