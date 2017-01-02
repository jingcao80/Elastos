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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_AMBIENTSTATE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_AMBIENTSTATE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

/**
 * A global state to track all input states for the algorithm.
 */
class AmbientState
    : public Object
    , public IAmbientState
{
public:
    CAR_INTERFACE_DECL()

    AmbientState();

    CARAPI GetScrollY(
        /* [out] */ Int32* y);

    CARAPI SetScrollY(
        /* [in] */ Int32 scrollY);

    CARAPI OnBeginDrag(
        /* [in] */ IView* view);

    CARAPI OnDragFinished(
        /* [in] */ IView* view);

    CARAPI GetDraggedViews(
        /* [out] */ IArrayList** list);

    /**
     * @param dimmed Whether we are in a dimmed state (on the lockscreen), where the backgrounds are
     *               translucent and everything is scaled back a bit.
     */
    CARAPI SetDimmed(
        /* [in] */ Boolean dimmed);

    /** In dark mode, we draw as little as possible, assuming a black background */
    CARAPI SetDark(
        /* [in] */ Boolean dark);

    CARAPI SetHideSensitive(
        /* [in] */ Boolean hideSensitive);

    /**
     * In dimmed mode, a child can be activated, which happens on the first tap of the double-tap
     * interaction. This child is then scaled normally and its background is fully opaque.
     */
    CARAPI SetActivatedChild(
        /* [in] */ IActivatableNotificationView* activatedChild);

    CARAPI IsDimmed(
        /* [out] */ Boolean* result);

    CARAPI IsDark(
        /* [out] */ Boolean* result);

    CARAPI IsHideSensitive(
        /* [out] */ Boolean* result);

    CARAPI GetActivatedChild(
        /* [out] */ IActivatableNotificationView** view);

    CARAPI SetOverScrollAmount(
        /* [in] */ Float amount,
        /* [in] */ Boolean onTop);

    CARAPI GetOverScrollAmount(
        /* [in] */ Boolean top,
        /* [out] */ Float* result);

    CARAPI GetSpeedBumpIndex(
        /* [out] */ Int32* index);

    CARAPI SetSpeedBumpIndex(
        /* [in] */ Int32 speedBumpIndex);

private:
    AutoPtr<IArrayList> mDraggedViews;  /*<View*/
    Int32 mScrollY;
    Boolean mDimmed;
    AutoPtr<IActivatableNotificationView> mActivatedChild;
    Float mOverScrollTopAmount;
    Float mOverScrollBottomAmount;
    Int32 mSpeedBumpIndex;
    Boolean mDark;
    Boolean mHideSensitive;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_AMBIENTSTATE_H__
