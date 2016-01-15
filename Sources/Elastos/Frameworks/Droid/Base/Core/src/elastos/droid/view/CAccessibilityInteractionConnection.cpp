#include "elastos/droid/view/CAccessibilityInteractionConnection.h"
#include "elastos/droid/view/ViewRootImpl.h"

namespace Elastos {
namespace Droid {
namespace View {

ECode CAccessibilityInteractionConnection::constructor(
    /* [in] */ Handle32 viewRootImpl)
{
    mViewRootImpl = (ViewRootImpl*)viewRootImpl;
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FindAccessibilityNodeInfoByAccessibilityId(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid)
{
    if (mViewRootImpl != NULL && mViewRootImpl->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        controller = mViewRootImpl->GetAccessibilityInteractionController();
        controller->FindAccessibilityNodeInfoByAccessibilityIdClientThread(
            accessibilityNodeId, interactionId, callback, flags,
            interrogatingPid, interrogatingTid);
    }
    else {
        // We cannot make the call and notify the caller so it does not wait.
        callback->SetFindAccessibilityNodeInfosResult(NULL, interactionId);
    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::PerformAccessibilityAction(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interogatingPid,
    /* [in] */ Int64 interrogatingTid)
{
    if (mViewRootImpl != NULL && mViewRootImpl->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        controller = mViewRootImpl->GetAccessibilityInteractionController();
        controller->PerformAccessibilityActionClientThread(
            accessibilityNodeId, action, arguments, interactionId,
            callback, flags, interogatingPid, interrogatingTid);
    }
    else {
        // We cannot make the call and notify the caller so it does not wait.
        callback->SetPerformAccessibilityActionResult(FALSE, interactionId);

    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FindAccessibilityNodeInfoByViewId(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid)
{
    if (mViewRootImpl != NULL && mViewRootImpl->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        controller = mViewRootImpl->GetAccessibilityInteractionController();
        controller->FindAccessibilityNodeInfoByViewIdClientThread(
            accessibilityNodeId, viewId, interactionId, callback,
            flags, interrogatingPid, interrogatingTid);
    }
    else {
        // We cannot make the call and notify the caller so it does not wait.
        callback->SetFindAccessibilityNodeInfoResult(NULL, interactionId);

    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FindAccessibilityNodeInfosByText(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ const String& text,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid)
{
    if (mViewRootImpl != NULL && mViewRootImpl->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        controller = mViewRootImpl->GetAccessibilityInteractionController();
        controller->FindAccessibilityNodeInfosByTextClientThread(accessibilityNodeId, text,
            interactionId, callback, flags, interrogatingPid, interrogatingTid);
    }
    else {
        // We cannot make the call and notify the caller so it does not wait.
        callback->SetFindAccessibilityNodeInfosResult(NULL, interactionId);

    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FindFocus(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 focusType,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid)
{
    if (mViewRootImpl != NULL && mViewRootImpl->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        controller = mViewRootImpl->GetAccessibilityInteractionController();
        controller->FindFocusClientThread(
            accessibilityNodeId, focusType, interactionId,
            callback, flags, interrogatingPid, interrogatingTid);
    }
    else {
        // We cannot make the call and notify the caller so it does not wait.
        callback->SetFindAccessibilityNodeInfoResult(NULL, interactionId);

    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FocusSearch(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid)
{
    if (mViewRootImpl != NULL && mViewRootImpl->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        controller = mViewRootImpl->GetAccessibilityInteractionController();
        controller->FocusSearchClientThread(
            accessibilityNodeId, direction, interactionId,
            callback, flags, interrogatingPid, interrogatingTid);
    }
    else {
        // We cannot make the call and notify the caller so it does not wait.
        callback->SetFindAccessibilityNodeInfoResult(NULL, interactionId);

    }
    return NOERROR;
}

} // View
} // Droid
} // Elastos
