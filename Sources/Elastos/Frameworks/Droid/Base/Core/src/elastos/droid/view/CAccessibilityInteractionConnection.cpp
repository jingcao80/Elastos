//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/view/CAccessibilityInteractionConnection.h"
#include "elastos/droid/view/ViewRootImpl.h"

using Elastos::Droid::View::Accessibility::EIID_IIAccessibilityInteractionConnection;
namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CAccessibilityInteractionConnection, Object, IIAccessibilityInteractionConnection)

CAR_OBJECT_IMPL(CAccessibilityInteractionConnection)

ECode CAccessibilityInteractionConnection::constructor(
    /* [in] */ IWeakReference* viewRootImpl)
{
    mViewRootImpl = viewRootImpl;
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FindAccessibilityNodeInfoByAccessibilityId(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    AutoPtr<IViewRootImpl> viewRoot;
    if (mViewRootImpl != NULL) {
        mViewRootImpl->Resolve(EIID_IViewRootImpl, (IInterface**)&viewRoot);
    }

    if (viewRoot != NULL && ((ViewRootImpl*)viewRoot.Get())->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        viewRoot->GetAccessibilityInteractionController((IAccessibilityInteractionController**)&controller);
        controller->FindAccessibilityNodeInfoByAccessibilityIdClientThread(accessibilityNodeId,
                    interactiveRegion, interactionId, callback, flags, interrogatingPid,
                    interrogatingTid, spec);
    } else {
        // We cannot make the call and notify the caller so it does not wait.
        // try {
        return callback->SetFindAccessibilityNodeInfosResult(NULL, interactionId);
        // } catch (RemoteException re) {
            /* best effort - ignore */
        // }
    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FindAccessibilityNodeInfosByViewId(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ const String& viewId,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    AutoPtr<IViewRootImpl> viewRoot;
    if (mViewRootImpl != NULL) {
        mViewRootImpl->Resolve(EIID_IViewRootImpl, (IInterface**)&viewRoot);
    }

    if (viewRoot != NULL && ((ViewRootImpl*)viewRoot.Get())->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        viewRoot->GetAccessibilityInteractionController((IAccessibilityInteractionController**)&controller);
        controller->FindAccessibilityNodeInfosByViewIdClientThread(accessibilityNodeId,
                            viewId, interactiveRegion, interactionId, callback, flags,
                            interrogatingPid, interrogatingTid, spec);
    } else {
        // We cannot make the call and notify the caller so it does not wait.
        // try {
        return callback->SetFindAccessibilityNodeInfoResult(NULL, interactionId);
        // } catch (RemoteException re) {
            /* best effort - ignore */
        // }
    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FindAccessibilityNodeInfosByText(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ const String& text,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    AutoPtr<IViewRootImpl> viewRoot;
    if (mViewRootImpl != NULL) {
        mViewRootImpl->Resolve(EIID_IViewRootImpl, (IInterface**)&viewRoot);
    }

    if (viewRoot != NULL && ((ViewRootImpl*)viewRoot.Get())->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        viewRoot->GetAccessibilityInteractionController((IAccessibilityInteractionController**)&controller);
        controller->FindAccessibilityNodeInfosByTextClientThread(accessibilityNodeId, text,
                            interactiveRegion, interactionId, callback, flags, interrogatingPid,
                            interrogatingTid, spec);
    } else {
        // We cannot make the call and notify the caller so it does not wait.
        // try {
        return callback->SetFindAccessibilityNodeInfosResult(NULL, interactionId);
        // } catch (RemoteException re) {
            /* best effort - ignore */
        // }
    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FindFocus(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 focusType,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    AutoPtr<IViewRootImpl> viewRoot;
    if (mViewRootImpl != NULL) {
        mViewRootImpl->Resolve(EIID_IViewRootImpl, (IInterface**)&viewRoot);
    }

    if (viewRoot != NULL && ((ViewRootImpl*)viewRoot.Get())->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        viewRoot->GetAccessibilityInteractionController((IAccessibilityInteractionController**)&controller);
        controller->FindFocusClientThread(accessibilityNodeId, focusType, interactiveRegion,
            interactionId, callback, flags, interrogatingPid, interrogatingTid, spec);
    } else {
        // We cannot make the call and notify the caller so it does not wait.
        // try {
        return callback->SetFindAccessibilityNodeInfoResult(NULL, interactionId);
        // } catch (RemoteException re) {
            /* best effort - ignore */
        // }
    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::FocusSearch(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 direction,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    AutoPtr<IViewRootImpl> viewRoot;
    if (mViewRootImpl != NULL) {
        mViewRootImpl->Resolve(EIID_IViewRootImpl, (IInterface**)&viewRoot);
    }

    if (viewRoot != NULL && ((ViewRootImpl*)viewRoot.Get())->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        viewRoot->GetAccessibilityInteractionController((IAccessibilityInteractionController**)&controller);
        controller->FocusSearchClientThread(accessibilityNodeId, direction, interactiveRegion,
            interactionId, callback, flags, interrogatingPid, interrogatingTid, spec);
    } else {
        // We cannot make the call and notify the caller so it does not wait.
        // try {
        return callback->SetFindAccessibilityNodeInfoResult(NULL, interactionId);
        // } catch (RemoteException re) {
            /* best effort - ignore */
        // }
    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::PerformAccessibilityAction(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid)
{
    AutoPtr<IViewRootImpl> viewRoot;
    if (mViewRootImpl != NULL) {
        mViewRootImpl->Resolve(EIID_IViewRootImpl, (IInterface**)&viewRoot);
    }

    if (viewRoot != NULL && ((ViewRootImpl*)viewRoot.Get())->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        viewRoot->GetAccessibilityInteractionController((IAccessibilityInteractionController**)&controller);
        controller->PerformAccessibilityActionClientThread(accessibilityNodeId, action, arguments,
            interactionId, callback, flags, interrogatingPid, interrogatingTid);
    } else {
        // We cannot make the call and notify the caller so it does not wait.
        // try {
        return callback->SetPerformAccessibilityActionResult(FALSE, interactionId);
        // } catch (RemoteException re) {
            /* best effort - ignore */
        // }
    }
    return NOERROR;
}

ECode CAccessibilityInteractionConnection::ComputeClickPointInScreen(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    AutoPtr<IViewRootImpl> viewRoot;
    if (mViewRootImpl != NULL) {
        mViewRootImpl->Resolve(EIID_IViewRootImpl, (IInterface**)&viewRoot);
    }

    if (viewRoot != NULL && ((ViewRootImpl*)viewRoot.Get())->mView != NULL) {
        AutoPtr<IAccessibilityInteractionController> controller;
        viewRoot->GetAccessibilityInteractionController((IAccessibilityInteractionController**)&controller);
        controller->ComputeClickPointInScreenClientThread(accessibilityNodeId,
            interactiveRegion, interactionId, callback, interrogatingPid,
            interrogatingTid, spec);
    } else {
        // We cannot make the call and notify the caller so it does not wait.
        // try {
        return callback->SetComputeClickPointInScreenActionResult(NULL, interactionId);
        // } catch (RemoteException re) {
            /* best effort - ignore */
        // }
    }
    return NOERROR;
}

} // View
} // Droid
} // Elastos
