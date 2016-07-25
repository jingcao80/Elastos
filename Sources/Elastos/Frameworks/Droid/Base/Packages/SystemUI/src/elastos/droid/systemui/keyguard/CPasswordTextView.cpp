
#include "elastos/droidsystemui/keyguard/CPasswordTextView::.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mCancelled) {
        mOutHost->mTextChars->Remove(TO_IINTERFACE(mInHost));
        mOutHost->mCharPool->Push(TO_IINTERFACE(mInHost));
        Reset();
        CancelAnimator(textTranslateAnimator);
        mInHost->mTextTranslateAnimator = NULL;
    }
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mCancelled = FALSE;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mDotAnimator = NULL;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mTextAnimator = NULL;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter4::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mTextTranslateAnimator = NULL;
    return NOERROR;
}

ECode CPasswordTextView::CharState::MyAnimatorListenerAdapter5::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mWidthAnimator = NULL;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CPasswordTextView::CharState::MyAnimatorUpdateListener, Object,
        IAnimatorUpdateListener)

ECode CPasswordTextView::CharState::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue(&value);
    AutoPtr<IFloat> f = IFloat::Probe(value);
    f->GetValue(&(mInHost->mCurrentDotSizeFactor));
    return mOutHost->Invalidate();
}

CAR_INTERFACE_IMPL(CPasswordTextView::CharState::MyAnimatorUpdateListener2, Object,
        IAnimatorUpdateListener)

ECode CPasswordTextView::CharState::MyAnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue(&value);
    AutoPtr<IFloat> f = IFloat::Probe(value);
    f->GetValue(&(mInHost->mCurrentTextSizeFactor));
    return mOutHost->Invalidate();
}

CAR_INTERFACE_IMPL(CPasswordTextView::CharState::MyAnimatorUpdateListener3, Object,
        IAnimatorUpdateListener)

ECode CPasswordTextView::CharState::MyAnimatorUpdateListener3::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue(&value);
    AutoPtr<IFloat> f = IFloat::Probe(value);
    f->GetValue(&(mInHost->mCurrentTextTranslationY));
    return mOutHost->Invalidate();
}

CAR_INTERFACE_IMPL(CPasswordTextView::CharState::MyAnimatorUpdateListener4, Object,
        IAnimatorUpdateListener)

ECode CPasswordTextView::CharState::MyAnimatorUpdateListener4::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue(&value);
    AutoPtr<IFloat> f = IFloat::Probe(value);
    f->GetValue(&(mInHost->mCurrentWidthFactor));
    return mOutHost->Invalidate();
}

ECode CPasswordTextView::CharState::MyRunnable::Run()
{
    mHost->PerformSwap();
    mHost->mIsDotSwapPending = FALSE;
    return NOERROR;
}

CPasswordTextView::CharState::CharState(
    /* [in] */ CPasswordTextView* host)
    : mWhichChar(0)
    , mTextAnimationIsGrowing(FALSE)
    , mDotAnimationIsGrowing(FALSE)
    , mWidthAnimationIsGrowing(FALSE)
    , mCurrentTextSizeFactor(0.0f)
    , mCurrentDotSizeFactor(0.0f)
    , mCurrentWidthFactor(0.0f)
    , mIsDotSwapPending(FALSE)
    , mCurrentTextTranslationY(1.0f)
    , mHost(host)
{
    mRemoveEndListener = new MyAnimatorListenerAdapter(this, mHost);
    mDotFinishListener = new MyAnimatorListenerAdapter(this);
    mTextFinishListener = new MyAnimatorListenerAdapter(this);
    mTextTranslateFinishListener = new MyAnimatorListenerAdapter(this);
    mWidthFinishListener = new MyAnimatorListenerAdapter(this);

    mDotSizeUpdater = new MyAnimatorUpdateListener(this, mHost);
    mTextSizeUpdater = new MyAnimatorUpdateListener2(this, mHost);
    mTextTranslationUpdater = new MyAnimatorUpdateListener3(this, mHost);
    mWidthUpdater = new MyAnimatorUpdateListener4(this, mHost);

    mDotSwapperRunnable = new MyRunnable(this);
}

ECode CPasswordTextView::CharState::Reset()
{
    mWhichChar = 0;
    mCurrentTextSizeFactor = 0.0f;
    mCurrentDotSizeFactor = 0.0f;
    mCurrentWidthFactor = 0.0f;
    CancelAnimator(textAnimator);
    mTextAnimator = null;
    CancelAnimator(dotAnimator);
    mDotAnimator = null;
    CancelAnimator(widthAnimator);
    mWidthAnimator = null;
    mCurrentTextTranslationY = 1.0f;
    RemoveDotSwapCallbacks();
    return NOERROR;
}

ECode CPasswordTextView::CharState::StartRemoveAnimation(
    /* [in] */ Int64 startDelay,
    /* [in] */ Int64 widthDelay)
{
    Boolean dotNeedsAnimation = (mCurrentDotSizeFactor > 0.0f && mDotAnimator == NULL)
            || (mDotAnimator != NULL && mDotAnimationIsGrowing);
    Boolean textNeedsAnimation = (mCurrentTextSizeFactor > 0.0f && mTextAnimator == NULL)
            || (mTextAnimator != NULL && mTextAnimationIsGrowing);
    Boolean widthNeedsAnimation = (mCurrentWidthFactor > 0.0f && widthAnimator == NULL)
            || (mWidthAnimator != NULL && mWidthAnimationIsGrowing);
    if (mDotNeedsAnimation) {
        StartDotDisappearAnimation(startDelay);
    }
    if (mTextNeedsAnimation) {
        StartTextDisappearAnimation(startDelay);
    }
    if (mWidthNeedsAnimation) {
        StartWidthDisappearAnimation(widthDelay);
    }
    return NOERROR;
}

ECode CPasswordTextView::CharState::StartAppearAnimation()
{
    Boolean dotNeedsAnimation = !mShowPassword
            && (mDotAnimator == NULL || !mDotAnimationIsGrowing);
    Boolean textNeedsAnimation = mShowPassword
            && (mTextAnimator == NULL || !mTextAnimationIsGrowing);
    Boolean widthNeedsAnimation = (mWidthAnimator == NULL || !mWidthAnimationIsGrowing);
    if (mDotNeedsAnimation) {
        StartDotAppearAnimation(0);
    }
    if (mTextNeedsAnimation) {
        StartTextAppearAnimation();
    }
    if (mWidthNeedsAnimation) {
        StartWidthAppearAnimation();
    }
    if (mShowPassword) {
        PostDotSwap(TEXT_VISIBILITY_DURATION);
    }
    return NOERROR;
}

void CPasswordTextView::CharState::PostDotSwap(
    /* [in] */ Int64 delay)
{
    RemoveDotSwapCallbacks();
    PostDelayed(dotSwapperRunnable, delay);
    mIsDotSwapPending = TRUE;
}

void CPasswordTextView::CharState::RemoveDotSwapCallbacks()
{
    RemoveCallbacks(dotSwapperRunnable);
    mIsDotSwapPending = FALSE;
}

ECode CPasswordTextView::CharState::SwapToDotWhenAppearFinished()
{
    RemoveDotSwapCallbacks();
    if (mTextAnimator != NULL) {
        Int64 duration;
        mTextAnimator->GetDuration(&duration);
        Int64 playeTime;
        mTextAnimator->GetCurrentPlayTime(&playeTime);
        Int64 remainingDuration = duration - playeTime;
        PostDotSwap(remainingDuration + TEXT_REST_DURATION_AFTER_APPEAR);
    }
    else {
        PerformSwap();
    }
    return NOERROR;
}

void CPasswordTextView::CharState::PerformSwap()
{
    StartTextDisappearAnimation(0);
    StartDotAppearAnimation(DISAPPEAR_DURATION
            - DOT_APPEAR_TEXT_DISAPPEAR_OVERLAP_DURATION);
}

void CPasswordTextView::CharState::StartWidthDisappearAnimation(
    /* [in] */ Int64 widthDelay)
{
    CancelAnimator(mWidthAnimator);

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    helper->OfFloat(mCurrentWidthFactor, 0.0f, (IValueAnimator**)&mWidthAnimator);
    mWidthAnimator->AddUpdateListener(widthUpdater);
    mWidthAnimator->AddListener(widthFinishListener);
    mWidthAnimator->AddListener(removeEndListener);
    mWidthAnimator->SetDuration((Int64) (DISAPPEAR_DURATION * mCurrentWidthFactor));
    mWidthAnimator->SetStartDelay(widthDelay);
    mWidthAnimator->Start();
    mWidthAnimationIsGrowing = FALSE;
}

void CPasswordTextView::CharState::StartTextDisappearAnimation(
    /* [in] */ Int64 startDelay)
{
    CancelAnimator(mTextAnimator);
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    helper->OfFloat(mCurrentTextSizeFactor, 0.0f, (IValueAnimator**)&mTextAnimator);
    mTextAnimator->AddUpdateListener(mTextSizeUpdater);
    mTextAnimator->AddListener(mTextFinishListener);
    mTextAnimator->SetInterpolator(mDisappearInterpolator);
    mTextAnimator->SetDuration((Int64) (DISAPPEAR_DURATION * mCurrentTextSizeFactor));
    mTextAnimator->SetStartDelay(startDelay);
    mTextAnimator->Start();
    mTextAnimationIsGrowing = FALSE;
}

void CPasswordTextView::CharState::StartDotDisappearAnimation(
    /* [in] */ Int64 startDelay)
{
    CancelAnimator(mDotAnimator);
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> animator;
    helper->OfFloat(mCurrentDotSizeFactor, 0.0f, (IValueAnimator**)&animator);
    animator->AddUpdateListener(mDotSizeUpdater);
    animator->AddListener(mDotFinishListener);
    animator->SetInterpolator(mDisappearInterpolator);
    Int64 duration = (Int64) (DISAPPEAR_DURATION * Math::Min(mCurrentDotSizeFactor, 1.0f));
    animator->SetDuration(duration);
    animator->SetStartDelay(startDelay);
    animator->Start();
    mDotAnimator = animator;
    mDotAnimationIsGrowing = FALSE;
}

void CPasswordTextView::CharState::StartWidthAppearAnimation()
{
    CancelAnimator(mWidthAnimator);
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    helper->OfFloat(mCurrentWidthFactor, 1.0f, (IValueAnimator**)&mWidthAnimator);
    mWidthAnimator->AddUpdateListener(mWidthUpdater);
    mWidthAnimator->AddListener(mWidthFinishListener);
    mWidthAnimator->SetDuration((Int64) (APPEAR_DURATION * (1.0f - mCurrentWidthFactor)));
    mWidthAnimator->Start();
    mWidthAnimationIsGrowing = TRUE;
}

void CPasswordTextView::CharState::StartTextAppearAnimation()
{
    CancelAnimator(mTextAnimator);
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    helper->OfFloat(mCurrentTextSizeFactor, 1.0f, (IValueAnimator**)&mTextAnimator);
    mTextAnimator->AddUpdateListener(mTextSizeUpdater);
    mTextAnimator->AddListener(mTextFinishListener);
    mTextAnimator->SetInterpolator(mAppearInterpolator);
    mTextAnimator->SetDuration((Int64) (APPEAR_DURATION * (1f - mCurrentTextSizeFactor)));
    mTextAnimator->Start();
    mTextAnimationIsGrowing = TRUE;

    // handle translation
    if (mTextTranslateAnimator == NULL) {
        helper->OfFloat(1.0f, 0.0f, (IValueAnimator**)&mTextTranslateAnimator);
        mTextTranslateAnimator->AddUpdateListener(mTextTranslationUpdater);
        mTextTranslateAnimator->AddListener(mTextTranslateFinishListener);
        mTextTranslateAnimator->SetInterpolator(mAppearInterpolator);
        mTextTranslateAnimator->SetDuration(APPEAR_DURATION);
        mTextTranslateAnimator->Start();
    }
}

void CPasswordTextView::CharState::StartDotAppearAnimation(
    /* [in] */ Int64 delay)
{
    CancelAnimator(mDotAnimator);

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    if (!mShowPassword) {
        // We perform an overshoot animation
        AutoPtr<IValueAnimator> overShootAnimator;
        helper->OfFloat(mCurrentDotSizeFactor, DOT_OVERSHOOT_FACTOR,
                (IValueAnimator**)&overShootAnimator);
        overShootAnimator->AddUpdateListener(mDotSizeUpdater);
        overShootAnimator->SetInterpolator(mAppearInterpolator);
        Int64 overShootDuration = (Int64) (DOT_APPEAR_DURATION_OVERSHOOT
                * OVERSHOOT_TIME_POSITION);
        overShootAnimator->SetDuration(overShootDuration);

        AutoPtr<IValueAnimator> settleBackAnimator;
        helper->OfFloat(DOT_OVERSHOOT_FACTOR, 1.0f, (IValueAnimator**)&settleBackAnimator);
        settleBackAnimator->AddUpdateListener(mDotSizeUpdater);
        settleBackAnimator->SetDuration(DOT_APPEAR_DURATION_OVERSHOOT - overShootDuration);
        settleBackAnimator->AddListener(mDotFinishListener);

        AutoPtr<IAnimatorSet> animatorSet;
        CAnimatorSet::New((IAnimatorSet**)&animatorSet);
        animatorSet->PlaySequentially(overShootAnimator, settleBackAnimator);
        animatorSet->SetStartDelay(delay);
        animatorSet->Start();
        mDotAnimator = animatorSet;
    }
    else {
        AutoPtr<IValueAnimator> growAnimator;
        helper->OfFloat(mCurrentDotSizeFactor, 1.0f, (IValueAnimator**)&growAnimator);
        growAnimator->AddUpdateListener(mDotSizeUpdater);
        growAnimator->SetDuration((Int64) (APPEAR_DURATION * (1.0f - mCurrentDotSizeFactor)));
        growAnimator->AddListener(mDotFinishListener);
        growAnimator->SetStartDelay(delay);
        growAnimator->Start();
        mDotAnimator = growAnimator;
    }
    mDotAnimationIsGrowing = TRUE;
}

void CPasswordTextView::CharState::CancelAnimator(
    /* [in] */ IAnimator* animator)
{
    if (animator != NULL) {
        animator->Cancel();
    }
}

ECode CPasswordTextView::CharState::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Float currentDrawPosition,
    /* [in] */ Int32 charHeight,
    /* [in] */ Float yPosition,
    /* [in] */ Float charLength,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean textVisible = mCurrentTextSizeFactor > 0;
    Boolean dotVisible = mCurrentDotSizeFactor > 0;
    Float charWidth = charLength * mCurrentWidthFactor;
    if (textVisible) {
        Float currYPosition = yPosition + charHeight / 2.0f * mCurrentTextSizeFactor
                + charHeight * mCurrentTextTranslationY * 0.8f;
        canvas->Save();
        Float centerX = currentDrawPosition + charWidth / 2;
        canvas->Translate(centerX, currYPosition);
        canvas->Scale(mCurrentTextSizeFactor, mCurrentTextSizeFactor);
        canvas->DrawText(StringUtils::ToString(mWhichChar), 0, 0, mDrawPaint);
        canvas->Restore();
    }
    if (dotVisible) {
        canvas->Save();
        Float centerX = currentDrawPosition + charWidth / 2;
        canvas->Translate(centerX, yPosition);
        canvas->DrawCircle(0, 0, mDotSize / 2 * mCurrentDotSizeFactor, mDrawPaint);
        canvas->Restore();
    }
    *result = charWidth + mCharPadding * mCurrentWidthFactor;
    return NOERROR;
}

const Float CPasswordTextView::DOT_OVERSHOOT_FACTOR = 1.5f;
const Int64 CPasswordTextView::DOT_APPEAR_DURATION_OVERSHOOT = 320;
const Int64 CPasswordTextView::APPEAR_DURATION = 160;
const Int64 CPasswordTextView::DISAPPEAR_DURATION = 160;
const Int64 CPasswordTextView::RESET_DELAY_PER_ELEMENT = 40;
const Int64 CPasswordTextView::RESET_MAX_DELAY = 200;

const Int64 CPasswordTextView::DOT_APPEAR_TEXT_DISAPPEAR_OVERLAP_DURATION = 130;

const Int64 CPasswordTextView::TEXT_REST_DURATION_AFTER_APPEAR = 100;

const Int64 CPasswordTextView::TEXT_VISIBILITY_DURATION = 1300;

const Float CPasswordTextView::OVERSHOOT_TIME_POSITION = 0.5f;

CAR_OBJECT_IMPL(CPasswordTextView)

CPasswordTextView::CPasswordTextView()
    : mTextHeightRaw(0)
    , mText(String(""))
    , mDotSize(0)
    , mCharPadding(0)
    , mShowPassword(FALSE)
{
    CArrayList::New((IArrayList**)&mTextChars);

    CStack::New((IStack**)&mCharPool);

    CPaint::New((IPaint**)&mDrawPaint);
}

ECode CPasswordTextView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CPasswordTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CPasswordTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CPasswordTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    View::constructor(context, attrs, defStyleAttr, defStyleRes);

    SetFocusableInTouchMode(TRUE);
    SetFocusable(TRUE);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, R::styleable::PasswordTextView, (ITypedArray**)&a);
    //try {
    a->GetInt32(R::styleable::PasswordTextView_scaledTextSize, 0, &mTextHeightRaw);
    //} finally {
    a->Recycle();
    //}
    mDrawPaint->SetFlags(IPaint::SUBPIXEL_TEXT_FLAG | IPaint::ANTI_ALIAS_FLAG);
    mDrawPaint->SetTextAlign(IPaintAlign_CENTER);
    mDrawPaint->SetColor(0xffffffff);

    AutoPtr<ITypefaceHelper> helper;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);
    AutoPtr<ITypeface> typeface;
    helper->Create(String("sans-serif-light"), 0, (ITypeface**)&typeface);
    mDrawPaint->SetTypeface(typeface);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    resources->GetDimensionPixelSize(R::dimen::password_dot_size, &mDotSize);
    resources->GetDimensionPixelSize(R::dimen::password_char_padding, &mCharPadding);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSystem> helper2;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper2);
    Int32 pw;
    helper2->GetInt32(cr, ISettingsSystem::TEXT_SHOW_PASSWORD, 1, &pw);
    mShowPassword = pw == 1;

    AutoPtr<IAnimationUtils> helper3;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&helper3);
    helper3->LoadInterpolator(mContext,
            Elastos::Droid::R::interpolator::linear_out_slow_in,
            (IInterpolator**)&mAppearInterpolator);
    helper3->LoadInterpolator(mContext,
            Elastos::Droid::R::interpolator::fast_out_linear_in,
            (IInterpolator**)&mDisappearInterpolator);
    helper3->LoadInterpolator(mContext,
            Elastos::Droid::R::interpolator::fast_out_slow_in,
            (IInterpolator**)&mFastOutSlowInInterpolator);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPM = IPowerManager::Probe(obj);
    return NOERROR;
}

void CPasswordTextView::OnDraw(
    /* [in] */ ICanvas* canvas);
{
    Float totalDrawingWidth;
    GetDrawingWidth(&totalDrawingWidth);

    Int32 width;
    GetWidth(&width);
    Float currentDrawPosition = width / 2 - totalDrawingWidth / 2;
    Int32 length;
    mTextChars->GetSize(&length);
    AutoPtr<IRect> bounds;
    GetCharBounds((IRect**)&bounds);

    Int32 bottom;
    bounds->GetBottom(&bottom);
    Int32 top;
    bounds->GetTop(&top);
    Int32 charHeight = (bottom - top);

    Int32 height;
    GetHeight(&height);
    Float yPosition = height / 2;

    Int32 right;
    bounds->GetRight(&right);
    Int32 left;
    bounds->GetLeft(&left);
    Float charLength = right - left;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        mTextChars->Get(i, (IInterface**)&obj);
        AutoPtr<CharState> charState = (CharState*)IObject::Probe(obj);
        Float charWidth;
        charState->Draw(canvas, currentDrawPosition, charHeight, yPosition,
                charLength, &charWidth);
        currentDrawPosition += charWidth;
    }
}

ECode CPasswordTextView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

AutoPtr<IRect> CPasswordTextView::GetCharBounds()
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Int32 density;
    metrics->GetScaledDensity(&density);
    float textHeight = mTextHeightRaw * density;
    mDrawPaint->SetTextSize(textHeight);
    AutoPtr<IRect> bounds;
    CRect::New((IRect**)&bounds);
    mDrawPaint->GetTextBounds(String("0"), 0, 1, bounds);
    return bounds;
}

Float CPasswordTextView::GetDrawingWidth()
{
    Int32 width = 0;
    Int32 length;
    mTextChars->GetSize(&length);
    AutoPtr<IRect> bounds = GetCharBounds();

    Int32 right;
    bounds->GetRight(&right);
    Int32 left;
    bounds->GetLeft(&left);
    Int32 charLength = right - left;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        mTextChars->Get(i, (IInterface**)&obj);
        AutoPtr<CharState> charState = (CharState*)IObject::Probe(obj);
        if (i != 0) {
            width += mCharPadding * charState->mCurrentWidthFactor;
        }
        width += charLength * charState->mCurrentWidthFactor;
    }
    return width;
}

ECode CPasswordTextView::Append(
    /* [in] */ Char32 c)
{
    Int32 visibleChars;
    mTextChars->GetSize(&visibleChars);
    mText = mText + c;
    Int32 newLength = mText.GetLength();
    AutoPtr<CharState> charState;
    if (newLength > visibleChars) {
        charState = ObtainCharState(c);
        mTextChars->Add(TO_IINTERFACE(charState));
    }
    else {
        AutoPtr<IInterface> obj;
        mTextChars->Get(newLength - 1, (IInterface**)&obj);
        charState = (CharState*)IObject::Probe(obj);
        charState->mWhichChar = c;
    }
    charState->StartAppearAnimation();

    // ensure that the previous element is being swapped
    if (newLength > 1) {
        AutoPtr<IInterface> obj;
        mTextChars->Get(newLength - 2, (IInterface**)&obj);
        AutoPtr<CharState> previousState = (CharState*)IObject::Probe(obj);
        if (previousState->mIsDotSwapPending) {
            previousState->SwapToDotWhenAppearFinished();
        }
    }
    UserActivity();
    return NOERROR;
}

void CPasswordTextView::UserActivity()
{
    mPM->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
}

ECode CPasswordTextView::DeleteLastChar()
{
    Int32 length = mText.GetLength();
    if (length > 0) {
        mText = mText.Substring(0, length - 1);
        AutoPtr<IInterface> obj;
        mTextChars->Get(length - 1, (IInterface**)&obj);
        AutoPtr<CharState> charState = (CharState*)IObject::Probe(obj);
        charState->StartRemoveAnimation(0, 0);
    }
    UserActivity();
    return NOERROR;
}

ECode CPasswordTextView::GetText(
    /* [out] */ String* test)
{
    VALIDATE_NOT_NULL(test)

    *test = mText;
    return NOERROR;
}

AutoPtr<CharState> CPasswordTextView::ObtainCharState(
    /* [in] */ Char32 c)
{
    AutoPtr<CharState> charState;

    Boolean res;
    if(mCharPool->IsEmpty(&res), res) {
        charState = new CharState();
    }
    else {
        AutoPtr<IInterface> outface;
        mCharPool->Pop((IInterface**)&outface);
        charState = (CharState*)IObject::Probe(outface);
        charState->Reset();
    }
    charState->mWhichChar = c;
    return charState;
}

ECode CPasswordTextView::Reset(
    /* [in] */ Boolean animated)
{
    mText = String("");
    Int32 length;
    mTextChars->GetSize(&length);
    Int32 middleIndex = (length - 1) / 2;
    Int64 delayPerElement = RESET_DELAY_PER_ELEMENT;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        mTextChars->Get(i, (IInterface**)&obj);
        AutoPtr<CharState> charState = (CharState*)IObject::Probe(obj);
        if (animated) {
            Int32 delayIndex;
            if (i <= middleIndex) {
                delayIndex = i * 2;
            }
            else {
                Int32 distToMiddle = i - middleIndex;
                delayIndex = (length - 1) - (distToMiddle - 1) * 2;
            }
            Int64 startDelay = delayIndex * delayPerElement;
            startDelay = Math::Min(startDelay, RESET_MAX_DELAY);
            Int64 maxDelay = delayPerElement * (length - 1);
            maxDelay = Math::Min(maxDelay, RESET_MAX_DELAY) + DISAPPEAR_DURATION;
            charState->StartRemoveAnimation(startDelay, maxDelay);
            charState->RemoveDotSwapCallbacks();
        }
        else {
            mCharPool->Push(TO_IINTERFACE(charState));
        }
    }
    if (!animated) {
        mTextChars->Clear();
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
