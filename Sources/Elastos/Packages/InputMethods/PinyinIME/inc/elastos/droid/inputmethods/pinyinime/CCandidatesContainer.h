
#ifndef __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CCANDIDATESCONTAINER_H__
#define __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CCANDIDATESCONTAINER_H__

#include "_Elastos_Droid_Inputmethods_PinyinIME_CCandidatesContainer.h"
#include "elastos/droid/widget/RelativeLayout.h"

using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::IViewFlipper;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

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
    , public Elastos::Droid::Widget::RelativeLayout
    , public ICandidatesContainer
{
public:
    CandidatesContainer();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI OnAnimationStart(
        /* [in] */  IAnimation* animation);

    CARAPI OnAnimationEnd(
        /* [in] */  IAnimation* animation);

    CARAPI OnAnimationRepeat(
        /* [in] */  IAnimation* animation);

    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetIgnoreGravity(
        /* [in] */ Int32 viewId);

    CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    CARAPI SetHorizontalGravity(
        /* [in] */ Int32 horizontalGravity);

    CARAPI SetVerticalGravity(
        /* [in] */ Int32 verticalGravity);

    CARAPI Initialize(
        /* [in] */ ICandidateViewListener* cvListener,
        /* [in] */ IBalloonHint* balloonHint,
        /* [in] */ IGestureDetector* gestureDetector);

    CARAPI ShowCandidates(
        /* [in] */ IDecodingInfo* decInfo,
        /* [in] */ Boolean enableActiveHighlight);

    CARAPI GetCurrentPage(
        /* [out] */ Int32* page);

    CARAPI EnableActiveHighlight(
        /* [in] */ Boolean enableActiveHighlight);

    CARAPI ActiveCurseBackward(
        /* [out] */ Boolean* result);

    CARAPI ActiveCurseForward(
        /* [out] */ Boolean* result);

    CARAPI PageBackward(
        /* [in] */ Boolean animLeftRight,
        /* [in] */ Boolean enableActiveHighlight,
        /* [out] */ Boolean* result);

    CARAPI PageForward(
        /* [in] */ Boolean animLeftRight,
        /* [in] */ Boolean enableActiveHighlight,
        /* [out] */ Boolean* result);

    CARAPI GetActiveCandiatePos(
        /* [out] */ Int32* pos);

    CARAPI UpdateArrowStatus();

    // The reason why we handle candiate view's touch events here is because
    // that the view under the focused view may get touch events instead of the
    // focused one.
    Boolean OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI LoadAnimation(
        /* [in] */ Boolean animLeftRight,
        /* [in] */ Boolean forward);

protected:
    void OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    void EnableArrow(
        /* [in] */ IImageButton* arrowBtn,
        /* [in] */ Boolean enabled);

    void ShowArrow(
        /* [in] */ IImageButton* arrowBtn,
        /* [in] */ Boolean show);

    AutoPtr<IAnimation> CreateAnimation(
        /* [in] */ Float xFrom,
        /* [in] */ Float xTo,
        /* [in] */ Float yFrom,
        /* [in] */ Float yTo,
        /* [in] */ Float alphaFrom,
        /* [in] */ Float alphaTo,
        /* [in] */ Int64 duration);

    void StartAnimation();

    void StopAnimation();

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
    AutoPtr<IDecodingInfo> mDecInfo;

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
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CCANDIDATESCONTAINER_H__
