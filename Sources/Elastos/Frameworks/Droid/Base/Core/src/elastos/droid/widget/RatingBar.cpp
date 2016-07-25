
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/RatingBar.h"
#include "elastos/droid/graphics/drawable/shapes/CRectShape.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Graphics::Drawable::Shapes::CRectShape;
using Elastos::Droid::Graphics::Drawable::Shapes::IRectShape;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::R;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

const String RatingBar::RATINGBAR_NAME("RatingBar");

CAR_INTERFACE_IMPL(RatingBar, AbsSeekBar, IRatingBar);

RatingBar::RatingBar()
    : mNumStars(5)
    , mProgressOnStartTracking(0)
{
}

RatingBar::~RatingBar()
{
}

ECode RatingBar::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode RatingBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::seekBarStyle);
}

ECode RatingBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode RatingBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    AbsSeekBar::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::RatingBar);

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));
    Int32 numStars = 0;
    a->GetInt32(R::styleable::RatingBar_numStars, mNumStars, &numStars);
    Boolean res = FALSE;
    a->GetBoolean(R::styleable::RatingBar_isIndicator, !mIsUserSeekable, &res);
    SetIsIndicator(res);
    Float rating = 0, stepSize = 0;
    a->GetFloat(R::styleable::RatingBar_rating, -1, &rating);
    a->GetFloat(R::styleable::RatingBar_stepSize, -1, &stepSize);
    a->Recycle();

    if(numStars > 0 && numStars != mNumStars) {
        SetNumStars(numStars);
    }

    if(stepSize > 0) {
        SetStepSize(stepSize);
    }
    else {
        SetStepSize(0.5f);
    }

    if(rating > 0) {
        SetRating(rating);
    }

    // A touch inside a star fill up to that fractional area (slightly more
    // than 1 so boundaries round up).
    mTouchProgressOffset = 1.1f;

    return NOERROR;
}

ECode RatingBar::SetOnRatingBarChangeListener(
    /* [in] */ IOnRatingBarChangeListener* listener)
{
    mOnRatingBarChangeListener = listener;
    return NOERROR;
}

ECode RatingBar::GetOnRatingBarChangeListener(
    /* [out] */ IOnRatingBarChangeListener** listener)
{
    VALIDATE_NOT_NULL(listener);
    *listener = mOnRatingBarChangeListener;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode RatingBar::SetIsIndicator(
    /* [in] */ Boolean isIndicator)
{
    mIsUserSeekable = !isIndicator;
    SetFocusable(!isIndicator);
    return NOERROR;
}

ECode RatingBar::IsIndicator(
    /* [out] */ Boolean* isIndicator)
{
    VALIDATE_NOT_NULL(isIndicator);
    *isIndicator = !mIsUserSeekable;
    return NOERROR;
}

ECode RatingBar::SetNumStars(
    /* [in] */ Int32 numStars)
{
    if (numStars < 0) {
        return NOERROR;
    }
    mNumStars = numStars;
    RequestLayout();

    return NOERROR;
}

ECode RatingBar::GetNumStars(
    /* [out] */ Int32* numStars)
{
    VALIDATE_NOT_NULL(numStars);
    *numStars = mNumStars;
    return NOERROR;
}

ECode RatingBar::SetRating(
    /* [in] */ Float rating)
{
    ProgressBar::SetProgress(Elastos::Core::Math::Round(rating * GetProgressPerStar()));
    return NOERROR;
}

ECode RatingBar::GetRating(
    /* [out] */ Float* rating)
{
    VALIDATE_NOT_NULL(rating);
    Int32 progress;
    GetProgress(&progress);
    *rating = progress / GetProgressPerStar();
    return NOERROR;
}

ECode RatingBar::SetStepSize(
    /* [in] */ Float stepSize)
{
    if (stepSize <= 0) {
        return NOERROR;
    }
    Float newMax = mNumStars / stepSize;
    Int32 max;
    GetMax(&max);
    Int32 progress;
    GetProgress(&progress);
    Int32 newProgress = (Int32)(newMax / max * progress);
    SetMax((Int32)newMax);
    ProgressBar::SetProgress(newProgress);
    return NOERROR;
}

ECode RatingBar::GetStepSize(
    /* [out] */ Float* stepSize)
{
    VALIDATE_NOT_NULL(stepSize);

    Int32 max;
    GetMax(&max);
    Int32 numStars;
    GetNumStars(&numStars);
    *stepSize = (Float)numStars / max;
    return NOERROR;
}

Float RatingBar::GetProgressPerStar()
{
    if (mNumStars > 0) {
        Int32 max;
        GetMax(&max);
        return 1.f * max / mNumStars;
    }
    else {
        return 1.f;
    }
}

AutoPtr<IShape> RatingBar::GetDrawableShape()
{
    AutoPtr<IRectShape> shape;
    CRectShape::New((IRectShape**)&shape);
    return IShape::Probe(shape);
}

ECode RatingBar::OnProgressRefresh(
    /* [in] */ Float scale,
    /* [in] */ Boolean fromUser)
{
    AbsSeekBar::OnProgressRefresh(scale, fromUser);
    Int32 progress;
    GetProgress(&progress);
    UpdateSecondaryProgress(progress);
    if (!fromUser) {
        DispatchRatingChange(FALSE);
    }
    return NOERROR;
}

void RatingBar::UpdateSecondaryProgress(
    /* [in] */ Int32 progress)
{
    Float ratio = GetProgressPerStar();
    if (ratio > 0) {
        Float progressInStars = progress / ratio;
        Int32 secondaryProgress = (Int32)(Elastos::Core::Math::Ceil(progressInStars) * ratio);
        SetSecondaryProgress(secondaryProgress);
    }
}

ECode RatingBar::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    {    AutoLock syncLock(this);
        AbsSeekBar::OnMeasure(widthMeasureSpec, heightMeasureSpec);

        if(mSampleTile != NULL) {
            Int32 width = 0;
            mSampleTile->GetWidth(&width);
            width *= mNumStars;
            Int32 height;
            GetMeasuredHeight(&height);
            SetMeasuredDimension(ResolveSizeAndState(width, widthMeasureSpec, 0), height);
        }
    }
    return NOERROR;
}

ECode RatingBar::OnStartTrackingTouch()
{
    GetProgress(&mProgressOnStartTracking);
    AbsSeekBar::OnStartTrackingTouch();
    return NOERROR;
}

ECode RatingBar::OnStopTrackingTouch()
{
    AbsSeekBar::OnStopTrackingTouch();
    Int32 progress;
    GetProgress(&progress);
    if (progress != mProgressOnStartTracking) {
        DispatchRatingChange(TRUE);
    }
    return NOERROR;
}

ECode RatingBar::OnKeyChange()
{
    AbsSeekBar::OnKeyChange();
    DispatchRatingChange(TRUE);
    return NOERROR;
}

ECode RatingBar::AnimateSetProgress(
    /* [in] */ Int32 progress)
{
    return NOERROR;
}

ECode RatingBar::DispatchRatingChange(
    /* [in] */ Boolean fromUser)
{
    if (mOnRatingBarChangeListener != NULL) {
        Float rating;
        GetRating(&rating);
        mOnRatingBarChangeListener->OnRatingChanged(this, rating, fromUser);
    }
    return NOERROR;
}

ECode RatingBar::SetMax(
    /* [in] */ Int32 max)
{
    {    AutoLock syncLock(this);
        if (max <= 0) {
            return NOERROR;
        }
        AbsSeekBar::SetMax(max);
        return NOERROR;
    }
    return NOERROR;
}

ECode RatingBar::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AbsSeekBar::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(RATINGBAR_NAME, (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode RatingBar::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AbsSeekBar::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(RATINGBAR_NAME, (ICharSequence**)&seq));
    info->SetClassName(seq);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
