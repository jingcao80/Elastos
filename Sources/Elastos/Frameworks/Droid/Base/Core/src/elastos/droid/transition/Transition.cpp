
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/transition/Transition.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/transition/TransitionValuesMaps.h"
#include "elastos/droid/transition/TransitionValuesMaps.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/utility/CArrayMap.h"
//#include "elastos/droid/utility/CSparseInt64Array.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/R.h"

#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::ISparseInt64Array;
//using Elastos::Droid::Utility::CSparseInt64Array;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IAdapterView;

using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CStringTokenizer;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IMap;
using Elastos::Utility::IStringTokenizer;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// Transition::
//===============================================================
const String Transition::TAG("Transition");
Boolean Transition::DBG = FALSE;

const Int32 Transition::MATCH_INSTANCE = 0x1;
const Int32 Transition::MATCH_FIRST = MATCH_INSTANCE;
const Int32 Transition::MATCH_NAME = 0x2;
const Int32 Transition::MATCH_ID = 0x3;
const Int32 Transition::MATCH_ITEM_ID = 0x4;
const Int32 Transition::MATCH_LAST = MATCH_ITEM_ID;

const String Transition::MATCH_INSTANCE_STR("instance");
const String Transition::MATCH_NAME_STR("name");

const String Transition::MATCH_VIEW_NAME_STR("viewName");
const String Transition::MATCH_ID_STR("id");
const String Transition::MATCH_ITEM_ID_STR("itemId");

static AutoPtr< ArrayOf<Int32> > InitDEFAULT_MATCH_ORDER()
{
    AutoPtr< ArrayOf<Int32> > order =  ArrayOf<Int32>::Alloc(4);
    (*order)[0] = Transition::MATCH_NAME;
    (*order)[1] = Transition::MATCH_INSTANCE;
    (*order)[2] = Transition::MATCH_ID;
    (*order)[3] = Transition::MATCH_ITEM_ID;
    return order;
}

AutoPtr<ArrayOf<Int32> > Transition::DEFAULT_MATCH_ORDER = InitDEFAULT_MATCH_ORDER();

AutoPtr<IPathMotion> Transition::STRAIGHT_PATH_MOTION = new PathMotionOverride();

// Per-animator information used for later canceling when future transitions overlap
//AutoPtr<IThreadLocal> Transition::sRunningAnimators = new ThreadLocal();

CAR_INTERFACE_IMPL_2(Transition, Object, ITransition, ICloneable)

Transition::Transition()
    : mStartDelay(-1)
    , mDuration(-1)
    , mCanRemoveViews(FALSE)
    , mNumInstances(0)
    , mPaused(FALSE)
    , mEnded(FALSE)
{
    CArrayList::New((IArrayList**)&mTargetIds);
    CArrayList::New((IArrayList**)&mTargets);
    CArrayList::New((IArrayList**)&mCurrentAnimators);
    CArrayList::New((IArrayList**)&mAnimators);
    mStartValues = new TransitionValuesMaps();
    mEndValues = new TransitionValuesMaps();
    mPathMotion = STRAIGHT_PATH_MOTION;
}

ECode Transition::constructor()
{
    return NOERROR;
}

ECode Transition::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Transition);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    Int32 duration = 0;
    a->GetInt32(R::styleable::Transition_duration, -1, &duration);
    if (duration >= 0) {
        SetDuration((Int64)duration);
    }
    Int32 startDelay = 0;
    a->GetInt32(R::styleable::Transition_startDelay, -1, &startDelay);
    if (startDelay > 0) {
        SetStartDelay((Int64)startDelay);
    }
    Int32 resID = 0;
    a->GetResourceId(R::styleable::Animator_interpolator, 0, &resID);
    if (resID > 0) {
        AutoPtr<IInterpolator> pol;
        AnimationUtils::LoadInterpolator(context, resID, (IInterpolator**)&pol);
        SetInterpolator(ITimeInterpolator::Probe(pol));
    }
    String matchOrder;
    a->GetString(R::styleable::Transition_matchOrder, &matchOrder);
    if (matchOrder != NULL) {
        SetMatchOrder(ParseMatchOrder(matchOrder));
    }
    a->Recycle();
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > Transition::ParseMatchOrder(
    /* [in] */ const String& matchOrderString)
{
    AutoPtr<IStringTokenizer> st;
    CStringTokenizer::New(matchOrderString, String(","), (IStringTokenizer**)&st);
    Int32 tok = 0;
    st->CountTokens(&tok);
    AutoPtr<ArrayOf<Int32> > matches = ArrayOf<Int32>::Alloc(tok);
    Int32 index = 0;
    Boolean bHasMore = FALSE;
    while ((st->HasMoreTokens(&bHasMore), bHasMore)) {
        String token;
        st->GetNextToken(&token);
        token.Trim();
        if (MATCH_ID_STR.EqualsIgnoreCase(token)) {
            (*matches)[index] = Transition::MATCH_ID;
        }
        else if (MATCH_INSTANCE_STR.EqualsIgnoreCase(token)) {
            (*matches)[index] = Transition::MATCH_INSTANCE;
        }
        else if (MATCH_NAME_STR.EqualsIgnoreCase(token)) {
            (*matches)[index] = Transition::MATCH_NAME;
        }
        else if (MATCH_VIEW_NAME_STR.EqualsIgnoreCase(token)) {
            (*matches)[index] = Transition::MATCH_NAME;
        }
        else if (MATCH_ITEM_ID_STR.EqualsIgnoreCase(token)) {
            (*matches)[index] = Transition::MATCH_ITEM_ID;
        }
        else if (token.IsEmpty()) {
            AutoPtr<ArrayOf<Int32> > smallerMatches = ArrayOf<Int32>::Alloc(matches->GetLength() - 1);
            smallerMatches->Copy(0, matches, 0, index);
            matches = smallerMatches;
            index--;
        }
        else {
            return NULL;
        //     throw new InflateException("Unknown match type in matchOrder: '" + token + "'");
        }
        index++;
    }
    return matches;
}

ECode Transition::SetDuration(
    /* [in] */ Int64 duration)
{
    mDuration = duration;
    return NOERROR;
}

ECode Transition::GetDuration(
    /* [out] */ Int64* dura)
{
    VALIDATE_NOT_NULL(dura)

    *dura = mDuration;
    return NOERROR;
}

ECode Transition::SetStartDelay(
    /* [in] */ Int64 startDelay)
{
    mStartDelay = startDelay;
    return NOERROR;
}

ECode Transition::GetStartDelay(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStartDelay;
    return NOERROR;
}

ECode Transition::SetInterpolator(
    /* [in] */ ITimeInterpolator* interpolator)
{
    mInterpolator = interpolator;
    return NOERROR;
}

ECode Transition::GetInterpolator(
    /* [out] */ ITimeInterpolator** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mInterpolator;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::GetTransitionProperties(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode Transition::CreateAnimator(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode Transition::SetMatchOrder(
    /* [in] */ ArrayOf<Int32>* matches)
{
    if (matches == NULL || matches->GetLength() == 0) {
        mMatchOrder = DEFAULT_MATCH_ORDER;
    }
    else {
        for (Int32 i = 0; i < matches->GetLength(); i++) {
            Int32 match = (*matches)[i];
            if (!IsValidMatch(match)) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            if (AlreadyContains(matches, i)) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        mMatchOrder = matches->Clone();
    }
    return NOERROR;
}

Boolean Transition::IsValidMatch(
    /* [in] */ Int32 match)
{
    return (match >= MATCH_FIRST && match <= MATCH_LAST);
}

Boolean Transition::AlreadyContains(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 searchIndex)
{
    Int32 value = (*array)[searchIndex];
    for (Int32 i = 0; i < searchIndex; i++) {
        if ((*array)[i] == value) {
            return TRUE;
        }
    }
    return FALSE;
}

void Transition::MatchInstances(
    /* [in] */ IArrayMap* unmatchedStart,
    /* [in] */ IArrayMap* unmatchedEnd)
{
    Int32 i = 0;
    IMap::Probe(unmatchedStart)->GetSize(&i);
    i = i - 1;
    for (; i >= 0; i--) {
        AutoPtr<IInterface> v;
        unmatchedStart->GetKeyAt(i, (IInterface**)&v);
        AutoPtr<IView> view = IView::Probe(v);
        AutoPtr<IInterface> e;
        IMap::Probe(unmatchedEnd)->Remove(view, (IInterface**)&e);
        AutoPtr<ITransitionValues> end = ITransitionValues::Probe(e);
        if (end != NULL) {
            AutoPtr<IInterface> s;
            unmatchedStart->RemoveAt(i, (IInterface**)&s);
            AutoPtr<ITransitionValues> start = ITransitionValues::Probe(s);
            mStartValuesList->Add(start);
            mEndValuesList->Add(end);
        }
    }
}

void Transition::MatchItemIds(
    /* [in] */ IArrayMap* unmatchedStart,
    /* [in] */ IArrayMap* unmatchedEnd,
    /* [in] */ IInt64SparseArray* startItemIds,
    /* [in] */ IInt64SparseArray* endItemIds)
{
    Int32 numStartIds = 0;
    startItemIds->GetSize(&numStartIds);
    for (Int32 i = 0; i < numStartIds; i++) {
        AutoPtr<IInterface> sv;
        startItemIds->ValueAt(i, (IInterface**)&sv);
        AutoPtr<IView> startView = IView::Probe(sv);
        if (startView != NULL) {
            Int64 k = 0;
            startItemIds->KeyAt(i, &k);
            AutoPtr<IInterface> ev;
            endItemIds->Get(k, (IInterface**)&ev);
            AutoPtr<IView> endView = IView::Probe(ev);
            if (endView != NULL) {
                AutoPtr<IInterface> sV;
                IMap::Probe(unmatchedStart)->Get(startView, (IInterface**)&sV);
                AutoPtr<ITransitionValues> startValues = ITransitionValues::Probe(sV);
                AutoPtr<IInterface> eV;
                IMap::Probe(unmatchedEnd)->Get(endView, (IInterface**)&eV);
                AutoPtr<ITransitionValues> endValues = ITransitionValues::Probe(eV);
                if (startValues != NULL && endValues != NULL) {
                    mStartValuesList->Add(startValues);
                    mEndValuesList->Add(endValues);
                    IMap::Probe(unmatchedStart)->Remove(startView);
                    IMap::Probe(unmatchedEnd)->Remove(endView);
                }
            }
        }
    }
}

void Transition::MatchIds(
    /* [in] */ IArrayMap* unmatchedStart,
    /* [in] */ IArrayMap* unmatchedEnd,
    /* [in] */ ISparseArray* startIds,
    /* [in] */ ISparseArray* endIds)
{
    Int32 numStartIds = 0;
    startIds->GetSize(&numStartIds);
    for (Int32 i = 0; i < numStartIds; i++) {
        AutoPtr<IInterface> sv;
        startIds->ValueAt(i, (IInterface**)&sv);
        AutoPtr<IView> startView = IView::Probe(sv);
        Boolean bSV = FALSE;
        if (startView != NULL && (IsValidTarget(startView, &bSV), bSV)) {
            Int32 k = 0;
            startIds->KeyAt(i, &k);
            AutoPtr<IInterface> ev;
            endIds->Get(k, (IInterface**)&ev);
            AutoPtr<IView> endView = IView::Probe(ev);
            Boolean bEV = FALSE;
            if (endView != NULL && (IsValidTarget(endView, &bEV), bEV)) {
                AutoPtr<IInterface> sV;
                IMap::Probe(unmatchedStart)->Get(startView, (IInterface**)&sV);
                AutoPtr<ITransitionValues> startValues = ITransitionValues::Probe(sV);
                AutoPtr<IInterface> eV;
                IMap::Probe(unmatchedEnd)->Get(endView, (IInterface**)&eV);
                AutoPtr<ITransitionValues> endValues = ITransitionValues::Probe(eV);
                if (startValues != NULL && endValues != NULL) {
                    mStartValuesList->Add(startValues);
                    mEndValuesList->Add(endValues);
                    IMap::Probe(unmatchedStart)->Remove(startView);
                    IMap::Probe(unmatchedEnd)->Remove(endView);
                }
            }
        }
    }
}

void Transition::MatchNames(
    /* [in] */ IArrayMap* unmatchedStart,
    /* [in] */ IArrayMap* unmatchedEnd,
    /* [in] */ IArrayMap* startNames,
    /* [in] */ IArrayMap* endNames)
{
    Int32 numStartNames = 0;
    IMap::Probe(startNames)->GetSize(&numStartNames);
    for (Int32 i = 0; i < numStartNames; i++) {
        AutoPtr<IInterface> sV;
        startNames->GetValueAt(i, (IInterface**)&sV);
        AutoPtr<IView> startView = IView::Probe(sV);
        Boolean bSV = FALSE;
        if (startView != NULL && (IsValidTarget(startView, &bSV), bSV)) {
            AutoPtr<IInterface> k;
            startNames->GetKeyAt(i, (IInterface**)&k);
            AutoPtr<IInterface> eV;
            IMap::Probe(endNames)->Get(k, (IInterface**)&eV);
            AutoPtr<IView> endView = IView::Probe(eV);
            Boolean bValid = FALSE;
            if (endView != NULL && (IsValidTarget(endView, &bValid), bValid)) {
                AutoPtr<IInterface> sV;
                IMap::Probe(unmatchedStart)->Get(startView, (IInterface**)&sV);
                AutoPtr<ITransitionValues> startValues = ITransitionValues::Probe(sV);
                AutoPtr<IInterface> eV;
                IMap::Probe(unmatchedEnd)->Get(endView, (IInterface**)&eV);
                AutoPtr<ITransitionValues> endValues = ITransitionValues::Probe(eV);
                if (startValues != NULL && endValues != NULL) {
                    mStartValuesList->Add(startValues);
                    mEndValuesList->Add(endValues);
                    IMap::Probe(unmatchedStart)->Remove(startView);
                    IMap::Probe(unmatchedEnd)->Remove(endView);
                }
            }
        }
    }
}

void Transition::AddUnmatched(
    /* [in] */ IArrayMap* unmatchedStart,
    /* [in] */ IArrayMap* unmatchedEnd)
{
    // Views that only exist in the start Scene
    Int32 sStart = 0;
    IMap::Probe(unmatchedStart)->GetSize(&sStart);
    for (Int32 i = 0; i < sStart; i++) {
        AutoPtr<IInterface> v;
        unmatchedStart->GetValueAt(i, (IInterface**)&v);
        mStartValuesList->Add(v);
        mEndValuesList->Add(NULL);
    }

    // Views that only exist in the end Scene
    Int32 sEnd = 0;
    IMap::Probe(unmatchedEnd)->GetSize(&sEnd);
    for (Int32 i = 0; i < sEnd; i++) {
        AutoPtr<IInterface> v;
        unmatchedStart->GetValueAt(i, (IInterface**)&v);
        mEndValuesList->Add(v);
        mStartValuesList->Add(NULL);
    }
}

void Transition::MatchStartAndEnd(
    /* [in] */ ITransitionValuesMaps* startValues,
    /* [in] */ ITransitionValuesMaps* endValues)
{
    AutoPtr<TransitionValuesMaps> cStart = (TransitionValuesMaps*)startValues;
    AutoPtr<TransitionValuesMaps> cEnd = (TransitionValuesMaps*)endValues;
    AutoPtr<IArrayMap> unmatchedStart;
    CArrayMap::New(cStart->mViewValues, (IArrayMap**)&unmatchedStart);
    AutoPtr<IArrayMap> unmatchedEnd;
    CArrayMap::New(cEnd->mViewValues, (IArrayMap**)&unmatchedEnd);

    for (Int32 i = 0; i < mMatchOrder->GetLength(); i++) {
        switch ((*mMatchOrder)[i]) {
            case MATCH_INSTANCE:
                MatchInstances(unmatchedStart, unmatchedEnd);
                break;
            case MATCH_NAME:
                MatchNames(unmatchedStart, unmatchedEnd,
                        cStart->mNameValues, cEnd->mNameValues);
                break;
            case MATCH_ID:
                MatchIds(unmatchedStart, unmatchedEnd,
                        cStart->mIdValues, cEnd->mIdValues);
                break;
            case MATCH_ITEM_ID:
                MatchItemIds(unmatchedStart, unmatchedEnd,
                        cStart->mItemIdValues, cEnd->mItemIdValues);
                break;
        }
    }
    AddUnmatched(unmatchedStart, unmatchedEnd);
}

ECode Transition::CreateAnimators(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValuesMaps* startValues,
    /* [in] */ ITransitionValuesMaps* endValues,
    /* [in] */ IArrayList* startValuesList,
    /* [in] */ IArrayList* endValuesList)
{
    if (DBG) {
        Logger::D(LOG_TAG, "createAnimators() for %p", this);
    }
    AutoPtr<IArrayMap> runningAnimators = GetRunningAnimators();
    Int64 minStartDelay = Elastos::Core::Math::INT64_MAX_VALUE;
    Int32 minAnimator = 0;
    mAnimators->GetSize(&minAnimator);
    AutoPtr<ISparseInt64Array> startDelays;
    assert(0 && "TODO");
//    CSparseInt64Array::New((ISparseInt64Array**)&startDelays);
    Int32 startValuesListCount = 0;
    startValuesList->GetSize(&startValuesListCount);
    for (Int32 i = 0; i < startValuesListCount; ++i) {
        AutoPtr<IInterface> sT;
        startValuesList->Get(i, (IInterface**)&sT);
        AutoPtr<ITransitionValues> start = ITransitionValues::Probe(sT);
        AutoPtr<CTransitionValues> cStart = (CTransitionValues*)start.Get();
        AutoPtr<IInterface> eT;
        endValuesList->Get(i, (IInterface**)&eT);
        AutoPtr<ITransitionValues> end = ITransitionValues::Probe(eT);
        AutoPtr<CTransitionValues> cEnd = (CTransitionValues*)end.Get();
        Boolean bStartContain = FALSE, bEndContain = FALSE;
        if (start != NULL &&
            !(cStart->mTargetedTransitions->Contains(TO_IINTERFACE(this), &bStartContain), bStartContain)) {
            start = NULL;
        }
        if (end != NULL &&
            !(cEnd->mTargetedTransitions->Contains(TO_IINTERFACE(this), &bEndContain), bEndContain)) {
            end = NULL;
        }
        if (start == NULL && end == NULL) {
            continue;
        }
        // Only bother trying to animate with values that differ between start/end
        Boolean bValChange = FALSE;
        Boolean isChanged = start == NULL || end == NULL || (AreValuesChanged(start, end, &bValChange), bValChange);
        if (isChanged) {
            // if (DBG) {
            //     AutoPtr<IView> view = (end != NULL) ? end->mView : start->mView;
            //     Log.d(LOG_TAG, "  differing start/end values for view " + view);
            //     if (start == NULL || end == NULL) {
            //         Log.d(LOG_TAG, "    " + ((start == NULL) ?
            //                 "start null, end non-null" : "start non-null, end null"));
            //     }
            //     else {
            //         for (String key : start.values->KeySet()) {
            //             AutoPtr<IInterface> startValue = start.values->Get(key);
            //             AutoPtr<IInterface> endValue = end.values->Get(key);
            //             if (startValue != endValue && !startValue->Equals(endValue)) {
            //                 Log.d(LOG_TAG, "    " + key + ": start(" + startValue +
            //                         "), end(" + endValue + ")");
            //             }
            //         }
            //     }
            // }
            // TODO: what to do about targetIds and itemIds?
            AutoPtr<IAnimator> animator;
            CreateAnimator(sceneRoot, start, end, (IAnimator**)&animator);
            if (animator != NULL) {
                // Save animation info for future cancellation purposes
                AutoPtr<IView> view;
                AutoPtr<ITransitionValues> infoValues;
                if (end != NULL) {
                    view = cEnd->mView;
                    AutoPtr<ArrayOf<String> > properties;
                    GetTransitionProperties((ArrayOf<String>**)&properties);
                    if (view != NULL && properties != NULL && properties->GetLength() > 0) {
                        CTransitionValues::New((ITransitionValues**)&infoValues);
                        AutoPtr<CTransitionValues> cInfo = (CTransitionValues*)infoValues.Get();
                        cInfo->mView = view;
                        AutoPtr<TransitionValuesMaps> cEnd = (TransitionValuesMaps*)endValues;
                        AutoPtr<IInterface> val;
                        IMap::Probe(cEnd->mViewValues)->Get(view, (IInterface**)&val);
                        AutoPtr<ITransitionValues> newValues = ITransitionValues::Probe(val);
                        if (newValues != NULL) {
                            for (Int32 j = 0; j < properties->GetLength(); ++j) {
                                AutoPtr<CTransitionValues> cNewVal = (CTransitionValues*)newValues.Get();
                                AutoPtr<ICharSequence> pro;
                                CString::New((*properties)[j], (ICharSequence**)&pro);
                                AutoPtr<IInterface> p;
                                cNewVal->mValues->Get(pro, (IInterface**)&p);
                                cInfo->mValues->Put(pro, p);
                            }
                        }
                        Int32 numExistingAnims = 0;
                        IMap::Probe(runningAnimators)->GetSize(&numExistingAnims);
                        for (Int32 j = 0; j < numExistingAnims; ++j) {
                            AutoPtr<IInterface> k;
                            runningAnimators->GetKeyAt(j, (IInterface**)&k);
                            AutoPtr<IAnimator> anim = IAnimator::Probe(k);
                            AutoPtr<IInterface> v;
                            IMap::Probe(runningAnimators)->Get(anim, (IInterface**)&v);
                            AutoPtr<AnimationInfo> info = (AnimationInfo*)(IObject*)v.Get();
                            if (info->mValues != NULL && info->mView == view) {
                                String name;
                                GetName(&name);
                                if ((info->mName == NULL && name.IsEmpty()) ||
                                                info->mName.Equals(name)) {
                                    if (Object::Equals(info->mValues->Probe(EIID_IInterface), infoValues->Probe(EIID_IInterface))) {
                                        // Favor the old animator
                                        animator = NULL;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    view = (start != NULL) ? cStart->mView : NULL;
                }
                if (animator != NULL) {
                    if (mPropagation != NULL) {
                        Int64 delay = 0;
                        mPropagation->GetStartDelay(sceneRoot, this, start, end, &delay);
                        Int32 size = 0;
                        mAnimators->GetSize(&size);
                        startDelays->Put(size, delay);
                        minStartDelay = Elastos::Core::Math::Min(delay, minStartDelay);
                    }
                    String name;
                    GetName(&name);
                    AutoPtr<IWindowId> id;
                    IView::Probe(sceneRoot)->GetWindowId((IWindowId**)&id);
                    AutoPtr<AnimationInfo> info = new AnimationInfo(view, name, this,
                            id, infoValues);
                    IMap::Probe(runningAnimators)->Put(animator, info->Probe(EIID_IInterface));
                    mAnimators->Add(animator);
                }
            }
        }
    }
    if (minStartDelay != 0) {
        Int32 s = 0;
        startDelays->GetSize(&s);
        for (Int32 i = 0; i < s; i++) {
            Int32 index = 0;
            startDelays->KeyAt(i, &index);
            AutoPtr<IInterface> anim;
            mAnimators->Get(index, (IInterface**)&anim);
            AutoPtr<IAnimator> animator = IAnimator::Probe(anim);
            Int64 vl = 0;
            startDelays->ValueAt(i, &vl);
            Int64 d = 0;
            animator->GetStartDelay(&d);
            Int64 delay = vl - minStartDelay + d;
            animator->SetStartDelay(delay);
        }
    }
    return NOERROR;
}

ECode Transition::IsValidTarget(
    /* [in] */ IView* target,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 targetId = 0;
    target->GetId(&targetId);
    AutoPtr<IInteger32> pID;
    CInteger32::New(targetId, (IInteger32**)&pID);
    Boolean bContains = FALSE;
    if (mTargetIdExcludes != NULL && (mTargetIdExcludes->Contains(pID, &bContains), bContains)) {
        *result = FALSE;
        return NOERROR;
    }
    if (mTargetExcludes != NULL && (mTargetExcludes->Contains(target, &bContains), bContains)) {
        *result = FALSE;
        return NOERROR;
    }
    if (mTargetTypeExcludes != NULL && target != NULL) {
        Int32 numTypes = 0;
        mTargetTypeExcludes->GetSize(&numTypes);
        for (Int32 i = 0; i < numTypes; ++i) {
            assert(0 && "TODO");
            // Class type = mTargetTypeExcludes.get(i);
            // if (type->IsInstance(target)) {
            //     *result = FALSE;
            //     return NOERROR;
            // }
        }
    }
    String name;
    target->GetTransitionName(&name);
    if (mTargetNameExcludes != NULL && target != NULL && !name.IsNull()) {
        AutoPtr<ICharSequence> cName;
        CString::New(name, (ICharSequence**)&cName);
        Boolean bContainName = FALSE;
        if ((mTargetNameExcludes->Contains(cName, &bContainName), bContainName)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    Int32 idSize = 0, tSize = 0;
    mTargetIds->GetSize(&idSize);
    mTargets->GetSize(&tSize);
    Boolean bType = FALSE, bName = FALSE;
    if (idSize == 0 && tSize == 0 &&
            (mTargetTypes == NULL || (mTargetTypes->IsEmpty(&bType), bType)) &&
            (mTargetNames == NULL || (mTargetNames->IsEmpty(&bName), bName))) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean bContainsId = FALSE, bContainsT = FALSE;
    if ((mTargetIds->Contains(pID, &bContainsId), bContainsId) ||
        (mTargets->Contains(target, &bContainsT), bContainsT)) {
        *result = TRUE;
        return NOERROR;
    }
    Boolean bContainsName = FALSE;
    AutoPtr<ICharSequence> cName;
    CString::New(name, (ICharSequence**)&cName);
    if (mTargetNames != NULL && (mTargetNames->Contains(cName, &bContainsName), bContainsName)) {
        *result = TRUE;
        return NOERROR;
    }
    if (mTargetTypes != NULL) {
        Int32 size = 0;
        mTargetTypes->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> type;
            mTargetTypes->Get(i, (IInterface**)&type);
            assert(0 && "TODO");
            // if (type->IsInstance(target)) {
            //     *result = TRUE;
            //     return NOERROR;
            // }
        }
    }
    *result = FALSE;
    return NOERROR;
}

AutoPtr<IArrayMap> Transition::GetRunningAnimators()
{
    AutoPtr<IArrayMap> runningAnimators;// = sRunningAnimators->Get();
    // if (runningAnimators == NULL) {
    //     CArrayMap::New((IArrayMap**)&runningAnimators);
    //     sRunningAnimators->Set(runningAnimators);
    // }
    return runningAnimators;
}

ECode Transition::RunAnimators()
{
    if (DBG) {
        Logger::D(LOG_TAG, "runAnimators() on %p", this);
    }
    Start();
    AutoPtr<IArrayMap> runningAnimators = GetRunningAnimators();
    // Now start every Animator that was previously created for this transition
    AutoPtr<IIterator> it;
    mAnimators->GetIterator((IIterator**)&it);
    Boolean b = FALSE;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IAnimator> anim = IAnimator::Probe(p);
        if (DBG) {
            Logger::D(LOG_TAG, "  anim: %p", anim.Get());
        }
        Boolean bContainsKey = FALSE;
        if ((IMap::Probe(runningAnimators)->ContainsKey(anim, &bContainsKey), bContainsKey)) {
            Start();
            RunAnimator(anim, runningAnimators);
        }
    }
    mAnimators->Clear();
    End();
    return NOERROR;
}

void Transition::RunAnimator(
    /* [in] */ IAnimator* animator,
    /* [in] */ IArrayMap* runningAnimators)
{
    if (animator != NULL) {
        // TODO: could be a single listener instance for all of them since it uses the param
        AutoPtr<AnimatorListenerAdapter_1> p = new AnimatorListenerAdapter_1(mCurrentAnimators, runningAnimators);
        animator->AddListener(IAnimatorListener::Probe(p));
        Animate(animator);
    }
}

ECode Transition::AddTarget(
    /* [in] */ Int32 targetId)
{
    if (targetId > 0) {
        AutoPtr<IInteger32> pID;
        CInteger32::New(targetId, (IInteger32**)&pID);
        mTargetIds->Add(pID);
    }
    return NOERROR;
}

ECode Transition::AddTarget(
    /* [in] */ const String& targetName)
{
    if (targetName != NULL) {
        if (mTargetNames == NULL) {
            CArrayList::New((IArrayList**)&mTargetNames);
        }
        AutoPtr<ICharSequence> cName;
        CString::New(targetName, (ICharSequence**)&cName);
        mTargetNames->Add(cName);
    }
    return NOERROR;
}

// public Transition addTarget(Class targetType)
// {
//     if (targetType != null) {
//         if (mTargetTypes == null) {
//             mTargetTypes = new ArrayList<Class>();
//         }
//         mTargetTypes.add(targetType);
//     }
//     return this;
// }

ECode Transition::RemoveTarget(
    /* [in] */ Int32 targetId)
{
    if (targetId > 0) {
        mTargetIds->Remove(targetId);
    }
    return NOERROR;
}

ECode Transition::RemoveTarget(
    /* [in] */ const String& targetName)
{
    if (!targetName.IsNull() && mTargetNames != NULL) {
        AutoPtr<ICharSequence> cName;
        CString::New(targetName, (ICharSequence**)&cName);
        mTargetNames->Remove(cName);
    }
    return NOERROR;
}

ECode Transition::ExcludeTarget(
    /* [in] */ Int32 targetId,
    /* [in] */ Boolean exclude)
{
    if (targetId >= 0) {
        AutoPtr<IInteger32> pID;
        CInteger32::New(targetId, (IInteger32**)&pID);
        mTargetIdExcludes = ExcludeObject(mTargetIdExcludes, pID, exclude);
    }
    return NOERROR;
}

ECode Transition::ExcludeTarget(
    /* [in] */ const String& targetName,
    /* [in] */ Boolean exclude)
{
    AutoPtr<ICharSequence> cName;
    CString::New(targetName, (ICharSequence**)&cName);
    mTargetNameExcludes = ExcludeObject(mTargetNameExcludes, cName, exclude);
    return NOERROR;
}

ECode Transition::ExcludeChildren(
    /* [in] */ Int32 targetId,
    /* [in] */ Boolean exclude)
{
    if (targetId >= 0) {
        AutoPtr<IInteger32> pID;
        CInteger32::New(targetId, (IInteger32**)&pID);
        mTargetIdChildExcludes = ExcludeObject(mTargetIdChildExcludes, pID, exclude);
    }
    return NOERROR;
}

ECode Transition::ExcludeTarget(
    /* [in] */ IView* target,
    /* [in] */ Boolean exclude)
{
    mTargetExcludes = ExcludeObject(mTargetExcludes, target, exclude);
    return NOERROR;
}

ECode Transition::ExcludeChildren(
    /* [in] */ IView* target,
    /* [in] */ Boolean exclude)
{
    mTargetChildExcludes = ExcludeObject(mTargetChildExcludes, target, exclude);
    return NOERROR;
}

AutoPtr<IArrayList> Transition::ExcludeObject(
    /* [in] */ IArrayList* list,
    /* [in] */ IInterface* target,
    /* [in] */ Boolean exclude)
{
    if (target != NULL) {
        if (exclude) {
            list = ArrayListManager::Add(list, target);
        }
        else {
            list = ArrayListManager::Remove(list, target);
        }
    }
    return list;
}

// public Transition excludeTarget(Class type, boolean exclude)
// {
//     mTargetTypeExcludes = excludeObject(mTargetTypeExcludes, type, exclude);
//     return this;
// }

// public Transition excludeChildren(Class type, boolean exclude)
// {
//     mTargetTypeChildExcludes = excludeObject(mTargetTypeChildExcludes, type, exclude);
//     return this;
// }

ECode Transition::AddTarget(
    /* [in] */ IView* target)
{
    mTargets->Add(target);
    return NOERROR;
}

ECode Transition::RemoveTarget(
    /* [in] */ IView* target)
{
    if (target != NULL) {
        mTargets->Remove(target);
    }
    return NOERROR;
}

// public Transition removeTarget(Class target)
// {
//     if (target != null) {
//         mTargetTypes.remove(target);
//     }
//     return this;
// }

ECode Transition::GetTargetIds(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IList::Probe(mTargetIds);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::GetTargets(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IList::Probe(mTargets);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::GetTargetNames(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IList::Probe(mTargetNames);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::GetTargetViewNames(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IList::Probe(mTargetNames);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::GetTargetTypes(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IList::Probe(mTargetTypes);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::CaptureValues(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ Boolean start)
{
    ClearValues(start);
    Int32 sizeID = 0, sizeT = 0;
    mTargetIds->GetSize(&sizeID);
    mTargets->GetSize(&sizeT);
    Boolean bName = FALSE, bType = FALSE;
    if ((sizeID > 0 || sizeT > 0)
            && (mTargetNames == NULL || (mTargetNames->IsEmpty(&bName), bName))
            && (mTargetTypes == NULL || (mTargetTypes->IsEmpty(&bType), bType))) {
        for (Int32 i = 0; i < sizeID; ++i) {
            AutoPtr<IInterface> pId;
            mTargetIds->Get(i, (IInterface**)&pId);
            AutoPtr<IInteger32> p = IInteger32::Probe(pId);
            Int32 id = 0;
            p->GetValue(&id);
            AutoPtr<IView> view;
            IView::Probe(sceneRoot)->FindViewById(id, (IView**)&view);
            if (view != NULL) {
                AutoPtr<ITransitionValues> values;
                CTransitionValues::New((ITransitionValues**)&values);
                AutoPtr<CTransitionValues> cv = (CTransitionValues*)values.Get();
                cv->mView = view;
                if (start) {
                    CaptureStartValues(values);
                }
                else {
                    CaptureEndValues(values);
                }
                cv->mTargetedTransitions->Add(TO_IINTERFACE(this));
                CapturePropagationValues(values);
                if (start) {
                    AddViewValues(mStartValues, view, values);
                }
                else {
                    AddViewValues(mEndValues, view, values);
                }
            }
        }
        Int32 s = 0;
        mTargets->GetSize(&s);
        for (Int32 i = 0; i < s; ++i) {
            AutoPtr<IInterface> node;
            mTargets->Get(i, (IInterface**)&node);
            AutoPtr<IView> view = IView::Probe(node);
            AutoPtr<ITransitionValues> values;
            CTransitionValues::New((ITransitionValues**)&values);
            AutoPtr<CTransitionValues> cv = (CTransitionValues*)values.Get();
            cv->mView = view;
            if (start) {
                CaptureStartValues(values);
            }
            else {
                CaptureEndValues(values);
            }
            cv->mTargetedTransitions->Add(TO_IINTERFACE(this));
            CapturePropagationValues(values);
            if (start) {
                AddViewValues(mStartValues, view, values);
            }
            else {
                AddViewValues(mEndValues, view, values);
            }
        }
    }
    else {
        CaptureHierarchy(IView::Probe(sceneRoot), start);
    }
    if (!start && mNameOverrides != NULL) {
        Int32 numOverrides = 0;
        IMap::Probe(mNameOverrides)->GetSize(&numOverrides);
        AutoPtr<IArrayList> overriddenViews;
        CArrayList::New(numOverrides, (IArrayList**)&overriddenViews);
        for (Int32 i = 0; i < numOverrides; i++) {
            AutoPtr<IInterface> fromname;
            mNameOverrides->GetKeyAt(i, (IInterface**)&fromname);
            AutoPtr<TransitionValuesMaps> cSV = (TransitionValuesMaps*)mStartValues.Get();
            AutoPtr<IInterface> vls;
            IMap::Probe(cSV->mNameValues)->Remove(fromname, (IInterface**)&vls);
            overriddenViews->Add(vls);
        }
        for (Int32 i = 0; i < numOverrides; i++) {
            AutoPtr<IInterface> v;
            overriddenViews->Get(i, (IInterface**)&v);
            AutoPtr<IView> view = IView::Probe(v);
            if (view != NULL) {
                AutoPtr<IInterface> vl;
                mNameOverrides->GetValueAt(i, (IInterface**)&vl);
                AutoPtr<TransitionValuesMaps> m = (TransitionValuesMaps*)mStartValues.Get();
                IMap::Probe(m->mNameValues)->Put(vl, view);
            }
        }
    }
    return NOERROR;
}

void Transition::AddViewValues(
    /* [in] */ ITransitionValuesMaps* transitionValuesMaps,
    /* [in] */ IView* view,
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<TransitionValuesMaps> p = (TransitionValuesMaps*)transitionValuesMaps;
    IMap::Probe(p->mViewValues)->Put(view, transitionValues);
    Int32 id = 0;
    view->GetId(&id);
    if (id >= 0) {
        Int32 index = 0;
        if ((p->mIdValues->IndexOfKey(id, &index), index) >= 0) {
            // Duplicate IDs cannot match by ID.
            p->mIdValues->Put(id, NULL);
        }
        else {
            p->mIdValues->Put(id, view);
        }
    }
    String name;
    view->GetTransitionName(&name);
    AutoPtr<ICharSequence> cName;
    CString::New(name, (ICharSequence**)&cName);
    if (cName != NULL) {
        Boolean bContainsKey = FALSE;
        if ((IMap::Probe(p->mNameValues)->ContainsKey(cName, &bContainsKey), bContainsKey)) {
            // Duplicate transitionNames: cannot match by transitionName.
            IMap::Probe(p->mNameValues)->Put(cName, NULL);
        }
        else {
            IMap::Probe(p->mNameValues)->Put(cName, view);
        }
    }
    AutoPtr<IViewParent> vp;
    view->GetParent((IViewParent**)&vp);
    if (IListView::Probe(vp) != NULL) {
        AutoPtr<IListView> listview = IListView::Probe(vp);
        AutoPtr<IAdapter> adp;
        IAdapterView::Probe(listview)->GetAdapter((IAdapter**)&adp);
        Boolean hasStableIds = FALSE;
        if ((adp->HasStableIds(&hasStableIds), hasStableIds)) {
            Int32 position = 0;
            IAdapterView::Probe(listview)->GetPositionForView(view, &position);
            Int64 itemId = 0;
            IAdapterView::Probe(listview)->GetItemIdAtPosition(position, &itemId);
            Int32 idx = 0;
            p->mItemIdValues->IndexOfKey(itemId, &idx);
            if (idx >= 0) {
                // Duplicate item IDs: cannot match by item ID.
                AutoPtr<IInterface> aM;
                p->mItemIdValues->Get(itemId, (IInterface**)&aM);
                AutoPtr<IView> alreadyMatched = IView::Probe(aM);
                if (alreadyMatched != NULL) {
                    alreadyMatched->SetHasTransientState(FALSE);
                    p->mItemIdValues->Put(itemId, NULL);
                }
            }
            else {
                view->SetHasTransientState(TRUE);
                p->mItemIdValues->Put(itemId, view);
            }
        }
    }
}

ECode Transition::ClearValues(
    /* [in] */ Boolean start)
{
    if (start) {
        AutoPtr<TransitionValuesMaps> cSV = (TransitionValuesMaps*)mStartValues.Get();
        IMap::Probe(cSV->mViewValues)->Clear();
        IMap::Probe(cSV->mIdValues)->Clear();
        IMap::Probe(cSV->mItemIdValues)->Clear();
        IMap::Probe(cSV->mNameValues)->Clear();
        mStartValuesList = NULL;
    }
    else {
        AutoPtr<TransitionValuesMaps> cEV = (TransitionValuesMaps*)mEndValues.Get();
        IMap::Probe(cEV->mViewValues)->Clear();
        IMap::Probe(cEV->mIdValues)->Clear();
        IMap::Probe(cEV->mItemIdValues)->Clear();
        IMap::Probe(cEV->mNameValues)->Clear();
        mEndValuesList = NULL;
    }
    return NOERROR;
}

void Transition::CaptureHierarchy(
    /* [in] */ IView* view,
    /* [in] */ Boolean start)
{
    if (view == NULL) {
        return;
    }
    Int32 id = 0;
    view->GetId(&id);
    AutoPtr<IInteger32> pID;
    CInteger32::New(id, (IInteger32**)&pID);
    Boolean bIdContain = FALSE;
    if (mTargetIdExcludes != NULL && (mTargetIdExcludes->Contains(pID, &bIdContain), bIdContain)) {
        return;
    }
    Boolean bContain = FALSE;
    if (mTargetExcludes != NULL && (mTargetExcludes->Contains(view, &bContain), bContain)) {
        return;
    }
    if (mTargetTypeExcludes != NULL && view != NULL) {
        Int32 numTypes = 0;
        mTargetTypeExcludes->GetSize(&numTypes);
        for (Int32 i = 0; i < numTypes; ++i) {
            AutoPtr<IInterface> p;
            mTargetTypeExcludes->Get(i, (IInterface**)&p);
            assert(0 && "TODO");
            // if (p->IsInstance(view)) {
            //     return;
            // }
        }
    }
    AutoPtr<IViewParent> p;
    view->GetParent((IViewParent**)&p);
    if ( IViewGroup::Probe(p) != NULL) {
        AutoPtr<ITransitionValues> values;
        CTransitionValues::New((ITransitionValues**)&values);
        AutoPtr<CTransitionValues> cv = (CTransitionValues*)values.Get();
        cv->mView = view;
        if (start) {
            CaptureStartValues(values);
        }
        else {
            CaptureEndValues(values);
        }
        cv->mTargetedTransitions->Add(TO_IINTERFACE(this));
        CapturePropagationValues(values);
        if (start) {
            AddViewValues(mStartValues, view, values);
        }
        else {
            AddViewValues(mEndValues, view, values);
        }
    }
    if (IViewGroup::Probe(view) != NULL) {
        // Don't traverse child hierarchy if there are any child-excludes on this view
        if (mTargetIdChildExcludes != NULL && (mTargetIdChildExcludes->Contains(pID, &bIdContain), bIdContain)) {
            return;
        }
        if (mTargetChildExcludes != NULL && (mTargetChildExcludes->Contains(view, &bContain), bContain)) {
            return;
        }
        if (mTargetTypeChildExcludes != NULL) {
            Int32 numTypes = 0;
            mTargetTypeChildExcludes->GetSize(&numTypes);
            for (Int32 i = 0; i < numTypes; ++i) {
                AutoPtr<IInterface> p;
                mTargetTypeChildExcludes->Get(i, (IInterface**)&p);
                assert(0 && "TODO");
                // if (p->IsInstance(view)) {
                //     return;
                // }
            }
        }
        AutoPtr<IViewGroup> parent = IViewGroup::Probe(view);
        Int32 count = 0;
        parent->GetChildCount(&count);
        for (Int32 i = 0; i < count; ++i) {
            AutoPtr<IView> child;
            parent->GetChildAt(i, (IView**)&child);
            CaptureHierarchy(child, start);
        }
    }
}

ECode Transition::GetTransitionValues(
    /* [in] */ IView* view,
    /* [in] */ Boolean start,
    /* [out] */ ITransitionValues** result)
{
    VALIDATE_NOT_NULL(result)

    if (mParent != NULL) {
        return ITransition::Probe(mParent)->GetTransitionValues(view, start, result);
    }
    AutoPtr<ITransitionValuesMaps> valuesMaps = start ? mStartValues : mEndValues;
    AutoPtr<TransitionValuesMaps> cvm = (TransitionValuesMaps*)valuesMaps.Get();
    AutoPtr<IInterface> res;
    IMap::Probe(cvm->mViewValues)->Get(view, (IInterface**)&res);
    *result = ITransitionValues::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::GetMatchedTransitionValues(
    /* [in] */ IView* view,
    /* [in] */ Boolean viewInStart,
    /* [out] */ ITransitionValues** result)
{
    VALIDATE_NOT_NULL(result)

    if (mParent != NULL) {
        return ITransition::Probe(mParent)->GetMatchedTransitionValues(view, viewInStart, result);
    }
    AutoPtr<IArrayList> lookIn = viewInStart ? mStartValuesList : mEndValuesList;
    if (lookIn == NULL) {
        *result = NULL;
        return NOERROR;
    }
    Int32 count = 0;
    lookIn->GetSize(&count);
    Int32 index = -1;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> v;
        lookIn->Get(i, (IInterface**)&v);
        AutoPtr<ITransitionValues> values = ITransitionValues::Probe(v);
        if (values == NULL) {
            *result = NULL;
            return NOERROR;
        }
        AutoPtr<CTransitionValues> cv = (CTransitionValues*)values.Get();
        if (Object::Equals(cv->mView->Probe(EIID_IInterface), view->Probe(EIID_IInterface))) {
            index = i;
            break;
        }
    }
    AutoPtr<ITransitionValues> values;
    if (index >= 0) {
        AutoPtr<IArrayList> matchIn = viewInStart ? mEndValuesList : mStartValuesList;
        AutoPtr<IInterface> vls;
        matchIn->Get(index, (IInterface**)&vls);
        values = ITransitionValues::Probe(vls);
    }
    *result = values;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::Pause(
    /* [in] */ IView* sceneRoot)
{
    if (!mEnded) {
        AutoPtr<IArrayMap> runningAnimators = GetRunningAnimators();
        Int32 numOldAnims = 0;
        IMap::Probe(runningAnimators)->GetSize(&numOldAnims);
        if (sceneRoot != NULL) {
            AutoPtr<IWindowId> windowId;
            sceneRoot->GetWindowId((IWindowId**)&windowId);
            for (Int32 i = numOldAnims - 1; i >= 0; i--) {
                AutoPtr<IInterface> pInfo;
                runningAnimators->GetValueAt(i, (IInterface**)&pInfo);
                AutoPtr<AnimationInfo> info = (AnimationInfo*)(IObject*)pInfo.Get();
                if (info->mView != NULL && windowId != NULL &&
                    Object::Equals(windowId->Probe(EIID_IInterface), info->mWindowId->Probe(EIID_IInterface))) {
                    AutoPtr<IInterface> pAnim;
                    runningAnimators->GetKeyAt(i, (IInterface**)&pAnim);
                    AutoPtr<IAnimator> anim = IAnimator::Probe(pAnim);
                    anim->Pause();
                }
            }
        }
        Int32 s = 0;
        mListeners->GetSize(&s);
        if (mListeners != NULL && s > 0) {
            assert(0 && "TODO");
            AutoPtr<IArrayList> tmpListeners;// = (IArrayList*) mListeners->Clone();
            Int32 numListeners = 0;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> p;
                tmpListeners->Get(i, (IInterface**)&p);
                AutoPtr<ITransitionListener> tl = ITransitionListener::Probe(p);
                tl->OnTransitionPause(this);
            }
        }
        mPaused = TRUE;
    }
    return NOERROR;
}

ECode Transition::Resume(
    /* [in] */ IView* sceneRoot)
{
    if (mPaused) {
        if (!mEnded) {
            AutoPtr<IArrayMap> runningAnimators = GetRunningAnimators();
            Int32 numOldAnims = 0;
            IMap::Probe(runningAnimators)->GetSize(&numOldAnims);
            AutoPtr<IWindowId> windowId;
            sceneRoot->GetWindowId((IWindowId**)&windowId);
            for (Int32 i = numOldAnims - 1; i >= 0; i--) {
                AutoPtr<IInterface> v;
                runningAnimators->GetValueAt(i, (IInterface**)&v);
                AutoPtr<AnimationInfo> info = (AnimationInfo*)(IObject*)v.Get();
                if (info->mView != NULL && windowId != NULL &&
                    Object::Equals(windowId->Probe(EIID_IInterface), info->mWindowId->Probe(EIID_IInterface))) {
                    AutoPtr<IInterface> k;
                    runningAnimators->GetKeyAt(i, (IInterface**)&k);
                    AutoPtr<IAnimator> anim = IAnimator::Probe(k);
                    anim->Resume();
                }
            }
            Int32 s = 0;
            mListeners->GetSize(&s);
            if (mListeners != NULL && s > 0) {
                assert(0 && "TODO");
                AutoPtr<IArrayList> tmpListeners;// = (IArrayList*) mListeners->Clone();
                Int32 numListeners = 0;
                tmpListeners->GetSize(&numListeners);
                for (Int32 i = 0; i < numListeners; ++i) {
                    AutoPtr<IInterface> p;
                    tmpListeners->Get(i, (IInterface**)&p);
                    AutoPtr<ITransitionListener> tl = ITransitionListener::Probe(p);
                    tl->OnTransitionResume(this);
                }
            }
        }
        mPaused = FALSE;
    }
    return NOERROR;
}

ECode Transition::PlayTransition(
    /* [in] */ IViewGroup* sceneRoot)
{
    CArrayList::New((IArrayList**)&mStartValuesList);
    CArrayList::New((IArrayList**)&mEndValuesList);
    MatchStartAndEnd(mStartValues, mEndValues);

    AutoPtr<IArrayMap> runningAnimators = GetRunningAnimators();
    Int32 numOldAnims = 0;
    IMap::Probe(runningAnimators)->GetSize(&numOldAnims);
    AutoPtr<IWindowId> windowId;
    IView::Probe(sceneRoot)->GetWindowId((IWindowId**)&windowId);
    for (Int32 i = numOldAnims - 1; i >= 0; i--) {
        AutoPtr<IInterface> pAnim;
        runningAnimators->GetKeyAt(i, (IInterface**)&pAnim);
        AutoPtr<IAnimator> anim = IAnimator::Probe(pAnim);
        if (anim != NULL) {
            AutoPtr<IInterface> p;
            IMap::Probe(runningAnimators)->Get(anim, (IInterface**)&p);
            AutoPtr<AnimationInfo> oldInfo = (AnimationInfo*)(IObject*)p.Get();
            if (oldInfo != NULL && oldInfo->mView != NULL &&
                Object::Equals(oldInfo->mWindowId->Probe(EIID_IInterface), windowId->Probe(EIID_IInterface))) {
                AutoPtr<ITransitionValues> oldValues = oldInfo->mValues;
                AutoPtr<CTransitionValues> cOldValues = (CTransitionValues*)oldValues.Get();
                AutoPtr<IView> oldView = cOldValues->mView;
                AutoPtr<ITransitionValues> startValues;
                GetTransitionValues(oldView, TRUE, (ITransitionValues**)&startValues);
                AutoPtr<ITransitionValues> endValues;
                GetMatchedTransitionValues(oldView, TRUE, (ITransitionValues**)&endValues);
                Boolean bValChange = FALSE;
                Boolean cancel = (startValues != NULL || endValues != NULL) &&
                        (oldInfo->mTransition->AreValuesChanged(oldValues, endValues, &bValChange), bValChange);
                if (cancel) {
                    Boolean bRun = FALSE, bStart = FALSE;
                    if ((anim->IsRunning(&bRun), bRun) || (anim->IsStarted(&bStart), bStart)) {
                        if (DBG) {
                            Logger::D(LOG_TAG, "Canceling anim %p", anim.Get());
                        }
                        anim->Cancel();
                    }
                    else {
                        if (DBG) {
                            Logger::D(LOG_TAG, "removing anim from info list: %p", anim.Get());
                        }
                        IMap::Probe(runningAnimators)->Remove(anim);
                    }
                }
            }
        }
    }

    CreateAnimators(sceneRoot, mStartValues, mEndValues, mStartValuesList, mEndValuesList);
    RunAnimators();
    return NOERROR;
}

ECode Transition::AreValuesChanged(
    /* [in] */ ITransitionValues* oldValues,
    /* [in] */ ITransitionValues* newValues,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean valuesChanged = FALSE;
    // if oldValues NULL, then transition didn't care to stash values,
    // and won't get canceled
    if (oldValues != NULL && newValues != NULL) {
        AutoPtr<ArrayOf<String> > properties;
        GetTransitionProperties((ArrayOf<String>**)&properties);
        if (properties != NULL) {
            Int32 count = properties->GetLength();
            for (Int32 i = 0; i < count; i++) {
                if (IsValueChanged(oldValues, newValues, (*properties)[i])) {
                    valuesChanged = TRUE;
                    break;
                }
            }
        }
        else {
            AutoPtr<CTransitionValues> cOld = (CTransitionValues*)oldValues;
            AutoPtr<ISet> ks;
            cOld->mValues->GetKeySet((ISet**)&ks);
            AutoPtr<IIterator> it;
            ks->GetIterator((IIterator**)&it);
            Boolean bHasMore = FALSE;
            while ((it->HasNext(&bHasMore), bHasMore)) {
                AutoPtr<IInterface> k;
                it->GetNext((IInterface**)&k);
                AutoPtr<ICharSequence> pKey = ICharSequence::Probe(k);
                String key;
                pKey->ToString(&key);
                if (IsValueChanged(oldValues, newValues, key)) {
                    valuesChanged = TRUE;
                    break;
                }
            }
        }
    }
    *result = valuesChanged;
    return NOERROR;
}

Boolean Transition::IsValueChanged(
    /* [in] */ ITransitionValues* oldValues,
    /* [in] */ ITransitionValues* newValues,
    /* [in] */ const String& key)
{
    AutoPtr<CTransitionValues> cOld = (CTransitionValues*)oldValues;
    AutoPtr<CTransitionValues> cNew = (CTransitionValues*)newValues;
    AutoPtr<ICharSequence> pKey;
    CString::New(key, (ICharSequence**)&pKey);
    AutoPtr<IInterface> oldValue;
    cOld->mValues->Get(pKey, (IInterface**)&oldValue);
    AutoPtr<IInterface> newValue;
    cNew->mValues->Get(pKey, (IInterface**)&newValue);
    Boolean changed;
    if (oldValue == NULL && newValue == NULL) {
        // both are null
        changed = FALSE;
    }
    else if (oldValue == NULL || newValue == NULL) {
        // one is null
        changed = TRUE;
    }
    else {
        // neither is null
        changed = !Object::Equals(oldValue, newValue);
    }
    if (DBG && changed) {
        Logger::D(LOG_TAG, "Transition.playTransition: "
                "oldValue != newValue for %s"
                ": old, new = %p, %p", (const char*)key, oldValue.Get(), newValue.Get());
    }
    return changed;
}

ECode Transition::Animate(
    /* [in] */ IAnimator* animator)
{
    // TODO: maybe pass auto-end as a boolean parameter?
    if (animator == NULL) {
        End();
    }
    else {
        Int64 d = 0;
        if ((GetDuration(&d), d) >= 0) {
            animator->SetDuration((GetDuration(&d), d));
        }
        Int64 sd = 0;
        if ((GetStartDelay(&sd), sd) >= 0) {
            Int64 asd = 0;
            animator->SetStartDelay((GetStartDelay(&sd), sd) + (animator->GetStartDelay(&asd), asd));
        }
        AutoPtr<ITimeInterpolator> p;
        GetInterpolator((ITimeInterpolator**)&p);
        if (p != NULL) {
            animator->SetInterpolator(p);
        }
        AutoPtr<AnimatorListenerAdapter_2> la = new AnimatorListenerAdapter_2(this);
        animator->AddListener(IAnimatorListener::Probe(la));
        animator->Start();
    }
    return NOERROR;
}

ECode Transition::Start()
{
    if (mNumInstances == 0) {
        Int32 s = 0;
        mListeners->GetSize(&s);
        if (mListeners != NULL && s > 0) {
            AutoPtr<IArrayList> tmpListeners;// = (IArrayList*) mListeners->Clone();
            Int32 numListeners = 0;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> p;
                tmpListeners->Get(i, (IInterface**)&p);
                AutoPtr<ITransitionListener> tl = ITransitionListener::Probe(p);
                tl->OnTransitionStart(this);
            }
        }
        mEnded = FALSE;
    }
    mNumInstances++;
    return NOERROR;
}

ECode Transition::End()
{
    --mNumInstances;
    if (mNumInstances == 0) {
        Int32 s = 0;
        mListeners->GetSize(&s);
        if (mListeners != NULL && s > 0) {
            AutoPtr<IArrayList> tmpListeners;// = (IArrayList*) mListeners->Clone();
            Int32 numListeners = 0;
            tmpListeners->GetSize(&numListeners);
            for (Int32 i = 0; i < numListeners; ++i) {
                AutoPtr<IInterface> p;
                tmpListeners->Get(i, (IInterface**)&p);
                AutoPtr<ITransitionListener> tl = ITransitionListener::Probe(p);
                tl->OnTransitionEnd(this);
            }
        }
        AutoPtr<TransitionValuesMaps> cEndv = (TransitionValuesMaps*)mEndValues.Get();
        AutoPtr<TransitionValuesMaps> cStartv = (TransitionValuesMaps*)mStartValues.Get();
        Int32 sE = 0;
        cEndv->mItemIdValues->GetSize(&sE);
        for (Int32 i = 0; i < sE; ++i) {
            AutoPtr<IInterface> p;
            cStartv->mItemIdValues->ValueAt(i, (IInterface**)&p);
            AutoPtr<IView> view = IView::Probe(p);
            if (view != NULL) {
                view->SetHasTransientState(FALSE);
            }
        }
        Int32 sS = 0;
        cEndv->mItemIdValues->GetSize(&sS);
        for (Int32 i = 0; i < sS; ++i) {
            AutoPtr<IInterface> p;
            cEndv->mItemIdValues->ValueAt(i, (IInterface**)&p);
            AutoPtr<IView> view = IView::Probe(p);
            if (view != NULL) {
                view->SetHasTransientState(FALSE);
            }
        }
        mEnded = TRUE;
    }
    return NOERROR;
}

ECode Transition::Cancel()
{
    Int32 numAnimators = 0;
    mCurrentAnimators->GetSize(&numAnimators);
    for (Int32 i = numAnimators - 1; i >= 0; i--) {
        AutoPtr<IInterface> p;
        mCurrentAnimators->Get(i, (IInterface**)&p);
        AutoPtr<IAnimator> animator = IAnimator::Probe(p);
        animator->Cancel();
    }
    Int32 s = 0;
    mListeners->GetSize(&s);
    if (mListeners != NULL && s > 0) {
        AutoPtr<IArrayList> tmpListeners;// = (IArrayList*) mListeners->Clone();
        Int32 numListeners = 0;
        tmpListeners->GetSize(&numListeners);
        for (Int32 i = 0; i < numListeners; ++i) {
            AutoPtr<IInterface> p;
            tmpListeners->Get(i, (IInterface**)&p);
            AutoPtr<ITransitionListener> tl = ITransitionListener::Probe(p);
            tl->OnTransitionCancel(this);
        }
    }
    return NOERROR;
}

ECode Transition::AddListener(
    /* [in] */ ITransitionListener* listener)
{
    if (mListeners == NULL) {
        CArrayList::New((IArrayList**)&mListeners);
    }
    mListeners->Add(listener);
    return NOERROR;
}

ECode Transition::RemoveListener(
    /* [in] */ ITransitionListener* listener)
{
    if (mListeners == NULL) {
        return NOERROR;
    }
    mListeners->Remove(listener);
    Int32 size = 0;
    if ((mListeners->GetSize(&size), size) == 0) {
        mListeners = NULL;
    }
    return NOERROR;
}

ECode Transition::SetEpicenterCallback(
    /* [in] */ IEpicenterCallback* epicenterCallback)
{
    mEpicenterCallback = epicenterCallback;
    return NOERROR;
}

ECode Transition::GetEpicenterCallback(
    /* [out] */ IEpicenterCallback** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEpicenterCallback;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::GetEpicenter(
    /* [out] */ IRect** result)
{
    VALIDATE_NOT_NULL(result)

    if (mEpicenterCallback == NULL) {
        *result = NULL;
        return NOERROR;
    }
    return mEpicenterCallback->OnGetEpicenter(this, result);
}

ECode Transition::SetPathMotion(
    /* [in] */ IPathMotion* pathMotion)
{
    if (pathMotion == NULL) {
        mPathMotion = STRAIGHT_PATH_MOTION;
    }
    else {
        mPathMotion = pathMotion;
    }
    return NOERROR;
}

ECode Transition::GetPathMotion(
    /* [out] */ IPathMotion** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPathMotion;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::SetPropagation(
    /* [in] */ ITransitionPropagation* transitionPropagation)
{
    mPropagation = transitionPropagation;
    return NOERROR;
}

ECode Transition::GetPropagation(
    /* [out] */ ITransitionPropagation** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPropagation;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::CapturePropagationValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ctv = (CTransitionValues*)transitionValues;
    Boolean bIsEmpty = FALSE;
    if (mPropagation != NULL && !(ctv->mValues->IsEmpty(&bIsEmpty), bIsEmpty)) {
        AutoPtr<ArrayOf<String> > propertyNames;
        mPropagation->GetPropagationProperties((ArrayOf<String>**)&propertyNames);
        if (propertyNames == NULL) {
            return NOERROR;
        }
        Boolean containsAll = TRUE;
        for (Int32 i = 0; i < propertyNames->GetLength(); i++) {
            AutoPtr<ICharSequence> pProName;
            CString::New((*propertyNames)[i], (ICharSequence**)&pProName);
            Boolean bContains = FALSE;
            if (!(ctv->mValues->ContainsKey(pProName, &bContains), bContains)) {
                containsAll = FALSE;
                break;
            }
        }
        if (!containsAll) {
            mPropagation->CaptureValues(transitionValues);
        }
    }
    return NOERROR;
}

ECode Transition::SetSceneRoot(
    /* [in] */ IViewGroup* sceneRoot)
{
    mSceneRoot = sceneRoot;
    return NOERROR;
}

ECode Transition::SetCanRemoveViews(
    /* [in] */ Boolean canRemoveViews)
{
    mCanRemoveViews = canRemoveViews;
    return NOERROR;
}

ECode Transition::CanRemoveViews(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mCanRemoveViews;
    return NOERROR;
}

ECode Transition::SetNameOverrides(
    /* [in] */ IArrayMap* overrides)
{
    mNameOverrides = overrides;
    return NOERROR;
}

ECode Transition::GetNameOverrides(
    /* [out] */ IArrayMap** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mNameOverrides;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Transition::ForceVisibility(
    /* [in] */ Int32 visibility,
    /* [in] */ Boolean isStartValue)
{
    return NOERROR;
}

ECode Transition::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    return ToString(String(""), result);
}

ECode Transition::CloneImpl(
    /* [in] */ ITransition* transObj)
{
    VALIDATE_NOT_NULL(transObj)
    Transition* trans = (Transition*)transObj;

    CArrayList::New((IArrayList**)&(trans->mAnimators));
    trans->mStartValues = new TransitionValuesMaps();
    trans->mEndValues = new TransitionValuesMaps();
    trans->mStartValuesList = NULL;
    trans->mEndValuesList = NULL;
    return NOERROR;
}

ECode Transition::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mName;
    return NOERROR;
}

ECode Transition::ToString(
    /* [in] */ const String& indent,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    StringBuilder result;// = indent + getClass().getSimpleName() + "@" +
        //    Integer::ToHexString(hashCode()) + ": ";
    if (mDuration != -1) {
        result += "dur(";
        result += mDuration;
        result += ") ";
    }
    if (mStartDelay != -1) {
        result += "dly(";
        result += mStartDelay;
        result += ") ";
    }
    if (mInterpolator != NULL) {
        result += "interp(";
        result += mInterpolator;
        result += ") ";
    }
    Int32 s1 = 0, s2 = 0;
    if ((mTargetIds->GetSize(&s1), s1) > 0 || (mTargets->GetSize(&s2), s2) > 0) {
        result += "tgts(";
        if (s1 > 0) {
            for (Int32 i = 0; i < s1; ++i) {
                if (i > 0) {
                    result += ", ";
                }
                AutoPtr<IInterface> tid;
                mTargetIds->Get(i, (IInterface**)&tid);
                result += tid;
            }
        }
        if (s2 > 0) {
            for (Int32 i = 0; i < s2; ++i) {
                if (i > 0) {
                    result += ", ";
                }
                AutoPtr<IInterface> t;
                mTargets->Get(i, (IInterface**)&t);
                result += t;
            }
        }
        result += ")";
    }
    *res = result.ToString();
    return NOERROR;
}

//===============================================================
// Transition::AnimationInfo::
//===============================================================

Transition::AnimationInfo::AnimationInfo(
    /* [in] */ IView* view,
    /* [in] */ const String& name,
    /* [in] */ ITransition* transition,
    /* [in] */ IWindowId* windowId,
    /* [in] */ ITransitionValues* values)
{
    this->mView = view;
    this->mName = name;
    this->mValues = values;
    this->mWindowId = windowId;
    this->mTransition = transition;
}

//===============================================================
// Transition::ArrayListManager::
//===============================================================

AutoPtr<IArrayList> Transition::ArrayListManager::Add(
    /* [in] */ IArrayList* inList,
    /* [in] */ IInterface* item)
{
    AutoPtr<IArrayList> list = inList;
    if (list == NULL) {
        CArrayList::New((IArrayList**)&list);
    }
    Boolean bCon = FALSE;
    if (!(list->Contains(item, &bCon), bCon)) {
        list->Add(item);
    }
    return list;
}

AutoPtr<IArrayList> Transition::ArrayListManager::Remove(
    /* [in] */ IArrayList* inList,
    /* [in] */ IInterface* item)
{
    AutoPtr<IArrayList> list = inList;
    if (list != NULL) {
        list->Remove(item);
        Boolean bIsEmpty = FALSE;
        if ((list->IsEmpty(&bIsEmpty), bIsEmpty)) {
            list = NULL;
        }
    }
    return list;
}

//===============================================================
// Transition::TransitionListenerAdapter::
//===============================================================
CAR_INTERFACE_IMPL(Transition::TransitionListenerAdapter, Object, ITransitionListener)

ECode Transition::TransitionListenerAdapter::OnTransitionStart(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Transition::TransitionListenerAdapter::OnTransitionEnd(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Transition::TransitionListenerAdapter::OnTransitionCancel(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Transition::TransitionListenerAdapter::OnTransitionPause(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

ECode Transition::TransitionListenerAdapter::OnTransitionResume(
    /* [in] */ ITransition* transition)
{
    return NOERROR;
}

//===============================================================
// Transition::AnimatorListenerAdapter_1::
//===============================================================
Transition::AnimatorListenerAdapter_1::AnimatorListenerAdapter_1()
{
}

Transition::AnimatorListenerAdapter_1::AnimatorListenerAdapter_1(
    /* [in] */ IArrayList* curAnimators,
    /* [in] */ IArrayMap* runningAnimators)
{
    mCurAnimators = curAnimators;
    mRunningAnimators = runningAnimators;
}

ECode Transition::AnimatorListenerAdapter_1::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mCurAnimators->Add(animation);
    return NOERROR;
}

ECode Transition::AnimatorListenerAdapter_1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    IMap::Probe(mRunningAnimators)->Remove(animation);
    mCurAnimators->Remove(animation);
    return NOERROR;
}

//===============================================================
// Transition::AnimatorListenerAdapter_2::
//===============================================================
Transition::AnimatorListenerAdapter_2::AnimatorListenerAdapter_2(
    /* [in] */ Transition* owner)
{
    mOwner = owner;
}

ECode Transition::AnimatorListenerAdapter_2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mOwner->End();
    animation->RemoveListener(IAnimatorListener::Probe(this));
    return NOERROR;
}

//===============================================================
// Transition::PathMotionOverride::
//===============================================================
ECode Transition::PathMotionOverride::GetPath(
    /* [in] */ Float startX,
    /* [in] */ Float startY,
    /* [in] */ Float endX,
    /* [in] */ Float endY,
    /* [out] */ IPath** result)
{
    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    path->MoveTo(startX, startY);
    path->LineTo(endX, endY);
    *result = path.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
