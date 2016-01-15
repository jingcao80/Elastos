
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/internal/widget/multiwaveview/GlowPadView.h"
#include "elastos/droid/internal/widget/multiwaveview/Tweener.h"
#include "elastos/droid/internal/widget/multiwaveview/CTargetDrawable.h"
#include "elastos/droid/internal/widget/multiwaveview/CPointCloud.h"
#include "elastos/droid/internal/widget/multiwaveview/Ease.h"
#include "elastos/droid/content/pm/CActivityInfo.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/utility/CTypedValueHelper.h"
#include "elastos/droid/view/accessibility/CAccessibilityManagerHelper.h"
#include "elastos/droid/view/CGravity.h"
#include "elastos/droid/R.h"

#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::CActivityInfo;
using Elastos::Droid::Internal::Widget::Multiwaveview::Tweener;
using Elastos::Droid::Internal::Widget::Multiwaveview::CTargetDrawable;
using Elastos::Droid::Internal::Widget::Multiwaveview::CPointCloud;
using Elastos::Droid::Internal::Widget::Multiwaveview::Ease;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CGravity;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;

using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

String GlowPadView::TAG(String("GlowPadView"));
Boolean GlowPadView::DEBUG = FALSE;

const Int32 GlowPadView::STATE_IDLE = 0;
const Int32 GlowPadView::STATE_START = 1;
const Int32 GlowPadView::STATE_FIRST_TOUCH = 2;
const Int32 GlowPadView::STATE_TRACKING = 3;
const Int32 GlowPadView::STATE_SNAP = 4;
const Int32 GlowPadView::STATE_FINISH = 5;

Float GlowPadView::SNAP_MARGIN_DEFAULT = 20.0f; // distance to ring before we snap to it

Int32 GlowPadView::WAVE_ANIMATION_DURATION = 1000;
Int32 GlowPadView::RETURN_TO_HOME_DELAY = 1200;
Int32 GlowPadView::RETURN_TO_HOME_DURATION = 200;
Int32 GlowPadView::HIDE_ANIMATION_DELAY = 200;
Int32 GlowPadView::HIDE_ANIMATION_DURATION = 200;
Int32 GlowPadView::SHOW_ANIMATION_DURATION = 200;
Int32 GlowPadView::SHOW_ANIMATION_DELAY = 50;
Int32 GlowPadView::INITIAL_SHOW_HANDLE_DURATION = 200;
Int32 GlowPadView::REVEAL_GLOW_DELAY = 0;
Int32 GlowPadView::REVEAL_GLOW_DURATION = 0;

Float GlowPadView::TAP_RADIUS_SCALE_ACCESSIBILITY_ENABLED = 1.3f;
Float GlowPadView::TARGET_SCALE_EXPANDED = 1.0f;
Float GlowPadView::TARGET_SCALE_COLLAPSED = 0.8f;
Float GlowPadView::RING_SCALE_EXPANDED = 1.0f;
Float GlowPadView::RING_SCALE_COLLAPSED = 0.5f;

AutoPtr<IAudioAttributes> GlowPadView::VIBRATION_ATTRIBUTES;

//=====================================================================
//               GlowPadView::AnimationBundle::
//=====================================================================
Int64 GlowPadView::AnimationBundle::mSerialVersionUID = 0xA84D78726F127468L;

CAR_INTERFACE_IMPL(GlowPadView::AnimationBundle, AbstractList, IAnimationBundle)

ECode GlowPadView::AnimationBundle::Start()
{
    if (mSuspended)
        return NOERROR; // ignore attempts to start animations
    Int32 count = 0;
    GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        Get(i, (IInterface**)&p);
        AutoPtr<ITweener> anim = ITweener::Probe(p);
        AutoPtr<Tweener> canim = (Tweener*)anim.Get();
        IAnimator::Probe(canim->mAnimator)->Start();
    }
    return NOERROR;
}

ECode GlowPadView::AnimationBundle::Cancel()
{
    Int32 count = 0;
    GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        Get(i, (IInterface**)&p);
        AutoPtr<ITweener> anim = ITweener::Probe(p);
        AutoPtr<Tweener> canim = (Tweener*)anim.Get();
        IAnimator::Probe(canim->mAnimator)->Cancel();
    }
    Clear();
    return NOERROR;
}

ECode GlowPadView::AnimationBundle::Stop()
{
    Int32 count = 0;
    GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        Get(i, (IInterface**)&p);
        AutoPtr<ITweener> anim = ITweener::Probe(p);
        AutoPtr<Tweener> canim = (Tweener*)anim.Get();
        IAnimator::Probe(canim->mAnimator)->End();
    }
    Clear();
    return NOERROR;
}

ECode GlowPadView::AnimationBundle::SetSuspended(
    /* [in] */ Boolean suspend)
{
    mSuspended = suspend;
    return NOERROR;
}

ECode GlowPadView::AnimationBundle::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    assert(0 && "TODO");
    return NOERROR;
}

//=====================================================================
//               GlowPadView::AnimatorListenerAdapter_1::
//=====================================================================

GlowPadView::AnimatorListenerAdapter_1::AnimatorListenerAdapter_1(
    /* [in] */ GlowPadView* host)
    : mHost(host)
{}

ECode GlowPadView::AnimatorListenerAdapter_1::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    mHost->SwitchToState(STATE_IDLE, mHost->mWaveCenterX, mHost->mWaveCenterY);
    mHost->DispatchOnFinishFinalAnimation();
    return NOERROR;
}

//=====================================================================
//               GlowPadView::AnimatorListenerAdapter_2::
//=====================================================================

GlowPadView::AnimatorListenerAdapter_2::AnimatorListenerAdapter_2(
    /* [in] */ GlowPadView* host)
    : mHost(host)
{}

ECode GlowPadView::AnimatorListenerAdapter_2::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    mHost->Ping();
    mHost->SwitchToState(STATE_IDLE, mHost->mWaveCenterX, mHost->mWaveCenterY);
    mHost->DispatchOnFinishFinalAnimation();
    return NOERROR;
}

//=====================================================================
//               GlowPadView::AnimatorUpdateListener_1::
//=====================================================================
CAR_INTERFACE_IMPL(GlowPadView::AnimatorUpdateListener_1, Object, IAnimatorUpdateListener)

GlowPadView::AnimatorUpdateListener_1::AnimatorUpdateListener_1(
    /* [in] */ GlowPadView* host)
    : mHost(host)
{}

ECode GlowPadView::AnimatorUpdateListener_1::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    mHost->Invalidate();
    return NOERROR;
}

//=====================================================================
//               GlowPadView::AnimatorListenerAdapter_3::
//=====================================================================
GlowPadView::AnimatorListenerAdapter_3::AnimatorListenerAdapter_3(
    /* [in] */ GlowPadView* host)
    : mHost(host)
{}

ECode GlowPadView::AnimatorListenerAdapter_3::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    if (mHost->mNewTargetResources != 0) {
        mHost->InternalSetTargetResources(mHost->mNewTargetResources);
        mHost->mNewTargetResources = 0;
        mHost->HideTargets(FALSE, FALSE);
    }
    mHost->mAnimatingTargets = FALSE;
    return NOERROR;
}

//=====================================================================
//               GlowPadView::AnimatorListenerAdapter_4::
//=====================================================================

GlowPadView::AnimatorListenerAdapter_4::AnimatorListenerAdapter_4(
    /* [in] */ GlowPadView* host)
    : mHost(host)
{}

ECode GlowPadView::AnimatorListenerAdapter_4::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    AutoPtr<CPointCloud> pc = (CPointCloud*)(mHost->mPointCloud.Get());
    pc->mWaveManager->SetRadius(0.0f);
    pc->mWaveManager->SetAlpha(0.0f);
    return NOERROR;
}

//=====================================================================
//               GlowPadView::
//=====================================================================
CAR_INTERFACE_IMPL(GlowPadView, Elastos::Droid::View::View, IGlowPadView)

GlowPadView::GlowPadView()
    : mFeedbackCount(3)
    , mVibrationDuration(0)
    , mGrabbedState(0)
    , mActiveTarget(-1)
    , mGlowRadius(0.0f)
    , mWaveCenterX(0.0f)
    , mWaveCenterY(0.0f)
    , mMaxTargetHeight(0)
    , mMaxTargetWidth(0)
    , mRingScaleFactor(1.0f)
    , mAllowScaling(FALSE)
    , mOuterRadius(0.0f)
    , mSnapMargin(0.0f)
    , mFirstItemOffset(0.0f)
    , mMagneticTargets(FALSE)
    , mDragging(FALSE)
    , mNewTargetResources(0)
    , mTargetResourceId(0)
    , mTargetDescriptionsResourceId(0)
    , mDirectionDescriptionsResourceId(0)
    , mAlwaysTrackFinger(FALSE)
    , mHorizontalInset(0)
    , mVerticalInset(0)
    , mGravity(IGravity::TOP)
    , mInitialLayout(TRUE)
    , mInnerRadius(0.0f)
    , mPointerId(0)
{
    CArrayList::New((IArrayList**)&mTargetDrawables);
    mWaveAnimations = new AnimationBundle();
    mTargetAnimations = new AnimationBundle();
    mGlowAnimations = new AnimationBundle();

    mResetListener = new AnimatorListenerAdapter_1(this);

    mResetListenerWithPing = new AnimatorListenerAdapter_2(this);

    mUpdateListener = new AnimatorUpdateListener_1(this);

    mTargetUpdateListener = new AnimatorListenerAdapter_3(this);

    AutoPtr<IAudioAttributesBuilder> bld;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&bld);
    bld->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    bld->SetUsage(IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION);
    bld->Build((IAudioAttributes**)&VIBRATION_ATTRIBUTES);
}

ECode GlowPadView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode GlowPadView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    View::constructor(context, attrs);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32*>(R::styleable::GlowPadView),
        ArraySize(R::styleable::GlowPadView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetDimension(R::styleable::GlowPadView_innerRadius, mInnerRadius, &mInnerRadius);
    a->GetDimension(R::styleable::GlowPadView_outerRadius, mOuterRadius, &mOuterRadius);
    a->GetDimension(R::styleable::GlowPadView_snapMargin, mSnapMargin, &mSnapMargin);
    Float fio = 0.0f;
    a->GetFloat(R::styleable::GlowPadView_firstItemOffset,
                    (Float) Elastos::Core::Math::ToDegrees(mFirstItemOffset), &fio);
    mFirstItemOffset = (Float) Elastos::Core::Math::ToRadians(fio);
    a->GetInt32(R::styleable::GlowPadView_vibrationDuration,
            mVibrationDuration, &mVibrationDuration);
    a->GetInt32(R::styleable::GlowPadView_feedbackCount,
            mFeedbackCount, &mFeedbackCount);
    a->GetBoolean(R::styleable::GlowPadView_allowScaling, FALSE, &mAllowScaling);
    AutoPtr<ITypedValue> handle;
    a->PeekValue(R::styleable::GlowPadView_handleDrawable, (ITypedValue**)&handle);
    AutoPtr<CTypedValue> chandle = (CTypedValue*)handle.Get();
    CTargetDrawable::New(res, chandle != NULL ? chandle->mResourceId : 0, (ITargetDrawable**)&mHandleDrawable);
    mHandleDrawable->SetState(CTargetDrawable::STATE_INACTIVE);
    CTargetDrawable::New(res,
            GetResourceId(a, R::styleable::GlowPadView_outerRingDrawable), (ITargetDrawable**)&mOuterRing);

    a->GetBoolean(R::styleable::GlowPadView_alwaysTrackFinger, FALSE, &mAlwaysTrackFinger);
    a->GetBoolean(R::styleable::GlowPadView_magneticTargets, mMagneticTargets, &mMagneticTargets);

    Int32 pointId = GetResourceId(a, R::styleable::GlowPadView_pointDrawable);
    AutoPtr<IDrawable> pointDrawable;
    if (pointId != 0) {
        context->GetDrawable(pointId, (IDrawable**)&pointDrawable);
    }
    a->GetDimension(R::styleable::GlowPadView_glowRadius, 0.0f, &mGlowRadius);

    CPointCloud::New(pointDrawable, (IPointCloud**)&mPointCloud);
    mPointCloud->MakePointCloud(mInnerRadius, mOuterRadius);
    AutoPtr<CPointCloud> cPC = (CPointCloud*)mPointCloud.Get();
    cPC->mGlowManager->SetRadius(mGlowRadius);

    AutoPtr<ITypedValue> outValue;
    CTypedValue::New((ITypedValue**)&outValue);

    // Read array of target drawables
    Boolean b = FALSE;
    a->GetValue(R::styleable::GlowPadView_targetDrawables, outValue, &b);
    if (b) {
        Int32 resID = 0;
        outValue->GetResourceId(&resID);
        InternalSetTargetResources(resID);
    }
    Int32 tSize = 0;
    if (mTargetDrawables == NULL || (mTargetDrawables->GetSize(&tSize), tSize) == 0) {
        // throw new IllegalStateException("Must specify at least one target drawable");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Read array of target descriptions
    a->GetValue(R::styleable::GlowPadView_targetDescriptions, outValue, &b);
    if (b) {
        Int32 resourceId = 0;
        outValue->GetResourceId(&resourceId);
        if (resourceId == 0) {
            // throw new IllegalStateException("Must specify target descriptions");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        SetTargetDescriptionsResourceId(resourceId);
    }

    // Read array of direction descriptions
    a->GetValue(R::styleable::GlowPadView_directionDescriptions, outValue, &b);
    if (b) {
        Int32 resourceId = 0;
        outValue->GetResourceId(&resourceId);
        if (resourceId == 0) {
            // throw new IllegalStateException("Must specify direction descriptions");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        SetDirectionDescriptionsResourceId(resourceId);
    }

    a->GetInt32(R::styleable::GlowPadView_gravity, IGravity::TOP, &mGravity);

    a->Recycle();

    SetVibrateEnabled(mVibrationDuration > 0);

    AssignDefaultsIfNeeded();
    return NOERROR;
}

Int32 GlowPadView::GetResourceId(
    /* [in] */ ITypedArray* a,
    /* [in] */ Int32 id)
{
    AutoPtr<ITypedValue> tv;
    a->PeekValue(id, (ITypedValue**)&tv);
    Int32 resId = 0;
    return tv == NULL ? 0 : (tv->GetResourceId(&resId), resId);
}

void GlowPadView::Dump()
{
    Logger::V(TAG, "Outer Radius = %d", mOuterRadius);
    Logger::V(TAG, "SnapMargin = %d", mSnapMargin);
    Logger::V(TAG, "FeedbackCount = %d", mFeedbackCount);
    Logger::V(TAG, "VibrationDuration = %d", mVibrationDuration);
    Logger::V(TAG, "GlowRadius = %d", mGlowRadius);
    Logger::V(TAG, "WaveCenterX = %d", mWaveCenterX);
    Logger::V(TAG, "WaveCenterY = %d", mWaveCenterY);
}

ECode GlowPadView::SuspendAnimations()
{
    mWaveAnimations->SetSuspended(TRUE);
    mTargetAnimations->SetSuspended(TRUE);
    mGlowAnimations->SetSuspended(TRUE);
    return NOERROR;
}

ECode GlowPadView::ResumeAnimations()
{
    mWaveAnimations->SetSuspended(FALSE);
    mTargetAnimations->SetSuspended(FALSE);
    mGlowAnimations->SetSuspended(FALSE);
    mWaveAnimations->Start();
    mTargetAnimations->Start();
    mGlowAnimations->Start();
    return NOERROR;
}

Int32 GlowPadView::GetSuggestedMinimumWidth()
{
    // View should be large enough to contain the background + handle and
    // target drawable on either edge.
    Int32 w = 0;
    mOuterRing->GetWidth(&w);
    return (Int32) (Elastos::Core::Math::Max((Float) w, 2 * mOuterRadius) + mMaxTargetWidth);
}

Int32 GlowPadView::GetSuggestedMinimumHeight()
{
    // View should be large enough to contain the unlock ring + target and
    // target drawable on either edge
    Int32 h = 0;
    mOuterRing->GetHeight(&h);
    return (Int32) (Elastos::Core::Math::Max((Float) h, 2 * mOuterRadius) + mMaxTargetHeight);
}

Int32 GlowPadView::GetScaledSuggestedMinimumWidth()
{
    Int32 w = 0;
    mOuterRing->GetWidth(&w);
    return (Int32) (mRingScaleFactor * Elastos::Core::Math::Max((Float) w, 2 * mOuterRadius)
            + mMaxTargetWidth);
}

Int32 GlowPadView::GetScaledSuggestedMinimumHeight()
{
    Int32 h = 0;
    mOuterRing->GetHeight(&h);
    return (Int32) (mRingScaleFactor * Elastos::Core::Math::Max((Float) h, 2 * mOuterRadius)
            + mMaxTargetHeight);
}

Int32 GlowPadView::ResolveMeasured(
    /* [in] */ Int32 measureSpec,
    /* [in] */ Int32 desired)
{
    Int32 result = 0;
    Int32 specSize = View::MeasureSpec::GetSize(measureSpec);
    switch (View::MeasureSpec::GetMode(measureSpec)) {
        case View::MeasureSpec::UNSPECIFIED:
            result = desired;
            break;
        case View::MeasureSpec::AT_MOST:
            result = Elastos::Core::Math::Min(specSize, desired);
            break;
        case View::MeasureSpec::EXACTLY:
        default:
            result = specSize;
    }
    return result;
}

void GlowPadView::SwitchToState(
    /* [in] */ Int32 state,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    switch (state) {
        case STATE_IDLE: {
            DeactivateTargets();
            HideGlow(0, 0, 0.0f, NULL);
            StartBackgroundAnimation(0, 0.0f);
            mHandleDrawable->SetState(CTargetDrawable::STATE_INACTIVE);
            mHandleDrawable->SetAlpha(1.0f);
            break;
        }

        case STATE_START:
            StartBackgroundAnimation(0, 0.0f);
            break;

        case STATE_FIRST_TOUCH: {
            mHandleDrawable->SetAlpha(0.0f);
            DeactivateTargets();
            ShowTargets(TRUE);
            StartBackgroundAnimation(INITIAL_SHOW_HANDLE_DURATION, 1.0f);
            SetGrabbedState(IOnTriggerListener::CENTER_HANDLE);
            AutoPtr<IAccessibilityManagerHelper> hlp;
            CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&hlp);
            AutoPtr<IAccessibilityManager> mg;
            hlp->GetInstance(mContext, (IAccessibilityManager**)&mg);
            Boolean bEbl = FALSE;
            if ((mg->IsEnabled(&bEbl), bEbl)) {
                AnnounceTargets();
            }
            break;
        }

        case STATE_TRACKING: {
            mHandleDrawable->SetAlpha(0.0f);
            ShowGlow(REVEAL_GLOW_DURATION , REVEAL_GLOW_DELAY, 1.0f, NULL);
            break;
        }

        case STATE_SNAP: {
            // TODO: Add transition states (see list_selector_background_transition.xml)
            mHandleDrawable->SetAlpha(0.0f);
            ShowGlow(REVEAL_GLOW_DURATION , REVEAL_GLOW_DELAY, 0.0f, NULL);
            break;
        }

        case STATE_FINISH:
            DoFinish();
            break;
    }
}

void GlowPadView::ShowGlow(
    /* [in] */ Int32 duration,
    /* [in] */ Int32 delay,
    /* [in] */ Float finalAlpha,
    /* [in] */ IAnimatorListener* finishListener)
{
    mGlowAnimations->Cancel();
    AutoPtr<CPointCloud> cPC = (CPointCloud*)mPointCloud.Get();
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(10);
    AutoPtr<ICharSequence> pEA;
    CString::New(String("ease"), (ICharSequence**)&pEA);
    (*arr)[0] = pEA;
    (*arr)[1] = (IObject*)(Ease::Cubic::mEaseIn);

    AutoPtr<ICharSequence> pDl;
    CString::New(String("delay"), (ICharSequence**)&pDl);
    (*arr)[2] = pDl;

    AutoPtr<IInteger32> pDly;
    CInteger32::New(delay, (IInteger32**)&pDly);
    (*arr)[3] = pDly;

    AutoPtr<ICharSequence> pAl;
    CString::New(String("alpha"), (ICharSequence**)&pAl);
    (*arr)[4] = pAl;

    AutoPtr<IFloat> pFA;
    CFloat::New(finalAlpha, (IFloat**)&pFA);
    (*arr)[5] = pFA;

    AutoPtr<ICharSequence> pOUp;
    CString::New(String("onUpdate"), (ICharSequence**)&pOUp);
    (*arr)[6] = pOUp;

    (*arr)[7] = mUpdateListener;

    AutoPtr<ICharSequence> pComplete;
    CString::New(String("onComplete"), (ICharSequence**)&pComplete);
    (*arr)[8] = pComplete;

    (*arr)[9] = finishListener;

    AutoPtr<ITweener> res;
    Tweener::To(cPC->mGlowManager, duration, arr, (ITweener**)&res);
    IList::Probe(mGlowAnimations)->Add(res);
    mGlowAnimations->Start();
}

void GlowPadView::HideGlow(
    /* [in] */ Int32 duration,
    /* [in] */ Int32 delay,
    /* [in] */ Float finalAlpha,
    /* [in] */ IAnimatorListener* finishListener)
{
    mGlowAnimations->Cancel();
    AutoPtr<CPointCloud> cPC = (CPointCloud*)mPointCloud.Get();
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(14);
    AutoPtr<ICharSequence> pEA;
    CString::New(String("ease"), (ICharSequence**)&pEA);
    (*arr)[0] = pEA;
    (*arr)[1] = (IObject*)(Ease::Quart::mEaseOut);

    AutoPtr<ICharSequence> pDl;
    CString::New(String("delay"), (ICharSequence**)&pDl);
    (*arr)[2] = pDl;

    AutoPtr<IInteger32> pDly;
    CInteger32::New(delay, (IInteger32**)&pDly);
    (*arr)[3] = pDly;

    AutoPtr<ICharSequence> pAl;
    CString::New(String("alpha"), (ICharSequence**)&pAl);
    (*arr)[4] = pAl;

    AutoPtr<IFloat> pFA;
    CFloat::New(finalAlpha, (IFloat**)&pFA);
    (*arr)[5] = pFA;

    AutoPtr<ICharSequence> pX;
    CString::New(String("x"), (ICharSequence**)&pX);
    (*arr)[6] = pX;

    AutoPtr<IFloat> pZero;
    CFloat::New(0.0f, (IFloat**)&pZero);
    (*arr)[7] = pZero;

    AutoPtr<ICharSequence> pY;
    CString::New(String("y"), (ICharSequence**)&pY);
    (*arr)[8] = pY;

    AutoPtr<IFloat> p0;
    CFloat::New(0.0f, (IFloat**)&p0);
    (*arr)[9] = p0;

    AutoPtr<ICharSequence> pOUp;
    CString::New(String("onUpdate"), (ICharSequence**)&pOUp);
    (*arr)[10] = pOUp;

    (*arr)[11] = mUpdateListener;

    AutoPtr<ICharSequence> pComplete;
    CString::New(String("onComplete"), (ICharSequence**)&pComplete);
    (*arr)[12] = pComplete;

    (*arr)[13] = finishListener;

    AutoPtr<ITweener> res;
    Tweener::To(cPC->mGlowManager, duration, arr, (ITweener**)&res);
    IList::Probe(mGlowAnimations)->Add(res);
    mGlowAnimations->Start();
}

void GlowPadView::DeactivateTargets()
{
    Int32 count = 0;
    mTargetDrawables->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
        target->SetState(CTargetDrawable::STATE_INACTIVE);
    }
    mActiveTarget = -1;
}

void GlowPadView::DispatchTriggerEvent(
    /* [in] */ Int32 whichTarget)
{
    Vibrate();
    if (mOnTriggerListener != NULL) {
        mOnTriggerListener->OnTrigger(this, whichTarget);
    }
}

void GlowPadView::DispatchOnFinishFinalAnimation()
{
    if (mOnTriggerListener != NULL) {
        mOnTriggerListener->OnFinishFinalAnimation();
    }
}

void GlowPadView::DoFinish()
{
    Int32 activeTarget = mActiveTarget;
    Boolean targetHit =  activeTarget != -1;

    if (targetHit) {
        if (DEBUG) Logger::V(TAG, "Finish with target hit = %d", targetHit);

        HighlightSelected(activeTarget);

        // Inform listener of any active targets.  Typically only one will be active.
        HideGlow(RETURN_TO_HOME_DURATION, RETURN_TO_HOME_DELAY, 0.0f, mResetListener);
        DispatchTriggerEvent(activeTarget);
        if (!mAlwaysTrackFinger) {
            // Force ring and targets to finish animation to expanded state
            mTargetAnimations->Stop();
        }
    }
    else {
        // Animate handle back to the center based on current state.
        HideGlow(HIDE_ANIMATION_DURATION, 0, 0.0f, mResetListenerWithPing);
        HideTargets(TRUE, FALSE);
    }

    SetGrabbedState(IOnTriggerListener::NO_HANDLE);
}

void GlowPadView::HighlightSelected(
    /* [in] */ Int32 activeTarget)
{
    // Highlight the given target and fade others
    AutoPtr<IInterface> p;
    mTargetDrawables->Get(activeTarget, (IInterface**)&p);
    ITargetDrawable::Probe(p)->SetState(CTargetDrawable::STATE_ACTIVE);
    HideUnselected(activeTarget);
}

void GlowPadView::HideUnselected(
    /* [in] */ Int32 active)
{
    Int32 s = 0;
    mTargetDrawables->GetSize(&s);
    for (Int32 i = 0; i < s; i++) {
        if (i != active) {
            AutoPtr<IInterface> p;
            mTargetDrawables->Get(i, (IInterface**)&p);
            ITargetDrawable::Probe(p)->SetAlpha(0.0f);
        }
    }
}

void GlowPadView::HideTargets(
    /* [in] */ Boolean animate,
    /* [in] */ Boolean expanded)
{
    mTargetAnimations->Cancel();
    // Note: these animations should complete at the same time so that we can swap out
    // the target assets asynchronously from the setTargetResources() call.
    mAnimatingTargets = animate;
    Int32 duration = animate ? HIDE_ANIMATION_DURATION : 0;
    Int32 delay = animate ? HIDE_ANIMATION_DELAY : 0;

    Float targetScale = expanded ?
            TARGET_SCALE_EXPANDED : TARGET_SCALE_COLLAPSED;
    Int32 length = 0;
    mTargetDrawables->GetSize(&length);
    AutoPtr<ITimeInterpolator> interpolator = Ease::Cubic::mEaseOut;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
        target->SetState(CTargetDrawable::STATE_INACTIVE);

        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(12);
        AutoPtr<ICharSequence> pEA;
        CString::New(String("ease"), (ICharSequence**)&pEA);
        (*arr)[0] = pEA;
        (*arr)[1] = (IObject*)interpolator.Get();

        AutoPtr<ICharSequence> pAl;
        CString::New(String("alpha"), (ICharSequence**)&pAl);
        (*arr)[2] = pAl;

        AutoPtr<IFloat> pZero;
        CFloat::New(0.0f, (IFloat**)&pZero);
        (*arr)[3] = pZero;

        AutoPtr<ICharSequence> pX;
        CString::New(String("scaleX"), (ICharSequence**)&pX);
        (*arr)[4] = pX;

        AutoPtr<IFloat> pTS;
        CFloat::New(targetScale, (IFloat**)&pTS);
        (*arr)[5] = pTS;

        AutoPtr<ICharSequence> pY;
        CString::New(String("scaleY"), (ICharSequence**)&pY);
        (*arr)[6] = pY;

        AutoPtr<IFloat> pTS2;
        CFloat::New(targetScale, (IFloat**)&pTS2);
        (*arr)[7] = pTS2;

        AutoPtr<ICharSequence> pDl;
        CString::New(String("delay"), (ICharSequence**)&pDl);
        (*arr)[8] = pDl;

        AutoPtr<IInteger32> pDly;
        CInteger32::New(delay, (IInteger32**)&pDly);
        (*arr)[9] = pDly;

        AutoPtr<ICharSequence> pOUp;
        CString::New(String("onUpdate"), (ICharSequence**)&pOUp);
        (*arr)[10] = pOUp;

        (*arr)[11] = mUpdateListener;

        AutoPtr<ITweener> res;
        Tweener::To(target, duration, arr, (ITweener**)&res);
        IList::Probe(mTargetAnimations)->Add(res);
    }

    Float ringScaleTarget = expanded ?
            RING_SCALE_EXPANDED : RING_SCALE_COLLAPSED;
    ringScaleTarget *= mRingScaleFactor;

    AutoPtr<ArrayOf<IInterface*> > arr2 = ArrayOf<IInterface*>::Alloc(12);
    AutoPtr<ICharSequence> pEA2;
    CString::New(String("ease"), (ICharSequence**)&pEA2);
    (*arr2)[0] = pEA2;
    (*arr2)[1] = (IObject*)interpolator.Get();

    AutoPtr<ICharSequence> pAl2;
    CString::New(String("alpha"), (ICharSequence**)&pAl2);
    (*arr2)[2] = pAl2;

    AutoPtr<IFloat> pZero2;
    CFloat::New(0.0f, (IFloat**)&pZero2);
    (*arr2)[3] = pZero2;

    AutoPtr<ICharSequence> pX2;
    CString::New(String("scaleX"), (ICharSequence**)&pX2);
    (*arr2)[4] = pX2;

    AutoPtr<IFloat> pST;
    CFloat::New(ringScaleTarget, (IFloat**)&pST);
    (*arr2)[5] = pST;

    AutoPtr<ICharSequence> pY2;
    CString::New(String("scaleY"), (ICharSequence**)&pY2);
    (*arr2)[6] = pY2;

    AutoPtr<IFloat> pST2;
    CFloat::New(ringScaleTarget, (IFloat**)&pST2);
    (*arr2)[7] = pST2;

    AutoPtr<ICharSequence> pDl2;
    CString::New(String("delay"), (ICharSequence**)&pDl2);
    (*arr2)[8] = pDl2;

    AutoPtr<IInteger32> pDly2;
    CInteger32::New(delay, (IInteger32**)&pDly2);
    (*arr2)[9] = pDly2;

    AutoPtr<ICharSequence> pOUp2;
    CString::New(String("onUpdate"), (ICharSequence**)&pOUp2);
    (*arr2)[10] = pOUp2;

    (*arr2)[11] = mUpdateListener;

    AutoPtr<ICharSequence> pCom;
    CString::New(String("onComplete"), (ICharSequence**)&pCom);
    (*arr2)[12] = pCom;

    (*arr2)[13] = mTargetUpdateListener;

    AutoPtr<ITweener> res2;
    Tweener::To(mOuterRing, duration, arr2, (ITweener**)&res2);
    IList::Probe(mTargetAnimations)->Add(res2);

    mTargetAnimations->Start();
}

void GlowPadView::ShowTargets(
    /* [in] */ Boolean animate)
{
    mTargetAnimations->Stop();
    mAnimatingTargets = animate;
    Int32 delay = animate ? SHOW_ANIMATION_DELAY : 0;
    Int32 duration = animate ? SHOW_ANIMATION_DURATION : 0;
    Int32 length = 0;
    mTargetDrawables->GetSize(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
        target->SetState(CTargetDrawable::STATE_INACTIVE);

        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(12);
        AutoPtr<ICharSequence> pEA;
        CString::New(String("ease"), (ICharSequence**)&pEA);
        (*arr)[0] = pEA;
        (*arr)[1] = (IObject*)(Ease::Cubic::mEaseOut);

        AutoPtr<ICharSequence> pAl;
        CString::New(String("alpha"), (ICharSequence**)&pAl);
        (*arr)[2] = pAl;

        AutoPtr<IFloat> pZero;
        CFloat::New(1.0f, (IFloat**)&pZero);
        (*arr)[3] = pZero;

        AutoPtr<ICharSequence> pX;
        CString::New(String("scaleX"), (ICharSequence**)&pX);
        (*arr)[4] = pX;

        AutoPtr<IFloat> pOne1;
        CFloat::New(1.0f, (IFloat**)&pOne1);
        (*arr)[5] = pOne1;

        AutoPtr<ICharSequence> pY;
        CString::New(String("scaleY"), (ICharSequence**)&pY);
        (*arr)[6] = pY;

        AutoPtr<IFloat> pOne2;
        CFloat::New(1.0f, (IFloat**)&pOne2);
        (*arr)[7] = pOne2;

        AutoPtr<ICharSequence> pDl;
        CString::New(String("delay"), (ICharSequence**)&pDl);
        (*arr)[8] = pDl;

        AutoPtr<IInteger32> pDly;
        CInteger32::New(delay, (IInteger32**)&pDly);
        (*arr)[9] = pDly;

        AutoPtr<ICharSequence> pOUp;
        CString::New(String("onUpdate"), (ICharSequence**)&pOUp);
        (*arr)[10] = pOUp;

        (*arr)[11] = mUpdateListener;

        AutoPtr<ITweener> res;
        Tweener::To(target, duration, arr, (ITweener**)&res);
        IList::Probe(mTargetAnimations)->Add(res);
    }

    Float ringScale = mRingScaleFactor * RING_SCALE_EXPANDED;

    AutoPtr<ArrayOf<IInterface*> > arr2 = ArrayOf<IInterface*>::Alloc(12);
    AutoPtr<ICharSequence> pEA2;
    CString::New(String("ease"), (ICharSequence**)&pEA2);
    (*arr2)[0] = pEA2;
    (*arr2)[1] = (IObject*)Ease::Cubic::mEaseOut;

    AutoPtr<ICharSequence> pAl2;
    CString::New(String("alpha"), (ICharSequence**)&pAl2);
    (*arr2)[2] = pAl2;

    AutoPtr<IFloat> pZero2;
    CFloat::New(1.0f, (IFloat**)&pZero2);
    (*arr2)[3] = pZero2;

    AutoPtr<ICharSequence> pX2;
    CString::New(String("scaleX"), (ICharSequence**)&pX2);
    (*arr2)[4] = pX2;

    AutoPtr<IFloat> pST;
    CFloat::New(ringScale, (IFloat**)&pST);
    (*arr2)[5] = pST;

    AutoPtr<ICharSequence> pY2;
    CString::New(String("scaleY"), (ICharSequence**)&pY2);
    (*arr2)[6] = pY2;

    AutoPtr<IFloat> pST2;
    CFloat::New(ringScale, (IFloat**)&pST2);
    (*arr2)[7] = pST2;

    AutoPtr<ICharSequence> pDl2;
    CString::New(String("delay"), (ICharSequence**)&pDl2);
    (*arr2)[8] = pDl2;

    AutoPtr<IInteger32> pDly2;
    CInteger32::New(delay, (IInteger32**)&pDly2);
    (*arr2)[9] = pDly2;

    AutoPtr<ICharSequence> pOUp2;
    CString::New(String("onUpdate"), (ICharSequence**)&pOUp2);
    (*arr2)[10] = pOUp2;

    (*arr2)[11] = mUpdateListener;

    AutoPtr<ICharSequence> pCom;
    CString::New(String("onComplete"), (ICharSequence**)&pCom);
    (*arr2)[12] = pCom;

    (*arr2)[13] = mTargetUpdateListener;

    AutoPtr<ITweener> res2;
    Tweener::To(mOuterRing, duration, arr2, (ITweener**)&res2);
    IList::Probe(mTargetAnimations)->Add(res2);

    mTargetAnimations->Start();
}

void GlowPadView::Vibrate()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 val = 0;
    Settings::System::GetInt32ForUser(
            cr, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, 1,
            IUserHandle::USER_CURRENT, &val);
    Boolean hapticEnabled = val != 0;
    if (mVibrator != NULL && hapticEnabled) {
        mVibrator->Vibrate(mVibrationDuration, VIBRATION_ATTRIBUTES);
    }
}

AutoPtr<IArrayList> GlowPadView::LoadDrawableArray(
    /* [in] */ Int32 resourceId)
{
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IResources> res;
    cxt->GetResources((IResources**)&res);
    AutoPtr<ITypedArray> array;
    res->ObtainTypedArray(resourceId, (ITypedArray**)&array);
    Int32 count = 0;
    array->GetLength(&count);
    AutoPtr<IArrayList> drawables;
    CArrayList::New(count, (IArrayList**)&drawables);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<ITypedValue> value;
        array->PeekValue(i, (ITypedValue**)&value);
        AutoPtr<ITargetDrawable> target;
        Int32 resId = 0;
        CTargetDrawable::New(res, value != NULL ? (value->GetResourceId(&resId), resId) : 0, (ITargetDrawable**)&target);
        drawables->Add(target);
    }
    array->Recycle();
    return drawables;
}

void GlowPadView::InternalSetTargetResources(
    /* [in] */ Int32 resourceId)
{
    AutoPtr<IArrayList> targets = LoadDrawableArray(resourceId);
    mTargetDrawables = targets;
    mTargetResourceId = resourceId;

    Int32 maxWidth = 0;
    mHandleDrawable->GetWidth(&maxWidth);
    Int32 maxHeight = 0;
    mHandleDrawable->GetHeight(&maxHeight);
    Int32 count = 0;
    targets->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> p;
        targets->Get(i, (IInterface**)&p);
        AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
        Int32 w = 0, h = 0;
        target->GetWidth(&w);
        target->GetHeight(&h);
        maxWidth = Elastos::Core::Math::Max(maxWidth, w);
        maxHeight = Elastos::Core::Math::Max(maxHeight, h);
    }
    if (mMaxTargetWidth != maxWidth || mMaxTargetHeight != maxHeight) {
        mMaxTargetWidth = maxWidth;
        mMaxTargetHeight = maxHeight;
        RequestLayout(); // required to resize layout and call updateTargetPositions()
    }
    else {
        UpdateTargetPositions(mWaveCenterX, mWaveCenterY);
        UpdatePointCloudPosition(mWaveCenterX, mWaveCenterY);
    }
}

ECode GlowPadView::SetTargetResources(
    /* [in] */ Int32 resourceId)
{
    if (mAnimatingTargets) {
        // postpone this change until we return to the initial state
        mNewTargetResources = resourceId;
    }
    else {
        InternalSetTargetResources(resourceId);
    }
    return NOERROR;
}

ECode GlowPadView::GetTargetResourceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTargetResourceId;
    return NOERROR;
}

ECode GlowPadView::SetTargetDescriptionsResourceId(
    /* [in] */ Int32 resourceId)
{
    mTargetDescriptionsResourceId = resourceId;
    if (mTargetDescriptions != NULL) {
        mTargetDescriptions->Clear();
    }
    return NOERROR;
}

ECode GlowPadView::GetTargetDescriptionsResourceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTargetDescriptionsResourceId;
    return NOERROR;
}

ECode GlowPadView::SetDirectionDescriptionsResourceId(
    /* [in] */ Int32 resourceId)
{
    mDirectionDescriptionsResourceId = resourceId;
    if (mDirectionDescriptions != NULL) {
        mDirectionDescriptions->Clear();
    }
    return NOERROR;
}

ECode GlowPadView::GetDirectionDescriptionsResourceId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDirectionDescriptionsResourceId;
    return NOERROR;
}

ECode GlowPadView::SetVibrateEnabled(
    /* [in] */ Boolean enabled)
{
    if (enabled && mVibrator == NULL) {
        AutoPtr<IContext> cxt;
        GetContext((IContext**)&cxt);
        AutoPtr<IInterface> p;
        cxt->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&p);
        mVibrator = IVibrator::Probe(p);
    }
    else {
        mVibrator = NULL;
    }
    return NOERROR;
}

ECode GlowPadView::Ping()
{
    if (mFeedbackCount > 0) {
        Boolean doWaveAnimation = TRUE;
        AutoPtr<IAnimationBundle> waveAnimations = mWaveAnimations;

        // Don't do a wave if there's already one in progress
        Int32 s = 0;
        IList::Probe(waveAnimations)->GetSize(&s);
        AutoPtr<IInterface> p;
        IList::Probe(waveAnimations)->Get(0, (IInterface**)&p);
        AutoPtr<Tweener> realNode = (Tweener*)ITweener::Probe(p);
        Boolean bIsRn = FALSE;
        if (s > 0 && (IAnimator::Probe(realNode->mAnimator)->IsRunning(&bIsRn), bIsRn)) {
            Int64 t = 0;
            IValueAnimator::Probe(realNode->mAnimator)->GetCurrentPlayTime(&t);
            if (t < WAVE_ANIMATION_DURATION/2) {
                doWaveAnimation = FALSE;
            }
        }

        if (doWaveAnimation) {
            StartWaveAnimation();
        }
    }
    return NOERROR;
}

void GlowPadView::StopAndHideWaveAnimation()
{
    mWaveAnimations->Cancel();
    AutoPtr<PointCloud> pc = (PointCloud*)mPointCloud.Get();
    pc->mWaveManager->SetAlpha(0.0f);
}

void GlowPadView::StartWaveAnimation()
{
    mWaveAnimations->Cancel();
    AutoPtr<PointCloud> pc = (PointCloud*)mPointCloud.Get();
    pc->mWaveManager->SetAlpha(1.0f);
    Int32 w = 0;
    mHandleDrawable->GetWidth(&w);
    pc->mWaveManager->SetRadius(w/2.0f);

    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(10);
    AutoPtr<ICharSequence> pEA;
    CString::New(String("ease"), (ICharSequence**)&pEA);
    (*arr)[0] = pEA;
    (*arr)[1] = (IObject*)Ease::Quad::mEaseOut;

    AutoPtr<ICharSequence> pDl;
    CString::New(String("delay"), (ICharSequence**)&pDl);
    (*arr)[2] = pDl;

    AutoPtr<IFloat> pZero;
    CFloat::New(1.0f, (IFloat**)&pZero);
    (*arr)[3] = pZero;

    AutoPtr<ICharSequence> pRd;
    CString::New(String("radius"), (ICharSequence**)&pRd);
    (*arr)[4] = pRd;

    AutoPtr<IFloat> pOR;
    CFloat::New(2.0f * mOuterRadius, (IFloat**)&pOR);
    (*arr)[5] = pOR;

    AutoPtr<ICharSequence> pOUp;
    CString::New(String("onUpdate"), (ICharSequence**)&pOUp);
    (*arr)[6] = pOUp;

    (*arr)[7] = mUpdateListener;

    AutoPtr<ICharSequence> pCom;
    CString::New(String("onComplete"), (ICharSequence**)&pCom);
    (*arr)[8] = pCom;

    AutoPtr<AnimatorListenerAdapter_4> p = new AnimatorListenerAdapter_4(this);
    (*arr)[9] = IAnimatorListener::Probe(p);

    AutoPtr<ITweener> res;
    Tweener::To(pc->mWaveManager, WAVE_ANIMATION_DURATION, arr, (ITweener**)&res);
    IList::Probe(mWaveAnimations)->Add(res);

    mWaveAnimations->Start();
}

ECode GlowPadView::Reset(
    /* [in] */ Boolean animate)
{
    mGlowAnimations->Stop();
    mTargetAnimations->Stop();
    StartBackgroundAnimation(0, 0.0f);
    StopAndHideWaveAnimation();
    HideTargets(animate, FALSE);
    HideGlow(0, 0, 0.0f, NULL);
    Tweener::Reset();
    return NOERROR;
}

void GlowPadView::StartBackgroundAnimation(
    /* [in] */ Int32 duration,
    /* [in] */ Float alpha)
{
    AutoPtr<IDrawable> background;
    GetBackground((IDrawable**)&background);
    if (mAlwaysTrackFinger && background != NULL) {
        AutoPtr<Tweener> ba = (Tweener*)mBackgroundAnimator.Get();
        if (mBackgroundAnimator != NULL) {
            IAnimator::Probe(ba->mAnimator)->Cancel();
        }
        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(6);
        AutoPtr<ICharSequence> pEA;
        CString::New(String("ease"), (ICharSequence**)&pEA);
        (*arr)[0] = pEA;
        (*arr)[1] = (IObject*)Ease::Cubic::mEaseIn;

        AutoPtr<ICharSequence> pAl;
        CString::New(String("alpha"), (ICharSequence**)&pAl);
        (*arr)[2] = pAl;

        AutoPtr<IInteger32> pInt;
        CInteger32::New((Int32)(255.0f * alpha), (IInteger32**)&pInt);
        (*arr)[3] = pInt;

        AutoPtr<ICharSequence> pDl;
        CString::New(String("delay"), (ICharSequence**)&pDl);
        (*arr)[4] = pDl;

        AutoPtr<IInteger32> pInt2;
        CInteger32::New(SHOW_ANIMATION_DELAY, (IInteger32**)&pInt2);
        (*arr)[5] = pInt2;

        Tweener::To(background, duration, arr, (ITweener**)&mBackgroundAnimator);
        IAnimator::Probe(ba->mAnimator)->Start();
    }
}

ECode GlowPadView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action = 0;
    event->GetActionMasked(&action);
    Boolean handled = FALSE;
    switch (action) {
        case IMotionEvent::ACTION_POINTER_DOWN:
        case IMotionEvent::ACTION_DOWN:
            if (DEBUG) Logger::V(TAG, "*** DOWN ***");
            HandleDown(event);
            HandleMove(event);
            handled = TRUE;
            break;

        case IMotionEvent::ACTION_MOVE:
            if (DEBUG) Logger::V(TAG, "*** MOVE ***");
            HandleMove(event);
            handled = TRUE;
            break;

        case IMotionEvent::ACTION_POINTER_UP:
        case IMotionEvent::ACTION_UP:
            if (DEBUG) Logger::V(TAG, "*** UP ***");
            HandleMove(event);
            HandleUp(event);
            handled = TRUE;
            break;

        case IMotionEvent::ACTION_CANCEL:
            if (DEBUG) Logger::V(TAG, "*** CANCEL ***");
            HandleMove(event);
            HandleCancel(event);
            handled = TRUE;
            break;

    }
    Invalidate();
    if (handled) {
        *result = TRUE;
        return NOERROR;
    }
    else {
        return View::OnTouchEvent(event, result);
    }
}

void GlowPadView::UpdateGlowPosition(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Float oX = 0.0f, oY = 0.0f;
    mOuterRing->GetX(&oX);
    mOuterRing->GetY(&oY);
    Float dx = x - oX;
    Float dy = y - oY;
    dx *= 1.0f / mRingScaleFactor;
    dy *= 1.0f / mRingScaleFactor;
    AutoPtr<PointCloud> pc = (PointCloud*)mPointCloud.Get();
    pc->mGlowManager->SetX(oX + dx);
    pc->mGlowManager->SetY(oY + dy);
}

void GlowPadView::HandleDown(
    /* [in] */ IMotionEvent* event)
{
    Int32 actionIndex = 0;
    event->GetActionIndex(&actionIndex);
    Float eventX = 0.0f;
    event->GetX(actionIndex, &eventX);
    Float eventY = 0.0f;
    event->GetY(actionIndex, &eventY);
    SwitchToState(STATE_START, eventX, eventY);
    if (!TrySwitchToFirstTouchState(eventX, eventY)) {
        mDragging = FALSE;
    }
    else {
        event->GetPointerId(actionIndex, &mPointerId);
        UpdateGlowPosition(eventX, eventY);
    }
}

void GlowPadView::HandleUp(
    /* [in] */ IMotionEvent* event)
{
    if (DEBUG && mDragging) {
        Logger::V(TAG, "** Handle RELEASE");
    }
    Int32 actionIndex = 0;
    event->GetActionIndex(&actionIndex);
    Int32 pId = 0;
    event->GetPointerId(actionIndex, &pId);
    if (pId == mPointerId) {
        Float x = 0.0f, y = 0.0f;
        event->GetX(actionIndex, &x);
        event->GetY(actionIndex, &y);
        SwitchToState(STATE_FINISH, x, y);
    }
}

void GlowPadView::HandleCancel(
    /* [in] */ IMotionEvent* event)
{
    if (DEBUG && mDragging) {
        Logger::V(TAG, "** Handle CANCEL");
    }

    // Drop the active target if canceled.
    mActiveTarget = -1;

    Int32 actionIndex = 0;
    event->FindPointerIndex(mPointerId, &actionIndex);
    actionIndex = actionIndex == -1 ? 0 : actionIndex;
    Float aiX = 0.0f, aiY = 0.0f;
    event->GetX(actionIndex, &aiX);
    event->GetY(actionIndex, &aiY);
    SwitchToState(STATE_FINISH, aiX, aiY);
}

void GlowPadView::HandleMove(
    /* [in] */ IMotionEvent* event)
{
    Int32 activeTarget = -1;
    Int32 historySize = 0;
    event->GetHistorySize(&historySize);
    AutoPtr<IArrayList> targets = mTargetDrawables;
    Int32 ntargets = 0;
    targets->GetSize(&ntargets);
    Float x = 0.0f;
    Float y = 0.0f;
    Float activeAngle = 0.0f;
    Int32 actionIndex = 0;
    event->FindPointerIndex(mPointerId, &actionIndex);

    if (actionIndex == -1) {
        return;  // no data for this pointer
    }

    for (Int32 k = 0; k < historySize + 1; k++) {
        Float hX = 0.0f, hY = 0.0f, x = 0.0f, y = 0.0f;
        event->GetHistoricalX(actionIndex, k, &hX);
        event->GetX(actionIndex, &x);
        event->GetHistoricalY(actionIndex, k, &hY);
        event->GetY(actionIndex, &y);
        Float eventX = k < historySize ? hX : x;
        Float eventY = k < historySize ? hY : y;
        // tx and ty are relative to wave center
        Float tx = eventX - mWaveCenterX;
        Float ty = eventY - mWaveCenterY;
        Float touchRadius = (Float) Elastos::Core::Math::Sqrt(Dist2(tx, ty));
        Float scale = touchRadius > mOuterRadius ? mOuterRadius / touchRadius : 1.0f;
        Float limitX = tx * scale;
        Float limitY = ty * scale;
        Double angleRad = Elastos::Core::Math::Atan2(-ty, tx);

        if (!mDragging) {
            TrySwitchToFirstTouchState(eventX, eventY);
        }

        if (mDragging) {
            // For multiple targets, snap to the one that matches
            Float snapRadius = mRingScaleFactor * mOuterRadius - mSnapMargin;
            Float snapDistance2 = snapRadius * snapRadius;
            // Find first target in range
            for (Int32 i = 0; i < ntargets; i++) {
                AutoPtr<IInterface> p;
                targets->Get(i, (IInterface**)&p);
                AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);

                Double targetMinRad = mFirstItemOffset + (i - 0.5) * 2 * Elastos::Core::Math::PI / ntargets;
                Double targetMaxRad = mFirstItemOffset + (i + 0.5) * 2 * Elastos::Core::Math::PI / ntargets;
                Boolean bEnbl = FALSE;
                target->IsEnabled(&bEnbl);
                if (bEnbl) {
                    Boolean angleMatches =
                        (angleRad > targetMinRad && angleRad <= targetMaxRad) ||
                        (angleRad + 2 * Elastos::Core::Math::PI > targetMinRad &&
                         angleRad + 2 * Elastos::Core::Math::PI <= targetMaxRad) ||
                        (angleRad - 2 * Elastos::Core::Math::PI > targetMinRad &&
                         angleRad - 2 * Elastos::Core::Math::PI <= targetMaxRad);
                    if (angleMatches && (Dist2(tx, ty) > snapDistance2)) {
                        activeTarget = i;
                        activeAngle = (Float) -angleRad;
                    }
                }
            }
        }
        x = limitX;
        y = limitY;
    }

    if (!mDragging) {
        return;
    }

    if (activeTarget != -1) {
        SwitchToState(STATE_SNAP, x,y);
        UpdateGlowPosition(x, y);
    }
    else {
        SwitchToState(STATE_TRACKING, x, y);
        UpdateGlowPosition(x, y);
    }

    if (mActiveTarget != activeTarget) {
        // Defocus the old target
        if (mActiveTarget != -1) {
            AutoPtr<IInterface> p;
            targets->Get(mActiveTarget, (IInterface**)&p);
            AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
            Boolean bHasSt = FALSE;
            target->HasState(CTargetDrawable::STATE_FOCUSED, &bHasSt);
            if (bHasSt) {
                target->SetState(CTargetDrawable::STATE_INACTIVE);
            }
            if (mMagneticTargets) {
                UpdateTargetPosition(mActiveTarget, mWaveCenterX, mWaveCenterY);
            }
        }
        // Focus the new target
        if (activeTarget != -1) {
            AutoPtr<IInterface> p;
            targets->Get(activeTarget, (IInterface**)&p);
            AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
            Boolean bHasSt = FALSE;
            target->HasState(CTargetDrawable::STATE_FOCUSED, &bHasSt);
            if (bHasSt) {
                target->SetState(CTargetDrawable::STATE_FOCUSED);
            }
            if (mMagneticTargets) {
                UpdateTargetPosition(activeTarget, mWaveCenterX, mWaveCenterY, activeAngle);
            }
            AutoPtr<IAccessibilityManagerHelper> hlp;
            CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&hlp);
            AutoPtr<IAccessibilityManager> mg;
            hlp->GetInstance(mContext, (IAccessibilityManager**)&mg);
            Boolean b = FALSE;
            mg->IsEnabled(&b);
            if (b) {
                String targetContentDescription = GetTargetDescription(activeTarget);
                AutoPtr<ICharSequence> tCD;
                CString::New(targetContentDescription, (ICharSequence**)&tCD);
                AnnounceForAccessibility(tCD);
            }
        }
    }
    mActiveTarget = activeTarget;
}

ECode GlowPadView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IAccessibilityManagerHelper> hlp;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&hlp);
    AutoPtr<IAccessibilityManager> mg;
    hlp->GetInstance(mContext, (IAccessibilityManager**)&mg);
    Boolean b = FALSE;
    mg->IsTouchExplorationEnabled(&b);
    if (b) {
        Int32 action = 0;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_HOVER_ENTER:
                event->SetAction(IMotionEvent::ACTION_DOWN);
                break;
            case IMotionEvent::ACTION_HOVER_MOVE:
                event->SetAction(IMotionEvent::ACTION_MOVE);
                break;
            case IMotionEvent::ACTION_HOVER_EXIT:
                event->SetAction(IMotionEvent::ACTION_UP);
                break;
        }
        OnTouchEvent(event, result);
        event->SetAction(action);
    }
    View::OnHoverEvent(event, result);
    *result = TRUE;
    return NOERROR;
}

void GlowPadView::SetGrabbedState(
    /* [in] */ Int32 newState)
{
    if (newState != mGrabbedState) {
        if (newState != IOnTriggerListener::NO_HANDLE) {
            Vibrate();
        }
        mGrabbedState = newState;
        if (mOnTriggerListener != NULL) {
            if (newState == IOnTriggerListener::NO_HANDLE) {
                mOnTriggerListener->OnReleased(this, IOnTriggerListener::CENTER_HANDLE);
            }
            else {
                mOnTriggerListener->OnGrabbed(this, IOnTriggerListener::CENTER_HANDLE);
            }
            mOnTriggerListener->OnGrabbedStateChange(this, newState);
        }
    }
}

Boolean GlowPadView::TrySwitchToFirstTouchState(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Float tx = x - mWaveCenterX;
    Float ty = y - mWaveCenterY;
    if (mAlwaysTrackFinger || Dist2(tx,ty) <= GetScaledGlowRadiusSquared()) {
        if (DEBUG) {
            Logger::V(TAG, "** Handle HIT");
        }
        SwitchToState(STATE_FIRST_TOUCH, x, y);
        UpdateGlowPosition(tx, ty);
        mDragging = TRUE;
        return TRUE;
    }
    return FALSE;
}

void GlowPadView::AssignDefaultsIfNeeded()
{
    if (mOuterRadius == 0.0f) {
        Int32 w = 0, h = 0;
        mOuterRing->GetWidth(&w);
        mOuterRing->GetHeight(&h);
        mOuterRadius = Elastos::Core::Math::Max(w, h)/2.0f;
    }
    if (mSnapMargin == 0.0f) {
        AutoPtr<IContext> cxt;
        GetContext((IContext**)&cxt);
        AutoPtr<IResources> res;
        cxt->GetResources((IResources**)&res);
        AutoPtr<IDisplayMetrics> dm;
        res->GetDisplayMetrics((IDisplayMetrics**)&dm);
        AutoPtr<ITypedValueHelper> hlp;
        CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&hlp);
        hlp->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP,
                SNAP_MARGIN_DEFAULT, dm, &mSnapMargin);
    }
    if (mInnerRadius == 0.0f) {
        Int32 w = 0;
        mHandleDrawable->GetWidth(&w);
        mInnerRadius = w / 10.0f;
    }
}

void GlowPadView::ComputeInsets(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);
    AutoPtr<IGravity> gra;
    CGravity::AcquireSingleton((IGravity**)&gra);
    Int32 absoluteGravity = 0;
    gra->GetAbsoluteGravity(mGravity, layoutDirection, &absoluteGravity);

    switch (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
        case IGravity::LEFT:
            mHorizontalInset = 0;
            break;
        case IGravity::RIGHT:
            mHorizontalInset = dx;
            break;
        case IGravity::CENTER_HORIZONTAL:
        default:
            mHorizontalInset = dx / 2;
            break;
    }
    switch (absoluteGravity & IGravity::VERTICAL_GRAVITY_MASK) {
        case IGravity::TOP:
            mVerticalInset = 0;
            break;
        case IGravity::BOTTOM:
            mVerticalInset = dy;
            break;
        case IGravity::CENTER_VERTICAL:
        default:
            mVerticalInset = dy / 2;
            break;
    }
}

Float GlowPadView::ComputeScaleFactor(
    /* [in] */ Int32 desiredWidth,
    /* [in] */ Int32 desiredHeight,
    /* [in] */ Int32 actualWidth,
    /* [in] */ Int32 actualHeight)
{
    // Return unity if scaling is not allowed.
    if (!mAllowScaling) return 1.0f;

    Int32 layoutDirection = 0;
    GetLayoutDirection(&layoutDirection);
    AutoPtr<IGravity> gra;
    CGravity::AcquireSingleton((IGravity**)&gra);
    Int32 absoluteGravity = 0;
    gra->GetAbsoluteGravity(mGravity, layoutDirection, &absoluteGravity);

    Float scaleX = 1.0f;
    Float scaleY = 1.0f;

    // We use the gravity as a cue for whether we want to scale on a particular axis.
    // We only scale to fit horizontally if we're not pinned to the left or right. Likewise,
    // we only scale to fit vertically if we're not pinned to the top or bottom. In these
    // cases, we want the ring to hang off the side or top/bottom, respectively.
    switch (absoluteGravity & IGravity::HORIZONTAL_GRAVITY_MASK) {
        case IGravity::LEFT:
        case IGravity::RIGHT:
            break;
        case IGravity::CENTER_HORIZONTAL:
        default:
            if (desiredWidth > actualWidth) {
                scaleX = (1.0f * actualWidth - mMaxTargetWidth) /
                        (desiredWidth - mMaxTargetWidth);
            }
            break;
    }
    switch (absoluteGravity & IGravity::VERTICAL_GRAVITY_MASK) {
        case IGravity::TOP:
        case IGravity::BOTTOM:
            break;
        case IGravity::CENTER_VERTICAL:
        default:
            if (desiredHeight > actualHeight) {
                scaleY = (1.0f * actualHeight - mMaxTargetHeight) /
                        (desiredHeight - mMaxTargetHeight);
            }
            break;
    }
    return Elastos::Core::Math::Min(scaleX, scaleY);
}

void GlowPadView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 minimumWidth = GetSuggestedMinimumWidth();
    Int32 minimumHeight = GetSuggestedMinimumHeight();
    Int32 computedWidth = ResolveMeasured(widthMeasureSpec, minimumWidth);
    Int32 computedHeight = ResolveMeasured(heightMeasureSpec, minimumHeight);

    mRingScaleFactor = ComputeScaleFactor(minimumWidth, minimumHeight,
            computedWidth, computedHeight);

    Int32 scaledWidth = GetScaledSuggestedMinimumWidth();
    Int32 scaledHeight = GetScaledSuggestedMinimumHeight();

    ComputeInsets(computedWidth - scaledWidth, computedHeight - scaledHeight);
    SetMeasuredDimension(computedWidth, computedHeight);
}

Float GlowPadView::GetRingWidth()
{
    Int32 w = 0;
    mOuterRing->GetWidth(&w);
    return mRingScaleFactor * Elastos::Core::Math::Max((Float) w, 2 * mOuterRadius);
}

Float GlowPadView::GetRingHeight()
{
    Int32 h = 0;
    mOuterRing->GetHeight(&h);
    return mRingScaleFactor * Elastos::Core::Math::Max((Float) h, 2 * mOuterRadius);
}

ECode GlowPadView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    View::OnLayout(changed, left, top, right, bottom);
    Int32 width = right - left;
    Int32 height = bottom - top;

    // Target placement width/height. This puts the targets on the greater of the ring
    // width or the specified outer radius.
    Float placementWidth = GetRingWidth();
    Float placementHeight = GetRingHeight();
    Float newWaveCenterX = mHorizontalInset
            + Elastos::Core::Math::Max((Float) width, mMaxTargetWidth + placementWidth) / 2;
    Float newWaveCenterY = mVerticalInset
            + Elastos::Core::Math::Max((Float) height, + mMaxTargetHeight + placementHeight) / 2;

    if (mInitialLayout) {
        StopAndHideWaveAnimation();
        HideTargets(FALSE, FALSE);
        mInitialLayout = FALSE;
    }

    mOuterRing->SetPositionX(newWaveCenterX);
    mOuterRing->SetPositionY(newWaveCenterY);

    mPointCloud->SetScale(mRingScaleFactor);

    mHandleDrawable->SetPositionX(newWaveCenterX);
    mHandleDrawable->SetPositionY(newWaveCenterY);

    UpdateTargetPositions(newWaveCenterX, newWaveCenterY);
    UpdatePointCloudPosition(newWaveCenterX, newWaveCenterY);
    UpdateGlowPosition(newWaveCenterX, newWaveCenterY);

    mWaveCenterX = newWaveCenterX;
    mWaveCenterY = newWaveCenterY;

    if (DEBUG) {
        Dump();
    }
    return NOERROR;
}

void GlowPadView::UpdateTargetPosition(
    /* [in] */ Int32 i,
    /* [in] */ Float centerX,
    /* [in] */ Float centerY)
{
    Float angle = GetAngle(GetSliceAngle(), i);
    UpdateTargetPosition(i, centerX, centerY, angle);
}

void GlowPadView::UpdateTargetPosition(
    /* [in] */ Int32 i,
    /* [in] */ Float centerX,
    /* [in] */ Float centerY,
    /* [in] */ Float angle)
{
    Float placementRadiusX = GetRingWidth() / 2;
    Float placementRadiusY = GetRingHeight() / 2;
    if (i >= 0) {
        AutoPtr<IArrayList> targets = mTargetDrawables;
        AutoPtr<IInterface> p;
        targets->Get(i, (IInterface**)&p);
        AutoPtr<ITargetDrawable> targetIcon = ITargetDrawable::Probe(p);
        targetIcon->SetPositionX(centerX);
        targetIcon->SetPositionY(centerY);
        targetIcon->SetX(placementRadiusX * (Float) Elastos::Core::Math::Cos(angle));
        targetIcon->SetY(placementRadiusY * (Float) Elastos::Core::Math::Sin(angle));
    }
}

void GlowPadView::UpdateTargetPositions(
    /* [in] */ Float centerX,
    /* [in] */ Float centerY)
{
    UpdateTargetPositions(centerX, centerY, FALSE);
}

void GlowPadView::UpdateTargetPositions(
    /* [in] */ Float centerX,
    /* [in] */ Float centerY,
    /* [in] */ Boolean skipActive)
{
    Int32 size = 0;
    mTargetDrawables->GetSize(&size);
    Float alpha = GetSliceAngle();
    // Reposition the target drawables if the view changed.
    for (Int32 i = 0; i < size; i++) {
        if (!skipActive || i != mActiveTarget) {
            UpdateTargetPosition(i, centerX, centerY, GetAngle(alpha, i));
        }
    }
}

Float GlowPadView::GetAngle(
    /* [in] */ Float alpha,
    /* [in] */ Int32 i)
{
    return mFirstItemOffset + alpha * i;
}

Float GlowPadView::GetSliceAngle()
{
    Int32 s = 0;
    mTargetDrawables->GetSize(&s);
    return (Float) (-2.0f * Elastos::Core::Math::PI / s);
}

void GlowPadView::UpdatePointCloudPosition(
    /* [in] */ Float centerX,
    /* [in] */ Float centerY)
{
    mPointCloud->SetCenter(centerX, centerY);
}

void GlowPadView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    mPointCloud->Draw(canvas);
    mOuterRing->Draw(canvas);
    Int32 ntargets = 0;
    mTargetDrawables->GetSize(&ntargets);
    for (Int32 i = 0; i < ntargets; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
        if (target != NULL) {
            target->Draw(canvas);
        }
    }
    mHandleDrawable->Draw(canvas);
}

ECode GlowPadView::SetOnTriggerListener(
    /* [in] */ IOnTriggerListener* listener)
{
    mOnTriggerListener = listener;
    return NOERROR;
}

Float GlowPadView::Square(
    /* [in] */ Float d)
{
    return d * d;
}

Float GlowPadView::Dist2(
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    return dx*dx + dy*dy;
}

Float GlowPadView::GetScaledGlowRadiusSquared()
{
    Float scaledTapRadius = 0.0f;
    AutoPtr<IAccessibilityManagerHelper> hlp;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&hlp);
    AutoPtr<IAccessibilityManager> mg;
    hlp->GetInstance(mContext, (IAccessibilityManager**)&mg);
    Boolean b = FALSE;
    mg->IsEnabled(&b);
    if (b) {
        scaledTapRadius = TAP_RADIUS_SCALE_ACCESSIBILITY_ENABLED * mGlowRadius;
    }
    else {
        scaledTapRadius = mGlowRadius;
    }
    return Square(scaledTapRadius);
}

void GlowPadView::AnnounceTargets()
{
    StringBuilder utterance;
    Int32 targetCount = 0;
    mTargetDrawables->GetSize(&targetCount);
    for (Int32 i = 0; i < targetCount; i++) {
        String targetDescription = GetTargetDescription(i);
        String directionDescription = GetDirectionDescription(i);
        if (!TextUtils::IsEmpty(targetDescription)
                && !TextUtils::IsEmpty(directionDescription)) {
            AutoPtr<ICharSequence> cs;
            CString::New(targetDescription, (ICharSequence**)&cs);
            AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
            (*arr)[0] = cs;
            String text = StringUtils::Format(directionDescription, arr);
            utterance.Append(text);
        }
    }
    if (utterance.GetLength() > 0) {
        String str = utterance.ToString();
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        AnnounceForAccessibility(cs);
    }
}

String GlowPadView::GetTargetDescription(
    /* [in] */ Int32 index)
{
    Boolean bEmp = FALSE;
    if (mTargetDescriptions == NULL || (mTargetDescriptions->IsEmpty(&bEmp), bEmp)) {
        mTargetDescriptions = LoadDescriptions(mTargetDescriptionsResourceId);
        Int32 s1 = 0, s2 = 0;
        mTargetDrawables->GetSize(&s1);
        mTargetDescriptions->GetSize(&s2);
        if (s1 != s2) {
            Logger::W(TAG, "The number of target drawables must be"
                        " equal to the number of target descriptions.");
            return String(NULL);
        }
    }
    AutoPtr<IInterface> p;
    mTargetDescriptions->Get(index, (IInterface**)&p);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(p);
    String str;
    cs->ToString(&str);
    return str;
}

String GlowPadView::GetDirectionDescription(
    /* [in] */ Int32 index)
{
    Boolean bEmp = FALSE;
    if (mDirectionDescriptions == NULL || (mDirectionDescriptions->IsEmpty(&bEmp), bEmp)) {
        mDirectionDescriptions = LoadDescriptions(mDirectionDescriptionsResourceId);
        Int32 s1 = 0, s2 = 0;
        mTargetDrawables->GetSize(&s1);
        mDirectionDescriptions->GetSize(&s2);
        if (s1 != s2) {
            Logger::W(TAG, "The number of target drawables must be"
                        " equal to the number of direction descriptions.");
            return String(NULL);
        }
    }
    AutoPtr<IInterface> p;
    mDirectionDescriptions->Get(index, (IInterface**)&p);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(p);
    String str;
    cs->ToString(&str);
    return str;
}

AutoPtr<IArrayList> GlowPadView::LoadDescriptions(
    /* [in] */ Int32 resourceId)
{
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IResources> res;
    cxt->GetResources((IResources**)&res);
    AutoPtr<ITypedArray> array;
    res->ObtainTypedArray(resourceId, (ITypedArray**)&array);
    Int32 count = 0;
    array->GetLength(&count);
    AutoPtr<IArrayList> targetContentDescriptions;
    CArrayList::New(count, (IArrayList**)&targetContentDescriptions);
    for (Int32 i = 0; i < count; i++) {
        String contentDescription;
        array->GetString(i, &contentDescription);
        AutoPtr<ICharSequence> cs;
        CString::New(contentDescription, (ICharSequence**)&cs);
        targetContentDescriptions->Add(cs);
    }
    array->Recycle();
    return targetContentDescriptions;
}

ECode GlowPadView::GetResourceIdForTarget(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> p;
    mTargetDrawables->Get(index, (IInterface**)&p);
    AutoPtr<ITargetDrawable> drawable = ITargetDrawable::Probe(p);
    Int32 id = 0;
    *result = drawable == NULL ? 0 : (drawable->GetResourceId(&id), id);
    return NOERROR;
}

ECode GlowPadView::SetEnableTarget(
    /* [in] */ Int32 resourceId,
    /* [in] */ Boolean enabled)
{
    Int32 s = 0;
    mTargetDrawables->GetSize(&s);
    for (Int32 i = 0; i < s; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
        Int32 id = 0;
        target->GetResourceId(&id);
        if (id == resourceId) {
            target->SetEnabled(enabled);
            break; // should never be more than one match
        }
    }
    return NOERROR;
}

ECode GlowPadView::GetTargetPosition(
    /* [in] */ Int32 resourceId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 s = 0;
    mTargetDrawables->GetSize(&s);
    for (Int32 i = 0; i < s; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
        Int32 id = 0;
        target->GetResourceId(&id);
        if (id == resourceId) {
            *result = i; // should never be more than one match
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

Boolean GlowPadView::ReplaceTargetDrawables(
    /* [in] */ IResources* res,
    /* [in] */ Int32 existingResourceId,
    /* [in] */ Int32 newResourceId)
{
    if (existingResourceId == 0 || newResourceId == 0) {
        return FALSE;
    }

    Boolean result = FALSE;
    AutoPtr<IArrayList> drawables = mTargetDrawables;
    Int32 size = 0;
    drawables->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        mTargetDrawables->Get(i, (IInterface**)&p);
        AutoPtr<ITargetDrawable> target = ITargetDrawable::Probe(p);
        Int32 id = 0;
        target->GetResourceId(&id);
        if (target != NULL && id == existingResourceId) {
            target->SetDrawable(res, newResourceId);
            result = TRUE;
        }
    }

    if (result) {
        RequestLayout(); // in case any given drawable's size changes
    }

    return result;
}

ECode GlowPadView::ReplaceTargetDrawablesIfPresent(
    /* [in] */ IComponentName* component,
    /* [in] */ const String& name,
    /* [in] */ Int32 existingResId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (existingResId == 0) {
        return FALSE;
    }

    Boolean replaced = FALSE;
    if (component != NULL) {
        // try {
        AutoPtr<IPackageManager> packageManager;
        mContext->GetPackageManager((IPackageManager**)&packageManager);
        // Look for the search icon specified in the activity meta-data
        AutoPtr<IActivityInfo> ainfo;
        packageManager->GetActivityInfo(
                component, IPackageManager::GET_META_DATA, (IActivityInfo**)&ainfo);
        AutoPtr<CActivityInfo> cinfo = (CActivityInfo*)ainfo.Get();
        AutoPtr<IBundle> metaData = cinfo->mMetaData;
        if (metaData != NULL) {
            Int32 iconResId = 0;
            metaData->GetInt32(name, &iconResId);
            if (iconResId != 0) {
                AutoPtr<IResources> res;
                packageManager->GetResourcesForActivity(component, (IResources**)&res);
                replaced = ReplaceTargetDrawables(res, existingResId, iconResId);
            }
        }
        // } catch (NameNotFoundException e) {
        //     Log.w(TAG, "Failed to swap drawable; "
        //             + component.flattenToShortString() + " not found", e);
        // } catch (Resources.NotFoundException nfe) {
        //     Log.w(TAG, "Failed to swap drawable from "
        //             + component.flattenToShortString(), nfe);
        // }
    }
    if (!replaced) {
        // Restore the original drawable
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        ReplaceTargetDrawables(res, existingResId, existingResId);
    }
    return replaced;
}

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
