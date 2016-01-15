#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEPROVIDER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEPROVIDER_H__

#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

class AccessibilityNodeProvider
    : public Object
    , public IAccessibilityNodeProvider
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns an {@link AccessibilityNodeInfo} representing a virtual view,
     * i.e. a descendant of the host View, with the given <code>virtualViewId</code>
     * or the host View itself if <code>virtualViewId</code> equals to {@link View#NO_ID}.
     * <p>
     * A virtual descendant is an imaginary View that is reported as a part of the view
     * hierarchy for accessibility purposes. This enables custom views that draw complex
     * content to report them selves as a tree of virtual views, thus conveying their
     * logical structure.
     * </p>
     * <p>
     * The implementer is responsible for obtaining an accessibility node info from the
     * pool of reusable instances and setting the desired properties of the node info
     * before returning it.
     * </p>
     *
     * @param virtualViewId A client defined virtual view id.
     * @return A populated {@link AccessibilityNodeInfo} for a virtual descendant or the
     *     host View.
     *
     * @see View#createAccessibilityNodeInfo()
     * @see AccessibilityNodeInfo
     */
    CARAPI CreateAccessibilityNodeInfo(
        /* [in] */ Int32 virtualViewId,
        /* [out] */ IAccessibilityNodeInfo** nodeInfo);

    /**
     * Performs an accessibility action on a virtual view, i.e. a descendant of the
     * host View, with the given <code>virtualViewId</code> or the host View itself
     * if <code>virtualViewId</code> equals to {@link View#NO_ID}.
     *
     * @param virtualViewId A client defined virtual view id.
     * @param action The action to perform.
     * @param arguments Optional action arguments.
     * @return True if the action was performed.
     *
     * @see View#performAccessibilityAction(int, Bundle)
     * @see #createAccessibilityNodeInfo(int)
     * @see AccessibilityNodeInfo
     */
    CARAPI PerformAction(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* result);

    /**
     * Finds {@link AccessibilityNodeInfo}s by text. The match is case insensitive
     * containment. The search is relative to the virtual view, i.e. a descendant of the
     * host View, with the given <code>virtualViewId</code> or the host View itself
     * <code>virtualViewId</code> equals to {@link View#NO_ID}.
     *
     * @param virtualViewId A client defined virtual view id which defined
     *     the root of the tree in which to perform the search.
     * @param text The searched text.
     * @return A list of node info.
     *
     * @see #createAccessibilityNodeInfo(int)
     * @see AccessibilityNodeInfo
     */
    CARAPI FindAccessibilityNodeInfosByText(
        /* [in] */ const String& text,
        /* [in] */ Int32 virtualViewId,
        /* [out] */ IList** nodeInfos);

    /**
     * Find the virtual view, i.e. a descendant of the host View, that has the
     * specified focus type.
     *
     * @param focus The focus to find. One of
     *            {@link AccessibilityNodeInfo#FOCUS_INPUT} or
     *            {@link AccessibilityNodeInfo#FOCUS_ACCESSIBILITY}.
     * @return The node info of the focused view or null.
     * @see AccessibilityNodeInfo#FOCUS_INPUT
     * @see AccessibilityNodeInfo#FOCUS_ACCESSIBILITY
     */
    CARAPI FindFocus(
        /* [in] */ Int32 focus,
        /* [out] */  IAccessibilityNodeInfo** info);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_ACCESSIBILITYNODEPROVIDER_H__
