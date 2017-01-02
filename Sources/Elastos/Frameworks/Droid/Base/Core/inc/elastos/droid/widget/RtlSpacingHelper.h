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

#ifndef __ELASTOS_DROID_WIDGET_RTLSPACINGHELPER_H__
#define __ELASTOS_DROID_WIDGET_RTLSPACINGHELPER_H__

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * RtlSpacingHelper manages the relationship between left/right and start/end for views
 * that need to maintain both absolute and relative settings for a form of spacing similar
 * to view padding.
 */
class RtlSpacingHelper: public Object
{
public:
    RtlSpacingHelper();

    CARAPI_(Int32) GetLeft();

    CARAPI_(Int32) GetRight();

    CARAPI_(Int32) GetStart();

    CARAPI_(Int32) GetEnd();

    CARAPI_(void) SetRelative(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(void) SetAbsolute(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right);

    CARAPI_(void) SetDirection(
        /* [in] */ Boolean isRtl);

public:
    static const Int32 UNDEFINED;

private:
    Int32 mLeft;
    Int32 mRight;
    Int32 mStart;
    Int32 mEnd;
    Int32 mExplicitLeft;
    Int32 mExplicitRight;

    Boolean mIsRtl;
    Boolean mIsRelative;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_RTLSPACINGHELPER_H__
