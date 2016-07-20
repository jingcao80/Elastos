
#include "elastos/droid/systemui/recents/views/TaskView.h"
#include "elastos/droid/systemui/recents/views/CFakeShadowDrawable.h"
#include "elastos/droid/systemui/recents/views/CAnimateableViewBounds.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include "elastos/droid/systemui/recents/AlternateRecentsComponent.h"
#include "elastos/droid/systemui/recents/model/RecentsTaskLoader.h"
#include "../R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::CPorterDuffColorFilter;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::PorterDuffMode_MULTIPLY;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::SystemUI::Recents::Model::RecentsTaskLoader;
using Elastos::Droid::SystemUI::Recents::Model::EIID_ITaskCallbacks;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

static const String TAG("TaskView");

//======================================================================
// TaskView::Wrapper
//======================================================================
CAR_INTERFACE_IMPL_4(TaskView::Wrapper, Object, ITaskCallbacks,
    ITaskFooterViewCallbacks, IViewOnClickListener, IViewOnLongClickListener)

TaskView::Wrapper::Wrapper(
    /* [in] */ TaskView* host)
    : mHost(host)
{}

ECode TaskView::Wrapper::OnTaskDataLoaded()
{
    return mHost->OnTaskDataLoaded();
}

ECode TaskView::Wrapper::OnTaskDataUnloaded()
{
    return mHost->OnTaskDataUnloaded();
}

ECode TaskView::Wrapper::OnTaskFooterHeightChanged(
    /* [in] */ Int32 height,
    /* [in] */ Int32 maxHeight)
{
    return mHost->OnTaskFooterHeightChanged(height, maxHeight);
}

ECode TaskView::Wrapper::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

ECode TaskView::Wrapper::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    return mHost->OnLongClick(v, result);
}

//======================================================================
// TaskView::TaskProgressAnimatorUpdateListener
//======================================================================
CAR_INTERFACE_IMPL(TaskView::TaskProgressAnimatorUpdateListener, Object, IAnimatorUpdateListener)

TaskView::TaskProgressAnimatorUpdateListener::TaskProgressAnimatorUpdateListener(
    /* [in] */ TaskView* host)
    : mHost(host)
{}

ECode TaskView::TaskProgressAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    Float f;
    IFloat::Probe(value)->GetValue(&f);
    mHost->SetTaskProgress(f);
    return NOERROR;
}

//======================================================================
// TaskView::ClipTopAnimatorUpdateListener
//======================================================================
CAR_INTERFACE_IMPL(TaskView::ClipTopAnimatorUpdateListener, Object, IAnimatorUpdateListener)

TaskView::ClipTopAnimatorUpdateListener::ClipTopAnimatorUpdateListener(
    /* [in] */ TaskView* host)
    : mHost(host)
{}

ECode TaskView::ClipTopAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    Int32 y;
    IInteger32::Probe(value)->GetValue(&y);
    mHost->mHeaderView->SetTranslationY(y);
    return NOERROR;
}

//======================================================================
// TaskView::MyViewOutlineProvider
//======================================================================
TaskView::MyViewOutlineProvider::MyViewOutlineProvider(
    /* [in] */ TaskView* host)
    : mHost(host)
{}

ECode TaskView::MyViewOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    // Set the outline to match the FAB background
    Int32 w, h;
    mHost->mActionButtonView->GetWidth(&w);
    mHost->mActionButtonView->GetHeight(&h);
    outline->SetOval(0, 0, w, h);
    return NOERROR;
}

//======================================================================
// TaskView::EndLaunchedFromAppWithScreenshotRunnable
//======================================================================
TaskView::EndLaunchedFromAppWithScreenshotRunnable::EndLaunchedFromAppWithScreenshotRunnable(
    /* [in] */ TaskView* host,
    /* [in] */ ViewAnimation::TaskViewEnterContext* ctx)
    : mHost(host)
    , mCtx(ctx)
{}

ECode TaskView::EndLaunchedFromAppWithScreenshotRunnable::Run()
{
    mHost->SetIsFullScreen(FALSE);
    mHost->RequestLayout();

    // Reset the clip
    mHost->mViewBounds->SetClipTop(0);
    mHost->mViewBounds->SetClipBottom(0);
    mHost->mViewBounds->SetClipRight(0);
    // Reset the bar translation
    mHost->mHeaderView->SetTranslationY(0);
    // Animate the footer into view (if it is the front most task)
    mHost->AnimateFooterVisibility(TRUE, mHost->mConfig->mTaskBarEnterAnimDuration);

    // Unbind the thumbnail from the screenshot
    RecentsTaskLoader::GetInstance()->LoadTaskData(mHost->mTask);
    // Recycle the full screen screenshot
    AlternateRecentsComponent::ConsumeLastScreenshot();

    mHost->mCb->OnTaskViewFullScreenTransitionCompleted();

    // Decrement the post animation trigger
    mCtx->mPostAnimationTrigger->Decrement();
    return NOERROR;
}

//======================================================================
// TaskView::AnimateDimRunnable
//======================================================================
TaskView::AnimateDimRunnable::AnimateDimRunnable(
    /* [in] */ TaskView* host,
    /* [in] */ ViewAnimation::TaskViewEnterContext* ctx)
    : mHost(host)
    , mCtx(ctx)
{}

ECode TaskView::AnimateDimRunnable::Run()
{
    mHost->AnimateDimToProgress(0,
        mHost->mConfig->mTaskBarEnterAnimDuration,
        mCtx->mPostAnimationTrigger->DecrementOnAnimationEnd());
    return NOERROR;
}

//======================================================================
// TaskView::EndLaunchedFromAppWithThumbnailRunnable
//======================================================================
TaskView::EndLaunchedFromAppWithThumbnailRunnable::EndLaunchedFromAppWithThumbnailRunnable(
    /* [in] */ ViewAnimation::TaskViewEnterContext* ctx)
    : mCtx(ctx)
{}

ECode TaskView::EndLaunchedFromAppWithThumbnailRunnable::Run()
{
    // Decrement the post animation trigger
    mCtx->mPostAnimationTrigger->Decrement();
    return NOERROR;
}

//======================================================================
// TaskView::EnableFocusAnimationsDimRunnable
//======================================================================
TaskView::EnableFocusAnimationsDimRunnable::EnableFocusAnimationsDimRunnable(
    /* [in] */ TaskView* host)
    : mHost(host)
{}

ECode TaskView::EnableFocusAnimationsDimRunnable::Run()
{
    mHost->EnableFocusAnimations();
    return NOERROR;
}

//======================================================================
// TaskView::EndDeleteTaskAnimationRunnable
//======================================================================
TaskView::EndDeleteTaskAnimationRunnable::EndDeleteTaskAnimationRunnable(
    /* [in] */ TaskView* host,
    /* [in] */ IRunnable* r)
    : mHost(host)
    , mR(r)
{}

ECode TaskView::EndDeleteTaskAnimationRunnable::Run()
{
    // We just throw this into a runnable because starting a view property
    // animation using layers can cause inconsisten results if we try and
    // update the layers while the animation is running.  In some cases,
    // the runnabled passed in may start an animation which also uses layers
    // so we defer all this by posting this.
    mR->Run();

    // Re-enable clipping with the stack (we will reuse this view)
    mHost->SetClipViewInStack(TRUE);
    return NOERROR;
}

//======================================================================
// TaskView::DismissTaskRunnable
//======================================================================
TaskView::DismissTaskRunnable::DismissTaskRunnable(
    /* [in] */ TaskView* host)
    : mHost(host)
{}

ECode TaskView::DismissTaskRunnable::Run()
{
    AutoPtr<TaskView> tv = mHost;
    mHost->mCb->OnTaskViewDismissed(tv);
    return NOERROR;
}

//======================================================================
// TaskView::OnClickRunnable
//======================================================================
TaskView::OnClickRunnable::OnClickRunnable(
    /* [in] */ TaskView* host,
    /* [in] */ IView* v)
    : mHost(host)
    , mV(v)
{}

ECode TaskView::OnClickRunnable::Run()
{
    AutoPtr<TaskView> tv = mHost;
    if (Constants::DebugFlags::App::EnableTaskFiltering
        && mV.Get() == IView::Probe(mHost->mHeaderView->mApplicationIcon)) {
        mHost->mCb->OnTaskViewAppIconClicked(tv);
    }
    else if (mV.Get() == IView::Probe(mHost->mHeaderView->mDismissButton)) {
        mHost->DismissTask();
    }
    return NOERROR;
}

//======================================================================
// TaskView
//======================================================================
CAR_INTERFACE_IMPL(TaskView, FrameLayout, ITaskView)

TaskView::TaskView()
    : mTaskProgress(0.0f)
    , mMaxDimScale(0.0f)
    , mDim(0)
    , mTaskDataLoaded(FALSE)
    , mIsFocused(FALSE)
    , mFocusAnimationsEnabled(FALSE)
    , mIsFullScreenView(FALSE)
    , mClipViewInStack(FALSE)
{
}

ECode TaskView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode TaskView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode TaskView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode TaskView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes));

    CAccelerateInterpolator::New(1.0f, (IAccelerateInterpolator**)&mDimInterpolator);
    CPorterDuffColorFilter::New(0, PorterDuffMode_MULTIPLY, (IPorterDuffColorFilter**)&mDimColorFilter);
    CPaint::New((IPaint**)&mLayerPaint);
    mUpdateDimListener = new TaskProgressAnimatorUpdateListener(this);
    mWrapper = new Wrapper(this);

    mConfig = RecentsConfiguration::GetInstance();
    mMaxDimScale = mConfig->mTaskStackMaxDim / 255.0f;
    mClipViewInStack = TRUE;
    AutoPtr<IAnimateableViewBounds> avb;
    CAnimateableViewBounds::New(this, mConfig->mTaskViewRoundedCornerRadiusPx,
        (IAnimateableViewBounds**)&avb);
    mViewBounds = (AnimateableViewBounds*)avb.Get();
    SetTaskProgress(GetTaskProgress());
    SetDim(GetDim());
    if (mConfig->mFakeShadows) {
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        AutoPtr<IDrawable> drawable;
        CFakeShadowDrawable::New(res, mConfig.Get(), (IDrawable**)&drawable);
        SetBackground(drawable);
    }
    SetOutlineProvider(mViewBounds);
    return NOERROR;
}

/** Set callback */
void TaskView::SetCallbacks(
    /* [in] */ ITaskViewCallbacks* cb)
{
    mCb = cb;
}

/** Gets the task */
AutoPtr<Task> TaskView::GetTask()
{
    return mTask;
}

/** Returns the view bounds. */
AutoPtr<AnimateableViewBounds> TaskView::GetViewBounds()
{
    return mViewBounds;
}

// @Override
ECode TaskView::OnFinishInflate()
{
    // Bind the views
    mContent = NULL;
    FindViewById(R::id::task_view_content, (IView**)&mContent);
    AutoPtr<IView> view;
    FindViewById(R::id::task_view_bar, (IView**)&view);
    mHeaderView = (TaskViewHeader*)ITaskViewHeader::Probe(view);
    view = NULL;
    FindViewById(R::id::task_view_thumbnail, (IView**)&view);
    mThumbnailView = (TaskViewThumbnail*)ITaskViewThumbnail::Probe(view);
    mThumbnailView->EnableTaskBarClip(mHeaderView);
    mActionButtonView = NULL;
    FindViewById(R::id::lock_to_app_fab, (IView**)&mActionButtonView);
    AutoPtr<ViewOutlineProvider> provider = new MyViewOutlineProvider(this);
    mActionButtonView->SetOutlineProvider(provider);
    if (mFooterView != NULL) {
        mFooterView->SetCallbacks(mWrapper);
    }
    return NOERROR;
}

// @Override
ECode TaskView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width = MeasureSpec::GetSize(widthMeasureSpec);
    Int32 height = MeasureSpec::GetSize(heightMeasureSpec);

    Int32 widthWithoutPadding = width - mPaddingLeft - mPaddingRight;
    Int32 heightWithoutPadding = height - mPaddingTop - mPaddingBottom;

    // Measure the content
    mContent->Measure(
        MeasureSpec::MakeMeasureSpec(widthWithoutPadding, MeasureSpec::EXACTLY),
        MeasureSpec::MakeMeasureSpec(widthWithoutPadding, MeasureSpec::EXACTLY));

    // Measure the bar view, thumbnail, and footer
    mHeaderView->Measure(
        MeasureSpec::MakeMeasureSpec(widthWithoutPadding, MeasureSpec::EXACTLY),
        MeasureSpec::MakeMeasureSpec(mConfig->mTaskBarHeight, MeasureSpec::EXACTLY));
    if (mFooterView != NULL) {
        mFooterView->Measure(
            MeasureSpec::MakeMeasureSpec(widthWithoutPadding, MeasureSpec::EXACTLY),
            MeasureSpec::MakeMeasureSpec(mConfig->mTaskViewLockToAppButtonHeight, MeasureSpec::EXACTLY));
    }
    mActionButtonView->Measure(
        MeasureSpec::MakeMeasureSpec(widthWithoutPadding, MeasureSpec::AT_MOST),
        MeasureSpec::MakeMeasureSpec(heightWithoutPadding, MeasureSpec::AT_MOST));
    if (mIsFullScreenView) {
        // Measure the thumbnail height to be the full dimensions
        mThumbnailView->Measure(
            MeasureSpec::MakeMeasureSpec(widthWithoutPadding, MeasureSpec::EXACTLY),
            MeasureSpec::MakeMeasureSpec(heightWithoutPadding, MeasureSpec::EXACTLY));
    }
    else {
        // Measure the thumbnail to be square
        mThumbnailView->Measure(
            MeasureSpec::MakeMeasureSpec(widthWithoutPadding, MeasureSpec::EXACTLY),
            MeasureSpec::MakeMeasureSpec(widthWithoutPadding, MeasureSpec::EXACTLY));
    }
    SetMeasuredDimension(width, height);
    InvalidateOutline();
    return NOERROR;
}

/** Synchronizes this view's properties with the task's transform */
void TaskView::UpdateViewPropertiesToTaskTransform(
    /* [in] */ TaskViewTransform* toTransform,
    /* [in] */ Int32 duration)
{
    UpdateViewPropertiesToTaskTransform(toTransform, duration, NULL);
}

void TaskView::UpdateViewPropertiesToTaskTransform(
    /* [in] */ TaskViewTransform* toTransform,
    /* [in] */ Int32 duration,
    /* [in] */ IAnimatorUpdateListener* updateCallback)
{
    // If we are a full screen view, then only update the Z to keep it in order
    // XXX: Also update/animate the dim as well
    if (mIsFullScreenView) {
        Float z;
        if (!mConfig->mFakeShadows &&
            toTransform->HasTranslationZChangedFrom((GetTranslationZ(&z), z))) {
            SetTranslationZ(toTransform->mTranslationZ);
        }
        return;
    }

    // Apply the transform
    toTransform->ApplyToTaskView(this, duration,
        IInterpolator::Probe(mConfig->mFastOutSlowInInterpolator), FALSE,
        !mConfig->mFakeShadows, updateCallback);

    // Update the task progress
    if (mTaskProgressAnimator != NULL) {
        mTaskProgressAnimator->RemoveAllListeners();
        mTaskProgressAnimator->Cancel();
    }
    if (duration <= 0) {
        SetTaskProgress(toTransform->mP);
    }
    else {
         AutoPtr<IObjectAnimatorHelper> oaHelper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelper);
        AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
        (*values)[0] = toTransform->mP;
        AutoPtr<IObjectAnimator> animator;
        oaHelper->OfFloat(TO_IINTERFACE(this), String("taskProgress"), values,
            (IObjectAnimator**)&animator);
        mTaskProgressAnimator = IAnimator::Probe(animator);
        mTaskProgressAnimator->SetDuration(duration);
        IValueAnimator::Probe(mTaskProgressAnimator)->AddUpdateListener(mUpdateDimListener);
        mTaskProgressAnimator->Start();
    }
}

/** Resets this view's properties */
void TaskView::ResetViewProperties()
{
    SetDim(0);
    TaskViewTransform::Reset(this);
}

/**
 * When we are un/filtering, this method will set up the transform that we are animating to,
 * in order to hide the task.
 */
void TaskView::PrepareTaskTransformForFilterTaskHidden(
    /* [in] */ TaskViewTransform* toTransform)
{
    // Fade the view out and slide it away
    toTransform->mAlpha = 0.0f;
    toTransform->mTranslationY += 200;
    toTransform->mTranslationZ = 0;
}

/**
 * When we are un/filtering, this method will setup the transform that we are animating from,
 * in order to show the task.
 */
void TaskView::PrepareTaskTransformForFilterTaskVisible(
    /* [in] */ TaskViewTransform* fromTransform)
{
    // Fade the view in
    fromTransform->mAlpha = 0.0f;
}

/** Prepares this task view for the enter-recents animations.  This is called earlier in the
 * first layout because the actual animation into recents may take a long time. */
void TaskView::PrepareEnterRecentsAnimation(
    /* [in] */ Boolean isTaskViewLaunchTargetTask,
    /* [in] */ Boolean occludesLaunchTarget,
    /* [in] */ Int32 offscreenY)
{
    Int32 initialDim = GetDim();
    if (mConfig->mLaunchedFromAppWithScreenshot) {
        if (isTaskViewLaunchTargetTask) {
            // Hide the footer during the transition in, and animate it out afterwards?
            if (mFooterView != NULL) {
                mFooterView->AnimateFooterVisibility(FALSE, 0);
            }
        }
        else {
            // Don't do anything for the side views when animating in
        }
    }
    else if (mConfig->mLaunchedFromAppWithThumbnail) {
        if (isTaskViewLaunchTargetTask) {
            // Hide the action button if it exists
            mActionButtonView->SetAlpha(0.0f);
            // Set the dim to 0 so we can animate it in
            initialDim = 0;
        }
        else if (occludesLaunchTarget) {
            // Move the task view off screen (below) so we can animate it in
            SetTranslationY(offscreenY);
        }

    }
    else if (mConfig->mLaunchedFromHome) {
        // Move the task view off screen (below) so we can animate it in
        SetTranslationY(offscreenY);
        SetTranslationZ(0.0f);
        SetScaleX(1.0f);
        SetScaleY(1.0f);
    }
    // Apply the current dim
    SetDim(initialDim);
    // Prepare the thumbnail view alpha
    mThumbnailView->PrepareEnterRecentsAnimation(isTaskViewLaunchTargetTask);
}

/** Animates this task view as it enters recents */
void TaskView::StartEnterRecentsAnimation(
    /* [in] */ ViewAnimation::TaskViewEnterContext* ctx)
{
    TaskViewTransform* transform = ctx->mCurrentTaskTransform;
    Int32 startDelay = 0;

    if (mConfig->mLaunchedFromAppWithScreenshot) {
        if (mTask->mIsLaunchTarget) {
            AutoPtr<IRect> taskRect = ctx->mCurrentTaskRect;
            Int32 duration = mConfig->mTaskViewEnterFromHomeDuration * 10;
            Int32 windowInsetTop;
            mConfig->mSystemInsets->GetTop(&windowInsetTop); // XXX: Should be for the window
            Int32 width, measuredWidth, measuredHeight;
            taskRect->GetWidth(&width);
            GetMeasuredWidth(&measuredWidth);
            GetMeasuredHeight(&measuredHeight);
            Float taskScale = ((Float)width / measuredWidth) * transform->mScale;
            Float scaledYOffset = ((1.0f - taskScale) * measuredHeight) / 2;
            Float scaledWindowInsetTop = (Int32) (taskScale * windowInsetTop);
            Int32 top;
            taskRect->GetTop(&top);
            Float scaledTranslationY = top + transform->mTranslationY -
                    (scaledWindowInsetTop + scaledYOffset);
            startDelay = mConfig->mTaskViewEnterFromHomeStaggerDelay;

            // Animate the top clip
            AutoPtr<IAnimatorUpdateListener> listener = new ClipTopAnimatorUpdateListener(this);
            mViewBounds->AnimateClipTop(windowInsetTop, duration, listener);
            // Animate the bottom or right clip
            Int32 size = Elastos::Core::Math::Round(width / taskScale);
            if (mConfig->HasHorizontalLayout()) {
                mViewBounds->AnimateClipRight(measuredWidth - size, duration);
            }
            else {
                mViewBounds->AnimateClipBottom(measuredHeight - (windowInsetTop + size), duration);
            }
            // Animate the task bar of the first task view
            AutoPtr<Runnable> runnable = new EndLaunchedFromAppWithScreenshotRunnable(this, ctx);
            AutoPtr<IViewPropertyAnimator> animator;
            Animate((IViewPropertyAnimator**)&animator);
            animator->ScaleX(taskScale);
            animator->ScaleY(taskScale);
            animator->TranslationY(scaledTranslationY);
            animator->SetDuration(duration);
            animator->WithEndAction(runnable);
            animator->Start();
        }
        else {
            // Animate the footer into view
            AnimateFooterVisibility(TRUE, 0);
        }
        ctx->mPostAnimationTrigger->Increment();
    }
    else if (mConfig->mLaunchedFromAppWithThumbnail) {
        if (mTask->mIsLaunchTarget) {
            // Animate the dim/overlay
            if (Constants::DebugFlags::App::EnableThumbnailAlphaOnFrontmost) {
                // Animate the thumbnail alpha before the dim animation (to prevent updating the
                // hardware layer)
                AutoPtr<Runnable> runnable = new AnimateDimRunnable(this, ctx);
                mThumbnailView->StartEnterRecentsAnimation(mConfig->mTaskBarEnterAnimDelay, runnable);
            }
            else {
                // Immediately start the dim animation
                AnimateDimToProgress(mConfig->mTaskBarEnterAnimDelay,
                    mConfig->mTaskBarEnterAnimDuration,
                    ctx->mPostAnimationTrigger->DecrementOnAnimationEnd());
            }
            ctx->mPostAnimationTrigger->Increment();

            // Animate the footer into view
            AnimateFooterVisibility(TRUE, mConfig->mTaskBarEnterAnimDuration);

            // Animate the action button in
            AutoPtr<IViewPropertyAnimator> animator;
            mActionButtonView->Animate((IViewPropertyAnimator**)&animator);
            animator->Alpha(1.0f);
            animator->SetStartDelay(mConfig->mTaskBarEnterAnimDelay);
            animator->SetDuration(mConfig->mTaskBarEnterAnimDuration);
            animator->SetInterpolator(mConfig->mFastOutLinearInInterpolator);
            animator->WithLayer();
            animator->Start();
        }
        else {
            // Animate the task up if it was occluding the launch target
            if (ctx->mCurrentTaskOccludesLaunchTarget) {
                SetTranslationY(transform->mTranslationY + mConfig->mTaskViewAffiliateGroupEnterOffsetPx);
                SetAlpha(0.0f);
                AutoPtr<IViewPropertyAnimator> animator;
                Animate((IViewPropertyAnimator**)&animator);
                animator->Alpha(1.0f);
                animator->TranslationY(transform->mTranslationY);
                animator->SetStartDelay(mConfig->mTaskBarEnterAnimDelay);
                animator->SetUpdateListener(NULL);
                animator->SetInterpolator(mConfig->mFastOutSlowInInterpolator);
                animator->SetDuration(mConfig->mTaskViewEnterFromHomeDuration);
                AutoPtr<Runnable> runnable = new EndLaunchedFromAppWithThumbnailRunnable(ctx);
                animator->WithEndAction(runnable);
                animator->Start();
                ctx->mPostAnimationTrigger->Increment();
            }
        }
        startDelay = mConfig->mTaskBarEnterAnimDelay;
    }
    else if (mConfig->mLaunchedFromHome) {
        // Animate the tasks up
        Int32 frontIndex = (ctx->mCurrentStackViewCount - ctx->mCurrentStackViewIndex - 1);
        Int32 delay = mConfig->mTaskViewEnterFromHomeDelay +
                frontIndex * mConfig->mTaskViewEnterFromHomeStaggerDelay;

        SetScaleX(transform->mScale);
        SetScaleY(transform->mScale);
        AutoPtr<IViewPropertyAnimator> animator;
        Animate((IViewPropertyAnimator**)&animator);
        if (!mConfig->mFakeShadows) {
            animator->TranslationZ(transform->mTranslationZ);
        }
        animator->TranslationY(transform->mTranslationY);
        animator->SetStartDelay(delay);
        animator->SetUpdateListener(ctx->mUpdateListener);
        animator->SetInterpolator(mConfig->mQuintOutInterpolator);
        animator->SetDuration(mConfig->mTaskViewEnterFromHomeDuration
            + frontIndex * mConfig->mTaskViewEnterFromHomeStaggerDelay);
        AutoPtr<Runnable> runnable = new EndLaunchedFromAppWithThumbnailRunnable(ctx);
        animator->WithEndAction(runnable);
        animator->Start();
        ctx->mPostAnimationTrigger->Increment();

        // Animate the footer into view
        AnimateFooterVisibility(TRUE, mConfig->mTaskViewEnterFromHomeDuration);
        startDelay = delay;

    }
    else {
        // Animate the footer into view
        AnimateFooterVisibility(TRUE, 0);
    }

    // Enable the focus animations from this point onwards so that they aren't affected by the
    // window transitions
    AutoPtr<Runnable> runnable = new EnableFocusAnimationsDimRunnable(this);
    Boolean res;
    PostDelayed(runnable, (startDelay / 2), &res);
}

/** Animates this task view as it leaves recents by pressing home. */
void TaskView::StartExitToHomeAnimation(
    /* [in] */ ViewAnimation::TaskViewExitContext* ctx)
{
    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);
    animator->TranslationY(ctx->mOffscreenTranslationY);
    animator->SetStartDelay(0);
    animator->SetUpdateListener(NULL);
    animator->SetInterpolator(
        mConfig->mFastOutLinearInInterpolator);
    animator->SetDuration(mConfig->mTaskViewExitToHomeDuration);
    animator->WithEndAction(ctx->mPostAnimationTrigger->DecrementAsRunnable());
    animator->Start();
    ctx->mPostAnimationTrigger->Increment();
}

/** Animates this task view as it exits recents */
void TaskView::StartLaunchTaskAnimation(
    /* [in] */ IRunnable* postAnimRunnable,
    /* [in] */ Boolean isLaunchingTask,
    /* [in] */ Boolean occludesLaunchTarget,
    /* [in] */ Boolean lockToTask)
{
    if (isLaunchingTask) {
        // Animate the thumbnail alpha back into full opacity for the window animation out
        mThumbnailView->StartLaunchTaskAnimation(postAnimRunnable);

        // Animate the dim
        if (mDim > 0) {
            AutoPtr<IObjectAnimatorHelper> oaHelper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelper);
            AutoPtr<ArrayOf<Int32> > params = ArrayOf<Int32>::Alloc(1);
            (*params)[0] = 0;
            AutoPtr<IObjectAnimator> anim;
            oaHelper->OfInt32(TO_IINTERFACE(this), String("dim"), params, (IObjectAnimator**)&anim);
            IAnimator* animator = IAnimator::Probe(anim);
            animator->SetDuration(mConfig->mTaskBarExitAnimDuration);
            animator->SetInterpolator(mConfig->mFastOutLinearInInterpolator);
            animator->Start();
        }

        // Animate the action button away
        AutoPtr<IViewPropertyAnimator> animator;
        mActionButtonView->Animate((IViewPropertyAnimator**)&animator);
        if (!lockToTask) {
            Float toScale = 0.9f;
            animator->ScaleX(toScale);
            animator->ScaleY(toScale);
        }
        animator->Alpha(0.0f);
        animator->SetStartDelay(0);
        animator->SetDuration(mConfig->mTaskBarExitAnimDuration);
        animator->SetInterpolator(
            mConfig->mFastOutLinearInInterpolator);
        animator->WithLayer();
        animator->Start();
    }
    else {
        // Hide the dismiss button
        mHeaderView->StartLaunchTaskDismissAnimation();
        // If this is another view in the task grouping and is in front of the launch task,
        // animate it away first
        if (occludesLaunchTarget) {
            AutoPtr<IViewPropertyAnimator> animator;
            Animate((IViewPropertyAnimator**)&animator);
            animator->Alpha(0.0f);
            Float y;
            GetTranslationY(&y);
            animator->TranslationY(y + mConfig->mTaskViewAffiliateGroupEnterOffsetPx);
            animator->SetStartDelay(0);
            animator->SetUpdateListener(NULL);
            animator->SetInterpolator(mConfig->mFastOutLinearInInterpolator);
            animator->SetDuration(mConfig->mTaskBarExitAnimDuration);
            animator->Start();
        }
    }
}

/** Animates the deletion of this task view */
void TaskView::StartDeleteTaskAnimation(
    /* [in] */ IRunnable* r)
{
    // Disabling clipping with the stack while the view is animating away
    SetClipViewInStack(FALSE);

    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);
    animator->TranslationX(mConfig->mTaskViewRemoveAnimTranslationXPx);
    animator->Alpha(0.0f);
    animator->SetStartDelay(0);
    animator->SetUpdateListener(NULL);
    animator->SetInterpolator(mConfig->mFastOutSlowInInterpolator);
    animator->SetDuration(mConfig->mTaskViewRemoveAnimDuration);
    AutoPtr<Runnable> runnable = new EndDeleteTaskAnimationRunnable(this, r);
    animator->WithEndAction(runnable);
    animator->Start();
}

/** Animates this task view if the user does not interact with the stack after a certain time. */
void TaskView::StartNoUserInteractionAnimation()
{
    mHeaderView->StartNoUserInteractionAnimation();
}

/** Mark this task view that the user does has not interacted with the stack after a certain time. */
void TaskView::SetNoUserInteractionState()
{
    mHeaderView->SetNoUserInteractionState();
}

/** Dismisses this task. */
void TaskView::DismissTask()
{
    // Animate out the view and call the callback
    AutoPtr<Runnable> runnable = new DismissTaskRunnable(this);
    StartDeleteTaskAnimation(runnable);
    // Hide the footer
    AnimateFooterVisibility(FALSE, mConfig->mTaskViewRemoveAnimDuration);
}

/** Sets whether this task view is full screen or not. */
void TaskView::SetIsFullScreen(
    /* [in] */ Boolean isFullscreen)
{
    mIsFullScreenView = isFullscreen;
    mHeaderView->SetIsFullscreen(isFullscreen);
    if (isFullscreen) {
        // If we are full screen, then disable the bottom outline clip for the footer
        mViewBounds->SetOutlineClipBottom(0);
    }
}

/** Returns whether this task view should currently be drawn as a full screen view. */
Boolean TaskView::IsFullScreenView()
{
    return mIsFullScreenView;
}

/**
 * Returns whether this view should be clipped, or any views below should clip against this
 * view.
 */
Boolean TaskView::ShouldClipViewInStack()
{
    Int32 v;
    return mClipViewInStack && !mIsFullScreenView && (GetVisibility(&v), v == IView::VISIBLE);
}

/** Sets whether this view should be clipped, or clipped against. */
void TaskView::SetClipViewInStack(
    /* [in] */ Boolean clip)
{
    if (clip != mClipViewInStack) {
        mClipViewInStack = clip;
        mCb->OnTaskViewClipStateChanged(this);
    }
}

/** Gets the max footer height. */
Int32 TaskView::GetMaxFooterHeight()
{
    if (mFooterView != NULL) {
        return mFooterView->mMaxFooterHeight;
    }
    else {
        return 0;
    }
}

/** Animates the footer into and out of view. */
void TaskView::AnimateFooterVisibility(
    /* [in] */ Boolean visible,
    /* [in] */ Int32 duration)
{
    // Hide the footer if we are a full screen view
    if (mIsFullScreenView) return;
    // Hide the footer if the current task can not be locked to
    if (!mTask->mLockToTaskEnabled || !mTask->mLockToThisTask) return;
    // Otherwise, animate the visibility
    if (mFooterView != NULL) {
        mFooterView->AnimateFooterVisibility(visible, duration);
    }
}

/** Sets the current task progress. */
ECode TaskView::SetTaskProgress(
    /* [in] */ Float p)
{
    mTaskProgress = p;
    mViewBounds->SetAlpha(p);
    UpdateDimFromTaskProgress();
    return NOERROR;
}

ECode TaskView::GetTaskProgress(
    /* [out] */ Float* p)
{
    VALIDATE_NOT_NULL(p)
    *p = mTaskProgress;
    return NOERROR;
}

/** Returns the current task progress. */
Float TaskView::GetTaskProgress()
{
    return mTaskProgress;
}

/** Returns the current dim. */
ECode TaskView::SetDim(
    /* [in] */ Int32 dim)
{
    mDim = dim;
    if (mDimAnimator != NULL) {
        mDimAnimator->RemoveAllListeners();
        mDimAnimator->Cancel();
    }
    if (mConfig->mUseHardwareLayers) {
        // Defer setting hardware layers if we have not yet measured, or there is no dim to draw
        Int32 measuredWidth, measuredHeight;
        GetMeasuredWidth(&measuredWidth);
        GetMeasuredHeight(&measuredHeight);
        if (measuredWidth > 0 && measuredHeight > 0) {
            if (mDimAnimator != NULL) {
                mDimAnimator->RemoveAllListeners();
                mDimAnimator->Cancel();
            }

            Int32 inverse = 255 - mDim;
            AutoPtr<IColor> color;
            CColor::AcquireSingleton((IColor**)&color);
            Int32 argb;
            color->Argb(0xFF, inverse, inverse, inverse, &argb);
            mDimColorFilter->SetColor(argb);
            mLayerPaint->SetColorFilter(IColorFilter::Probe(mDimColorFilter));
            mContent->SetLayerType(LAYER_TYPE_HARDWARE, mLayerPaint);
        }
    }
    else {
        Float dimAlpha = mDim / 255.0f;
        if (mThumbnailView != NULL) {
            mThumbnailView->SetDimAlpha(dimAlpha);
        }
        if (mHeaderView != NULL) {
            mHeaderView->SetDimAlpha(dim);
        }
    }
    return NOERROR;
}

ECode TaskView::GetDim(
    /* [out] */ Int32* dim)
{
    VALIDATE_NOT_NULL(dim)
    *dim = mDim;
    return NOERROR;
}

/** Returns the current dim. */
Int32 TaskView::GetDim()
{
    return mDim;
}

/** Animates the dim to the task progress. */
void TaskView::AnimateDimToProgress(
    /* [in] */ Int32 delay,
    /* [in] */ Int32 duration,
    /* [in] */ IAnimatorListener* postAnimRunnable)
{
    // Animate the dim into view as well
    Int32 toDim = GetDimFromTaskProgress();
    if (toDim != GetDim()) {
        AutoPtr<IObjectAnimatorHelper> oaHelper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelper);
        AutoPtr<ArrayOf<Int32> > params = ArrayOf<Int32>::Alloc(1);
        (*params)[0] = toDim;
        AutoPtr<IObjectAnimator> objAnim;
        oaHelper->OfInt32(TO_IINTERFACE(this), String("dim"), params, (IObjectAnimator**)&objAnim);
        AutoPtr<IAnimator> anim = IAnimator::Probe(objAnim);
        anim->SetStartDelay(delay);
        anim->SetDuration(duration);
        if (postAnimRunnable != NULL) {
            anim->AddListener(postAnimRunnable);
        }
        anim->Start();
    }
}

/** Compute the dim as a function of the scale of this view. */
Int32 TaskView::GetDimFromTaskProgress()
{
    Float interpolation;
    mDimInterpolator->GetInterpolation(1.0f - mTaskProgress, &interpolation);
    Float dim = mMaxDimScale * interpolation;
    return (Int32) (dim * 255);
}

/** Update the dim as a function of the scale of this view. */
void TaskView::UpdateDimFromTaskProgress()
{
    SetDim(GetDimFromTaskProgress());
}

/**** View focus state ****/

/**
 * Sets the focused task explicitly. We need a separate flag because requestFocus() won't happen
 * if the view is not currently visible, or we are in touch state (where we still want to keep
 * track of focus).
 */
void TaskView::SetFocusedTask()
{
    mIsFocused = TRUE;
    if (mFocusAnimationsEnabled) {
        // Focus the header bar
        mHeaderView->OnTaskViewFocusChanged(TRUE);
    }
    // Update the thumbnail alpha with the focus
    mThumbnailView->OnFocusChanged(TRUE);
    // Call the callback
    mCb->OnTaskViewFocusChanged(this, TRUE);
    // Workaround, we don't always want it focusable in touch mode, but we want the first task
    // to be focused after the enter-recents animation, which can be triggered from either touch
    // or keyboard
    SetFocusableInTouchMode(TRUE);
    Boolean res;
    RequestFocus(&res);
    SetFocusableInTouchMode(FALSE);
    Invalidate();
}

/**
 * Unsets the focused task explicitly.
 */
void TaskView::UnsetFocusedTask()
{
    mIsFocused = FALSE;
    if (mFocusAnimationsEnabled) {
        // Un-focus the header bar
        mHeaderView->OnTaskViewFocusChanged(FALSE);
    }

    // Update the thumbnail alpha with the focus
    mThumbnailView->OnFocusChanged(FALSE);
    // Call the callback
    mCb->OnTaskViewFocusChanged(this, FALSE);
    Invalidate();
}

/**
 * Updates the explicitly focused state when the view focus changes.
 */
// @Override
void TaskView::OnFocusChanged(
    /* [in] */ Boolean gainFocus,
    /* [in] */ Int32 direction,
    /* [in] */ IRect* previouslyFocusedRect)
{
    FrameLayout::OnFocusChanged(gainFocus, direction, previouslyFocusedRect);
    if (!gainFocus) {
        UnsetFocusedTask();
    }
}

/**
 * Returns whether we have explicitly been focused.
 */
Boolean TaskView::IsFocusedTask()
{
    Boolean isFocused;
    return mIsFocused || (IsFocused(&isFocused), isFocused);
}

/** Enables all focus animations. */
void TaskView::EnableFocusAnimations()
{
    Boolean wasFocusAnimationsEnabled = mFocusAnimationsEnabled;
    mFocusAnimationsEnabled = TRUE;
    if (mIsFocused && !wasFocusAnimationsEnabled) {
        // Re-notify the header if we were focused and animations were not previously enabled
        mHeaderView->OnTaskViewFocusChanged(TRUE);
    }
}

/**** TaskCallbacks Implementation ****/

/** Binds this task view to the task */
void TaskView::OnTaskBound(
    /* [in] */ ITask* t)
{
    mTask = (Task*)t;
    mTask->SetCallbacks(mWrapper);
    Int32 measuredWidth;
    GetMeasuredWidth(&measuredWidth);
    if (measuredWidth == 0) {
        // If we haven't yet measured, we should just set the footer height with any animation
        AnimateFooterVisibility(mTask->mLockToThisTask, 0);
    }
    else {
        AnimateFooterVisibility(mTask->mLockToThisTask, mConfig->mTaskViewLockToAppLongAnimDuration);
    }
    // Hide the action button if lock to app is disabled
    Int32 v;
    if (!mTask->mLockToTaskEnabled && (mActionButtonView->GetVisibility(&v), v != IView::GONE)) {
        mActionButtonView->SetVisibility(IView::GONE);
    }
}

ECode TaskView::OnTaskDataLoaded()
{
    if (mThumbnailView != NULL && mHeaderView != NULL) {
        // Bind each of the views to the new task data
        if (mIsFullScreenView) {
            AutoPtr<IBitmap> bmp;
            AlternateRecentsComponent::GetLastScreenshot((IBitmap**)&bmp);
            mThumbnailView->BindToScreenshot(bmp);
        }
        else {
            mThumbnailView->RebindToTask(mTask);
        }
        mHeaderView->RebindToTask(mTask);
        // Rebind any listeners
        IView::Probe(mHeaderView->mApplicationIcon)->SetOnClickListener(mWrapper);
        IView::Probe(mHeaderView->mDismissButton)->SetOnClickListener(mWrapper);
        if (mFooterView != NULL) {
            mFooterView->SetOnClickListener(mWrapper);
        }
        mActionButtonView->SetOnClickListener(mWrapper);
        if (Constants::DebugFlags::App::EnableDevAppInfoOnLongPress) {
            if (mConfig->mDeveloperOptionsEnabled) {
                IView::Probe(mHeaderView->mApplicationIcon)->SetOnLongClickListener(mWrapper);
            }
        }
    }
    mTaskDataLoaded = TRUE;
    return NOERROR;
}

ECode TaskView::OnTaskDataUnloaded()
{
    if (mThumbnailView != NULL && mHeaderView != NULL) {
        // Unbind each of the views from the task data and remove the task callback
        mTask->SetCallbacks(NULL);
        mThumbnailView->UnbindFromTask();
        mHeaderView->UnbindFromTask();
        // Unbind any listeners
        IView::Probe(mHeaderView->mApplicationIcon)->SetOnClickListener(NULL);
        IView::Probe(mHeaderView->mDismissButton)->SetOnClickListener(NULL);
        if (mFooterView != NULL) {
            mFooterView->SetOnClickListener(NULL);
        }
        mActionButtonView->SetOnClickListener(NULL);
        if (Constants::DebugFlags::App::EnableDevAppInfoOnLongPress) {
            IView::Probe(mHeaderView->mApplicationIcon)->SetOnLongClickListener(NULL);
        }
    }
    mTaskDataLoaded = FALSE;
    return NOERROR;
}

/** Enables/disables handling touch on this task view. */
void TaskView::SetTouchEnabled(
    /* [in] */ Boolean enabled)
{
    SetOnClickListener(enabled ? mWrapper : NULL);
}

/**** TaskViewFooter.TaskFooterViewCallbacks ****/

ECode TaskView::OnTaskFooterHeightChanged(
    /* [in] */ Int32 height,
    /* [in] */ Int32 maxHeight)
{
    if (mIsFullScreenView) {
        // Disable the bottom outline clip when fullscreen
        mViewBounds->SetOutlineClipBottom(0);
    }
    else {
        // Update the bottom clip in our outline provider
        mViewBounds->SetOutlineClipBottom(maxHeight - height);
    }
    return NOERROR;
}

/**** View.OnClickListener Implementation ****/

ECode TaskView::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<TaskView> tv = this;
    Boolean delayViewClick = (v != (IView*)this) && (v != mActionButtonView);
    if (delayViewClick) {
        // We purposely post the handler delayed to allow for the touch feedback to draw
        AutoPtr<Runnable> runnable = new OnClickRunnable(this, v);
        Boolean res;
        PostDelayed(runnable, 125, &res);
    }
    else {
        if (v == mActionButtonView) {
            // Reset the translation of the action button before we animate it out
            mActionButtonView->SetTranslationZ(0.0f);
        }
        mCb->OnTaskViewClicked(tv, tv->GetTask(),
            (v == (IView*)mFooterView || v == mActionButtonView));
    }
    return NOERROR;
}

/**** View.OnLongClickListener Implementation ****/

// @Override
ECode TaskView::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (v == IView::Probe(mHeaderView->mApplicationIcon)) {
        mCb->OnTaskViewAppInfoClicked(this);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
