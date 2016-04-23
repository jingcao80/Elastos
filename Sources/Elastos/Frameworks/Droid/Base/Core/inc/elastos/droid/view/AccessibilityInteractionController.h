#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITYINTERACTIONCONTROLLER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITYINTERACTIONCONTROLLER_H__

#include "elastos/droid/os/Handler.h"
#include "elastos/droid/view/ViewRootImpl.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Internal::Utility::IPredicate;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;
using Elastos::Droid::View::Accessibility::IIAccessibilityInteractionConnectionCallback;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {

class ViewRootImpl;

/**
 * This class is an interface this ViewAncestor provides to the
 * AccessibilityManagerService to the latter can interact with
 * the view hierarchy in this ViewAncestor.
 */
class AccessibilityInteractionController
    : public Object
    , public IAccessibilityInteractionController
{
private:
    /**
     * This class encapsulates a prefetching strategy for the accessibility APIs for
     * querying window content. It is responsible to prefetch a batch of
     * AccessibilityNodeInfos in addition to the one for a requested node.
     */
    class AccessibilityNodePrefetcher
        : public Object
    {
    public:
        AccessibilityNodePrefetcher(
            /* [in] */ AccessibilityInteractionController* host);

        CARAPI_(void) PrefetchAccessibilityNodeInfos(
            /* [in] */ IView* view,
            /* [in] */ Int32 virtualViewId,
            /* [in] */ Int32 fetchFlags,
            /* [in] */ IList* outInfos);

    private:
        CARAPI EnforceNodeTreeConsistent(
            /* [in] */ IList* nodes);

        CARAPI_(void) PrefetchPredecessorsOfRealNode(
            /* [in] */ IView* view,
            /* [in] */ IList* outInfos);

        CARAPI_(void) PrefetchSiblingsOfRealNode(
            /* [in] */ IView* current,
            /* [in] */ IList* outInfos);

        CARAPI_(void) PrefetchDescendantsOfRealNode(
            /* [in] */ IView* root,
            /* [in] */ IList* outInfos);

        CARAPI_(void) PrefetchPredecessorsOfVirtualNode(
            /* [in] */ IAccessibilityNodeInfo* root,
            /* [in] */ IView* providerHost,
            /* [in] */ IAccessibilityNodeProvider* provider,
            /* [in] */ IList* outInfos);

        CARAPI_(void) PrefetchSiblingsOfVirtualNode(
            /* [in] */ IAccessibilityNodeInfo* current,
            /* [in] */ IView* providerHost,
            /* [in] */ IAccessibilityNodeProvider* provider,
            /* [in] */ IList* outInfos);

        CARAPI_(void) PrefetchDescendantsOfVirtualNode(
            /* [in] */ IAccessibilityNodeInfo* root,
            /* [in] */ IAccessibilityNodeProvider* provider,
            /* [in] */ IList* outInfos);

    private:
        static const Int32 MAX_ACCESSIBILITY_NODE_INFO_BATCH_SIZE;
        AutoPtr<IArrayList> mTempViewList;
        AccessibilityInteractionController* mHost;
    };

    class PrivateHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AccessibilityInteractionController::PrivateHandler")

        PrivateHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ AccessibilityInteractionController* host);

        String GetMessageNameImpl(
            /* [in] */ IMessage* message);

        CARAPI HandleMessage(
            /* [in] */ IMessage* message);

    public:
        static const Int32 MSG_PERFORM_ACCESSIBILITY_ACTION;
        static const Int32 MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_ACCESSIBILITY_ID;
        static const Int32 MSG_FIND_ACCESSIBILITY_NODE_INFOS_BY_VIEW_ID;
        static const Int32 MSG_FIND_ACCESSIBILITY_NODE_INFO_BY_TEXT;
        static const Int32 MSG_FIND_FOCUS;
        static const Int32 MSG_FOCUS_SEARCH;
        static const Int32 MSG_COMPUTE_CLICK_POINT_IN_SCREEN;

    private:
        AccessibilityInteractionController* mHost;
    };

    class AddNodeInfosForViewId
        : public Object
        , public IPredicate
    {
    public:
        CAR_INTERFACE_DECL();

        AddNodeInfosForViewId(
            /* [in] */ AccessibilityInteractionController* host);

        ~AddNodeInfosForViewId();

        CARAPI_(void) Init(
            /* [in] */ Int32 viewId,
            /* [in] */ IList* infos);

        CARAPI_(void) Reset();

        //@Overrie
        CARAPI Apply(
            /* [in] */ IInterface* t,
            /* [out] */ Boolean* result);

    private:
        AccessibilityInteractionController* mHost;
        Int32 mViewId;
        AutoPtr<IList> mInfos;
    };

public:
    CAR_INTERFACE_DECL();

    AccessibilityInteractionController();

    ~AccessibilityInteractionController();

    CARAPI constructor(
        /* [in] */ IViewRootImpl* viewRootImpl);

    CARAPI FindAccessibilityNodeInfoByAccessibilityIdClientThread(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ IRegion* interactiveRegion,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI FindAccessibilityNodeInfosByViewIdClientThread(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ const String& viewId,
        /* [in] */ IRegion* interactiveRegion,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI FindAccessibilityNodeInfosByTextClientThread(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ const String& text,
        /* [in] */ IRegion* interactiveRegion,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI FindFocusClientThread(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 focusType,
        /* [in] */ IRegion* interactiveRegion,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI FocusSearchClientThread(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 direction,
        /* [in] */ IRegion* interactiveRegion,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI PerformAccessibilityActionClientThread(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interogatingPid,
        /* [in] */ Int64 interrogatingTid);

    CARAPI ComputeClickPointInScreenClientThread(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ IRegion* interactiveRegion,
        /* [in] */ Int32 interactionId,
        /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid,
        /* [in] */ IMagnificationSpec* spec);

private:
    CARAPI_(Boolean) IsShown(
        /* [in] */ IView* view);

    CARAPI_(void) FindAccessibilityNodeInfoByAccessibilityIdUiThread(
        /* [in] */ IMessage* message);

    CARAPI_(void) FindAccessibilityNodeInfosByViewIdUiThread(
        /* [in] */ IMessage* message);

    CARAPI_(void) FindAccessibilityNodeInfosByTextUiThread(
        /* [in] */ IMessage* message);

    CARAPI_(void) FindFocusUiThread(
        /* [in] */ IMessage* message);

    CARAPI_(void) FocusSearchUiThread(
        /* [in] */ IMessage* message);

    CARAPI_(void) PerfromAccessibilityActionUiThread(
        /* [in] */ IMessage* message);

    CARAPI_(void) ComputeClickPointInScreenUiThread(
        /* [in] */ IMessage* message);

    AutoPtr<IView> FindViewByAccessibilityId(
        /* [in] */ Int32 accessibilityId);

    CARAPI_(void) ApplyAppScaleAndMagnificationSpecIfNeeded(
        /* [in] */ IList* infos,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI_(void) AdjustIsVisibleToUserIfNeeded(
        /* [in] */ IList* infos,
        /* [in] */ IRegion* interactiveRegion);

    CARAPI_(void) AdjustIsVisibleToUserIfNeeded(
        /* [in] */ IAccessibilityNodeInfo* info,
        /* [in] */ IRegion* interactiveRegion);

    CARAPI_(void) ApplyAppScaleAndMagnificationSpecIfNeeded(
        /* [in] */ IPoint* point,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI_(void) ApplyAppScaleAndMagnificationSpecIfNeeded(
        /* [in] */ IAccessibilityNodeInfo* info,
        /* [in] */ IMagnificationSpec* spec);

    CARAPI_(Boolean) ShouldApplyAppScaleAndMagnificationSpec(
        /* [in] */ Float appScale,
        /* [in] */ IMagnificationSpec* spec);

private:
    static const Boolean ENFORCE_NODE_TREE_CONSISTENT;

    AutoPtr<IArrayList> mTempAccessibilityNodeInfoList;

    AutoPtr<PrivateHandler> mHandler;

    ViewRootImpl* mViewRootImpl;

    AutoPtr<AccessibilityNodePrefetcher> mPrefetcher;

    Int64 mMyLooperThreadId;

    Int32 mMyProcessId;

    AutoPtr<IArrayList> mTempArrayList;

    AutoPtr<IPoint> mTempPoint;
    AutoPtr<IRect> mTempRect;
    AutoPtr<IRect> mTempRect1;
    AutoPtr<IRect> mTempRect2;

    AutoPtr<AddNodeInfosForViewId> mAddNodeInfosForViewId;
};

} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITYINTERACTIONCONTROLLER_H__
