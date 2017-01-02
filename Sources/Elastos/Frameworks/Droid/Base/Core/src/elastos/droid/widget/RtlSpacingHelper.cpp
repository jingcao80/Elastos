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

#include "elastos/droid/widget/RtlSpacingHelper.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 RtlSpacingHelper::UNDEFINED = Elastos::Core::Math::INT32_MIN_VALUE;
RtlSpacingHelper::RtlSpacingHelper()
    : mLeft(0)
    , mRight(0)
    , mStart(UNDEFINED)
    , mEnd(UNDEFINED)
    , mExplicitLeft(0)
    , mExplicitRight(0)
    , mIsRtl(FALSE)
    , mIsRelative(FALSE)
{}

Int32 RtlSpacingHelper::GetLeft()
{
    return mLeft;
}

Int32 RtlSpacingHelper::GetRight()
{
    return mRight;
}

Int32 RtlSpacingHelper::GetStart()
{
    return mIsRtl ? mRight : mLeft;
}

Int32 RtlSpacingHelper::GetEnd()
{
    return mIsRtl ? mLeft : mRight;
}

void RtlSpacingHelper::SetRelative(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    mStart = start;
    mEnd = end;
    mIsRelative = TRUE;
    if (mIsRtl) {
        if (end != UNDEFINED) mLeft = end;
        if (start != UNDEFINED) mRight = start;
    }
    else {
        if (start != UNDEFINED) mLeft = start;
        if (end != UNDEFINED) mRight = end;
    }
}

void RtlSpacingHelper::SetAbsolute(
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    mIsRelative = FALSE;
    if (left != UNDEFINED) mLeft = mExplicitLeft = left;
    if (right != UNDEFINED) mRight = mExplicitRight = right;
}

void RtlSpacingHelper::SetDirection(
    /* [in] */ Boolean isRtl)
{
    if (isRtl == mIsRtl) {
        return;
    }
    mIsRtl = isRtl;
    if (mIsRelative) {
        if (isRtl) {
            mLeft = mEnd != UNDEFINED ? mEnd : mExplicitLeft;
            mRight = mStart != UNDEFINED ? mStart : mExplicitRight;
        }
        else {
            mLeft = mStart != UNDEFINED ? mStart : mExplicitLeft;
            mRight = mEnd != UNDEFINED ? mEnd : mExplicitRight;
        }
    }
    else {
        mLeft = mExplicitLeft;
        mRight = mExplicitRight;
    }
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
