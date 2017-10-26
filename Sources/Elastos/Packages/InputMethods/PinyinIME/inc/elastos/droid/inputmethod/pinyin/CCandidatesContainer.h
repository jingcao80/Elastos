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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CCANDIDATESCONTAINER_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CCANDIDATESCONTAINER_H__

#include "_Elastos_Droid_InputMethod_Pinyin_CCandidatesContainer.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/inputmethod/pinyin/CPinyinIME.h"
#include "elastos/droid/widget/RelativeLayout.h"

using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::Animation::IAnimationAnimationListener;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::IViewFlipper;
using Elastos::Droid::Widget::RelativeLayout;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

class CBalloonHint;

/**
 * Container used to host the two candidate views. When user drags on candidate
 * view, animation is used to dismiss the current candidate view and show a new
 * one. These two candidate views and their parent are hosted by this container.
 * <p>
 * Besides the candidate views, there are two arrow views to show the page
 * forward/backward arrows.
 * </p>
 */
CarClass(CCandidatesContainer)
    , public RelativeLayout
    , public ICandidatesContainer
    , public IArrowUpdater
{
private:
    class InnerListener
        : public Object
        , public IViewOnTouchListener
        , public IAnimationAnimationListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CCandidatesContainer* host);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnAnimationEnd(
            /* [in] */  IAnimation* animation);

        CARAPI OnAnimationRepeat(
            /* [in] */  IAnimation* animation);

        CARAPI OnAnimationStart(
            /* [in] */  IAnimation* animation);

    private:
        CCandidatesContainer* mHost;
    };

public:
    CCandidatesContainer();

    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Initialize(
        /* [in] */ ICandidateViewListener* cvListener,
        /* [in] */ CBalloonHint* balloonHint,
        /* [in] */ IGestureDetector* gestureDetector);

    CARAPI ShowCandidates(
        /* [in] */ CPinyinIME::DecodingInfo* decInfo,
        /* [in] */ Boolean enableActiveHighlight);

    CARAPI_(Int32) GetCurrentPage();

    CARAPI EnableActiveHighlight(
        /* [in] */ Boolean enableActiveHighlight);

    Boolean ActiveCurseBackward();

    Boolean ActiveCurseForward();

    CARAPI_(Boolean) PageBackward(
        /* [in] */ Boolean animLeftRight,
        /* [in] */ Boolean enableActiveHighlight);

    CARAPI_(Boolean) PageForward(
        /* [in] */ Boolean animLeftRight,
        /* [in] */ Boolean enableActiveHighlight);

    CARAPI_(Int32) GetActiveCandiatePos();

    CARAPI UpdateArrowStatus();

    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // The reason why we handle candiate view's touch events here is because
    // that the view under the focused view may get touch events instead of the
    // focused one.
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI LoadAnimation(
        /* [in] */ Boolean animLeftRight,
        /* [in] */ Boolean forward);

    using RelativeLayout::OnAnimationEnd;

    CARAPI OnAnimationEnd(
        /* [in] */  IAnimation* animation);

    CARAPI OnAnimationRepeat(
        /* [in] */  IAnimation* animation);

    using RelativeLayout::OnAnimationStart;

    CARAPI OnAnimationStart(
        /* [in] */  IAnimation* animation);

protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    CARAPI_(void) EnableArrow(
        /* [in] */ IImageButton* arrowBtn,
        /* [in] */ Boolean enabled);

    CARAPI_(void) ShowArrow(
        /* [in] */ IImageButton* arrowBtn,
        /* [in] */ Boolean show);

    CARAPI_(AutoPtr<IAnimation>) CreateAnimation(
        /* [in] */ Float xFrom,
        /* [in] */ Float xTo,
        /* [in] */ Float yFrom,
        /* [in] */ Float yTo,
        /* [in] */ Float alphaFrom,
        /* [in] */ Float alphaTo,
        /* [in] */ Int64 duration);

    using RelativeLayout::StartAnimation;

    CARAPI_(void) StartAnimation();

    CARAPI_(void) StopAnimation();

protected:
    /**
     * Alpha value to show an enabled arrow.
     */
    static Int32 ARROW_ALPHA_ENABLED;

    /**
     * Alpha value to show an disabled arrow.
     */
    static Int32 ARROW_ALPHA_DISABLED;

    /**
     * Animation time to show a new candidate view and dismiss the old one.
     */
    static Int32 ANIMATION_TIME;

    /**
     * Listener used to notify IME that user clicks a candidate, or navigate
     * between them.
     */
    AutoPtr<ICandidateViewListener> mCvListener;

    /**
     * The left arrow button used to show previous page.
     */
    AutoPtr<IImageButton> mLeftArrowBtn;

    /**
     * The right arrow button used to show next page.
     */
    AutoPtr<IImageButton> mRightArrowBtn;

    /**
     * Decoding result to show.
     */
    AutoPtr<CPinyinIME::DecodingInfo> mDecInfo;

    /**
     * The animation view used to show candidates. It contains two views.
     * Normally, the candidates are shown one of them. When user navigates to
     * another page, animation effect will be performed.
     */
    AutoPtr<IViewFlipper> mFlipper;

    /**
     * The x offset of the flipper in this container.
     */
    Int32 mXOffsetForFlipper;

    /**
     * Animation used by the incoming view when the user navigates to a left
     * page.
     */
    AutoPtr<IAnimation> mInAnimPushLeft;

    /**
     * Animation used by the incoming view when the user navigates to a right
     * page.
     */
    AutoPtr<IAnimation> mInAnimPushRight;

    /**
     * Animation used by the incoming view when the user navigates to a page
     * above. If the page navigation is triggered by DOWN key, this animation is
     * used.
     */
    AutoPtr<IAnimation> mInAnimPushUp;

    /**
     * Animation used by the incoming view when the user navigates to a page
     * below. If the page navigation is triggered by UP key, this animation is
     * used.
     */
    AutoPtr<IAnimation> mInAnimPushDown;

    /**
     * Animation used by the outgoing view when the user navigates to a left
     * page.
     */
    AutoPtr<IAnimation> mOutAnimPushLeft;

    /**
     * Animation used by the outgoing view when the user navigates to a right
     * page.
     */
    AutoPtr<IAnimation> mOutAnimPushRight;

    /**
     * Animation used by the outgoing view when the user navigates to a page
     * above. If the page navigation is triggered by DOWN key, this animation is
     * used.
     */
    AutoPtr<IAnimation> mOutAnimPushUp;

    /**
     * Animation used by the incoming view when the user navigates to a page
     * below. If the page navigation is triggered by UP key, this animation is
     * used.
     */
    AutoPtr<IAnimation> mOutAnimPushDown;

    /**
     * Animation object which is used for the incoming view currently.
     */
    AutoPtr<IAnimation> mInAnimInUse;

    /**
     * Animation object which is used for the outgoing view currently.
     */
    AutoPtr<IAnimation> mOutAnimInUse;

    /**
     * Current page number in display.
     */
    Int32 mCurrentPage;

    AutoPtr<InnerListener> mListener;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INPUTMETHOD_PINYIN_CCANDIDATESCONTAINER_H__
