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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWHEADER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWHEADER_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/view/ViewOutlineProvider.h>
#include <elastos/droid/widget/FrameLayout.h>
#include "Elastos.Droid.Animation.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IOutline;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPorterDuffColorFilter;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IGradientDrawable;
using Elastos::Droid::Graphics::Drawable::IRippleDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewOutlineProvider;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::SystemUI::Recents::Model::ITask;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/* The task bar view */
class TaskViewHeader
    : public FrameLayout
    , public ITaskViewHeader
{
private:
    class OutlineProvider : public ViewOutlineProvider
    {
    public:
        OutlineProvider(
            /* [in] */ TaskViewHeader* host);

        // @Override
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);

    private:
        TaskViewHeader* mHost;
    };

    class MyAnimatorListenerAdapter : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ TaskViewHeader* host);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        TaskViewHeader* mHost;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        AnimatorUpdateListener(
            /* [in] */ TaskViewHeader* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        TaskViewHeader* mHost;
    };

public:
    TaskViewHeader();

    CAR_INTERFACE_DECL()

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

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /** Sets whether the current task is full screen or not. */
    CARAPI SetIsFullscreen(
        /* [in] */ Boolean isFullscreen);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

    /** Returns the secondary color for a primary color. */
    CARAPI_(Int32) GetSecondaryColor(
        /* [in] */ Int32 primaryColor,
        /* [in] */ Boolean useLightOverlayColor);

    /** Binds the bar view to the task */
    CARAPI RebindToTask(
        /* [in] */ ITask* t);

    /** Unbinds the bar view from the task */
    CARAPI UnbindFromTask();

    /** Animates this task bar dismiss button when launching a task. */
    CARAPI StartLaunchTaskDismissAnimation();

    /** Animates this task bar if the user does not interact with the stack after a certain time. */
    CARAPI StartNoUserInteractionAnimation();

    /** Mark this task view that the user does has not interacted with the stack after a certain time. */
    CARAPI SetNoUserInteractionState();

    /** Notifies the associated TaskView has been focused. */
    CARAPI OnTaskViewFocusChanged(
        /* [in] */ Boolean focused);

    CARAPI SetDimAlpha(
        /* [in] */ Int32 alpha);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);
    // @Override
    CARAPI OnCreateDrawableState(
        /* [in] */ Int32 extraSpace,
        /* [out, callee] */ ArrayOf<Int32>** drawableState);

public:
    AutoPtr<RecentsConfiguration> mConfig;

    AutoPtr<IImageView> mDismissButton;
    AutoPtr<IImageView> mApplicationIcon;
    AutoPtr<ITextView> mActivityDescription;

    AutoPtr<IRippleDrawable> mBackground;
    AutoPtr<IGradientDrawable> mBackgroundColorDrawable;
    Int32 mBackgroundColor;
    AutoPtr<IDrawable> mLightDismissDrawable;
    AutoPtr<IDrawable> mDarkDismissDrawable;
    AutoPtr<IAnimatorSet> mFocusAnimator;
    AutoPtr<IValueAnimator> backgroundColorAnimator;
    AutoPtr<IPorterDuffColorFilter> mDimFilter;

    Boolean mIsFullscreen;
    Boolean mCurrentPrimaryColorIsDark;
    Int32 mCurrentPrimaryColor;

    static AutoPtr<IPaint> sHighlightPaint;

private:
    AutoPtr<IPaint> mDimPaint;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWHEADER_H__
