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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSCRIMVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSCRIMVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CScrimView.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/view/View.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::View::Animation::IInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

/**
 * A view which can draw a scrim
 */
CarClass(CScrimView)
    , public Elastos::Droid::View::View
    , public IScrimView
{
private:
    class AlphaUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        AlphaUpdateListener(
            /* [in] */ CScrimView* host);

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CScrimView* mHost;
    };

    class ClearAnimatorListener
        : public AnimatorListenerAdapter
    {
    public:
        ClearAnimatorListener(
            /* [in] */ CScrimView* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CScrimView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CScrimView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetDrawAsSrc(
        /* [in] */ Boolean asSrc);

    CARAPI SetScrimColor(
        /* [in] */ Int32 color);

    CARAPI GetScrimColor(
        /* [out] */ Int32* color);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* has);

    CARAPI SetViewAlpha(
        /* [in] */ Float alpha);

    CARAPI AnimateViewAlpha(
        /* [in] */ Float alpha,
        /* [in] */ Int64 durationOut,
        /* [in] */ IInterpolator* interpolator);

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    Int32 mScrimColor;
    Boolean mIsEmpty;
    Boolean mDrawAsSrc;
    Float mViewAlpha;
    AutoPtr<IValueAnimator> mAlphaAnimator;
    AutoPtr<IAnimatorUpdateListener> mAlphaUpdateListener;
    AutoPtr<AnimatorListenerAdapter> mClearAnimatorListener;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CSCRIMVIEW_H__
