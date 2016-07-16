
#include "elastos/droid/systemui/keyguard/CKeyguardPINView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardPINView::MyRunnable::Run()
{
    return mHost->EnableClipping(TRUE);
}

CAR_OBJECT_IMPL(CKeyguardPINView)

CKeyguardPINView::CKeyguardPINView()
    : mDisappearYTranslation(0)
{
}

ECode CKeyguardPINView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardPINView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    KeyguardPinBasedInputView::constructor(context, attrs);
    mAppearAnimationUtils = new AppearAnimationUtils(context);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    return resources->GetDimensionPixelSize(R::dimen::disappear_y_translation, &mDisappearYTranslation);
}

ECode CKeyguardPINView::ResetState()
{
    KeyguardPinBasedInputView::ResetState();
    Boolean res;
    if (KeyguardUpdateMonitor::GetInstance(mContext)->GetMaxBiometricUnlockAttemptsReached(&res), res) {
        mSecurityMessageDisplay->SetMessage(R::string::faceunlock_multiple_failures, TRUE);
    }
    else {
        mSecurityMessageDisplay->SetMessage(R::string::kg_pin_instructions, FALSE);
    }
    return NOERROR;
}

ECode CKeyguardPINView::GetPasswordTextViewId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = R::id::pinEntry;
    return NOERROR;
}

ECode ECode CKeyguardPINView::::OnFinishInflate()
{
    KeyguardPinBasedInputView::OnFinishInflate();

    AutoPtr<IView> view;
    FindViewById(R::id::keyguard_bouncer_frame, (IView**)&view);
    mKeyguardBouncerFrame = IViewGroup::Probe(view);

    AutoPtr<IView> view0;
    FindViewById(R::id::row0, (IView**)&view0);
    mRow0 = IViewGroup::Probe(view0);

    AutoPtr<IView> view1;
    FindViewById(R::id::row1, (IView**)&view1);
    mRow1 = IViewGroup::Probe(view1);

    AutoPtr<IView> view2;
    FindViewById(R::id::row2, (IView**)&view2);
    mRow2 = IViewGroup::Probe(view2);

    AutoPtr<IView> view3;
    FindViewById(R::id::row3, (IView**)&view3);
    mRow3 = IViewGroup::Probe(view3);

    return FindViewById(R::id::divider, (IView**)&mDivider);
}

ECode CKeyguardPINView::ShowUsabilityHint()
{
    return NOERROR;
}

ECode CKeyguardPINView::GetWrongPasswordStringId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = R::string::kg_wrong_pin;
    return NOERROR;
}

ECode CKeyguardPINView::StartAppearAnimation()
{
    EnableClipping(FALSE);
    SetAlpha(1f);
    Float value;
    mAppearAnimationUtils->GetStartTranslation(&value);
    SetTranslationY(value);

    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);
    animator->SetDuration(500);
    AutoPtr<IInterpolator> interpolator;
    mAppearAnimationUtils->GetInterpolator((IInterpolator**)&interpolator);
    animator->SetInterpolator(interpolator);
    animator->TranslationY(0);

    AutoPtr<ArrayOf<ArrayOf<IView*>* > > array = ArrayOf<ArrayOf<IView*> >::Allco(6);

    AutoPtr<ArrayOf<IView*> > array0 = ArrayOf<IView*>::Alloc(3);
    array0->Set(0, mRow0);
    array0->Set(1, NULL);
    array0->Set(2, NULL);
    array->Set(0, array0);

    AutoPtr<ArrayOf<IView*> > array1 = ArrayOf<IView*>::Alloc(3);
    AutoPtr<IView> view1;
    FindViewById(R::id::key1, (IView**)&view1);
    AutoPtr<IView> view2;
    FindViewById(R::id::key2, (IView**)&view2);
    AutoPtr<IView> view3;
    FindViewById(R::id::key3, (IView**)&view3);
    array1->Set(0, view1);
    array1->Set(1, view2);
    array1->Set(2, view3);
    array->Set(1, array1);

    AutoPtr<ArrayOf<IView*> > array2 = ArrayOf<IView*>::Alloc(3);
    AutoPtr<IView> view4;
    FindViewById(R::id::key4, (IView**)&view4);
    AutoPtr<IView> view5;
    FindViewById(R::id::key5, (IView**)&view5);
    AutoPtr<IView> view6;
    FindViewById(R::id::key6, (IView**)&view6);
    array1->Set(0, view4);
    array1->Set(1, view5);
    array1->Set(2, view6);
    array->Set(2, array2);

    AutoPtr<ArrayOf<IView*> > array3 = ArrayOf<IView*>::Alloc(3);
    AutoPtr<IView> view7;
    FindViewById(R::id::key7, (IView**)&view7);
    AutoPtr<IView> view8;
    FindViewById(R::id::key8, (IView**)&view8);
    AutoPtr<IView> view9;
    FindViewById(R::id::key9, (IView**)&view9);
    array1->Set(0, view7);
    array1->Set(1, view8);
    array1->Set(2, view9);
    array->Set(3, array3);

    AutoPtr<ArrayOf<IView*> > array4 = ArrayOf<IView*>::Alloc(3);
    AutoPtr<IView> view10;
    FindViewById(R::id::key0, (IView**)&view10);
    AutoPtr<IView> view11;
    FindViewById(R::id::key_enter, (IView**)&view11);
    array1->Set(0, NULL);
    array1->Set(1, view10);
    array1->Set(2, view11);
    array->Set(4, array4);

    AutoPtr<ArrayOf<IView*> > array5 = ArrayOf<IView*>::Alloc(3);
    array1->Set(0, NULL);
    array1->Set(1, mEcaView);
    array1->Set(2, NULL);
    array->Set(5, array5);

    AutoPtr<IRunnable> r = new MyRunnable(this);

    mAppearAnimationUtils->StartAppearAnimation(array, r);
}

ECode CKeyguardPINView::StartDisappearAnimation(
    /* [in] */ IRunnable* finishRunnable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(0f);
    animator->TranslationY(mDisappearYTranslation);

    AutoPtr<IAnimationUtils> helper;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&helper);
    AutoPtr<IInterpolator> interpolator;
    helper->LoadInterpolator(mContext,
            Elastos::Droid::R::interpolator::fast_out_linear_in, (IInterpolator**)&interpolator);
    animator->SetInterpolator(interpolator);
    animator->SetDuration(100);
    animator->WithEndAction(finishRunnable);
    *result = TRUE;
    return NOERROR;
}

void CKeyguardPINView::EnableClipping(
    /* [in] */ Boolean enable)
{
    mKeyguardBouncerFrame->SetClipToPadding(enable);
    mKeyguardBouncerFrame->SetClipChildren(enable);
    mRow1->SetClipToPadding(enable);
    mRow2->SetClipToPadding(enable);
    mRow3->SetClipToPadding(enable);
    SetClipChildren(enable);
}

ECode CKeyguardPINView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
