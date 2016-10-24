
#include "elastos/droid/view/COverlayViewGroup.h"

using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(COverlayViewGroup)

CAR_INTERFACE_IMPL(COverlayViewGroup, ViewGroup, IOverlayViewGroup)

ECode COverlayViewGroup::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* hostView)
{
    ViewGroup::constructor(context);
    mHostView = hostView;
    mAttachInfo = VIEW_PROBE(mHostView)->mAttachInfo;
    if (mAttachInfo != NULL) mAttachInfo->mViewRootImpl->AddRef();   // see View::DispatchAttachedToWindow and View::DispatchDetachedFromWindow
    hostView->GetWidth(&mRight);
    hostView->GetHeight(&mBottom);
    return NOERROR;
}

ECode COverlayViewGroup::Add(
    /* [in] */ IDrawable* drawable)
{
    if (mDrawables == NULL) {
        CArrayList::New((IArrayList**)&mDrawables);
    }
    Boolean value = FALSE;
    if (mDrawables->Contains(drawable, &value), !value) {
        // Make each drawable unique in the overlay; can't add it more than once
        mDrawables->Add(drawable);
        AutoPtr<IRect> bounds;
        drawable->GetBounds((IRect**)&bounds);
        Invalidate(bounds);
        drawable->SetCallback(this);
    }
    return NOERROR;
}

ECode COverlayViewGroup::Remove(
    /* [in] */ IDrawable* drawable)
{
    if (mDrawables != NULL) {
        mDrawables->Remove(drawable);
        AutoPtr<IRect> bounds;
        drawable->GetBounds((IRect**)&bounds);
        Invalidate(bounds);
        drawable->SetCallback(NULL);
    }
    return NOERROR;
}

Boolean COverlayViewGroup::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    Boolean value = FALSE;
    return ViewGroup::VerifyDrawable(who) || (mDrawables != NULL && (mDrawables->Contains(who, &value), value));
}

ECode COverlayViewGroup::Add(
    /* [in] */ IView* child)
{
    AutoPtr<IViewParent> vp;
    child->GetParent((IViewParent**)&vp);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(vp);
    if (parent) {
        AutoPtr<IViewParent> temp;
        vp->GetParent((IViewParent**)&temp);
        vp = temp;
        if (IView::Probe(parent) != mHostView && vp.Get() != NULL &&
                ((ViewGroup*)parent.Get())->mAttachInfo != NULL) {
            // Moving to different container; figure out how to position child such that
            // it is in the same location on the screen
            AutoPtr<ArrayOf<Int32> > parentLocation = ArrayOf<Int32>::Alloc(2);
            AutoPtr<ArrayOf<Int32> > hostViewLocation = ArrayOf<Int32>::Alloc(2);
            IView::Probe(parent)->GetLocationOnScreen(parentLocation);
            mHostView->GetLocationOnScreen(hostViewLocation);
            child->OffsetLeftAndRight((*parentLocation)[0] - (*hostViewLocation)[0]);
            child->OffsetTopAndBottom((*parentLocation)[1] - (*hostViewLocation)[1]);
        }
        IViewManager::Probe(parent)->RemoveView(child);
        AutoPtr<ILayoutTransition> lt;
        if ((parent->GetLayoutTransition((ILayoutTransition**)&lt), lt.Get()) != NULL) {
            // LayoutTransition will cause the child to delay removal - cancel it
            lt->Cancel(ILayoutTransition::DISAPPEARING);
        }
        // fail-safe if view is still attached for any reason
        vp = NULL;
        if ((child->GetParent((IViewParent**)&vp), vp.Get()) != NULL) {
            VIEW_PROBE(child)->mParent = NULL;
        }
    }
    return ViewGroup::AddView(child);
}

ECode COverlayViewGroup::Remove(
    /* [in] */ IView* view)
{
    return ViewGroup::RemoveView(view);
}

ECode COverlayViewGroup::Clear()
{
    RemoveAllViews();
    if (mDrawables != NULL) {
        mDrawables->Clear();
    }
    return NOERROR;
}

ECode COverlayViewGroup::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);
    Int32 value = 0;
    if ((GetChildCount(&value), value) == 0 &&
            (mDrawables == NULL || (mDrawables->GetSize(&value), value) == 0)) {
        *empty = TRUE;
        return NOERROR;
    }
    *empty = FALSE;
    return NOERROR;
}

ECode COverlayViewGroup::InvalidateDrawable(
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IRect> bounds;
    drawable->GetBounds((IRect**)&bounds);
    Invalidate(bounds);
    return NOERROR;
}

ECode COverlayViewGroup::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    ViewGroup::DispatchDraw(canvas);
    Int32 numDrawables = 0;
    if (mDrawables != NULL) {
        mDrawables->GetSize(&numDrawables);
    }

    for (Int32 i = 0; i < numDrawables; ++i) {
        AutoPtr<IInterface> item;
        mDrawables->Get(i, (IInterface**)&item);
        IDrawable::Probe(item)->Draw(canvas);
    }
    return NOERROR;
}

ECode COverlayViewGroup::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // Noop: children are positioned absolutely
    return NOERROR;
}

ECode COverlayViewGroup::Invalidate(
    /* [in] */ IRect* dirty)
{
    ViewGroup::Invalidate(dirty);
    if (mHostView != NULL) {
        mHostView->Invalidate(dirty);
    }
    return NOERROR;
}

ECode COverlayViewGroup::Invalidate(
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    ViewGroup::Invalidate(l, t, r, b);
    if (mHostView != NULL) {
        mHostView->Invalidate(l, t, r, b);
    }
    return NOERROR;
}

ECode COverlayViewGroup::Invalidate()
{
    ViewGroup::Invalidate();
    if (mHostView != NULL) {
        mHostView->Invalidate();
    }
    return NOERROR;
}

void COverlayViewGroup::Invalidate(
    /* [in] */ Boolean invalidateCache)
{
    ViewGroup::Invalidate(invalidateCache);
    if (mHostView != NULL) {
        VIEW_PROBE(mHostView)->Invalidate(invalidateCache);
    }
}

void COverlayViewGroup::InvalidateViewProperty(
    /* [in] */ Boolean invalidateParent,
    /* [in] */ Boolean forceRedraw)
{
    ViewGroup::InvalidateViewProperty(invalidateParent, forceRedraw);
    if (mHostView != NULL) {
        VIEW_PROBE(mHostView)->InvalidateViewProperty(invalidateParent, forceRedraw);
    }
}

void COverlayViewGroup::InvalidateParentCaches()
{
    ViewGroup::InvalidateParentCaches();
    if (mHostView != NULL) {
        VIEW_PROBE(mHostView)->InvalidateParentCaches();
    }
}

void COverlayViewGroup::InvalidateParentIfNeeded()
{
    ViewGroup::InvalidateParentIfNeeded();
    if (mHostView != NULL) {
        VIEW_PROBE(mHostView)->InvalidateParentIfNeeded();
    }
}

ECode COverlayViewGroup::DamageChild(
    /* [in] */ IView* child,
    /* [in] */ IRect* dirty)
{
    if (mHostView != NULL) {
        // Note: This is not a "fast" invalidation. Would be nice to instead invalidate
        // using DisplayList properties and a dirty rect instead of causing a real
        // invalidation of the host view
        Int32 left = VIEW_PROBE(child)->mLeft;
        Int32 top = VIEW_PROBE(child)->mTop;
        AutoPtr<IMatrix> matrix;
        child->GetMatrix((IMatrix**)&matrix);
        Boolean value = FALSE;
        if (matrix->IsIdentity(&value), !value) {
            VIEW_PROBE(child)->TransformRect(dirty);
        }
        dirty->Offset(left, top);
        mHostView->Invalidate(dirty);
    }
    return NOERROR;
}

AutoPtr<IViewParent> COverlayViewGroup::DamageChildInParent(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ IRect* dirty)
{
    if (IViewGroup::Probe(mHostView)) {
        return ((ViewGroup*) IViewGroup::Probe(mHostView))->DamageChildInParent(left, top, dirty);
    }
    return NULL;
}

ECode COverlayViewGroup::InvalidateChildInParent(
    /* [in] */ ArrayOf<Int32>* location,
    /* [in] */ IRect* dirty,
    /* [out] */ IViewParent** parent)
{
    VALIDATE_NOT_NULL(parent);
    if (mHostView != NULL) {
        dirty->Offset((*location)[0], (*location)[1]);
        if (IViewGroup::Probe(mHostView)) {
            (*location)[0] = 0;
            (*location)[1] = 0;
            AutoPtr<IViewParent> p;
            ViewGroup::InvalidateChildInParent(location, dirty, (IViewParent**)&p);
            p = NULL;
            IViewParent::Probe(mHostView)->InvalidateChildInParent(location, dirty, (IViewParent**)&p);
            *parent = p;
            REFCOUNT_ADD(*parent);
            return NOERROR;
        } else {
            Invalidate(dirty);
        }
    }
    *parent = NULL;
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
