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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWTHUMBNAIL_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWTHUMBNAIL_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/view/View.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapShader;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::ILightingColorFilter;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

class Task;

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
using Elastos::Droid::SystemUI::Recents::Model::Task;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/** The task thumbnail view */
class TaskViewThumbnail
    : public Elastos::Droid::View::View
    , public ITaskViewThumbnail
{
private:
    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        AnimatorUpdateListener(
            /* [in] */ TaskViewThumbnail* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        TaskViewThumbnail* mHost;
    };

    class MyAnimatorListenerAdapter : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ IRunnable* postAnimRunnable);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    public:
        Boolean mCancelled;

    private:
        AutoPtr<IRunnable> mPostAnimRunnable;
    };

public:
    TaskViewThumbnail();

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

    /** Updates the clip rect based on the given task bar. */
    CARAPI_(void) EnableTaskBarClip(
        /* [in] */ IView* taskBar);

    CARAPI_(void) UpdateVisibility(
        /* [in] */ Int32 clipBottom);

    /** Binds the thumbnail view to the screenshot. */
    CARAPI_(Boolean) BindToScreenshot(
        /* [in] */ IBitmap* ss);

    /** Unbinds the thumbnail view from the screenshot. */
    CARAPI_(void) UnbindFromScreenshot();

    /** Binds the thumbnail view to the task */
    CARAPI RebindToTask(
        /* [in] */ Task* t);

    CARAPI SetImageBitmap(
        /* [in] */ IBitmap* bm);

    CARAPI SetDimAlpha(
        /* [in] */ Float dimAlpha);

    /** Unbinds the thumbnail view from the task */
    CARAPI_(void) UnbindFromTask();

    using View::OnFocusChanged;

    /** Handles focus changes. */
    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean focused);

    /** Prepares for the enter recents animation. */
    CARAPI_(void) PrepareEnterRecentsAnimation(
        /* [in] */ Boolean isTaskViewLaunchTargetTask);

    /** Animates this task thumbnail as it enters recents */
    CARAPI_(void) StartEnterRecentsAnimation(
        /* [in] */ Int32 delay,
        /* [in] */ IRunnable* postAnimRunnable);

    /** Animates this task thumbnail as it exits recents */
    CARAPI_(void) StartLaunchTaskAnimation(
        /* [in] */ IRunnable* postAnimRunnable);

    /** Animates the thumbnail alpha. */
    CARAPI_(void) StartFadeAnimation(
        /* [in] */ Float finalAlpha,
        /* [in] */ Int32 delay,
        /* [in] */ Int32 duration,
        /* [in] */ IRunnable* postAnimRunnable);

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    CARAPI_(void) UpdateFilter();

    CARAPI_(void) UpdateBitmapScale();

public:
    AutoPtr<RecentsConfiguration> mConfig;

    // Task bar clipping
    AutoPtr<IRect> mClipRect;
    AutoPtr<IPaint> mDrawPaint;
    AutoPtr<ILightingColorFilter> mLightingColorFilter;

private:
    Int32 mCornerRadius;
    AutoPtr<IMatrix> mScaleMatrix;
    AutoPtr<IRectF> mBitmapRect;
    AutoPtr<IRectF> mLayoutRect;
    AutoPtr<IBitmapShader> mBitmapShader;
    Float mBitmapAlpha;
    Float mDimAlpha;
    AutoPtr<IView> mTaskBar;
    Boolean mInvisible;
    AutoPtr<IValueAnimator> mAlphaAnimator;
    AutoPtr<IAnimatorUpdateListener> mAlphaUpdateListener;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWTHUMBNAIL_H__
