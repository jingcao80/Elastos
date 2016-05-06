
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/transition/CChangeText.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/animation/AnimatorSet.h"
#include "elastos/droid/animation/ValueAnimator.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/graphics/Color.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::AnimatorSet;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::ValueAnimator;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Widget::ITextView;

using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Transition {

const String CChangeText::TAG("TextChange");

const String CChangeText::PROPNAME_TEXT("android:textchange:text");
const String CChangeText::PROPNAME_TEXT_SELECTION_START("android:textchange:textSelectionStart");
const String CChangeText::PROPNAME_TEXT_SELECTION_END("android:textchange:textSelectionEnd");
const String CChangeText::PROPNAME_TEXT_COLOR("android:textchange:textColor");

AutoPtr<ArrayOf<String> > CChangeText::sTransitionProperties = ArrayOf<String>::Alloc(3);

//===============================================================
// CChangeText::TransitionListenerAdapter_1::
//===============================================================
CChangeText::TransitionListenerAdapter_1::TransitionListenerAdapter_1(
    /* [in] */ Int32 behavior,
    /* [in] */ ITextView* v,
    /* [in] */ Int32 startSelectionStart,
    /* [in] */ Int32 startSelectionEnd,
    /* [in] */ Int32 endSelectionStart,
    /* [in] */ Int32 endSelectionEnd,
    /* [in] */ ICharSequence* startText,
    /* [in] */ ICharSequence* endText,
    /* [in] */ Int32 endColor,
    /* [in] */ CChangeText* owner)
{
    mPausedColor = 0;
    mBehavior = behavior;
    mView = v;
    mStartSelectionStart = startSelectionStart;
    mStartSelectionEnd = startSelectionEnd;
    mEndSelectionStart = endSelectionStart;
    mEndSelectionEnd = endSelectionEnd;
    mStartText = startText;
    mEndText = endText;
    mEndColor = endColor;
    mOwner = owner;
}

ECode CChangeText::TransitionListenerAdapter_1::OnTransitionPause(
    /* [in] */ ITransition* transition)
{
    if (mBehavior != CHANGE_BEHAVIOR_IN) {
        mView->SetText(mEndText);
        if (IEditText::Probe(mView) != NULL) {
            mOwner->SetSelection(IEditText::Probe(mView), mEndSelectionStart, mEndSelectionEnd);
        }
    }
    if (mBehavior > CHANGE_BEHAVIOR_KEEP) {
        mView->GetCurrentTextColor(&mPausedColor);
        mView->SetTextColor(mEndColor);
    }
    return NOERROR;
}

ECode CChangeText::TransitionListenerAdapter_1::OnTransitionResume(
    /* [in] */ ITransition* transition)
{
    if (mBehavior != CHANGE_BEHAVIOR_IN) {
        mView->SetText(mStartText);
        if (IEditText::Probe(mView) != NULL) {
            mOwner->SetSelection(IEditText::Probe(mView), mStartSelectionStart, mStartSelectionEnd);
        }
    }
    if (mBehavior > CHANGE_BEHAVIOR_KEEP) {
        mView->SetTextColor(mPausedColor);
    }
    return NOERROR;
}

//===============================================================
// CChangeText::AnimatorListenerAdapter_1::
//===============================================================
CChangeText::AnimatorListenerAdapter_1::AnimatorListenerAdapter_1(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 endColor)
{
    mView = v;
    mEndColor = endColor;
}

ECode CChangeText::AnimatorListenerAdapter_1::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    // restore opaque alpha and correct end color
    mView->SetTextColor(mEndColor);
    return NOERROR;
}

//===============================================================
// CChangeText::AnimatorListenerAdapter_2::
//===============================================================
CChangeText::AnimatorListenerAdapter_2::AnimatorListenerAdapter_2(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 endSelectionStart,
    /* [in] */ Int32 endSelectionEnd,
    /* [in] */ ICharSequence* startText,
    /* [in] */ ICharSequence* endText,
    /* [in] */ Int32 endColor,
    /* [in] */ CChangeText* owner)
{
    mView = v;
    mEndSelectionStart = endSelectionStart;
    mEndSelectionEnd = endSelectionEnd;
    mStartText = startText;
    mEndText = endText;
    mEndColor = endColor;
    mOwner = owner;
}

ECode CChangeText::AnimatorListenerAdapter_2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<ICharSequence> t;
    mView->GetText((ICharSequence**)&t);
    if (Object::Equals(mStartText->Probe(EIID_IInterface), t->Probe(EIID_IInterface))) {
        // Only set if it hasn't been changed since anim started
        mView->SetText(mEndText);
        if (IEditText::Probe(mView) != NULL) {
            mOwner->SetSelection(IEditText::Probe(mView), mEndSelectionStart,
                    mEndSelectionEnd);
        }
    }
    // restore opaque alpha and correct end color
    mView->SetTextColor(mEndColor);
    return NOERROR;
}

//===============================================================
// CChangeText::AnimatorListenerAdapter_3::
//===============================================================
CChangeText::AnimatorListenerAdapter_3::AnimatorListenerAdapter_3(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 endSelectionStart,
    /* [in] */ Int32 endSelectionEnd,
    /* [in] */ ICharSequence* startText,
    /* [in] */ ICharSequence* endText,
    /* [in] */ CChangeText* owner)
{
    mView = v;
    mEndSelectionStart = endSelectionStart;
    mEndSelectionEnd = endSelectionEnd;
    mStartText = startText;
    mEndText = endText;
    mOwner = owner;
}

ECode CChangeText::AnimatorListenerAdapter_3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<ICharSequence> t;
    mView->GetText((ICharSequence**)&t);
    if (Object::Equals(mStartText->Probe(EIID_IInterface), t->Probe(EIID_IInterface))) {
        // Only set if it hasn't been changed since anim started
        mView->SetText(mEndText);
        if (IEditText::Probe(mView) != NULL) {
            mOwner->SetSelection(IEditText::Probe(mView), mEndSelectionStart, mEndSelectionEnd);
        }
    }
    return NOERROR;
}

//===============================================================
// CChangeText::AnimatorUpdateListener_1::
//===============================================================
CAR_INTERFACE_IMPL(CChangeText::AnimatorUpdateListener_1, Object, IAnimatorUpdateListener)

CChangeText::AnimatorUpdateListener_1::AnimatorUpdateListener_1(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 startColor)
{
    mView = v;
    mStartColor = startColor;
}

ECode CChangeText::AnimatorUpdateListener_1::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> av;
    animation->GetAnimatedValue((IInterface**)&av);
    Int32 currAlpha;
    IInteger32::Probe(av)->GetValue(&currAlpha);
    mView->SetTextColor((currAlpha << 24) | (mStartColor & 0xff0000) |
            (mStartColor & 0xff00) | (mStartColor & 0xff));
    return NOERROR;
}

//===============================================================
// CChangeText::AnimatorUpdateListener_2::
//===============================================================
CAR_INTERFACE_IMPL(CChangeText::AnimatorUpdateListener_2, Object, IAnimatorUpdateListener)

CChangeText::AnimatorUpdateListener_2::AnimatorUpdateListener_2(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 endColor)
{
    mView = v;
    mEndColor = endColor;
}

ECode CChangeText::AnimatorUpdateListener_2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> av;
    animation->GetAnimatedValue((IInterface**)&av);
    Int32 currAlpha;
    IInteger32::Probe(av)->GetValue(&currAlpha);
    mView->SetTextColor(currAlpha << 24 | Color::Red(mEndColor) << 16 |
            Color::Green(mEndColor) << 8 | Color::Red(mEndColor));
    return NOERROR;
}

//===============================================================
// CChangeText::
//===============================================================
CAR_INTERFACE_IMPL(CChangeText, Transition, IChangeText)

CAR_OBJECT_IMPL(CChangeText)

CChangeText::CChangeText()
{
    (*sTransitionProperties)[0] = PROPNAME_TEXT;
    (*sTransitionProperties)[1] = PROPNAME_TEXT_SELECTION_START;
    (*sTransitionProperties)[2] = PROPNAME_TEXT_SELECTION_END;
}

ECode CChangeText::SetChangeBehavior(
    /* [in] */ Int32 changeBehavior)
{
    if (changeBehavior >= CHANGE_BEHAVIOR_KEEP && changeBehavior <= CHANGE_BEHAVIOR_OUT_IN) {
        mChangeBehavior = changeBehavior;
    }
    return NOERROR;
}

ECode CChangeText::GetTransitionProperties(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = sTransitionProperties;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CChangeText::GetChangeBehavior(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mChangeBehavior;
    return NOERROR;
}

void CChangeText::CaptureValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ct = (CTransitionValues*)transitionValues;
    if (ITextView::Probe(ct->mView) != NULL) {
        AutoPtr<ITextView> textview = ITextView::Probe(ct->mView);
        AutoPtr<ICharSequence> t;
        textview->GetText((ICharSequence**)&t);
        AutoPtr<ICharSequence> pro_text;
        CString::New(PROPNAME_TEXT, (ICharSequence**)&pro_text);
        ct->mValues->Put(pro_text, t);
        if (IEditText::Probe(textview) != NULL) {
            AutoPtr<ICharSequence> pro_text_start;
            CString::New(PROPNAME_TEXT_SELECTION_START, (ICharSequence**)&pro_text_start);
            Int32 s = 0;
            textview->GetSelectionStart(&s);
            AutoPtr<IInteger32> iS;
            CInteger32::New(s, (IInteger32**)&iS);
            ct->mValues->Put(pro_text_start, iS);
            AutoPtr<ICharSequence> pro_text_end;
            CString::New(PROPNAME_TEXT_SELECTION_END, (ICharSequence**)&pro_text_end);
            Int32 e = 0;
            textview->GetSelectionEnd(&e);
            AutoPtr<IInteger32> iE;
            CInteger32::New(e, (IInteger32**)&iE);
            ct->mValues->Put(pro_text_end, iE);
        }
        if (mChangeBehavior > CHANGE_BEHAVIOR_KEEP) {
            AutoPtr<ICharSequence> pro_text_clr;
            CString::New(PROPNAME_TEXT_COLOR, (ICharSequence**)&pro_text_clr);
            Int32 clr = 0;
            textview->GetCurrentTextColor(&clr);
            AutoPtr<IInteger32> iC;
            CInteger32::New(clr, (IInteger32**)&iC);
            ct->mValues->Put(pro_text_clr, iC);
        }
    }
}

ECode CChangeText::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CChangeText::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues);
    return NOERROR;
}

ECode CChangeText::CreateAnimator(
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

    AutoPtr<CTransitionValues> sV = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> eV = (CTransitionValues*)endValues;
    if ( !(ITextView::Probe(sV->mView) != NULL) ||
         !(ITextView::Probe(eV->mView) != NULL)) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ITextView> view = ITextView::Probe(eV->mView);
    AutoPtr<IMap> startVals = sV->mValues;
    AutoPtr<IMap> endVals = eV->mValues;
    AutoPtr<ICharSequence> pro_text;
    CString::New(PROPNAME_TEXT, (ICharSequence**)&pro_text);
    AutoPtr<IInterface> sT;
    startVals->Get(pro_text, (IInterface**)&sT);
    AutoPtr<ICharSequence> startText;
    if ( sT != NULL ) {
        startText = ICharSequence::Probe(sT);
    }
    else {
        CString::New(String(""), (ICharSequence**)&startText);
    }
    AutoPtr<IInterface> eT;
    endVals->Get(pro_text, (IInterface**)&eT);
    AutoPtr<ICharSequence> endText;
    if ( eT != NULL ) {
        endText = ICharSequence::Probe(eT);
    }
    else {
        CString::New(String(""), (ICharSequence**)&endText);
    }
    Int32 startSelectionStart, startSelectionEnd, endSelectionStart, endSelectionEnd;
    if (IEditText::Probe(view) != NULL) {
        AutoPtr<ICharSequence> pro_text_start;
        CString::New(PROPNAME_TEXT_SELECTION_START, (ICharSequence**)&pro_text_start);
        AutoPtr<ICharSequence> pro_text_end;
        CString::New(PROPNAME_TEXT_SELECTION_END, (ICharSequence**)&pro_text_end);
        AutoPtr<IInterface> sS, sE, eS, eE;
        startVals->Get(pro_text_start, (IInterface**)&sS);
        startSelectionStart = -1;
        if (sS != NULL)
            IInteger32::Probe(sS)->GetValue(&startSelectionStart);
        startVals->Get(pro_text_end, (IInterface**)&sE);
        startSelectionEnd = startSelectionStart;
        if (sE != NULL)
            IInteger32::Probe(sE)->GetValue(&startSelectionEnd);

        endVals->Get(pro_text_start, (IInterface**)&eS);
        endSelectionStart = -1;
        if (eS != NULL)
            IInteger32::Probe(eS)->GetValue(&endSelectionStart);

        endVals->Get(pro_text_end, (IInterface**)&eE);
        endSelectionEnd = endSelectionStart;
        if (eE != NULL)
            IInteger32::Probe(eE)->GetValue(&endSelectionEnd);
    }
    else {
        startSelectionStart = startSelectionEnd = endSelectionStart = endSelectionEnd = -1;
    }
    if (!Object::Equals(startText, endText)) {
        Int32 startColor;
        Int32 endColor;
        if (mChangeBehavior != CHANGE_BEHAVIOR_IN) {
            view->SetText(startText);
            if (IEditText::Probe(view) != NULL) {
                SetSelection(IEditText::Probe(view), startSelectionStart, startSelectionEnd);
            }
        }
        AutoPtr<IAnimator> anim;
        if (mChangeBehavior == CHANGE_BEHAVIOR_KEEP) {
            startColor = endColor = 0;
            AutoPtr<ArrayOf<Float> > arr = ArrayOf<Float>::Alloc(2);
            (*arr)[0] = 0;
            (*arr)[1] = 1;
            AutoPtr<IValueAnimator> vA = ValueAnimator::OfFloat(arr);
            anim = IAnimator::Probe(vA);
            anim->AddListener(
                new AnimatorListenerAdapter_3(view,
                    endSelectionStart,
                    endSelectionEnd,
                    startText,
                    endText,
                    this));
        }
        else {
            AutoPtr<ICharSequence> pro_text_clr;
            CString::New(PROPNAME_TEXT_COLOR, (ICharSequence**)&pro_text_clr);
            AutoPtr<IInterface> sV, eV;
            startVals->Get(pro_text_clr, (IInterface**)&sV);
            endVals->Get(pro_text_clr, (IInterface**)&eV);
            IInteger32::Probe(sV)->GetValue(&startColor);
            IInteger32::Probe(sV)->GetValue(&endColor);
            // Fade out start text
            AutoPtr<IValueAnimator> outAnim, inAnim;
            if (mChangeBehavior == CHANGE_BEHAVIOR_OUT_IN ||
                    mChangeBehavior == CHANGE_BEHAVIOR_OUT) {
                AutoPtr<ArrayOf<Int32> > arrInt = ArrayOf<Int32>::Alloc(2);
                (*arrInt)[0] = 255;
                (*arrInt)[1] = 0;
                outAnim = ValueAnimator::OfInt32(arrInt);
                outAnim->AddUpdateListener(new AnimatorUpdateListener_1(view, startColor));
                IAnimator::Probe(outAnim)->AddListener(
                    new AnimatorListenerAdapter_2(view,
                        endSelectionStart,
                        endSelectionEnd,
                        startText,
                        endText,
                        endColor,
                        this));
            }
            if (mChangeBehavior == CHANGE_BEHAVIOR_OUT_IN ||
                    mChangeBehavior == CHANGE_BEHAVIOR_IN) {
                AutoPtr<ArrayOf<Int32> > arrInt = ArrayOf<Int32>::Alloc(2);
                (*arrInt)[0] = 0;
                (*arrInt)[1] = 255;
                inAnim = ValueAnimator::OfInt32(arrInt);
                inAnim->AddUpdateListener(new AnimatorUpdateListener_2(view, endColor));
                IAnimator::Probe(inAnim)->AddListener(new AnimatorListenerAdapter_1(view, endColor));
            }
            if (outAnim != NULL && inAnim != NULL) {
                CAnimatorSet::New((IAnimator**)&anim);
                AutoPtr<ArrayOf<IAnimator*> > arr = ArrayOf<IAnimator*>::Alloc(2);
                arr->Set(0, IAnimator::Probe(outAnim));
                arr->Set(1, IAnimator::Probe(inAnim));
                IAnimatorSet::Probe(anim)->PlaySequentially(arr);
            }
            else if (outAnim != NULL) {
                anim = IAnimator::Probe(outAnim);
            }
            else {
                // Must be an in-only animation
                anim = IAnimator::Probe(inAnim);
            }
        }
        AutoPtr<ITransitionListener> transitionListener =
            new TransitionListenerAdapter_1(mChangeBehavior,
                view,
                startSelectionStart,
                startSelectionEnd,
                endSelectionStart,
                endSelectionEnd,
                startText,
                endText,
                endColor,
                this);
        Transition::AddListener(transitionListener);
        if (DBG) {
            Logger::D(TAG, "createAnimator returning %p", anim.Get());
        }
        *result = anim;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

void CChangeText::SetSelection(
    /* [in] */ IEditText* editText,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (start >= 0 && end >= 0) {
        editText->SetSelection(start, end);
    }
}

ECode CChangeText::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<IChangeText> changes;
    CChangeText::New((IChangeText**)&changes);

    CloneImpl(changes);
    *obj = changes;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CChangeText::CloneImpl(
    /* [in] */ IChangeText* obj)
{
    VALIDATE_NOT_NULL(obj);

    return Transition::CloneImpl(ITransition::Probe(obj));
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
