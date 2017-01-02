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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_ANIMATIONFILTER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_ANIMATIONFILTER_H__

#include "_Elastos.Droid.SystemUI.h"
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
 * Filters the animations for only a certain type of properties.
 */
class AnimationFilter
    : public Object
    , public IAnimationFilter
{
public:
    CAR_INTERFACE_DECL()

    AnimationFilter();

    CARAPI AnimateAlpha();

    CARAPI AnimateY();

    CARAPI HasDelays();

    CARAPI AnimateZ();

    CARAPI AnimateScale();

    CARAPI AnimateHeight();

    CARAPI AnimateTopInset();

    CARAPI AnimateDimmed();

    CARAPI AnimateDark();

    CARAPI AnimateHideSensitive();

    /**
     * Combines multiple filters into {@code this} filter, using or as the operand .
     *
     * @param events The animation events from the filters to combine.
     */
    CARAPI ApplyCombination(
        /* [in] */ IArrayList/*<NotificationStackScrollLayout.AnimationEvent>*/* events);

private:
    CARAPI_(void) CombineFilter(
        /* [in] */ AnimationFilter* filter);

    CARAPI_(void) Reset();

public:
    Boolean mAnimateAlpha;
    Boolean mAnimateY;
    Boolean mAnimateZ;
    Boolean mAnimateScale;
    Boolean mAnimateHeight;
    Boolean mAnimateTopInset;
    Boolean mAnimateDimmed;
    Boolean mAnimateDark;
    Boolean mAnimateHideSensitive;
    Boolean mHasDelays;
    Boolean mHasGoToFullShadeEvent;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_ANIMATIONFILTER_H__
