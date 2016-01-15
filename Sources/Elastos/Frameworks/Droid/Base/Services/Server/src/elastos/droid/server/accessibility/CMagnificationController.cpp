
#include "accessibility/CMagnificationController.h"
#include "accessibility/CMagnificationSpec.h"
#include "accessibility/ScreenMagnifier.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Utility::IPropertyHelper;
using Elastos::Droid::Utility::CPropertyHelper;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::EIID_ITypeEvaluator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

const String CMagnificationController::PROPERTY_NAME_ACCESSIBILITY_TRANSFORMATION("accessibilityTransformation");
const Boolean CMagnificationController::DEBUG = TRUE;

CAR_INTERFACE_IMPL(CMagnificationController::MagnificationSpecTypeEvaluator, ITypeEvaluator)

CMagnificationController::MagnificationSpecTypeEvaluator::MagnificationSpecTypeEvaluator(
    /* [in] */ ScreenMagnifier* screenMagnifier)
{
    CMagnificationSpec::New((Handle32)screenMagnifier, (IMagnificationSpec**)&mTempTransformationSpec);
}

ECode CMagnificationController::MagnificationSpecTypeEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* fromSpecObj,
    /* [in] */ IInterface* toSpecObj,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(fromSpecObj);
    VALIDATE_NOT_NULL(toSpecObj);

    AutoPtr<IMagnificationSpec> from = IMagnificationSpec::Probe(fromSpecObj);
    AutoPtr<IMagnificationSpec> to = IMagnificationSpec::Probe(toSpecObj);
    CMagnificationSpec* fromSpec = (CMagnificationSpec*)from.Get();
    CMagnificationSpec* toSpec = (CMagnificationSpec*)to.Get();

    AutoPtr<IMagnificationSpec> temp = mTempTransformationSpec;
    temp->SetScale(fromSpec->mScale
        + (toSpec->mScale - fromSpec->mScale) * fraction);
    temp->SetMagnifiedRegionCenterX(fromSpec->mMagnifiedRegionCenterX
        + (toSpec->mMagnifiedRegionCenterX - fromSpec->mMagnifiedRegionCenterX)
        * fraction);
    temp->SetMagnifiedRegionCenterY(fromSpec->mMagnifiedRegionCenterY
        + (toSpec->mMagnifiedRegionCenterY - fromSpec->mMagnifiedRegionCenterY)
        * fraction);
    temp->SetScaledOffsetX(fromSpec->mScaledOffsetX
        + (toSpec->mScaledOffsetX - fromSpec->mScaledOffsetX)
        * fraction);
    temp->SetScaledOffsetY(fromSpec->mScaledOffsetY
        + (toSpec->mScaledOffsetY - fromSpec->mScaledOffsetY)
        * fraction);

    *result =  temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMagnificationController::constructor(
    /* [in] */ Handle32 screenMagnifier,
    /* [in] */ Int32 animationDuration)
{
    mHost = (ScreenMagnifier*)screenMagnifier;

    CMagnificationSpec::New(screenMagnifier, (IMagnificationSpec**)&mSentMagnificationSpec);
    CMagnificationSpec::New(screenMagnifier, (IMagnificationSpec**)&mCurrentMagnificationSpec);
    CRect::New((IRect**)&mTempRect);

    AutoPtr<IPropertyHelper> helper;
    CPropertyHelper::AcquireSingleton((IPropertyHelper**)&helper);
    AutoPtr<IProperty> property;
    helper->Of(ECLSID_CMagnificationControllerClassObject,
        CarDataType_Interface,
        PROPERTY_NAME_ACCESSIBILITY_TRANSFORMATION, (IProperty**)&property);

    AutoPtr<IObjectAnimatorHelper> objAnimhelper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&objAnimhelper);

    AutoPtr<ArrayOf<IInterface*> > objArrays = ArrayOf<IInterface*>::Alloc(2);
    objArrays->Set(0, mSentMagnificationSpec->Probe(EIID_IInterface));
    objArrays->Set(1, mCurrentMagnificationSpec->Probe(EIID_IInterface));

    AutoPtr<IObjectAnimator> objAnim;
    AutoPtr<ITypeEvaluator> evaluator = new MagnificationSpecTypeEvaluator(mHost);
    objAnimhelper->OfObject(Probe(EIID_IInterface), property, evaluator,
        objArrays, (IObjectAnimator**)&objAnim);
    mTransformationAnimator = IValueAnimator::Probe(objAnim);

    Int64 duration = animationDuration;
    mTransformationAnimator->SetDuration(duration);
    mTransformationAnimator->SetInterpolator(mHost->mInterpolator);
    return NOERROR;
}

ECode CMagnificationController::IsMagnifying(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Float scale;
    mCurrentMagnificationSpec->GetScale(&scale);
    *result = scale > 1.0f;
    return NOERROR;
}

ECode CMagnificationController::Reset(
    /* [in] */ Boolean animate)
{
    Boolean isRunning;
    mTransformationAnimator->IsRunning(&isRunning);

    if (isRunning) {
        mTransformationAnimator->Cancel();
    }

    mCurrentMagnificationSpec->Reset();
    if (animate) {
        AnimateAccessibilityTranformation(mSentMagnificationSpec, mCurrentMagnificationSpec);
    }
    else {
        SetAccessibilityTransformation(mCurrentMagnificationSpec);
    }
    return NOERROR;
}

ECode CMagnificationController::GetMagnifiedRegionBounds(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    mTempRect->Set(mHost->mViewport->GetBounds());
    Float sx, sy, scale;
    mCurrentMagnificationSpec->GetScaledOffsetX(&sx);
    mCurrentMagnificationSpec->GetScaledOffsetY(&sy);
    mCurrentMagnificationSpec->GetScale(&scale);
    Int32 offsetX = -sx;
    Int32 offsetY = -sy;
    mTempRect->Offset(offsetX, offsetY);

    mTempRect->Scale(1.0f / scale);
    *rect = mTempRect;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode CMagnificationController::GetScale(
    /* [out] */ Float* value)
{
    return mCurrentMagnificationSpec->GetScale(value);
}

ECode CMagnificationController::GetMagnifiedRegionCenterX(
    /* [out] */ Float* value)
{
    return mCurrentMagnificationSpec->GetMagnifiedRegionCenterX(value);
}

ECode CMagnificationController::GetMagnifiedRegionCenterY(
    /* [out] */ Float* value)
{
    return mCurrentMagnificationSpec->GetMagnifiedRegionCenterY(value);
}

ECode CMagnificationController::GetScaledOffsetX(
    /* [out] */ Float* value)
{
    return mCurrentMagnificationSpec->GetScaledOffsetX(value);
}

ECode CMagnificationController::GetScaledOffsetY(
    /* [out] */ Float* value)
{
    return mCurrentMagnificationSpec->GetScaledOffsetY(value);
}

ECode CMagnificationController::SetScale(
    /* [in] */ Float scale,
    /* [in] */ Float pivotX,
    /* [in] */ Float pivotY,
    /* [in] */ Boolean animate)
{
    AutoPtr<CMagnificationSpec> spec = (CMagnificationSpec*)mCurrentMagnificationSpec.Get();
    Float oldScale = spec->mScale;
    Float oldCenterX = spec->mMagnifiedRegionCenterX;
    Float oldCenterY = spec->mMagnifiedRegionCenterY;
    Float normPivotX = (-spec->mScaledOffsetX + pivotX) / oldScale;
    Float normPivotY = (-spec->mScaledOffsetY + pivotY) / oldScale;
    Float offsetX = (oldCenterX - normPivotX) * (oldScale / scale);
    Float offsetY = (oldCenterY - normPivotY) * (oldScale / scale);
    Float centerX = normPivotX + offsetX;
    Float centerY = normPivotY + offsetY;
    SetScaleAndMagnifiedRegionCenter(scale, centerX, centerY, animate);
    return NOERROR;
}

ECode CMagnificationController::SetMagnifiedRegionCenter(
    /* [in] */ Float centerX,
    /* [in] */ Float centerY,
    /* [in] */ Boolean animate)
{
    Float scale;
    mCurrentMagnificationSpec->GetScale(&scale);
    return SetScaleAndMagnifiedRegionCenter(scale, centerX, centerY, animate);
}

ECode CMagnificationController::SetScaleAndMagnifiedRegionCenter(
    /* [in] */ Float scale,
    /* [in] */ Float centerX,
    /* [in] */ Float centerY,
    /* [in] */ Boolean animate)
{
    Float s, cx, cy;
    mCurrentMagnificationSpec->GetMagnifiedRegionCenterX(&cx);
    mCurrentMagnificationSpec->GetMagnifiedRegionCenterY(&cy);
    mCurrentMagnificationSpec->GetScale(&s);
    if (s == scale && cx == centerX && cy == centerY) {
        return NOERROR;
    }

    Boolean isRunning;
    if (mTransformationAnimator->IsRunning(&isRunning), isRunning) {
        mTransformationAnimator->Cancel();
    }

    if (DEBUG) {
        Slogger::I(String("CMagnificationController"), "scale: %f centerX: %f centerY: %f",
            scale, centerX, centerY);
    }

    mCurrentMagnificationSpec->Initialize(scale, centerX, centerY);
    if (animate) {
        AnimateAccessibilityTranformation(mSentMagnificationSpec, mCurrentMagnificationSpec);
    }
    else {
        SetAccessibilityTransformation(mCurrentMagnificationSpec);
    }
    return NOERROR;
}

ECode CMagnificationController::AnimateAccessibilityTranformation(
    /* [in] */ IMagnificationSpec* fromSpec,
    /* [in] */ IMagnificationSpec* toSpec)
{
    AutoPtr<ArrayOf<IInterface*> > objArrays = ArrayOf<IInterface*>::Alloc(2);
    objArrays->Set(0, fromSpec->Probe(EIID_IInterface));
    objArrays->Set(1, toSpec->Probe(EIID_IInterface));
    mTransformationAnimator->SetObjectValues(objArrays);
    mTransformationAnimator->Start();
    return NOERROR;
}

ECode CMagnificationController::GetAccessibilityTransformation(
    /* [out] */ IMagnificationSpec** spec)
{
    VALIDATE_NOT_NULL(spec);
    *spec = mSentMagnificationSpec;
    REFCOUNT_ADD(*spec);
    return NOERROR;
}

ECode CMagnificationController::SetAccessibilityTransformation(
    /* [in] */ IMagnificationSpec* transformation)
{
    // try {
    Float scale, sx, sy;
    mCurrentMagnificationSpec->GetScaledOffsetX(&sx);
    mCurrentMagnificationSpec->GetScaledOffsetY(&sy);
    mCurrentMagnificationSpec->GetScale(&scale);

    if (DEBUG) {
        Slogger::I(String("CMagnificationController"), "Transformation scale: %f offsetX: %f offsetY: %f",
            scale, sx, sy);
    }

    mSentMagnificationSpec->UpdateFrom(transformation);
    Int32 displayId;
    mHost->mDisplayProvider->GetDisplay()->GetDisplayId(&displayId);
    mHost->mWindowManagerService->MagnifyDisplay(displayId, scale, sx, sy);
    // } catch (RemoteException re) {
    //     /* ignore */
    // }
    return NOERROR;
}

} // Accessibility
} // Server
} // Droid
} // Elastos
