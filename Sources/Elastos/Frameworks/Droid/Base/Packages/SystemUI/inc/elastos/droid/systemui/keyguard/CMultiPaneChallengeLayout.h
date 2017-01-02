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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CMULTIPANECHALLENGELAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CMULTIPANECHALLENGELAYOUT_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CMultiPaneChallengeLayout.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/droid/view/ViewGroup.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CMultiPaneChallengeLayout)
    , public ViewGroup
    , public IChallengeLayout
    , public IMultiPaneChallengeLayout
{
public:
    class MultiPaneChallengeLayoutLayoutParams
        : public ViewGroup::MarginLayoutParams
        , public IMultiPaneChallengeLayoutLayoutParams
    {
    public:
        TO_STRING_IMPL("CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams")

        CAR_INTERFACE_DECL()

        MultiPaneChallengeLayoutLayoutParams();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ IMultiPaneChallengeLayout* parent);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IViewGroupMarginLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IMultiPaneChallengeLayoutLayoutParams* source);

    public:
        Float mCenterWithinArea;

        Int32 mChildType;

        Int32 mGravity;

        Int32 mMaxWidth;
        Int32 mMaxHeight;
    };

private:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CMultiPaneChallengeLayout::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CMultiPaneChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMultiPaneChallengeLayout* mHost;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("CMultiPaneChallengeLayout::MyAnimatorListenerAdapter")

        MyAnimatorListenerAdapter(
            /* [in] */ CMultiPaneChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        CMultiPaneChallengeLayout* mHost;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        TO_STRING_IMPL("CMultiPaneChallengeLayout::MyAnimatorListenerAdapter2")

        MyAnimatorListenerAdapter2(
            /* [in] */ CMultiPaneChallengeLayout* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CMultiPaneChallengeLayout* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CMultiPaneChallengeLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI SetInsets(
        /* [in] */ IRect* insets);

    //@Override
    CARAPI IsChallengeShowing(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI IsChallengeOverlapping(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ShowChallenge(
        /* [in] */ Boolean b);

    //@Override
    CARAPI GetBouncerAnimationDuration(
        /* [out] */ Int32* duration);

    //@Override
    CARAPI ShowBouncer();

    //@Override
    CARAPI HideBouncer();

    //@Override
    CARAPI IsBouncing(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetOnBouncerStateChangedListener(
        /* [in] */ IChallengeLayoutOnBouncerStateChangedListener* listener);

    //@Override
    CARAPI RequestChildFocus(
        /* [in] */ IView* child,
        /* [in] */ IView* focused);

    CARAPI SetScrimView(
        /* [in] */ IView* scrim);

    //@Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** p);

protected:
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthSpec,
        /* [in] */ Int32 heightSpec);

    //@Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    // @Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** outp);

    // @Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

private:
    CARAPI_(Int32) GetVirtualHeight(
        /* [in] */ IMultiPaneChallengeLayoutLayoutParams* lp,
        /* [in] */ Int32 height,
        /* [in] */ Int32 heightUsed);

    CARAPI_(void) LayoutWithGravity(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ IView* child,
        /* [in] */ IRect* padding,
        /* [in] */ Boolean adjustPadding);


private:
    friend class CMultiPaneChallengeLayout::MultiPaneChallengeLayoutLayoutParams;
    static const String TAG;

    Int32 mOrientation;
    Boolean mIsBouncing;

    AutoPtr<IKeyguardSecurityContainer> mChallengeView;
    AutoPtr<IView> mUserSwitcherView;
    AutoPtr<IView> mScrimView;
    AutoPtr<IChallengeLayoutOnBouncerStateChangedListener> mBouncerListener;

    AutoPtr<IRect> mTempRect;
    AutoPtr<IRect> mZeroPadding;
    AutoPtr<IRect> mInsets;

    AutoPtr<IDisplayMetrics> mDisplayMetrics;

    AutoPtr<IViewOnClickListener> mScrimClickListener;

};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CMULTIPANECHALLENGELAYOUT_H__
