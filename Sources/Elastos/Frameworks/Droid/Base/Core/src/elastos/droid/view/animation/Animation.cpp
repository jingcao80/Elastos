
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/Animation.h"
#ifdef DROID_CORE
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/animation/CTransformation.h"
#include "elastos/droid/view/animation/CAccelerateDecelerateInterpolator.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/utility/CTypedValue.h"
#endif
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::R;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::Math;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

static Boolean InitUSECLOSEGUARD()
{
    Boolean value;
    SystemProperties::GetBoolean(String("ro.monkey"), FALSE, &value);
    return value;
}

Boolean Animation::USE_CLOSEGUARD = InitUSECLOSEGUARD();
const Int32 Animation::SetListenerHandlerRunable::ONSTART_TYPE = 0;
const Int32 Animation::SetListenerHandlerRunable::ONREPEAT_TYPE = 1;
const Int32 Animation::SetListenerHandlerRunable::ONEND_TYPE = 2;

/* Animation::SetListenerHandlerRunable */
Animation::SetListenerHandlerRunable::SetListenerHandlerRunable(
    /* [in] */ Animation* host,
    /* [in] */ Int32 type)
    : mHost(host)
    , mType(type)
{}

Animation::SetListenerHandlerRunable::~SetListenerHandlerRunable()
{}

ECode Animation::SetListenerHandlerRunable::Run()
{
    if (mHost->mListener) {
        switch(mType) {
            case ONSTART_TYPE:
                mHost->mListener->OnAnimationStart((IAnimation*)mHost);
                break;
            case ONREPEAT_TYPE:
                mHost->mListener->OnAnimationRepeat((IAnimation*)mHost);
                break;
            case ONEND_TYPE:
                mHost->mListener->OnAnimationEnd((IAnimation*)mHost);
                break;
            default:
                break;
        }
    }
    return NOERROR;
}

/* Animation::Description */
Animation::Description::Description()
    : mType(0)
    , mValue(0.f)
{}

Animation::Description::~Description()
{}

ECode Animation::Description::constructor()
{
    return NOERROR;
}

AutoPtr<Animation::Description> Animation::Description::ParseValue(
    /* [in] */ ITypedValue* value)
{
    AutoPtr<Description> d = new Description();
    if (value == NULL) {
        d->mType = IAnimation::ABSOLUTE;
        d->mValue = 0;
    }
    else {
        Int32 type;
        value->GetType(&type);
        if (type == ITypedValue::TYPE_FRACTION) {
            Int32 data;
            value->GetData(&data);
            d->mType = ((data & ITypedValue::COMPLEX_UNIT_MASK)
                    == ITypedValue::COMPLEX_UNIT_FRACTION_PARENT) ?
                    IAnimation::RELATIVE_TO_PARENT : IAnimation::RELATIVE_TO_SELF;
            d->mValue = CTypedValue::ComplexToFloat(data);
            return d;
        }
        else if (type ==ITypedValue::TYPE_FLOAT) {
            d->mType = IAnimation::ABSOLUTE;
            value->GetFloat(&d->mValue);
            return d;
        }
        else if (type >= ITypedValue::TYPE_FIRST_INT &&
                type <= ITypedValue::TYPE_LAST_INT) {
            d->mType = IAnimation::ABSOLUTE;
            Int32 data;
            value->GetData(&data);
            d->mValue = (Float)data;
            return d;
        }
    }

    d->mType = IAnimation::ABSOLUTE;
    d->mValue = 0.0f;

    return d;
}

/* Animation */
CAR_INTERFACE_IMPL_2(Animation, Object, IAnimation, ICloneable);

Animation::Animation()
    : mEnded(FALSE)
    , mStarted(FALSE)
    , mCycleFlip(FALSE)
    , mInitialized(FALSE)
    , mFillBefore(TRUE)
    , mFillAfter(FALSE)
    , mFillEnabled(FALSE)
    , mStartTime(-1)
    , mStartOffset(0)
    , mDuration(0)
    , mRepeatCount(0)
    , mRepeated(0)
    , mRepeatMode(IAnimation::RESTART)
    , mZAdjustment(0)
    , mBackgroundColor(0)
    , mScaleFactor(1.0f)
    , mDetachWallpaper(FALSE)
    , mMore(TRUE)
    , mOneMoreTime(TRUE)
{
    ASSERT_SUCCEEDED(CRectF::New((IRectF**)&mPreviousRegion));
    ASSERT_SUCCEEDED(CRectF::New((IRectF**)&mRegion));
    ASSERT_SUCCEEDED(CTransformation::New((ITransformation**)&mTransformation));
    ASSERT_SUCCEEDED(CTransformation::New((ITransformation**)&mPreviousTransformation));
    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mGuard);
}

Animation::~Animation()
{
    // try {
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }
    // } finally {
    //     super.finalize();
    // }
}

ECode Animation::constructor()
{
    EnsureInterpolator();
    return NOERROR;
}

ECode Animation::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Animation);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    Int32 data;
    a->GetInt32(R::styleable::Animation_duration, 0, &data);
    SetDuration((Int64)data);
    a->GetInt32(R::styleable::Animation_startOffset, 0, &data);
    SetStartOffset((Int64)data);

    Boolean flag;
    a->GetBoolean(R::styleable::Animation_fillEnabled, mFillEnabled, &flag);
    SetFillEnabled(flag);
    a->GetBoolean(R::styleable::Animation_fillBefore, mFillBefore, &flag);
    SetFillBefore(flag);
    a->GetBoolean(R::styleable::Animation_fillAfter, mFillAfter, &flag);
    SetFillAfter(flag);

    a->GetInt32(R::styleable::Animation_repeatCount, mRepeatCount, &data);
    SetRepeatCount(data);
    a->GetInt32(R::styleable::Animation_repeatMode, IAnimation::RESTART, &data);
    SetRepeatMode(data);
    a->GetInt32(R::styleable::Animation_zAdjustment, IAnimation::ZORDER_NORMAL, &data);
    SetZAdjustment(data);

    a->GetInt32(R::styleable::Animation_background, 0, &data);
    SetBackgroundColor(data);

    a->GetBoolean(R::styleable::Animation_detachWallpaper, FALSE, &flag);
    SetDetachWallpaper(flag);

    Int32 resID;
    a->GetResourceId(R::styleable::Animation_interpolator, 0, &resID);

    a->Recycle();

    if (resID > 0) {
        SetInterpolator(context, resID);
    }

    EnsureInterpolator();
    return NOERROR;
}

ECode Animation::CloneImpl(
    /* [in] */ IAnimation* object)
{
    Animation* animation = (Animation*)object;

    CRectF::New((IRectF**)&animation->mPreviousRegion);
    CRectF::New((IRectF**)&animation->mRegion);
    CTransformation::New((ITransformation**)&animation->mTransformation);
    CTransformation::New((ITransformation**)&animation->mPreviousTransformation);
    return NOERROR;
}

ECode Animation::Reset()
{
    mPreviousRegion->SetEmpty();
    mPreviousTransformation->Clear();
    mInitialized = FALSE;
    mCycleFlip = FALSE;
    mRepeated = 0;
    mMore = TRUE;
    mOneMoreTime = TRUE;
    mListenerHandler = NULL;

    return NOERROR;
}

ECode Animation::Cancel()
{
    if (mStarted && !mEnded) {
        FireAnimationEnd();
        mEnded = TRUE;

        mGuard->Close();
    }
    // Make sure we move the animation to the end
    mStartTime = Elastos::Core::Math::INT64_MIN_VALUE;
    mMore = mOneMoreTime = FALSE;

    return NOERROR;
}

ECode Animation::Detach()
{
    if (mStarted && !mEnded) {
        mEnded = TRUE;

        mGuard->Close();

        FireAnimationEnd();
    }

    return NOERROR;
}

ECode Animation::IsInitialized(
    /* [out] */ Boolean* init)
{
    VALIDATE_NOT_NULL(init);
    *init = mInitialized;
    return NOERROR;
}

ECode Animation::Initialize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 parentWidth,
    /* [in] */ Int32 parentHeight)
{
    Reset();
    mInitialized = TRUE;

    return NOERROR;
}

ECode Animation::SetListenerHandler(
    /* [in] */ IHandler* handler)
{
    if (mListenerHandler == NULL) {
        mOnStart = new SetListenerHandlerRunable(this, SetListenerHandlerRunable::ONSTART_TYPE);
        mOnRepeat = new SetListenerHandlerRunable(this, SetListenerHandlerRunable::ONREPEAT_TYPE);
        mOnEnd = new SetListenerHandlerRunable(this, SetListenerHandlerRunable::ONEND_TYPE);
    }

    mListenerHandler = handler;
    return NOERROR;
}

ECode Animation::SetInterpolator(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resID)
{
    AutoPtr<IInterpolator> interpolator;
    AnimationUtils::LoadInterpolator(context, resID, (IInterpolator**)&interpolator);
    return SetInterpolator(interpolator);
}

ECode Animation::SetInterpolator(
    /* [in] */ IInterpolator* i)
{
    mInterpolator = i;
    assert(mInterpolator != NULL);
    return NOERROR;
}

ECode Animation::SetStartOffset(
    /* [in] */ Int64 startOffset)
{
    mStartOffset = startOffset;
    return NOERROR;
}

ECode Animation::SetDuration(
    /* [in] */ Int64 durationMillis)
{
    if (durationMillis < 0) {
        Logger::E("Animation", "Animation duration cannot be negative");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDuration = durationMillis;

    return NOERROR;
}

ECode Animation::RestrictDuration(
    /* [in] */ Int64 durationMillis)
{
    // If we start after the duration, then we just won't run.
    if (mStartOffset > durationMillis) {
        mStartOffset = durationMillis;
        mDuration = 0;
        mRepeatCount = 0;
        return NOERROR;
    }

    Int64 dur = mDuration + mStartOffset;
    if (dur > durationMillis) {
        mDuration = durationMillis - mStartOffset;
        dur = durationMillis;
    }
    // If the duration is 0 or less, then we won't run.
    if (mDuration <= 0) {
        mDuration = 0;
        mRepeatCount = 0;
        return NOERROR;
    }
    // Reduce the number of repeats to keep below the maximum duration.
    // The comparison between mRepeatCount and duration is to catch
    // overflows after multiplying them.
    if (mRepeatCount < 0 || mRepeatCount > durationMillis
            || (dur*mRepeatCount) > durationMillis) {
        // Figure out how many times to do the animation.  Subtract 1 since
        // repeat count is the number of times to repeat so 0 runs once.
        mRepeatCount = (Int32)(durationMillis/dur) - 1;
        if (mRepeatCount < 0) {
            mRepeatCount = 0;
        }
    }

    return NOERROR;
}

ECode Animation::ScaleCurrentDuration(
    /* [in] */ Float scale)
{
    mDuration = (Int64)(mDuration * scale);
    mStartOffset = (Int64) (mStartOffset * scale);

    return NOERROR;
}

ECode Animation::SetStartTime(
    /* [in] */ Int64 startTimeMillis)
{
    mStartTime = startTimeMillis;
    mStarted = mEnded = FALSE;
    mCycleFlip = FALSE;
    mRepeated = 0;
    mMore = TRUE;

    return NOERROR;
}

ECode Animation::Start()
{
    return SetStartTime(-1);
}

ECode Animation::StartNow()
{
    Int64 time;
    AnimationUtils::CurrentAnimationTimeMillis(&time);
    return SetStartTime(time);
}

ECode Animation::SetRepeatMode(
    /* [in] */ Int32 repeatMode)
{
    mRepeatMode = repeatMode;
    return NOERROR;
}

ECode Animation::SetRepeatCount(
    /* [in] */ Int32 repeatCount)
{
    if (repeatCount < 0) {
        repeatCount = IAnimation::ANIMATION_INFINITE;
    }
    mRepeatCount = repeatCount;

    return NOERROR;
}

ECode Animation::IsFillEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = mFillEnabled;
    return NOERROR;
}

ECode Animation::SetFillEnabled(
    /* [in] */ Boolean fillEnabled)
{
    mFillEnabled = fillEnabled;
    return NOERROR;
}

ECode Animation::SetFillBefore(
    /* [in] */ Boolean fillBefore)
{
    mFillBefore = fillBefore;
    return NOERROR;
}

ECode Animation::SetFillAfter(
    /* [in] */ Boolean fillAfter)
{
    mFillAfter = fillAfter;
    return NOERROR;
}

ECode Animation::SetZAdjustment(
    /* [in] */ Int32 zAdjustment)
{
    mZAdjustment = zAdjustment;
    return NOERROR;
}

ECode Animation::SetBackgroundColor(
    /* [in] */ Int32 bg)
{
    mBackgroundColor = bg;
    return NOERROR;
}

ECode Animation::GetBackgroundColor(
    /* [out] */ Int32* bg)
{
    assert(bg != NULL);
    *bg = mBackgroundColor;
    return NOERROR;
}

ECode Animation::SetDetachWallpaper(
    /* [in] */ Boolean detachWallpaper)
{
    mDetachWallpaper = detachWallpaper;
    return NOERROR;
}

ECode Animation::GetInterpolator(
    /* [out] */ IInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = mInterpolator;
    REFCOUNT_ADD(*interpolator);
    return NOERROR;
}

ECode Animation::GetStartTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mStartTime;
    return NOERROR;
}

ECode Animation::GetDuration(
    /* [out] */ Int64* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = mDuration;
    return NOERROR;
}

ECode Animation::GetStartOffset(
    /* [out] */ Int64* startOffset)
{
    VALIDATE_NOT_NULL(startOffset);
    *startOffset = mStartOffset;
    return NOERROR;
}

ECode Animation::GetRepeatMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mRepeatMode;
    return NOERROR;
}

ECode Animation::GetRepeatCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mRepeatCount;
    return NOERROR;
}

ECode Animation::GetFillBefore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFillBefore;
    return NOERROR;
}

ECode Animation::GetFillAfter(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFillAfter;
    return NOERROR;
}

ECode Animation::GetZAdjustment(
    /* [out] */ Int32* zAdjustment)
{
    VALIDATE_NOT_NULL(zAdjustment);
    *zAdjustment = mZAdjustment;
    return NOERROR;
}

ECode Animation::GetDetachWallpaper(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDetachWallpaper;
    return NOERROR;
}

ECode Animation::WillChangeTransformationMatrix(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // assume we will change the matrix
    *result = TRUE;
    return NOERROR;
}

ECode Animation::WillChangeBounds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // assume we will change the bounds
    *result = TRUE;
    return NOERROR;
}

ECode Animation::SetAnimationListener(
    /* [in] */ IAnimationAnimationListener* listener)
{
    mListener = listener;
    return NOERROR;
}

void Animation::EnsureInterpolator()
{
    if (mInterpolator == NULL) {
        CAccelerateDecelerateInterpolator::New((IInterpolator**)&mInterpolator);
    }
}

ECode Animation::ComputeDurationHint(
    /* [out] */ Int64* hint)
{
    VALIDATE_NOT_NULL(hint);
    Int64 d1;
    GetStartOffset(&d1);
    Int64 d2;
    GetDuration(&d2);
    Int32 d3;
    GetRepeatCount(&d3);
    *hint = (d1 + d2) * (d3 + 1);
    return NOERROR;
}

ECode Animation::GetTransformation(
    /* [in] */ Int64 currentTime,
    /* [in] */ ITransformation* outTransformation,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mStartTime == -1) {
        mStartTime = currentTime;
    }

    Int64 startOffset;
    GetStartOffset(&startOffset);
    Int64 duration = mDuration;
    Float normalizedTime;
    if (duration != 0) {
        normalizedTime = ((Float)(currentTime - (mStartTime + startOffset))) /
                (Float)duration;
    }
    else {
        // time is a step-change with a zero duration
        normalizedTime = currentTime < mStartTime ? 0.0f : 1.0f;
    }

    Boolean expired = normalizedTime >= 1.0f;
    mMore = !expired;

    using Elastos::Core::Math;
    if (!mFillEnabled) {
        normalizedTime = Math::Max(Math::Min(normalizedTime, 1.0f), 0.0f);
    }

    if ((normalizedTime >= 0.0f || mFillBefore)
        && (normalizedTime <= 1.0f || mFillAfter)) {
        if (!mStarted) {
            FireAnimationStart();
            mStarted = TRUE;

           // CloseGuard is just used to remind you with Log infomations
           if (USE_CLOSEGUARD) {
               mGuard->Open(String("cancel or detach or getTransformation"));
           }
        }

        if (mFillEnabled) {
            normalizedTime = Math::Max(Math::Min(normalizedTime, 1.0f), 0.0f);
        }

        if (mCycleFlip) {
            normalizedTime = 1.0f - normalizedTime;
        }

        Float interpolatedTime;
        ITimeInterpolator::Probe(mInterpolator)->GetInterpolation(normalizedTime, &interpolatedTime);
        ApplyTransformation(interpolatedTime, outTransformation);
    }

    if (expired) {
        if (mRepeatCount == mRepeated) {
            if (!mEnded) {
                mEnded = TRUE;
                mGuard->Close();
                FireAnimationEnd();
            }
        }
        else {
            if (mRepeatCount > 0) {
                mRepeated++;
            }

            if (mRepeatMode == IAnimation::REVERSE) {
                mCycleFlip = !mCycleFlip;
            }

            mStartTime = -1;
            mMore = TRUE;

            FireAnimationRepeat();
        }
    }

    if (!mMore && mOneMoreTime) {
        mOneMoreTime = FALSE;
        *result = TRUE;
        return NOERROR;
    }

    *result = mMore;
    return NOERROR;
}

ECode Animation::GetTransformation(
    /* [in] */ Int64 currentTime,
    /* [in, out] */ ITransformation* outTransformation,
    /* [in] */ Float scale,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mScaleFactor = scale;
    return GetTransformation(currentTime, outTransformation, result);
}

ECode Animation::HasStarted(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = mStarted;
    return NOERROR;
}

ECode Animation::HasEnded(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = mEnded;
    return NOERROR;
}

ECode Animation::HasAlpha(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = FALSE;
    return NOERROR;
}

void Animation::ApplyTransformation(
    /* [in] */ Float interpolatedTime,
    /* [in] */ ITransformation* t)
{
}

Float Animation::ResolveSize(
    /* [in] */ Int32 type,
    /* [in] */ Float value,
    /* [in] */ Int32 size,
    /* [in] */ Int32 parentSize)
{
    switch (type) {
        case IAnimation::ABSOLUTE:
            return value;
        case IAnimation::RELATIVE_TO_SELF:
            return size * value;
        case IAnimation::RELATIVE_TO_PARENT:
            return parentSize * value;
        default:
            return value;
    }
}

ECode Animation::GetInvalidateRegion(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ IRectF* invalidate,
    /* [in] */ ITransformation* transformation)
{
    IRectF* tempRegion = mRegion;
    IRectF* previousRegion = mPreviousRegion;

    invalidate->Set(left, top, right, bottom);

    AutoPtr<IMatrix> matrix;
    transformation->GetMatrix((IMatrix**)&matrix);
    Boolean res;
    matrix->MapRect(invalidate, &res);

    // Enlarge the invalidate region to account for rounding errors
    invalidate->Inset(-1.0f, -1.0f);
    tempRegion->Set(invalidate);
    invalidate->Union(previousRegion);

    previousRegion->Set(tempRegion);

    mTransformation->Set(transformation);
    transformation->Set(mPreviousTransformation);
    mPreviousTransformation->Set(mTransformation);

    return NOERROR;
}

ECode Animation::InitializeInvalidateRegion(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    mPreviousRegion->Set(left, top, right, bottom);
    // Enlarge the invalidate region to account for rounding errors
    mPreviousRegion->Inset(-1.0f, -1.0f);
    if (mFillBefore) {
        Float time;
        ITimeInterpolator::Probe(mInterpolator)->GetInterpolation(0.0f, &time);
        ApplyTransformation(time, mPreviousTransformation);
    }
    return NOERROR;
}

Float Animation::GetScaleFactor()
{
    return mScaleFactor;
}

void Animation::Finalize()
{
}

void Animation::FireAnimationStart()
{
    if (mListener != NULL) {
        if (mListenerHandler == NULL) {
            mListener->OnAnimationStart(this);
        }
        else {
            Boolean result;
            mListenerHandler->PostAtFrontOfQueue(mOnStart, &result);
        }
    }
}

void Animation::FireAnimationRepeat()
{
    if (mListener != NULL) {
        if (mListenerHandler == NULL) {
            mListener->OnAnimationRepeat(this);
        }
        else {
            Boolean result;
            mListenerHandler->PostAtFrontOfQueue(mOnRepeat, &result);
        }
    }
}

void Animation::FireAnimationEnd()
{
    if (mListener != NULL) {
        if (mListenerHandler == NULL) {
            mListener->OnAnimationEnd(this);
        }
        else {
            Boolean result;
            mListenerHandler->PostAtFrontOfQueue(mOnEnd, &result);
        }
    }
}

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos
