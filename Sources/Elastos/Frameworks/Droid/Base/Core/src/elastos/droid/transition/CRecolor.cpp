
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/transition/CRecolor.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/animation/ObjectAnimator.h"

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Widget::ITextView;

using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CRecolor::
//===============================================================

const String CRecolor::PROPNAME_BACKGROUND("android:recolor:background");
const String CRecolor::PROPNAME_TEXT_COLOR("android:recolor:textColor");

CAR_OBJECT_IMPL(CRecolor)

ECode CRecolor::constructor()
{
    return NOERROR;
}

ECode CRecolor::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Transition::constructor(context, attrs);
    return NOERROR;
}

void CRecolor::CaptureValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ctv = (CTransitionValues*)transitionValues;
    AutoPtr<ICharSequence> pro_background;
    CString::New(PROPNAME_BACKGROUND, (ICharSequence**)&pro_background);
    AutoPtr<IDrawable> bg;
    ctv->mView->GetBackground((IDrawable**)&bg);
    ctv->mValues->Put(pro_background, bg);
    if (ITextView::Probe(ctv->mView) != NULL) {
        AutoPtr<ICharSequence> pro_color;
        CString::New(PROPNAME_TEXT_COLOR, (ICharSequence**)&pro_color);
        Int32 clr = 0;
        (ITextView::Probe(ctv->mView))->GetCurrentTextColor(&clr);
        AutoPtr<IInteger32> iClr;
        CInteger32::New(clr, (IInteger32**)&iClr);
        ctv->mValues->Put(pro_color, iClr);
    }
}

ECode CRecolor::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CRecolor::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CRecolor::CreateAnimator(
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
    AutoPtr<CTransitionValues> cStartValues = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> cEndValues = (CTransitionValues*)endValues;
    AutoPtr<IView> view = cEndValues->mView;
    AutoPtr<ICharSequence> pro_background;
    CString::New(PROPNAME_BACKGROUND, (ICharSequence**)&pro_background);
    AutoPtr<IInterface> sB, eB;
    cStartValues->mValues->Get(pro_background, (IInterface**)&sB);
    cEndValues->mValues->Get(pro_background, (IInterface**)&eB);
    AutoPtr<IDrawable> startBackground = IDrawable::Probe(sB);
    AutoPtr<IDrawable> endBackground = IDrawable::Probe(eB);
    Boolean changed = FALSE;
    if ( (IColorDrawable::Probe(startBackground) != NULL) && (IColorDrawable::Probe(endBackground) != NULL) ) {
        AutoPtr<IColorDrawable> startColor = IColorDrawable::Probe(startBackground);
        AutoPtr<IColorDrawable> endColor = IColorDrawable::Probe(endBackground);
        Int32 sC = 0, eC = 0;
        if ((startColor->GetColor(&sC), sC) != (endColor->GetColor(&eC), eC)) {
            startColor->GetColor(&sC);
            endColor->SetColor(sC);
            changed = TRUE;
            AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(2);
            (*arr)[0] = sC;
            (*arr)[1] = eC;
            AutoPtr<IObjectAnimator> oa = ObjectAnimator::OfArgb(endBackground, String("color"), arr);
            *result = IAnimator::Probe(oa);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    if (ITextView::Probe(view) != NULL) {
        AutoPtr<ITextView> textView = ITextView::Probe(view);
        AutoPtr<ICharSequence> pro_color;
        CString::New(PROPNAME_TEXT_COLOR, (ICharSequence**)&pro_color);
        AutoPtr<IInterface> s, e;
        cStartValues->mValues->Get(pro_color, (IInterface**)&s);
        cEndValues->mValues->Get(pro_color, (IInterface**)&e);
        Int32 start = 0, end = 0;
        IInteger32::Probe(s)->GetValue(&start);
        IInteger32::Probe(s)->GetValue(&start);
        if (start != end) {
            textView->SetTextColor(end);
            changed = TRUE;
            AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(2);
            (*arr)[0] = start;
            (*arr)[1] = end;
            AutoPtr<IObjectAnimator> oa = ObjectAnimator::OfArgb(textView, String("textColor"), arr);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode CRecolor::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<ITransition> changes;
    CRecolor::New((ITransition**)&changes);

    CloneImpl(changes);
    *obj = changes;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CRecolor::CloneImpl(
    /* [in] */ ITransition* obj)
{
    VALIDATE_NOT_NULL(obj);

    return Transition::CloneImpl(ITransition::Probe(obj));
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
