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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYINTERACTIONCLIENT_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYINTERACTIONCLIENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_View_Accessibility_CAccessibilityInteractionClient.h"
#include <elastos/core/Object.h>

using Elastos::Droid::AccessibilityService::IIAccessibilityServiceConnection;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::View::Accessibility::IIAccessibilityInteractionConnectionCallback;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityInteractionClient)
    , public Object
    , public IAccessibilityInteractionClient
    , public IIAccessibilityInteractionConnectionCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityInteractionClient();

    ~CAccessibilityInteractionClient();

    CARAPI constructor();

    /**
     * @return The client for the current thread.
     */
    static CARAPI_(AutoPtr<IAccessibilityInteractionClient>) GetInstance();

    /**
     * <strong>Note:</strong> We keep one instance per interrogating thread since
     * the instance contains state which can lead to undesired thread interleavings.
     * We do not have a thread local variable since other threads should be able to
     * look up the correct client knowing a thread id. See ViewRootImpl for details.
     *
     * @return The client for a given <code>threadId</code>.
     */
    static CARAPI_(AutoPtr<IAccessibilityInteractionClient>) GetInstanceForThread(
        /* [in] */ Int64 threadId);

    /**
     * Sets the message to be processed if the interacted view hierarchy
     * and the interacting client are running in the same thread.
     *
     * @param message The message.
     */
    CARAPI SetSameThreadMessage(
        /* [in] */ IMessage* message);

    /**
     * Gets the root {@link AccessibilityNodeInfo} in the currently active window.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @return The root {@link AccessibilityNodeInfo} if found, null otherwise.
     */
    CARAPI GetRootInActiveWindow(
        /* [in] */ Int32 connectionId,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Gets the info for a window.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @param accessibilityWindowId A unique window id. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ACTIVE_WINDOW_ID}
     *     to query the currently active window.
     * @return The {@link AccessibilityWindowInfo}.
     */
    CARAPI GetWindow(
        /* [in] */ Int32 connectionId,
        /* [in] */ Int32 accessibilityWindowId,
        /* [out] */ IAccessibilityWindowInfo** info);

    /**
     * Gets the info for all windows.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @return The {@link AccessibilityWindowInfo} list.
     */
    CARAPI GetWindows(
        /* [in] */ Int32 connectionId,
        /* [out] */ IList** list);

    /**
     * Finds an {@link AccessibilityNodeInfo} by accessibility id.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @param accessibilityWindowId A unique window id. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ACTIVE_WINDOW_ID}
     *     to query the currently active window.
     * @param accessibilityNodeId A unique view id or virtual descendant id from
     *     where to start the search. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ROOT_NODE_ID}
     *     to start from the root.
     * @param bypassCache Whether to bypass the cache while looking for the node.
     * @param prefetchFlags flags to guide prefetching.
     * @return An {@link AccessibilityNodeInfo} if found, null otherwise.
     */
    CARAPI FindAccessibilityNodeInfoByAccessibilityId(
        /* [in] */ Int32 connectionId,
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Boolean bypassCache,
        /* [in] */ Int32 prefetchFlags,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Finds an {@link AccessibilityNodeInfo} by View id. The search is performed in
     * the window whose id is specified and starts from the node whose accessibility
     * id is specified.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @param accessibilityWindowId A unique window id. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ACTIVE_WINDOW_ID}
     *     to query the currently active window.
     * @param accessibilityNodeId A unique view id or virtual descendant id from
     *     where to start the search. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ROOT_NODE_ID}
     *     to start from the root.
     * @param viewId The fully qualified resource name of the view id to find.
     * @return An list of {@link AccessibilityNodeInfo} if found, empty list otherwise.
     */
    CARAPI FindAccessibilityNodeInfosByViewId(
        /* [in] */ Int32 connectionId,
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ const String& viewId,
        /* [out] */ IList** list);

    /**
     * Finds {@link AccessibilityNodeInfo}s by View text. The match is case
     * insensitive containment. The search is performed in the window whose
     * id is specified and starts from the node whose accessibility id is
     * specified.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @param accessibilityWindowId A unique window id. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ACTIVE_WINDOW_ID}
     *     to query the currently active window.
     * @param accessibilityNodeId A unique view id or virtual descendant id from
     *     where to start the search. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ROOT_NODE_ID}
     *     to start from the root.
     * @param text The searched text.
     * @return A list of found {@link AccessibilityNodeInfo}s.
     */
    CARAPI FindAccessibilityNodeInfosByText(
        /* [in] */ Int32 connectionId,
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ const String& text,
        /* [out] */ IList** list);

    /**
     * Finds the {@link android.view.accessibility.AccessibilityNodeInfo} that has the
     * specified focus type. The search is performed in the window whose id is specified
     * and starts from the node whose accessibility id is specified.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @param accessibilityWindowId A unique window id. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ACTIVE_WINDOW_ID}
     *     to query the currently active window.
     * @param accessibilityNodeId A unique view id or virtual descendant id from
     *     where to start the search. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ROOT_NODE_ID}
     *     to start from the root.
     * @param focusType The focus type.
     * @return The accessibility focused {@link AccessibilityNodeInfo}.
     */
    CARAPI FindFocus(
        /* [in] */ Int32 connectionId,
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 focusType,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Finds the accessibility focused {@link android.view.accessibility.AccessibilityNodeInfo}.
     * The search is performed in the window whose id is specified and starts from the
     * node whose accessibility id is specified.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @param accessibilityWindowId A unique window id. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ACTIVE_WINDOW_ID}
     *     to query the currently active window.
     * @param accessibilityNodeId A unique view id or virtual descendant id from
     *     where to start the search. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ROOT_NODE_ID}
     *     to start from the root.
     * @param direction The direction in which to search for focusable.
     * @return The accessibility focused {@link AccessibilityNodeInfo}.
     */
    CARAPI FocusSearch(
        /* [in] */ Int32 connectionId,
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 direction,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Performs an accessibility action on an {@link AccessibilityNodeInfo}.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @param accessibilityWindowId A unique window id. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ACTIVE_WINDOW_ID}
     *     to query the currently active window.
     * @param accessibilityNodeId A unique view id or virtual descendant id from
     *     where to start the search. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ROOT_NODE_ID}
     *     to start from the root.
     * @param action The action to perform.
     * @param arguments Optional action arguments.
     * @return Whether the action was performed.
     */
    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 connectionId,
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* result);

    /**
     * Computes a point in screen coordinates where sending a down/up events would
     * perform a click on an {@link AccessibilityNodeInfo}.
     *
     * @param connectionId The id of a connection for interacting with the system.
     * @param accessibilityWindowId A unique window id. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ACTIVE_WINDOW_ID}
     *     to query the currently active window.
     * @param accessibilityNodeId A unique view id or virtual descendant id from
     *     where to start the search. Use
     *     {@link android.view.accessibility.AccessibilityNodeInfo#ROOT_NODE_ID}
     *     to start from the root.
     * @return Point the click point of null if no such point.
     */
    CARAPI ComputeClickPointInScreen(
        /* [in] */ Int32 connectionId,
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [out] */ IPoint** point);

    CARAPI ClearCache();

    CARAPI OnAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * {@inheritDoc}
     */
    CARAPI SetFindAccessibilityNodeInfoResult(
        /* [in] */ IAccessibilityNodeInfo* info,
        /* [in] */ Int32 interactionId);

    /**
     * {@inheritDoc}
     */
    CARAPI SetFindAccessibilityNodeInfosResult(
        /* [in] */ IList* infos,
        /* [in] */ Int32 interactionId);

    /**
     * {@inheritDoc}
     */
    CARAPI SetPerformAccessibilityActionResult(
        /* [in] */ Boolean succeeded,
        /* [in] */ Int32 interactionId);

    /**
     * {@inheritDoc}
     */
    CARAPI SetComputeClickPointInScreenActionResult(
        /* [in] */ IPoint* point,
        /* [in] */ Int32 interactionId);

    /**
     * Gets a cached accessibility service connection.
     *
     * @param connectionId The connection id.
     * @return The cached connection if such.
     */
    CARAPI GetConnection(
        /* [in] */ Int32 connectionId,
        /* [out] */ IIAccessibilityServiceConnection** connection);

    /**
     * Adds a cached accessibility service connection.
     *
     * @param connectionId The connection id.
     * @param connection The connection.
     */
    CARAPI AddConnection(
        /* [in] */ Int32 connectionId,
        /* [in] */ IIAccessibilityServiceConnection* connection);

    /**
     * Removes a cached accessibility service connection.
     *
     * @param connectionId The connection id.
     */
    CARAPI RemoveConnection(
        /* [in] */ Int32 connectionId);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * Gets the the result of an async request that returns an {@link AccessibilityNodeInfo}.
     *
     * @param interactionId The interaction id to match the result with the request.
     * @return The result {@link AccessibilityNodeInfo}.
     */
    CARAPI_(AutoPtr<IAccessibilityNodeInfo>) GetFindAccessibilityNodeInfoResultAndClear(
        /* [in] */ Int32 interactionId);

    /**
     * Gets the the result of an async request that returns {@link AccessibilityNodeInfo}s.
     *
     * @param interactionId The interaction id to match the result with the request.
     * @return The result {@link AccessibilityNodeInfo}s.
     */
    CARAPI_(AutoPtr<IList>) GetFindAccessibilityNodeInfosResultAndClear(
        /* [in] */ Int32 interactionId);

    /**
     * Gets the result of a request to perform an accessibility action.
     *
     * @param interactionId The interaction id to match the result with the request.
     * @return Whether the action was performed.
     */
    CARAPI_(Boolean) GetPerformAccessibilityActionResultAndClear(
        /* [in] */ Int32 interactionId);

    /**
     * Gets the result of a request to compute a point in screen for clicking on a node.
     *
     * @param interactionId The interaction id to match the result with the request.
     * @return The point or null if no such point.
     */
    CARAPI_(AutoPtr<IPoint>) GetComputeClickPointInScreenResultAndClear(
        /* [in] */ Int32 interactionId);

    /**
     * Clears the result state.
     */
    CARAPI_(void) ClearResultLocked();

    /**
     * Waits up to a given bound for a result of a request and returns it.
     *
     * @param interactionId The interaction id to match the result with the request.
     * @return Whether the result was received.
     */
    CARAPI_(Boolean) WaitForResultTimedLocked(
        /* [in] */ Int32 interactionId);

    /**
     * Finalize an {@link AccessibilityNodeInfo} before passing it to the client.
     *
     * @param info The info.
     * @param connectionId The id of the connection to the system.
     */
    CARAPI_(void) FinalizeAndCacheAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info,
        /* [in] */ Int32 connectionId);

    /**
     * Finalize {@link AccessibilityNodeInfo}s before passing them to the client.
     *
     * @param infos The {@link AccessibilityNodeInfo}s.
     * @param connectionId The id of the connection to the system.
     */
    CARAPI_(void) FinalizeAndCacheAccessibilityNodeInfos(
        /* [in] */ IList* infos,
        /* [in] */ Int32 connectionId);

    /**
     * Gets the message stored if the interacted and interacting
     * threads are the same.
     *
     * @return The message.
     */
    CARAPI_(AutoPtr<IMessage>) GetSameProcessMessageAndClear();

    /**
     * Checks whether the infos are a fully connected tree with no duplicates.
     *
     * @param infos The result list to check.
     */
    CARAPI_(void) CheckFindAccessibilityNodeInfoResultIntegrity(
        /* [in] */ IList* infos);

private:
    static const String TAG;

    static const Boolean DEBUG;

    static const Boolean CHECK_INTEGRITY;

    static const Int64 TIMEOUT_INTERACTION_MILLIS;

    static Object sStaticLock;

    static const AutoPtr<IInt64SparseArray> sClients;

    AutoPtr<IAtomicInteger32> mInteractionIdCounter;

    Object mInstanceLock;

    Int32 mInteractionId;

    AutoPtr<IAccessibilityNodeInfo> mFindAccessibilityNodeInfoResult;

    AutoPtr<IList> mFindAccessibilityNodeInfosResult;

    Boolean mPerformAccessibilityActionResult;

    AutoPtr<IPoint> mComputeClickPointResult;

    AutoPtr<IMessage> mSameThreadMessage;

    static const AutoPtr<ISparseArray> sConnectionCache;

    static const AutoPtr<IAccessibilityCache> sAccessibilityCache;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYINTERACTIONCLIENT_H__
