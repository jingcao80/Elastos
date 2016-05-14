#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/ProgressBar.h"
#include "elastos/droid/widget/CProgressBarSavedState.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/graphics/drawable/CLayerDrawable.h"
#include "elastos/droid/graphics/drawable/CStateListDrawable.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/graphics/drawable/CShapeDrawable.h"
#include "elastos/droid/graphics/drawable/CClipDrawable.h"
#include "elastos/droid/graphics/drawable/CAnimationDrawable.h"
#include "elastos/droid/graphics/drawable/shapes/CRoundRectShape.h"
#include "elastos/droid/graphics/CBitmapShader.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include "elastos/droid/view/animation/CTransformation.h"
#include "elastos/droid/view/animation/CAlphaAnimation.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include <elastos/core/Math.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CBitmapShader;
using Elastos::Droid::Graphics::IBitmapShader;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ShaderTileMode;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Graphics::Drawable::ILayerDrawable;
using Elastos::Droid::Graphics::Drawable::CLayerDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_ILayerDrawable;
using Elastos::Droid::Graphics::Drawable::IStateListDrawable;
using Elastos::Droid::Graphics::Drawable::CStateListDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IStateListDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IShapeDrawable;
using Elastos::Droid::Graphics::Drawable::CShapeDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IShapeDrawable;
using Elastos::Droid::Graphics::Drawable::IClipDrawable;
using Elastos::Droid::Graphics::Drawable::CClipDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IClipDrawable;
using Elastos::Droid::Graphics::Drawable::IAnimationDrawable;
using Elastos::Droid::Graphics::Drawable::CAnimationDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IAnimationDrawable;
using Elastos::Droid::Graphics::Drawable::Shapes::IRoundRectShape;
using Elastos::Droid::Graphics::Drawable::Shapes::CRoundRectShape;
using Elastos::Droid::Graphics::Drawable::Shapes::EIID_IRoundRectShape;
using Elastos::Droid::Graphics::Drawable::IClipDrawable;
using Elastos::Droid::Graphics::Drawable::IAnimatable;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::R;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IAbsSavedState;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::IAlphaAnimation;
using Elastos::Droid::View::Animation::CAlphaAnimation;
using Elastos::Droid::View::Animation::ILinearInterpolator;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::View::Animation::CTransformation;
using Elastos::Droid::View::Animation::ITransformation;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Accessibility::CAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 ProgressBar::RefreshData::POOL_MAX = 24;
AutoPtr<Pools::SynchronizedPool<ProgressBar::RefreshData> > ProgressBar::RefreshData::sPool =
        new Pools::SynchronizedPool<ProgressBar::RefreshData>(POOL_MAX);

ProgressBar::RefreshProgressRunnable::RefreshProgressRunnable(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{
}

ECode ProgressBar::RefreshProgressRunnable::Run()
{
    AutoPtr<IInterface> obj;
    mWeakHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    IProgressBar* pb = IProgressBar::Probe(obj);

    if (pb != NULL) {
        ProgressBar* host = (ProgressBar*)pb;
        AutoLock lock(host);
        Int32 count = host->mRefreshData.GetSize();
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<RefreshData> rd = host->mRefreshData[i];
            host->DoRefreshProgress(rd->mId, rd->mProgress, rd->mFromUser, TRUE, rd->mAnimate);
            rd->Recycle();
        }
        host->mRefreshData.Clear();
        host->mRefreshIsPosted = FALSE;
    }

    return NOERROR;
}

ProgressBar::AccessibilityEventSender::AccessibilityEventSender(
    /* [in] */ IWeakReference* host) : mWeakHost(host)
{
}

ECode ProgressBar::AccessibilityEventSender::Run()
{
    AutoPtr<IInterface> obj;
    mWeakHost->Resolve(EIID_IInterface, (IInterface**)&obj);
    IProgressBar* pb = IProgressBar::Probe(obj);

    if (pb != NULL) {
        ProgressBar* host = (ProgressBar*)pb;
        host->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
    }
    return NOERROR;
}

ProgressBar::RefreshData::RefreshData()
    : mId(0)
    , mProgress(0)
    , mFromUser(FALSE)
    , mAnimate(FALSE)
{
}

ECode ProgressBar::RefreshData::Recycle()
{
    sPool->ReleaseItem(this);
    return NOERROR;
}

AutoPtr<ProgressBar::RefreshData> ProgressBar::RefreshData::Obtain(
    /* [in] */ Int32 id,
    /* [in] */ Float progress,
    /* [in] */ Boolean fromUser,
    /* [in] */ Boolean animate)
{
    AutoPtr<RefreshData> rd = sPool->AcquireItem();
    if (rd == NULL) {
        rd = new RefreshData();
    }
    rd->mId = id;
    rd->mProgress = progress;
    rd->mFromUser = fromUser;
    rd->mAnimate = animate;
    return rd;
}

ProgressBar::ProgressTintInfo::ProgressTintInfo()
    : mIndeterminateTintMode(-1)
    , mHasIndeterminateTint(FALSE)
    , mHasIndeterminateTintMode(FALSE)
    , mProgressTintMode(-1)
    , mHasProgressTint(FALSE)
    , mHasProgressTintMode(FALSE)
    , mProgressBackgroundTintMode(-1)
    , mHasProgressBackgroundTint(FALSE)
    , mHasProgressBackgroundTintMode(FALSE)
    , mSecondaryProgressTintMode(-1)
    , mHasSecondaryProgressTint(FALSE)
    , mHasSecondaryProgressTintMode(FALSE)
{}

CAR_INTERFACE_IMPL(ProgressBar::ProgressBarSavedState, BaseSavedState, IProgressBarSavedState);
ProgressBar::ProgressBarSavedState::ProgressBarSavedState()
    : mProgress(0)
    , mSecondaryProgress(0)
{}

ProgressBar::ProgressBarSavedState::~ProgressBarSavedState()
{}

ECode ProgressBar::ProgressBarSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    BaseSavedState::WriteToParcel(dest);

    dest->WriteInt32(mProgress);
    dest->WriteInt32(mSecondaryProgress);
    return NOERROR;
}

ECode ProgressBar::ProgressBarSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    BaseSavedState::ReadFromParcel(source);

    source->ReadInt32(&mProgress);
    source->ReadInt32(&mSecondaryProgress);
    return NOERROR;
}

CAR_INTERFACE_IMPL(ProgressBar, View, IProgressBar);
ProgressBar::ProgressBar()
    : mMinWidth(0)
    , mMaxWidth(0)
    , mMinHeight(0)
    , mMaxHeight(0)
    , mMirrorForRtl(FALSE)
    , mProgress(0)
    , mSecondaryProgress(0)
    , mMax(0)
    , mBehavior(0)
    , mDuration(0)
    , mIndeterminate(FALSE)
    , mOnlyIndeterminate(FALSE)
    , mHasAnimation(FALSE)
    , mNoInvalidate(FALSE)
    , mUiThreadId(0)
    , mShouldStartAnimationDrawable(FALSE)
    , mAnimationPosition(0)
    , mInDrawing(FALSE)
    , mAttached(FALSE)
    , mRefreshIsPosted(FALSE)
{}

ECode ProgressBar::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ProgressBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::progressBarStyle);
}

ECode ProgressBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ProgressBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(View::constructor(context, attrs, defStyleAttr, defStyleRes));
    return InitFromAttributes(context, attrs, defStyleAttr, defStyleRes);
}

ECode ProgressBar::InitFromAttributes(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    Thread::GetCurrentThread()->GetId(&mUiThreadId);
    InitProgressBar();

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::ProgressBar),
            ArraySize(R::styleable::ProgressBar));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
        attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    mNoInvalidate = TRUE;
    AutoPtr<IDrawable> progressDrawable;
    a->GetDrawable(R::styleable::ProgressBar_progressDrawable, (IDrawable**)&progressDrawable);
    if (progressDrawable != NULL) {
        // Calling this method can set mMaxHeight, make sure the corresponding
        // XML attribute for mMaxHeight is read after calling this method
        SetProgressDrawableTiled(progressDrawable);
    }

    a->GetInt32(R::styleable::ProgressBar_indeterminateDuration, mDuration, &mDuration);

    a->GetDimensionPixelSize(R::styleable::ProgressBar_minWidth, mMinWidth, &mMinWidth);
    a->GetDimensionPixelSize(R::styleable::ProgressBar_maxWidth, mMaxWidth, &mMaxWidth);
    a->GetDimensionPixelSize(R::styleable::ProgressBar_minHeight, mMinHeight, &mMinHeight);
    a->GetDimensionPixelSize(R::styleable::ProgressBar_maxHeight, mMaxHeight, &mMaxHeight);

    a->GetInt32(R::styleable::ProgressBar_indeterminateBehavior, mBehavior, &mBehavior);

    Int32 resID;
    a->GetResourceId(
        R::styleable::ProgressBar_interpolator,
        R::anim::linear_interpolator, &resID); // default to linear interpolator
    if (resID > 0) {
        SetInterpolator(context, resID);
    }
    Int32 result;
    a->GetInt32(R::styleable::ProgressBar_max, mMax, &result);
    SetMax(result);
    a->GetInt32(R::styleable::ProgressBar_progress, mProgress, &result);
    SetProgress(result);
    a->GetInt32(R::styleable::ProgressBar_secondaryProgress, mSecondaryProgress, &result);
    SetSecondaryProgress(result);
    AutoPtr<IDrawable> indeterminateDrawable;
    a->GetDrawable(R::styleable::ProgressBar_indeterminateDrawable, (IDrawable**)&indeterminateDrawable);
    if (indeterminateDrawable != NULL) {
        SetIndeterminateDrawableTiled(indeterminateDrawable);
    }

    a->GetBoolean(
        R::styleable::ProgressBar_indeterminateOnly, mOnlyIndeterminate, &mOnlyIndeterminate);
    mNoInvalidate = FALSE;

    Boolean b;
    a->GetBoolean(R::styleable::ProgressBar_indeterminate, mIndeterminate, &b);
    SetIndeterminate(mOnlyIndeterminate || b);

    a->GetBoolean(R::styleable::ProgressBar_mirrorForRtl, mMirrorForRtl, &mMirrorForRtl);

    Boolean has = FALSE;
    if (a->HasValue(R::styleable::ProgressBar_progressTintMode, &has), has) {
        if (mProgressTintInfo == NULL) {
            mProgressTintInfo = new ProgressTintInfo();
        }
        Int32 iv = 0;
        a->GetInt32(R::styleable::ProgressBar_progressBackgroundTintMode, -1, &iv);
        Drawable::ParseTintMode(iv, -1, &mProgressTintInfo->mProgressTintMode);
        mProgressTintInfo->mHasProgressTintMode = TRUE;
    }

    if (a->HasValue(R::styleable::ProgressBar_progressTint, &has), has) {
        if (mProgressTintInfo == NULL) {
            mProgressTintInfo = new ProgressTintInfo();
        }

        a->GetColorStateList(R::styleable::ProgressBar_progressTint, (IColorStateList**)&mProgressTintInfo->mProgressTintList);
        mProgressTintInfo->mHasProgressTint = TRUE;
    }

    if (a->HasValue(R::styleable::ProgressBar_progressBackgroundTintMode, &has), has) {
        if (mProgressTintInfo == NULL) {
            mProgressTintInfo = new ProgressTintInfo();
        }
        Int32 iv = 0;
        a->GetInt32(R::styleable::ProgressBar_progressTintMode, -1, &iv);
        Drawable::ParseTintMode(iv, -1, &mProgressTintInfo->mProgressBackgroundTintMode);
        mProgressTintInfo->mHasProgressBackgroundTintMode = TRUE;
    }

    if (a->HasValue(R::styleable::ProgressBar_progressBackgroundTint, &has), has) {
        if (mProgressTintInfo == NULL) {
            mProgressTintInfo = new ProgressTintInfo();
        }
        a->GetColorStateList(R::styleable::ProgressBar_progressBackgroundTint,
                (IColorStateList**)&mProgressTintInfo->mProgressBackgroundTintList);
        mProgressTintInfo->mHasProgressBackgroundTint = TRUE;
    }

    if (a->HasValue(R::styleable::ProgressBar_secondaryProgressTintMode, &has), has) {
        if (mProgressTintInfo == NULL) {
            mProgressTintInfo = new ProgressTintInfo();
        }
        Int32 iv = 0;
        a->GetInt32(R::styleable::ProgressBar_secondaryProgressTintMode, -1, &iv);
        Drawable::ParseTintMode(iv, -1, &mProgressTintInfo->mSecondaryProgressTintMode);
        mProgressTintInfo->mHasSecondaryProgressTintMode = TRUE;
    }

    if (a->HasValue(R::styleable::ProgressBar_secondaryProgressTint, &has), has) {
        if (mProgressTintInfo == NULL) {
            mProgressTintInfo = new ProgressTintInfo();
        }
        a->GetColorStateList(R::styleable::ProgressBar_secondaryProgressTint,
            (IColorStateList**)&mProgressTintInfo->mSecondaryProgressTintList);
        mProgressTintInfo->mHasSecondaryProgressTint = TRUE;
    }

    if (a->HasValue(R::styleable::ProgressBar_indeterminateTint, &has), has) {
        if (mProgressTintInfo == NULL) {
            mProgressTintInfo = new ProgressTintInfo();
        }
        Int32 iv = 0;
        a->GetInt32(R::styleable::ProgressBar_indeterminateTintMode, -1, &iv);
        Drawable::ParseTintMode(iv, -1, &mProgressTintInfo->mIndeterminateTintMode);
        mProgressTintInfo->mHasIndeterminateTintMode = TRUE;
    }

    if (a->HasValue(R::styleable::ProgressBar_indeterminateTint, &has), has) {
        if (mProgressTintInfo == NULL) {
            mProgressTintInfo = new ProgressTintInfo();
        }
        a->GetColorStateList(R::styleable::ProgressBar_indeterminateTint,
            (IColorStateList**)&mProgressTintInfo->mIndeterminateTintList);
        mProgressTintInfo->mHasIndeterminateTint = TRUE;
    }

    a->Recycle();

    ApplyProgressTints();
    ApplyIndeterminateTint();

    // If not explicitly specified this view is important for accessibility.
    Int32 mode = 0;
    if ((GetImportantForAccessibility(&mode), mode) == IView::IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        SetImportantForAccessibility(IView::IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
    return NOERROR;
}

AutoPtr<IDrawable> ProgressBar::Tileify(
    /* [in] */ IDrawable* drawable,
    /* [in] */ Boolean clip)
{
    if (drawable->Probe(EIID_ILayerDrawable)) {
        AutoPtr<ILayerDrawable> background = (ILayerDrawable*) drawable->Probe(EIID_ILayerDrawable);
        Int32 N;
        background->GetNumberOfLayers(&N);
        AutoPtr<ArrayOf<IDrawable*> > outDrawables = ArrayOf<IDrawable*>::Alloc(N);

        for (Int32 i = 0; i < N; i++) {
            Int32 id;
            background->GetId(i, &id);
            AutoPtr<IDrawable> dTemp;
            background->GetDrawable(i, (IDrawable**)&dTemp);
            outDrawables->Set(i, Tileify(dTemp,
                    (id == R::id::progress || id == R::id::secondaryProgress)));
        }

        AutoPtr<ILayerDrawable> newBg;
        CLayerDrawable::New(outDrawables, (ILayerDrawable**)&newBg);

        for (Int32 i = 0; i < N; i++) {
            Int32 id;
            background->GetId(i, &id);
            newBg->SetId(i, id);
        }

        return IDrawable::Probe(newBg);

    } else if (drawable->Probe(EIID_IStateListDrawable)) {
        AutoPtr<IStateListDrawable> in =
                (IStateListDrawable*) drawable->Probe(EIID_IStateListDrawable);
        AutoPtr<IStateListDrawable> out;
        CStateListDrawable::New((IStateListDrawable**)&out);
        Int32 numStates;
        in->GetStateCount(&numStates);
        for (Int32 i = 0; i < numStates; i++) {
            AutoPtr<ArrayOf<Int32> > stateSet;
            AutoPtr<IDrawable> stateDrawable;
            in->GetStateSet(i, (ArrayOf<Int32>**)&stateSet);
            in->GetStateDrawable(i, (IDrawable**)&stateDrawable);
            AutoPtr<IDrawable> tileify = Tileify(stateDrawable, clip);
            out->AddState(stateSet, tileify);
        }
        return IDrawable::Probe(out);

    } else if (drawable->Probe(EIID_IBitmapDrawable)) {
        AutoPtr<IBitmapDrawable> bitmap = IBitmapDrawable::Probe(drawable);
        AutoPtr<IBitmap> tileBitmap;
        bitmap->GetBitmap((IBitmap**)&tileBitmap);
        if (mSampleTile == NULL) {
            mSampleTile = tileBitmap;
        }

        AutoPtr<IShapeDrawable> shapeDrawable;
        CShapeDrawable::New(GetDrawableShape(), (IShapeDrawable**)&shapeDrawable);
        AutoPtr<IBitmapShader> bitmapShader;
        CBitmapShader::New(tileBitmap,
                Elastos::Droid::Graphics::ShaderTileMode_REPEAT,
                Elastos::Droid::Graphics::ShaderTileMode_CLAMP, (IBitmapShader**)&bitmapShader);
        AutoPtr<IPaint> paint;
        shapeDrawable->GetPaint((IPaint**)&paint);
        paint->SetShader(IShader::Probe(bitmapShader));

        // Ensure the tint and filter are propagated in the correct order.
        AutoPtr<IColorStateList> list;
        bitmap->GetTint((IColorStateList**)&list);
        IDrawable::Probe(shapeDrawable)->SetTintList(list);
        PorterDuffMode mode = -1;
        bitmap->GetTintMode(&mode);
        IDrawable::Probe(shapeDrawable)->SetTintMode(mode);
        AutoPtr<IColorFilter> cf;
        IDrawable::Probe(bitmap)->GetColorFilter((IColorFilter**)&cf);
        IDrawable::Probe(shapeDrawable)->SetColorFilter(cf);

        if(clip) {
            AutoPtr<IDrawable> clipDrawable;
            CClipDrawable::New(IDrawable::Probe(shapeDrawable), IGravity::LEFT,
                    IClipDrawable::HORIZONTAL, (IDrawable**)&clipDrawable);
            return clipDrawable;
        }
        else{
            return IDrawable::Probe(shapeDrawable);
        }
    }

    return drawable;
}

AutoPtr<IShape> ProgressBar::GetDrawableShape()
{
    AutoPtr<ArrayOf<Float> > roundedCorners = ArrayOf<Float>::Alloc(8);
    for(Int32 i = 0; i < 8; i++)
    {
        (*roundedCorners)[i] = 5.0f;
    }
    AutoPtr<IShape> result;
    CRoundRectShape::New(roundedCorners, NULL, NULL, (IShape**)&result);
    return result;
}

AutoPtr<IDrawable> ProgressBar::TileifyIndeterminate(
    /* [in] */ IDrawable* inDrawable)
{
    AutoPtr<IDrawable> drawable = inDrawable;
    AutoPtr<IAnimationDrawable> background = IAnimationDrawable::Probe(drawable);
    if (background) {
        Int32 N = 0;
        background->GetNumberOfFrames(&N);
        AutoPtr<IAnimationDrawable> newBg;
        CAnimationDrawable::New((IAnimationDrawable**)&newBg);
        Boolean oneShot;
        background->IsOneShot(&oneShot);
        newBg->SetOneShot(oneShot);

        Boolean rst;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IDrawable> temp;
            background->GetFrame(i, (IDrawable**)&temp);
            AutoPtr<IDrawable> frame = Tileify(temp, TRUE);
            frame->SetLevel(10000, &rst);
            Int32 duration;
            background->GetDuration(i, &duration);
            newBg->AddFrame(frame, duration);
        }
        IDrawable::Probe(newBg)->SetLevel(10000, &rst);
        drawable = IDrawable::Probe(newBg);
    }
    return drawable;
}

void ProgressBar::InitProgressBar()
{
    mMax = 100;
    mProgress = 0;
    mSecondaryProgress = 0;
    mIndeterminate = FALSE;
    mOnlyIndeterminate = FALSE;
    mDuration = 4000;
    mBehavior = IAnimation::RESTART;
    mMinWidth = 24;
    mMaxWidth = 48;
    mMinHeight = 24;
    mMaxHeight = 48;
}

ECode ProgressBar::IsIndeterminate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(this);
    *result = mIndeterminate;
    return NOERROR;
}

ECode ProgressBar::SetIndeterminate(
    /* [in] */ Boolean indeterminate)
{
    AutoLock lock(this);

    if ((!mOnlyIndeterminate || !mIndeterminate) && indeterminate != mIndeterminate) {
        mIndeterminate = indeterminate;

        if (indeterminate) {
            // swap between indeterminate and regular backgrounds
            mCurrentDrawable = mIndeterminateDrawable;
            StartAnimation();
        }
        else {
            mCurrentDrawable = mProgressDrawable;
            StopAnimation();
        }
    }

    return NOERROR;
}

ECode ProgressBar::GetIndeterminateDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = mIndeterminateDrawable;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode ProgressBar::SetIndeterminateDrawable(
    /* [in] */ IDrawable* d)
{
    if (mIndeterminateDrawable.Get() != d) {
        if (mIndeterminateDrawable != NULL) {
            mIndeterminateDrawable->SetCallback(NULL);
            UnscheduleDrawable(mIndeterminateDrawable);
        }

        mIndeterminateDrawable = d;

        if (d != NULL) {
            d->SetCallback(this);
            Int32 dir = 0;
            GetLayoutDirection(&dir);
            d->SetLayoutDirection(dir);
            Boolean is = FALSE;
            if (d->IsStateful(&is), is) {
                AutoPtr<ArrayOf<Int32> > states;
                GetDrawableState((ArrayOf<Int32>**)& states);
                Boolean isStateful = FALSE;
                d->SetState(states, &isStateful);
            }
            ApplyIndeterminateTint();
        }

        if (mIndeterminate) {
            mCurrentDrawable = d;
            PostInvalidate();
        }
    }

    return NOERROR;
}

ECode ProgressBar::SetIndeterminateTintList(
    /* [in] */ /*@Nullable*/ IColorStateList* tint)
{
    if (mProgressTintInfo == NULL) {
        mProgressTintInfo = new ProgressTintInfo();
    }
    mProgressTintInfo->mIndeterminateTintList = tint;
    mProgressTintInfo->mHasIndeterminateTint = TRUE;

    ApplyIndeterminateTint();
    return NOERROR;
}

ECode ProgressBar::GetIndeterminateTintList(
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mProgressTintInfo != NULL ? mProgressTintInfo->mIndeterminateTintList.Get() : NULL;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode ProgressBar::SetIndeterminateTintMode(
    /* [in] */ /*@Nullable*/ PorterDuffMode tintMode)
{
    if (mProgressTintInfo == NULL) {
        mProgressTintInfo = new ProgressTintInfo();
    }
    mProgressTintInfo->mIndeterminateTintMode = tintMode;
    mProgressTintInfo->mHasIndeterminateTintMode = TRUE;

    ApplyIndeterminateTint();
    return NOERROR;
}

ECode ProgressBar::GetIndeterminateTintMode(
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mProgressTintInfo != NULL ? mProgressTintInfo->mIndeterminateTintMode : -1;
    return NOERROR;
}

void ProgressBar::ApplyIndeterminateTint()
{
    if (mIndeterminateDrawable != NULL && mProgressTintInfo != NULL) {
        AutoPtr<ProgressTintInfo> tintInfo = mProgressTintInfo;
        if (tintInfo->mHasIndeterminateTint || tintInfo->mHasIndeterminateTintMode) {
            mIndeterminateDrawable->Mutate();

            if (tintInfo->mHasIndeterminateTint) {
                mIndeterminateDrawable->SetTintList(tintInfo->mIndeterminateTintList);
            }

            if (tintInfo->mHasIndeterminateTintMode) {
                mIndeterminateDrawable->SetTintMode(tintInfo->mIndeterminateTintMode);
            }
        }
    }
}

ECode ProgressBar::SetIndeterminateDrawableTiled(
    /* [in] */ IDrawable* d)
{
    if (d != NULL) {
        d = TileifyIndeterminate(d);
    }

    SetIndeterminateDrawable(d);
    return NOERROR;
}

ECode ProgressBar::GetProgressDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = mProgressDrawable;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode ProgressBar::SetProgressDrawable(
    /* [in] */ IDrawable* d)
{
    if (mProgressDrawable.Get() != d) {
        if (mProgressDrawable != NULL) {
            mProgressDrawable->SetCallback(NULL);
            UnscheduleDrawable(mProgressDrawable);
        }

        mProgressDrawable = d;

        if (d != NULL) {
            d->SetCallback(this);
            Int32 dir = 0;
            GetLayoutDirection(&dir);
            d->SetLayoutDirection(dir);
            Boolean is = FALSE;
            if (d->IsStateful(&is), is) {
                AutoPtr<ArrayOf<Int32> > state;
                GetDrawableState((ArrayOf<Int32>**)&state);
                Boolean isStateful = FALSE;
                d->SetState(state, &isStateful);
            }

            // Make sure the ProgressBar is always tall enough
            Int32 drawableHeight = 0;
            d->GetMinimumHeight(&drawableHeight);
            if (mMaxHeight < drawableHeight) {
                mMaxHeight = drawableHeight;
                RequestLayout();
            }

            ApplyProgressTints();
        }

        if (!mIndeterminate) {
            mCurrentDrawable = d;
            PostInvalidate();
        }

        Int32 width = 0, height = 0;
        GetWidth(&width);
        GetHeight(&height);
        UpdateDrawableBounds(width, height);
        UpdateDrawableState();

        DoRefreshProgress(R::id::progress, mProgress, FALSE, FALSE);
        DoRefreshProgress(R::id::secondaryProgress, mSecondaryProgress, FALSE, FALSE);
    }

    return NOERROR;
}

void ProgressBar::ApplyProgressTints()
{
    if (mProgressDrawable != NULL && mProgressTintInfo != NULL) {
        ApplyPrimaryProgressTint();
        ApplyProgressBackgroundTint();
        ApplySecondaryProgressTint();
    }
}

void ProgressBar::ApplyPrimaryProgressTint()
{
    if (mProgressTintInfo->mHasProgressTint
            || mProgressTintInfo->mHasProgressTintMode) {
        AutoPtr<IDrawable> target = GetTintTarget(R::id::progress, TRUE);
        if (target != NULL) {
            if (mProgressTintInfo->mHasProgressTint) {
                target->SetTintList(mProgressTintInfo->mProgressTintList);
            }
            if (mProgressTintInfo->mHasProgressTintMode) {
                target->SetTintMode(mProgressTintInfo->mProgressTintMode);
            }
        }
    }
}

void ProgressBar::ApplyProgressBackgroundTint()
{
    if (mProgressTintInfo->mHasProgressBackgroundTint
            || mProgressTintInfo->mHasProgressBackgroundTintMode) {
        AutoPtr<IDrawable> target = GetTintTarget(R::id::background, FALSE);
        if (target != NULL) {
            if (mProgressTintInfo->mHasProgressBackgroundTint) {
                target->SetTintList(mProgressTintInfo->mProgressBackgroundTintList);
            }
            if (mProgressTintInfo->mHasProgressBackgroundTintMode) {
                target->SetTintMode(mProgressTintInfo->mProgressBackgroundTintMode);
            }
        }
    }
}

void ProgressBar::ApplySecondaryProgressTint()
{
    if (mProgressTintInfo->mHasSecondaryProgressTint
            || mProgressTintInfo->mHasSecondaryProgressTintMode) {
        AutoPtr<IDrawable> target = GetTintTarget(R::id::secondaryProgress, FALSE);
        if (target != NULL) {
            if (mProgressTintInfo->mHasSecondaryProgressTint) {
                target->SetTintList(mProgressTintInfo->mSecondaryProgressTintList);
            }
            if (mProgressTintInfo->mHasSecondaryProgressTintMode) {
                target->SetTintMode(mProgressTintInfo->mSecondaryProgressTintMode);
            }
        }
    }
}

ECode ProgressBar::SetProgressTintList(
    /* [in] */ /*@Nullable*/ IColorStateList* tint)
{
    if (mProgressTintInfo == NULL) {
        mProgressTintInfo = new ProgressTintInfo();
    }
    mProgressTintInfo->mProgressTintList = tint;
    mProgressTintInfo->mHasProgressTint = TRUE;

    if (mProgressDrawable != NULL) {
        ApplyPrimaryProgressTint();
    }
    return NOERROR;
}

ECode ProgressBar::GetProgressTintList(
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mProgressTintInfo != NULL ? mProgressTintInfo->mProgressTintList.Get() : NULL;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode ProgressBar::SetProgressTintMode(
    /* [in] */ /*@Nullable*/ PorterDuffMode tintMode)
{
    if (mProgressTintInfo == NULL) {
        mProgressTintInfo = new ProgressTintInfo();
    }
    mProgressTintInfo->mProgressTintMode = tintMode;
    mProgressTintInfo->mHasProgressTintMode = TRUE;

    if (mProgressDrawable != NULL) {
        ApplyPrimaryProgressTint();
    }
    return NOERROR;
}

ECode ProgressBar::GetProgressTintMode(
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mProgressTintInfo != NULL ? mProgressTintInfo->mProgressTintMode : -1;
    return NOERROR;
}

ECode ProgressBar::SetProgressBackgroundTintList(
    /* [in] */ /*@Nullable*/ IColorStateList* tint)
{
    if (mProgressTintInfo == NULL) {
        mProgressTintInfo = new ProgressTintInfo();
    }
    mProgressTintInfo->mProgressBackgroundTintList = tint;
    mProgressTintInfo->mHasProgressBackgroundTint = TRUE;

    if (mProgressDrawable != NULL) {
        ApplyProgressBackgroundTint();
    }
    return NOERROR;
}

ECode ProgressBar::GetProgressBackgroundTintList(
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mProgressTintInfo != NULL ? mProgressTintInfo->mProgressBackgroundTintList : NULL;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode ProgressBar::SetProgressBackgroundTintMode(
    /* [in] */ /*@Nullable*/ PorterDuffMode tintMode)
{
    if (mProgressTintInfo == NULL) {
        mProgressTintInfo = new ProgressTintInfo();
    }
    mProgressTintInfo->mProgressBackgroundTintMode = tintMode;
    mProgressTintInfo->mHasProgressBackgroundTintMode = TRUE;

    if (mProgressDrawable != NULL) {
        ApplyProgressBackgroundTint();
    }
    return NOERROR;
}

ECode ProgressBar::GetProgressBackgroundTintMode(
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mProgressTintInfo != NULL ? mProgressTintInfo->mProgressBackgroundTintMode : -1;
    return NOERROR;
}

ECode ProgressBar::SetSecondaryProgressTintList(
    /* [in] */ /*@Nullable*/ IColorStateList* tint)
{
    if (mProgressTintInfo == NULL) {
        mProgressTintInfo = new ProgressTintInfo();
    }
    mProgressTintInfo->mSecondaryProgressTintList = tint;
    mProgressTintInfo->mHasSecondaryProgressTint = TRUE;

    if (mProgressDrawable != NULL) {
        ApplySecondaryProgressTint();
    }
    return NOERROR;
}

ECode ProgressBar::GetSecondaryProgressTintList(
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mProgressTintInfo != NULL ? mProgressTintInfo->mSecondaryProgressTintList.Get() : NULL;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode ProgressBar::SetSecondaryProgressTintMode(
    /* [in] */ /*@Nullable*/ PorterDuffMode tintMode)
{
    if (mProgressTintInfo == NULL) {
        mProgressTintInfo = new ProgressTintInfo();
    }
    mProgressTintInfo->mSecondaryProgressTintMode = tintMode;
    mProgressTintInfo->mHasSecondaryProgressTintMode = TRUE;

    if (mProgressDrawable != NULL) {
        ApplySecondaryProgressTint();
    }
    return NOERROR;
}

ECode ProgressBar::GetSecondaryProgressTintMode(
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mProgressTintInfo != NULL ? mProgressTintInfo->mSecondaryProgressTintMode : -1;
    return NOERROR;
}

AutoPtr<IDrawable> ProgressBar::GetTintTarget(
    /* [in] */ Int32 layerId,
    /* [in] */ Boolean shouldFallback)
{
    AutoPtr<IDrawable> layer;
    if (mProgressDrawable != NULL) {
        mProgressDrawable->Mutate();

        if (ILayerDrawable::Probe(mProgressDrawable)) {
            ILayerDrawable::Probe(mProgressDrawable)->FindDrawableByLayerId(layerId, (IDrawable**)&layer);
        }

        if (shouldFallback && layer == NULL) {
            layer = mProgressDrawable;
        }
    }

    return layer;
}

ECode ProgressBar::SetProgressDrawableTiled(
    /* [in] */ IDrawable* d)
{
    AutoPtr<IDrawable> drawable = d;
    if (drawable != NULL) {
        drawable = Tileify(drawable, FALSE);
    }

    SetProgressDrawable(drawable);
    return NOERROR;
}

AutoPtr<IDrawable> ProgressBar::GetCurrentDrawable()
{
    return mCurrentDrawable;
}

Boolean ProgressBar::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return who == mProgressDrawable || who == mIndeterminateDrawable
        || View::VerifyDrawable(who);
}

ECode ProgressBar::JumpDrawablesToCurrentState()
{
    View::JumpDrawablesToCurrentState();
    if (mProgressDrawable != NULL) mProgressDrawable->JumpToCurrentState();
    if (mIndeterminateDrawable != NULL) mIndeterminateDrawable->JumpToCurrentState();
    return NOERROR;
}

ECode ProgressBar::OnResolveDrawables(
    /* [in] */ Int32 layoutDirection)
{
    AutoPtr<IDrawable > d = mCurrentDrawable;
    if (d != NULL) {
        d->SetLayoutDirection(layoutDirection);
    }
    if (mIndeterminateDrawable != NULL) {
        mIndeterminateDrawable->SetLayoutDirection(layoutDirection);
    }
    if (mProgressDrawable != NULL) {
        mProgressDrawable->SetLayoutDirection(layoutDirection);
    }
    return NOERROR;
}

ECode ProgressBar::PostInvalidate()
{
    if (!mNoInvalidate) {
        View::PostInvalidate();
    }

    return NOERROR;
}

void ProgressBar::SetDrawableTint(
    /* [in] */ Int32 id,
    /* [in] */ IColorStateList* tint,
    /* [in] */ PorterDuffMode tintMode,
    /* [in] */ Boolean fallback)
{
    AutoPtr<IDrawable> layer;

    // We expect a layer drawable, so try to find the target ID.
    AutoPtr<IDrawable> d = mCurrentDrawable;
    if (ILayerDrawable::Probe(d)) {
        ILayerDrawable::Probe(d)->FindDrawableByLayerId(id, (IDrawable**)&layer);
    }

    if (fallback && layer == NULL) {
        layer = d;
    }

    layer->Mutate();
    layer->SetTintList(tint);
    layer->SetTintMode(tintMode);
}

Float ProgressBar::GetScale(
    /* [in] */ Float progress)
{
    return mMax > 0 ? progress / (Float) mMax : 0;
}

void ProgressBar::DoRefreshProgress(
    /* [in] */ Int32 id,
    /* [in] */ Float progress,
    /* [in] */ Boolean fromUser,
    /* [in] */ Boolean callBackToApp)
{
    AutoLock lock(this);
    DoRefreshProgress(id, progress, fromUser, callBackToApp, FALSE);
}

void ProgressBar::DoRefreshProgress(
    /* [in] */ Int32 id,
    /* [in] */ Float progress,
    /* [in] */ Boolean fromUser,
    /* [in] */ Boolean callBackToApp,
    /* [in] */ Boolean animate)
{
    AutoLock lock(this);
    Float scale = GetScale(progress);

    AutoPtr<IDrawable> d = mCurrentDrawable;
    if (d != NULL) {
        AutoPtr<IDrawable> progressDrawable;

        if (ILayerDrawable::Probe(d)) {
            ILayerDrawable::Probe(d)->FindDrawableByLayerId(id, (IDrawable**)&progressDrawable);
            Boolean can = FALSE;
            if (progressDrawable != NULL && (CanResolveLayoutDirection(&can), can)) {
                Int32 dir = 0;
                GetLayoutDirection(&dir);
                progressDrawable->SetLayoutDirection(dir);
            }
        }

        const Int32 level = (Int32) (scale * MAX_LEVEL);
        Boolean changed = FALSE;
        (progressDrawable != NULL ? progressDrawable : d)->SetLevel(level, &changed);
    } else {
        Invalidate();
    }

    if (id == R::id::progress) {
        if (animate) {
            OnAnimatePosition(scale, fromUser);
        } else if (callBackToApp) {
            OnProgressRefresh(scale, fromUser);
        }
    }
}

void ProgressBar::OnAnimatePosition(
    /* [in] */ Float scale,
    /* [in] */ Boolean fromUser)
{
}

void ProgressBar::SetProgressValueOnly(
    /* [in] */ Int32 progress)
{
    mProgress = progress;
    OnProgressRefresh(GetScale(progress), TRUE);
}

void ProgressBar::SetAnimationPosition(
    /* [in] */ Float position)
{
    mAnimationPosition = position;
    RefreshProgress(R::id::progress, position, TRUE, TRUE);
}

Float ProgressBar::GetAnimationPosition()
{
    return mAnimationPosition;
}

void ProgressBar::OnProgressRefresh(
    /* [in] */ Float scale,
    /* [in] */ Boolean fromUser)
{
    Boolean enable = FALSE;
    AutoPtr<IAccessibilityManager> am;
    CAccessibilityManager::GetInstance(mContext, (IAccessibilityManager**)&am);
    if (am) {
        am->IsEnabled(&enable);
    }

    if (enable) {
        ScheduleAccessibilityEventSender();
    }
}

void ProgressBar::RefreshProgress(
    /* [in] */ Int32 id,
    /* [in] */ Float progress,
    /* [in] */ Boolean fromUser)
{
    AutoLock lock(this);
    RefreshProgress(id, progress, fromUser, FALSE);
}

void ProgressBar::RefreshProgress(
    /* [in] */ Int32 id,
    /* [in] */ Float progress,
    /* [in] */ Boolean fromUser,
    /* [in] */ Boolean animate)
{
    AutoLock lock(this);
    Int64 currentId = 0;
    Thread::GetCurrentThread()->GetId(&currentId);
    if (mUiThreadId == currentId) {
        DoRefreshProgress(id, progress, fromUser, TRUE, animate);
    }
    else {
        if (mRefreshProgressRunnable == NULL) {
            AutoPtr<IWeakReference> wr;
            GetWeakReference((IWeakReference**)&wr);
            mRefreshProgressRunnable = new RefreshProgressRunnable(wr);
        }

        AutoPtr<RefreshData> rd = RefreshData::Obtain(id, progress, fromUser, animate);
        mRefreshData.PushBack(rd);
        if (mAttached && !mRefreshIsPosted) {
            Boolean result = FALSE;
            Post(mRefreshProgressRunnable, &result);
            mRefreshIsPosted = TRUE;
        }
    }
}

ECode ProgressBar::SetProgress(
    /* [in] */ Int32 progress)
{
    AutoLock lock(this);
    return SetProgress(progress, FALSE);
}

ECode ProgressBar::SetProgress(
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    AutoLock lock(this);

    if (mIndeterminate) {
        return NOERROR;
    }

    if (progress < 0) {
        progress = 0;
    }

    if (progress > mMax) {
        progress = mMax;
    }

    if (progress != mProgress) {
        mProgress = progress;
        RefreshProgress(R::id::progress, mProgress, fromUser);
    }
    return NOERROR;
}

ECode ProgressBar::SetSecondaryProgress(
    /* [in] */ Int32 secondaryProgress)
{
    AutoLock lock(this);

    if (mIndeterminate) {
        return NOERROR;
    }

    if (secondaryProgress < 0) {
        secondaryProgress = 0;
    }

    if (secondaryProgress > mMax) {
        secondaryProgress = mMax;
    }

    if (secondaryProgress != mSecondaryProgress) {
        mSecondaryProgress = secondaryProgress;
        RefreshProgress(R::id::secondaryProgress, mSecondaryProgress, FALSE);
    }
    return NOERROR;
}

ECode ProgressBar::GetProgress(
    /* [out] */ Int32* progress)
{
    VALIDATE_NOT_NULL(progress);
    AutoLock lock(this);
    *progress = mIndeterminate ? 0 : mProgress;
    return NOERROR;
}

ECode ProgressBar::GetSecondaryProgress(
    /* [out] */ Int32* progress)
{
    VALIDATE_NOT_NULL(progress);
    AutoLock lock(this);
    *progress = mIndeterminate ? 0 : mSecondaryProgress;
    return NOERROR;
}

ECode ProgressBar::GetMax(
    /* [out] */ Int32* max)
{
    VALIDATE_NOT_NULL(max);
    AutoLock lock(this);
    *max = mMax;
    return NOERROR;
}

ECode ProgressBar::SetMax(
    /* [in] */ Int32 max)
{
    AutoLock lock(this);
    if (max < 0) {
        max = 0;
    }
    if (max != mMax) {
        mMax = max;
        PostInvalidate();

        if (mProgress > max) {
            mProgress = max;

        }

        RefreshProgress(R::id::progress, mProgress, FALSE);
    }

    return NOERROR;
}

ECode ProgressBar::IncrementProgressBy(
    /* [in] */ Int32 diff)
{
    AutoLock lock(this);
    return SetProgress(mProgress + diff, FALSE);
}

ECode ProgressBar::IncrementSecondaryProgressBy(
    /* [in] */ Int32 diff)
{
    AutoLock lock(this);
    return SetSecondaryProgress(mSecondaryProgress + diff);
}

void ProgressBar::StartAnimation()
{
    Int32 visibility = 0;
    if ((GetVisibility(&visibility), visibility) != IView::VISIBLE) {
        return;
    }

    if (IAnimatable::Probe(mIndeterminateDrawable.Get())) {
        mShouldStartAnimationDrawable = TRUE;
        mHasAnimation = FALSE;
    }
    else {
        mHasAnimation = TRUE;

        if (mInterpolator == NULL) {
            CLinearInterpolator::New((IInterpolator**)&mInterpolator);
        }

        if (mTransformation == NULL) {
            CTransformation::New((ITransformation**)&mTransformation);
        } else {
            mTransformation->Clear();
        }

        if (mAnimation == NULL) {
            CAlphaAnimation::New(0.0f, 1.0f, (IAnimation**)&mAnimation);
        } else {
            mAnimation->Reset();
        }

        mAnimation->SetRepeatMode(mBehavior);
        mAnimation->SetRepeatCount(IAnimation::ANIMATION_INFINITE);
        mAnimation->SetDuration(mDuration);
        mAnimation->SetInterpolator(mInterpolator);
        mAnimation->SetStartTime(IAnimation::START_ON_FIRST_FRAME);
    }

    PostInvalidate();
}

void ProgressBar::StopAnimation()
{
    mHasAnimation = FALSE;
    if (IAnimatable::Probe(mIndeterminateDrawable.Get())) {
        IAnimatable::Probe(mIndeterminateDrawable.Get())->Stop();
        mShouldStartAnimationDrawable = FALSE;
    }

    PostInvalidate();
}

ECode ProgressBar::SetInterpolator(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resID)
{
    AutoPtr<IInterpolator> p;
    AnimationUtils::LoadInterpolator(context, resID, (IInterpolator**)&p);
    SetInterpolator(p);

    return NOERROR;
}

ECode ProgressBar::SetInterpolator(
    /* [in] */ IInterpolator* interpolator)
{
    mInterpolator = interpolator;

    return NOERROR;
}

ECode ProgressBar::GetInterpolator(
    /* [out] */ IInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = mInterpolator;
    REFCOUNT_ADD(*interpolator);
    return NOERROR;
}

ECode ProgressBar::SetVisibility(
    /* [in] */ Int32 v)
{
    Int32 visibility = 0;
    if ((GetVisibility(&visibility), visibility) != v) {
        View::SetVisibility(v);

        if (mIndeterminate) {
            // let's be nice with the UI thread
            if (v == IView::GONE || v == IView::INVISIBLE) {
                StopAnimation();
            }
            else {
                StartAnimation();
            }
        }
    }

    return NOERROR;
}

ECode ProgressBar::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    View::OnVisibilityChanged(changedView, visibility);

    if (mIndeterminate) {
        // let's be nice with the UI thread
        if (visibility == IView::GONE || visibility == IView::INVISIBLE) {
            StopAnimation();
        }
        else {
            StartAnimation();
        }
    }
    return NOERROR;
}

ECode ProgressBar::InvalidateDrawable(
    /* [in] */ IDrawable* dr)
{
    if (!mInDrawing) {
        if (VerifyDrawable(dr)) {
            AutoPtr<IRect> dirty;
            dr->GetBounds((IRect**)&dirty);
            Int32 scrollX = mScrollX + mPaddingLeft;
            Int32 scrollY = mScrollY + mPaddingTop;

            CRect* _dirty = (CRect*)dirty.Get();
            Invalidate(_dirty->mLeft + scrollX, _dirty->mTop + scrollY,
                    _dirty->mRight + scrollX, _dirty->mBottom + scrollY);
        }
        else {
            View::InvalidateDrawable(dr);
        }
    }

    return NOERROR;
}

void ProgressBar::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    UpdateDrawableBounds(w, h);
}

void ProgressBar::UpdateDrawableBounds(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    // onDraw will translate the canvas so we draw starting at 0,0.
    // Subtract out padding for the purposes of the calculations below.
    w -= mPaddingRight + mPaddingLeft;
    h -= mPaddingTop + mPaddingBottom;

    Int32 right = w;
    Int32 bottom = h;
    Int32 top = 0;
    Int32 left = 0;

    if (mIndeterminateDrawable != NULL) {
        // Aspect ratio logic does not apply to AnimationDrawables
        if (mOnlyIndeterminate && !(mIndeterminateDrawable->Probe(EIID_IAnimationDrawable))) {
            // Maintain aspect ratio. Certain kinds of animated drawables
            // get very confused otherwise.
            Int32 intrinsicWidth;
            mIndeterminateDrawable->GetIntrinsicWidth(&intrinsicWidth);
            Int32 intrinsicHeight;
            mIndeterminateDrawable->GetIntrinsicHeight(&intrinsicHeight);
            Float intrinsicAspect = (Float) intrinsicWidth / intrinsicHeight;
            Float boundAspect = (Float) w / h;
            if (intrinsicAspect != boundAspect) {
                if (boundAspect > intrinsicAspect) {
                    // New width is larger. Make it smaller to match height.
                    Int32 width = (Int32) (h * intrinsicAspect);
                    left = (w - width) / 2;
                    right = left + width;
                } else {
                    // New height is larger. Make it smaller to match width.
                    Int32 height = (Int32) (w * (1 / intrinsicAspect));
                    top = (h - height) / 2;
                    bottom = top + height;
                }
            }
        }

        Boolean rtl = FALSE;
        if ((IsLayoutRtl(&rtl), rtl) && mMirrorForRtl) {
            Int32 tempLeft = left;
            left = w - right;
            right = w - tempLeft;
        }
        mIndeterminateDrawable->SetBounds(left, top, right, bottom);
    }

    if (mProgressDrawable != NULL) {
        mProgressDrawable->SetBounds(0, 0, right, bottom);
    }
}

void ProgressBar::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    AutoLock lock(this);
    View::OnDraw(canvas);
    DrawTrack(canvas);
}

void ProgressBar::DrawTrack(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IDrawable> d = mCurrentDrawable;
    if (d != NULL) {
        // Translate canvas so a indeterminate circular progress bar with padding
        // rotates properly in its animation
        Int32 saveCount = 0;
        canvas->Save(&saveCount);

        Boolean rtl = FALSE;
        if((IsLayoutRtl(&rtl), rtl) && mMirrorForRtl) {
            Int32 width = 0;
            GetWidth(&width);
            canvas->Translate(width - mPaddingRight, mPaddingTop);
            canvas->Scale(-1.0f, 1.0f);
        } else {
            canvas->Translate(mPaddingLeft, mPaddingTop);
        }

        Int64 time = 0;
        GetDrawingTime(&time);
        if (mHasAnimation) {
            Boolean result = FALSE;
            mAnimation->GetTransformation(time, mTransformation, &result);
            Float scale = 0;
            mTransformation->GetAlpha(&scale);
            // try {
            mInDrawing = TRUE;
            d->SetLevel((Int32) (scale * MAX_LEVEL), &result);
            // } finally {
            mInDrawing = FALSE;
            // }
            PostInvalidateOnAnimation();
        }

        d->Draw(canvas);
        canvas->RestoreToCount(saveCount);

        if (mShouldStartAnimationDrawable && IAnimatable::Probe(d)) {
            IAnimatable::Probe(d)->Start();
            mShouldStartAnimationDrawable = FALSE;
        }
    }
}

void ProgressBar::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AutoLock lock(this);
    AutoPtr<IDrawable> d = mCurrentDrawable;

    Int32 dw = 0;
    Int32 dh = 0;
    if (d != NULL) {
        d->GetIntrinsicWidth(&dw);
        d->GetIntrinsicHeight(&dh);
        dw = Elastos::Core::Math::Max(mMinWidth, Elastos::Core::Math::Min(mMaxWidth, dw));
        dh = Elastos::Core::Math::Max(mMinHeight, Elastos::Core::Math::Min(mMaxHeight, dh));
    }

    UpdateDrawableState();
    dw += mPaddingLeft + mPaddingRight;
    dh += mPaddingTop + mPaddingBottom;

    SetMeasuredDimension(ResolveSizeAndState(dw, widthMeasureSpec, 0),
            ResolveSizeAndState(dh, heightMeasureSpec, 0));
}

ECode ProgressBar::DrawableStateChanged()
{
    View::DrawableStateChanged();
    UpdateDrawableState();
    return NOERROR;
}

void ProgressBar::UpdateDrawableState()
{
    AutoPtr<ArrayOf<Int32> > state;
    GetDrawableState((ArrayOf<Int32>**)&state);

    Boolean stateful;
    if (mProgressDrawable != NULL && (mProgressDrawable->IsStateful(&stateful), stateful)) {
        mProgressDrawable->SetState(state, &stateful);
    }

    if (mIndeterminateDrawable != NULL && (mIndeterminateDrawable->IsStateful(&stateful), stateful)) {
        mIndeterminateDrawable->SetState(state, &stateful);
    }
}

ECode ProgressBar::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    View::DrawableHotspotChanged(x, y);

    if (mProgressDrawable != NULL) {
        mProgressDrawable->SetHotspot(x, y);
    }

    if (mIndeterminateDrawable != NULL) {
        mIndeterminateDrawable->SetHotspot(x, y);
    }
    return NOERROR;
}

AutoPtr<IParcelable> ProgressBar::OnSaveInstanceState()
{
    // Force our ancestor class to save its state
    AutoPtr<IParcelable> superState = View::OnSaveInstanceState();
    AutoPtr<CProgressBarSavedState> ss;
    CProgressBarSavedState::NewByFriend(superState, (CProgressBarSavedState**)&ss);

    ss->mProgress = mProgress;
    ss->mSecondaryProgress = mSecondaryProgress;

    return IParcelable::Probe(ss);
}

void ProgressBar::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<IProgressBarSavedState> ss = IProgressBarSavedState::Probe(state);

    AutoPtr<IParcelable> p;
    IAbsSavedState::Probe(ss)->GetSuperState((IParcelable**)&p);
    View::OnRestoreInstanceState(p);

    SetProgress(((CProgressBarSavedState*)(ss.Get()))->mProgress);
    SetSecondaryProgress(((CProgressBarSavedState*)ss.Get())->mSecondaryProgress);
}

ECode ProgressBar::OnAttachedToWindow()
{
    View::OnAttachedToWindow();
    if (mIndeterminate) {
        StartAnimation();
    }
    {
        AutoLock lock(this);
        List<AutoPtr<RefreshData> >::Iterator ator = mRefreshData.Begin();
        for (; ator != mRefreshData.End(); ++ator) {
            AutoPtr<RefreshData> rd = *ator;
            DoRefreshProgress(rd->mId, rd->mProgress, rd->mFromUser, rd->mAnimate);
            rd->Recycle();
        }
        mRefreshData.Clear();
    }
    mAttached = TRUE;
    return NOERROR;
}

ECode ProgressBar::OnDetachedFromWindow()
{
//
    if (mIndeterminate) {
        StopAnimation();
    }
    Boolean result = FALSE;
    if (mRefreshProgressRunnable != NULL) {
        RemoveCallbacks(mRefreshProgressRunnable, &result);
    }
    if (mRefreshProgressRunnable != NULL && mRefreshIsPosted) {
        RemoveCallbacks(mRefreshProgressRunnable, &result);
    }
    if (mAccessibilityEventSender != NULL) {
        RemoveCallbacks(mAccessibilityEventSender, &result);
    }
    // This should come after stopAnimation(), otherwise an invalidate message remains in the
    // queue, which can prevent the entire view hierarchy from being GC'ed during a rotation
    View::OnDetachedFromWindow();
    mAttached = FALSE;
    return NOERROR;
}

ECode ProgressBar::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    View::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(String("CProgressBar"), (ICharSequence**)&className));
    IAccessibilityRecord::Probe(event)->SetClassName(className);
    IAccessibilityRecord::Probe(event)->SetItemCount(mMax);
    IAccessibilityRecord::Probe(event)->SetCurrentItemIndex(mProgress);
    return NOERROR;
}

ECode ProgressBar::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    View::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> className;
    FAIL_RETURN(CString::New(String("CProgressBar"), (ICharSequence**)&className));
    info->SetClassName(className);
    return NOERROR;
}

void ProgressBar::ScheduleAccessibilityEventSender()
{
    if (mAccessibilityEventSender == NULL) {
        AutoPtr<IWeakReferenceSource> wrs = this;
        assert(wrs);
        AutoPtr<IWeakReference> wr;
        wrs->GetWeakReference((IWeakReference**)&wr);
        mAccessibilityEventSender = new AccessibilityEventSender(wr);
    }
    else {
        Boolean result = FALSE;
        RemoveCallbacks(mAccessibilityEventSender, &result);
    }
    Boolean result = FALSE;
    PostDelayed(mAccessibilityEventSender, TIMEOUT_SEND_ACCESSIBILITY_EVENT, &result);
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
