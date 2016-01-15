
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/transition/CChangeImageTransform.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/transition/CTransitionUtils.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/widget/ImageView.h"

using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::ITypeConverter;
using Elastos::Droid::Animation::EIID_ITypeEvaluator;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::ECLSID_CMatrix;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Transition::CTransitionValues;
using Elastos::Droid::Transition::CTransitionUtils;
using Elastos::Droid::Utility::EIID_IProperty;
using Elastos::Droid::Widget::EIID_IImageView;
using Elastos::Droid::Widget::ImageView;
using Elastos::Droid::Widget::ImageViewScaleType;

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CChangeImageTransform::
//===============================================================
String CChangeImageTransform::TAG("ChangeImageTransform");

String CChangeImageTransform::PROPNAME_MATRIX("android:changeImageTransform:matrix");
String CChangeImageTransform::PROPNAME_BOUNDS("android:changeImageTransform:bounds");

AutoPtr<ArrayOf<String> > CChangeImageTransform::sTransitionProperties = ArrayOf<String>::Alloc(2);

AutoPtr<ITypeEvaluator> CChangeImageTransform::NULL_MATRIX_EVALUATOR = new TypeEvaluator();

AutoPtr<IProperty> CChangeImageTransform::ANIMATED_TRANSFORM_PROPERTY = new MatrixProperty(String("animatedTransform"));

CAR_OBJECT_IMPL(CChangeImageTransform)

CChangeImageTransform::CChangeImageTransform()
{
    (*sTransitionProperties)[0] = PROPNAME_MATRIX;
    (*sTransitionProperties)[1] = PROPNAME_BOUNDS;
}

ECode CChangeImageTransform::constructor()
{
    return NOERROR;
}

ECode CChangeImageTransform::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Transition::constructor(context, attrs);
    return NOERROR;
}

void CChangeImageTransform::CaptureValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> cT = (CTransitionValues*)transitionValues;
    AutoPtr<IView> view = cT->mView;
    Int32 v = 0;
    view->GetVisibility(&v);
    if (!(view->Probe(EIID_IImageView) != NULL ) || v != IView::VISIBLE) {
        return;
    }
    AutoPtr<IImageView> imageView = IImageView::Probe(view);
    AutoPtr<IDrawable> drawable;
    imageView->GetDrawable((IDrawable**)&drawable);
    if (drawable == NULL) {
        return;
    }
    AutoPtr<IMap> values = cT->mValues;

    Int32 left;
    view->GetLeft(&left);
    Int32 top;
    view->GetTop(&top);
    Int32 right;
    view->GetRight(&right);
    Int32 bottom;
    view->GetBottom(&bottom);

    AutoPtr<IRect> bounds;
    CRect::New(left, top, right, bottom, (IRect**)&bounds);
    AutoPtr<ICharSequence> pro_bounds;
    CString::New(PROPNAME_BOUNDS, (ICharSequence**)&pro_bounds);
    values->Put(pro_bounds, bounds);
    AutoPtr<IMatrix> matrix;
    ImageViewScaleType scaleType;
    imageView->GetScaleType(&scaleType);
    if (scaleType == Elastos::Droid::Widget::ImageViewScaleType_FIT_XY) {
        imageView->GetImageMatrix((IMatrix**)&matrix);
        Boolean bIsIdentity = FALSE;
        matrix->IsIdentity(&bIsIdentity);
        if (!bIsIdentity) {
            CMatrix::New(matrix, (IMatrix**)&matrix);
        }
        else {
            Int32 drawableWidth;
            drawable->GetIntrinsicWidth(&drawableWidth);
            Int32 drawableHeight;
            drawable->GetIntrinsicHeight(&drawableHeight);
            if (drawableWidth > 0 && drawableHeight > 0) {
                Int32 w = 0, h = 0;
                bounds->GetWidth(&w);
                bounds->GetHeight(&h);
                Float scaleX = ((Float) w) / drawableWidth;
                Float scaleY = ((Float) h) / drawableHeight;
                CMatrix::New((IMatrix**)&matrix);
                matrix->SetScale(scaleX, scaleY);
            }
            else {
                matrix = NULL;
            }
        }
    }
    else {
        AutoPtr<IMatrix> m;
        imageView->GetImageMatrix((IMatrix**)&m);
        CMatrix::New(m, (IMatrix**)&matrix);
    }
    AutoPtr<ICharSequence> pro_matrix;
    CString::New(PROPNAME_MATRIX, (ICharSequence**)&pro_matrix);
    values->Put(pro_matrix, matrix);
}

ECode CChangeImageTransform::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CChangeImageTransform::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CChangeImageTransform::GetTransitionProperties(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = sTransitionProperties;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CChangeImageTransform::CreateAnimator(
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
    AutoPtr<CTransitionValues> cS = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> cE = (CTransitionValues*)endValues;
    AutoPtr<ICharSequence> pro_bounds;
    CString::New(PROPNAME_BOUNDS, (ICharSequence**)&pro_bounds);
    AutoPtr<IInterface> s, e;
    cS->mValues->Get(pro_bounds, (IInterface**)&s);
    cE->mValues->Get(pro_bounds, (IInterface**)&e);
    AutoPtr<IRect> startBounds = IRect::Probe(s);
    AutoPtr<IRect> endBounds = IRect::Probe(e);
    if (startBounds == NULL || endBounds == NULL) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<ICharSequence> pro_matrix;
    CString::New(PROPNAME_MATRIX, (ICharSequence**)&pro_matrix);
    AutoPtr<IInterface> sM, eM;
    cS->mValues->Get(pro_matrix, (IInterface**)&sM);
    cE->mValues->Get(pro_matrix, (IInterface**)&eM);
    AutoPtr<IMatrix> startMatrix = IMatrix::Probe(sM);
    AutoPtr<IMatrix> endMatrix = IMatrix::Probe(eM);

    Boolean matricesEqual = (startMatrix == NULL && endMatrix == NULL) ||
            (startMatrix != NULL && Object::Equals(startMatrix->Probe(EIID_IInterface), endMatrix->Probe(EIID_IInterface)));

    if (Object::Equals(startBounds->Probe(EIID_IInterface), endBounds->Probe(EIID_IInterface)) && matricesEqual) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IImageView> imageView = IImageView::Probe(cE->mView);
    AutoPtr<IDrawable> drawable;
    imageView->GetDrawable((IDrawable**)&drawable);
    Int32 drawableWidth;
    drawable->GetIntrinsicWidth(&drawableWidth);
    Int32 drawableHeight;
    drawable->GetIntrinsicHeight(&drawableHeight);

    AutoPtr<IObjectAnimator> animator;
    if (drawableWidth == 0 || drawableHeight == 0) {
        animator = CreateNullAnimator(imageView);
    }
    else {
        if (startMatrix == NULL) {
            startMatrix = CMatrix::IDENTITY_MATRIX;
        }
        if (endMatrix == NULL) {
            endMatrix = CMatrix::IDENTITY_MATRIX;
        }
        ANIMATED_TRANSFORM_PROPERTY->Set(imageView, startMatrix);
        animator = CreateMatrixAnimator(imageView, startMatrix, endMatrix);
    }
    *result = IAnimator::Probe(animator);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IObjectAnimator> CChangeImageTransform::CreateNullAnimator(
    /* [in] */ IImageView* imageView)
{
    return ObjectAnimator::OfObject(imageView->Probe(EIID_IInterface), ANIMATED_TRANSFORM_PROPERTY,
            ITypeConverter::Probe(NULL_MATRIX_EVALUATOR), NULL, NULL);
}

AutoPtr<IObjectAnimator> CChangeImageTransform::CreateMatrixAnimator(
    /* [in] */ IImageView* imageView,
    /* [in] */ IMatrix* startMatrix,
    /* [in] */ IMatrix* endMatrix)
{
    AutoPtr<CTransitionUtils::MatrixEvaluator> p = new CTransitionUtils::MatrixEvaluator();
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
    (*arr)[0] = endMatrix->Probe(EIID_IInterface);
    return ObjectAnimator::OfObject(imageView, ANIMATED_TRANSFORM_PROPERTY,
            ITypeConverter::Probe(p), ITypeEvaluator::Probe(startMatrix), arr);
}

ECode CChangeImageTransform::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<ITransition> changes;
    CChangeImageTransform::New((ITransition**)&changes);

    CloneImpl(changes);
    *obj = changes;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CChangeImageTransform::CloneImpl(
    /* [in] */ ITransition* obj)
{
    VALIDATE_NOT_NULL(obj);

    return Transition::CloneImpl(ITransition::Probe(obj));
}

//===============================================================
// CChangeImageTransform::MatrixProperty::
//===============================================================
CAR_INTERFACE_IMPL(CChangeImageTransform::MatrixProperty, Object, IProperty)

CChangeImageTransform::MatrixProperty::MatrixProperty(
    /* [in] */ const String& str)
{
    mName = str;
}

ECode CChangeImageTransform::MatrixProperty::IsReadOnly(
    /* [out] */ Boolean* readOnly)
{
    VALIDATE_NOT_NULL(readOnly)
    *readOnly = FALSE;
    return NOERROR;
}

ECode CChangeImageTransform::MatrixProperty::Set(
    /* [in] */ IInterface* object,
    /* [in] */ IInterface* value)
{
    AutoPtr<ImageView> cobj = (ImageView*)IImageView::Probe(object);
    cobj->AnimateTransform(IMatrix::Probe(value));
    return NOERROR;
}

ECode CChangeImageTransform::MatrixProperty::Get(
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

ECode CChangeImageTransform::MatrixProperty::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode CChangeImageTransform::MatrixProperty::GetType(
    /* [out] */ ClassID* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ECLSID_CMatrix;
    return NOERROR;
}

ECode CChangeImageTransform::MatrixProperty::IsWriteOnly(
    /* [out] */ Boolean* writeOnly)
{
    VALIDATE_NOT_NULL(writeOnly)
    *writeOnly = FALSE;
    return NOERROR;
}

ECode CChangeImageTransform::MatrixProperty::ForbiddenOperate(
    /* [out] */ Boolean* forbidden)
{
    VALIDATE_NOT_NULL(forbidden)
    *forbidden = FALSE;
    return NOERROR;
}

//===============================================================
// CChangeImageTransform::TypeEvaluator::
//===============================================================
CAR_INTERFACE_IMPL(CChangeImageTransform::TypeEvaluator, Object, ITypeEvaluator)

ECode CChangeImageTransform::TypeEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
