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

#include "elastos/droid/systemui/recents/views/TaskViewHeader.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#include "elastos/droid/systemui/recents/misc/Utilities.h"
#include "elastos/droid/systemui/recents/Constants.h"
#include <elastos/droid/R.h>
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::CArgbEvaluator;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::CColorStateListHelper;
using Elastos::Droid::Content::Res::IColorStateListHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPorterDuffColorFilter;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::PorterDuffMode_ADD;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawableConstantState;
using Elastos::Droid::Graphics::Drawable::ILayerDrawable;
using Elastos::Droid::Graphics::Drawable::IShapeDrawable;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::SystemUI::Recents::Model::Task;
using Elastos::Droid::SystemUI::Recents::Misc::Utilities;
using Elastos::Core::CoreUtils;
using Elastos::Core::IFloat;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

static const String TAG("TaskViewHeader");

TaskViewHeader::OutlineProvider::OutlineProvider(
    /* [in] */ TaskViewHeader* host)
    : mHost(host)
{}

// @Override
ECode TaskViewHeader::OutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 w, h;
    mHost->GetMeasuredWidth(&w);
    mHost->GetMeasuredHeight(&h);
    outline->SetRect(0, 0, w, h);
    return NOERROR;
}

TaskViewHeader::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ TaskViewHeader* host)
    : mHost(host)
{}

// @Override
ECode TaskViewHeader::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(0);
    Boolean res;
    IDrawable::Probe(mHost->mBackground)->SetState(array, &res);
    return NOERROR;
}

CAR_INTERFACE_IMPL(TaskViewHeader::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

TaskViewHeader::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ TaskViewHeader* host)
    : mHost(host)
{}

// @Override
ECode TaskViewHeader::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    Float f;
    IFloat::Probe(value)->GetValue(&f);
    Int32 color = (Int32)f;
    mHost->mBackgroundColorDrawable->SetColor(color);
    mHost->mBackgroundColor = color;
    return NOERROR;
}

AutoPtr<IPaint> TaskViewHeader::sHighlightPaint;;

CAR_INTERFACE_IMPL(TaskViewHeader, FrameLayout, ITaskViewHeader)

TaskViewHeader::TaskViewHeader()
    : mBackgroundColor(0)
    , mIsFullscreen(FALSE)
    , mCurrentPrimaryColorIsDark(FALSE)
    , mCurrentPrimaryColor(0)
{
    CPorterDuffColorFilter::New(0, PorterDuffMode_SRC_ATOP, (IPorterDuffColorFilter**)&mDimFilter);
    CPaint::New((IPaint**)&mDimPaint);
}

ECode TaskViewHeader::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode TaskViewHeader::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode TaskViewHeader::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode TaskViewHeader::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes));
    mConfig = RecentsConfiguration::GetInstance();
    SetWillNotDraw(FALSE);
    SetClipToOutline(TRUE);
    AutoPtr<ViewOutlineProvider> provider = new OutlineProvider(this);
    SetOutlineProvider(provider);

    // Load the dismiss resources
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDrawable(R::drawable::recents_dismiss_light, (IDrawable**)&mLightDismissDrawable);
    res->GetDrawable(R::drawable::recents_dismiss_dark, (IDrawable**)&mDarkDismissDrawable);

    // Configure the highlight paint
    if (sHighlightPaint == NULL) {
        CPaint::New((IPaint**)&sHighlightPaint);
        sHighlightPaint->SetStyle(PaintStyle_STROKE);
        sHighlightPaint->SetStrokeWidth(mConfig->mTaskViewHighlightPx);
        sHighlightPaint->SetColor(mConfig->mTaskBarViewHighlightColor);
        AutoPtr<IXfermode> mode;
        CPorterDuffXfermode::New(PorterDuffMode_ADD, (IXfermode**)&mode);
        sHighlightPaint->SetXfermode(mode);
        sHighlightPaint->SetAntiAlias(TRUE);
    }
    return NOERROR;
}

// @Override
ECode TaskViewHeader::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // We ignore taps on the task bar except on the filter and dismiss buttons
    if (!Constants::DebugFlags::App::EnableTaskBarTouchEvents) {
        *result = TRUE;
        return NOERROR;
    }

    return FrameLayout::OnTouchEvent(event, result);
}

// @Override
ECode TaskViewHeader::OnFinishInflate()
{
    // Set the outline provider
    AutoPtr<ViewOutlineProvider> provider = new OutlineProvider(this);
    SetOutlineProvider(provider);

    // Initialize the icon and description views
    AutoPtr<IView> view;
    FindViewById(R::id::application_icon, (IView**)&view);
    mApplicationIcon = IImageView::Probe(view);
    view = NULL;
    FindViewById(R::id::activity_description, (IView**)&view);
    mActivityDescription = ITextView::Probe(view);
    view = NULL;
    FindViewById(R::id::dismiss_task, (IView**)&view);
    mDismissButton = IImageView::Probe(view);

    // Hide the backgrounds if they are ripple drawables
    if (!Constants::DebugFlags::App::EnableTaskFiltering) {
        AutoPtr<IDrawable> drawable;
        IView::Probe(mApplicationIcon)->GetBackground((IDrawable**)&drawable);
        if (IRippleDrawable::Probe(drawable)) {
            IView::Probe(mApplicationIcon)->SetBackground(NULL);
        }
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> colorDrawable;
    context->GetDrawable(R::drawable::recents_task_view_header_bg_color, (IDrawable**)&colorDrawable);
    mBackgroundColorDrawable = IGradientDrawable::Probe(colorDrawable);

    // Copy the ripple drawable since we are going to be manipulating it
    AutoPtr<IColorStateListHelper> helper;
    CColorStateListHelper::AcquireSingleton((IColorStateListHelper**)&helper);

    AutoPtr<IDrawable> bgDrawable;
    context->GetDrawable(R::drawable::recents_task_view_header_bg, (IDrawable**)&bgDrawable);
    bgDrawable->Mutate();
    AutoPtr<IDrawableConstantState> state;
    bgDrawable->GetConstantState((IDrawableConstantState**)&state);
    AutoPtr<IDrawable> drawable;
    state->NewDrawable((IDrawable**)&drawable);
    mBackground = IRippleDrawable::Probe(drawable);
    ILayerDrawable* layerDrawable = ILayerDrawable::Probe(drawable);

    AutoPtr<IColorStateList> csList;
    helper->ValueOf(0, (IColorStateList**)&csList);
    mBackground->SetColor(csList);

    Int32 id;
    Boolean res;
    layerDrawable->GetId(0, &id);
    layerDrawable->SetDrawableByLayerId(id, colorDrawable, &res);
    SetBackground(drawable);
    return NOERROR;
}

// @Override
void TaskViewHeader::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (!mIsFullscreen) {
        // Draw the highlight at the top edge (but put the bottom edge just out of view)
        Float offset = (Float) Elastos::Core::Math::Ceil(mConfig->mTaskViewHighlightPx / 2.0f);
        Float radius = mConfig->mTaskViewRoundedCornerRadiusPx;
        Int32 count;
        canvas->Save(ICanvas::CLIP_SAVE_FLAG, &count);
        Int32 w, h;
        GetMeasuredWidth(&w);
        GetMeasuredHeight(&h);
        Boolean res;
        canvas->ClipRect(0, 0, w, h, &res);
        canvas->DrawRoundRect(-offset, 0.0f, (Float) w + offset,
                h + radius, radius, radius, sHighlightPaint);
        canvas->RestoreToCount(count);
    }
}

/** Sets whether the current task is full screen or not. */
ECode TaskViewHeader::SetIsFullscreen(
    /* [in] */ Boolean isFullscreen)
{
    mIsFullscreen = isFullscreen;
    return NOERROR;
}

// @Override
ECode TaskViewHeader::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

/** Returns the secondary color for a primary color. */
Int32 TaskViewHeader::GetSecondaryColor(
    /* [in] */ Int32 primaryColor,
    /* [in] */ Boolean useLightOverlayColor)
{
    Int32 overlayColor = useLightOverlayColor ? IColor::WHITE : IColor::BLACK;
    return Utilities::GetColorWithOverlay(primaryColor, overlayColor, 0.8f);
}

/** Binds the bar view to the task */
ECode TaskViewHeader::RebindToTask(
    /* [in] */ ITask* _t)
{
    Task* t = (Task*)_t;
    // If an activity icon is defined, then we use that as the primary icon to show in the bar,
    // otherwise, we fall back to the application icon
    if (t->mActivityIcon != NULL) {
        mApplicationIcon->SetImageDrawable(t->mActivityIcon);
    }
    else if (t->mApplicationIcon != NULL) {
        mApplicationIcon->SetImageDrawable(t->mApplicationIcon);
    }
    IView::Probe(mApplicationIcon)->SetContentDescription(CoreUtils::Convert(t->mActivityLabel));
    AutoPtr<ICharSequence> text;
    mActivityDescription->GetText((ICharSequence**)&text);
    String strText;
    text->ToString(&strText);
    if (!strText.Equals(t->mActivityLabel)) {
        mActivityDescription->SetText(CoreUtils::Convert(t->mActivityLabel));
    }
    // Try and apply the system ui tint
    AutoPtr<IDrawable> background;
    GetBackground((IDrawable**)&background);
    Int32 existingBgColor = 0;
    if (IColorDrawable::Probe(background)) {
        IColorDrawable::Probe(background)->GetColor(&existingBgColor);
    }
    if (existingBgColor != t->mColorPrimary) {
        mBackgroundColorDrawable->SetColor(t->mColorPrimary);
        mBackgroundColor = t->mColorPrimary;
    }
    mCurrentPrimaryColor = t->mColorPrimary;
    mCurrentPrimaryColorIsDark = t->mUseLightOnPrimaryColor;
    mActivityDescription->SetTextColor(t->mUseLightOnPrimaryColor ?
            mConfig->mTaskBarViewLightTextColor : mConfig->mTaskBarViewDarkTextColor);
    mDismissButton->SetImageDrawable(t->mUseLightOnPrimaryColor ?
            mLightDismissDrawable : mDarkDismissDrawable);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    array->Set(0, CoreUtils::Convert(t->mActivityLabel));
    String str;
    context->GetString(R::string::accessibility_recents_item_will_be_dismissed, array, &str);
    IView::Probe(mDismissButton)->SetContentDescription(CoreUtils::Convert(str));
    return NOERROR;
}

/** Unbinds the bar view from the task */
ECode TaskViewHeader::UnbindFromTask()
{
    return mApplicationIcon->SetImageDrawable(NULL);
}

/** Animates this task bar dismiss button when launching a task. */
ECode TaskViewHeader::StartLaunchTaskDismissAnimation()
{
    Int32 visibility;
    IView::Probe(mDismissButton)->GetVisibility(&visibility);
    if (visibility == IView::VISIBLE) {
        AutoPtr<IViewPropertyAnimator> animate;
        IView::Probe(mDismissButton)->Animate((IViewPropertyAnimator**)&animate);
        animate->Cancel();
        animate->Alpha(0.0f);
        animate->SetStartDelay(0);
        animate->SetInterpolator(ITimeInterpolator::Probe(mConfig->mFastOutSlowInInterpolator));
        animate->SetDuration(mConfig->mTaskBarExitAnimDuration);
        animate->WithLayer();
        animate->Start();
    }
    return NOERROR;
}

/** Animates this task bar if the user does not interact with the stack after a certain time. */
ECode TaskViewHeader::StartNoUserInteractionAnimation()
{
    IView* view = IView::Probe(mDismissButton);
    view->SetVisibility(IView::VISIBLE);
    view->SetAlpha(0.0f);
    AutoPtr<IViewPropertyAnimator> animate;
    view->Animate((IViewPropertyAnimator**)&animate);
    animate->Alpha(1.0f);
    animate->SetStartDelay(0);
    animate->SetInterpolator(ITimeInterpolator::Probe(mConfig->mFastOutLinearInInterpolator));
    animate->SetDuration(mConfig->mTaskBarEnterAnimDuration);
    animate->WithLayer();
    animate->Start();
    return NOERROR;
}

/** Mark this task view that the user does has not interacted with the stack after a certain time. */
ECode TaskViewHeader::SetNoUserInteractionState()
{
    IView* view = IView::Probe(mDismissButton);
    Int32 visibility;
    view->GetVisibility(&visibility);
    if (visibility != IView::VISIBLE) {
        AutoPtr<IViewPropertyAnimator> animate;
        view->Animate((IViewPropertyAnimator**)&animate);
        animate->Cancel();
        view->SetVisibility(IView::VISIBLE);
        view->SetAlpha(1.0f);
    }
    return NOERROR;
}

// @Override
ECode TaskViewHeader::OnCreateDrawableState(
    /* [in] */ Int32 extraSpace,
    /* [out, callee] */ ArrayOf<Int32>** drawableState)
{
    VALIDATE_NOT_NULL(drawableState);
    // Don't forward our state to the drawable - we do it manually in onTaskViewFocusChanged.
    // This is to prevent layer trashing when the view is pressed.
    *drawableState = ArrayOf<Int32>::Alloc(0);
    REFCOUNT_ADD(*drawableState)
    return NOERROR;
}

/** Notifies the associated TaskView has been focused. */
ECode TaskViewHeader::OnTaskViewFocusChanged(
    /* [in] */ Boolean focused)
{
    Boolean isRunning = FALSE;
    if (mFocusAnimator != NULL) {
        IAnimator* animator = IAnimator::Probe(mFocusAnimator);
        animator->IsRunning(&isRunning);
        animator->RemoveAllListeners();
        animator->Cancel();
    }
    if (focused) {
        Int32 secondaryColor = GetSecondaryColor(mCurrentPrimaryColor, mCurrentPrimaryColorIsDark);
        AutoPtr<ArrayOf<HANDLE> > states = ArrayOf<HANDLE>::Alloc(2);
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
        (*array)[0] = Elastos::Droid::R::attr::state_enabled;
        AutoPtr<ArrayOf<Int32> > array2 = ArrayOf<Int32>::Alloc(1);
        (*array2)[0] = Elastos::Droid::R::attr::state_pressed;
        (*states)[0] = (HANDLE)array.Get();
        (*states)[1] = (HANDLE)array2.Get();
        AutoPtr<ArrayOf<Int32> > newStates = ArrayOf<Int32>::Alloc(1);
        (*newStates)[0] = Elastos::Droid::R::attr::state_enabled;
        (*newStates)[1] = Elastos::Droid::R::attr::state_pressed;
        AutoPtr<ArrayOf<Int32> > colors = ArrayOf<Int32>::Alloc(1);
        (*colors)[0] = secondaryColor;
        (*colors)[1] = secondaryColor;
        AutoPtr<IColorStateList> csList;
        CColorStateList::New(states, colors, (IColorStateList**)&csList);
        mBackground->SetColor(csList);
        Boolean res;
        IDrawable::Probe(mBackground)->SetState(newStates, &res);

        // Pulse the background color
        Int32 currentColor = mBackgroundColor;
        Int32 lightPrimaryColor = GetSecondaryColor(mCurrentPrimaryColor, mCurrentPrimaryColorIsDark);
        AutoPtr<IValueAnimatorHelper> vaHelper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vaHelper);
        AutoPtr<ITypeEvaluator> evaluator;
        CArgbEvaluator::New((ITypeEvaluator**)&evaluator);
        AutoPtr<ArrayOf<IInterface*> > params;
        params->Set(0, CoreUtils::Convert(lightPrimaryColor));
        params->Set(1, CoreUtils::Convert(currentColor));
        AutoPtr<IValueAnimator> backgroundColor;
        vaHelper->OfObject(evaluator, params, (IValueAnimator**)&backgroundColor);
        AutoPtr<IAnimator> animator = IAnimator::Probe(backgroundColor);
        AutoPtr<AnimatorListenerAdapter> adapter = new MyAnimatorListenerAdapter(this);
        animator->AddListener(adapter);
        AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener(this);
        backgroundColor->AddUpdateListener(listener);
        backgroundColor->SetRepeatCount(IValueAnimator::ANIMATION_INFINITE);
        backgroundColor->SetRepeatMode(IValueAnimator::ANIMATION_REVERSE);

        // Pulse the translation
        AutoPtr<IObjectAnimatorHelper> oaHelper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelper);
        AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
        (*values)[0] = 15.0f;
        AutoPtr<IObjectAnimator> translation;
        oaHelper->OfFloat(TO_IINTERFACE(this), String("translationZ"), values,
            (IObjectAnimator**)&translation);
        IValueAnimator::Probe(translation)->SetRepeatCount(IValueAnimator::ANIMATION_INFINITE);
        IValueAnimator::Probe(translation)->SetRepeatMode(IValueAnimator::ANIMATION_REVERSE);

        mFocusAnimator = NULL;
        CAnimatorSet::New((IAnimatorSet**)&mFocusAnimator);
        animator = IAnimator::Probe(mFocusAnimator);
        AutoPtr<ArrayOf<IAnimator*> > anims = ArrayOf<IAnimator*>::Alloc(2);
        anims->Set(0, IAnimator::Probe(backgroundColor));
        anims->Set(1, IAnimator::Probe(translation));
        mFocusAnimator->PlayTogether(anims);
        animator->SetStartDelay(750);
        animator->SetDuration(750);
        animator->Start();
    }
    else {
        if (isRunning) {
            // Restore the background color
            Int32 currentColor = mBackgroundColor;
            AutoPtr<IValueAnimatorHelper> vaHelper;
            CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vaHelper);
            AutoPtr<ITypeEvaluator> evaluator;
            CArgbEvaluator::New((ITypeEvaluator**)&evaluator);
            AutoPtr<ArrayOf<IInterface*> > params;
            params->Set(0, CoreUtils::Convert(currentColor));
            params->Set(1, CoreUtils::Convert(mCurrentPrimaryColor));
            AutoPtr<IValueAnimator> backgroundColor;
            vaHelper->OfObject(evaluator, params, (IValueAnimator**)&backgroundColor);
            AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener(this);
            backgroundColor->AddUpdateListener(listener);

            // Restore the translation
            AutoPtr<IObjectAnimatorHelper> oaHelper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelper);
            AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
            (*values)[0] = 0.0f;
            AutoPtr<IObjectAnimator> translation;
            oaHelper->OfFloat(TO_IINTERFACE(this), String("translationZ"), values,
                (IObjectAnimator**)&translation);

            mFocusAnimator = NULL;
            CAnimatorSet::New((IAnimatorSet**)&mFocusAnimator);
            AutoPtr<ArrayOf<IAnimator*> > anims = ArrayOf<IAnimator*>::Alloc(2);
            anims->Set(0, IAnimator::Probe(backgroundColor));
            anims->Set(1, IAnimator::Probe(translation));
            mFocusAnimator->PlayTogether(anims);
            IAnimator::Probe(mFocusAnimator)->SetDuration(150);
            IAnimator::Probe(mFocusAnimator)->Start();
        }
        else {
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(0);
            Boolean res;
            IDrawable::Probe(mBackground)->SetState(array, &res);
            SetTranslationZ(0.0f);
        }
    }
    return NOERROR;
}

ECode TaskViewHeader::SetDimAlpha(
    /* [in] */ Int32 alpha)
{
    AutoPtr<IColor> cHelper;
    CColor::AcquireSingleton((IColor**)&cHelper);
    Int32 color;
    cHelper->Argb(alpha, 0, 0, 0, &color);
    mDimFilter->SetColor(color);
    mDimPaint->SetColorFilter(IColorFilter::Probe(mDimFilter));
    SetLayerType(LAYER_TYPE_HARDWARE, mDimPaint);
    return NOERROR;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
