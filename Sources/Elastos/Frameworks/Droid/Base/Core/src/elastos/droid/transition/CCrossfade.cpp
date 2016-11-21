
#include "elastos/droid/transition/CCrossfade.h"
#include "elastos/droid/transition/CTransitionValues.h"
#include "elastos/droid/animation/RectEvaluator.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/view/View.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::RectEvaluator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::ITextureView;

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Transition {

const String CCrossfade::TAG("Crossfade");

const String CCrossfade::PROPNAME_BITMAP("android:crossfade:bitmap");
const String CCrossfade::PROPNAME_DRAWABLE("android:crossfade:drawable");
const String CCrossfade::PROPNAME_BOUNDS("android:crossfade:bounds");

AutoPtr<ITypeEvaluator> CCrossfade::sRectEvaluator = new RectEvaluator();

//===============================================================
// CCrossfade::
//===============================================================
CAR_OBJECT_IMPL(CCrossfade)

CAR_INTERFACE_IMPL(CCrossfade, Transition, ICrossfade)

ECode CCrossfade::SetFadeBehavior(
    /* [in] */ Int32 fadeBehavior,
    /* [out] */ ICrossfade** result)
{
    VALIDATE_NOT_NULL(result)

    if (fadeBehavior >= FADE_BEHAVIOR_CROSSFADE && fadeBehavior <= FADE_BEHAVIOR_OUT_IN) {
        mFadeBehavior = fadeBehavior;
    }
    *result = this;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CCrossfade::GetFadeBehavior(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mFadeBehavior;
    return NOERROR;
}

ECode CCrossfade::SetResizeBehavior(
    /* [in] */ Int32 resizeBehavior,
    /* [out] */ ICrossfade** result)
{
    VALIDATE_NOT_NULL(result)

    if (resizeBehavior >= RESIZE_BEHAVIOR_NONE && resizeBehavior <= RESIZE_BEHAVIOR_SCALE) {
        mResizeBehavior = resizeBehavior;
    }
    *result = this;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CCrossfade::GetResizeBehavior(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mResizeBehavior;
    return NOERROR;
}

ECode CCrossfade::CreateAnimator(
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
    Boolean useParentOverlay = mFadeBehavior != FADE_BEHAVIOR_REVEAL;
    AutoPtr<IView> view = cEnd->mView;
    AutoPtr<IMap> startVals = cStart->mValues;
    AutoPtr<IMap> endVals = cEnd->mValues;
    AutoPtr<ICharSequence> pro_bounds;
    CString::New(PROPNAME_BOUNDS, (ICharSequence**)&pro_bounds);
    AutoPtr<IInterface> sB;
    startVals->Get(pro_bounds, (IInterface**)&sB);
    AutoPtr<IRect> startBounds = IRect::Probe(sB);
    AutoPtr<IInterface> eB;
    endVals->Get(pro_bounds, (IInterface**)&eB);
    AutoPtr<IRect> endBounds = IRect::Probe(eB);
    AutoPtr<ICharSequence> pro_bmp;
    CString::New(PROPNAME_BITMAP, (ICharSequence**)&pro_bmp);
    AutoPtr<IInterface> sBmp;
    startVals->Get(pro_bmp, (IInterface**)&sBmp);
    AutoPtr<IBitmap> startBitmap = IBitmap::Probe(sBmp);
    AutoPtr<IInterface> eBmp;
    endVals->Get(pro_bmp, (IInterface**)&eBmp);
    AutoPtr<IBitmap> endBitmap = IBitmap::Probe(eBmp);
    AutoPtr<ICharSequence> pro_drawable;
    CString::New(PROPNAME_DRAWABLE, (ICharSequence**)&pro_drawable);
    AutoPtr<IInterface> sD;
    startVals->Get(pro_drawable, (IInterface**)&sD);
    AutoPtr<IBitmapDrawable> startDrawable = IBitmapDrawable::Probe(sD);
    AutoPtr<IInterface> eD;
    endVals->Get(pro_drawable, (IInterface**)&eD);
    AutoPtr<IBitmapDrawable> endDrawable = IBitmapDrawable::Probe(eD);
    if (Transition::DBG) {
        Boolean b = FALSE;
        Logger::D(TAG, "StartBitmap.sameAs(endBitmap) = %d for start, end: %p, %p",
                    (startBitmap->SameAs(endBitmap, &b), b), startBitmap.Get(), endBitmap.Get());
    }
    Boolean bSame = FALSE;
    if (startDrawable != NULL && endDrawable != NULL && !(startBitmap->SameAs(endBitmap, &bSame), bSame)) {
        AutoPtr<IViewOverlay> overlay;
        if (useParentOverlay) {
            AutoPtr<IViewParent> vp;
            view->GetParent((IViewParent**)&vp);
            (IView::Probe(vp))->GetOverlay((IViewOverlay**)&overlay);
        }
        else {
            view->GetOverlay((IViewOverlay**)&overlay);
        }
        if (mFadeBehavior == FADE_BEHAVIOR_REVEAL) {
            overlay->Add(IDrawable::Probe(endDrawable));
        }
        overlay->Add(IDrawable::Probe(startDrawable));
        // The transition works by placing the end drawable under the start drawable and
        // gradually fading out the start drawable. So it's not really a cross-fade, but rather
        // a reveal of the end scene over time. Also, animate the bounds of both drawables
        // to mimic the change in the size of the view itself between scenes.
        AutoPtr<IObjectAnimator> anim;
        if (mFadeBehavior == FADE_BEHAVIOR_OUT_IN) {
            // Fade out completely halfway through the transition
            AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(3);
            (*arr)[0] = 255;
            (*arr)[1] = 0;
            (*arr)[2] = 0;
            anim = ObjectAnimator::OfInt32(startDrawable, String("alpha"), arr);
        }
        else {
            AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(1);
            (*arr)[0] = 0;
            anim = ObjectAnimator::OfInt32(startDrawable, String("alpha"), arr);
        }
        IValueAnimator::Probe(anim)->AddUpdateListener(new AnimatorUpdateListenerOverride(startDrawable, view));
        AutoPtr<IObjectAnimator> anim1 = NULL;
        if (mFadeBehavior == FADE_BEHAVIOR_OUT_IN) {
            AutoPtr<ArrayOf<Float> > arr = ArrayOf<Float>::Alloc(3);
            (*arr)[0] = 0.0f;
            (*arr)[1] = 0.0f;
            (*arr)[2] = 1.0f;
            // start fading in halfway through the transition
            anim1 = ObjectAnimator::OfFloat(view, Elastos::Droid::View::View::ALPHA, arr);
        }
        else if (mFadeBehavior == FADE_BEHAVIOR_CROSSFADE) {
            AutoPtr<ArrayOf<Float> > arr = ArrayOf<Float>::Alloc(2);
            (*arr)[0] = 0.0f;
            (*arr)[1] = 1.0f;
            anim1 = ObjectAnimator::OfFloat(view, Elastos::Droid::View::View::ALPHA, arr);
        }
        if (Transition::DBG) {
            Logger::D(TAG, "Crossfade: created anim %p for start, end values %p, %p",
                                anim.Get(), startValues, endValues);
        }
        AutoPtr<AnimatorListenerAdapter_1> p = new AnimatorListenerAdapter_1(useParentOverlay,
                                                                            view,
                                                                            startDrawable,
                                                                            endDrawable,
                                                                            mFadeBehavior);
        IAnimator::Probe(anim)->AddListener(IAnimatorListener::Probe(p));
        AutoPtr<IAnimatorSet> set;
        CAnimatorSet::New((IAnimatorSet**)&set);
        AutoPtr<ArrayOf<IAnimator*> > arr = ArrayOf<IAnimator*>::Alloc(1);
        (*arr)[0] = IAnimator::Probe(anim);
        set->PlayTogether(arr);
        if (anim1 != NULL) {
            AutoPtr<ArrayOf<IAnimator*> > arr1 = ArrayOf<IAnimator*>::Alloc(1);
            (*arr1)[0] = IAnimator::Probe(anim1);
            set->PlayTogether(arr1);
        }
        Boolean bEqual = FALSE;
        if (mResizeBehavior == RESIZE_BEHAVIOR_SCALE &&
             !(startBounds->Equals(endBounds, &bEqual), bEqual)) {
            if (Transition::DBG) {
                Logger::D(TAG, "animating from startBounds to endBounds: %p, %p",
                        startBounds.Get(), endBounds.Get());
            }
            AutoPtr<ArrayOf<IInterface*> > vls = ArrayOf<IInterface*>::Alloc(2);
            (*vls)[0] = startBounds;
            (*vls)[1] = endBounds;
            AutoPtr<IObjectAnimator> objectAnim2 = ObjectAnimator::OfObject(startDrawable, String("bounds"),
                    sRectEvaluator, vls);
            AutoPtr<IAnimator> anim2 = IAnimator::Probe(objectAnim2);
            AutoPtr<ArrayOf<IAnimator*> > arr2 = ArrayOf<IAnimator*>::Alloc(1);
            (*arr2)[0] = IAnimator::Probe(anim2);
            set->PlayTogether(arr2);
            if (mResizeBehavior == RESIZE_BEHAVIOR_SCALE) {
                // TODO: How to handle resizing with a CROSSFADE (vs. REVEAL) effect
                // when we are animating the view directly?
                AutoPtr<IObjectAnimator> objectAnim3 = ObjectAnimator::OfObject(endDrawable, String("bounds"),
                        sRectEvaluator, vls);
                AutoPtr<IAnimator> anim3 = IAnimator::Probe(objectAnim3);
                AutoPtr<ArrayOf<IAnimator*> > arr3 = ArrayOf<IAnimator*>::Alloc(1);
                (*arr3)[0] = IAnimator::Probe(anim3);
                set->PlayTogether(arr3);
            }
        }
        *result = IAnimator::Probe(set);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        *result = NULL;
        return NOERROR;
    }
}

ECode CCrossfade::CaptureValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    AutoPtr<CTransitionValues> ctv = (CTransitionValues*)transitionValues;
    AutoPtr<IView> view = ctv->mView;
    Int32 w = 0, h = 0;
    view->GetWidth(&w);
    view->GetHeight(&h);
    AutoPtr<IRect> bounds;
    CRect::New(0, 0, w, h, (IRect**)&bounds);
    if (mFadeBehavior != FADE_BEHAVIOR_REVEAL) {
        Int32 l = 0, t = 0;
        view->GetLeft(&l);
        view->GetTop(&t);
        bounds->Offset(l, t);
    }
    AutoPtr<ICharSequence> pro_bounds;
    CString::New(PROPNAME_BOUNDS, (ICharSequence**)&pro_bounds);
    ctv->mValues->Put(pro_bounds, bounds);

    if (Transition::DBG) {
        AutoPtr<IInterface> p;
        ctv->mValues->Get(pro_bounds, (IInterface**)&p);
        Logger::D(TAG, "Captured bounds %p", p.Get());
    }
    Int32 w1 = 0, h1 = 0;
    view->GetWidth(&w1);
    view->GetHeight(&h1);
    AutoPtr<IBitmap> bitmap;
    CBitmap::CreateBitmap(w1, h1, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
    if (ITextureView::Probe(view) != NULL) {
        ITextureView::Probe(view)->GetBitmap((IBitmap**)&bitmap);
    }
    else {
        AutoPtr<ICanvas> c;
        CCanvas::New(bitmap, (ICanvas**)&c);
        view->Draw(c);
    }
    AutoPtr<ICharSequence> pro_bmp;
    CString::New(PROPNAME_BITMAP, (ICharSequence**)&pro_bmp);
    ctv->mValues->Put(pro_bmp, bitmap);
    // TODO: I don't have resources, can't call the non-deprecated method?
    AutoPtr<IDrawable> drawable;
    CBitmapDrawable::New(bitmap, (IDrawable**)&drawable);
    // TODO: lrtb will be wrong if the view has transXY set
    drawable->SetBounds(bounds);
    AutoPtr<ICharSequence> pro_drawable;
    CString::New(PROPNAME_DRAWABLE, (ICharSequence**)&pro_drawable);
    ctv->mValues->Put(pro_drawable, drawable);
    return NOERROR;
}

ECode CCrossfade::CaptureStartValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    return CaptureValues(transitionValues);
}

ECode CCrossfade::CaptureEndValues(
    /* [in] */ ITransitionValues* transitionValues)
{
    return CaptureValues(transitionValues);
}

ECode CCrossfade::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<ICrossfade> changes;
    CCrossfade::New((ICrossfade**)&changes);

    CloneImpl(changes);
    *obj = changes;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CCrossfade::CloneImpl(
    /* [in] */ ICrossfade* obj)
{
    VALIDATE_NOT_NULL(obj);

    return Transition::CloneImpl(ITransition::Probe(obj));
}

//===============================================================
// CCrossfade::AnimatorUpdateListenerOverride::
//===============================================================
CAR_INTERFACE_IMPL(CCrossfade::AnimatorUpdateListenerOverride, Object, IAnimatorUpdateListener)

CCrossfade::AnimatorUpdateListenerOverride::AnimatorUpdateListenerOverride(
    /* [in] */ IBitmapDrawable* startDrawable,
    /* [in] */ IView* view)
{
    mStartDrawable = startDrawable;
    mView = view;
}

ECode CCrossfade::AnimatorUpdateListenerOverride::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
 {
    // TODO: some way to auto-invalidate views based on drawable changes? callbacks?
    AutoPtr<IRect> rt;
    IDrawable::Probe(mStartDrawable)->GetBounds((IRect**)&rt);
    mView->Invalidate(rt);
    return NOERROR;
}

//===============================================================
// CCrossfade::AnimatorListenerAdapter_1::
//===============================================================
CCrossfade::AnimatorListenerAdapter_1::AnimatorListenerAdapter_1(
    /* [in] */ Boolean useParentOverlay,
    /* [in] */ IView* view,
    /* [in] */ IBitmapDrawable* startDrawable,
    /* [in] */ IBitmapDrawable* endDrawable,
    /* [in] */ Int32 fadeBehavior)
{
    mUseParentOverlay = useParentOverlay;
    mView = view;
    mStartDrawable = startDrawable;
    mEndDrawable = endDrawable;
    mFadeBehavior = fadeBehavior;
}

ECode CCrossfade::AnimatorListenerAdapter_1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IViewOverlay> overlay;
    if (mUseParentOverlay) {
        AutoPtr<IViewParent> sp;
        mView->GetParent((IViewParent**)&sp);
        (IView::Probe(sp))->GetOverlay((IViewOverlay**)&overlay);
    }
    else {
        mView->GetOverlay((IViewOverlay**)&overlay);
    }
    overlay->Remove(IDrawable::Probe(mStartDrawable));
    if (mFadeBehavior == FADE_BEHAVIOR_REVEAL) {
        overlay->Remove(IDrawable::Probe(mEndDrawable));
    }
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
