
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/AbsSeekBar.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::R;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 AbsSeekBar::PROGRESS_ANIMATION_DURATION = 250;
const Int32 AbsSeekBar::NO_ALPHA = 0xFF;

CAR_INTERFACE_IMPL(AbsSeekBar, ProgressBar, IAbsSeekBar);

AbsSeekBar::AbsSeekBar()
    : mHasThumbTint(FALSE)
    , mHasThumbTintMode(FALSE)
    , mThumbOffset(0)
    , mSplitTrack(FALSE)
    , mKeyProgressIncrement(1)
    , mDisabledAlpha(0.0f)
    , mScaledTouchSlop(0)
    , mTouchDownX(0.0f)
    , mIsDragging(FALSE)
    , mTouchProgressOffset(0.f)
    , mIsUserSeekable(TRUE)
{
    CRect::New((IRect**)&mTempRect);
}

AbsSeekBar::~AbsSeekBar()
{}

ECode AbsSeekBar::constructor(
    /* [in] */ IContext* context)
{
    return ProgressBar::constructor(context);
}

ECode AbsSeekBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ProgressBar::constructor(context, attrs);
}

ECode AbsSeekBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode AbsSeekBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ProgressBar::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::SeekBar);
    AutoPtr<ITypedArray> a;

    context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    AutoPtr<IDrawable> thumb;
    FAIL_RETURN(a->GetDrawable(R::styleable::SeekBar_thumb,
            (IDrawable**)&thumb));
    SetThumb(thumb);

    Boolean hasValue;
    if (a->HasValue(R::styleable::SeekBar_thumbTintMode, &hasValue), hasValue) {
        Int32 value;
        a->GetInt32(R::styleable::SeekBar_thumbTintMode, -1, &value);
        Drawable::ParseTintMode(value, mThumbTintMode, &mThumbTintMode);
        mHasThumbTintMode = TRUE;
    }

    if (a->HasValue(R::styleable::SeekBar_thumbTint, &hasValue), hasValue) {
        a->GetColorStateList(R::styleable::SeekBar_thumbTint, (IColorStateList**)&mThumbTintList);
        mHasThumbTint = TRUE;
    }

    // Guess thumb offset if thumb != null, but allow layout to override.
    Int32 offset;
    GetThumbOffset(&offset);
    Int32 thumbOffset = 0;
    a->GetDimensionPixelOffset(
            R::styleable::SeekBar_thumbOffset, offset, &thumbOffset);
    SetThumbOffset(thumbOffset);

    a->GetBoolean(R::styleable::SeekBar_splitTrack, FALSE, &mSplitTrack);

    FAIL_RETURN(a->Recycle());

    a = NULL;
    attrIds = TO_ATTRS_ARRAYOF(R::styleable::Theme);
    FAIL_RETURN(context->ObtainStyledAttributes(
            attrs, attrIds, 0, 0, (ITypedArray**)&a));
    FAIL_RETURN(a->GetFloat(
        R::styleable::Theme_disabledAlpha,
            0.5f, &mDisabledAlpha));
    ECode ec = a->Recycle();

    ApplyThumbTint();

    CViewConfiguration::Get(context)->GetScaledTouchSlop(&mScaledTouchSlop);

    return ec;
}

ECode AbsSeekBar::SetThumb(
    /* [in] */ IDrawable* thumb)
{
    Boolean needUpdate = FALSE;
    // This way, calling setThumb again with the same bitmap will result in
    // it recalcuating mThumbOffset (if for example it the bounds of the
    // drawable changed)
    if (mThumb != NULL && thumb != mThumb) {
        mThumb->SetCallback(NULL);
        needUpdate = TRUE;
    } else {
        needUpdate = FALSE;
    }

    if (thumb != NULL) {
        thumb->SetCallback(this);
        Boolean res;
        if (CanResolveLayoutDirection(&res), res) {
            Int32 layoutDirection;
            GetLayoutDirection(&layoutDirection);
            thumb->SetLayoutDirection(layoutDirection);
        }
        // Assuming the thumb drawable is symmetric, set the thumb offset
        // such that the thumb will hang halfway off either edge of the
        // progress bar.
        thumb->GetIntrinsicWidth(&mThumbOffset);
        mThumbOffset /= 2;

        if (needUpdate) {
            Int32 w1, w2, h1, h2;
            thumb->GetIntrinsicWidth(&w1);
            mThumb->GetIntrinsicWidth(&w2);
            thumb->GetIntrinsicHeight(&h1);
            mThumb->GetIntrinsicHeight(&h2);
            // If we're updating get the new states
            if (w1 != w2 || h1 != h2) {
                    RequestLayout();
            }
        }
    }

    mThumb = thumb;

    ApplyThumbTint();
    Invalidate();
    if (needUpdate) {
        Int32 width, height;
        updateThumbAndTrackPos((GetWidth(&width), width), (GetHeight(&height), height));
        Boolean res;
        if (thumb != NULL && (thumb->IsStateful(&res), res)) {
            // Note that if the states are different this won't work.
            // For now, let's consider that an app bug.
            AutoPtr<ArrayOf<Int32> > state;
            GetDrawableState((ArrayOf<Int32>**)&state);
            Boolean res;
            thumb->SetState(state ,&res);
        }
    }
    return NOERROR;
}

ECode AbsSeekBar::GetThumb(
    /* [out] */ IDrawable** thumb)
{
    VALIDATE_NOT_NULL(thumb);
    *thumb = mThumb;
    REFCOUNT_ADD(*thumb);
    return NOERROR;
}

ECode AbsSeekBar::SetThumbTintList(
    /* [in] */ IColorStateList* tint)
{
    mThumbTintList = tint;
    mHasThumbTint = TRUE;

    ApplyThumbTint();
    return NOERROR;
}

ECode AbsSeekBar::GetThumbTintList(
    /* [out] */ IColorStateList** tint)
{
    VALIDATE_NOT_NULL(tint);
    *tint = mThumbTintList;
    REFCOUNT_ADD(*tint);
    return NOERROR;
}

ECode AbsSeekBar::SetThumbTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    mThumbTintMode = tintMode;
    mHasThumbTintMode = TRUE;

    ApplyThumbTint();
    return NOERROR;
}

ECode AbsSeekBar::GetThumbTintMode(
    /* [out] */ PorterDuffMode* tintMode)
{
    VALIDATE_NOT_NULL(tintMode);
    *tintMode = mThumbTintMode;
    return NOERROR;
}

void AbsSeekBar::ApplyThumbTint()
{
    if (mThumb != NULL && (mHasThumbTint || mHasThumbTintMode)) {
        mThumb->Mutate();

        if (mHasThumbTint) {
            mThumb->SetTintList(mThumbTintList);
        }

        if (mHasThumbTintMode) {
            mThumb->SetTintMode(mThumbTintMode);
        }
    }
}

ECode AbsSeekBar::GetThumbOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = mThumbOffset;
    return NOERROR;
}

ECode AbsSeekBar::SetThumbOffset(
    /* [in] */ Int32 thumbOffset)
{
    mThumbOffset = thumbOffset;
    Invalidate();

    return NOERROR;
}

ECode AbsSeekBar::SetSplitTrack(
    /* [in] */ Boolean splitTrack)
{
    mSplitTrack = splitTrack;
    Invalidate();
    return NOERROR;
}

ECode AbsSeekBar::GetSplitTrack(
    /* [out] */ Boolean* track)
{
    VALIDATE_NOT_NULL(track);
    *track = mSplitTrack;
    return NOERROR;
}

ECode AbsSeekBar::SetKeyProgressIncrement(
    /* [in] */ Int32 increment)
{
    mKeyProgressIncrement = increment < 0 ? -increment : increment;

    return NOERROR;
}

ECode AbsSeekBar::GetKeyProgressIncrement(
    /* [out] */ Int32* increment)
{
    VALIDATE_NOT_NULL(increment);
    *increment = mKeyProgressIncrement;
    return NOERROR;
}

ECode AbsSeekBar::SetMax(
    /* [in] */ Int32 max)
{
    {    AutoLock syncLock(this);
        ProgressBar::SetMax(max);

        Int32 max;
        GetMax(&max);
        if ((mKeyProgressIncrement == 0) || ( max / mKeyProgressIncrement > 20)) {
            // It will take the user too long to change this via keys, change it
            // to something more reasonable
            SetKeyProgressIncrement(Elastos::Core::Math::Max(1, Elastos::Core::Math::Round((Float) max / 20)));
        }

        return NOERROR;
    }
    return NOERROR;
}

Boolean AbsSeekBar::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return who == mThumb || ProgressBar::VerifyDrawable(who);
}

ECode AbsSeekBar::JumpDrawablesToCurrentState()
{
    ProgressBar::JumpDrawablesToCurrentState();
    if (mThumb != NULL) {
        mThumb->JumpToCurrentState();
    }
    return NOERROR;
}

ECode AbsSeekBar::DrawableStateChanged()
{
    FAIL_RETURN(ProgressBar::DrawableStateChanged());

    AutoPtr<IDrawable> progressDrawable;
    GetProgressDrawable((IDrawable**)&progressDrawable);
    if (progressDrawable != NULL) {
        Boolean res;
        progressDrawable->SetAlpha((IsEnabled(&res), res) ? NO_ALPHA : (Int32) (NO_ALPHA * mDisabledAlpha));
    }

    AutoPtr<IDrawable> thumb = mThumb;
    Boolean stateful;
    if (thumb != NULL && (thumb->IsStateful(&stateful), stateful)) {
        AutoPtr<ArrayOf<Int32> > state;
        GetDrawableState((ArrayOf<Int32>**)&state);
        Boolean res;
        thumb->SetState(state, &res);
    }

    return NOERROR;
}

ECode AbsSeekBar::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    ProgressBar::DrawableHotspotChanged(x, y);

    if (mThumb != NULL) {
        mThumb->SetHotspot(x, y);
    }
    return NOERROR;
}

void AbsSeekBar::OnProgressRefresh(
    /* [in] */ Float scale,
    /* [in] */ Boolean fromUser)
{
    ProgressBar::OnProgressRefresh(scale, fromUser);

    AutoPtr<IDrawable> thumb = mThumb;
    if (!IsAnimationRunning()) {
        SetThumbPos(scale);
    }
}

void AbsSeekBar::OnAnimatePosition(
    /* [in] */ Float scale,
    /* [in] */ Boolean fromUser)
{
    SetThumbPos(scale);
}

ECode AbsSeekBar::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    ProgressBar::OnSizeChanged(w, h, oldw, oldh);
    updateThumbAndTrackPos(w, h);
    return NOERROR;
}

void AbsSeekBar::updateThumbAndTrackPos(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    const AutoPtr<IDrawable> track = GetCurrentDrawable();
    AutoPtr<IDrawable> thumb = mThumb;

    // The max height does not incorporate padding, whereas the height
    // parameter does
    const Int32 trackHeight = Elastos::Core::Math::Min(mMaxHeight, h - mPaddingTop - mPaddingBottom);
    Int32 height;
    const Int32 thumbHeight = thumb == NULL ? 0 : (thumb->GetIntrinsicHeight(&height), height);

    // Apply offset to whichever item is taller.
    Int32 trackOffset;
    Int32 thumbOffset;
    if (thumbHeight > trackHeight) {
        trackOffset = (thumbHeight - trackHeight) / 2;
        thumbOffset = 0;
    }
    else {
        trackOffset = 0;
        thumbOffset = (trackHeight - thumbHeight) / 2;
    }

    if (track != NULL) {
        track->SetBounds(0, trackOffset, w - mPaddingRight - mPaddingLeft,
                h - mPaddingBottom - trackOffset - mPaddingTop);
    }

    if (thumb != NULL) {
        SetThumbPos(w, thumb, GetScale(), thumbOffset);
    }
}

Float AbsSeekBar::GetScale()
{
    Int32 max;
    GetMax(&max);
    Int32 progress;
    GetProgress(&progress);
    return max > 0 ? (Float) progress / (Float) max : 0;
}

void AbsSeekBar::SetThumbPos(
    /* [in] */ Float scale)
{
    const AutoPtr<IDrawable> thumb = mThumb;
    if (thumb != NULL) {
        Int32 width;
        GetWidth(&width);
        SetThumbPos(width, thumb, scale, Elastos::Core::Math::INT32_MIN_VALUE);
        // Since we draw translated, the drawable's bounds that it signals
        // for invalidation won't be the actual bounds we want invalidated,
        // so just invalidate this whole view.
        Invalidate();
    }
}

void AbsSeekBar::SetThumbPos(
    /* [in] */ Int32 w,
    /* [in] */ IDrawable* thumb,
    /* [in] */ Float scale,
    /* [in] */ Int32 offset)
{
    Int32 available = w - mPaddingLeft - mPaddingRight;
    Int32 thumbWidth;
    thumb->GetIntrinsicWidth(&thumbWidth);
    Int32 thumbHeight;
    thumb->GetIntrinsicHeight(&thumbHeight);
    available -= thumbWidth;

    // The extra space for the thumb to move on the track
    available += mThumbOffset * 2;

    const Int32 thumbPos = (Int32) (scale * available + 0.5f);

    Int32 top, bottom;
    if (offset == Elastos::Core::Math::INT32_MIN_VALUE) {
        AutoPtr<IRect> oldBounds;
        thumb->GetBounds((IRect**)&oldBounds);
        oldBounds->GetTop(&top);
        oldBounds->GetBottom(&bottom);
    }
    else {
        top = offset;
        bottom = offset + thumbHeight;
    }

    Boolean res;
    const Int32 left = ((IsLayoutRtl(&res), res) && mMirrorForRtl) ? available - thumbPos : thumbPos;
    const Int32 right = left + thumbWidth;

    AutoPtr<IDrawable> background;
    GetBackground((IDrawable**)&background);
    if (background != NULL) {
        AutoPtr<IRect> bounds;
        thumb->GetBounds((IRect**)&bounds);
        const Int32 offsetX = mPaddingLeft - mThumbOffset;
        const Int32 offsetY = mPaddingTop;
        background->SetHotspotBounds(left + offsetX, top + offsetY,
                right + offsetX, bottom + offsetY);
    }

    // Canvas will be translated, so 0,0 is where we start drawing
    thumb->SetBounds(left, top, right, bottom);
}

ECode AbsSeekBar::OnResolveDrawables(
    /* [in] */ Int32 layoutDirection)
{
    ProgressBar::OnResolveDrawables(layoutDirection);

    if (mThumb != NULL) {
        mThumb->SetLayoutDirection(layoutDirection);
    }
    return NOERROR;
}

void AbsSeekBar::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    {    AutoLock syncLock(this);
        ProgressBar::OnDraw(canvas);
        DrawThumb(canvas);
    }
}

void AbsSeekBar::DrawTrack(
    /* [in] */ ICanvas* canvas)
{
    const AutoPtr<IDrawable> thumbDrawable = mThumb;
    if (thumbDrawable != NULL && mSplitTrack) {
        AutoPtr<IInsets> insets;
        thumbDrawable->GetOpticalInsets((IInsets**)&insets);
        AutoPtr<IRect> tempRect = mTempRect;
        thumbDrawable->CopyBounds(tempRect);
        tempRect->Offset(mPaddingLeft - mThumbOffset, mPaddingTop);
        AutoPtr<CRect> rect = (CRect*)tempRect.Get();
        Int32 left, right;
        insets->GetLeft(&left);
        insets->GetRight(&right);
        rect->mLeft += left;
        rect->mRight -= right;

        Int32 saveCount;
        canvas->Save(&saveCount);
        Boolean res;
        canvas->ClipRect(tempRect, Elastos::Droid::Graphics::RegionOp_DIFFERENCE, &res);
        ProgressBar::DrawTrack(canvas);
        canvas->RestoreToCount(saveCount);
    }
    else {
        ProgressBar::DrawTrack(canvas);
    }
}

void AbsSeekBar::DrawThumb(
    /* [in] */ ICanvas* canvas)
{
    if (mThumb != NULL) {
        Int32 count;
        canvas->Save(&count);
        // Translate the padding. For the x, we need to allow the thumb to
        // draw in its extra space
        canvas->Translate(mPaddingLeft - mThumbOffset, mPaddingTop);
        mThumb->Draw(canvas);
        canvas->Restore();
    }
}

ECode AbsSeekBar::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    {    AutoLock syncLock(this);
        AutoPtr<IDrawable> d = GetCurrentDrawable();

        Int32 h;
        Int32 thumbHeight = mThumb == NULL ? 0 : (mThumb->GetIntrinsicHeight(&h), h);
        Int32 dw = 0;
        Int32 dh = 0;
        if (d != NULL) {
            using Elastos::Core::Math;
            dw = Math::Max(mMinWidth, Math::Min(mMaxWidth, (d->GetIntrinsicWidth(&dw), dw)));
            dh = Math::Max(mMinHeight, Math::Min(mMaxHeight, (d->GetIntrinsicHeight(&dh), dh)));
            dh = Math::Max(thumbHeight, dh);
        }
        dw += mPaddingLeft + mPaddingRight;
        dh += mPaddingTop + mPaddingBottom;

        SetMeasuredDimension(ResolveSizeAndState(dw, widthMeasureSpec, 0),
                ResolveSizeAndState(dh, heightMeasureSpec, 0));
    }
    return NOERROR;
}

ECode AbsSeekBar::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean result;
    if (!mIsUserSeekable || (IsEnabled(&result), !result)) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 action;
    switch (event->GetAction(&action), action) {
        case IMotionEvent::ACTION_DOWN:
            if (IsInScrollingContainer(&result), result) {
                event->GetX(&mTouchDownX);
            }
            else {
                SetPressed(TRUE);
                if (mThumb != NULL) {
                    AutoPtr<IRect> bounds;
                    mThumb->GetBounds((IRect**)&bounds);
                    Invalidate(bounds); // This may be within the padding region
                }
                OnStartTrackingTouch();
                TrackTouchEvent(event);
                AttemptClaimDrag();
            }
            break;

        case IMotionEvent::ACTION_MOVE:
            if (mIsDragging) {
                TrackTouchEvent(event);
            }
            else {
                Float x;
                event->GetX(&x);
                if (Elastos::Core::Math::Abs(x - mTouchDownX) > mScaledTouchSlop) {
                    SetPressed(TRUE);
                    if (mThumb != NULL) {
                        AutoPtr<IRect> bounds;
                        mThumb->GetBounds((IRect**)&bounds);
                        Invalidate(bounds); // This may be within the padding region
                    }
                    OnStartTrackingTouch();
                    TrackTouchEvent(event);
                    AttemptClaimDrag();
                }
            }
            break;

        case IMotionEvent::ACTION_UP:
            if (mIsDragging) {
                TrackTouchEvent(event);
                OnStopTrackingTouch();
                SetPressed(FALSE);
            }
            else {
                // Touch up when we never crossed the touch slop threshold should
                // be interpreted as a tap-seek to that location.
                OnStartTrackingTouch();
                TrackTouchEvent(event);
                OnStopTrackingTouch();
            }
            // ProgressBar doesn't know to repaint the thumb drawable
            // in its inactive state when the touch stops (because the
            // value has not apparently changed)
            Invalidate();
            break;

        case IMotionEvent::ACTION_CANCEL:
            if (mIsDragging) {
                OnStopTrackingTouch();
                SetPressed(FALSE);
            }
            Invalidate(); // see above explanation
            break;
    }
    *res = TRUE;
    return NOERROR;
}

void AbsSeekBar::SetHotspot(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    AutoPtr<IDrawable> bg;
    GetBackground((IDrawable**)&bg);

    if (bg != NULL) {
        bg->SetHotspot(x, y);
    }
}

void AbsSeekBar::TrackTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    Int32 width;
    GetWidth(&width);
    Int32 available = width - mPaddingLeft - mPaddingRight;
    Float _x;
    event->GetX(&_x);
    Int32 x = (Int32)_x;
    Float scale;
    Float progress = 0;

    Boolean res;
    if ((IsLayoutRtl(&res), res) && mMirrorForRtl) {
        if (x > width - mPaddingRight) {
            scale = 0.0f;
        }
        else if (x < mPaddingLeft) {
            scale = 1.0f;
        }
        else {
            scale = (Float)(available - x + mPaddingLeft) / (Float)available;
            progress = mTouchProgressOffset;
        }
    }
    else {
        if (x < mPaddingLeft) {
            scale = 0.0f;
        }
        else if (x > width - mPaddingRight) {
            scale = 1.0f;
        }
        else {
            scale = (Float)(x - mPaddingLeft) / (Float)available;
            progress = mTouchProgressOffset;
        }
    }

    Int32 max;
    GetMax(&max);
    progress += scale * max;

    Float y;
    event->GetY(&y);
    SetHotspot(x, (Int32)y);
    Int32 pTmp;
    GetProgress(&pTmp);
    SetProgress(UpdateTouchProgress(pTmp, (Int32) progress), TRUE);
}

void AbsSeekBar::AttemptClaimDrag()
{
    if (mParent != NULL) {
        mParent->RequestDisallowInterceptTouchEvent(TRUE);
    }
}

void AbsSeekBar::OnStartTrackingTouch()
{
    mIsDragging = TRUE;
}

void AbsSeekBar::OnStopTrackingTouch()
{
    mIsDragging = FALSE;
}

void AbsSeekBar::OnKeyChange()
{
}

ECode AbsSeekBar::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Int32 max;
    Boolean result;
    if (IsEnabled(&result), result) {
        Int32 progress;
        GetProgress(&progress);
        switch (keyCode) {
            case IKeyEvent::KEYCODE_DPAD_LEFT:
                if (progress <= 0) break;
                AnimateSetProgress(progress - mKeyProgressIncrement);
                OnKeyChange();
                *res = TRUE;
                return NOERROR;

            case IKeyEvent::KEYCODE_DPAD_RIGHT:
                if (progress >= (GetMax(&max), max)) break;
                AnimateSetProgress(progress + mKeyProgressIncrement);
                OnKeyChange();
                *res = TRUE;
                return NOERROR;
        }
    }

    return ProgressBar::OnKeyDown(keyCode, event, res);
}

Boolean AbsSeekBar::IsAnimationRunning()
{
    Boolean res;
    return mPositionAnimator != NULL && (IAnimator::Probe(mPositionAnimator)->IsRunning(&res), res);
}

ECode AbsSeekBar::SetProgress(
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    if (IsAnimationRunning()) {
        IAnimator::Probe(mPositionAnimator)->Cancel();
    }
    return ProgressBar::SetProgress(progress, fromUser);
}

void AbsSeekBar::AnimateSetProgress(
    /* [in] */ Int32 progress)
{
    Int32 pro;
    Float curProgress = IsAnimationRunning() ? GetAnimationPosition() : (GetProgress(&pro), pro);

    Int32 max;
    GetMax(&max);
    if (progress < 0) {
        progress = 0;
    }
    else if (progress > max) {
        progress = max;
    }
    SetProgressValueOnly(progress);

    AutoPtr< ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
    (*values)[0] = curProgress;
    (*values)[1] = (Float) progress;

    mPositionAnimator = ObjectAnimator::OfFloat(TO_IINTERFACE(this),
            String("animationPosition"), values);
    IAnimator::Probe(mPositionAnimator)->SetDuration(PROGRESS_ANIMATION_DURATION);
    mPositionAnimator->SetAutoCancel(TRUE);
    IAnimator::Probe(mPositionAnimator)->Start();
}

Int32 AbsSeekBar::UpdateTouchProgress(
    /* [in] */ Int32 lastProgress,
    /* [in] */ Int32 newProgress)
{
    return newProgress;
}

ECode AbsSeekBar::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ProgressBar::OnInitializeAccessibilityEvent(event);
    String sName("AbsSeekBar");
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(sName, (ICharSequence**)&className));
    IAccessibilityRecord::Probe(event)->SetClassName(className);
    return NOERROR;
}

ECode AbsSeekBar::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ProgressBar::OnInitializeAccessibilityNodeInfo(info);
    String sName("AbsSeekBar");
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(sName, (ICharSequence**)&className));
    info->SetClassName(className);

    Boolean res;
    if (IsEnabled(&res), res) {
        Int32 progress;
        GetProgress(&progress);
        if (progress > 0) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
        }
        Int32 max;
        GetMax(&max);
        if (progress < max) {
            info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
        }
    }
    return NOERROR;
}

ECode AbsSeekBar::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;

    Boolean result;
    if (ProgressBar::PerformAccessibilityAction(action, arguments, &result), result) {
        *res = TRUE;
        return NOERROR;
    }
    if (IsEnabled(&result), !result) {
        return NOERROR;
    }
    Int32 progress;
    GetProgress(&progress);
    Int32 max;
    GetMax(&max);
    Int32 increment = Elastos::Core::Math::Max(1, Elastos::Core::Math::Round((Float) max / 5));
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD: {
            if (progress <= 0) {
                return NOERROR;
            }
            SetProgress(progress - increment, TRUE);
            OnKeyChange();
            *res = TRUE;
            return NOERROR;
        }
        case IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD: {
            if (progress >= max) {
                return NOERROR;
            }
            SetProgress(progress + increment, TRUE);
            OnKeyChange();
            *res = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode AbsSeekBar::OnRtlPropertiesChanged(
    /* [in] */ Int32 layoutDirection)
{
    ProgressBar::OnRtlPropertiesChanged(layoutDirection);

    const AutoPtr<IDrawable> thumb = mThumb;
    if (thumb != NULL) {
        Int32 width;
        GetWidth(&width);
        SetThumbPos(width, thumb, GetScale(), Elastos::Core::Math::INT32_MIN_VALUE);

        // * Since we draw translated, the drawable's bounds that it signals
        // * for invalidation won't be the actual bounds we want invalidated,
        // * so just invalidate this whole view.

        Invalidate();
    }
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace Widget
