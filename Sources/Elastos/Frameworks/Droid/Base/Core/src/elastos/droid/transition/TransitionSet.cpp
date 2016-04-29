
#include "Elastos.Droid.Content.h"
#include "elastos/droid/transition/TransitionSet.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/transition/CTransitionSet.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Transition::CTransitionSet;

using Elastos::Utility::CArrayList;
using Elastos::Utility::IListIterator;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// TransitionSet::
//===============================================================
CAR_INTERFACE_IMPL(TransitionSet, Transition, ITransitionSet)
TransitionSet::TransitionSet()
{
    CArrayList::New((IArrayList**)&mTransitions);
}

ECode TransitionSet::constructor()
{
    return NOERROR;
}

ECode TransitionSet::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Transition::constructor(context, attrs);
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                const_cast<Int32*>(R::styleable::TransitionSet),
                ArraySize(R::styleable::TransitionSet));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    Int32 ordering = 0;
    a->GetInt32(R::styleable::TransitionSet_transitionOrdering,
            TransitionSet::ORDERING_TOGETHER, &ordering);
    SetOrdering(ordering);
    a->Recycle();
    return NOERROR;
}

ECode TransitionSet::SetOrdering(
    /* [in] */ Int32 ordering)
{
    switch (ordering) {
        case ORDERING_SEQUENTIAL:
            mPlayTogether = FALSE;
            break;
        case ORDERING_TOGETHER:
            mPlayTogether = TRUE;
            break;
        default:
            return E_RUNTIME_EXCEPTION;
            // throw new AndroidRuntimeException("Invalid parameter for TransitionSet " +
            //         "ordering: " + ordering);
    }
    return NOERROR;
}

ECode TransitionSet::GetOrdering(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPlayTogether ? ORDERING_TOGETHER : ORDERING_SEQUENTIAL;
    return NOERROR;
}

ECode TransitionSet::AddTransition(
    /* [in] */ ITransition* transition)
{
    if (transition != NULL) {
        mTransitions->Add(transition);
        AutoPtr<Transition> ct = (Transition*)transition;
        ct->mParent = this;
        if (mDuration >= 0) {
            transition->SetDuration(mDuration);
        }
    }
    return NOERROR;
}

ECode TransitionSet::GetTransitionCount(
    /* [out] */ Int32* result)
{
    return mTransitions->GetSize(result);
}

ECode TransitionSet::GetTransitionAt(
    /* [in] */ Int32 index,
    /* [out] */ ITransition** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    if (index < 0 || index >= numTransitions) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IInterface> t;
    mTransitions->Get(index, (IInterface**)&t);
    *result = ITransition::Probe(t);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TransitionSet::SetDuration(
    /* [in] */ Int64 duration)
{
    Transition::SetDuration(duration);
    if (mDuration >= 0 && mTransitions != NULL) {
        Int32 numTransitions = 0;
        mTransitions->GetSize(&numTransitions);
        for (Int32 i = 0; i < numTransitions; ++i) {
            AutoPtr<IInterface> t;
            mTransitions->Get(i, (IInterface**)&t);
            AutoPtr<ITransition> p = ITransition::Probe(t);
            p->SetDuration(duration);
        }
    }
    return NOERROR;
}

ECode TransitionSet::SetStartDelay(
    /* [in] */ Int64 startDelay)
{
    return Transition::SetStartDelay(startDelay);
}

ECode TransitionSet::SetInterpolator(
    /* [in] */ ITimeInterpolator* interpolator)
{
    return Transition::SetInterpolator(interpolator);
}

ECode TransitionSet::AddTarget(
    /* [in] */ IView* target)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->AddTarget(target);
    }

    return Transition::AddTarget(target);
}

ECode TransitionSet::AddTarget(
    /* [in] */ Int32 targetId)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->AddTarget(targetId);
    }
    return Transition::AddTarget(targetId);
}

ECode TransitionSet::AddTarget(
    /* [in] */ const String& targetName)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->AddTarget(targetName);
    }
    return Transition::AddTarget(targetName);
}

// ECode TransitionSet::AddTarget(
//     /* [in] */ Class targetType)
// {
//     VALIDATE_NOT_NULL(result)

//     Int32 numTransitions = 0;
//     mTransitions->GetSize(&numTransitions);
//     for (Int32 i = 0; i < numTransitions; i++) {
//         AutoPtr<IInterface> t;
//         mTransitions->Get(i, (IInterface**)&t);
//         AutoPtr<ITransition> p = ITransition::Probe(t);
//         p->AddTarget(targetType);
//     }

//     return Transition::AddTarget(targetType);
// }

ECode TransitionSet::AddListener(
    /* [in] */ ITransitionListener* listener)
{
    return Transition::AddListener(listener);
}

ECode TransitionSet::RemoveTarget(
    /* [in] */ Int32 targetId)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->RemoveTarget(targetId);
    }
    return Transition::RemoveTarget(targetId);
}

ECode TransitionSet::RemoveTarget(
    /* [in] */ IView* target)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->RemoveTarget(target);
    }
    return Transition::RemoveTarget(target);
}

// ECode TransitionSet::RemoveTarget(
//     /* [in] */ Class target)
// {
//     Int32 numTransitions = 0;
//     mTransitions->GetSize(&numTransitions);
//     for (Int32 i = 0; i < numTransitions; i++) {
//         AutoPtr<IInterface> t;
//         mTransitions->Get(i, (IInterface**)&t);
//         AutoPtr<ITransition> p = ITransition::Probe(t);
//         p->RemoveTarget(target);
//     }
//     return Transition::RemoveTarget(target, (ITransition**)&p);
// }

ECode TransitionSet::RemoveTarget(
    /* [in] */ const String& target)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->RemoveTarget(target);
    }

    return Transition::RemoveTarget(target);
}

ECode TransitionSet::ExcludeTarget(
    /* [in] */ IView* target,
    /* [in] */ Boolean exclude)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->ExcludeTarget(target, exclude);
    }
    return Transition::ExcludeTarget(target, exclude);
}

ECode TransitionSet::ExcludeTarget(
    /* [in] */ const String& targetName,
    /* [in] */ Boolean exclude)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->ExcludeTarget(targetName, exclude);
    }
    return Transition::ExcludeTarget(targetName, exclude);
}

ECode TransitionSet::ExcludeTarget(
    /* [in] */ Int32 targetId,
    /* [in] */ Boolean exclude)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->ExcludeTarget(targetId, exclude);
    }
    return Transition::ExcludeTarget(targetId, exclude);
}

// ECode TransitionSet::ExcludeTarget(
//     /* [in] */ Class type,
//     /* [in] */ Boolean exclude)
// {
//     VALIDATE_NOT_NULL(result)

//     Int32 numTransitions = 0;
//     mTransitions->GetSize(&numTransitions);
//     for (Int32 i = 0; i < numTransitions; i++) {
//         AutoPtr<IInterface> t;
//         mTransitions->Get(i, (IInterface**)&t);
//         AutoPtr<ITransition> p = ITransition::Probe(t);
//         p->ExcludeTarget(type, exclude);
//     }
//     return Transition::ExcludeTarget(type, exclude);
// }

ECode TransitionSet::RemoveListener(
    /* [in] */ ITransitionListener* listener)
{
    return Transition::RemoveListener(listener);
}

ECode TransitionSet::SetPathMotion(
    /* [in] */ IPathMotion* pathMotion)
{
    Transition::SetPathMotion(pathMotion);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->SetPathMotion(pathMotion);
    }
    return NOERROR;
}

ECode TransitionSet::ForceVisibility(
    /* [in] */ Int32 visibility,
    /* [in] */ Boolean isStartValue)
{
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->ForceVisibility(visibility, isStartValue);
    }
    return NOERROR;
}

ECode TransitionSet::RemoveTransition(
    /* [in] */ ITransition* transition)
{
    mTransitions->Remove(transition);
    AutoPtr<Transition> ct = (Transition*)transition;
    ct->mParent = NULL;
    return NOERROR;
}

void TransitionSet::SetupStartEndListeners()
{
    AutoPtr<TransitionSetListener> listener = new TransitionSetListener(this);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> childTransition = ITransition::Probe(t);
        childTransition->AddListener(listener);
    }
    mTransitions->GetSize(&mCurrentListeners);
}

ECode TransitionSet::CreateAnimators(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValuesMaps* startValues,
    /* [in] */ ITransitionValuesMaps* endValues,
    /* [in] */ IArrayList* startValuesList,
    /* [in] */ IArrayList* endValuesList)
{
    Int64 startDelay = 0;
    GetStartDelay(&startDelay);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; i++) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> childTransition = ITransition::Probe(t);
        // We only set the start delay on the first transition if we are playing
        // the transitions sequentially.
        if (startDelay > 0 && (mPlayTogether || i == 0)) {
            Int64 childStartDelay = 0;
            childTransition->GetStartDelay(&childStartDelay);
            if (childStartDelay > 0) {
                childTransition->SetStartDelay(startDelay + childStartDelay);
            }
            else {
                childTransition->SetStartDelay(startDelay);
            }
        }
        assert(0 && "TODO");
        // childTransition->CreateAnimators(sceneRoot, startValues, endValues, startValuesList,
        //         endValuesList);
    }
    return NOERROR;
}

ECode TransitionSet::RunAnimators()
{
    Boolean bEmp = FALSE;
    if ((mTransitions->IsEmpty(&bEmp), bEmp)) {
        Start();
        End();
        return NOERROR;
    }
    SetupStartEndListeners();
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    if (!mPlayTogether) {
        // Setup sequence with listeners
        // TODO: Need to add listeners in such a way that we can remove them later if canceled
        for (Int32 i = 1; i < numTransitions; ++i) {
            AutoPtr<IInterface> pt;
            mTransitions->Get(i - 1, (IInterface**)&pt);
            AutoPtr<ITransition> previousTransition = ITransition::Probe(pt);
            AutoPtr<IInterface> nt;
            mTransitions->Get(i, (IInterface**)&nt);
            AutoPtr<ITransition> nextTransition = ITransition::Probe(nt);
            AutoPtr<TransitionListenerAdapter_1> p = new TransitionListenerAdapter_1(nextTransition);
            previousTransition->AddListener(ITransitionListener::Probe(p));
        }
        AutoPtr<IInterface> ft;
        mTransitions->Get(0, (IInterface**)&ft);
        AutoPtr<ITransition> firstTransition = ITransition::Probe(ft);
        if (firstTransition != NULL) {
            firstTransition->RunAnimators();
        }
    }
    else {
        for (Int32 i = 0; i < numTransitions; ++i) {
            AutoPtr<IInterface> t;
            mTransitions->Get(i, (IInterface**)&t);
            AutoPtr<ITransition> p = ITransition::Probe(t);
            p->RunAnimators();
        }
    }
    return NOERROR;
}

ECode TransitionSet::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ctv = (CTransitionValues*)transitionValues;
    Boolean b = FALSE;
    if ((IsValidTarget(ctv->mView, &b), b)) {
        AutoPtr<IIterator> lit;
        mTransitions->GetIterator((IIterator**)&lit);
        Boolean bHasMore = FALSE;
        while ((lit->HasNext(&bHasMore), bHasMore)) {
            AutoPtr<IInterface> p;
            lit->GetNext((IInterface**)&p);
            AutoPtr<ITransition> childTransition = ITransition::Probe(p);
            Boolean bValidT = FALSE;
            if ((childTransition->IsValidTarget(ctv->mView, &bValidT), bValidT)) {
                childTransition->CaptureStartValues(transitionValues);
                ctv->mTargetedTransitions->Add(childTransition);
            }
        }
    }
    return NOERROR;
}

ECode TransitionSet::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ctv = (CTransitionValues*)transitionValues;
    Boolean b = FALSE;
    if ((IsValidTarget(ctv->mView, &b), b)) {
        AutoPtr<IIterator> lit;
        mTransitions->GetIterator((IIterator**)&lit);
        Boolean bHasMore = FALSE;
        while ((lit->HasNext(&bHasMore), bHasMore)) {
            AutoPtr<IInterface> p;
            lit->GetNext((IInterface**)&p);
            AutoPtr<ITransition> childTransition = ITransition::Probe(p);
            Boolean bValidT = FALSE;
            if ((childTransition->IsValidTarget(ctv->mView, &bValidT), bValidT)) {
                childTransition->CaptureEndValues(transitionValues);
                ctv->mTargetedTransitions->Add(childTransition);
            }
        }
    }
    return NOERROR;
}

ECode TransitionSet::CapturePropagationValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    Transition::CapturePropagationValues(transitionValues);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->CapturePropagationValues(transitionValues);
    }
    return NOERROR;
}

ECode TransitionSet::Pause(
    /* [in] */ IView* sceneRoot)
{
    Transition::Pause(sceneRoot);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->Pause(sceneRoot);
    }
    return NOERROR;
}

ECode TransitionSet::Resume(
    /* [in] */ IView* sceneRoot)
{
    Transition::Resume(sceneRoot);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->Resume(sceneRoot);
    }
    return NOERROR;
}

ECode TransitionSet::Cancel()
{
    Transition::Cancel();
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->Cancel();
    }
    return NOERROR;
}

ECode TransitionSet::SetSceneRoot(
    /* [in] */ IViewGroup* sceneRoot)
{
    Transition::SetSceneRoot(sceneRoot);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->SetSceneRoot(sceneRoot);
    }
    return NOERROR;
}

ECode TransitionSet::SetCanRemoveViews(
    /* [in] */ Boolean canRemoveViews)
{
    Transition::SetCanRemoveViews(canRemoveViews);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->SetCanRemoveViews(canRemoveViews);
    }
    return NOERROR;
}

ECode TransitionSet::SetPropagation(
    /* [in] */ ITransitionPropagation* propagation)
{
    Transition::SetPropagation(propagation);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->SetPropagation(propagation);
    }
    return NOERROR;
}

ECode TransitionSet::SetEpicenterCallback(
    /* [in] */ IEpicenterCallback* epicenterCallback)
{
    Transition::SetEpicenterCallback(epicenterCallback);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        p->SetEpicenterCallback(epicenterCallback);
    }
    return NOERROR;
}

ECode TransitionSet::ToString(
    /* [in] */ const String& indent,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)

    String result;
    Transition::ToString(indent, &result);
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<ITransition> p = ITransition::Probe(t);
        result += "\n";
        String strP;
        p->ToString(indent + "  ", &strP);
        result += strP;
    }
    *res = result;
    return NOERROR;
}

ECode TransitionSet::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<ITransitionSet> trans;
    CTransitionSet::New((ITransitionSet**)&trans);

    CloneImpl(trans);
    *obj = trans;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode TransitionSet::CloneImpl(
    /* [in] */ ITransitionSet* transObj)
{
    VALIDATE_NOT_NULL(transObj);

    Transition::CloneImpl(ITransition::Probe(transObj));
    TransitionSet* transSet = (TransitionSet*)transObj;

    CArrayList::New((IArrayList**)&(transSet->mTransitions));
    Int32 numTransitions = 0;
    mTransitions->GetSize(&numTransitions);
    for (Int32 i = 0; i < numTransitions; ++i) {
        AutoPtr<IInterface> t;
        mTransitions->Get(i, (IInterface**)&t);
        AutoPtr<IInterface> pInf;
        ICloneable::Probe(t)->Clone((IInterface**)&pInf);
        transSet->AddTransition(ITransition::Probe(pInf));
    }

    return NOERROR;
}

//===============================================================
// TransitionSet::TransitionSetListener::
//===============================================================

TransitionSet::TransitionSetListener::TransitionSetListener(
    /* [in] */ ITransitionSet* transitionSet)
{
    mTransitionSet = transitionSet;
}

ECode TransitionSet::TransitionSetListener::OnTransitionStart(
    /* [in] */ ITransition* transition)
{
    AutoPtr<TransitionSet> cts = (TransitionSet*)mTransitionSet.Get();
    if (!cts->mStarted) {
        ITransition::Probe(mTransitionSet)->Start();
        cts->mStarted = TRUE;
    }
    return NOERROR;
}

ECode TransitionSet::TransitionSetListener::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    AutoPtr<TransitionSet> cts = (TransitionSet*)mTransitionSet.Get();
    --cts->mCurrentListeners;
    if (cts->mCurrentListeners == 0) {
        // All child trans
        cts->mStarted = FALSE;
        ITransition::Probe(mTransitionSet)->End();
    }

    return transition->RemoveListener(this);
}

//===============================================================
// TransitionSet::TransitionListenerAdapter_1::
//===============================================================
TransitionSet::TransitionListenerAdapter_1::TransitionListenerAdapter_1(
    /* [in] */ ITransition* nxtT)
{
    mNextTransition = nxtT;
}

ECode TransitionSet::TransitionListenerAdapter_1::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    mNextTransition->RunAnimators();
    return transition->RemoveListener(this);
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
