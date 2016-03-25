
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CAnimationSet.h"
#include "elastos/droid/view/animation/CTransformation.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::Build;
using Elastos::Droid::R;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

const Int32 AnimationSet::PROPERTY_FILL_AFTER_MASK = 0x1;
const Int32 AnimationSet::PROPERTY_FILL_BEFORE_MASK = 0x2;
const Int32 AnimationSet::PROPERTY_REPEAT_MODE_MASK = 0x4;
const Int32 AnimationSet::PROPERTY_START_OFFSET_MASK = 0x8;
const Int32 AnimationSet::PROPERTY_SHARE_INTERPOLATOR_MASK = 0x10;
const Int32 AnimationSet::PROPERTY_DURATION_MASK = 0x20;
const Int32 AnimationSet::PROPERTY_MORPH_MATRIX_MASK = 0x40;
const Int32 AnimationSet::PROPERTY_CHANGE_BOUNDS_MASK = 0x80;

CAR_INTERFACE_IMPL(AnimationSet, Animation, IAnimationSet);
AnimationSet::AnimationSet()
    : mFlags(0)
    , mDirty(FALSE)
    , mHasAlpha(FALSE)
    , mLastEnd(0)
{
    CArrayList::New((IArrayList**)&mAnimations);
    ASSERT_SUCCEEDED(CTransformation::New((ITransformation**)&mTempTransformation));
}

AnimationSet::~AnimationSet()
{}

ECode AnimationSet::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(Animation::constructor(context, attrs));

    // member variables may be changed in Animation::constructor, initialize them here
    mFlags = 0;
    mDirty = FALSE;
    mHasAlpha = FALSE;
    mLastEnd = 0;
    mStoredOffsets = NULL;

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::AnimationSet), ArraySize(R::styleable::AnimationSet));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    Boolean share;
    a->GetBoolean(R::styleable::AnimationSet_shareInterpolator, TRUE, &share);

    SetFlag(PROPERTY_SHARE_INTERPOLATOR_MASK, share);
    Init();

    Int32 targetSdkVersion;
    AutoPtr<IApplicationInfo> application;
    context->GetApplicationInfo((IApplicationInfo**)&application);
    application->GetTargetSdkVersion(&targetSdkVersion);
    if (targetSdkVersion >= Build::VERSION_CODES::ICE_CREAM_SANDWICH) {
        Boolean has;
        if (a->HasValue(R::styleable::AnimationSet_duration, &has), has) {
            mFlags |= PROPERTY_DURATION_MASK;
        }
        if (a->HasValue(R::styleable::AnimationSet_fillBefore, &has), has) {
            mFlags |= PROPERTY_FILL_BEFORE_MASK;
        }
        if (a->HasValue(R::styleable::AnimationSet_fillAfter, &has), has) {
            mFlags |= PROPERTY_FILL_AFTER_MASK;
        }
        if (a->HasValue(R::styleable::AnimationSet_repeatMode, &has), has) {
            mFlags |= PROPERTY_REPEAT_MODE_MASK;
        }
        if (a->HasValue(R::styleable::AnimationSet_startOffset, &has), has) {
            mFlags |= PROPERTY_START_OFFSET_MASK;
        }
    }
    a->Recycle();

    return NOERROR;
}

ECode AnimationSet::constructor(
    /* [in] */ Boolean shareInterpolator)
{
    SetFlag(PROPERTY_SHARE_INTERPOLATOR_MASK, shareInterpolator);
    Init();

    return NOERROR;
}

ECode AnimationSet::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IAnimationSet> obj;
    CAnimationSet::New(TRUE, (IAnimationSet**)&obj);
    AnimationSet::CloneImpl(obj);
    *object = obj;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode AnimationSet::CloneImpl(
    /* [in] */ IAnimationSet* object)
{
    Animation::CloneImpl(IAnimation::Probe(object));

    AnimationSet* obj = (AnimationSet*)object;
    obj->mFlags = mFlags;
    obj->mDirty = mDirty;
    obj->mHasAlpha = mHasAlpha;
    obj->mLastEnd = mLastEnd;
    obj->mStoredOffsets = mStoredOffsets;
    CTransformation::New((ITransformation**)&obj->mTempTransformation);
    CArrayList::New((IArrayList**)&obj->mAnimations);

    Int32 count;
    mAnimations->GetSize(&count);
    AutoPtr<IArrayList> animations = mAnimations;

    for (Int32 i = 0; i < count; i++) {
        obj = NULL;
        animations->Get(i, (IInterface**)&obj);
        AutoPtr<IInterface> objectClone;
        ICloneable::Probe(obj)->Clone((IInterface**)&objectClone);
        obj->mAnimations->Add(objectClone);
    }

    return NOERROR;
}

void AnimationSet::SetFlag(
    /* [in] */ Int32 mask,
    /* [in] */ Boolean value)
{
    if (value) {
        mFlags |= mask;
    }
    else {
        mFlags &= ~mask;
    }
}

void AnimationSet::Init()
{
    mStartTime = 0;
}

ECode AnimationSet::SetFillAfter(
    /* [in] */ Boolean fillAfter)
{
    mFlags |= PROPERTY_FILL_AFTER_MASK;
    return Animation::SetFillAfter(fillAfter);
}

ECode AnimationSet::SetFillBefore(
    /* [in] */ Boolean fillBefore)
{
    mFlags |= PROPERTY_FILL_BEFORE_MASK;
    return Animation::SetFillBefore(fillBefore);
}

ECode AnimationSet::SetRepeatMode(
    /* [in] */ Int32 repeatMode)
{
    mFlags |= PROPERTY_REPEAT_MODE_MASK;
    return Animation::SetRepeatMode(repeatMode);
}

ECode AnimationSet::SetStartOffset(
    /* [in] */ Int64 startOffset)
{
    mFlags |= PROPERTY_START_OFFSET_MASK;
    return Animation::SetStartOffset(startOffset);
}

ECode AnimationSet::HasAlpha(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);

    if (mDirty) {
        mDirty = mHasAlpha = FALSE;

        Int32 count;
        mAnimations->GetSize(&count);
        AutoPtr<IArrayList> animations = mAnimations;

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            animations->Get(i, (IInterface**)&obj);
            Boolean res;
            if (IAnimation::Probe(obj)->HasAlpha(&res), res) {
                mHasAlpha = TRUE;
                break;
            }
        }
    }
    *has = mHasAlpha;
    return NOERROR;
}

ECode AnimationSet::SetDuration(
    /* [in] */ Int64 durationMillis)
{
    mFlags |= PROPERTY_DURATION_MASK;
    Animation::SetDuration(durationMillis);

    mLastEnd = mStartOffset + mDuration;
    return NOERROR;
}

ECode AnimationSet::AddAnimation(
    /* [in] */ IAnimation* a)
{
    mAnimations->Add(a);

    Boolean noMatrix = (mFlags & PROPERTY_MORPH_MATRIX_MASK) == 0;
    Boolean willChange;
    a->WillChangeTransformationMatrix(&willChange);
    if (noMatrix && willChange) {
        mFlags |= PROPERTY_MORPH_MATRIX_MASK;
    }

    Boolean changeBounds = (mFlags & PROPERTY_CHANGE_BOUNDS_MASK) == 0;
    a->WillChangeBounds(&willChange);
    if (changeBounds && willChange) {
        mFlags |= PROPERTY_CHANGE_BOUNDS_MASK;
    }

    if ((mFlags & PROPERTY_DURATION_MASK) == PROPERTY_DURATION_MASK) {
        mLastEnd = mStartOffset + mDuration;
    }
    else {
        Int64 startOffset, duration;
        a->GetStartOffset(&startOffset);
        a->GetDuration(&duration);
        Int32 size;
        if ((mAnimations->GetSize(&size), size) == 1) {
            mDuration = startOffset + duration;
            mLastEnd = mStartOffset + mDuration;
        }
        else {
            mLastEnd = Elastos::Core::Math::Max(mLastEnd, startOffset + duration);
            mDuration = mLastEnd - mStartOffset;
        }
    }

    mDirty = TRUE;
    return NOERROR;
}


ECode AnimationSet::SetStartTime(
    /* [in] */ Int64 startTimeMillis)
{
    Animation::SetStartTime(startTimeMillis);

    Int32 count;
    mAnimations->GetSize(&count);
    AutoPtr<IArrayList> animations = mAnimations;

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        animations->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimation> a = IAnimation::Probe(obj);
        a->SetStartTime(startTimeMillis);
    }

    return NOERROR;
}

ECode AnimationSet::GetStartTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    Int64 startTime = Elastos::Core::Math::INT64_MAX_VALUE;

    Int32 count;
    mAnimations->GetSize(&count);
    AutoPtr<IArrayList> animations = mAnimations;

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        animations->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimation> a = IAnimation::Probe(obj);
        Int64 temp;
        a->GetStartTime(&temp);
        startTime = Elastos::Core::Math::Min(startTime, temp);
    }

    *time = startTime;
    return NOERROR;
}

ECode AnimationSet::RestrictDuration(
    /* [in] */ Int64 durationMillis)
{
    Animation::RestrictDuration(durationMillis);

    AutoPtr<IArrayList> animations = mAnimations;
    Int32 count;
    animations->GetSize(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        animations->Get(i, (IInterface**)&obj);
        IAnimation::Probe(obj)->RestrictDuration(durationMillis);
    }

    return NOERROR;
}

ECode AnimationSet::GetDuration(
    /* [out] */ Int64* _duration)
{
    VALIDATE_NOT_NULL(_duration);

    AutoPtr<IArrayList> animations = mAnimations;
    Int32 count;
    animations->GetSize(&count);
    Int64 duration = 0;

    Boolean durationSet = (mFlags & PROPERTY_DURATION_MASK) == PROPERTY_DURATION_MASK;
    if (durationSet) {
        duration = mDuration;
    }
    else {
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            animations->Get(i, (IInterface**)&obj);
            Int64 temp;
            IAnimation::Probe(obj)->GetDuration(&temp);
            duration = Elastos::Core::Math::Max(duration, temp);
        }
    }

    *_duration = duration;
    return NOERROR;
}

ECode AnimationSet::ComputeDurationHint(
    /* [out] */ Int64* hint)
{
    VALIDATE_NOT_NULL(hint);

    Int64 duration = 0;
    Int32 count;
    mAnimations->GetSize(&count);
    AutoPtr<IArrayList> animations = mAnimations;

    for (Int32 i = count -1; i >= 0; --i) {
        AutoPtr<IInterface> obj;
        animations->Get(i, (IInterface**)&obj);
        Int64 d;
        IAnimation::Probe(obj)->ComputeDurationHint(&d);
        if (d > duration) {
            duration = d;
        }
    }

    *hint = duration;
    return NOERROR;
}

ECode AnimationSet::InitializeInvalidateRegion(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<IRectF> region = mPreviousRegion;
    region->Set(left, top, right, bottom);
    region->Inset(-1.0f, -1.0f);

    if (mFillBefore) {
        Int32 count;
        mAnimations->GetSize(&count);
        AutoPtr<IArrayList> animations = mAnimations;
        AutoPtr<ITransformation> temp = mTempTransformation;

        AutoPtr<ITransformation> previousTransformation = mPreviousTransformation;

        for (Int32 i = count -1; i >= 0; --i) {
            AutoPtr<IInterface> obj;
            animations->Get(i, (IInterface**)&obj);
            AutoPtr<IAnimation> a = IAnimation::Probe(obj);
            Boolean res, res1;
            Int64 startOffset;
            if ((a->IsFillEnabled(&res), !res) || (a->GetFillBefore(&res1), res1)
                    || (a->GetStartOffset(&startOffset), startOffset == 0)) {
                temp->Clear();
                AutoPtr<Animation> animation = (Animation*)a.Get();
                AutoPtr<IInterpolator> interpolator = animation->mInterpolator;
                Float interpolation = 0.0f;
                if (interpolator != NULL) {
                    ITimeInterpolator::Probe(interpolator)->GetInterpolation(0.0f, &interpolation);
                }
                animation->ApplyTransformation(interpolation, temp);
                previousTransformation->Compose(temp);
            }
        }
    }

    return NOERROR;
}

ECode AnimationSet::GetTransformation(
    /* [in] */ Int64 currentTime,
    /* [in] */ ITransformation* t,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count;
    mAnimations->GetSize(&count);
    AutoPtr<IArrayList> animations = mAnimations;
    AutoPtr<ITransformation> temp = mTempTransformation;

    Boolean more = FALSE;
    Boolean started = FALSE;
    Boolean ended = TRUE;

    t->Clear();

    for (Int32 i = count -1; i >= 0; --i) {
        AutoPtr<IInterface> obj;
        animations->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimation> a = IAnimation::Probe(obj);
        temp->Clear();
        Boolean res;
        more = (a->GetTransformation(currentTime, mTempTransformation, GetScaleFactor(), &res), res) || more;
        t->Compose(temp);

        started = started || (a->HasStarted(&res), res);
        ended = (a->HasEnded(&res), res) && ended;
    }

    if (started && !mStarted) {
        if (mListener != NULL) {
            mListener->OnAnimationStart(this);
        }
        mStarted = TRUE;
    }

    if (ended != mEnded) {
        if (mListener != NULL) {
            mListener->OnAnimationEnd(this);
        }
        mEnded = ended;
    }

    *result = more;
    return NOERROR;
}

ECode AnimationSet::ScaleCurrentDuration(
    /* [in] */ Float scale)
{
    AutoPtr<IArrayList> animations = mAnimations;
    Int32 count;
    animations->GetSize(&count);

    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        animations->Get(i, (IInterface**)&obj);
        IAnimation::Probe(obj)->ScaleCurrentDuration(scale);
    }

    return NOERROR;
}

ECode AnimationSet::Initialize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 parentWidth,
    /* [in] */ Int32 parentHeight)
{
    Animation::Initialize(width, height, parentWidth, parentHeight);

    Boolean durationSet = (mFlags & PROPERTY_DURATION_MASK) == PROPERTY_DURATION_MASK;
    Boolean fillAfterSet = (mFlags & PROPERTY_FILL_AFTER_MASK) == PROPERTY_FILL_AFTER_MASK;
    Boolean fillBeforeSet = (mFlags & PROPERTY_FILL_BEFORE_MASK) == PROPERTY_FILL_BEFORE_MASK;
    Boolean repeatModeSet = (mFlags & PROPERTY_REPEAT_MODE_MASK) == PROPERTY_REPEAT_MODE_MASK;
    Boolean shareInterpolator = (mFlags & PROPERTY_SHARE_INTERPOLATOR_MASK)
            == PROPERTY_SHARE_INTERPOLATOR_MASK;
    Boolean startOffsetSet = (mFlags & PROPERTY_START_OFFSET_MASK)
            == PROPERTY_START_OFFSET_MASK;

    if (shareInterpolator) {
        EnsureInterpolator();
    }

    AutoPtr<IArrayList> children = mAnimations;
    Int32 count;
    children->GetSize(&count);

    Int64 duration = mDuration;
    Boolean fillAfter = mFillAfter;
    Boolean fillBefore = mFillBefore;
    Int32 repeatMode = mRepeatMode;
    AutoPtr<IInterpolator> interpolator = mInterpolator;
    Int64 startOffset = mStartOffset;

    AutoPtr< ArrayOf<Int64> > storedOffsets = mStoredOffsets;

    if (startOffsetSet) {
        Int32 count;
        mAnimations->GetSize(&count);
        if (storedOffsets == NULL || storedOffsets->GetLength() != count) {
            storedOffsets = mStoredOffsets = ArrayOf<Int64>::Alloc(count);
        }
    }
    else if (storedOffsets != NULL) {
        storedOffsets = mStoredOffsets = NULL;
    }

    for (int i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        children->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimation> a = IAnimation::Probe(obj);
        if (durationSet) {
            a->SetDuration(duration);
        }
        if (fillAfterSet) {
            a->SetFillAfter(fillAfter);
        }
        if (fillBeforeSet) {
            a->SetFillBefore(fillBefore);
        }
        if (repeatModeSet) {
            a->SetRepeatMode(repeatMode);
        }
        if (shareInterpolator) {
            a->SetInterpolator(interpolator);
        }
        if (startOffsetSet) {
            Int64 offset;
            a->GetStartOffset(&offset);
            a->SetStartOffset(offset + startOffset);
            (*storedOffsets)[i] = offset;
        }
        a->Initialize(width, height, parentWidth, parentHeight);
    }

    return NOERROR;
}

ECode AnimationSet::Reset()
{
    Animation::Reset();
    return RestoreChildrenStartOffset();
}

ECode AnimationSet::RestoreChildrenStartOffset()
{
    AutoPtr< ArrayOf<Int64> > offsets = mStoredOffsets;
    if (offsets == NULL) {
        return NOERROR;
    }

    AutoPtr<IArrayList> children = mAnimations;
    Int32 count;
    children->GetSize(&count);

    for (int i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        children->Get(i, (IInterface**)&obj);
        IAnimation::Probe(obj)->SetStartOffset((*offsets)[i]);
    }

    return NOERROR;
}

ECode AnimationSet::GetAnimations(
    /* [out] */ IList** animations)
{
    VALIDATE_NOT_NULL(animations);
    *animations = IList::Probe(mAnimations);
    REFCOUNT_ADD(*animations);
    return NOERROR;
}

ECode AnimationSet::WillChangeTransformationMatrix(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mFlags & PROPERTY_MORPH_MATRIX_MASK) == PROPERTY_MORPH_MATRIX_MASK;
    return NOERROR;
}

ECode AnimationSet::WillChangeBounds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mFlags & PROPERTY_CHANGE_BOUNDS_MASK) == PROPERTY_CHANGE_BOUNDS_MASK;
    return NOERROR;
}

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
