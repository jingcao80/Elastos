
#include "elastos/droid/systemui/keyguard/KeyguardWidgetFrame.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::ILinearGradient;
using Elastos::Droid::Graphics::CLinearGradient;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;
using Elastos::Droid::Graphics::PorterDuffMode_ADD;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Core::Math;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode KeyguardWidgetFrame::MyKeyguardUpdateMonitorCallback::OnBootCompleted()
{
    if (mHost->mPerformAppWidgetSizeUpdateOnBootComplete) {
        mHost->PerformAppWidgetSizeCallbacksIfNecessary();
        mHost->mPerformAppWidgetSizeUpdateOnBootComplete = FALSE;
    }
    return NOERROR;
}

static AutoPtr<IPorterDuffXfermode> initAddBlendMode()
{
    AutoPtr<IPorterDuffXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_ADD, (IPorterDuffXfermode**)&mode);
    return mode;
}

AutoPtr<IPorterDuffXfermode> KeyguardWidgetFrame::sAddBlendMode = initAddBlendMode();

const Float KeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER = 0.6f;
const Int32 KeyguardWidgetFrame::HOVER_OVER_DELETE_DROP_TARGET_OVERLAY_COLOR = 0x99FF0000;

// Temporarily disable this for the time being until we know why the gfx is messing up
const Boolean KeyguardWidgetFrame::ENABLE_HOVER_OVER_DELETE_DROP_TARGET_OVERLAY = TRUE;

CAR_INTERFACE_IMPL(KeyguardWidgetFrame, FrameLayout, IKeyguardWidgetFrame)

KeyguardWidgetFrame::KeyguardWidgetFrame()
    : mGradientColor(0)
    , mLeftToRight(TRUE)
    , mOverScrollAmount(0.0f)
    , mForegroundAlpha(0)
    , mIsSmall(FALSE)
    , mBackgroundAlpha(0.0f)
    , mContentAlpha(0.0f)
    , mBackgroundAlphaMultiplier(1.0f)
    , mSmallWidgetHeight(0)
    , mSmallFrameHeight(0)
    , mWidgetLockedSmall(FALSE)
    , mMaxChallengeTop(-1)
    , mFrameStrokeAdjustment(0)
    , mPerformAppWidgetSizeUpdateOnBootComplete(FALSE)
    , mFrameHeight(0)
    , mIsHoveringOverDeleteDropTarget(FALSE)
{
}

ECode KeyguardWidgetFrame::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode KeyguardWidgetFrame::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode KeyguardWidgetFrame::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);

    CPaint::New((IPaint**)&mGradientPaint);
    CRect::New((IRect**)&mForegroundRect);
    CRect::New((IRect**)&mBackgroundRect);

    mUpdateMonitorCallbacks = new MyKeyguardUpdateMonitorCallback(this);

    mLongPressHelper = new CheckLongPressHelper(this);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    // TODO: this padding should really correspond to the padding embedded in the background
    // drawable (ie. outlines).
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    Int32 padding = (Int32)(density * 8);
    SetPadding(padding, padding, padding, padding);

    mFrameStrokeAdjustment = 2 + (Int32)(2 * density);

    // This will be overriden on phones based on the current security mode, however on tablets
    // we need to specify a height.
    res->GetDimensionPixelSize(R::dimen::kg_small_widget_height, &mSmallWidgetHeight);
    res->GetDrawable(R::drawable::kg_widget_bg_padded, (IDrawable**)&mBackgroundDrawable);
    res->GetColor(R::color::kg_widget_pager_gradient, &mGradientColor);
    return mGradientPaint->SetXfermode(IXfermode::Probe(sAddBlendMode));
}

ECode KeyguardWidgetFrame::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();
    CancelLongPress();
    return KeyguardUpdateMonitor::GetInstance(mContext)->RemoveCallback(mUpdateMonitorCallbacks);
}

ECode KeyguardWidgetFrame::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    return KeyguardUpdateMonitor::GetInstance(mContext)->RegisterCallback(mUpdateMonitorCallbacks);
}

ECode KeyguardWidgetFrame::SetIsHoveringOverDeleteDropTarget(
    /* [in] */ Boolean isHovering)
{
    if (ENABLE_HOVER_OVER_DELETE_DROP_TARGET_OVERLAY) {
        if (mIsHoveringOverDeleteDropTarget != isHovering) {
            mIsHoveringOverDeleteDropTarget = isHovering;
            Int32 resId = isHovering ? R::string::keyguard_accessibility_delete_widget_start
                    : R::string::keyguard_accessibility_delete_widget_end;
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IResources> resources;
            context->GetResources((IResources**)&resources);
            AutoPtr<ICharSequence> cchar;
            GetContentDescription((ICharSequence**)&cchar);
            AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
            array->Set(0, TO_IINTERFACE(cchar));
            String text;
            resources->GetString(resId, array, &text);
            AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(text);
            AnnounceForAccessibility(cchar2);
            Invalidate();
        }
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Watch for longpress events at this level to make sure
    // users can always pick up this widget
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            mLongPressHelper->PostCheckForLongPress(ev);
            break;
        case IMotionEvent::ACTION_MOVE:
            mLongPressHelper->OnMove(ev);
            break;
        case IMotionEvent::ACTION_POINTER_DOWN:
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mLongPressHelper->CancelLongPress();
            break;
    }

    // Otherwise continue letting touch events fall through to children
    *result = FALSE;
    return NOERROR;
}

ECode KeyguardWidgetFrame::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // Watch for longpress events at this level to make sure
    // users can always pick up this widget
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_MOVE:
            mLongPressHelper->OnMove(ev);
            break;
        case IMotionEvent::ACTION_POINTER_DOWN:
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mLongPressHelper->CancelLongPress();
            break;
    }

    // We return true here to ensure that we will get cancel / up signal
    // even if none of our children have requested touch.
    *result = TRUE;
    return NOERROR;
}

ECode KeyguardWidgetFrame::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean disallowIntercept)
{
    FrameLayout::RequestDisallowInterceptTouchEvent(disallowIntercept);
    CancelLongPress();
    return NOERROR;
}

ECode KeyguardWidgetFrame::CancelLongPress()
{
    FrameLayout::CancelLongPress();
    return mLongPressHelper->CancelLongPress();
}

void KeyguardWidgetFrame::DrawGradientOverlay(
    /* [in] */ ICanvas* c)
{
    mGradientPaint->SetShader(IShader::Probe(mForegroundGradient));
    mGradientPaint->SetAlpha(mForegroundAlpha);
    c->DrawRect(mForegroundRect, mGradientPaint);
}

void KeyguardWidgetFrame::DrawHoveringOverDeleteOverlay(
    /* [in] */ ICanvas* c)
{
    if (mIsHoveringOverDeleteDropTarget) {
        c->DrawColor(HOVER_OVER_DELETE_DROP_TARGET_OVERLAY_COLOR);
    }
}

ECode KeyguardWidgetFrame::DrawBg(
    /* [in] */ ICanvas* canvas)
{
    if (mBackgroundAlpha > 0.0f) {
        AutoPtr<IDrawable> bg = mBackgroundDrawable;

        bg->SetAlpha((Int32)(mBackgroundAlpha * mBackgroundAlphaMultiplier * 255));
        bg->SetBounds(mBackgroundRect);
        bg->Draw(canvas);
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    if (ENABLE_HOVER_OVER_DELETE_DROP_TARGET_OVERLAY) {
        Int32 res;
        canvas->Save(&res);
    }
    DrawBg(canvas);
    FrameLayout::DispatchDraw(canvas);
    DrawGradientOverlay(canvas);
    if (ENABLE_HOVER_OVER_DELETE_DROP_TARGET_OVERLAY) {
        DrawHoveringOverDeleteOverlay(canvas);
        canvas->Restore();
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::EnableHardwareLayersForContent()
{
    AutoPtr<IView> widget;
    GetContent((IView**)&widget);

    Boolean res;
    if (widget != NULL && (widget->IsHardwareAccelerated(&res), res)) {
        widget->SetLayerType(LAYER_TYPE_HARDWARE, NULL);
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::DisableHardwareLayersForContent()
{
    AutoPtr<IView> widget;
    GetContent((IView**)&widget);
    if (widget != NULL) {
        widget->SetLayerType(LAYER_TYPE_NONE, NULL);
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::GetContent(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    return GetChildAt(0, view);
}

ECode KeyguardWidgetFrame::GetContentAppWidgetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)

    AutoPtr<IView> content;
    GetContent((IView**)&content);

    if (IAppWidgetHostView::Probe(content) != NULL) {
        return IAppWidgetHostView::Probe(content)->GetAppWidgetId(id);
    }
    else if (IKeyguardStatusView::Probe(content) != NULL) {
        return IKeyguardStatusView::Probe(content)->GetAppWidgetId(id);
    }
    else {
        *id = IAppWidgetManager::INVALID_APPWIDGET_ID;
        return NOERROR;
    }
}

ECode KeyguardWidgetFrame::GetBackgroundAlpha(
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)

    *res = mBackgroundAlpha;
    return NOERROR;
}

ECode KeyguardWidgetFrame::SetBackgroundAlphaMultiplier(
    /* [in] */ Float multiplier)
{
    if (mBackgroundAlphaMultiplier != multiplier) {
        mBackgroundAlphaMultiplier = multiplier;
        Invalidate();
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::GetBackgroundAlphaMultiplier(
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)

    *res = mBackgroundAlphaMultiplier;
    return NOERROR;
}

ECode KeyguardWidgetFrame::SetBackgroundAlpha(
    /* [in] */ Float alpha)
{
    if (mBackgroundAlpha !=  alpha) {
        mBackgroundAlpha = alpha;
        Invalidate();
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::GetContentAlpha(
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)

    *res = mContentAlpha;
    return NOERROR;
}

ECode KeyguardWidgetFrame::SetContentAlpha(
    /* [in] */ Float alpha)
{
    mContentAlpha = alpha;
    AutoPtr<IView> content;
    GetContent((IView**)&content);
    if (content != NULL) {
        content->SetAlpha(alpha);
    }
    return NOERROR;
}

void KeyguardWidgetFrame::SetWidgetHeight(
    /* [in] */ Int32 height)
{
    Boolean needLayout = FALSE;
    AutoPtr<IView> widget;
    GetContent((IView**)&widget);
    if (widget != NULL) {
        AutoPtr<IViewGroupLayoutParams> p;
        widget->GetLayoutParams((IViewGroupLayoutParams**)&p);
        AutoPtr<FrameLayout::LayoutParams> lp =
                (FrameLayout::LayoutParams*)IFrameLayoutLayoutParams::Probe(p);
        if (lp->mHeight != height) {
            needLayout = TRUE;
            lp->mHeight = height;
        }
    }
    if (needLayout) {
        RequestLayout();
    }
}

ECode KeyguardWidgetFrame::SetMaxChallengeTop(
    /* [in] */ Int32 top)
{
    Boolean dirty = mMaxChallengeTop != top;
    mMaxChallengeTop = top;

    Int32 t;
    GetPaddingTop(&t);
    mSmallWidgetHeight = top - t;

    Int32 b;
    GetPaddingBottom(&b);
    mSmallFrameHeight = top + b;

    if (dirty && mIsSmall) {
        SetWidgetHeight(mSmallWidgetHeight);
        SetFrameHeight(mSmallFrameHeight);
    }
    else if (dirty && mWidgetLockedSmall) {
        SetWidgetHeight(mSmallWidgetHeight);
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::IsSmall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsSmall;
    return NOERROR;
}

ECode KeyguardWidgetFrame::AdjustFrame(
    /* [in] */ Int32 challengeTop)
{
    Int32 b;
    GetPaddingBottom(&b);
    Int32 frameHeight = challengeTop + b;
    return SetFrameHeight(frameHeight);
}

ECode KeyguardWidgetFrame::ShrinkWidget(
    /* [in] */ Boolean alsoShrinkFrame)
{
    mIsSmall = TRUE;
    SetWidgetHeight(mSmallWidgetHeight);

    if (alsoShrinkFrame) {
        SetFrameHeight(mSmallFrameHeight);
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::GetSmallFrameHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)

    *height = mSmallFrameHeight;
    return NOERROR;
}

ECode KeyguardWidgetFrame::SetWidgetLockedSmall(
    /* [in] */ Boolean locked)
{
    if (locked) {
        SetWidgetHeight(mSmallWidgetHeight);
    }
    mWidgetLockedSmall = locked;
    return NOERROR;
}

ECode KeyguardWidgetFrame::ResetSize()
{
    mIsSmall = FALSE;
    if (!mWidgetLockedSmall) {
        SetWidgetHeight(IViewGroupLayoutParams::MATCH_PARENT);
    }
    Int32 height;
    GetMeasuredHeight(&height);
    return SetFrameHeight(height);
}

ECode KeyguardWidgetFrame::SetFrameHeight(
    /* [in] */ Int32 height)
{
    mFrameHeight = height;
    Int32 w;
    GetMeasuredWidth(&w);
    Int32 h;
    GetMeasuredHeight(&h);
    mBackgroundRect->Set(0, 0, w, Elastos::Core::Math::Min(mFrameHeight, h));
    mForegroundRect->Set(mFrameStrokeAdjustment, mFrameStrokeAdjustment, w -
            mFrameStrokeAdjustment, Elastos::Core::Math::Min(h, mFrameHeight) -
            mFrameStrokeAdjustment);
    UpdateGradient();
    Invalidate();
    return NOERROR;
}

ECode KeyguardWidgetFrame::HideFrame(
    /* [in] */ IInterface* caller)
{
    return FadeFrame(caller, FALSE, 0.0f,
            IKeyguardWidgetPager::CHILDREN_OUTLINE_FADE_OUT_DURATION);
}

ECode KeyguardWidgetFrame::ShowFrame(
    /* [in] */ IInterface* caller)
{
    return FadeFrame(caller, TRUE, OUTLINE_ALPHA_MULTIPLIER,
            IKeyguardWidgetPager::CHILDREN_OUTLINE_FADE_IN_DURATION);
}

ECode KeyguardWidgetFrame::FadeFrame(
    /* [in] */ IInterface* caller,
    /* [in] */ Boolean takeControl,
    /* [in] */ Float alpha,
    /* [in] */ Int32 duration)
{
    if (takeControl) {
        mBgAlphaController = caller;
    }

    if (TO_IINTERFACE(mBgAlphaController) != TO_IINTERFACE(caller) && mBgAlphaController != NULL) {
        return NOERROR;
    }

    if (mFrameFade != NULL) {
        mFrameFade->Cancel();
        mFrameFade = NULL;
    }
    AutoPtr<IPropertyValuesHolderHelper> helper;
    CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
    (*array)[0] = alpha;
    AutoPtr<IPropertyValuesHolder> bgAlpha;
    helper->OfFloat(String("backgroundAlpha"), array, (IPropertyValuesHolder**)&bgAlpha);

    AutoPtr<IObjectAnimatorHelper> helper2;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper2);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > array2 =
            ArrayOf<IPropertyValuesHolder*>::Alloc(1);
    array2->Set(0, bgAlpha);
    AutoPtr<IObjectAnimator> oa;
    helper2->OfPropertyValuesHolder(TO_IINTERFACE(this), array2, (IObjectAnimator**)&oa);
    mFrameFade = IAnimator::Probe(oa);
    mFrameFade->SetDuration(duration);
    return mFrameFade->Start();
}

void KeyguardWidgetFrame::UpdateGradient()
{
    Int32 width;
    mForegroundRect->GetWidth(&width);
    Float x0 = mLeftToRight ? 0 : width;
    Float x1 = mLeftToRight ? width: 0;
    CLinearGradient::New(x0, 0.0f, x1, 0.0f,
            mGradientColor, 0, ShaderTileMode_CLAMP, (ILinearGradient**)&mLeftToRightGradient);
    CLinearGradient::New(x1, 0.0f, x0, 0.0f,
            mGradientColor, 0, ShaderTileMode_CLAMP, (ILinearGradient**)&mRightToLeftGradient);
}

ECode KeyguardWidgetFrame::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    FrameLayout::OnSizeChanged(w, h, oldw, oldh);

    if (!mIsSmall) {
        mFrameHeight = h;
    }

    // mFrameStrokeAdjustment is a cludge to prevent the overlay from drawing outside the
    // rounded rect background.
    mForegroundRect->Set(mFrameStrokeAdjustment, mFrameStrokeAdjustment,
            w - mFrameStrokeAdjustment, Elastos::Core::Math::Min(h, mFrameHeight) - mFrameStrokeAdjustment);

    Int32 width;
    GetMeasuredWidth(&width);
    mBackgroundRect->Set(0, 0, width, Elastos::Core::Math::Min(h, mFrameHeight));
    UpdateGradient();
    return Invalidate();
}

ECode KeyguardWidgetFrame::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    PerformAppWidgetSizeCallbacksIfNecessary();
    return NOERROR;
}

void KeyguardWidgetFrame::PerformAppWidgetSizeCallbacksIfNecessary()
{
    AutoPtr<IView> content;
    GetContent((IView**)&content);
    if (IAppWidgetHostView::Probe(content) == NULL) return;

    Boolean res;
    if (KeyguardUpdateMonitor::GetInstance(mContext)->HasBootCompleted(&res), !res) {
        mPerformAppWidgetSizeUpdateOnBootComplete = TRUE;
        return;
    }

    // TODO: there's no reason to force the AppWidgetHostView to catch duplicate size calls.
    // We can do that even more cheaply here. It's not an issue right now since we're in the
    // system process and hence no binder calls.
    AutoPtr<IAppWidgetHostView> awhv = IAppWidgetHostView::Probe(content);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);

    Int32 _width;
    content->GetMeasuredWidth(&_width);
    Int32 _height;
    content->GetMeasuredHeight(&_height);
    Int32 width = (Int32) (_width / density);
    Int32 height = (Int32) (_height / density);
    awhv->UpdateAppWidgetSize(NULL, width, height, width, height, TRUE);
}

ECode KeyguardWidgetFrame::SetOverScrollAmount(
    /* [in] */ Float r,
    /* [in] */ Boolean left)
{
    if (mOverScrollAmount !=  r) {
        mOverScrollAmount = r;
        mForegroundGradient = left ? mLeftToRightGradient : mRightToLeftGradient;
        mForegroundAlpha = (Int32) Elastos::Core::Math::Round((0.5f * r * 255));

        // We bump up the alpha of the outline to hide the fact that the overlay is drawing
        // over the rounded part of the frame.
        Float bgAlpha = Elastos::Core::Math::Min(OUTLINE_ALPHA_MULTIPLIER + r * (1 - OUTLINE_ALPHA_MULTIPLIER),
                1.0f);
        SetBackgroundAlpha(bgAlpha);
        Invalidate();
    }
    return NOERROR;
}

ECode KeyguardWidgetFrame::OnActive(
    /* [in] */ Boolean isActive)
{
    // hook for subclasses
    return NOERROR;
}

ECode KeyguardWidgetFrame::OnUserInteraction(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // hook for subclasses

    *result = FALSE;
    return NOERROR;
}

ECode KeyguardWidgetFrame::OnBouncerShowing(
    /* [in] */ Boolean showing)
{
    // hook for subclasses
    return NOERROR;
}

ECode KeyguardWidgetFrame::SetWorkerHandler(
    /* [in] */ IHandler* workerHandler)
{
    mWorkerHandler = workerHandler;
    return NOERROR;
}

ECode KeyguardWidgetFrame::GetWorkerHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)

    *handler = mWorkerHandler;
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
