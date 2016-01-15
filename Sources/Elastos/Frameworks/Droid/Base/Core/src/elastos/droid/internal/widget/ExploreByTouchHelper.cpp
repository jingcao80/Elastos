
#include "elastos/droid/internal/widget/ExploreByTouchHelper.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::Accessibility::CAccessibilityEvent;
using Elastos::Droid::View::Accessibility::CAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::EIID_IAccessibilityDelegate;
using Elastos::Droid::View::IViewParent;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

ExploreByTouchHelper::ExploreByTouchNodeProvider::ExploreByTouchNodeProvider(
    /* [in] */ ExploreByTouchHelper* host)
    : mHost(host)
{}

ECode ExploreByTouchHelper::ExploreByTouchNodeProvider::CreateAccessibilityNodeInfo(
    /* [in] */ Int32 virtualViewId,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return mHost->CreateNode(virtualViewId, info);
}

ECode ExploreByTouchHelper::ExploreByTouchNodeProvider::PerformAction(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHost->PerformAction(virtualViewId, action, arguments);
    return NOERROR;
}


const String ExploreByTouchHelper::DEFAULT_CLASS_NAME("CView"); // = View.class.getName();
const String ExploreByTouchHelper::TAG("ExploreByTouchHelper");
CAR_INTERFACE_IMPL_2(ExploreByTouchHelper, Object, IAccessibilityDelegate, IExploreByTouchHelper);
ExploreByTouchHelper::ExploreByTouchHelper()
    : mFocusedVirtualViewId(INVALID_ID)
    , mHoveredVirtualViewId(INVALID_ID)
{
    CRect::New((IRect**)&mTempScreenRect);
    CRect::New((IRect**)&mTempParentRect);
    CRect::New((IRect**)&mTempVisibleRect);
    mTempGlobalRect = ArrayOf<Int32>::Alloc(2);
}

ECode ExploreByTouchHelper::constructor(
    /* [in] */ IView* forView)
{
    if (forView == NULL) {
        // throw new IllegalArgumentException("View may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mView = forView;
    forView->GetContext((IContext**)&mContext);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    mManager = IAccessibilityManager::Probe(obj);
    return NOERROR;
}

ECode ExploreByTouchHelper::GetAccessibilityNodeProvider(
    /* [in] */ IView* host,
    /* [out] */ IAccessibilityNodeProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    if (mNodeProvider == NULL) {
        mNodeProvider = new ExploreByTouchNodeProvider(this);
    }
    *provider = mNodeProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode ExploreByTouchHelper::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Boolean enabled1 = FALSE, enabled2 = FALSE;
    mManager->IsEnabled(&enabled1);
    if (!enabled1 || (mManager->IsTouchExplorationEnabled(&enabled2), !enabled2)) {
        return NOERROR;
    }

    Int32 action = 0;
    event->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_HOVER_MOVE:
        case IMotionEvent::ACTION_HOVER_ENTER: {
            Float x = 0, y = 0;
            event->GetX(&x);
            event->GetY(&y);
            const Int32 virtualViewId = GetVirtualViewAt(x, y);
            UpdateHoveredVirtualView(virtualViewId);
            *result = (virtualViewId != INVALID_ID);
            return NOERROR;
        }
        case IMotionEvent::ACTION_HOVER_EXIT:
            if (mFocusedVirtualViewId != INVALID_ID) {
                UpdateHoveredVirtualView(INVALID_ID);
                *result = TRUE;
                return NOERROR;
            }
            return NOERROR;
    }
    return NOERROR;
}

ECode ExploreByTouchHelper::SendEventForVirtualView(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 eventType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Boolean enabled = FALSE;
    if ((virtualViewId == INVALID_ID) || (mManager->IsEnabled(&enabled), !enabled)) {
        return NOERROR;
    }

    AutoPtr<IViewParent> parent;
    mView->GetParent((IViewParent**)&parent);
    if (parent == NULL) {
        return NOERROR;
    }

    AutoPtr<IAccessibilityEvent> event;
    FAIL_RETURN(CreateEvent(virtualViewId, eventType, (IAccessibilityEvent**)&event));
    return parent->RequestSendAccessibilityEvent(mView, event, result);
}

ECode ExploreByTouchHelper::InvalidateRoot()
{
    return InvalidateVirtualView(IView::NO_ID);
}

ECode ExploreByTouchHelper::InvalidateVirtualView(
    /* [in] */ Int32 virtualViewId)
{
    Boolean tmp = FALSE;
    return SendEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED, &tmp);
}

ECode ExploreByTouchHelper::GetFocusedVirtualView(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFocusedVirtualViewId;
    return NOERROR;
}

void ExploreByTouchHelper::UpdateHoveredVirtualView(
    /* [in] */ Int32 virtualViewId)
{
    if (mHoveredVirtualViewId == virtualViewId) {
        return;
    }

    const Int32 previousVirtualViewId = mHoveredVirtualViewId;
    mHoveredVirtualViewId = virtualViewId;

    // Stay consistent with framework behavior by sending ENTER/EXIT pairs
    // in reverse order. This is accurate as of API 18.
    Boolean tmp = FALSE;
    SendEventForVirtualView(virtualViewId, IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER, &tmp);
    SendEventForVirtualView(previousVirtualViewId, IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT, &tmp);
}

ECode ExploreByTouchHelper::CreateEvent(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 eventType,
    /* [out] */ IAccessibilityEvent** event)
{
    VALIDATE_NOT_NULL(event);
    AutoPtr<IAccessibilityEvent> e;
    switch (virtualViewId) {
        case IView::NO_ID:
            e = CreateEventForHost(eventType);
        default:
            return CreateEventForChild(virtualViewId, eventType, event);
    }
    *event = e;
    REFCOUNT_ADD(*event);
    return NOERROR;
}

AutoPtr<IAccessibilityEvent> ExploreByTouchHelper::CreateEventForHost(
    /* [in] */ Int32 eventType)
{
    AutoPtr<IAccessibilityEvent> event;
    CAccessibilityEvent::Obtain(eventType, (IAccessibilityEvent**)&event);
    OnInitializeAccessibilityEvent(mView, event);
    return event;
}

ECode ExploreByTouchHelper::CreateEventForChild(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 eventType,
    /* [out] */ IAccessibilityEvent** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<IAccessibilityEvent> event;
    CAccessibilityEvent::Obtain(eventType, (IAccessibilityEvent**)&event);
    IAccessibilityRecord::Probe(event)->SetEnabled(TRUE);

    AutoPtr<ICharSequence> seq;
    CString::New(DEFAULT_CLASS_NAME, (ICharSequence**)&seq);
    IAccessibilityRecord::Probe(event)->SetClassName(seq);

    // Allow the client to populate the event.
    OnPopulateEventForVirtualView(virtualViewId, event);

    // Make sure the developer is following the rules.
    AutoPtr<IList> list;
    IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
    AutoPtr<ICharSequence> desc;
    Boolean empty = FALSE;
    if ((list->IsEmpty(&empty), empty)
        && ((IAccessibilityRecord::Probe(event)->GetContentDescription((ICharSequence**)&desc), desc) == NULL)) {
        // throw new RuntimeException("Callbacks must add text or a content description in "
        //         + "populateEventForVirtualViewId()");
        return E_RUNTIME_EXCEPTION;
    }

    // Don't allow the client to override these properties.
    AutoPtr<IContext> context;
    mView->GetContext((IContext**)&context);
    String name;
    context->GetPackageName(&name);
    seq = NULL;
    CString::New(name, (ICharSequence**)&seq);
    event->SetPackageName(seq);
    IAccessibilityRecord::Probe(event)->SetSource(mView, virtualViewId);

    *result = event;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ExploreByTouchHelper::CreateNode(
    /* [in] */ Int32 virtualViewId,
    /* [out] */ IAccessibilityNodeInfo** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IAccessibilityNodeInfo> info;
    switch (virtualViewId) {
        case IView::NO_ID:
            info = CreateNodeForHost();
        default:
            CreateNodeForChild(virtualViewId, (IAccessibilityNodeInfo**)&info);
    }
    *result = info;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IAccessibilityNodeInfo> ExploreByTouchHelper::CreateNodeForHost()
{
    AutoPtr<IAccessibilityNodeInfo> node;
    CAccessibilityNodeInfo::Obtain(mView, (IAccessibilityNodeInfo**)&node);
    OnInitializeAccessibilityNodeInfo(mView, node);

    // Add the virtual descendants.
    AutoPtr<ILinkedList> virtualViewIds;
    CLinkedList::New((ILinkedList**)&virtualViewIds);
    GetVisibleVirtualViews(IList::Probe(virtualViewIds));

    Int32 size = 0;
    IList::Probe(virtualViewIds)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        IList::Probe(virtualViewIds)->Get(i, (IInterface**)&obj);
        AutoPtr<IInteger32> childVirtualViewId = IInteger32::Probe(obj);
        Int32 v = 0;
        childVirtualViewId->GetValue(&v);
        node->AddChild(mView, v);
    }

    return node;
}

ECode ExploreByTouchHelper::CreateNodeForChild(
    /* [in] */ Int32 virtualViewId,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    AutoPtr<IAccessibilityNodeInfo> node;
    CAccessibilityNodeInfo::Obtain((IAccessibilityNodeInfo**)&node);

    // Ensure the client has good defaults.
    node->SetEnabled(TRUE);
    AutoPtr<ICharSequence> seq;
    CString::New(DEFAULT_CLASS_NAME, (ICharSequence**)&seq);
    node->SetClassName(seq);

    // Allow the client to populate the node.
    OnPopulateNodeForVirtualView(virtualViewId, node);

    // Make sure the developer is following the rules.
    AutoPtr<ICharSequence> text;
    node->GetText((ICharSequence**)&text);
    AutoPtr<ICharSequence> desc;
    if ((text == NULL) && ((node->GetContentDescription((ICharSequence**)&desc), desc) == NULL)) {
        Slogger::D(TAG, "Callbacks must add text or a content description in populateNodeForVirtualViewId()");
        return E_RUNTIME_EXCEPTION;
    }

    node->GetBoundsInParent(mTempParentRect);
    Boolean empty = FALSE;
    if (mTempParentRect->IsEmpty(&empty), empty) {
        Slogger::D(TAG, "Callbacks must set parent bounds in populateNodeForVirtualViewId()");
        return NOERROR;
    }

    Int32 actions = 0;
    node->GetActions(&actions);
    if ((actions & IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS) != 0) {
        Slogger::D(TAG, "Callbacks must not add ACTION_ACCESSIBILITY_FOCUS in populateNodeForVirtualViewId()");
        return NOERROR;
    }
    if ((actions & IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS) != 0) {
        Slogger::D(TAG, "Callbacks must not add ACTION_CLEAR_ACCESSIBILITY_FOCUS in populateNodeForVirtualViewId()");
        return E_RUNTIME_EXCEPTION;
    }

    // Don't allow the client to override these properties.
    AutoPtr<IContext> context;
    mView->GetContext((IContext**)&context);
    String name;
    context->GetPackageName(&name);

    seq = NULL;
    CString::New(name, (ICharSequence**)&seq);
    node->SetPackageName(seq);
    node->SetSource(mView, virtualViewId);
    node->SetParent(mView);

    // Manage internal accessibility focus state.
    if (mFocusedVirtualViewId == virtualViewId) {
        node->SetAccessibilityFocused(TRUE);
        node->AddAction(IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS);
    } else {
        node->SetAccessibilityFocused(FALSE);
        node->AddAction(IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS);
    }

    // Set the visibility based on the parent bound.
    if (IntersectVisibleToUser(mTempParentRect)) {
        node->SetVisibleToUser(TRUE);
        node->SetBoundsInParent(mTempParentRect);
    }

    // Calculate screen-relative bound.
    mView->GetLocationOnScreen(mTempGlobalRect);
    const Int32 offsetX = (*mTempGlobalRect)[0];
    const Int32 offsetY = (*mTempGlobalRect)[1];
    mTempScreenRect->Set(mTempParentRect);
    mTempScreenRect->Offset(offsetX, offsetY);
    node->SetBoundsInScreen(mTempScreenRect);

    *info = node;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

Boolean ExploreByTouchHelper::PerformAction(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    switch (virtualViewId) {
        case IView::NO_ID:
            return PerformActionForHost(action, arguments);
        default:
            return PerformActionForChild(virtualViewId, action, arguments);
    }
}

Boolean ExploreByTouchHelper::PerformActionForHost(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    Boolean value = FALSE;
    PerformAccessibilityAction(mView, action, arguments, &value);
    return value;
}

Boolean ExploreByTouchHelper::PerformActionForChild(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS:
        case IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS:
            return ManageFocusForChild(virtualViewId, action, arguments);
        default:
            return OnPerformActionForVirtualView(virtualViewId, action, arguments);
    }
}

Boolean ExploreByTouchHelper::ManageFocusForChild(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS:
            return RequestAccessibilityFocus(virtualViewId);
        case IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS:
            return ClearAccessibilityFocus(virtualViewId);
        default:
            return FALSE;
    }
}

Boolean ExploreByTouchHelper::IntersectVisibleToUser(
    /* [in] */ IRect* localRect)
{
    // Missing or empty bounds mean this view is not visible.
    Boolean result = FALSE;
    if ((localRect == NULL) || (localRect->IsEmpty(&result), result)) {
        return FALSE;
    }

    // Attached to invisible window means this view is not visible.
    Int32 v = 0;
    if ((mView->GetWindowVisibility(&v), v) != IView::VISIBLE) {
        return FALSE;
    }

    // An invisible predecessor means that this view is not visible.
    AutoPtr<IViewParent> viewParent;
    mView->GetParent((IViewParent**)&viewParent);
    while (IView::Probe(viewParent)) {
        AutoPtr<IView> view = IView::Probe(viewParent);
        Float alpha = 0;
        if (((view->GetAlpha(&alpha), alpha) <= 0) || ((view->GetVisibility(&v), v) != IView::VISIBLE)) {
            return FALSE;
        }
        viewParent = NULL;
        view->GetParent((IViewParent**)&viewParent);
    }

    // A null parent implies the view is not visible.
    if (viewParent == NULL) {
        return FALSE;
    }

    // If no portion of the parent is visible, this view is not visible.
    if (mView->GetLocalVisibleRect(mTempVisibleRect, &result), !result) {
        return FALSE;
    }

    // Check if the view intersects the visible portion of the parent.
    localRect->Intersect(mTempVisibleRect, &result);
    return result;
}

Boolean ExploreByTouchHelper::IsAccessibilityFocused(
    /* [in] */ Int32 virtualViewId)
{
    return (mFocusedVirtualViewId == virtualViewId);
}

Boolean ExploreByTouchHelper::RequestAccessibilityFocus(
    /* [in] */ Int32 virtualViewId)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IAccessibilityManager> accessibilityManager = IAccessibilityManager::Probe(obj);

    Boolean enabled1 = FALSE, enabled2 = FALSE;
    mManager->IsEnabled(&enabled1);
    if (!enabled1 || (accessibilityManager->IsTouchExplorationEnabled(&enabled2), !enabled2)) {
        return FALSE;
    }
    // TODO: Check virtual view visibility.
    if (!IsAccessibilityFocused(virtualViewId)) {
        mFocusedVirtualViewId = virtualViewId;
        // TODO: Only invalidate virtual view bounds.
        mView->Invalidate();
        Boolean tmp = FALSE;
        SendEventForVirtualView(virtualViewId,
                IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED, &tmp);
        return TRUE;
    }
    return FALSE;
}

Boolean ExploreByTouchHelper::ClearAccessibilityFocus(
    /* [in] */ Int32 virtualViewId)
{
    if (IsAccessibilityFocused(virtualViewId)) {
        mFocusedVirtualViewId = INVALID_ID;
        mView->Invalidate();
        Boolean tmp = FALSE;
        SendEventForVirtualView(virtualViewId,
                IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED, &tmp);
        return TRUE;
    }
    return FALSE;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
