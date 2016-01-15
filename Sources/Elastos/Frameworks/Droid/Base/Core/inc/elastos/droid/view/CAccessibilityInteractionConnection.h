#ifndef __ELASTOS_DROID_VIEW_CACCESSIBILITYINTERACTIONCONNECTION_H__
#define __ELASTOS_DROID_VIEW_CACCESSIBILITYINTERACTIONCONNECTION_H__

#include "_Elastos_Droid_View_CAccessibilityInteractionConnection.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionConnection;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionConnectionCallback;

namespace Elastos {
namespace Droid {
namespace View {

class ViewRootImpl;

/**
 * This class is an interface this ViewAncestor provides to the
 * AccessibilityManagerService to the latter can interact with
 * the view hierarchy in this ViewAncestor.
 */
CarClass(CAccessibilityInteractionConnection)
{
public:
    CARAPI constructor(
        /* [in] */ Handle32 viewRootImpl);

    CARAPI FindAccessibilityNodeInfoByAccessibilityId(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid);

    CARAPI PerformAccessibilityAction(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interogatingPid,
        /* [in] */ Int64 interrogatingTid);

    CARAPI FindAccessibilityNodeInfoByViewId(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 viewId,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid);

    CARAPI FindAccessibilityNodeInfosByText(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ const String& text,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid);

    CARAPI FindFocus(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 focusType,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid);

    CARAPI FocusSearch(
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 interrogatingPid,
        /* [in] */ Int64 interrogatingTid);

private:
    ViewRootImpl* mViewRootImpl;
};

} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_CACCESSIBILITYINTERACTIONCONNECTION_H__
