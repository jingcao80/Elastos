#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYCACHE_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYCACHE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_Accessibility_CAccessibilityCache.h"
#include "elastos/droid/utility/Pools.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/utility/CInt64SparseArray.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::Pools;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityCache)
    , public Object
    , public IAccessibilityCache
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityCache();

    ~CAccessibilityCache();

    CARAPI constructor();

    CARAPI AddWindow(
        /* [in] */ IAccessibilityWindowInfo* window);

    /**
     * Notifies the cache that the something in the UI changed. As a result
     * the cache will either refresh some nodes or evict some nodes.
     *
     * @param event An event.
     */
    CARAPI OnAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * Gets a cached {@link AccessibilityNodeInfo} given the id of the hosting
     * window and the accessibility id of the node.
     *
     * @param windowId The id of the window hosting the node.
     * @param accessibilityNodeId The info accessibility node id.
     * @return The cached {@link AccessibilityNodeInfo} or null if such not found.
     */
    CARAPI GetNode(
        /* [in] */ Int32 windowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [out] */ IAccessibilityNodeInfo** info);

    CARAPI GetWindows(
        /* [out] */ IList** list);

    CARAPI GetWindow(
        /* [in] */ Int32 windowId,
        /* [out] */ IAccessibilityWindowInfo** info);

    /**
     * Caches an {@link AccessibilityNodeInfo}.
     *
     * @param info The node to cache.
     */
    CARAPI Add(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * Clears the cache.
     */
    CARAPI Clear();

    /**
     * Check the integrity of the cache which is nodes from different windows
     * are not mixed, there is a single active window, there is a single focused
     * window, for every window there are no duplicates nodes, all nodes for a
     * window are connected, for every window there is a single input focused
     * node, and for every window there is a single accessibility focused node.
     */
    CARAPI CheckIntegrity();

private:
    CARAPI_(void) RefreshCachedNodeLocked(
        /* [in] */ Int32 windowId,
        /* [in] */ Int64 sourceId);

    CARAPI_(void) ClearNodesForWindowLocked(
        /* [in] */ Int32 windowId);

    /**
     * Clears a subtree rooted at the node with the given id that is
     * hosted in a given window.
     *
     * @param windowId The id of the hosting window.
     * @param rootNodeId The root id.
     */
    CARAPI_(void) ClearSubTreeLocked(
        /* [in] */ Int32 windowId,
        /* [in] */ Int64 rootNodeId);

    /**
     * Clears a subtree given a pointer to the root id and the nodes
     * in the hosting window.
     *
     * @param nodes The nodes in the hosting window.
     * @param rootNodeId The id of the root to evict.
     */
    CARAPI_(void) ClearSubTreeRecursiveLocked(
        /* [in] */ IInt64SparseArray* nodes,
        /* [in] */ Int64 rootNodeId);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean CHECK_INTEGRITY;
    Object mLock;
    AutoPtr<ISparseArray> mWindowCache;
    AutoPtr<ISparseArray> mNodeCache;
    AutoPtr<ISparseArray> mTempWindowArray;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYCACHE_H__
