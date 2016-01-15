
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/transition/Visibility.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/transition/CTransitionUtils.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewOverlay;

using Elastos::Core::CString;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// Visibility::
//===============================================================
String Visibility::PROPNAME_VISIBILITY = String("android:visibility:visibility");
String Visibility::PROPNAME_PARENT = String("android:visibility:parent");
String Visibility::PROPNAME_SCREEN_LOCATION = String("android:visibility:screenLocation");

Int32 Visibility::MODE_IN = 0x1;

Int32 Visibility::MODE_OUT = 0x2;

AutoPtr<ArrayOf<String> > Visibility::sTransitionProperties = ArrayOf<String>::Alloc(1);

CAR_INTERFACE_IMPL(Visibility, Transition, IVisibility)

Visibility::Visibility()
{
    (*sTransitionProperties)[0] = PROPNAME_VISIBILITY;
}

ECode Visibility::constructor()
{
    return NOERROR;
}

ECode Visibility::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Transition::constructor(context, attrs);
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32*>(R::styleable::VisibilityTransition),
        ArraySize(R::styleable::VisibilityTransition));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    Int32 mode = 0;
    a->GetInt32(R::styleable::VisibilityTransition_transitionVisibilityMode, 0, &mode);
    a->Recycle();
    if (mode != 0) {
        SetMode(mode);
    }
    return NOERROR;
}

ECode Visibility::SetMode(
    /* [in] */ Int32 mode)
{
    if ((mode & ~(MODE_IN | MODE_OUT)) != 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMode = mode;
    return NOERROR;
}

ECode Visibility::GetMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mMode;
    return NOERROR;
}

ECode Visibility::GetTransitionProperties(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)

    *result = sTransitionProperties;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void Visibility::CaptureValues(
    /* [in] */ ITransitionValues* transitionValues,
    /* [in] */ Int32 forcedVisibility)
{
    AutoPtr<CTransitionValues> ctv = (CTransitionValues*)transitionValues;
    Int32 visibility = 0;
    if (forcedVisibility != -1) {
        visibility = forcedVisibility;
    }
    else {
        ctv->mView->GetVisibility(&visibility);
    }
    AutoPtr<ICharSequence> pro_vis;
    CString::New(PROPNAME_VISIBILITY, (ICharSequence**)&pro_vis);
    AutoPtr<IInteger32> pVis;
    CInteger32::New(visibility, (IInteger32**)&pVis);
    ctv->mValues->Put(pro_vis, pVis);
    AutoPtr<ICharSequence> pro_par;
    CString::New(PROPNAME_PARENT, (ICharSequence**)&pro_par);
    AutoPtr<IViewParent> vp;
    ctv->mView->GetParent((IViewParent**)&vp);
    ctv->mValues->Put(pro_par, vp);
    AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(2);
    ctv->mView->GetLocationOnScreen(temp);
    AutoPtr<ICharSequence> pro_loc;
    CString::New(PROPNAME_SCREEN_LOCATION, (ICharSequence**)&pro_loc);
    AutoPtr<IInteger32> pLoc1;
    CInteger32::New((*temp)[0], (IInteger32**)&pLoc1);
    AutoPtr<IInteger32> pLoc2;
    CInteger32::New((*temp)[1], (IInteger32**)&pLoc2);
    assert(0 && "TODO");
//    ctv->mValues->Put(pro_loc, pLoc1, pLoc2);
}

ECode Visibility::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues, mForcedStartVisibility);
    return NOERROR;
}

ECode Visibility::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    CaptureValues(transitionValues, mForcedEndVisibility);
    return NOERROR;
}

ECode Visibility::ForceVisibility(
    /* [in] */ Int32 visibility,
    /* [in] */ Boolean isStartValue)
{
    if (isStartValue) {
        mForcedStartVisibility = visibility;
    }
    else {
        mForcedEndVisibility = visibility;
    }
    return NOERROR;
}

ECode Visibility::IsVisible(
    /* [in] */ ITransitionValues* values,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (values == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<CTransitionValues> cv = (CTransitionValues*)values;
    AutoPtr<ICharSequence> pro_vis;
    CString::New(PROPNAME_VISIBILITY, (ICharSequence**)&pro_vis);
    AutoPtr<IInterface> vis;
    cv->mValues->Get(pro_vis, (IInterface**)&vis);
    AutoPtr<IInteger32> pVis = IInteger32::Probe(vis);
    Int32 visibility = 0;
    pVis->GetValue(&visibility);
    AutoPtr<ICharSequence> pro_par;
    CString::New(PROPNAME_PARENT, (ICharSequence**)&pro_par);
    AutoPtr<IInterface> par;
    cv->mValues->Get(pro_par, (IInterface**)&par);
    AutoPtr<IView> parent = IView::Probe(par);

    *result = visibility == IView::VISIBLE && parent != NULL;
    return NOERROR;
}

AutoPtr<Visibility::VisibilityInfo> Visibility::GetVisibilityChangeInfo(
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues)
{
    AutoPtr<VisibilityInfo> visInfo = new VisibilityInfo();
    visInfo->mVisibilityChange = FALSE;
    visInfo->mFadeIn = FALSE;
    AutoPtr<CTransitionValues> csv = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> cev = (CTransitionValues*)endValues;
    AutoPtr<ICharSequence> pro_vis;
    CString::New(PROPNAME_VISIBILITY, (ICharSequence**)&pro_vis);
    AutoPtr<ICharSequence> pro_par;
    CString::New(PROPNAME_PARENT, (ICharSequence**)&pro_par);
    Boolean bContains = FALSE;
    if (startValues != NULL && (csv->mValues->ContainsKey(pro_vis, &bContains), bContains)) {
        AutoPtr<IInterface> pVis;
        csv->mValues->Get(pro_vis, (IInterface**)&pVis);
        IInteger32::Probe(pVis)->GetValue(&(visInfo->mStartVisibility));
        AutoPtr<IInterface> pPar;
        csv->mValues->Get(pro_par, (IInterface**)&pPar);
        visInfo->mStartParent = IViewGroup::Probe(pPar);
    }
    else {
        visInfo->mStartVisibility = -1;
        visInfo->mStartParent = NULL;
    }
    if (endValues != NULL && (cev->mValues->ContainsKey(pro_vis, &bContains), bContains)) {
        AutoPtr<IInterface> pVis;
        cev->mValues->Get(pro_vis, (IInterface**)&pVis);
        IInteger32::Probe(pVis)->GetValue(&(visInfo->mEndVisibility));
        AutoPtr<IInterface> pPar;
        cev->mValues->Get(pro_par, (IInterface**)&pPar);
        visInfo->mEndParent = IViewGroup::Probe(pPar);
    }
    else {
        visInfo->mEndVisibility = -1;
        visInfo->mEndParent = NULL;
    }
    if (startValues != NULL && endValues != NULL) {
        if (visInfo->mStartVisibility == visInfo->mEndVisibility &&
                visInfo->mStartParent == visInfo->mEndParent) {
            return visInfo;
        }
        else {
            if (visInfo->mStartVisibility != visInfo->mEndVisibility) {
                if (visInfo->mStartVisibility == IView::VISIBLE) {
                    visInfo->mFadeIn = FALSE;
                    visInfo->mVisibilityChange = TRUE;
                }
                else if (visInfo->mEndVisibility == IView::VISIBLE) {
                    visInfo->mFadeIn = TRUE;
                    visInfo->mVisibilityChange = TRUE;
                }
                // no visibilityChange if going between INVISIBLE and GONE
            }
            else if (visInfo->mStartParent != visInfo->mEndParent) {
                if (visInfo->mEndParent == NULL) {
                    visInfo->mFadeIn = FALSE;
                    visInfo->mVisibilityChange = TRUE;
                }
                else if (visInfo->mStartParent == NULL) {
                    visInfo->mFadeIn = TRUE;
                    visInfo->mVisibilityChange = TRUE;
                }
            }
        }
    }
    else if (startValues == NULL && visInfo->mEndVisibility == IView::VISIBLE) {
        visInfo->mFadeIn = TRUE;
        visInfo->mVisibilityChange = TRUE;
    }
    else if (endValues == NULL && visInfo->mStartVisibility == IView::VISIBLE) {
        visInfo->mFadeIn = FALSE;
        visInfo->mVisibilityChange = TRUE;
    }
    return visInfo;
}

ECode Visibility::CreateAnimator(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<VisibilityInfo> visInfo = GetVisibilityChangeInfo(startValues, endValues);
    if (visInfo->mVisibilityChange
            && (visInfo->mStartParent != NULL || visInfo->mEndParent != NULL)) {
        if (visInfo->mFadeIn) {
            return OnAppear(sceneRoot, startValues, visInfo->mStartVisibility,
                    endValues, visInfo->mEndVisibility, result);
        }
        else {
            return OnDisappear(sceneRoot, startValues, visInfo->mStartVisibility,
                    endValues, visInfo->mEndVisibility, result);
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode Visibility::OnAppear(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ Int32 startVisibility,
    /* [in] */ ITransitionValues* endValues,
    /* [in] */ Int32 endVisibility,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)

    if ((mMode & MODE_IN) != MODE_IN || endValues == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<CTransitionValues> cev = (CTransitionValues*)endValues;
    if (startValues == NULL) {
        AutoPtr<VisibilityInfo> parentVisibilityInfo;
        AutoPtr<IViewParent> vp;
        cev->mView->GetParent((IViewParent**)&vp);
        AutoPtr<IView> endParent = IView::Probe(vp);
        AutoPtr<ITransitionValues> startParentValues;
        GetMatchedTransitionValues(endParent, FALSE, (ITransitionValues**)&startParentValues);
        AutoPtr<ITransitionValues> endParentValues;
        GetTransitionValues(endParent, FALSE, (ITransitionValues**)&endParentValues);
        parentVisibilityInfo =
            GetVisibilityChangeInfo(startParentValues, endParentValues);
        if (parentVisibilityInfo->mVisibilityChange) {
            *result = NULL;
            return NOERROR;
        }
    }
    return OnAppear(sceneRoot, cev->mView, startValues, endValues, result);
}

ECode Visibility::OnAppear(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

ECode Visibility::OnDisappear(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ Int32 startVisibility,
    /* [in] */ ITransitionValues* endValues,
    /* [in] */ Int32 endVisibility,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)

    if ((mMode & MODE_OUT) != MODE_OUT) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<CTransitionValues> csv = (CTransitionValues*)startValues;
    AutoPtr<CTransitionValues> cev = (CTransitionValues*)endValues;
    AutoPtr<IView> startView = (startValues != NULL) ? csv->mView : NULL;
    AutoPtr<IView> endView = (endValues != NULL) ? cev->mView : NULL;
    AutoPtr<IView> overlayView;
    AutoPtr<IView> viewToKeep;
    AutoPtr<IViewParent> vp;
    if (endView == NULL || (endView->GetParent((IViewParent**)&vp), vp) == NULL) {
        if (endView != NULL) {
            // endView was removed from its parent - add it to the overlay
            overlayView = endView;
        }
        else if (startView != NULL) {
            // endView does not exist. Use startView only under certain
            // conditions, because placing a view in an overlay necessitates
            // it being removed from its current parent
            AutoPtr<IViewParent> svp;
            startView->GetParent((IViewParent**)&svp);
            if (svp == NULL) {
                // no parent - safe to use
                overlayView = startView;
            }
            else if (IView::Probe(svp) != NULL) {
                AutoPtr<IViewParent> startp;
                AutoPtr<IView> startParent = IView::Probe(svp);
                AutoPtr<ITransitionValues> startParentValues;
                GetTransitionValues(startParent, TRUE, (ITransitionValues**)&startParentValues);
                AutoPtr<ITransitionValues> endParentValues;
                GetMatchedTransitionValues(startParent, TRUE, (ITransitionValues**)&endParentValues);
                AutoPtr<VisibilityInfo> parentVisibilityInfo =
                        GetVisibilityChangeInfo(startParentValues, endParentValues);
                if (!parentVisibilityInfo->mVisibilityChange) {
                    AutoPtr<ITransitionUtils> utils;
                    CTransitionUtils::AcquireSingleton((ITransitionUtils**)&utils);
                    utils->CopyViewImage(sceneRoot, startView,
                            startParent, (IView**)&overlayView);
                }
                else if ((startParent->GetParent((IViewParent**)&startp), startp) == NULL) {
                    Int32 id = 0;
                    startParent->GetId(&id);
                    AutoPtr<IView> v;
                    if (id != IView::NO_ID && (IView::Probe(sceneRoot)->FindViewById(id, (IView**)&v), v) != NULL
                            && mCanRemoveViews) {
                        // no parent, but its parent is unparented  but the parent
                        // hierarchy has been replaced by a new hierarchy with the same id
                        // and it is safe to un-parent startView
                        overlayView = startView;
                    }
                }
            }
        }
    }
    else {
        // visibility change
        if (endVisibility == IView::INVISIBLE) {
            viewToKeep = endView;
        }
        else {
            // Becoming GONE
            if (startView == endView) {
                viewToKeep = endView;
            }
            else {
                overlayView = startView;
            }
        }
    }
    Int32 finalVisibility = endVisibility;
    AutoPtr<IViewGroup> finalSceneRoot = sceneRoot;

    if (overlayView != NULL) {
        // TODO: Need to do this for general case of adding to overlay
        AutoPtr<ICharSequence> pro_loc;
        CString::New(PROPNAME_SCREEN_LOCATION, (ICharSequence**)&pro_loc);
        AutoPtr<IInterface> locs;
        csv->mValues->Get(pro_loc, (IInterface**)&locs);
        AutoPtr<ArrayOf<Int32> > screenLoc;// = (Int32[]) locs;
        Int32 screenX = (*screenLoc)[0];
        Int32 screenY = (*screenLoc)[1];
        AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(2);
        IView::Probe(sceneRoot)->GetLocationOnScreen(temp);
        Int32 l = 0, t = 0;
        overlayView->GetLeft(&l);
        overlayView->GetTop(&t);
        overlayView->OffsetLeftAndRight((screenX - (*temp)[0]) - l);
        overlayView->OffsetTopAndBottom((screenY - (*temp)[1]) - t);
        AutoPtr<IViewOverlay> ol;
        IView::Probe(sceneRoot)->GetOverlay((IViewOverlay**)&ol);
        ol->Add(IDrawable::Probe(overlayView));
        AutoPtr<IAnimator> animator;
        OnDisappear(sceneRoot, overlayView, startValues, endValues, (IAnimator**)&animator);
        if (animator == NULL) {
            AutoPtr<IViewOverlay> ol;
            IView::Probe(sceneRoot)->GetOverlay((IViewOverlay**)&ol);
            ol->Remove(IDrawable::Probe(overlayView));
        }
        else {
            AutoPtr<IView> finalOverlayView = overlayView;
            AutoPtr<AnimatorListenerAdapter_1> pALA = new AnimatorListenerAdapter_1(finalSceneRoot, finalOverlayView);
            animator->AddListener(IAnimatorListener::Probe(pALA));
        }
        *result = animator;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    if (viewToKeep != NULL) {
        Int32 originalVisibility = -1;
        Boolean isForcedVisibility = mForcedStartVisibility != -1 ||
                mForcedEndVisibility != -1;
        if (!isForcedVisibility) {
            viewToKeep->GetVisibility(&originalVisibility);
            viewToKeep->SetVisibility(IView::VISIBLE);
        }
        AutoPtr<IAnimator> animator;
        OnDisappear(sceneRoot, viewToKeep, startValues, endValues, (IAnimator**)&animator);
        if (animator != NULL) {
            AutoPtr<IView> finalViewToKeep = viewToKeep;
            AutoPtr<AnimatorListenerAdapter_2> pALA = new AnimatorListenerAdapter_2(isForcedVisibility, finalViewToKeep, finalVisibility);
            animator->AddListener(IAnimatorListener::Probe(pALA));
        }
        else if (!isForcedVisibility) {
            viewToKeep->SetVisibility(originalVisibility);
        }
        *result = animator;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode Visibility::AreValuesChanged(
    /* [in] */ ITransitionValues* oldValues,
    /* [in] */ ITransitionValues* newValues,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<VisibilityInfo> changeInfo = GetVisibilityChangeInfo(oldValues, newValues);
    if (oldValues == NULL && newValues == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    *result = changeInfo->mVisibilityChange && (changeInfo->mStartVisibility == IView::VISIBLE ||
        changeInfo->mEndVisibility == IView::VISIBLE);
    return NOERROR;
}

ECode Visibility::OnDisappear(
    /* [in] */ IViewGroup* sceneRoot,
    /* [in] */ IView* view,
    /* [in] */ ITransitionValues* startValues,
    /* [in] */ ITransitionValues* endValues,
    /* [out] */ IAnimator** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NULL;
    return NOERROR;
}

//===============================================================
// Visibility::AnimatorListenerAdapter_1::
//===============================================================
Visibility::AnimatorListenerAdapter_1::AnimatorListenerAdapter_1(
    /* [in] */ IViewGroup* finalSR,
    /* [in] */ IView* finalOV)
{
    mFinalSceneRoot = finalSR;
    mFinalOverlayView = finalOV;
}

ECode Visibility::AnimatorListenerAdapter_1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IViewOverlay> ol;
    IView::Probe(mFinalSceneRoot)->GetOverlay((IViewOverlay**)&ol);
    ol->Remove(IDrawable::Probe(mFinalOverlayView));
    return NOERROR;
}

//===============================================================
// Visibility::AnimatorListenerAdapter_2::
//===============================================================
Visibility::AnimatorListenerAdapter_2::AnimatorListenerAdapter_2(
    /* [in] */ Boolean isFV,
    /* [in] */ IView* fVT,
    /* [in] */ Int32 fV)
{
    mCanceled = FALSE;
    mIsForcedVisibility = isFV;
    mFinalViewToKeep = fVT;
    mFinalVisibility = fV;
}

ECode Visibility::AnimatorListenerAdapter_2::OnAnimationPause(
    /* [in] */ IAnimator* animation)
{
    if (!mCanceled && !mIsForcedVisibility) {
        mFinalViewToKeep->SetVisibility(mFinalVisibility);
    }
    return NOERROR;
}

ECode Visibility::AnimatorListenerAdapter_2::OnAnimationResume(
    /* [in] */ IAnimator* animation)
{
    if (!mCanceled && !mIsForcedVisibility) {
        mFinalViewToKeep->SetVisibility(IView::VISIBLE);
    }
    return NOERROR;
}

ECode Visibility::AnimatorListenerAdapter_2::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCanceled = TRUE;
    return NOERROR;
}

ECode Visibility::AnimatorListenerAdapter_2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mCanceled) {
        if (mIsForcedVisibility) {
            mFinalViewToKeep->SetTransitionAlpha(0);
        }
        else {
            mFinalViewToKeep->SetVisibility(mFinalVisibility);
        }
    }
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
