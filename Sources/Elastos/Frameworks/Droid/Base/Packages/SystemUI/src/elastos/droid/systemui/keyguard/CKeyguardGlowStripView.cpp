
#include "elastos/droid/systemui/keyguard/CKeyguardGlowStripView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardGlowStripView::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mDrawDots = FALSE;
    // make sure we draw one frame at the end with everything gone.
    mHost->Invalidate();
    return NOERROR;
}

ECode CKeyguardGlowStripView::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mHost->mDrawDots = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardGlowStripView::MyAnimatorUpdateListener, Object, IAnimatorUpdateListener)

ECode CKeyguardGlowStripView::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IFloat> value = IFloat::Probe(obj);
    value->GetValue(&(mHost->mAnimationProgress));
    return mHost->Invalidate();
}

const Int32 CKeyguardGlowStripView::DURATION = 500;

const Float CKeyguardGlowStripView::SLIDING_WINDOW_SIZE = 0.4f;

CAR_OBJECT_IMPL(CKeyguardGlowStripView)

CAR_INTERFACE_IMPL(CKeyguardGlowStripView, LinearLayout, IKeyguardGlowStripView)

CKeyguardGlowStripView::CKeyguardGlowStripView()
    : mDotStripTop(0)
    , mHorizontalDotGap(0)
    , mDotSize(0)
    , mNumDots(0)
    , mLeftToRight(TRUE)
    , mAnimationProgress(0.0F)
    , mDrawDots(FALSE)
{
    CDecelerateInterpolator::New(0.5f, (IInterpolator**)&mDotAlphaInterpolator);
}

ECode CKeyguardGlowStripView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardGlowStripView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardGlowStripView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::constructor(context, attrs, defStyle);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, R::styleable::KeyguardGlowStripView, (ITypedArray**)&a);
    a->GetDimensionPixelSize(R::styleable::KeyguardGlowStripView_dotSize, mDotSize, &mDotSize);
    a->GetInt32(R::styleable::KeyguardGlowStripView_numDots, mNumDots, &mNumDots);
    a->GetDrawable(R::styleable::KeyguardGlowStripView_glowDot, (IDrawable**)&mDotDrawable);
    return a->GetBoolean(R::styleable::KeyguardGlowStripView_leftToRight, mLeftToRight, &mLeftToRight);
}

ECode CKeyguardGlowStripView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    Int32 left;
    GetPaddingLeft(&left);
    Int32 right;
    GetPaddingRight(&right);
    Int32 availableWidth = w - left - right;

    mHorizontalDotGap = (availableWidth - mDotSize * mNumDots) /  (mNumDots - 1);
    GetPaddingTop(&mDotStripTop);
    return Invalidate();
}

ECode CKeyguardGlowStripView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    LinearLayout::DispatchDraw(canvas);

    if (!mDrawDots) return NOERROR;

    Int32 xOffset;
    GetPaddingLeft(&xOffset);
    mDotDrawable->SetBounds(0, 0, mDotSize, mDotSize);

    for (Int32 i = 0; i < mNumDots; i++) {
        // We fudge the relative position to provide a fade in of the first dot and a fade
        // out of the final dot.
        Float relativeDotPosition = SLIDING_WINDOW_SIZE / 2 + ((1.0f * i) / (mNumDots - 1)) *
                (1 - SLIDING_WINDOW_SIZE);
        Float distance = Math::Abs(relativeDotPosition - mAnimationProgress);
        Float alpha = Math::Max(0, 1 - distance / (SLIDING_WINDOW_SIZE / 2));

        mDotAlphaInterpolator->GetInterpolation(alpha, &alpha);

        canvas->Save();
        canvas->Translate(xOffset, mDotStripTop);
        mDotDrawable->SetAlpha((Int32) (alpha * 255));
        mDotDrawable->Draw(canvas);
        canvas->Restore();
        xOffset += mDotSize + mHorizontalDotGap;
    }
    return NOERROR;
}

ECode CKeyguardGlowStripView::MakeEmGo()
{
    if (mAnimator != NULL) {
        mAnimator->Cancel();
    }
    Float from = mLeftToRight ? 0f : 1f;
    Float to = mLeftToRight ? 1f : 0f;

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    helper->OfFloat(from, to, (IValueAnimator**)&mAnimator);
    mAnimator->SetDuration(DURATION);
    AutoPtr<ILinearInterpolator> interpolator
    CLinearInterpolator::New((ILinearInterpolator**)&interpolator);
    mAnimator->SetInterpolator(interpolator);

    AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(this);
    IAnimator::Probe(mAnimator)->AddListener(lis);

    AutoPtr<IAnimatorUpdateListener> lis2 = new MyAnimatorUpdateListener(this);
    mAnimator->AddUpdateListener(lis2);
    return mAnimator->Start();
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
