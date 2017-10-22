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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityCache.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/view/accessibility/CAccessibilityWindowInfo.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/utility/CArraySet.h"
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::CInt64SparseArray;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::IInt64Array;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const String CAccessibilityCache::TAG("AccessibilityCache");
const Boolean CAccessibilityCache::DEBUG = FALSE;
const Boolean CAccessibilityCache::CHECK_INTEGRITY = Build::TYPE.Equals("eng");

CAR_INTERFACE_IMPL(CAccessibilityCache, Object, IAccessibilityCache)

CAR_OBJECT_IMPL(CAccessibilityCache)

CAccessibilityCache::CAccessibilityCache()
{
    CSparseArray::New((ISparseArray**)&mWindowCache);
    CSparseArray::New((ISparseArray**)&mNodeCache);
    CSparseArray::New((ISparseArray**)&mTempWindowArray);
}

CAccessibilityCache::~CAccessibilityCache()
{
}

ECode CAccessibilityCache::constructor()
{
    return NOERROR;
}

ECode CAccessibilityCache::AddWindow(
    /* [in] */ IAccessibilityWindowInfo* window)
{
    {    AutoLock syncLock(mLock);
        Int32 windowId;
        window->GetId(&windowId);
        if (DEBUG) {
            Logger::I(TAG, "Caching window: %d", windowId);
        }
        AutoPtr<IInterface> obj;
        mWindowCache->Get(windowId, (IInterface**)&obj);
        AutoPtr<IAccessibilityWindowInfo> oldWindow = IAccessibilityWindowInfo::Probe(obj);
        if (oldWindow != NULL) {
            oldWindow->Recycle();
        }
        AutoPtr<IAccessibilityWindowInfo> cloneInfo;
        CAccessibilityWindowInfo::Obtain(window, (IAccessibilityWindowInfo**)&cloneInfo);
        mWindowCache->Put(windowId, cloneInfo);
    }
    return NOERROR;
}

ECode CAccessibilityCache::OnAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    {    AutoLock syncLock(mLock);
        Int32 eventType;
        event->GetEventType(&eventType);
        switch (eventType) {
            case IAccessibilityEvent::TYPE_VIEW_FOCUSED:
            case IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED:
            case IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED:
            case IAccessibilityEvent::TYPE_VIEW_SELECTED:
            case IAccessibilityEvent::TYPE_VIEW_TEXT_CHANGED:
            case IAccessibilityEvent::TYPE_VIEW_TEXT_SELECTION_CHANGED:{
                Int32 windowId;
                IAccessibilityRecord::Probe(event)->GetWindowId(&windowId);
                Int64 sourceId;
                IAccessibilityRecord::Probe(event)->GetSourceNodeId(&sourceId);
                RefreshCachedNodeLocked(windowId, sourceId);
                break;
            }
            case IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED: {
                {    AutoLock syncLock(mLock);
                    Int32 windowId;
                    IAccessibilityRecord::Probe(event)->GetWindowId(&windowId);
                    Int64 sourceId;
                    IAccessibilityRecord::Probe(event)->GetSourceNodeId(&sourceId);
                    Int32 type;
                    event->GetContentChangeTypes(&type);
                    if ((type & IAccessibilityEvent::CONTENT_CHANGE_TYPE_SUBTREE) != 0) {
                        ClearSubTreeLocked(windowId, sourceId);
                    }
                    else {
                        RefreshCachedNodeLocked(windowId, sourceId);
                    }
                }
                break;
            }
            case IAccessibilityEvent::TYPE_VIEW_SCROLLED: {
                Int32 windowId;
                IAccessibilityRecord::Probe(event)->GetWindowId(&windowId);
                Int64 sourceId;
                IAccessibilityRecord::Probe(event)->GetSourceNodeId(&sourceId);
                ClearSubTreeLocked(windowId, sourceId);
                break;
            }
            case IAccessibilityEvent::TYPE_WINDOWS_CHANGED:
            case IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED: {
                Clear();
                break;
            }
            default:
                break;
        }
    }

    if (CHECK_INTEGRITY) {
        CheckIntegrity();
    }
    return NOERROR;
}

void CAccessibilityCache::RefreshCachedNodeLocked(
    /* [in] */ Int32 windowId,
    /* [in] */ Int64 sourceId)
{
    if (DEBUG) {
        Logger::I(TAG, "Refreshing cached node.");
    }

    AutoPtr<IInterface> obj;
    mNodeCache->Get(windowId, (IInterface**)&obj);
    AutoPtr<IInt64SparseArray> nodes = IInt64SparseArray::Probe(obj);
    if (nodes == NULL) {
        return;
    }
    obj = NULL;
    nodes->Get(sourceId, (IInterface**)&obj);
    AutoPtr<IAccessibilityNodeInfo> cachedInfo = IAccessibilityNodeInfo::Probe(obj);
    // If the source is not in the cache - nothing to do.
    if (cachedInfo == NULL) {
        return;
    }
    // The node changed so we will just refresh it right now.
    Boolean res;
    if ((cachedInfo->Refresh(TRUE, &res), res)) {
        return;
    }
    // Weird, we could not refresh. Just evict the entire sub-tree.
    ClearSubTreeLocked(windowId, sourceId);
}

ECode CAccessibilityCache::GetNode(
    /* [in] */ Int32 windowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    {    AutoLock syncLock(mLock);
        AutoPtr<IInterface> obj;
        mNodeCache->Get(windowId, (IInterface**)&obj);
        AutoPtr<IInt64SparseArray> nodes = IInt64SparseArray::Probe(obj);
        if (nodes == NULL) {
            return NOERROR;
        }
        obj = NULL;
        nodes->Get(accessibilityNodeId, (IInterface**)&obj);
        AutoPtr<IAccessibilityNodeInfo> _info = IAccessibilityNodeInfo::Probe(obj);
        if (_info != NULL) {
            // Return a copy since the client calls to AccessibilityNodeInfo#recycle()
            // will wipe the data of the cached info.
            AutoPtr<IAccessibilityNodeInfo> otherInfo;
            CAccessibilityNodeInfo::Obtain(_info, (IAccessibilityNodeInfo**)&otherInfo);
            _info = otherInfo;
        }
        if (DEBUG) {
            Logger::I(TAG, "get(%lld) = %p ", accessibilityNodeId, _info.Get());
        }

        *info = _info;
        REFCOUNT_ADD(*info);
        return NOERROR;
    }
    return NOERROR;
}

ECode CAccessibilityCache::GetWindows(
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    {    AutoLock syncLock(mLock);
        Int32 windowCount;
        mWindowCache->GetSize(&windowCount);
        if (windowCount > 0) {
            // Careful to return the windows in a decreasing layer order.
            AutoPtr<ISparseArray> sortedWindows = mTempWindowArray;
            sortedWindows->Clear();

            for (Int32 i = 0; i < windowCount; i++) {
                AutoPtr<IInterface> obj;
                mWindowCache->ValueAt(i, (IInterface**)&obj);
                AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);
                Int32 key;
                window->GetLayer(&key);
                sortedWindows->Put(key, window);
            }

            AutoPtr<IList> windows;
            CArrayList::New(windowCount, (IList**)&windows);
            for (Int32 i = windowCount - 1; i >= 0; i--) {
                AutoPtr<IInterface> obj;
                sortedWindows->ValueAt(i, (IInterface**)&obj);
                AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);
                AutoPtr<IAccessibilityWindowInfo> info;
                CAccessibilityWindowInfo::Obtain(window, (IAccessibilityWindowInfo**)&info);
                windows->Add(info);
                sortedWindows->RemoveAt(i);
            }
            *list = windows;
            REFCOUNT_ADD(*list);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CAccessibilityCache::GetWindow(
    /* [in] */ Int32 windowId,
    /* [out] */ IAccessibilityWindowInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    {    AutoLock syncLock(mLock);
        AutoPtr<IInterface> obj;
        mWindowCache->Get(windowId, (IInterface**)&obj);
        AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);
        if (window != NULL) {
            AutoPtr<IAccessibilityWindowInfo> otherInfo;
            CAccessibilityWindowInfo::Obtain(window, (IAccessibilityWindowInfo**)&otherInfo);
            *info = otherInfo;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CAccessibilityCache::Add(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    {    AutoLock syncLock(mLock);
        if (DEBUG) {
            Logger::I(TAG, "add(%p) ", info);
        }

        Int32 windowId;
        info->GetWindowId(&windowId);
        AutoPtr<IInterface> obj;
        mNodeCache->Get(windowId, (IInterface**)&obj);
        AutoPtr<IInt64SparseArray> nodes = IInt64SparseArray::Probe(obj);
        if (nodes == NULL) {
            CInt64SparseArray::New((IInt64SparseArray**)&nodes);
            mNodeCache->Put(windowId, nodes);
        }

        Int64 sourceId;
        info->GetSourceNodeId(&sourceId);
        obj = NULL;
        nodes->Get(sourceId, (IInterface**)&obj);
        AutoPtr<IAccessibilityNodeInfo> oldInfo = IAccessibilityNodeInfo::Probe(obj);
        if (oldInfo != NULL) {
            // If the added node is in the cache we have to be careful if
            // the new one represents a source state where some of the
            // children have been removed to remove the descendants that
            // are no longer present.
            AutoPtr<IInt64Array> newChildrenIds;
            info->GetChildNodeIds((IInt64Array**)&newChildrenIds);

            Int32 oldChildCount;
            oldInfo->GetChildCount(&oldChildCount);
            for (Int32 i = 0; i < oldChildCount; i++) {
                Int64 oldChildId;
                oldInfo->GetChildId(i, &oldChildId);
                // If the child is no longer present, remove the sub-tree.
                Int32 index;
                if (newChildrenIds == NULL || (newChildrenIds->IndexOf(oldChildId, &index), index) < 0) {
                    ClearSubTreeLocked(windowId, oldChildId);
                }
            }

            // Also be careful if the parent has changed since the new
            // parent may be a predecessor of the old parent which will
            // add cyclse to the cache.
            Int64 oldParentId;
            oldInfo->GetParentNodeId(&oldParentId);
            Int64 id;
            if (info->GetParentNodeId(&id), id != oldParentId) {
                ClearSubTreeLocked(windowId, oldParentId);
            }
       }

        // Cache a copy since the client calls to AccessibilityNodeInfo#recycle()
        // will wipe the data of the cached info.
        AutoPtr<IAccessibilityNodeInfo> clone;
        CAccessibilityNodeInfo::Obtain(info, (IAccessibilityNodeInfo**)&clone);
        nodes->Put(sourceId, clone);
    }
    return NOERROR;
}

ECode CAccessibilityCache::Clear()
{
    {    AutoLock syncLock(mLock);
        if (DEBUG) {
            Logger::I(TAG, "clear()");
        }
        Int32 windowCount;
        mWindowCache->GetSize(&windowCount);
        for (Int32 i = windowCount - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mWindowCache->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);
            window->Recycle();
            mWindowCache->RemoveAt(i);
        }
        Int32 nodesForWindowCount;
        mNodeCache->GetSize(&nodesForWindowCount);
        for (Int32 i = 0; i < nodesForWindowCount; i++) {
            Int32 windowId;
            mNodeCache->KeyAt(i, &windowId);
            ClearNodesForWindowLocked(windowId);
        }
    }
    return NOERROR;
}

void CAccessibilityCache::ClearNodesForWindowLocked(
    /* [in] */ Int32 windowId)
{
    if (DEBUG) {
        Logger::I(TAG, "clearNodesForWindowLocked(%d)", windowId);
    }
    AutoPtr<IInterface> obj;
    mNodeCache->Get(windowId, (IInterface**)&obj);
    AutoPtr<IInt64SparseArray> nodes = IInt64SparseArray::Probe(obj);
    if (nodes == NULL) {
        return;
    }
    // Recycle the nodes before clearing the cache.
    Int32 nodeCount;
    nodes->GetSize(&nodeCount);
    for (Int32 i = nodeCount - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        nodes->ValueAt(i, (IInterface**)&obj);
        AutoPtr<IAccessibilityNodeInfo> info = IAccessibilityNodeInfo::Probe(obj);
        nodes->RemoveAt(i);
        info->Recycle();
    }
    mNodeCache->Remove(windowId);
}

void CAccessibilityCache::ClearSubTreeLocked(
    /* [in] */ Int32 windowId,
    /* [in] */ Int64 rootNodeId)
{
    if (DEBUG) {
        Logger::I(TAG, "Clearing cached subtree.");
    }
    AutoPtr<IInterface> obj;
    mNodeCache->Get(windowId, (IInterface**)&obj);
    AutoPtr<IInt64SparseArray> nodes = IInt64SparseArray::Probe(obj);
    if (nodes != NULL) {
        ClearSubTreeRecursiveLocked(nodes, rootNodeId);
    }
}

void CAccessibilityCache::ClearSubTreeRecursiveLocked(
    /* [in] */ IInt64SparseArray* nodes,
    /* [in] */ Int64 rootNodeId)
{
    AutoPtr<IInterface> obj;
    nodes->Get(rootNodeId, (IInterface**)&obj);
    AutoPtr<IAccessibilityNodeInfo> current = IAccessibilityNodeInfo::Probe(obj);
    if (current == NULL) {
        return;
    }
    nodes->Remove(rootNodeId);
    Int32 childCount;
    current->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        Int64 childNodeId;
        current->GetChildId(i, &childNodeId);
        ClearSubTreeRecursiveLocked(nodes, childNodeId);
    }
}

ECode CAccessibilityCache::CheckIntegrity()
{
    {    AutoLock syncLock(mLock);
        // Get the root.
        Int32 size1, size2;
        Boolean res;
        AutoPtr<IInterface> obj;
        if ((mWindowCache->GetSize(&size1), size1 <= 0) && (mNodeCache->GetSize(&size2), size2 == 0)) {
            return NOERROR;
        }

        AutoPtr<IAccessibilityWindowInfo> focusedWindow;
        AutoPtr<IAccessibilityWindowInfo> activeWindow;

        Int32 windowCount;
        mWindowCache->GetSize(&windowCount);
        for (Int32 i = 0; i < windowCount; i++) {
            obj = NULL;
            mWindowCache->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);

            // Check for one active window.
            if (window->IsActive(&res), res) {
                if (activeWindow != NULL) {
                    Logger::E(TAG, "Duplicate active window: %p", window.Get());
                }
                else {
                    activeWindow = window;
                }
            }

            // Check for one focused window.
            if (window->IsFocused(&res), res) {
                if (focusedWindow != NULL) {
                    Logger::E(TAG, "Duplicate focused window: %p", window.Get());
                }
                else {
                    focusedWindow = window;
                }
            }
        }

        // Traverse the tree and do some checks.
        AutoPtr<IAccessibilityNodeInfo> accessFocus;
        AutoPtr<IAccessibilityNodeInfo> inputFocus;

        Int32 nodesForWindowCount;
        mNodeCache->GetSize(&nodesForWindowCount);
        for (Int32 i = 0; i < nodesForWindowCount; i++) {
            obj = NULL;
            mNodeCache->ValueAt(i, (IInterface**)&obj);
            AutoPtr<IInt64SparseArray> nodes = IInt64SparseArray::Probe(obj);
            Int32 size;
            if (nodes->GetSize(&size), size <= 0) {
                continue;
            }

            AutoPtr<IArraySet> seen;
            CArraySet::New((IArraySet**)&seen);
            Int32 windowId;
            mNodeCache->KeyAt(i, &windowId);

            Int32 nodeCount;
            nodes->GetSize(&nodeCount);
            for (Int32 j = 0; j < nodeCount; j++) {
                obj = NULL;
                mNodeCache->ValueAt(j, (IInterface**)&obj);
                AutoPtr<IAccessibilityNodeInfo> node = IAccessibilityNodeInfo::Probe(obj);

                // Check for duplicates
                if (ICollection::Probe(seen)->Add(node, &res), !res) {
                    Logger::E(TAG, "Duplicate node: %p, in window: %d", node.Get(), windowId);
                    // Stop now as we potentially found a loop.
                    continue;
                }

                // Check for one accessibility focus.
                if (node->IsAccessibilityFocused(&res), res) {
                    if (accessFocus != NULL) {
                        Logger::E(TAG, "Duplicate accessibility focus: %p, in window: %d", node.Get(), windowId);
                    }
                    else {
                        accessFocus = node;
                    }
                }

                // Check for one input focus.
                if (node->IsFocused(&res), res) {
                    if (inputFocus != NULL) {
                        Logger::E(TAG, "Duplicate input focus: %p, in window: %d", node.Get(), windowId);
                    }
                    else {
                        inputFocus = node;
                    }
                }

                // The node should be a child of its parent if we have the parent.
                Int64 parentId;
                node->GetParentNodeId(&parentId);
                obj = NULL;
                nodes->Get(parentId, (IInterface**)&obj);
                AutoPtr<IAccessibilityNodeInfo> nodeParent = IAccessibilityNodeInfo::Probe(obj);
                Int64 id;
                if (nodeParent != NULL) {
                    Boolean childOfItsParent = FALSE;
                    Int32 childCount;
                    nodeParent->GetChildCount(&childCount);
                    for (Int32 k = 0; k < childCount; k++) {
                        nodeParent->GetChildId(k, &id);
                        obj = NULL;
                        nodes->Get(id, (IInterface**)&obj);
                        AutoPtr<IAccessibilityNodeInfo> child = IAccessibilityNodeInfo::Probe(obj);
                        if (child == node) {
                            childOfItsParent = TRUE;
                            break;
                        }
                    }
                    if (!childOfItsParent) {
                        Logger::E(TAG, "Invalid parent-child relation between parent: %p, and child: %p", nodeParent.Get(), node.Get());
                    }
                }

                // The node should be the parent of its child if we have the child.
                Int32 childCount;
                node->GetChildCount(&childCount);
                for (Int32 k = 0; k < childCount; k++) {
                    node->GetChildId(k, &id);
                    obj = NULL;
                    nodes->Get(id, (IInterface**)&obj);
                    AutoPtr<IAccessibilityNodeInfo> child = IAccessibilityNodeInfo::Probe(obj);
                    if (child != NULL) {
                        child->GetParentNodeId(&parentId);
                        obj = NULL;
                        nodes->Get(parentId, (IInterface**)&obj);
                        AutoPtr<IAccessibilityNodeInfo> parent = IAccessibilityNodeInfo::Probe(obj);
                        if (parent != node) {
                            Logger::E(TAG, "Invalid child-parent relation between child: %p, and parent: %p", node.Get(), nodeParent.Get());
                        }
                    }
                }
            }
        }
    }

    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
