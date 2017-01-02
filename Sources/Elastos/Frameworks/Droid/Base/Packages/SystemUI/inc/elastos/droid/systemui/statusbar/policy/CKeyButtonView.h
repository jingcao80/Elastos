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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CKEYBUTTONVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CKEYBUTTONVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CKeyButtonView.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Media.h"
#include <elastos/core/Runnable.h>
#include <elastos/droid/widget/ImageView.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ImageView;
using Elastos::Droid::Widget::ImageViewScaleType;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CKeyButtonView)
    , public ImageView
    , public IKeyButtonView
{
private:
    class CheckLongPressRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CKeyButtonView::CheckLongPressRunnable")

        CheckLongPressRunnable(
            /* [in] */ CKeyButtonView* host);

        virtual CARAPI Run();

    private:
        CKeyButtonView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CKeyButtonView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    // @Override
    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* result);

    CARAPI SetQuiescentAlpha(
        /* [in] */ Float alpha,
        /* [in] */ Boolean animate);

    CARAPI GetQuiescentAlpha(
        /* [out] */ Float* result);

    CARAPI GetDrawingAlpha(
        /* [out] */ Float* result);

    CARAPI SetDrawingAlpha(
        /* [in] */ Float alpha);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI SendEvent(
        /* [in] */ Int32 action,
        /* [in] */ Int32 flags);

    CARAPI PlaySoundEffect(
        /* [in] */ Int32 soundConstant);

protected:
    CARAPI_(AutoPtr<IObjectAnimator>) AnimateToQuiescent();

    CARAPI_(void) SendEvent(
        /* [in] */ Int32 action,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 when);

private:
    static const String TAG;
    static const Boolean DEBUG;

    // TODO: Get rid of this
    static const Float DEFAULT_QUIESCENT_ALPHA;

    Int64 mDownTime;
    Int32 mCode;
    Int32 mTouchSlop;
    Float mDrawingAlpha;
    Float mQuiescentAlpha;
    Boolean mSupportsLongpress;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IAnimator> mAnimateToQuiescent;

    AutoPtr<IRunnable> mCheckLongPress;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CKEYBUTTONVIEW_H__
