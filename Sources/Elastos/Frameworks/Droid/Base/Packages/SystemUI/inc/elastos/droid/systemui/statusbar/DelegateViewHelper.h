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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DELEGATEVIEWHELPER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DELEGATEVIEWHELPER_H__

#include <elastos/droid/ext/frameworkext.h>
#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class DelegateViewHelper : public Object
{
public:
    DelegateViewHelper(
        /* [in] */ IView* sourceView);

    CARAPI_(void) SetDelegateView(
        /* [in] */ IView* view);

    CARAPI_(void) SetBar(
        /* [in] */ IBaseStatusBar* phoneStatusBar);

    CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) SetSourceView(
        /* [in] */ IView* view);

    /**
     * Selects the initial touch region based on a list of views.  This is meant to be called by
     * a container widget on children over which the initial touch should be detected.  Note this
     * will compute a minimum bound that contains all specified views.
     *
     * @param views
     */
    CARAPI_(void) SetInitialTouchRegion(
        /* [in] */ ArrayOf<IView*>* views);

    /**
     * When rotation is set to NO_SENSOR, then this allows swapping x/y for gesture detection
     * @param swap
     */
    CARAPI_(void) SetSwapXY(
        /* [in] */ Boolean swap);

    CARAPI_(void) SetDisabled(
        /* [in] */ Boolean disabled);

public:
    AutoPtr<IRectF> mInitialTouch;
    IBaseStatusBar* mBar;

private:
    AutoPtr<IView> mDelegateView;
    AutoPtr<IView> mSourceView;
    AutoPtr<ArrayOf<Int32> > mTempPoint;
    Float mDownPoint[2];
    Float mTriggerThreshhold;
    Boolean mPanelShowing;
    Boolean mStarted;
    Boolean mSwapXY;
    Boolean mDisabled;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_DELEGATEVIEWHELPER_H__
