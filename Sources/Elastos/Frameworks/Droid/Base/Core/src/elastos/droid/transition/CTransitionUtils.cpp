
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/transition/CTransitionUtils.h"
#include "elastos/droid/transition/CTransitionSet.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/widget/CImageView.h"

#include <elastos/core/Math.h>

using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::EIID_ITypeEvaluator;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::ImageViewScaleType_CENTER_CROP;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CTransitionUtils::
//===============================================================

Int32 CTransitionUtils::MAX_IMAGE_SIZE = (1024 * 1024);

CAR_SINGLETON_IMPL(CTransitionUtils)

CAR_INTERFACE_IMPL(CTransitionUtils, Singleton, ITransitionUtils)

ECode CTransitionUtils::MergeAnimators(
    /* [in] */ IAnimator* animator1,
    /* [in] */ IAnimator* animator2,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)

    if (animator1 == NULL) {
        *result = animator2;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (animator2 == NULL) {
        *result = animator1;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        AutoPtr<IAnimatorSet> animatorSet;
        CAnimatorSet::New((IAnimatorSet**)&animatorSet);
        AutoPtr<ArrayOf<IAnimator*> > arr = ArrayOf<IAnimator*>::Alloc(2);
        (*arr)[0] = animator1;
        (*arr)[1] = animator2;
        animatorSet->PlayTogether(arr);
        *result = IAnimator::Probe(animatorSet);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode CTransitionUtils::MergeTransitions(
    /* [in] */ ArrayOf<ITransition*>* transitions,
    /* [out] */ ITransition** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 count = 0;
    Int32 nonNullIndex = -1;
    for (Int32 i = 0; i < transitions->GetLength(); i++) {
        if ((*transitions)[i] != NULL) {
            count++;
            nonNullIndex = i;
        }
    }

    if (count == 0) {
        *result = NULL;
        return NOERROR;
    }

    if (count == 1) {
        *result = (*transitions)[nonNullIndex];
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<ITransitionSet> transitionSet;
    CTransitionSet::New((ITransitionSet**)&transitionSet);
    for (Int32 i = 0; i < transitions->GetLength(); i++) {
        if ((*transitions)[i] != NULL) {
            transitionSet->AddTransition((*transitions)[i]);
        }
    }
    *result = ITransition::Probe(transitionSet);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTransitionUtils::CopyViewImage(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [in] */ IView* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMatrix> matrix;
    CMatrix::New((IMatrix**)&matrix);
    Int32 x = 0, y = 0;
    parent->GetScrollX(&x);
    parent->GetScrollY(&y);
    matrix->SetTranslate(-x, -y);
    view->TransformMatrixToGlobal(matrix);
    IView::Probe(sceneRoot)->TransformMatrixToLocal(matrix);
    Int32 w = 0, h = 0;
    view->GetWidth(&w);
    view->GetHeight(&h);
    AutoPtr<IRectF> bounds;
    CRectF::New(0, 0, w, h, (IRectF**)&bounds);
    Boolean res;
    matrix->MapRect(bounds, &res);
    AutoPtr<CRectF> cB = (CRectF*)bounds.Get();
    Int32 left = Elastos::Core::Math::Round(cB->mLeft);
    Int32 top = Elastos::Core::Math::Round(cB->mTop);
    Int32 right = Elastos::Core::Math::Round(cB->mRight);
    Int32 bottom = Elastos::Core::Math::Round(cB->mBottom);

    AutoPtr<IContext> c;
    view->GetContext((IContext**)&c);
    AutoPtr<IImageView> copy;
    CImageView::New(c, (IImageView**)&copy);
    copy->SetScaleType(ImageViewScaleType_CENTER_CROP);
    AutoPtr<IBitmap> bitmap;
    CreateViewBitmap(view, matrix, bounds, (IBitmap**)&bitmap);
    if (bitmap != NULL) {
        copy->SetImageBitmap(bitmap);
    }
    Int32 widthSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(right - left,
                            Elastos::Droid::View::View::MeasureSpec::EXACTLY);
    Int32 heightSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(bottom - top,
                            Elastos::Droid::View::View::MeasureSpec::EXACTLY);
    IView::Probe(copy)->Measure(widthSpec, heightSpec);
    IView::Probe(copy)->Layout(left, top, right, bottom);
    *result = IView::Probe(copy);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTransitionUtils::CreateViewBitmap(
    /* [in] */ IView* view,
    /* [in] */ IMatrix* matrix,
    /* [in] */ IRectF* bounds,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)

    Float w = 0.0f, h = 0.0f;
    bounds->GetWidth(&w);
    bounds->GetHeight(&h);
    AutoPtr<IBitmap> bitmap;
    Int32 bitmapWidth = Elastos::Core::Math::Round(w);
    Int32 bitmapHeight = Elastos::Core::Math::Round(h);
    if (bitmapWidth > 0 && bitmapHeight > 0) {
        Float scale = Elastos::Core::Math::Min((Float)1, ((Float)MAX_IMAGE_SIZE) / (bitmapWidth * bitmapHeight));
        bitmapWidth *= scale;
        bitmapHeight *= scale;
        AutoPtr<CRectF> cB = (CRectF*)bounds;
        Boolean b;
        matrix->PostTranslate(-cB->mLeft, -cB->mTop, &b);
        matrix->PostScale(scale, scale, &b);
        CBitmap::CreateBitmap(bitmapWidth, bitmapHeight, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
        AutoPtr<ICanvas> canvas;
        CCanvas::New(bitmap, (ICanvas**)&canvas);
        canvas->Concat(matrix);
        view->Draw(canvas);
    }
    *result = bitmap;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================
// CTransitionUtils::MatrixEvaluator::
//===============================================================
CAR_INTERFACE_IMPL(CTransitionUtils::MatrixEvaluator, Object, ITypeEvaluator)

CTransitionUtils::MatrixEvaluator::MatrixEvaluator()
{
    mTempStartValues = ArrayOf<Float>::Alloc(9);

    mTempEndValues = ArrayOf<Float>::Alloc(9);

    CMatrix::New((IMatrix**)&mTempMatrix);
}

ECode CTransitionUtils::MatrixEvaluator::Evaluate(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* startValue,
    /* [in] */ IInterface* endValue,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatrix> sV = IMatrix::Probe(startValue);
    AutoPtr<IMatrix> eV = IMatrix::Probe(endValue);
    sV->GetValues(mTempStartValues);
    eV->GetValues(mTempEndValues);
    for (Int32 i = 0; i < 9; i++) {
        Float diff = (*mTempEndValues)[i] - (*mTempStartValues)[i];
        (*mTempEndValues)[i] = (*mTempStartValues)[i] + (fraction * diff);
    }
    mTempMatrix->SetValues(mTempEndValues);
    *result = mTempMatrix;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
