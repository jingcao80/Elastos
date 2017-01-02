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

#ifndef __ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICS_H__
#define __ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICS_H__

#include "_Elastos_Droid_Graphics_CPaintFontMetrics.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPaintFontMetrics)
    , public Object
    , public IPaintFontMetrics
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetTop(
        /* [out] */ Float* top);

    CARAPI SetTop(
        /* [in] */ Float top);

    CARAPI GetAscent(
        /* [out] */ Float* ascent);

    CARAPI SetAscent(
        /* [in] */ Float ascent);

    CARAPI GetDescent(
        /* [out] */ Float* descent);

    CARAPI SetDescent(
        /* [in] */ Float descent);

    CARAPI GetBottom(
        /* [out] */ Float* bottom);

    CARAPI SetBottom(
        /* [in] */ Float bottom);

    CARAPI GetLeading(
        /* [out] */ Float* leading);

    CARAPI SetLeading(
        /* [in] */ Float leading);

private:
    Float mTop;
    Float mAscent;
    Float mDescent;
    Float mBottom;
    Float mLeading;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CPAINTFONTMETRICS_H__
