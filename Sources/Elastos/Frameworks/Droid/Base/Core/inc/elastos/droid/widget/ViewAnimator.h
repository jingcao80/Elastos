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

#ifndef __ELASTOS_DROID_WIDGET_VIEWANIMATOR_H__
#define __ELASTOS_DROID_WIDGET_VIEWANIMATOR_H__

#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * Base class for a {@link FrameLayout} container that will perform animations
 * when switching between its views.
 *
 * @attr ref android.R.styleable#ViewAnimator_inAnimation
 * @attr ref android.R.styleable#ViewAnimator_outAnimation
 * @attr ref android.R.styleable#ViewAnimator_animateFirstView
 */
class ECO_PUBLIC ViewAnimator
    : public FrameLayout
    , public IViewAnimator
{
public:
    CAR_INTERFACE_DECL()

    ViewAnimator();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Initialize this {@link ViewAnimator}, possibly setting
     * {@link #setMeasureAllChildren(Boolean)} based on {@link FrameLayout} flags.
     */


    /**
     * Sets which child view will be displayed.
     *
     * @param whichChild the index of the child view to display
     */
    CARAPI SetDisplayedChild(
        /* [in] */ Int32 whichChild);

    /**
     * Returns the index of the currently displayed child view.
     */
    CARAPI GetDisplayedChild(
        /* [in] */ Int32* child);

    /**
     * Manually shows the next child.
     */
    CARAPI ShowNext();

    /**
     * Manually shows the previous child.
     */
    CARAPI ShowPrevious();

    using FrameLayout::AddView;

    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI RemoveAllViews();

    CARAPI RemoveView(
        /* [in] */ IView* view);


    CARAPI RemoveViewAt(
        /* [in] */ Int32 index);

    CARAPI RemoveViewInLayout(
        /* [in] */ IView* view);

    CARAPI RemoveViews(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    CARAPI RemoveViewsInLayout(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    /**
     * Returns the View corresponding to the currently displayed child.
     *
     * @return The View currently displayed.
     *
     * @see #getDisplayedChild()
     */
    CARAPI GetCurrentView(
        /* [out] */ IView** curView);

    /**
     * Returns the current animation used to animate a View that enters the screen.
     *
     * @return An Animation or NULL if none is set.
     *
     * @see #setInAnimation(android.view.animation.Animation)
     * @see #setInAnimation(android.content.Context, Int32)
     */
    CARAPI GetInAnimation(
        /* [out] */ IAnimation** anim);

    /**
     * Specifies the animation used to animate a View that enters the screen.
     *
     * @param inAnimation The animation started when a View enters the screen.
     *
     * @see #getInAnimation()
     * @see #setInAnimation(android.content.Context, Int32)
     */
    CARAPI SetInAnimation(
        /* [in] */ IAnimation* inAnimation);

    /**
     * Returns the current animation used to animate a View that exits the screen.
     *
     * @return An Animation or NULL if none is set.
     *
     * @see #setOutAnimation(android.view.animation.Animation)
     * @see #setOutAnimation(android.content.Context, Int32)
     */
    CARAPI GetOutAnimation(
        /* [out] */ IAnimation** anim);

    /**
     * Specifies the animation used to animate a View that exit the screen.
     *
     * @param outAnimation The animation started when a View exit the screen.
     *
     * @see #getOutAnimation()
     * @see #setOutAnimation(android.content.Context, Int32)
     */
    CARAPI SetOutAnimation(
        /* [in] */ IAnimation* inAnimation);

    /**
     * Specifies the animation used to animate a View that enters the screen.
     *
     * @param context The application's environment.
     * @param resourceID The resource id of the animation.
     *
     * @see #getInAnimation()
     * @see #setInAnimation(android.view.animation.Animation)
     */
    CARAPI SetInAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceID);

    /**
     * Specifies the animation used to animate a View that exit the screen.
     *
     * @param context The application's environment.
     * @param resourceID The resource id of the animation.
     *
     * @see #getOutAnimation()
     * @see #setOutAnimation(android.view.animation.Animation)
     */
    CARAPI SetOutAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceID);
    /**
     * Returns whether the current View should be animated the first time the ViewAnimator
     * is displayed.
     *
     * @return TRUE if the current View will be animated the first time it is displayed,
     * false otherwise.
     *
     * @see #setAnimateFirstView(Boolean)
     */
    CARAPI GetAnimateFirstView(
        /* [out] */ Boolean* animate);

    /**
     * Indicates whether the current View should be animated the first time
     * the ViewAnimator is displayed.
     *
     * @param animate True to animate the current View the first time it is displayed,
     *                false otherwise.
     */
    CARAPI SetAnimateFirstView(
        /* [in] */ Boolean animate);

    CARAPI GetBaseline(
        /* [out] */ Int32* baseline);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    /**
     * Shows only the specified child. The other displays Views exit the screen,
     * optionally with the with the {@link #getOutAnimation() out animation} and
     * the specified child enters the screen, optionally with the
     * {@link #getInAnimation() in animation}.
     *
     * @param childIndex The index of the child to be shown.
     * @param animate Whether or not to use the in and out animations, defaults
     *            to TRUE.
     */
    CARAPI ShowOnly(
        /* [in] */ Int32 childIndex,
        /* [in] */ Boolean animate);

    /**
     * Shows only the specified child. The other displays Views exit the screen
     * with the {@link #getOutAnimation() out animation} and the specified child
     * enters the screen with the {@link #getInAnimation() in animation}.
     *
     * @param childIndex The index of the child to be shown.
     */
    CARAPI ShowOnly(
        /* [in] */ Int32 childIndex);

private:
    CARAPI_(void) InitViewAnimator(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    Int32 mWhichChild;
    Boolean mFirstTime;
    Boolean mAnimateFirstTime;

    AutoPtr<IAnimation> mInAnimation;
    AutoPtr<IAnimation> mOutAnimation;
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_VIEWANIMATOR_H__
