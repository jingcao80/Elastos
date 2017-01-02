//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/CValueAnimator.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/ViewPropertyAnimator.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CValueAnimator;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::ValueAnimator;
using Elastos::Droid::View::EIID_IViewPropertyAnimator;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 ViewPropertyAnimator::NONE;
const Int32 ViewPropertyAnimator::TRANSLATION_X;
const Int32 ViewPropertyAnimator::TRANSLATION_Y;
const Int32 ViewPropertyAnimator::TRANSLATION_Z;
const Int32 ViewPropertyAnimator::SCALE_X;
const Int32 ViewPropertyAnimator::SCALE_Y;
const Int32 ViewPropertyAnimator::ROTATION;
const Int32 ViewPropertyAnimator::ROTATION_X;
const Int32 ViewPropertyAnimator::ROTATION_Y;
const Int32 ViewPropertyAnimator::_X;
const Int32 ViewPropertyAnimator::_Y;
const Int32 ViewPropertyAnimator::_Z;
const Int32 ViewPropertyAnimator::ALPHA;
const Int32 ViewPropertyAnimator::TRANSFORM_MASK;

//=====================================================================
//         ViewPropertyAnimator::AnimatorEventListener
//=====================================================================
CAR_INTERFACE_IMPL_2(ViewPropertyAnimator::AnimatorEventListener, Object, \
    IAnimatorUpdateListener, IAnimatorListener)

ViewPropertyAnimator::AnimatorEventListener::AnimatorEventListener(
    /* [in] */ ViewPropertyAnimator* host)
    : mHost(host)
{
    assert(NULL != mHost);
}

ECode ViewPropertyAnimator::AnimatorEventListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IInterface> animObj = TO_IINTERFACE(animation);
    if (mHost->mAnimatorSetupMap != NULL) {
        Boolean containKey = FALSE;
        mHost->mAnimatorSetupMap->ContainsKey(animObj, &containKey);
        if (containKey) {
            AutoPtr<IInterface> valueTmp;
            mHost->mAnimatorSetupMap->Get(animObj, (IInterface**)&valueTmp);
            IRunnable* value = IRunnable::Probe(valueTmp);
            value->Run();
            mHost->mAnimatorSetupMap->Remove(animObj);
        }
    }
    if (mHost->mAnimatorOnStartMap != NULL) {
        Boolean containKey = FALSE;
        mHost->mAnimatorOnStartMap->ContainsKey(animObj, &containKey);
        if (containKey) {
            AutoPtr<IInterface> valueTmp;
            mHost->mAnimatorOnStartMap->Get(animObj, (IInterface**)&valueTmp);
            IRunnable* value = IRunnable::Probe(valueTmp);
            value->Run();
            mHost->mAnimatorOnStartMap->Remove(animObj);
        }
    }
    if (mHost->mListener != NULL) {
        mHost->mListener->OnAnimationStart(animation);
    }
    return NOERROR;
}

ECode ViewPropertyAnimator::AnimatorEventListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IInterface> animObj = TO_IINTERFACE(animation);
    mHost->mView->SetHasTransientState(FALSE);
    if (mHost->mListener != NULL) {
        mHost->mListener->OnAnimationEnd(animation);
    }
    if (mHost->mAnimatorOnEndMap != NULL) {
        Boolean containKey = FALSE;
        mHost->mAnimatorOnEndMap->ContainsKey(animObj, &containKey);
        if (containKey) {
            AutoPtr<IInterface> valueTmp;
            mHost->mAnimatorOnEndMap->Get(animObj, (IInterface**)&valueTmp);
            IRunnable* value = IRunnable::Probe(valueTmp);
            value->Run();
            mHost->mAnimatorOnEndMap->Remove(animObj);
        }
    }
    if (mHost->mAnimatorCleanupMap != NULL) {
        Boolean containKey = FALSE;
        mHost->mAnimatorCleanupMap->ContainsKey(animObj, &containKey);
        if (containKey) {
            AutoPtr<IInterface> valueTmp;
            mHost->mAnimatorCleanupMap->Get(animObj, (IInterface**)&valueTmp);
            IRunnable* value = IRunnable::Probe(valueTmp);
            value->Run();
            mHost->mAnimatorCleanupMap->Remove(animObj);
        }
    }
    mHost->mAnimatorMap->Remove(animObj);
    return NOERROR;
}

ECode ViewPropertyAnimator::AnimatorEventListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IInterface> animObj = TO_IINTERFACE(animation);
    if (mHost->mListener != NULL) {
        mHost->mListener->OnAnimationCancel(animation);
    }

    if (mHost->mAnimatorOnEndMap != NULL) {
        Boolean isEmpty = FALSE;
        mHost->mAnimatorOnEndMap->IsEmpty(&isEmpty);
        if (!isEmpty) {
            mHost->mAnimatorOnEndMap->Remove(animObj);
        }
    }
    return NOERROR;
}

ECode ViewPropertyAnimator::AnimatorEventListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mListener != NULL) {
        mHost->mListener->OnAnimationRepeat(animation);
    }
    return NOERROR;
}

ECode ViewPropertyAnimator::AnimatorEventListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    IInterface* animator = TO_IINTERFACE(animation);
    AutoPtr<PropertyBundle> propertyBundle = NULL;
    Boolean containKey = FALSE;
    mHost->mAnimatorMap->ContainsKey(animator, &containKey);
    if (containKey) {
        AutoPtr<IInterface> valueTmp;
        mHost->mAnimatorMap->Get(animator, (IInterface**)&valueTmp);
        propertyBundle = (PropertyBundle*)IObject::Probe(valueTmp);
    }
    if (propertyBundle == NULL) {
        // Shouldn't happen, but just to play it safe
        return NOERROR;
    }

    View* hostView = VIEW_PROBE(mHost->mView);
    Boolean hardwareAccelerated = FALSE;
    hostView->IsHardwareAccelerated(&hardwareAccelerated);

    // alpha requires slightly different treatment than the other (transform) properties.
    // The logic in setAlpha() is not simply setting mAlpha, plus the invalidation
    // logic is dependent on how the view handles an internal call to onSetAlpha().
    // We track what kinds of properties are set, and how alpha is handled when it is
    // set, and perform the invalidation steps appropriately.
    Boolean alphaHandled = FALSE;
    if (!hardwareAccelerated) {
        hostView->InvalidateParentCaches();
    }
    Float fraction;
    animation->GetAnimatedFraction(&fraction);
    Int32 propertyMask = propertyBundle->mPropertyMask;
    if ((propertyMask & TRANSFORM_MASK) != 0) {
        hostView->InvalidateViewProperty(hardwareAccelerated, FALSE);
    }
    AutoPtr< List<AutoPtr<NameValuesHolder> > > valueList = propertyBundle->mNameValuesHolder;
    if (valueList != NULL) {
        List<AutoPtr<NameValuesHolder> >::Iterator lsIt = valueList->Begin();
        for (; lsIt != valueList->End(); lsIt++) {
            AutoPtr<NameValuesHolder> values = *lsIt;
            Float value = values->mFromValue + fraction * values->mDeltaValue;
            if (values->mNameConstant == ViewPropertyAnimator::ALPHA) {
                alphaHandled = hostView->SetAlphaNoInvalidation(value);
            } else {
                mHost->SetValue(values->mNameConstant, value);
            }
        }
    }
    if ((propertyMask & TRANSFORM_MASK) != 0) {
        if (!hardwareAccelerated) {
            hostView->mPrivateFlags |= View::PFLAG_DRAWN; // force another invalidation
        }
    }
    // invalidate(false) in all cases except if alphaHandled gets set to true
    // via the call to setAlphaNoInvalidation(), above
    if (alphaHandled) {
        hostView->Invalidate(TRUE);
    }
    else {
        hostView->InvalidateViewProperty(FALSE, FALSE);
    }

    if (mHost->mUpdateListener != NULL) {
        mHost->mUpdateListener->OnAnimationUpdate(animation);
    }
    return NOERROR;
}

//=====================================================================
//              ViewPropertyAnimator::PropertyBundle
//=====================================================================
ViewPropertyAnimator::PropertyBundle::PropertyBundle(
    /* [in] */ Int32 propertyMask,
    /* [in] */ List<AutoPtr<NameValuesHolder> >* nameValuesHolder)
    : mNameValuesHolder(nameValuesHolder)
{
    mPropertyMask = propertyMask;
}

ViewPropertyAnimator::PropertyBundle::~PropertyBundle()
{
}

Boolean ViewPropertyAnimator::PropertyBundle::Cancel(
    /* [in] */ Int32 propertyConstant)
{
    if ((mPropertyMask & propertyConstant) != 0 && mNameValuesHolder != NULL) {
        List<AutoPtr<NameValuesHolder> >::Iterator lsIt = mNameValuesHolder->Begin();
        for (; lsIt != mNameValuesHolder->End(); lsIt++) {
            AutoPtr<NameValuesHolder> nameValuesHolder = *lsIt;
            if (nameValuesHolder->mNameConstant == propertyConstant) {
                mNameValuesHolder->Erase(lsIt);
                mPropertyMask &= ~propertyConstant;
                return TRUE;
            }
        }
    }
    return FALSE;
}

//=====================================================================
//         ViewPropertyAnimator::InnerStartAnimationRunnable
//=====================================================================
CAR_INTERFACE_IMPL(ViewPropertyAnimator::InnerStartAnimationRunnable, Object, IRunnable)

ViewPropertyAnimator::InnerStartAnimationRunnable::InnerStartAnimationRunnable(
    /* [in] */ ViewPropertyAnimator* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ViewPropertyAnimator::InnerStartAnimationRunnable::Run()
{
    mOwner->StartAnimation();
    return NOERROR;
}

//=====================================================================
//         ViewPropertyAnimator::InnerBuildLayerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(ViewPropertyAnimator::InnerBuildLayerRunnable, Object, IRunnable)

ViewPropertyAnimator::InnerBuildLayerRunnable::InnerBuildLayerRunnable(
    /* [in] */ ViewPropertyAnimator* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ViewPropertyAnimator::InnerBuildLayerRunnable::Run()
{
    mOwner->mView->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
    Boolean resTmp = FALSE;
    mOwner->mView->IsAttachedToWindow(&resTmp);
    if (resTmp) {
        mOwner->mView->BuildLayer();
    }
    return NOERROR;
}

//=====================================================================
//         ViewPropertyAnimator::InnerSetLayerTypeRunnable
//=====================================================================
CAR_INTERFACE_IMPL(ViewPropertyAnimator::InnerSetLayerTypeRunnable, Object, IRunnable)

ViewPropertyAnimator::InnerSetLayerTypeRunnable::InnerSetLayerTypeRunnable(
    /* [in] */ ViewPropertyAnimator* owner,
    /* [in] */ Int32 type)
    : mOwner(owner)
    , mType(type)
{
    assert(mOwner);
}

ECode ViewPropertyAnimator::InnerSetLayerTypeRunnable::Run()
{
    mOwner->mView->SetLayerType(mType, NULL);
    return NOERROR;
}

//=====================================================================
//                          ViewPropertyAnimator
//=====================================================================
CAR_INTERFACE_IMPL(ViewPropertyAnimator, Object, IViewPropertyAnimator)

ViewPropertyAnimator::ViewPropertyAnimator()
    : mDuration(0)
    , mDurationSet(FALSE)
    , mStartDelay(0)
    , mStartDelaySet(FALSE)
    , mInterpolatorSet(FALSE)
{
}

ViewPropertyAnimator::~ViewPropertyAnimator()
{
    mPendingAnimations = NULL;
    mAnimatorMap = NULL;
    mAnimatorSetupMap = NULL;
    mAnimatorCleanupMap = NULL;
    mAnimatorOnStartMap = NULL;
    mAnimatorOnEndMap = NULL;
}

ECode ViewPropertyAnimator::constructor(
    /* [in] */ IView* v)
{
    mView = v;
    ((View*)v)->EnsureTransformationInfo();

    mPendingAnimations = new List<AutoPtr<NameValuesHolder> >();
    CHashMap::New((IHashMap**)&mAnimatorMap);
    mAnimatorEventListener = new AnimatorEventListener(this);
    mAnimationStarter = new InnerStartAnimationRunnable(this);
    return NOERROR;
}

ECode ViewPropertyAnimator::SetDuration(
    /* [in] */ Int64 duration)
{
    if (duration < 0) {
        // throw new IllegalArgumentException("Animators cannot have negative duration: " +
        //         duration);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDurationSet = TRUE;
    mDuration = duration;
    return NOERROR;
}

ECode ViewPropertyAnimator::GetDuration(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    if (mDurationSet) {
        *result = mDuration;
        return NOERROR;
    }
    else {
        // Just return the default from ValueAnimator, since that's what we'd get if
        // the value has not been set otherwise
        if (NULL == mTempValueAnimator) {
            CValueAnimator::New((IValueAnimator**)&mTempValueAnimator);
        }

        return mTempValueAnimator->GetDuration(result);
    }
}

ECode ViewPropertyAnimator::GetStartDelay(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    if (mStartDelaySet) {
        *result = mStartDelay;
    }
    else {
        // Just return the default from ValueAnimator (0), since that's what we'd get if
        // the value has not been set otherwise
        *result = 0;
    }
    return NOERROR;
}

ECode ViewPropertyAnimator::SetStartDelay(
    /* [in] */ Int64 startDelay)
{
    if (startDelay < 0) {
        //throw new IllegalArgumentException("Animators cannot have negative start " +
        //        "delay: " + startDelay);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mStartDelaySet = TRUE;
    mStartDelay = startDelay;
    return NOERROR;
}

ECode ViewPropertyAnimator::SetInterpolator(
    /* [in] */ ITimeInterpolator* interpolator)
{
    mInterpolatorSet = TRUE;
    mInterpolator = interpolator;
    return NOERROR;
}

ECode ViewPropertyAnimator::GetInterpolator(
    /* [out] */ ITimeInterpolator** result)
{
    VALIDATE_NOT_NULL(result);

    if (mInterpolatorSet) {
        *result = mInterpolator;
        REFCOUNT_ADD(*result);
    }
    else {
        // Just return the default from ValueAnimator, since that's what we'd get if
        // the value has not been set otherwise
        if (NULL == mTempValueAnimator) {
            CValueAnimator::New((IValueAnimator**)&mTempValueAnimator);
        }
        mTempValueAnimator->GetInterpolator(result);
    }
    return NOERROR;
}

ECode ViewPropertyAnimator::SetListener(
    /* [in] */ IAnimatorListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode ViewPropertyAnimator::GetListener(
    /* [out] */ IAnimatorListener** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mListener;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ViewPropertyAnimator::SetUpdateListener(
    /* [in] */ IAnimatorUpdateListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    mUpdateListener = listener;
    return NOERROR;
}

ECode ViewPropertyAnimator::GetUpdateListener(
    /* [out] */ IAnimatorUpdateListener** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUpdateListener;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ViewPropertyAnimator::Start()
{
    Boolean resTmp = FALSE;
    VIEW_PROBE(mView)->RemoveCallbacks(mAnimationStarter, &resTmp);
    StartAnimation();
    return NOERROR;
}

ECode ViewPropertyAnimator::Cancel()
{
    Int32 size = 0;
    mAnimatorMap->GetSize(&size);
    if (size > 0) {
        AutoPtr<ISet> keySet;
        mAnimatorMap->GetKeySet((ISet**)&keySet);
        AutoPtr< ArrayOf<IInterface*> > keyArray;
        keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
        Int32 keySize = keyArray->GetLength();
        for (Int32 idx=0; idx<keySize; ++idx) {
            IAnimator* animatorTmp = IAnimator::Probe((*keyArray)[idx]);
            animatorTmp->Cancel();
        }
    }
    mPendingAnimations->Clear();
    mPendingSetupAction = NULL;
    mPendingCleanupAction = NULL;
    mPendingOnStartAction = NULL;
    mPendingOnEndAction = NULL;

    Boolean resTmp = FALSE;
    mView->RemoveCallbacks(mAnimationStarter, &resTmp);
    // if (mRTBackend != NULL) {
    //     mRTBackend->CancelAll();
    // }
    return NOERROR;
}

ECode ViewPropertyAnimator::X(
    /* [in] */ Float value)
{
    AnimateProperty(_X, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::XBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(_X, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::Y(
    /* [in] */ Float value)
{
    AnimateProperty(_Y, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::YBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(_Y, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::Z(
    /* [in] */ Float value)
{
    AnimateProperty(_Z, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::ZBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(_Z, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::Rotation(
    /* [in] */ Float value)
{
    AnimateProperty(ROTATION, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::RotationBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(ROTATION, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::RotationX(
    /* [in] */ Float value)
{
    AnimateProperty(ROTATION_X, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::RotationXBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(ROTATION_X, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::RotationY(
    /* [in] */ Float value)
{
    AnimateProperty(ROTATION_Y, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::RotationYBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(ROTATION_Y, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::TranslationX(
    /* [in] */ Float value)
{
    AnimateProperty(TRANSLATION_X, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::TranslationXBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(TRANSLATION_X, value);
    return NOERROR;
}


ECode ViewPropertyAnimator::TranslationY(
    /* [in] */ Float value)
{
    AnimateProperty(TRANSLATION_Y, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::TranslationYBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(TRANSLATION_Y, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::TranslationZ(
    /* [in] */ Float value)
{
    AnimateProperty(TRANSLATION_Z, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::TranslationZBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(TRANSLATION_Z, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::ScaleX(
    /* [in] */ Float value)
{
    AnimateProperty(SCALE_X, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::ScaleXBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(SCALE_X, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::ScaleY(
    /* [in] */ Float value)
{
    AnimateProperty(SCALE_Y, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::ScaleYBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(SCALE_Y, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::Alpha(
    /* [in] */ Float value)
{
    AnimateProperty(ALPHA, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::AlphaBy(
    /* [in] */ Float value)
{
    AnimatePropertyBy(ALPHA, value);
    return NOERROR;
}

ECode ViewPropertyAnimator::WithLayer()
{
    mPendingSetupAction = new InnerBuildLayerRunnable(this);
    Int32 currentLayerType;
    mView->GetLayerType(&currentLayerType);
    mPendingCleanupAction = new InnerSetLayerTypeRunnable(this, currentLayerType);
    if (mAnimatorSetupMap == NULL) {
        CHashMap::New((IHashMap**)&mAnimatorSetupMap);
    }
    if (mAnimatorCleanupMap == NULL) {
        CHashMap::New((IHashMap**)&mAnimatorCleanupMap);
    }
    return NOERROR;
}

ECode ViewPropertyAnimator::WithStartAction(
    /* [in] */ IRunnable* runnable)
{
    mPendingOnStartAction = runnable;
    if (runnable != NULL && mAnimatorOnStartMap == NULL) {
        CHashMap::New((IHashMap**)&mAnimatorOnStartMap);
    }
    return NOERROR;
}

ECode ViewPropertyAnimator::WithEndAction(
    /* [in] */ IRunnable* runnable)
{
    mPendingOnEndAction = runnable;
    if (runnable != NULL && mAnimatorOnEndMap == NULL) {
        CHashMap::New((IHashMap**)&mAnimatorOnEndMap);
    }
    return NOERROR;
}

ECode ViewPropertyAnimator::HasActions(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPendingSetupAction != NULL
        || mPendingCleanupAction != NULL
        || mPendingOnStartAction != NULL
        || mPendingOnEndAction != NULL;
    return NOERROR;
}

ECode ViewPropertyAnimator::StartAnimation()
{
    // Boolean bval;
    // if (mRTBackend != NULL && (mRTBackend->StartAnimation(this, &bval), bval)) {
    //     return NOERROR;
    // }

    mView->SetHasTransientState(TRUE);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
    (*array)[0] = 1.0f;
    AutoPtr<IValueAnimator> animator = ValueAnimator::OfFloat(array);
    AutoPtr<List<AutoPtr<NameValuesHolder> > > nameValueList = new List<AutoPtr<NameValuesHolder> >(*mPendingAnimations);
    mPendingAnimations->Clear();
    Int32 propertyMask = 0;
    List<AutoPtr<NameValuesHolder> >::Iterator it = nameValueList->Begin();
    for (; it != nameValueList->End(); it++) {
        AutoPtr<NameValuesHolder> nameValuesHolder = *it;
        propertyMask |= nameValuesHolder->mNameConstant;
    }

    AutoPtr<PropertyBundle> pb = new PropertyBundle(propertyMask, nameValueList);
    IAnimator* animatorTmp = IAnimator::Probe(animator);
    assert(NULL != animatorTmp);
    IInterface* animObj = TO_IINTERFACE(animator);
    mAnimatorMap->Put(animObj, TO_IINTERFACE(pb));
    if (mPendingSetupAction != NULL) {
        mAnimatorSetupMap->Put(animObj, TO_IINTERFACE(mPendingSetupAction));
        mPendingSetupAction = NULL;
    }
    if (mPendingCleanupAction != NULL) {
        mAnimatorCleanupMap->Put(animObj, TO_IINTERFACE(mPendingCleanupAction));
        mPendingCleanupAction = NULL;
    }
    if (mPendingOnStartAction != NULL) {
        mAnimatorOnStartMap->Put(animObj, TO_IINTERFACE(mPendingOnStartAction));
        mPendingOnStartAction = NULL;
    }
    if (mPendingOnEndAction != NULL) {
        mAnimatorOnEndMap->Put(animObj, TO_IINTERFACE(mPendingOnEndAction));
        mPendingOnEndAction = NULL;
    }

    animator->AddUpdateListener(mAnimatorEventListener);
    animatorTmp->AddListener(mAnimatorEventListener);
    if (mStartDelaySet) {
        animatorTmp->SetStartDelay(mStartDelay);
    }
    if (mDurationSet) {
        animatorTmp->SetDuration(mDuration);
    }
    if (mInterpolatorSet) {
        animatorTmp->SetInterpolator(mInterpolator);
    }
    animatorTmp->Start();
    return NOERROR;
}

ECode ViewPropertyAnimator::AnimateProperty(
    /* [in] */ Int32 constantName,
    /* [in] */ Float toValue)
{
    Float fromValue = GetValue(constantName);
    Float deltaValue = toValue - fromValue;
    AnimatePropertyBy(constantName, fromValue, deltaValue);
    return NOERROR;
}

ECode ViewPropertyAnimator::AnimatePropertyBy(
    /* [in] */ Int32 constantName,
    /* [in] */ Float byValue)
{
    Float fromValue = GetValue(constantName);
    AnimatePropertyBy(constantName, fromValue, byValue);
    return NOERROR;
}

ECode ViewPropertyAnimator::AnimatePropertyBy(
    /* [in] */ Int32 constantName,
    /* [in] */ Float startValue,
    /* [in] */ Float byValue)
{
    // First, cancel any existing animations on this property
    Int32 size = 0;
    mAnimatorMap->GetSize(&size);
    if (size > 0) {
        AutoPtr<IAnimator> animatorToCancel = NULL;
        AutoPtr<ISet> keySet;
        mAnimatorMap->GetKeySet((ISet**)&keySet);
        AutoPtr< ArrayOf<IInterface*> > keyArray;
        keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
        Int32 keySize = keyArray->GetLength();
        for (Int32 idx=0; idx<keySize; ++idx) {
            IInterface* interfaceTmp = (*keyArray)[idx];
            AutoPtr<IInterface> valueTmp;
            mAnimatorMap->Get(interfaceTmp, (IInterface**)&valueTmp);
            PropertyBundle* bundle = (PropertyBundle*)IObject::Probe(valueTmp);
            if (bundle->Cancel(constantName)) {
                // property was canceled - cancel the animation if it's now empty
                // Note that it's safe to break out here because every new animation
                // on a property will cancel a previous animation on that property, so
                // there can only ever be one such animation running.
                if (bundle->mPropertyMask == NONE) {
                    // the animation is no longer changing anything - cancel it
                    animatorToCancel = IAnimator::Probe(interfaceTmp);
                    break;
                }
            }
        }
        if (animatorToCancel != NULL) {
            animatorToCancel->Cancel();
        }
    }

    AutoPtr<NameValuesHolder> nameValuePair = new NameValuesHolder(constantName, startValue, byValue);
    mPendingAnimations->PushBack(nameValuePair);

    Boolean resTmp = FALSE;
    mView->RemoveCallbacks(mAnimationStarter, &resTmp);
    mView->PostOnAnimation(mAnimationStarter);
    return NOERROR;
}

ECode ViewPropertyAnimator::SetValue(
    /* [in] */ Int32 propertyConstant,
    /* [in] */ Float value)
{
    AutoPtr<View::TransformationInfo> info = VIEW_PROBE(mView)->mTransformationInfo;
    AutoPtr<IRenderNode> renderNode = ((View*)mView.Get())->mRenderNode;
    switch (propertyConstant) {
        case TRANSLATION_X:
            {
                Boolean resTmp = FALSE;
                renderNode->SetTranslationX(value, &resTmp);
            }
            break;
        case TRANSLATION_Y:
            {
                Boolean resTmp = FALSE;
                renderNode->SetTranslationY(value, &resTmp);
            }
            break;
        case TRANSLATION_Z:
            {
                Boolean resTmp = FALSE;
                renderNode->SetTranslationZ(value, &resTmp);
            }
            break;
        case ROTATION:
            {
                Boolean resTmp = FALSE;
                renderNode->SetRotation(value, &resTmp);
            }
            break;
        case ROTATION_X:
            {
                Boolean resTmp = FALSE;
                renderNode->SetRotationX(value, &resTmp);
            }
            break;
        case ROTATION_Y:
            {
                Boolean resTmp = FALSE;
                renderNode->SetRotationY(value, &resTmp);
            }
            break;
        case SCALE_X:
            {
                Boolean resTmp = FALSE;
                renderNode->SetScaleX(value, &resTmp);
            }
            break;
        case SCALE_Y:
            {
                Boolean resTmp = FALSE;
                renderNode->SetScaleY(value, &resTmp);
            }
            break;
        case _X:
            {
                Boolean resTmp = FALSE;
                Int32 left = 0;
                mView->GetLeft(&left);
                renderNode->SetTranslationX(value - left, &resTmp);
            }
            break;
        case _Y:
            {
                Boolean resTmp = FALSE;
                Int32 top = 0;
                mView->GetTop(&top);
                renderNode->SetTranslationY(value - top, &resTmp);
            }
            break;
        case _Z:
            {
                Boolean resTmp = FALSE;
                Float elevation = 0.0f;
                renderNode->GetElevation(&elevation);
                renderNode->SetTranslationZ(value - elevation, &resTmp);
            }
            break;
        case ALPHA:
            {
                info->mAlpha = value;
                Boolean resTmp = FALSE;
                renderNode->SetAlpha(value, &resTmp);
            }
            break;
    }
    return NOERROR;
}

Float ViewPropertyAnimator::GetValue(
    /* [in] */ Int32 propertyConstant)
{
    AutoPtr<IRenderNode> node = ((View*)mView.Get())->mRenderNode;
    Float result = 0.0f;
    switch (propertyConstant) {
        case TRANSLATION_X:
            node->GetTranslationX(&result);
            break;
        case TRANSLATION_Y:
            node->GetTranslationY(&result);
            break;
        case TRANSLATION_Z:
            node->GetTranslationZ(&result);
            break;
        case ROTATION:
            node->GetRotation(&result);
            break;
        case ROTATION_X:
            node->GetRotationX(&result);
            break;
        case ROTATION_Y:
            node->GetRotationY(&result);
            break;
        case SCALE_X:
            node->GetScaleX(&result);
            break;
        case SCALE_Y:
            node->GetScaleY(&result);
            break;
        case _X:
            {
                Float tmp = 0.0f;
                node->GetTranslationX(&tmp);
                Int32 left = 0;
                mView->GetLeft(&left);
                result = left + tmp;
            }
            break;
        case _Y:
            {
                Float tmp = 0.0f;
                node->GetTranslationY(&tmp);
                Int32 top = 0;
                mView->GetTop(&top);
                result = top + tmp;
            }
            break;
        case _Z:
            {
                Float tmp = 0.0f;
                node->GetElevation(&tmp);
                Float tmp1 = 0.0f;
                node->GetTranslationZ(&tmp1);
                result = tmp + tmp1;
            }
            break;
        case ALPHA:
            result = ((View*)mView.Get())->mTransformationInfo->mAlpha;
            break;
    }
    return result;
}

} // namespace View
} // namespace Droid
} // namespace Elastos

