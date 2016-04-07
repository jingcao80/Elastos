
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/view/GhostView.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/view/COverlayViewGroup.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::COverlayViewGroup;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(GhostView, View, IGhostView)

GhostView::GhostView()
    : mView(NULL)
    , mReferences(0)
    , mBeingMoved(FALSE)
{}

ECode GhostView::constructor(
    /* [in] */ IView* view)
{
    AutoPtr<IContext> ctx;
    view->GetContext((IContext**)&ctx);
    View::constructor(ctx);
    mView = view;
    VIEW_PROBE(mView)->mGhostView = IGhostView::Probe(this);
    AutoPtr<IViewParent> temp;
    mView->GetParent((IViewParent**)&temp);
    SetGhostedVisibility(IView::INVISIBLE);
    IViewGroup* parent = IViewGroup::Probe(temp);
    ((ViewGroup*)parent)->mRecreateDisplayList = TRUE;
    AutoPtr<IRenderNode> result;
    IView::Probe(parent)->GetDisplayList((IRenderNode**)&result);
    return NOERROR;
}

void GhostView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (IHardwareCanvas::Probe(canvas)) {
        AutoPtr<IHardwareCanvas> hwCanvas = IHardwareCanvas::Probe(canvas);
        VIEW_PROBE(mView)->mRecreateDisplayList = TRUE;
        AutoPtr<IRenderNode> renderNode;
        mView->GetDisplayList((IRenderNode**)&renderNode);
        Boolean isValid;
        if (renderNode->IsValid(&isValid), isValid) {
            ICanvas::Probe(hwCanvas)->InsertReorderBarrier(); // enable shadow for this rendernode
            hwCanvas->DrawRenderNode(renderNode);
            ICanvas::Probe(hwCanvas)->InsertInorderBarrier(); // re-disable reordering/shadows
        }
    }
}

ECode GhostView::SetMatrix(
    /* [in] */ IMatrix* matrix)
{
    Boolean res;
    mRenderNode->SetAnimationMatrix(matrix, &res);
    return NOERROR;
}

ECode GhostView::SetVisibility(
    /* [in] */ Int32 visibility)
{
    View::SetVisibility(visibility);
    if(IGhostView::Probe(this) == VIEW_PROBE(mView)->mGhostView) {
        Int32 inverseVisibility = (visibility == IView::VISIBLE) ? IView::INVISIBLE : IView::VISIBLE;
        SetGhostedVisibility(inverseVisibility);
    }
    return NOERROR;
}

void GhostView::SetGhostedVisibility(
    /* [in] */ Int32 visibility)
{
    VIEW_PROBE(mView)->mViewFlags = (VIEW_PROBE(mView)->mViewFlags & ~View::VISIBILITY_MASK) | visibility;
}

ECode GhostView::OnDetachedFromWindow()
{
    View::OnDetachedFromWindow();
    if (!mBeingMoved) {
        SetGhostedVisibility(IView::VISIBLE);
        VIEW_PROBE(mView)->mGhostView = NULL;
        AutoPtr<IViewParent> temp;
        mView->GetParent((IViewParent**)&temp);
        IViewGroup* parent = IViewGroup::Probe(temp);
        if (parent != NULL) {
            ((ViewGroup*)parent)->mRecreateDisplayList = TRUE;
            AutoPtr<IRenderNode> result;
            IView::Probe(parent)->GetDisplayList((IRenderNode**)&result);
        }
    }
    return NOERROR;
}

void GhostView::CalculateMatrix(
    /* [in] */ IView* view,
    /* [in] */ IViewGroup* host,
    /* [in] */ IMatrix* matrix)
{
    AutoPtr<IViewParent> temp;
    view->GetParent((IViewParent**)&temp);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(temp);
    matrix->Reset();
    IView::Probe(parent)->TransformMatrixToGlobal(matrix);
    Int32 x, y;
    IView::Probe(parent)->GetScrollX(&x);
    IView::Probe(parent)->GetScrollY(&y);
    Boolean result;
    matrix->PreTranslate(-x, -y, &result);
    IView::Probe(host)->TransformMatrixToLocal(matrix);
}

ECode GhostView::AddGhost(
    /* [in] */ IView* view,
    /* [in] */ IViewGroup* viewGroup,
    /* [in] */ IMatrix* matrix,
    /* [out] */ IGhostView** res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<IViewParent> parent;
    view->GetParent((IViewParent**)&parent);
    if (IViewGroup::Probe(parent) == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IViewOverlay> obj;
    IView::Probe(viewGroup)->GetOverlay((IViewOverlay**)&obj);
    IViewGroupOverlay* overlay = IViewGroupOverlay::Probe(obj);
    assert(0);
    //AutoPtr<IOverlayViewGroup> overlayViewGroup = ((CViewGroupOverlay*)overlay.Get())->mOverlayViewGroup;
    AutoPtr<IGhostView> ghostView = VIEW_PROBE(view)->mGhostView;
    Int32 previousRefCount = 0;
    if (NULL != ghostView) {
        AutoPtr<IViewParent> temp;
        IView::Probe(ghostView)->GetParent((IViewParent**)&temp);
        AutoPtr<IView> oldParent = IView::Probe(temp);
        temp = NULL;
        oldParent->GetParent((IViewParent**)&temp);
        IViewGroup* oldGrandParent = IViewGroup::Probe(temp);
        assert(0);
        if (TRUE/*oldGrandParent != IViewGroup::Probe(overlayViewGroup)*/) {
            previousRefCount = ((GhostView*)ghostView.Get())->mReferences;
            ((ViewGroup*)oldGrandParent)->RemoveView(oldParent);
            ghostView = NULL;
        }
    }

    if(NULL == ghostView) {
        if (NULL == matrix) {
            AutoPtr<IMatrix> tempMatrix;
            CMatrix::New((IMatrix**)&tempMatrix);
            matrix = tempMatrix;
            CalculateMatrix(view, viewGroup, matrix);
        }
        assert(0);
        //CGhostView::New(view, (IGhostView**)&ghostView);
        //ghostView->SetMatrix(matrix);
        AutoPtr<IContext> ctx;
        view->GetContext((IContext**)&ctx);
        AutoPtr<IFrameLayout> parent;
        //CFrameLayout::New(ctx, (IFrameLayout**)&parent);
        IViewGroup::Probe(parent)->SetClipChildren(FALSE);
        CopySize(IView::Probe(viewGroup), IView::Probe(parent));
        CopySize(IView::Probe(viewGroup), IView::Probe(ghostView));
        IViewGroup::Probe(parent)->AddView(IView::Probe(ghostView));
        AutoPtr<IArrayList> tempViews;
        CArrayList::New((IArrayList**)&tempViews);
        assert(0);
        /*Int32 firstGhost = MoveGhostViewsToTop(((CViewGroupOverlay*)overlay.Get())->mOverlayViewGroup, tempViews);
        InsertIntoOverlay(((CViewGroupOverlay*)overlay.Get())->mOverlayViewGroup,
            parent, ghostView, tempViews, firstGhost);*/
        ((GhostView*)ghostView.Get())->mReferences = previousRefCount;
    } else if (NULL != matrix) {
        assert(0);
        //ghostView->SetMatrix(matrix);
    }
    ((GhostView*)ghostView.Get())->mReferences++;
    *res = ghostView;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

AutoPtr<IGhostView> GhostView::AddGhost(
    /* [in] */ IView* view,
    /* [in] */ IViewGroup* viewGroup)
{
    AutoPtr<IGhostView> result;
    AddGhost(view, viewGroup, NULL, (IGhostView**)&result);
    return result;
}

void GhostView::RemoveGhost(
    /* [in] */ IView* view)
{
    AutoPtr<IGhostView> ghostView = VIEW_PROBE(view)->mGhostView;
    if (NULL != ghostView) {
        ((GhostView*)ghostView.Get())->mReferences--;
        if (((GhostView*)ghostView.Get())->mReferences == 0) {
            AutoPtr<IViewParent> temp;
            IView::Probe(ghostView)->GetParent((IViewParent**)&temp);
            AutoPtr<IViewGroup> parent = IViewGroup::Probe(temp);
            temp = NULL;
            IView::Probe(parent)->GetParent((IViewParent**)&temp);
            IViewGroup* grandParent = IViewGroup::Probe(temp);
            ((ViewGroup*)grandParent)->RemoveView(IView::Probe(parent));
        }
    }
}

AutoPtr<IGhostView> GhostView::GetGhost(
    /* [in] */ IView* view)
{
    return VIEW_PROBE(view)->mGhostView;
}

void GhostView::CopySize(
    /* [in] */ IView* from,
    /* [in] */ IView* to)
{
    to->SetLeft(0);
    to->SetTop(0);
    Int32 width;
    from->GetWidth(&width);
    to->SetRight(width);
    Int32 height;
    from->GetHeight(&height);
    to->SetBottom(height);
}

/**
 * Move the GhostViews to the end so that they are on top of other views and it is easier
 * to do binary search for the correct location for the GhostViews in insertIntoOverlay.
 *
 * @return The index of the first GhostView or -1 if no GhostView is in the ViewGroup
 */
Int32 GhostView::MoveGhostViewsToTop(
    /* [in] */ IViewGroup* viewGroup,
    /* [in] */ IArrayList* tempViews)
{
    Int32 numChildren;
    viewGroup->GetChildCount(&numChildren);
    AutoPtr<IView> child;
    if (numChildren == 0) {
        return -1;
    } else if (IsGhostWrapper((viewGroup->GetChildAt(numChildren - 1, (IView**)&child), child))) {
        Int32 firstGhost = numChildren - 1;
        for (Int32 i = numChildren - 2; i >= 0; i--) {
            child = NULL;
            if (IsGhostWrapper((viewGroup->GetChildAt(i, (IView**)&child), child))) {
                break;
            }
            firstGhost = i;
        }
        return firstGhost;
    }

    // Remove all GhostViews from the middle

    for (Int32 i = numChildren - 2; i >= 0; i--) {
        AutoPtr<IView> tempChild;
        viewGroup->GetChildAt(i, (IView**)&tempChild);
        if (IsGhostWrapper(child)) {
            tempViews->Add(child);
            AutoPtr<IView> tempGhost;
            IViewGroup::Probe(tempChild)->GetChildAt(0, (IView**)&tempGhost);
            AutoPtr<IGhostView> ghostView = IGhostView::Probe(tempGhost);
            ((GhostView*)ghostView.Get())->mBeingMoved = TRUE;
            viewGroup->RemoveViewAt(i);
            ((GhostView*)ghostView.Get())->mBeingMoved = FALSE;
        }
    }

    Int32 firstGhost;
    Boolean isEmpty;
    if (tempViews->IsEmpty(&isEmpty), isEmpty) {
        firstGhost = -1;
    } else {
        viewGroup->GetChildCount(&firstGhost);
        // Add the GhostViews to the end
        Int32 size;
        tempViews->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> it;
            tempViews->Get(i, (IInterface**)&it);
            viewGroup->AddView(IView::Probe(it));
        }
        tempViews->Clear();
    }
    return firstGhost;
}

/**
 * Inserts a GhostView into the overlay's ViewGroup in the order in which they
 * should be displayed by the UI.
 */
void GhostView::InsertIntoOverlay(
    /* [in] */ IViewGroup* viewGroup,
    /* [in] */ IViewGroup* wrapper,
    /* [in] */ IGhostView* ghostView,
    /* [in] */ IArrayList* tempParents,
    /* [in] */ Int32 firstGhost)
{
    if (firstGhost == -1) {
        viewGroup->AddView(IView::Probe(wrapper));
    } else {
        AutoPtr<IArrayList> viewParents;
        CArrayList::New((IArrayList**)&viewParents);
        GetParents(((GhostView*)ghostView)->mView, viewParents);

        Int32 index = GetInsertIndex(viewGroup, viewParents, tempParents, firstGhost);
        Int32 childrenCount;
        if (index < 0 || index >= (viewGroup->GetChildCount(&childrenCount), childrenCount)) {
            viewGroup->AddView(IView::Probe(wrapper));
        } else {
            viewGroup->AddView(IView::Probe(wrapper), index);
        }
    }
}

/**
 * Find the index into the overlay to insert the GhostView based on the order that the
 * views should be drawn. This keeps GhostViews layered in the same order
 * that they are ordered in the UI.
 */
Int32 GhostView::GetInsertIndex(
    /* [in] */ IViewGroup* overlayViewGroup,
    /* [in] */ IArrayList* viewParents,
    /* [in] */ IArrayList* tempParents,
    /* [in] */ Int32 firstGhost)
{
    Int32 low = firstGhost;
    Int32 high;
    overlayViewGroup->GetChildCount(&high);
    high -= 1;

    while (low <= high) {
        Int32 mid = (low + high) / 2;
        AutoPtr<IView> temp;
        overlayViewGroup->GetChildAt(mid, (IView**)&temp);
        AutoPtr<IViewGroup> wrapper = IViewGroup::Probe(temp);
        temp = NULL;
        wrapper->GetChildAt(0, (IView**)&temp);
        AutoPtr<IGhostView> midView = IGhostView::Probe(temp);
        GetParents(((GhostView*)midView.Get())->mView, tempParents);
        if (IsOnTop(viewParents, tempParents)) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
        tempParents->Clear();
    }

    return low;
}

/**
 * Returns true if view is a GhostView's FrameLayout wrapper.
 */
Boolean GhostView::IsGhostWrapper(
    /* [in] */ IView* view)
{
    if (IFrameLayout::Probe(view)) {
        AutoPtr<IFrameLayout> frameLayout = IFrameLayout::Probe(view);
        Int32 childrenCount;
        IViewGroup::Probe(frameLayout)->GetChildCount(&childrenCount);
        if (childrenCount == 1) {
            AutoPtr<IView> child;
            IViewGroup::Probe(frameLayout)->GetChildAt(0, (IView**)&child);
            return NULL != IGhostView::Probe(child);
        }
    }

    return FALSE;
}

/**
 * Returns true if viewParents is from a View that is on top of the comparedWith's view.
 * The ArrayLists contain the ancestors of views in order from top most grandparent, to
 * the view itself, in order. The goal is to find the first matching parent and then
 * compare the draw order of the siblings.
 */
Boolean GhostView::IsOnTop(
    /* [in] */ IArrayList* viewParents,
    /* [in] */ IArrayList* comparedWith)
{
    Boolean left, right;
    AutoPtr<IInterface> leftIt, rightIt;
    if ((viewParents->IsEmpty(&left), left) || (comparedWith->IsEmpty(&right), right)
     || (viewParents->Get(0, (IInterface**)&leftIt), leftIt)
     != (comparedWith->Get(0, (IInterface**)&rightIt), rightIt)) {
        // Not the same decorView -- arbitrary ordering
        return TRUE;
    }
    Int32 leftSize, rightSize;
    viewParents->GetSize(&leftSize);
    comparedWith->GetSize(&rightSize);
    Int32 depth = Elastos::Core::Math::Min(leftSize, rightSize);
    for (Int32 i = 1; i < depth; i++) {
        AutoPtr<IInterface> temp;
        viewParents->Get(i, (IInterface**)&temp);
        AutoPtr<IView> viewParent = IView::Probe(temp);
        temp = NULL;
        comparedWith->Get(i, (IInterface**)&temp);
        AutoPtr<IView> comparedWithParent = IView::Probe(temp);

        if (viewParent != comparedWithParent) {
            return IsOnTop(viewParent, comparedWithParent);
        }
    }

    // one of these is the parent of the other
    Boolean isComparedWithTheParent = (rightSize == depth);
    return isComparedWithTheParent;
}

/**
 * Adds all the parents, grandparents, etc. of view to parents.
 */
void GhostView::GetParents(
    /* [in] */ IView* view,
    /* [in] */ IArrayList* parents)
{
    AutoPtr<IViewParent> parent;
    view->GetParent((IViewParent**)&parent);
    if (NULL != parent && IViewGroup::Probe(parent)) {
        GetParents(IView::Probe(parent), parents);
    }
    parents->Add(view);
}

/**
 * Returns true if view would be drawn on top of comparedWith or false otherwise.
 * view and comparedWith are siblings with the same parent. This uses the logic
 * that dispatchDraw uses to determine which View should be drawn first.
 */
Boolean GhostView::IsOnTop(
    /* [in] */ IView* view,
    /* [in] */ IView* comparedWith)
{
    AutoPtr<IViewParent> temp;
    view->GetParent((IViewParent**)&temp);
    IViewGroup* parent = IViewGroup::Probe(temp);
    ViewGroup* parentVG = (ViewGroup*)parent;

    Int32 childrenCount;
    parent->GetChildCount(&childrenCount);
    AutoPtr<IList> list = parentVG->BuildOrderedChildList();
    IArrayList* preorderedList = IArrayList::Probe(list);

    Boolean customOrder = preorderedList == NULL
        && parentVG->IsChildrenDrawingOrderEnabled();

    // This default value shouldn't be used because both view and comparedWith
    // should be in the list. If there is an error, then just return an arbitrary
    // view is on top.
    Boolean isOnTop = TRUE;
    for (Int32 i = 0; i < childrenCount; i++) {
        Int32 childIndex = customOrder ? parentVG->GetChildDrawingOrder(childrenCount, i) : i;
        AutoPtr<IView> child;
        if (NULL == preorderedList) {
            parent->GetChildAt(childIndex, (IView**)&child);
        } else {
            AutoPtr<IInterface> it;
            preorderedList->Get(childIndex, (IInterface**)&it);
            child = IView::Probe(it);
        }
        if (view == child) {
            isOnTop = FALSE;
            break;
        } else if (comparedWith == child) {
            isOnTop = TRUE;
            break;
        }
    }

    if (NULL != preorderedList) {
        preorderedList->Clear();
    }
    return isOnTop;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
