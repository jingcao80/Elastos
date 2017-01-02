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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#define HASH_FOR_VIEW
#include "elastos/droid/ext/frameworkhash.h"
#include "elastos/droid/view/AccessibilityInteractionController.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/view/accessibility/CAccessibilityInteractionClient.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/utility/CInt64SparseArray.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Utility::EIID_IPredicate;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Utility::CInt64SparseArray;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::View::Accessibility::CAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionClient;
using Elastos::Droid::View::Accessibility::CAccessibilityInteractionClient;
using Elastos::Core::CoreUtils;
using Elastos::Core::IThread;
using Elastos::Core::IInteger64;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IQueue;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {

const Boolean AccessibilityInteractionController::ENFORCE_NODE_TREE_CONSISTENT = FALSE;

const Int32 AccessibilityInteractionController::AccessibilityNodePrefetcher::MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE = 50;

const Int32 AccessibilityInteractionController::PrivateHandler::MSG_PERFORM_ACCESSIBILITY_ACTION = 1;
const Int32 AccessibilityInteractionController::PrivateHandler::MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_ACCESSIBILITY_ID = 2;
const Int32 AccessibilityInteractionController::PrivateHandler::MSG_FIND_ACCESSIBILITY_NODE_INFOS_BY_VIEW_ID = 3;
const Int32 AccessibilityInteractionController::PrivateHandler::MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_TEXT = 4;
const Int32 AccessibilityInteractionController::PrivateHandler::MSG_FIND_FOCUS = 5;
const Int32 AccessibilityInteractionController::PrivateHandler::MSG_FOCUS_SEARCH = 6;
const Int32 AccessibilityInteractionController::PrivateHandler::MSG_COMPUTE_CLICK_POINT_IN_SCREEN = 7;

//========================================================================================
// AccessibilityInteractionController::AccessibilityNodePrefetcher
//========================================================================================

AccessibilityInteractionController::AccessibilityNodePrefetcher::AccessibilityNodePrefetcher(
    /* [in] */ AccessibilityInteractionController* host)
    : mHost(host)
{
    CArrayList::New((IArrayList**)&mTempViewList);
}

void AccessibilityInteractionController::AccessibilityNodePrefetcher::PrefetchAccessibilityNodeInfos(
    /* [in] */ IView* view,
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 fetchFlags,
    /* [in] */ IList* outInfos)
{
    AutoPtr<IAccessibilityNodeProvider> provider;
    view->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
    if (provider == NULL) {
        AutoPtr<IAccessibilityNodeInfo> root;
        view->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&root);
        if (root != NULL) {
            outInfos->Add(root);
            if ((fetchFlags & IAccessibilityNodeInfo::FLAG_PREFETCH_PREDECESSORS) != 0) {
                PrefetchPredecessorsOfRealNode(view, outInfos);
            }
            if ((fetchFlags & IAccessibilityNodeInfo::FLAG_PREFETCH_SIBLINGS) != 0) {
                PrefetchSiblingsOfRealNode(view, outInfos);
            }
            if ((fetchFlags & IAccessibilityNodeInfo::FLAG_PREFETCH_DESCENDANTS) != 0) {
                PrefetchDescendantsOfRealNode(view, outInfos);
            }
        }
    }
    else {
        AutoPtr<IAccessibilityNodeInfo> root;
        if (virtualViewId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
            provider->CreateAccessibilityNodeInfo(virtualViewId, (IAccessibilityNodeInfo**)&root);
        }
        else {
            provider->CreateAccessibilityNodeInfo(
                    IAccessibilityNodeProvider::HOST_VIEW_ID, (IAccessibilityNodeInfo**)&root);
        }
        if (root != NULL) {
            outInfos->Add(root);
            if ((fetchFlags & IAccessibilityNodeInfo::FLAG_PREFETCH_PREDECESSORS) != 0) {
                PrefetchPredecessorsOfVirtualNode(root, view, provider, outInfos);
            }
            if ((fetchFlags & IAccessibilityNodeInfo::FLAG_PREFETCH_SIBLINGS) != 0) {
                PrefetchSiblingsOfVirtualNode(root, view, provider, outInfos);
            }
            if ((fetchFlags & IAccessibilityNodeInfo::FLAG_PREFETCH_DESCENDANTS) != 0) {
                PrefetchDescendantsOfVirtualNode(root, provider, outInfos);
            }
        }
    }
    if (ENFORCE_NODE_TREE_CONSISTENT) {
        EnforceNodeTreeConsistent(outInfos);
    }
}

ECode AccessibilityInteractionController::AccessibilityNodePrefetcher::EnforceNodeTreeConsistent(
    /* [in] */ IList* nodes)
{
    AutoPtr<IInt64SparseArray> nodeMap;
    CInt64SparseArray::New((IInt64SparseArray**)&nodeMap);
    Int32 nodeCount;
    nodes->GetSize(&nodeCount);
    for (Int32 i = 0; i < nodeCount; i++) {
        AutoPtr<IInterface> obj;
        nodes->Get(i, (IInterface**)&obj);
        AutoPtr<IAccessibilityNodeInfo> node = IAccessibilityNodeInfo::Probe(obj);
        Int64 id;
        node->GetSourceNodeId(&id);
        nodeMap->Put(id, node);
    }

    // If the nodes are a tree it does not matter from
    // which node we start to search for the root.
    AutoPtr<IInterface> object;
    nodeMap->ValueAt(0, (IInterface**)&object);
    AutoPtr<IAccessibilityNodeInfo> root = IAccessibilityNodeInfo::Probe(object);
    AutoPtr<IAccessibilityNodeInfo> parent = root;
    while (parent != NULL) {
        root = parent;
        Int64 parentId;
        parent->GetParentNodeId(&parentId);
        AutoPtr<IInterface> obj;
        nodeMap->Get(parentId, (IInterface**)&obj);
        parent = IAccessibilityNodeInfo::Probe(obj);
    }

    // Traverse the tree and do some checks.
    AutoPtr<IAccessibilityNodeInfo> accessFocus;
    AutoPtr<IAccessibilityNodeInfo> inputFocus;
    AutoPtr<IHashSet> seen;
    CHashSet::New((IHashSet**)&seen);
    AutoPtr<IQueue> fringe;
    CLinkedList::New((IQueue**)&fringe);
    fringe->Add(root);

    Boolean res;
    while (fringe->IsEmpty(&res), !res) {
        AutoPtr<IInterface> obj;
        fringe->Poll((IInterface**)&obj);
        AutoPtr<IAccessibilityNodeInfo> current = IAccessibilityNodeInfo::Probe(obj);

        // Check for duplicates
        Boolean result;
        if (seen->Add(current, &result), !result) {
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Duplicate node: "
            //         + current + " in window:"
            //         + mViewRootImpl.mAttachInfo.mAccessibilityWindowId);
        }

        // Check for one accessibility focus.
        if (current->IsAccessibilityFocused(&result), result) {
            if (accessFocus != NULL) {
                return E_ILLEGAL_STATE_EXCEPTION;
                // throw new IllegalStateException("Duplicate accessibility focus:"
                //         + current
                //         + " in window:" + mViewRootImpl.mAttachInfo.mAccessibilityWindowId);
            }
            else {
                accessFocus = current;
            }
        }

        // Check for one input focus.
        if (current->IsFocused(&result), result) {
            if (inputFocus != NULL) {
                return E_ILLEGAL_STATE_EXCEPTION;
                // throw new IllegalStateException("Duplicate input focus: "
                //     + current + " in window:"
                //     + mViewRootImpl.mAttachInfo.mAccessibilityWindowId);
            }
            else {
                inputFocus = current;
            }
        }

        Int32 childCount;
        current->GetChildCount(&childCount);
        for (Int32 j = 0; j < childCount; j++) {
            Int64 childId;
            current->GetChildId(j, &childId);
            AutoPtr<IInterface> obj;
            nodeMap->Get(childId, (IInterface**)&obj);
            AutoPtr<IAccessibilityNodeInfo> child = IAccessibilityNodeInfo::Probe(obj);
            if (child != NULL) {
                fringe->Add(child);
            }
        }
    }

    // Check for disconnected nodes.
    Int32 size;
    nodeMap->GetSize(&size);
    for (Int32 j = size - 1; j >= 0; j--) {
        AutoPtr<IInterface> obj;
        nodeMap->ValueAt(j, (IInterface**)&obj);
        AutoPtr<IAccessibilityNodeInfo> info = IAccessibilityNodeInfo::Probe(obj);
        if (seen->Contains(info, &res), !res) {
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Disconnected node: " + info);
        }
    }

    return NOERROR;
}

void AccessibilityInteractionController::AccessibilityNodePrefetcher::PrefetchPredecessorsOfRealNode(
    /* [in] */ IView* view,
    /* [in] */ IList* outInfos)
{
    AutoPtr<IViewParent> parent;
    view->GetParentForAccessibility((IViewParent**)&parent);
    Int32 size;
    while (IView::Probe(parent) != NULL
        && (outInfos->GetSize(&size), size < MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE)) {
        AutoPtr<IView> parentView = IView::Probe(parent);
        AutoPtr<IAccessibilityNodeInfo> info;
        parentView->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&info);
        if (info != NULL) {
            outInfos->Add(info);
        }
        AutoPtr<IViewParent> temp;
        parentView->GetParentForAccessibility((IViewParent**)&temp);
        parent = temp;
    }
}

void AccessibilityInteractionController::AccessibilityNodePrefetcher::PrefetchSiblingsOfRealNode(
    /* [in] */ IView* current,
    /* [in] */ IList* outInfos)
{
    AutoPtr<IViewParent> parent;
    current->GetParentForAccessibility((IViewParent**)&parent);
    if (IViewGroup::Probe(parent) != NULL) {
        AutoPtr<IViewGroup> parentGroup = IViewGroup::Probe(parent);
        AutoPtr<IArrayList> children = mTempViewList;
        children->Clear();
        // try {
        IView::Probe(parentGroup)->AddChildrenForAccessibility(children);
        Int32 childCount;
        children->GetSize(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            Int32 size;
            outInfos->GetSize(&size);
            if (size >= MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE) {
                return;
            }
            AutoPtr<IInterface> obj;
            children->Get(i, (IInterface**)&obj);
            AutoPtr<IView> child = IView::Probe(obj);

            Int32 childViewId, currentViewId;
            current->GetAccessibilityViewId(&currentViewId);
            child->GetAccessibilityViewId(&childViewId);

            if (childViewId != currentViewId && mHost->IsShown(child)) {
                AutoPtr<IAccessibilityNodeInfo> info;
                AutoPtr<IAccessibilityNodeProvider> provider;
                child->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
                if (provider == NULL) {
                    child->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&info);
                }
                else {
                    provider->CreateAccessibilityNodeInfo(
                        IAccessibilityNodeProvider::HOST_VIEW_ID, (IAccessibilityNodeInfo**)&info);
                }
                if (info != NULL) {
                    outInfos->Add(info);
                }
            }
        }
        // } finally {
        children->Clear();
        // }
    }
}

void AccessibilityInteractionController::AccessibilityNodePrefetcher::PrefetchDescendantsOfRealNode(
    /* [in] */ IView* root,
    /* [in] */ IList* outInfos)
{
    if (IViewGroup::Probe(root) == NULL) {
        return;
    }

    AutoPtr<HashMap<AutoPtr<IView>, AutoPtr<IAccessibilityNodeInfo> > > addedChildren =
            new HashMap<AutoPtr<IView>, AutoPtr<IAccessibilityNodeInfo> >();

    AutoPtr<IArrayList> children = mTempViewList;
    children->Clear();
    // try {
    root->AddChildrenForAccessibility(children);

    Int32 childCount;
    children->GetSize(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        Int32 size;
        outInfos->GetSize(&size);
        if (size >= MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE) {
            return;
        }
        AutoPtr<IInterface> obj;
        children->Get(i, (IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);
        if (mHost->IsShown(child)) {
            AutoPtr<IAccessibilityNodeProvider> provider;
            child->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
            if (provider == NULL) {
                AutoPtr<IAccessibilityNodeInfo> info;
                child->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&info);
                if (info != NULL) {
                    outInfos->Add(info);
                    (*addedChildren)[child] = NULL;
                }
            }
            else {
                AutoPtr<IAccessibilityNodeInfo> info;
                provider->CreateAccessibilityNodeInfo(
                        IAccessibilityNodeProvider::HOST_VIEW_ID, (IAccessibilityNodeInfo**)&info);
                if (info != NULL) {
                    outInfos->Add(info);
                    (*addedChildren)[child] = info;
                }
            }
        }
    }
    // } finally {
    children->Clear();
    // }
    Int32 size;
    outInfos->GetSize(&size);
    if (size < MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE) {
        HashMap<AutoPtr<IView>, AutoPtr<IAccessibilityNodeInfo> >::Iterator it;
        for (it = addedChildren->Begin(); it != addedChildren->End(); ++it) {
            AutoPtr<IView> addedChild = it->mFirst;
            AutoPtr<IAccessibilityNodeInfo> virtualRoot = it->mSecond;
            if (virtualRoot == NULL) {
                PrefetchDescendantsOfRealNode(addedChild, outInfos);
            }
            else {
                AutoPtr<IAccessibilityNodeProvider> provider;
                addedChild->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
                PrefetchDescendantsOfVirtualNode(virtualRoot, provider, outInfos);
            }
        }
    }
}

void AccessibilityInteractionController::AccessibilityNodePrefetcher::PrefetchPredecessorsOfVirtualNode(
    /* [in] */ IAccessibilityNodeInfo* root,
    /* [in] */ IView* providerHost,
    /* [in] */ IAccessibilityNodeProvider* provider,
    /* [in] */ IList* outInfos)
{
    Int64 parentNodeId;
    root->GetParentNodeId(&parentNodeId);
    Int32 accessibilityViewId = CAccessibilityNodeInfo::GetAccessibilityViewId(parentNodeId);

    while (accessibilityViewId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
        Int32 size;
        outInfos->GetSize(&size);
        if (size >= MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE) {
            return;
        }

        Int32 virtualDescendantId = CAccessibilityNodeInfo::GetVirtualDescendantId(parentNodeId);
        Int32 hostAccessViewId;
        providerHost->GetAccessibilityViewId(&hostAccessViewId);
        if (virtualDescendantId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID
            || accessibilityViewId == hostAccessViewId) {
            AutoPtr<IAccessibilityNodeInfo> parent;
            if (virtualDescendantId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
                provider->CreateAccessibilityNodeInfo(
                    virtualDescendantId, (IAccessibilityNodeInfo**)&parent);
            }
            else {
                provider->CreateAccessibilityNodeInfo(
                    IAccessibilityNodeProvider::HOST_VIEW_ID, (IAccessibilityNodeInfo**)&parent);
            }
            if (parent != NULL) {
                outInfos->Add(parent);
            }
            parent->GetParentNodeId(&parentNodeId);
            accessibilityViewId = CAccessibilityNodeInfo::GetAccessibilityViewId(parentNodeId);
        }
        else {
            PrefetchPredecessorsOfRealNode(providerHost, outInfos);
            return;
        }
    }
}

void AccessibilityInteractionController::AccessibilityNodePrefetcher::PrefetchSiblingsOfVirtualNode(
    /* [in] */ IAccessibilityNodeInfo* current,
    /* [in] */ IView* providerHost,
    /* [in] */ IAccessibilityNodeProvider* provider,
    /* [in] */ IList* outInfos)
{
    Int64 parentNodeId;
    current->GetParentNodeId(&parentNodeId);

    Int32 parentAccessibilityViewId = CAccessibilityNodeInfo::GetAccessibilityViewId(parentNodeId);
    Int32 parentVirtualDescendantId = CAccessibilityNodeInfo::GetVirtualDescendantId(parentNodeId);
    Int32 hostAccessViewId;
    providerHost->GetAccessibilityViewId(&hostAccessViewId);

    if (parentVirtualDescendantId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID
        || parentAccessibilityViewId == hostAccessViewId) {
        AutoPtr<IAccessibilityNodeInfo> parent;
        if(parentVirtualDescendantId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
            provider->CreateAccessibilityNodeInfo(
                    parentVirtualDescendantId, (IAccessibilityNodeInfo**)&parent);
        }
        else {
            provider->CreateAccessibilityNodeInfo(
                    IAccessibilityNodeProvider::HOST_VIEW_ID, (IAccessibilityNodeInfo**)&parent);
        }
        if (parent != NULL) {
            Int32 childCount;
            parent->GetChildCount(&childCount);


            for (Int32 i = 0; i < childCount; i++) {
                Int32 size;
                outInfos->GetSize(&size);
                if (size >= MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE) {
                    return;
                }

                Int64 childNodeId;
                parent->GetChildId(i, &childNodeId);
                Int64 sourceNodeId;
                current->GetSourceNodeId(&sourceNodeId);
                if (childNodeId != sourceNodeId) {
                    Int32 childVirtualDescendantId = CAccessibilityNodeInfo::GetVirtualDescendantId(childNodeId);
                    AutoPtr<IAccessibilityNodeInfo> child;
                    provider->CreateAccessibilityNodeInfo(
                            childVirtualDescendantId, (IAccessibilityNodeInfo**)&child);
                    if (child != NULL) {
                        outInfos->Add(child);
                    }
                }
            }
        }
    }
    else {
        PrefetchSiblingsOfRealNode(providerHost, outInfos);
    }
}

void AccessibilityInteractionController::AccessibilityNodePrefetcher::PrefetchDescendantsOfVirtualNode(
    /* [in] */ IAccessibilityNodeInfo* root,
    /* [in] */ IAccessibilityNodeProvider* provider,
    /* [in] */ IList* outInfos)
{
    Int32 initialOutInfosSize;
    outInfos->GetSize(&initialOutInfosSize);
    Int32 childCount;
    root->GetChildCount(&childCount);

    for (Int32 i = 0; i < childCount; i++) {
        if (initialOutInfosSize >= MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE) {
            return;
        }

        Int64 childNodeId;
        root->GetChildId(i, &childNodeId);

        Int32 virtualDescendantId = CAccessibilityNodeInfo::GetVirtualDescendantId(childNodeId);
        AutoPtr<IAccessibilityNodeInfo> child;
        provider->CreateAccessibilityNodeInfo(
                virtualDescendantId, (IAccessibilityNodeInfo**)&child);
        if (child != NULL) {
            outInfos->Add(child);
        }
    }

    Int32 size;
    outInfos->GetSize(&size);
    if (size < MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE) {
        Int32 addedChildCount = size - initialOutInfosSize;
        for (Int32 i = 0; i < addedChildCount; i++) {
            AutoPtr<IInterface> obj;
            outInfos->Get(initialOutInfosSize + i, (IInterface**)&obj);
            AutoPtr<IAccessibilityNodeInfo> child = IAccessibilityNodeInfo::Probe(obj);
            PrefetchDescendantsOfVirtualNode(child, provider, outInfos);
        }
    }
}

//========================================================================================
// AccessibilityInteractionController::PrivateHandler
//========================================================================================
AccessibilityInteractionController::PrivateHandler::PrivateHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ AccessibilityInteractionController* host)
    : Handler(looper)
    , mHost(host)
{}

String AccessibilityInteractionController::PrivateHandler::GetMessageNameImpl(
    /* [in] */ IMessage* message)
{
    Int32 type;
    message->GetWhat(&type);
    switch (type) {
        case MSG_PERFORM_ACCESSIBILITY_ACTION:
            return String("MSG_PERFORM_ACCESSIBILITY_ACTION");
        case MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_ACCESSIBILITY_ID:
            return String("MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_ACCESSIBILITY_ID");
        case MSG_FIND_ACCESSIBILITY_NODE_INFOS_BY_VIEW_ID:
            return String("MSG_FIND_ACCESSIBILITY_NODE_INFOS_BY_VIEW_ID");
        case MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_TEXT:
            return String("MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_TEXT");
        case MSG_FIND_FOCUS:
            return String("MSG_FIND_FOCUS");
        case MSG_FOCUS_SEARCH:
            return String("MSG_FOCUS_SEARCH");
        case MSG_COMPUTE_CLICK_POINT_IN_SCREEN:
            return String("MSG_COMPUTE_CLICK_POINT_IN_SCREEN");
        default:
            // throw new IllegalArgumentException("Unknown message type: " + type);
            break;
    }
    return String("unknown message");
}

ECode AccessibilityInteractionController::PrivateHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    Int32 type;
    message->GetWhat(&type);
    switch (type) {
        case MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_ACCESSIBILITY_ID: {
            mHost->FindAccessibilityNodeInfoByAccessibilityIdUiThread(message);
        } break;
        case MSG_PERFORM_ACCESSIBILITY_ACTION: {
            mHost->PerfromAccessibilityActionUiThread(message);
        } break;
        case MSG_FIND_ACCESSIBILITY_NODE_INFOS_BY_VIEW_ID: {
            mHost->FindAccessibilityNodeInfosByViewIdUiThread(message);
        } break;
        case MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_TEXT: {
            mHost->FindAccessibilityNodeInfosByTextUiThread(message);
        } break;
        case MSG_FIND_FOCUS: {
            mHost->FindFocusUiThread(message);
        } break;
        case MSG_FOCUS_SEARCH: {
            mHost->FocusSearchUiThread(message);
        } break;
        case MSG_COMPUTE_CLICK_POINT_IN_SCREEN: {
            mHost->ComputeClickPointInScreenUiThread(message);
        } break;
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Unknown message type: " + type);
    }
    return NOERROR;
}

//========================================================================================
// AccessibilityInteractionController::AddNodeInfosForViewId
//========================================================================================
CAR_INTERFACE_IMPL(AccessibilityInteractionController::AddNodeInfosForViewId, Object, IPredicate);

AccessibilityInteractionController::AddNodeInfosForViewId::AddNodeInfosForViewId(
    /* [in] */ AccessibilityInteractionController* host)
    : mHost(host)
    , mViewId(IView::NO_ID)
{}

AccessibilityInteractionController::AddNodeInfosForViewId::~AddNodeInfosForViewId()
{}

void AccessibilityInteractionController::AddNodeInfosForViewId::Init(
    /* [in] */ Int32 viewId,
    /* [in] */ IList* infos)
{
    mViewId = viewId;
    mInfos = infos;
}

void AccessibilityInteractionController::AddNodeInfosForViewId::Reset()
{
    mViewId = IView::NO_ID;
    mInfos = NULL;
}

ECode AccessibilityInteractionController::AddNodeInfosForViewId::Apply(
    /* [in] */ IInterface* t,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IView> view = IView::Probe(t);
    Int32 id;
    view->GetId(&id);
    if (id == mViewId && mHost->IsShown(view)) {
        AutoPtr<IAccessibilityNodeInfo> info;
        view->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&info);
        mInfos->Add(info);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

//========================================================================================
// AccessibilityInteractionController
//========================================================================================

CAR_INTERFACE_IMPL(AccessibilityInteractionController, Object, IAccessibilityInteractionController)

AccessibilityInteractionController::AccessibilityInteractionController()
    : mViewRootImpl(NULL)
    , mMyLooperThreadId(0)
    , mMyProcessId(0)
{
    CArrayList::New((IArrayList**)&mTempAccessibilityNodeInfoList);

    CArrayList::New((IArrayList**)&mTempArrayList);

    CPoint::New((IPoint**)&mTempPoint);
    CRect::New((IRect**)&mTempRect);
    CRect::New((IRect**)&mTempRect1);
    CRect::New((IRect**)&mTempRect2);
}

AccessibilityInteractionController::~AccessibilityInteractionController()
{}

ECode AccessibilityInteractionController::constructor(
    /* [in] */ IViewRootImpl* viewRootImpl)
{
    mViewRootImpl = (ViewRootImpl*)viewRootImpl;
    AutoPtr<ILooper> looper;
    mViewRootImpl->mHandler->GetLooper((ILooper**)&looper);
    AutoPtr<IThread> thread;
    looper->GetThread((IThread**)&thread);
    thread->GetId(&mMyLooperThreadId);
    mMyProcessId = Process::MyPid();
    mHandler = new PrivateHandler(looper, this);
    mHandler->constructor();
    mPrefetcher = new AccessibilityNodePrefetcher(this);
    return NOERROR;
}

Boolean AccessibilityInteractionController::IsShown(
    /* [in] */ IView* view)
{
    // The first two checks are made also made by IsShown() which
    // however traverses the tree up to the parent to catch that.
    // Therefore, we do some fail fast check to minimize the up
    // tree traversal.
    if (view == NULL) {
        return FALSE;
    }

    View* viewObj = (View*)view;
    assert(viewObj != NULL);
    Boolean res;

    return (viewObj->mAttachInfo != NULL
            && viewObj->mAttachInfo->mWindowVisibility == IView::VISIBLE
            && (viewObj->IsShown(&res), res));
}

ECode AccessibilityInteractionController::FindAccessibilityNodeInfoByAccessibilityIdClientThread(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage((IMessage**)&message);
    message->SetWhat(PrivateHandler::MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_ACCESSIBILITY_ID);
    message->SetArg1(flags);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();

    args->mArgi1 = CAccessibilityNodeInfo::GetAccessibilityViewId(accessibilityNodeId);
    args->mArgi2 = CAccessibilityNodeInfo::GetVirtualDescendantId(accessibilityNodeId);
    args->mArgi3 = interactionId;
    args->mArg1 = callback;
    args->mArg2 = spec;
    args->mArg3 = interactiveRegion;
    message->SetObj((ISomeArgs*)args.Get());

    // If the interrogation is performed by the same thread as the main UI
    // thread in this process, set the message as a static reference so
    // after this call completes the same thread but in the interrogating
    // client can handle the message to generate the result.
    if (interrogatingPid == mMyProcessId && interrogatingTid == mMyLooperThreadId) {
        AutoPtr<IAccessibilityInteractionClient> aic = CAccessibilityInteractionClient::GetInstanceForThread(interrogatingTid);
        aic->SetSameThreadMessage(message);
    }
    else {
        Boolean result;
        mHandler->SendMessage(message, &result);
    }
    return NOERROR;
}

void AccessibilityInteractionController::FindAccessibilityNodeInfoByAccessibilityIdUiThread(
    /* [in] */ IMessage* message)
{
    Int32 flags;
    message->GetArg1(&flags);

    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    SomeArgs* args = (SomeArgs*)ISomeArgs::Probe(obj);
    const Int32 accessibilityViewId = args->mArgi1;
    const Int32 virtualDescendantId = args->mArgi2;
    const Int32 interactionId = args->mArgi3;
    AutoPtr<IIAccessibilityInteractionConnectionCallback> callback =
            IIAccessibilityInteractionConnectionCallback::Probe(args->mArg1);
    AutoPtr<IMagnificationSpec> spec = IMagnificationSpec::Probe(args->mArg2);
    AutoPtr<IRegion> interactiveRegion = IRegion::Probe(args->mArg3);

    args->Recycle();

    AutoPtr<IList> infos = IList::Probe(mTempAccessibilityNodeInfoList);
    infos->Clear();
    // try {
    if (mViewRootImpl->mView == NULL || mViewRootImpl->mAttachInfo == NULL) {
        return;
    }
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags =flags;
    AutoPtr<IView> root;
    if (accessibilityViewId == IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
        root = mViewRootImpl->mView;
    }
    else {
        root = FindViewByAccessibilityId(accessibilityViewId);
    }
    if (root != NULL && IsShown(root)) {
        mPrefetcher->PrefetchAccessibilityNodeInfos(root, virtualDescendantId, flags, infos);
    }
    // } finally {
    //     try {
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = 0;
    ApplyAppScaleAndMagnificationSpecIfNeeded(infos, spec);
    if (spec != NULL) {
        spec->Recycle();
    }
    AdjustIsVisibleToUserIfNeeded(infos, interactiveRegion);
    callback->SetFindAccessibilityNodeInfosResult(infos, interactionId);
    infos->Clear();
    //  } catch (RemoteException re) {
    //              //ignore - the other side will time out
    //     }
    // }
}

ECode AccessibilityInteractionController::FindAccessibilityNodeInfosByViewIdClientThread(
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
    Int32 avId = CAccessibilityNodeInfo::GetAccessibilityViewId(accessibilityNodeId);

    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(
            PrivateHandler::MSG_FIND_ACCESSIBILITY_NODE_INFOS_BY_VIEW_ID,
            flags, avId, (IMessage**)&message);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArgi1 = interactionId;
    args->mArg1 = callback;
    args->mArg2 = spec;
    args->mArg3 = CoreUtils::Convert(viewId);
    args->mArg4 = interactiveRegion;

    message->SetObj((ISomeArgs*)args.Get());

    // If the interrogation is performed by the same thread as the main UI
    // thread in this process, set the message as a static reference so
    // after this call completes the same thread but in the interrogating
    // client can handle the message to generate the result.
    if (interrogatingPid == mMyProcessId && interrogatingTid == mMyLooperThreadId) {
        AutoPtr<IAccessibilityInteractionClient> aic = CAccessibilityInteractionClient::GetInstanceForThread(interrogatingTid);
        aic->SetSameThreadMessage(message);
    }
    else {
        Boolean result;
        mHandler->SendMessage(message, &result);
    }
    return NOERROR;
}

void AccessibilityInteractionController::FindAccessibilityNodeInfosByViewIdUiThread(
    /* [in] */ IMessage* message)
{
    Int32 flags;
    message->GetArg1(&flags);
    Int32 accessibilityViewId;
    message->GetArg2(&accessibilityViewId);

    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    SomeArgs* args = (SomeArgs*)ISomeArgs::Probe(obj);
    const Int32 interactionId = args->mArgi1;
    AutoPtr<IIAccessibilityInteractionConnectionCallback> callback =
            IIAccessibilityInteractionConnectionCallback::Probe(args->mArg1);
    AutoPtr<IMagnificationSpec> spec = IMagnificationSpec::Probe(args->mArg2);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(args->mArg3);
    String viewId;
    cs->ToString(&viewId);
    AutoPtr<IRegion> interactiveRegion = IRegion::Probe(args->mArg4);

    args->Recycle();

    AutoPtr<IList> infos = IList::Probe(mTempAccessibilityNodeInfoList);
    infos->Clear();
    // try {
    if (mViewRootImpl->mView == NULL || mViewRootImpl->mAttachInfo == NULL) {
        return;
    }
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = flags;
    AutoPtr<IView> root;
    if (accessibilityViewId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
        root = FindViewByAccessibilityId(accessibilityViewId);
    }
    else {
        root = mViewRootImpl->mView;
    }
    if (root != NULL) {
        AutoPtr<IContext> ctx;
        root->GetContext((IContext**)&ctx);
        AutoPtr<IResources> res;
        ctx->GetResources((IResources**)&res);
        Int32 resolvedViewId;
        res->GetIdentifier(viewId, String(NULL), String(NULL), &resolvedViewId);
        if (resolvedViewId <= 0) {
            return;
        }
        if (mAddNodeInfosForViewId == NULL) {
            mAddNodeInfosForViewId = new AddNodeInfosForViewId(this);
        }
        mAddNodeInfosForViewId->Init(resolvedViewId, infos);
        AutoPtr<IView> _view;
        root->FindViewByPredicate(mAddNodeInfosForViewId, (IView**)&_view);
        mAddNodeInfosForViewId->Reset();
    }
    // } finally {
    //     try {
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = 0;
    ApplyAppScaleAndMagnificationSpecIfNeeded(infos, spec);
    if (spec != NULL) {
        spec->Recycle();
    }
    AdjustIsVisibleToUserIfNeeded(infos, interactiveRegion);
    callback->SetFindAccessibilityNodeInfosResult(infos, interactionId);
    //     } catch (RemoteException re) {
    //              //ignore - the other side will time out
    //     }
    // }
}

ECode AccessibilityInteractionController::FindAccessibilityNodeInfosByTextClientThread(
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
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(
            PrivateHandler::MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_TEXT,
            flags, 0, (IMessage**)&message);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArg1 = CoreUtils::Convert(text);
    args->mArg2 = callback;
    args->mArg3 = spec;
    args->mArgi1 = CAccessibilityNodeInfo::GetAccessibilityViewId(accessibilityNodeId);
    args->mArgi2 = CAccessibilityNodeInfo::GetVirtualDescendantId(accessibilityNodeId);
    args->mArgi3 = interactionId;
    args->mArg4 = interactiveRegion;

    message->SetObj((ISomeArgs*)args.Get());

    // If the interrogation is performed by the same thread as the main UI
    // thread in this process, set the message as a static reference so
    // after this call completes the same thread but in the interrogating
    // client can handle the message to generate the result.
    if (interrogatingPid == mMyProcessId && interrogatingTid == mMyLooperThreadId) {
        AutoPtr<IAccessibilityInteractionClient> aic = CAccessibilityInteractionClient::GetInstanceForThread(interrogatingTid);
        aic->SetSameThreadMessage(message);
    }
    else {
        Boolean result;
        mHandler->SendMessage(message, &result);
    }
    return NOERROR;
}

void AccessibilityInteractionController::FindAccessibilityNodeInfosByTextUiThread(
    /* [in] */ IMessage* message)
{
    Int32 flags;
    message->GetArg1(&flags);

    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    SomeArgs* args = (SomeArgs*)ISomeArgs::Probe(obj);
    AutoPtr<ICharSequence> seq = ICharSequence::Probe(args->mArg1);
    String text;
    seq->ToString(&text);
    AutoPtr<IIAccessibilityInteractionConnectionCallback> callback =
            IIAccessibilityInteractionConnectionCallback::Probe(args->mArg2);
    AutoPtr<IMagnificationSpec> spec = IMagnificationSpec::Probe(args->mArg3);
    const Int32 accessibilityViewId = args->mArgi1;
    const Int32 virtualDescendantId = args->mArgi2;
    const Int32 interactionId = args->mArgi3;
    AutoPtr<IRegion> interactiveRegion = IRegion::Probe(args->mArg4);
    args->Recycle();

    AutoPtr<IList> infos;
    // try {
    if (mViewRootImpl->mView == NULL || mViewRootImpl->mAttachInfo == NULL) {
        return;
    }
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = flags;
    AutoPtr<IView> root;
    if (accessibilityViewId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
        root = FindViewByAccessibilityId(accessibilityViewId);
    }
    else {
        root = mViewRootImpl->mView;
    }

    if (root != NULL && IsShown(root)) {
        AutoPtr<IAccessibilityNodeProvider> provider;
        root->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
        if (provider != NULL) {
            if (virtualDescendantId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
                provider->FindAccessibilityNodeInfosByText(text,
                        virtualDescendantId, (IList**)&infos);
            }
            else {
                provider->FindAccessibilityNodeInfosByText(text,
                        IAccessibilityNodeProvider::HOST_VIEW_ID, (IList**)&infos);
            }
        }
        else if (virtualDescendantId == IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
            AutoPtr<IArrayList> foundViews = mTempArrayList;
            foundViews->Clear();
            root->FindViewsWithText(foundViews, seq,
                    IView::FIND_VIEWS_WITH_TEXT
                    | IView::FIND_VIEWS_WITH_CONTENT_DESCRIPTION
                    | IView::FIND_VIEWS_WITH_ACCESSIBILITY_NODE_PROVIDERS);
            Boolean res;
            if (foundViews->IsEmpty(&res), !res) {
                infos = IList::Probe(mTempAccessibilityNodeInfoList);
                infos->Clear();

                Int32 viewCount;
                foundViews->GetSize(&viewCount);
                for (Int32 i = 0; i < viewCount; i++) {
                    AutoPtr<IInterface> obj;
                    foundViews->Get(i, (IInterface**)&obj);
                    AutoPtr<IView> foundView = IView::Probe(obj);
                    if (IsShown(foundView)) {
                        provider = NULL;
                        foundView->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
                        if (provider != NULL) {
                            AutoPtr<IList> infosFromProvider;
                            provider->FindAccessibilityNodeInfosByText(text,
                                    IAccessibilityNodeProvider::HOST_VIEW_ID, (IList**)&infosFromProvider);
                            if (infosFromProvider != NULL) {
                                infos->AddAll(ICollection::Probe(infosFromProvider));
                            }
                        }
                        else {
                            AutoPtr<IAccessibilityNodeInfo> nodeInfo;
                            foundView->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&nodeInfo);
                            infos->Add(nodeInfo);
                        }
                    }
                }
            }
        }
    }
    // } finally {
    //     try {
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = 0;
    ApplyAppScaleAndMagnificationSpecIfNeeded(infos, spec);
    if (spec != NULL) {
        spec->Recycle();
    }
    AdjustIsVisibleToUserIfNeeded(infos, interactiveRegion);
    callback->SetFindAccessibilityNodeInfosResult(infos, interactionId);

    //     } catch (RemoteException re) {
    //         /* ignore - the other side will time out */
    //     }
    // }
}

ECode AccessibilityInteractionController::FindFocusClientThread(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 focusType,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(
            PrivateHandler::MSG_FIND_FOCUS,
            flags, focusType, (IMessage**)&message);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArgi1 = interactionId;
    args->mArgi2 = CAccessibilityNodeInfo::GetAccessibilityViewId(accessibilityNodeId);
    args->mArgi3 = CAccessibilityNodeInfo::GetVirtualDescendantId(accessibilityNodeId);
    args->mArg1 = callback;
    args->mArg2 = spec;
    args->mArg3 = interactiveRegion;

    message->SetObj((ISomeArgs*)args.Get());

    // If the interrogation is performed by the same thread as the main UI
    // thread in this process, set the message as a static reference so
    // after this call completes the same thread but in the interrogating
    // client can handle the message to generate the result.
    if (interogatingPid == mMyProcessId && interrogatingTid == mMyLooperThreadId) {
        AutoPtr<IAccessibilityInteractionClient> aic = CAccessibilityInteractionClient::GetInstanceForThread(interrogatingTid);
        aic->SetSameThreadMessage(message);
    }
    else {
        Boolean result;
        mHandler->SendMessage(message, &result);
    }
    return NOERROR;
}

void AccessibilityInteractionController::FindFocusUiThread(
    /* [in] */ IMessage* message)
{
    Int32 flags;
    message->GetArg1(&flags);
    Int32 focusType;
    message->GetArg2(&focusType);

    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    SomeArgs* args = (SomeArgs*)ISomeArgs::Probe(obj);
    const Int32 interactionId = args->mArgi1;
    const Int32 accessibilityViewId = args->mArgi2;
    const Int32 virtualDescendantId = args->mArgi3;
    AutoPtr<IIAccessibilityInteractionConnectionCallback> callback =
            IIAccessibilityInteractionConnectionCallback::Probe(args->mArg1);
    AutoPtr<IMagnificationSpec> spec = IMagnificationSpec::Probe(args->mArg2);
    AutoPtr<IRegion> interactiveRegion = IRegion::Probe(args->mArg3);
    args->Recycle();

    AutoPtr<IAccessibilityNodeInfo> focused;
    // try {
    if (mViewRootImpl->mView == NULL || mViewRootImpl->mAttachInfo == NULL) {
        return;
    }
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = flags;
    AutoPtr<IView> root;
    if (accessibilityViewId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
        root = FindViewByAccessibilityId(accessibilityViewId);
    }
    else {
        root = mViewRootImpl->mView;
    }

    if (root != NULL && IsShown(root)) {
        switch (focusType) {
            case IAccessibilityNodeInfo::FOCUS_ACCESSIBILITY: {
                AutoPtr<IView> host = mViewRootImpl->mAccessibilityFocusedHost;
                // If there is no accessibility focus host or it is not a descendant
                // of the root from which to start the search, then the search failed.
                if (host == NULL || !ViewRootImpl::IsViewDescendantOf(host, root)) {
                    break;
                }
                // The focused view not shown, we failed.
                if (!IsShown(host)) {
                    break;
                }
                // If the host has a provider ask this provider to search for the
                // focus instead fetching all provider nodes to do the search here.
                AutoPtr<IAccessibilityNodeProvider> provider;
                host->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
                if (provider != NULL) {
                    if (mViewRootImpl->mAccessibilityFocusedVirtualView != NULL) {
                        CAccessibilityNodeInfo::Obtain(
                                mViewRootImpl->mAccessibilityFocusedVirtualView,
                                (IAccessibilityNodeInfo**)&focused);
                    }
                }
                else if (virtualDescendantId == IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
                    host->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&focused);
                }
            } break;

            case IAccessibilityNodeInfo::FOCUS_INPUT: {
                AutoPtr<IView> target;
                root->FindFocus((IView**)&target);
                if (target == NULL && !IsShown(target)) {
                    break;
                }
                AutoPtr<IAccessibilityNodeProvider> provider;
                target->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
                if (provider != NULL) {
                    provider->FindFocus(focusType, (IAccessibilityNodeInfo**)&focused);
                }
                if (focused == NULL) {
                    target->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&focused);
                }
            } break;

            default:
                Slogger::E("AccessibilityInteractionController", "Unknown focus type:  %d", focusType);
                break;
                // throw new IllegalArgumentException("Unknown focus type: " + focusType);
        }
    }
    // } finally {
    //     try {
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = 0;
    ApplyAppScaleAndMagnificationSpecIfNeeded(focused, spec);
    if (spec != NULL) {
        spec->Recycle();
    }
    AdjustIsVisibleToUserIfNeeded(focused, interactiveRegion);
    callback->SetFindAccessibilityNodeInfoResult(focused, interactionId);
    //      } catch (RemoteException re) {
    //             /* ignore - the other side will time out */
    //     }
    // }
}

ECode AccessibilityInteractionController::FocusSearchClientThread(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 direction,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    Int32 avId = CAccessibilityNodeInfo::GetAccessibilityViewId(accessibilityNodeId);

    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(
            PrivateHandler::MSG_FOCUS_SEARCH,
            flags, avId, (IMessage**)&message);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArgi2 = direction;
    args->mArgi3 = interactionId;
    args->mArg1 = callback;
    args->mArg2 = spec;
    args->mArg3 = interactiveRegion;

    message->SetObj((ISomeArgs*)args.Get());

    // If the interrogation is performed by the same thread as the main UI
    // thread in this process, set the message as a static reference so
    // after this call completes the same thread but in the interrogating
    // client can handle the message to generate the result.
    if (interogatingPid == mMyProcessId && interrogatingTid == mMyLooperThreadId) {
        AutoPtr<IAccessibilityInteractionClient> aic = CAccessibilityInteractionClient::GetInstanceForThread(interrogatingTid);
        aic->SetSameThreadMessage(message);
    }
    else {
        Boolean result;
        mHandler->SendMessage(message, &result);
    }
    return NOERROR;
}

void AccessibilityInteractionController::FocusSearchUiThread(
    /* [in] */ IMessage* message)
{
    Int32 flags;
    message->GetArg1(&flags);
    Int32 accessibilityViewId;
    message->GetArg2(&accessibilityViewId);

    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    SomeArgs* args = (SomeArgs*)ISomeArgs::Probe(obj);
    const Int32 direction = args->mArgi2;
    const Int32 interactionId = args->mArgi3;
    AutoPtr<IIAccessibilityInteractionConnectionCallback> callback =
            IIAccessibilityInteractionConnectionCallback::Probe(args->mArg1);
    AutoPtr<IMagnificationSpec> spec = IMagnificationSpec::Probe(args->mArg2);
    AutoPtr<IRegion> interactiveRegion = IRegion::Probe(args->mArg3);

    args->Recycle();

    AutoPtr<IAccessibilityNodeInfo> next;
    if (mViewRootImpl->mView == NULL || mViewRootImpl->mAttachInfo == NULL) {
        return;
    }
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = flags;
    AutoPtr<IView> root;
    if (accessibilityViewId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
        root = FindViewByAccessibilityId(accessibilityViewId);
    }
    else {
        root = mViewRootImpl->mView;
    }
    if (root != NULL && IsShown(root)) {
        AutoPtr<IView> nextView;
        root->FocusSearch(direction, (IView**)&nextView);
        if (nextView != NULL) {
            nextView->CreateAccessibilityNodeInfo((IAccessibilityNodeInfo**)&next);
        }
    }
    // } finally {
    //     try {
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = 0;
    ApplyAppScaleAndMagnificationSpecIfNeeded(next, spec);
    if (spec != NULL) {
        spec->Recycle();
    }
    AdjustIsVisibleToUserIfNeeded(next, interactiveRegion);
    callback->SetFindAccessibilityNodeInfoResult(next, interactionId);
    //     } catch (RemoteException re) {
    //          ignore - the other side will time out
    //     }
    // }
}

ECode AccessibilityInteractionController::PerformAccessibilityActionClientThread(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 interogatingPid,
    /* [in] */ Int64 interrogatingTid)
{
    Int32 avId = CAccessibilityNodeInfo::GetAccessibilityViewId(accessibilityNodeId);

    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(
            PrivateHandler::MSG_PERFORM_ACCESSIBILITY_ACTION,
            flags, avId, (IMessage**)&message);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArgi1 = CAccessibilityNodeInfo::GetVirtualDescendantId(accessibilityNodeId);
    args->mArgi2 = action;
    args->mArgi3 = interactionId;
    args->mArg1 = callback;
    args->mArg2 = arguments;

    message->SetObj((ISomeArgs*)args.Get());

    // If the interrogation is performed by the same thread as the main UI
    // thread in this process, set the message as a static reference so
    // after this call completes the same thread but in the interrogating
    // client can handle the message to generate the result.
    if (interogatingPid == mMyProcessId && interrogatingTid == mMyLooperThreadId) {
        AutoPtr<IAccessibilityInteractionClient> aic = CAccessibilityInteractionClient::GetInstanceForThread(interrogatingTid);
        aic->SetSameThreadMessage(message);
    }
    else {
        Boolean result;
        mHandler->SendMessage(message, &result);
    }
    return NOERROR;
}

void AccessibilityInteractionController::PerfromAccessibilityActionUiThread(
    /* [in] */ IMessage* message)
{
    Int32 flags;
    message->GetArg1(&flags);
    Int32 accessibilityViewId;
    message->GetArg2(&accessibilityViewId);

    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    SomeArgs* args = (SomeArgs*)ISomeArgs::Probe(obj);
    const Int32 virtualDescendantId = args->mArgi1;
    const Int32 action = args->mArgi2;
    const Int32 interactionId = args->mArgi3;
    AutoPtr<IIAccessibilityInteractionConnectionCallback> callback =
        IIAccessibilityInteractionConnectionCallback::Probe(args->mArg1);
    AutoPtr<IBundle> arguments = IBundle::Probe(args->mArg2);

    args->Recycle();

    Boolean succeeded = FALSE;
    // try {
    if (mViewRootImpl->mView == NULL || mViewRootImpl->mAttachInfo == NULL) {
        return;
    }

    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = flags;
    AutoPtr<IView> target;
    if (accessibilityViewId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
        target = FindViewByAccessibilityId(accessibilityViewId);
    }
    else {
        target = mViewRootImpl->mView;
    }
    if (target != NULL && IsShown(target)) {
        AutoPtr<IAccessibilityNodeProvider> provider;
        target->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
        if (provider != NULL) {
            if (virtualDescendantId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
                provider->PerformAction(virtualDescendantId, action, arguments, &succeeded);
            }
            else {
                provider->PerformAction(IAccessibilityNodeProvider::HOST_VIEW_ID,
                        action, arguments, &succeeded);
            }
        }
        else if (virtualDescendantId == IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
            target->PerformAccessibilityAction(action, arguments, &succeeded);
        }
    }
    // } finally {
    //     try {
    mViewRootImpl->mAttachInfo->mAccessibilityFetchFlags = 0;
    callback->SetPerformAccessibilityActionResult(succeeded, interactionId);
    //     } catch (RemoteException re) {
    //          ignore - the other side will time out
    //     }
    // }
}

ECode AccessibilityInteractionController::ComputeClickPointInScreenClientThread(
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ IRegion* interactiveRegion,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 interrogatingPid,
    /* [in] */ Int64 interrogatingTid,
    /* [in] */ IMagnificationSpec* spec)
{
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage((IMessage**)&message);
    message->SetWhat(PrivateHandler::MSG_COMPUTE_CLICK_POINT_IN_SCREEN);

    AutoPtr<SomeArgs> args = SomeArgs::Obtain();
    args->mArgi1 = CAccessibilityNodeInfo::GetAccessibilityViewId(accessibilityNodeId);
    args->mArgi2 = CAccessibilityNodeInfo::GetVirtualDescendantId(accessibilityNodeId);
    args->mArgi3 = interactionId;
    args->mArg1 = callback;
    args->mArg2 = spec;
    args->mArg3 = interactiveRegion;

    message->SetObj((ISomeArgs*)args.Get());

    // If the interrogation is performed by the same thread as the main UI
    // thread in this process, set the message as a static reference so
    // after this call completes the same thread but in the interrogating
    // client can handle the message to generate the result.
    if (interrogatingPid == mMyProcessId && interrogatingTid == mMyLooperThreadId) {
        AutoPtr<IAccessibilityInteractionClient> aic = CAccessibilityInteractionClient::GetInstanceForThread(interrogatingTid);
        aic->SetSameThreadMessage(message);
    }
    else {
        Boolean result;
        mHandler->SendMessage(message, &result);
    }
    return NOERROR;
}

void AccessibilityInteractionController::ComputeClickPointInScreenUiThread(
    /* [in] */ IMessage* message)
{
    AutoPtr<IInterface> obj;
    message->GetObj((IInterface**)&obj);
    SomeArgs* args = (SomeArgs*)ISomeArgs::Probe(obj);
    const Int32 accessibilityViewId = args->mArgi1;
    const Int32 virtualDescendantId = args->mArgi2;
    const Int32 interactionId = args->mArgi3;
    AutoPtr<IIAccessibilityInteractionConnectionCallback> callback =
            IIAccessibilityInteractionConnectionCallback::Probe(args->mArg1);
    AutoPtr<IMagnificationSpec> spec = IMagnificationSpec::Probe(args->mArg2);
    AutoPtr<IRegion> interactiveRegion = IRegion::Probe(args->mArg3);
    args->Recycle();

    Boolean succeeded = FALSE;
    AutoPtr<IPoint> point = mTempPoint;
    // try {
    if (mViewRootImpl->mView == NULL || mViewRootImpl->mAttachInfo == NULL) {
        return;
    }

    AutoPtr<IView> target;
    if (accessibilityViewId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
        target = FindViewByAccessibilityId(accessibilityViewId);
    }
    else {
        target = mViewRootImpl->mView;
    }
    if (target != NULL && IsShown(target)) {
        AutoPtr<IAccessibilityNodeProvider> provider;
        target->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
        if (provider != NULL) {
            // For virtual views just use the center of the bounds in screen.
            AutoPtr<IAccessibilityNodeInfo> node;
            if (virtualDescendantId != IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
                provider->CreateAccessibilityNodeInfo(virtualDescendantId,
                        (IAccessibilityNodeInfo**)&node);
            }
            else {
                provider->CreateAccessibilityNodeInfo(IAccessibilityNodeProvider::HOST_VIEW_ID,
                        (IAccessibilityNodeInfo**)&node);
            }
            if (node != NULL) {
                succeeded = TRUE;
                AutoPtr<IRect> boundsInScreen = mTempRect;
                node->GetBoundsInScreen(boundsInScreen);
                Int32 x, y;
                boundsInScreen->GetCenterX(&x);
                boundsInScreen->GetCenterY(&y);
                point->Set(x, y);
            }
        }
        else if (virtualDescendantId == IAccessibilityNodeInfo::UNDEFINED_ITEM_ID) {
            // For a real view, ask the view to compute the click point.
            ((View*)target.Get())->ComputeClickPointInScreenForAccessibility(interactiveRegion, point, &succeeded);
        }
    }
    // } finally {
    //     try {
    AutoPtr<IPoint> result;
    if (succeeded) {
        ApplyAppScaleAndMagnificationSpecIfNeeded(point, spec);
        result = point;
    }
    callback->SetComputeClickPointInScreenActionResult(result, interactionId);
    //     } catch (RemoteException re) {
    //          ignore - the other side will time out
    //     }
    // }
}

AutoPtr<IView> AccessibilityInteractionController::FindViewByAccessibilityId(
    /* [in] */ Int32 accessibilityId)
{
    AutoPtr<IView> root = mViewRootImpl->mView;
    if (root == NULL) {
        return NULL;
    }

    AutoPtr<IView> foundView = ((View*)root.Get())->FindViewByAccessibilityId(accessibilityId);
    if (foundView != NULL && !IsShown(foundView)) {
        return NULL;
    }
    return foundView;
}

void AccessibilityInteractionController::ApplyAppScaleAndMagnificationSpecIfNeeded(
    /* [in] */ IList* infos,
    /* [in] */ IMagnificationSpec* spec)
{
    if (infos == NULL) {
        return;
    }
    const Float applicationScale = mViewRootImpl->mAttachInfo->mApplicationScale;
    if (ShouldApplyAppScaleAndMagnificationSpec(applicationScale, spec)) {
        Int32 infoCount;
        infos->GetSize(&infoCount);
        for (Int32 i = 0; i < infoCount; i++) {
            AutoPtr<IInterface> obj;
            infos->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityNodeInfo> info = IAccessibilityNodeInfo::Probe(obj);
            ApplyAppScaleAndMagnificationSpecIfNeeded(info, spec);
        }
    }
}

void AccessibilityInteractionController::AdjustIsVisibleToUserIfNeeded(
    /* [in] */ IList* infos,
    /* [in] */ IRegion* interactiveRegion)
{
    if (interactiveRegion == NULL || infos == NULL) {
        return;
    }

    Int32 infoCount;
    infos->GetSize(&infoCount);
    for (Int32 i = 0; i < infoCount; i++) {
        AutoPtr<IInterface> obj;
        infos->Get(i, (IInterface**)&obj);
        AutoPtr<IAccessibilityNodeInfo> info = IAccessibilityNodeInfo::Probe(obj);
        AdjustIsVisibleToUserIfNeeded(info, interactiveRegion);
    }
}

void AccessibilityInteractionController::AdjustIsVisibleToUserIfNeeded(
    /* [in] */ IAccessibilityNodeInfo* info,
    /* [in] */ IRegion* interactiveRegion)
{
    if (interactiveRegion == NULL || info == NULL) {
        return;
    }

    AutoPtr<IRect> boundsInScreen = mTempRect;
    info->GetBoundsInScreen(boundsInScreen);
    Boolean res;
    if (interactiveRegion->QuickReject(boundsInScreen, &res), res) {
        info->SetVisibleToUser(FALSE);
    }
}

void AccessibilityInteractionController::ApplyAppScaleAndMagnificationSpecIfNeeded(
    /* [in] */ IPoint* point,
    /* [in] */ IMagnificationSpec* spec)
{
    const Float applicationScale = mViewRootImpl->mAttachInfo->mApplicationScale;
    if (!ShouldApplyAppScaleAndMagnificationSpec(applicationScale, spec)) {
        return;
    }

    if (applicationScale != 1.0f) {
        Int32 x, y;
        point->GetX(&x);
        point->GetY(&y);
        x *= applicationScale;
        y *= applicationScale;
        point->Set(x, y);
    }

    if (spec != NULL) {
        Int32 x, y;
        point->Get(&x, &y);
        Float scale;
        spec->GetScale(&scale);
        x *= scale;
        y *= scale;

        Float offsetX, offsetY;
        spec->GetOffsetX(&offsetX);
        spec->GetOffsetY(&offsetY);
        x += (Int32)offsetX;
        y += (Int32)offsetY;
        point->Set(x, y);
    }
}

void AccessibilityInteractionController::ApplyAppScaleAndMagnificationSpecIfNeeded(
    /* [in] */ IAccessibilityNodeInfo* info,
    /* [in] */ IMagnificationSpec* spec)
{
    if (info == NULL) {
        return;
    }

    const Float applicationScale = mViewRootImpl->mAttachInfo->mApplicationScale;
    if (!ShouldApplyAppScaleAndMagnificationSpec(applicationScale, spec)) {
        return;
    }

    AutoPtr<IRect> boundsInParent = mTempRect;
    AutoPtr<IRect> boundsInScreen = mTempRect1;

    info->GetBoundsInParent(boundsInParent);
    info->GetBoundsInScreen(boundsInScreen);
    if (applicationScale != 1.0f) {
        boundsInParent->Scale(applicationScale);
        boundsInScreen->Scale(applicationScale);
    }
    if (spec != NULL) {
        Float scale;
        spec->GetScale(&scale);
        boundsInParent->Scale(scale);
        // boundsInParent must not be offset.
        boundsInScreen->Scale(scale);
        Float offsetX, offsetY;
        spec->GetOffsetX(&offsetX);
        spec->GetOffsetY(&offsetY);
        boundsInScreen->Offset((Int32)offsetX, (Int32)offsetY);
    }

    info->SetBoundsInParent(boundsInParent);
    info->SetBoundsInScreen(boundsInScreen);

    if (spec != NULL) {
        AutoPtr<View::AttachInfo> attachInfo = mViewRootImpl->mAttachInfo;
        if (attachInfo->mDisplay == NULL) {
            return;
        }

        Float scale0;
        spec->GetScale(&scale0);
        const Float scale = attachInfo->mApplicationScale * scale0;

        AutoPtr<IRect> visibleWinFrame = mTempRect1;

        Float offsetX, offsetY;
        spec->GetOffsetX(&offsetX);
        spec->GetOffsetY(&offsetY);

        Int32 left, top, right, bottom;
        left = (Int32) (attachInfo->mWindowLeft * scale + offsetX);
        top = (Int32) (attachInfo->mWindowTop * scale + offsetY);
        right = (Int32) (left + mViewRootImpl->mWidth * scale);
        bottom = (Int32) (top + mViewRootImpl->mHeight * scale);
        visibleWinFrame->Set(left, top, right, bottom);

        attachInfo->mDisplay->GetRealSize(mTempPoint);
        Int32 displayWidth;
        Int32 displayHeight;
        mTempPoint->Get(&displayWidth, &displayHeight);

        AutoPtr<IRect> visibleDisplayFrame = mTempRect2;
        visibleDisplayFrame->Set(0, 0, displayWidth, displayHeight);

        Boolean res;
        visibleWinFrame->Intersect(visibleDisplayFrame, &res);

        boundsInScreen->Get(&left, &top, &right, &bottom);
        if (visibleWinFrame->Intersects(left, top, right, bottom, &res), !res) {
            info->SetVisibleToUser(FALSE);
        }
    }
}

Boolean AccessibilityInteractionController::ShouldApplyAppScaleAndMagnificationSpec(
    /* [in] */ Float appScale,
    /* [in] */ IMagnificationSpec* spec)
{
    Boolean res;
    return appScale != 1.0f || (spec != NULL && (spec->IsNop(&res), !res));
}

} // View
} // Droid
} // Elastos
