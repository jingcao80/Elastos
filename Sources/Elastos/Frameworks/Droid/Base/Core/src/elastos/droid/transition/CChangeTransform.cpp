
#include "Elastos.Droid.Content.h"
#include "elastos/droid/transition/CChangeTransform.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/transition/CTransitionUtils.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/view/GhostView.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorPauseListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::ECLSID_CMatrix;
using Elastos::Droid::Utility::EIID_IProperty;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::GhostView;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Transition {

const String CChangeTransform::TAG("ChangeTransform");

const String CChangeTransform::PROPNAME_MATRIX("android:changeTransform:matrix");
const String CChangeTransform::PROPNAME_TRANSFORMS("android:changeTransform:transforms");
const String CChangeTransform::PROPNAME_PARENT("android:changeTransform:parent");
const String CChangeTransform::PROPNAME_PARENT_MATRIX("android:changeTransform:parentMatrix");
const String CChangeTransform::PROPNAME_INTERMEDIATE_PARENT_MATRIX("android:changeTransform:intermediateParentMatrix");
const String CChangeTransform::PROPNAME_INTERMEDIATE_MATRIX("android:changeTransform:intermediateMatrix");

AutoPtr<ArrayOf<String> > CChangeTransform::sTransitionProperties = ArrayOf<String>::Alloc(3);

AutoPtr<IProperty> CChangeTransform::ANIMATION_MATRIX_PROPERTY = new MatrixProperty(String("animationMatrix"));

//===============================================================
// CChangeTransform::
//===============================================================
CAR_OBJECT_IMPL(CChangeTransform)

CAR_INTERFACE_IMPL(CChangeTransform, Transition, IChangeTransform)

CChangeTransform::CChangeTransform()
{
    (*sTransitionProperties)[0] = PROPNAME_MATRIX;
    (*sTransitionProperties)[1] = PROPNAME_TRANSFORMS;
    (*sTransitionProperties)[2] = PROPNAME_PARENT_MATRIX;
}

ECode CChangeTransform::constructor()
{
    return NOERROR;
}

ECode CChangeTransform::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Transition::constructor(context, attrs);
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ChangeTransform);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetBoolean(R::styleable::ChangeTransform_reparentWithOverlay, TRUE, &mUseOverlay);
    a->GetBoolean(R::styleable::ChangeTransform_reparent, TRUE, &mReparent);
    a->Recycle();
    return NOERROR;
}

ECode CChangeTransform::GetReparentWithOverlay(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUseOverlay;
    return NOERROR;
}

ECode CChangeTransform::SetReparentWithOverlay(
    /* [in] */ Boolean reparentWithOverlay)
{
    mUseOverlay = reparentWithOverlay;
    return NOERROR;
}

ECode CChangeTransform::GetReparent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReparent;
    return NOERROR;
}

ECode CChangeTransform::SetReparent(
    /* [in] */ Boolean reparent)
{
    mReparent = reparent;
    return NOERROR;
}

ECode CChangeTransform::GetTransitionProperties(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = sTransitionProperties;
    return NOERROR;
}

void CChangeTransform::CaptureValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ct = (CTransitionValues*)transitionValues;
    AutoPtr<IView> view = ct->mView;
    Int32 v = 0;
    view->GetVisibility(&v);
    if (v == IView::GONE) {
        return;
    }
    AutoPtr<ICharSequence> pro_parent;
    CString::New(PROPNAME_PARENT, (ICharSequence**)&pro_parent);
    AutoPtr<IViewParent> vp;
    view->GetParent((IViewParent**)&vp);
    ct->mValues->Put(pro_parent, vp);
    AutoPtr<Transforms> transforms = new Transforms(view);
    AutoPtr<ICharSequence> pro_trans;
    CString::New(PROPNAME_TRANSFORMS, (ICharSequence**)&pro_trans);
    ct->mValues->Put(pro_trans, transforms->Probe(EIID_IInterface));
    AutoPtr<IMatrix> matrix;
    view->GetMatrix((IMatrix**)&matrix);
    Boolean bIsIdentity = FALSE;
    if (matrix == NULL || (matrix->IsIdentity(&bIsIdentity), bIsIdentity)) {
        matrix = NULL;
    }
    else {
        CMatrix::New(matrix, (IMatrix**)&matrix);
    }
    AutoPtr<ICharSequence> pro_matrix;
    CString::New(PROPNAME_MATRIX, (ICharSequence**)&pro_matrix);
    ct->mValues->Put(pro_matrix, matrix);
    if (mReparent) {
        AutoPtr<IMatrix> parentMatrix;
        CMatrix::New((IMatrix**)&parentMatrix);
        AutoPtr<IViewParent> p;
        view->GetParent((IViewParent**)&p);
        AutoPtr<IViewGroup> parent = IViewGroup::Probe(p);
        IView::Probe(parent)->TransformMatrixToGlobal(parentMatrix);
        Int32 x = 0, y = 0;
        IView::Probe(parent)->GetScrollX(&x);
        IView::Probe(parent)->GetScrollY(&y);
        Boolean res;
        parentMatrix->PreTranslate(-x, -y, &res);
        AutoPtr<ICharSequence> pro_parent_matrix;
        CString::New(PROPNAME_PARENT_MATRIX, (ICharSequence**)&pro_parent_matrix);
        ct->mValues->Put(pro_parent_matrix, parentMatrix);
        AutoPtr<IInterface> transTag;
        view->GetTag(R::id::transitionTransform, (IInterface**)&transTag);
        AutoPtr<ICharSequence> pMatrix;
        CString::New(PROPNAME_INTERMEDIATE_MATRIX, (ICharSequence**)&pMatrix);
        ct->mValues->Put(pMatrix, transTag);
        AutoPtr<IInterface> matTag;
        view->GetTag(R::id::parentMatrix, (IInterface**)&matTag);
        AutoPtr<ICharSequence> pPartMatrix;
        CString::New(PROPNAME_INTERMEDIATE_PARENT_MATRIX, (ICharSequence**)&pPartMatrix);
        ct->mValues->Put(pPartMatrix, matTag);
    }
    return;
}

ECode CChangeTransform::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CChangeTransform::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CChangeTransform::CreateAnimator(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)
    if (startValues == NULL || endValues == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<CTransitionValues> cStart = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> cEnd = (CTransitionValues*)endValues;
    AutoPtr<ICharSequence> pro_parent;
    CString::New(PROPNAME_PARENT, (ICharSequence**)&pro_parent);
    Boolean bStart = FALSE, bEnd = FALSE;
    if (!(cStart->mValues->ContainsKey(pro_parent, &bStart), bStart) ||
            !(cEnd->mValues->ContainsKey(pro_parent, &bEnd), bEnd)) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IInterface> sP, eP;
    cStart->mValues->Get(pro_parent, (IInterface**)&sP);
    cEnd->mValues->Get(pro_parent, (IInterface**)&eP);
    AutoPtr<IViewGroup> startParent = IViewGroup::Probe(sP);
    AutoPtr<IViewGroup> endParent = IViewGroup::Probe(eP);
    Boolean handleParentChange = mReparent && !ParentsMatch(startParent, endParent);

    AutoPtr<ICharSequence> pro_matrix;
    CString::New(PROPNAME_INTERMEDIATE_MATRIX, (ICharSequence**)&pro_matrix);
    AutoPtr<IInterface> sM;
    cStart->mValues->Get(pro_matrix, (IInterface**)&sM);
    AutoPtr<IMatrix> startMatrix = IMatrix::Probe(sM);
    if (startMatrix != NULL) {
        AutoPtr<ICharSequence> pro_matrix;
        CString::New(PROPNAME_MATRIX, (ICharSequence**)&pro_matrix);
        cStart->mValues->Put(pro_matrix, startMatrix);
    }

    AutoPtr<ICharSequence> pro_parent_matrix;
    CString::New(PROPNAME_INTERMEDIATE_PARENT_MATRIX, (ICharSequence**)&pro_parent_matrix);
    AutoPtr<IInterface> sPM;
    cStart->mValues->Get(pro_parent_matrix, (IInterface**)&sPM);
    AutoPtr<IMatrix> startParentMatrix = IMatrix::Probe(sPM);

    if (startParentMatrix != NULL) {
        AutoPtr<ICharSequence> pro_p_matrix;
        CString::New(PROPNAME_PARENT_MATRIX, (ICharSequence**)&pro_p_matrix);
        cStart->mValues->Put(pro_p_matrix, startParentMatrix);
    }

    // First handle the parent change:
    if (handleParentChange) {
        SetMatricesForParent(startValues, endValues);
    }

    // Next handle the normal matrix transform:
    AutoPtr<IObjectAnimator> transformAnimator = CreateTransformAnimator(startValues, endValues,
            handleParentChange);

    if (handleParentChange && transformAnimator != NULL && mUseOverlay) {
        CreateGhostView(sceneRoot, startValues, endValues);
    }

    *result = IAnimator::Probe(transformAnimator);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IObjectAnimator> CChangeTransform::CreateTransformAnimator(
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [in] */ Boolean handleParentChange)
{
    AutoPtr<CTransitionValues> cStart = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> cEnd = (CTransitionValues*)endValues;
    AutoPtr<ICharSequence> pro_matrix;
    CString::New(PROPNAME_MATRIX, (ICharSequence**)&pro_matrix);
    AutoPtr<IInterface> sM, eM;
    cStart->mValues->Get(pro_matrix, (IInterface**)&sM);
    cEnd->mValues->Get(pro_matrix, (IInterface**)&eM);
    AutoPtr<IMatrix> startMatrix = IMatrix::Probe(sM);
    AutoPtr<IMatrix> endMatrix = IMatrix::Probe(eM);

    if (startMatrix == NULL) {
        startMatrix = CMatrix::IDENTITY_MATRIX;
    }

    if (endMatrix == NULL) {
        endMatrix = CMatrix::IDENTITY_MATRIX;
    }

    if (Object::Equals(startMatrix->Probe(EIID_IInterface), endMatrix->Probe(EIID_IInterface))) {
        return NULL;
    }

    AutoPtr<ICharSequence> pro_trans;
    CString::New(PROPNAME_TRANSFORMS, (ICharSequence**)&pro_trans);
    AutoPtr<IInterface> t;
    cEnd->mValues->Get(pro_trans, (IInterface**)&t);
    AutoPtr<Transforms> transforms = (Transforms*)(IObject*)t.Get();

    // clear the transform properties so that we can use the animation matrix instead
    AutoPtr<IView> view = cEnd->mView;
    SetIdentityTransforms(view);

    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(2);
    (*arr)[0] = startMatrix;
    (*arr)[1] = endMatrix;
    AutoPtr<IObjectAnimator> animator = ObjectAnimator::OfObject(view, ANIMATION_MATRIX_PROPERTY,
            new CTransitionUtils::MatrixEvaluator(), arr);

    AutoPtr<IMatrix> finalEndMatrix = endMatrix;

    AutoPtr<AnimatorListenerAdapter_1> p = new AnimatorListenerAdapter_1(handleParentChange,
                                                                                mUseOverlay,
                                                                                finalEndMatrix,
                                                                                view,
                                                                                transforms,
                                                                                this);
    AutoPtr<IAnimatorListener> listener = IAnimatorListener::Probe(p);

    IAnimator::Probe(animator)->AddListener(listener);
    IAnimator::Probe(animator)->AddPauseListener(IAnimatorPauseListener::Probe(listener));
    return animator;
}

Boolean CChangeTransform::ParentsMatch(
    /* [in] */ IViewGroup* startParent,
    /* [in] */ IViewGroup* endParent)
{
    Boolean parentsMatch = FALSE;
    Boolean bIsStart = FALSE, bIsEnd = FALSE;
    if (!(IsValidTarget(IView::Probe(startParent), &bIsStart), bIsStart) ||
         !(IsValidTarget(IView::Probe(endParent), &bIsEnd), bIsEnd)) {
        parentsMatch = startParent == endParent;
    }
    else {
        AutoPtr<ITransitionValues> endValues;
        GetMatchedTransitionValues(IView::Probe(startParent), TRUE, (ITransitionValues**)&endValues);
        if (endValues != NULL) {
            AutoPtr<CTransitionValues> cEnd = (CTransitionValues*)endValues.Get();
            parentsMatch = Object::Equals(endParent->Probe(EIID_IInterface), cEnd->mView->Probe(EIID_IInterface));
        }
    }
    return parentsMatch;
}

void CChangeTransform::CreateGhostView(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues)
{
    AutoPtr<CTransitionValues> cStart = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> cEnd = (CTransitionValues*)endValues;
    AutoPtr<IView> view = cEnd->mView;

    AutoPtr<ICharSequence> pro_p_matrix;
    CString::New(PROPNAME_PARENT_MATRIX, (ICharSequence**)&pro_p_matrix);
    AutoPtr<IInterface> eM;
    cEnd->mValues->Get(pro_p_matrix, (IInterface**)&eM);
    AutoPtr<IMatrix> endMatrix = IMatrix::Probe(eM);
    AutoPtr<IMatrix> localEndMatrix;
    CMatrix::New(endMatrix, (IMatrix**)&localEndMatrix);
    IView::Probe(sceneRoot)->TransformMatrixToLocal(localEndMatrix);

    AutoPtr<IGhostView> ghostView;
    GhostView::AddGhost(view, sceneRoot, localEndMatrix, (IGhostView**)&ghostView);

    AutoPtr<ITransition> outerTransition = this;
    AutoPtr<Transition> cOuterTrans = (Transition*)outerTransition.Get();
    while (cOuterTrans->mParent != NULL) {
        cOuterTrans = (Transition*)ITransition::Probe(cOuterTrans->mParent);
    }
    AutoPtr<GhostListener> listener = new GhostListener(view, ghostView, endMatrix);
    outerTransition->AddListener(listener);

    if (!Object::Equals(cStart->mView->Probe(EIID_IInterface), cEnd->mView->Probe(EIID_IInterface))) {
        cStart->mView->SetTransitionAlpha(0);
    }
    view->SetTransitionAlpha(1);
}

void CChangeTransform::SetMatricesForParent(
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues)
{
    AutoPtr<CTransitionValues> cEnd = (CTransitionValues*)endValues;
    AutoPtr<ICharSequence> pro_parent_matrix;
    CString::New(PROPNAME_PARENT_MATRIX, (ICharSequence**)&pro_parent_matrix);
    AutoPtr<IInterface> eM;
    cEnd->mValues->Get(pro_parent_matrix, (IInterface**)&eM);
    AutoPtr<IMatrix> endParentMatrix = IMatrix::Probe(eM);
    cEnd->mView->SetTagInternal(R::id::parentMatrix, endParentMatrix);

    AutoPtr<IMatrix> toLocal = mTempMatrix;
    toLocal->Reset();
    Boolean res;
    endParentMatrix->Invert(toLocal, &res);

    AutoPtr<CTransitionValues> cStart = (CTransitionValues*)startValues;
    AutoPtr<ICharSequence> pro_matrix;
    CString::New(PROPNAME_MATRIX, (ICharSequence**)&pro_matrix);
    AutoPtr<IInterface> sL;
    cStart->mValues->Get(pro_matrix, (IInterface**)&sL);
    AutoPtr<IMatrix> startLocal = IMatrix::Probe(sL);
    if (startLocal == NULL) {
        CMatrix::New((IMatrix**)&startLocal);
        cStart->mValues->Put(pro_matrix, startLocal);
    }

    AutoPtr<IInterface> sP;
    cStart->mValues->Get(pro_parent_matrix, (IInterface**)&sP);
    AutoPtr<IMatrix> startParentMatrix = IMatrix::Probe(sP);
    startLocal->PostConcat(startParentMatrix, &res);
    startLocal->PostConcat(toLocal, &res);
}

void CChangeTransform::SetIdentityTransforms(
    /* [in] */ IView* view)
{
    SetTransforms(view, 0, 0, 0, 1, 1, 0, 0, 0);
}

void CChangeTransform::SetTransforms(
    /* [in] */ IView* view,
    /* [in] */ Float translationX,
    /* [in] */ Float translationY,
    /* [in] */ Float translationZ,
    /* [in] */ Float scaleX,
    /* [in] */ Float scaleY,
    /* [in] */ Float rotationX,
    /* [in] */ Float rotationY,
    /* [in] */ Float rotationZ)
{
    view->SetTranslationX(translationX);
    view->SetTranslationY(translationY);
    view->SetTranslationZ(translationZ);
    view->SetScaleX(scaleX);
    view->SetScaleY(scaleY);
    view->SetRotationX(rotationX);
    view->SetRotationY(rotationY);
    view->SetRotation(rotationZ);
}

ECode CChangeTransform::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<IChangeTransform> changes;
    CChangeTransform::New((IChangeTransform**)&changes);

    CloneImpl(changes);
    *obj = changes;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CChangeTransform::CloneImpl(
    /* [in] */ IChangeTransform* obj)
{
    VALIDATE_NOT_NULL(obj);

    return Transition::CloneImpl(ITransition::Probe(obj));
}

//===============================================================
// CChangeTransform::Transforms::
//===============================================================

CChangeTransform::Transforms::Transforms(
    /* [in] */ IView* view)
{
    view->GetTranslationX(&mTranslationX);
    view->GetTranslationY(&mTranslationY);
    view->GetTranslationZ(&mTranslationZ);
    view->GetScaleX(&mScaleX);
    view->GetScaleY(&mScaleY);
    view->GetRotationX(&mRotationX);
    view->GetRotationY(&mRotationY);
    view->GetRotation(&mRotationZ);
}

void CChangeTransform::Transforms::Restore(
    /* [in] */ IView* view)
{
    SetTransforms(view, mTranslationX, mTranslationY, mTranslationZ, mScaleX, mScaleY,
            mRotationX, mRotationY, mRotationZ);
}

Boolean CChangeTransform::Transforms::Equals(
    /* [in] */ IInterface* that)
{
    assert(0 && "TODO");
    // if (!(ITransforms::Probe(that) != NULL)) {
    //     return FALSE;
    // }
    AutoPtr<Transforms> thatTransform = (Transforms*)(IObject*)that;
    return thatTransform->mTranslationX == mTranslationX &&
            thatTransform->mTranslationY == mTranslationY &&
            thatTransform->mTranslationZ == mTranslationZ &&
            thatTransform->mScaleX == mScaleX &&
            thatTransform->mScaleY == mScaleY &&
            thatTransform->mRotationX == mRotationX &&
            thatTransform->mRotationY == mRotationY &&
            thatTransform->mRotationZ == mRotationZ;
}

//===============================================================
// CChangeTransform::GhostListener::
//===============================================================
CChangeTransform::GhostListener::GhostListener(
    /* [in] */ IView* view,
    /* [in] */ IGhostView* ghostView,
    /* [in] */ IMatrix* endMatrix)
{
    mView = view;
    mGhostView = ghostView;
    mEndMatrix = endMatrix;
}

ECode CChangeTransform::GhostListener::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    transition->RemoveListener(this);
    GhostView::RemoveGhost(mView);
    mView->SetTagInternal(R::id::transitionTransform, NULL);
    mView->SetTagInternal(R::id::parentMatrix, NULL);
    return NOERROR;
}

ECode CChangeTransform::GhostListener::OnTransitionPause(
    /* [in] */ ITransition* transition)
{
    IView::Probe(mGhostView)->SetVisibility(IView::INVISIBLE);
    return NOERROR;
}

ECode CChangeTransform::GhostListener::OnTransitionResume(
    /* [in] */ ITransition* transition)
{
    IView::Probe(mGhostView)->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

//===============================================================
// CChangeTransform::AnimatorListenerAdapter_1::
//===============================================================

CChangeTransform::AnimatorListenerAdapter_1::AnimatorListenerAdapter_1(
    /* [in] */ Boolean handleParentChange,
    /* [in] */ Boolean useOverlay,
    /* [in] */ IMatrix* finalEndMatrix,
    /* [in] */ IView* v,
    /* [in] */ Transforms* trans,
    /* [in] */ CChangeTransform* owner)
{
    mHandleParentChange = handleParentChange;
    mUseOverlay = useOverlay;
    mFinalEndMatrix = finalEndMatrix;
    mView = v;
    mTrans = trans;
    mOwner = owner;
}

ECode CChangeTransform::AnimatorListenerAdapter_1::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mIsCanceled = TRUE;
    return NOERROR;
}

ECode CChangeTransform::AnimatorListenerAdapter_1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mIsCanceled) {
        if (mHandleParentChange && mUseOverlay) {
            SetCurrentMatrix(mFinalEndMatrix);
        }
        else {
            mView->SetTagInternal(R::id::transitionTransform, NULL);
            mView->SetTagInternal(R::id::parentMatrix, NULL);
        }
    }
    CChangeTransform::ANIMATION_MATRIX_PROPERTY->Set(mView, NULL);
    mTrans->Restore(mView);
    return NOERROR;
}

ECode CChangeTransform::AnimatorListenerAdapter_1::OnAnimationPause(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IValueAnimator> animator = IValueAnimator::Probe(animation);
    AutoPtr<IInterface> v;
    animator->GetAnimatedValue((IInterface**)&v);
    AutoPtr<IMatrix> currentMatrix = IMatrix::Probe(v);
    SetCurrentMatrix(currentMatrix);
    return NOERROR;
}

ECode CChangeTransform::AnimatorListenerAdapter_1::OnAnimationResume(
    /* [in] */ IAnimator* animation)
{
    CChangeTransform::SetIdentityTransforms(mView);
    return NOERROR;
}

void CChangeTransform::AnimatorListenerAdapter_1::SetCurrentMatrix(
    /* [in] */ IMatrix* currentMatrix)
{
    mTempMatrix->Set(currentMatrix);
    mView->SetTagInternal(R::id::transitionTransform, mTempMatrix);
    mTrans->Restore(mView);
}

//===============================================================
// CChangeTransform::MatrixProperty::
//===============================================================
CAR_INTERFACE_IMPL(CChangeTransform::MatrixProperty, Object, IProperty)

CChangeTransform::MatrixProperty::MatrixProperty(
    /* [in] */ const String& str)
{
    mName = str;
}

ECode CChangeTransform::MatrixProperty::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly)
    *readOnly = FALSE;
    return NOERROR;
}

ECode CChangeTransform::MatrixProperty::Set(
    /* [in] */ IInterface* object,
    /* [in] */ IInterface* value)
{
    AutoPtr<IView> v = IView::Probe(object);
    v->SetAnimationMatrix(IMatrix::Probe(value));
    return NOERROR;
}

ECode CChangeTransform::MatrixProperty::Get(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

ECode CChangeTransform::MatrixProperty::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode CChangeTransform::MatrixProperty::GetType(
    /* [out] */ ClassID* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ECLSID_CMatrix;
    return NOERROR;
}

ECode CChangeTransform::MatrixProperty::IsWriteOnly(
    /* [out] */ Boolean* writeOnly)
{
    VALIDATE_NOT_NULL(writeOnly)
    *writeOnly = FALSE;
    return NOERROR;
}

ECode CChangeTransform::MatrixProperty::ForbiddenOperate(
    /* [out] */ Boolean* forbidden)
{
    VALIDATE_NOT_NULL(forbidden)
    *forbidden = FALSE;
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
