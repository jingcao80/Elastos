
#include "elastos/droid/systemui/keyguard/CKeyguardWidgetFrame.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardWidgetFrame::MyKeyguardUpdateMonitorCallback::OnBootCompleted()
{
    if (mHost->mPerformAppWidgetSizeUpdateOnBootComplete) {
        PerformAppWidgetSizeCallbacksIfNecessary();
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

AutoPtr<IPorterDuffXfermode> CKeyguardWidgetFrame::sAddBlendMode = initAddBlendMode();

const Float CKeyguardWidgetFrame::OUTLINE_ALPHA_MULTIPLIER = 0.6f;
const Int32 CKeyguardWidgetFrame::HOVER_OVER_DELETE_DROP_TARGET_OVERLAY_COLOR = 0x99FF0000;

// Temporarily disable this for the time being until we know why the gfx is messing up
const Boolean CKeyguardWidgetFrame::ENABLE_HOVER_OVER_DELETE_DROP_TARGET_OVERLAY = TRUE;

CAR_OBJECT_IMPL(CKeyguardWidgetFrame)

CKeyguardWidgetFrame::CKeyguardWidgetFrame()
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
    CPaint::New((IPain**)&mGradientPaint);

    CRect::New((IRect**)&mForegroundRect);

    CRect::New((IRect**)&mBackgroundRect);

    mUpdateMonitorCallbacks = new MyKeyguardUpdateMonitorCallback(this);
}

ECode CKeyguardWidgetFrame::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardWidgetFrame::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardWidgetFrame::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);

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
    return mGradientPaint->SetXfermode(sAddBlendMode);
}

ECode CKeyguardWidgetFrame::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();
    CancelLongPress();
    return KeyguardUpdateMonitor::GetInstance(mContext)->RemoveCallback(mUpdateMonitorCallbacks);
}

ECode CKeyguardWidgetFrame::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    return KeyguardUpdateMonitor::GetInstance(mContext)->RegisterCallback(mUpdateMonitorCallbacks);
}

ECode CKeyguardWidgetFrame::SetIsHoveringOverDeleteDropTarget(
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
            String text;
            resources->GetString(resId, cchar, &text);
            AnnounceForAccessibility(text);
            Invalidate();
        }
    }
    return NOERROR;
}

ECode CKeyguardWidgetFrame::OnInterceptTouchEvent(
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

ECode CKeyguardWidgetFrame::OnTouchEvent(
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

ECode CKeyguardWidgetFrame::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean disallowIntercept)
{
    FrameLayout::RequestDisallowInterceptTouchEvent(disallowIntercept);
    CancelLongPress();
    return NOERROR;
}

ECode CKeyguardWidgetFrame::CancelLongPress()
{
    FrameLayout::CancelLongPress();
    return mLongPressHelper->CancelLongPress();
}

void CKeyguardWidgetFrame::DrawGradientOverlay(
    /* [in] */ ICanvas* c)
{
    mGradientPaint->SetShader(mForegroundGradient);
    mGradientPaint->SetAlpha(mForegroundAlpha);
    c->DrawRect(mForegroundRect, mGradientPaint);
}

void CKeyguardWidgetFrame::DrawHoveringOverDeleteOverlay(
    /* [in] */ ICanvas* c)
{
    if (mIsHoveringOverDeleteDropTarget) {
        c->DrawColor(HOVER_OVER_DELETE_DROP_TARGET_OVERLAY_COLOR);
    }
}

ECode CKeyguardWidgetFrame::DrawBg(
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

ECode CKeyguardWidgetFrame::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    if (ENABLE_HOVER_OVER_DELETE_DROP_TARGET_OVERLAY) {
        canvas->Save();
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

ECode CKeyguardWidgetFrame::EnableHardwareLayersForContent()
{
    AutoPtr<IView> widget;
    GetContent((IView**)&widget);

    Boolean res;
    if (widget != NULL && (widget->IsHardwareAccelerated(&res), res)) {
        widget->SetLayerType(LAYER_TYPE_HARDWARE, NULL);
    }
    return NOERROR;
}

ECode CKeyguardWidgetFrame::DisableHardwareLayersForContent()
{
    AutoPtr<IView> widget;
    GetContent((IView**)&widget);
    if (widget != NULL) {
        widget->SetLayerType(LAYER_TYPE_NONE, null);
    }
    return NOERROR;
}

ECode CKeyguardWidgetFrame::GetContent(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    return GetChildAt(0, view);
}

ECode CKeyguardWidgetFrame::GetContentAppWidgetId(
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

ECode CKeyguardWidgetFrame::GetBackgroundAlpha(
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)

    *res = mBackgroundAlpha;
    return NOERROR;
}

ECode CKeyguardWidgetFrame::SetBackgroundAlphaMultiplier(
    /* [in] */ Float multiplier)
{
    if (mBackgroundAlphaMultiplier != multiplier) {
        mBackgroundAlphaMultiplier = multiplier;
        Invalidate();
    }
    return NOERROR;
}

ECode CKeyguardWidgetFrame::GetBackgroundAlphaMultiplier(
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)

    *res = mBackgroundAlphaMultiplier;
    return NOERROR;
}

ECode CKeyguardWidgetFrame::SetBackgroundAlpha(
    /* [in] */ Float alpha)
{
    if (mBackgroundAlpha !=  alpha) {
        mBackgroundAlpha = alpha;
        Invalidate();
    }
    return NOERROR;
}

ECode CKeyguardWidgetFrame::GetContentAlpha(
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)

    *res = mContentAlpha;
    return NOERROR;
}

ECode CKeyguardWidgetFrame::SetContentAlpha(
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

void CKeyguardWidgetFrame::SetWidgetHeight(
    /* [in] */ Int32 height)
{
    Boolean needLayout = FALSE;
    AutoPtr<IView> widget;
    GetContent((IView**)&widget);
    if (widget != NULL) {
        AutoPtr<IViewGroupLayoutParams> p;
        widget->GetLayoutParams((IViewGroupLayoutParams**)&p);
        AutoPtr<IFrameLayoutLayoutParams> lp = IFrameLayoutLayoutParams::Probe(p);
        Int32 h;
        lp->GetHeight(&h);
        if (h != height) {
            needLayout = TRUE;
            lp->SetHeight(height);
        }
    }
    if (needLayout) {
        RequestLayout();
    }
}

ECode CKeyguardWidgetFrame::SetMaxChallengeTop(
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

ECode CKeyguardWidgetFrame::IsSmall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mIsSmall;
    return NOERROR;
}

ECode CKeyguardWidgetFrame::AdjustFrame(
    /* [in] */ Int32 challengeTop)
{
    Int32 b;
    GetPaddingBottom(&b);
    Int32 frameHeight = challengeTop + b;
    return SetFrameHeight(frameHeight);
}

ECode CKeyguardWidgetFrame::ShrinkWidget(
    /* [in] */ Boolean alsoShrinkFrame)
{
    mIsSmall = TRUE;
    SetWidgetHeight(mSmallWidgetHeight);

    if (alsoShrinkFrame) {
        SetFrameHeight(mSmallFrameHeight);
    }
    return NOERROR;
}

ECode CKeyguardWidgetFrame::GetSmallFrameHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)

    *height = mSmallFrameHeight;
    return NOERROR;
}

ECode CKeyguardWidgetFrame::SetWidgetLockedSmall(
    /* [in] */ Boolean locked)
{
    if (locked) {
        SetWidgetHeight(mSmallWidgetHeight);
    }
    mWidgetLockedSmall = locked;
    return NOERROR;
}

ECode CKeyguardWidgetFrame::ResetSize()
{
    mIsSmall = FALSE;
    if (!mWidgetLockedSmall) {
        SetWidgetHeight(LayoutParams.MATCH_PARENT);
    }
    Int32 height;
    GetMeasuredHeight(&height);
    return SetFrameHeight(height);
}

ECode CKeyguardWidgetFrame::SetFrameHeight(
    /* [in] */ Int32 height)
{
    mFrameHeight = height;
    Int32 w;
    GetMeasuredWidth(&w);
    Int32 h;
    GetMeasuredHeight(&h);
    mBackgroundRect->Set(0, 0, w, Math::Min(mFrameHeight, h));
    mForegroundRect->Set(mFrameStrokeAdjustment, mFrameStrokeAdjustment, w -
            mFrameStrokeAdjustment, Math::Min(h, mFrameHeight) -
            mFrameStrokeAdjustment);
    UpdateGradient();
    Invalidate();
    return NOERROR;
}

ECode CKeyguardWidgetFrame::HideFrame(
    /* [in] */ IInterface* caller)
{
    return FadeFrame(caller, FALSE, 0.0f,
            IKeyguardWidgetPager::CHILDREN_OUTLINE_FADE_OUT_DURATION);
}

ECode CKeyguardWidgetFrame::ShowFrame(
    /* [in] */ IInterface* caller)
{
    return FadeFrame(caller, TRUE, OUTLINE_ALPHA_MULTIPLIER,
            IKeyguardWidgetPager::CHILDREN_OUTLINE_FADE_IN_DURATION);
}

ECode CKeyguardWidgetFrame::FadeFrame(
    /* [in] */ IInterface* caller,
    /* [in] */ Boolean takeControl,
    /* [in] */ Float alpha,
    /* [in] */ Int32 duration)
{
    if (takeControl) {
        mBgAlphaController = caller;
    }

    if (mBgAlphaController != caller && mBgAlphaController != NULL) {
        return;
    }

    if (mFrameFade != NULL) {
        mFrameFade->Cancel();
        mFrameFade = NULL;
    }
    AutoPtr<IPropertyValuesHolderHelper> helper;
    CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
    AutoPtr<IPropertyValuesHolder> bgAlpha;
    helper->OfFloat(String("backgroundAlpha"), alpha, (IPropertyValuesHolder**)&bgAlpha);

    AutoPtr<IObjectAnimatorHelper> helper2;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper2);
    helper2->OfPropertyValuesHolder(this, bgAlpha, (IAnimator**)&mFrameFade);
    mFrameFade->SetDuration(duration);
    return mFrameFade->Start();
}

void CKeyguardWidgetFrame::UpdateGradient()
{
    Int32 width;
    mForegroundRect->GetWidth(&width);
    Float x0 = mLeftToRight ? 0 : width;
    Float x1 = mLeftToRight ? width: 0;
    CLinearGradient::New(x0, 0f, x1, 0f,
            mGradientColor, 0, ShaderTileMode_CLAMP, (ILinearGradient**)&mLeftToRightGradient);
    CLinearGradient::New(x1, 0f, x0, 0f,
            mGradientColor, 0, ShaderTileMode_CLAMP, (ILinearGradient**)&mRightToLeftGradient);
}

ECode CKeyguardWidgetFrame::OnSizeChanged(
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
            w - mFrameStrokeAdjustment, Math::Min(h, mFrameHeight) - mFrameStrokeAdjustment);

    Int32 width;
    GetMeasuredWidth(&width);
    mBackgroundRect->Set(0, 0, width, Math::Min(h, mFrameHeight));
    UpdateGradient();
    return Invalidate();
}

ECode CKeyguardWidgetFrame::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    FrameLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    PerformAppWidgetSizeCallbacksIfNecessary();
    return NOERROR;
}

void CKeyguardWidgetFrame::PerformAppWidgetSizeCallbacksIfNecessary()
{
    AutoPtr<IView> content
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

    Int32 width;
    content->GetMeasuredWidth(&width);
    Int32 height;
    content->GetMeasuredHeight(&height);
    Int32 width = (Int32) (width / density);
    Int32 height = (Int32) (height / density);
    awhv->UpdateAppWidgetSize(null, width, height, width, height, TRUE);
}

ECode CKeyguardWidgetFrame::SetOverScrollAmount(
    /* [in] */ Float r,
    /* [in] */ Boolean left)
{
    if (mOverScrollAmount !=  r) {
        mOverScrollAmount = r;
        mForegroundGradient = left ? mLeftToRightGradient : mRightToLeftGradient;
        mForegroundAlpha = (Int32) Math::Round((0.5f * r * 255));

        // We bump up the alpha of the outline to hide the fact that the overlay is drawing
        // over the rounded part of the frame.
        Float bgAlpha = Math::Min(OUTLINE_ALPHA_MULTIPLIER + r * (1 - OUTLINE_ALPHA_MULTIPLIER),
                1f);
        SetBackgroundAlpha(bgAlpha);
        Invalidate();
    }
    return NOERROR;
}

ECode CKeyguardWidgetFrame::OnActive(
    /* [in] */ Boolean isActive)
{
    // hook for subclasses
    return NOERROR;
}

ECode CKeyguardWidgetFrame::OnUserInteraction(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // hook for subclasses

    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardWidgetFrame::OnBouncerShowing(
    /* [in] */ Boolean showing)
{
    // hook for subclasses
    return NOERROR;
}

ECode CKeyguardWidgetFrame::SetWorkerHandler(
    /* [in] */ IHandler* workerHandler)
{
    mWorkerHandler = workerHandler;
    return NOERROR;
}

ECode CKeyguardWidgetFrame::GetWorkerHandler(
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
