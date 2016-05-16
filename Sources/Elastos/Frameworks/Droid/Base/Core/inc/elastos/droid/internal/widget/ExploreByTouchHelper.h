
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_EXPLOREBYTOUCHHELPER_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_EXPLOREBYTOUCHHELPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/view/accessibility/AccessibilityNodeProvider.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::Accessibility::AccessibilityNodeProvider;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::IAccessibilityDelegate;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * ExploreByTouchHelper is a utility class for implementing accessibility
 * support in custom {@link android.view.View}s that represent a collection of View-like
 * logical items. It extends {@link android.view.accessibility.AccessibilityNodeProvider} and
 * simplifies many aspects of providing information to accessibility services
 * and managing accessibility focus. This class does not currently support
 * hierarchies of logical items.
 * <p>
 * This should be applied to the parent view using
 * {@link android.view.View#setAccessibilityDelegate}:
 *
 * <pre>
 * mAccessHelper = ExploreByTouchHelper.create(someView, mAccessHelperCallback);
 * ViewCompat.setAccessibilityDelegate(someView, mAccessHelper);
 * </pre>
 */
class ExploreByTouchHelper
    : public Elastos::Droid::View::View::AccessibilityDelegate
    , public IExploreByTouchHelper
{
private:
    /**
     * Exposes a virtual view hierarchy to the accessibility framework. Only
     * used in API 16+.
     */
    class ExploreByTouchNodeProvider : public AccessibilityNodeProvider
    {
    public:
        ExploreByTouchNodeProvider(
            /* [in] */ ExploreByTouchHelper* host);

        // @Override
        CARAPI CreateAccessibilityNodeInfo(
            /* [in] */ Int32 virtualViewId,
            /* [out] */ IAccessibilityNodeInfo** info);

        // @Override
        CARAPI PerformAction(
            /* [in] */ Int32 virtualViewId,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* arguments,
            /* [out] */ Boolean* result);

    private:
        ExploreByTouchHelper* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    ExploreByTouchHelper();

    /**
     * Factory method to create a new {@link ExploreByTouchHelper}.
     *
     * @param forView View whose logical children are exposed by this helper.
     */
    CARAPI constructor(
        /* [in] */ IView* forView);

    /**
     * Returns the {@link android.view.accessibility.AccessibilityNodeProvider} for this helper.
     *
     * @param host View whose logical children are exposed by this helper.
     * @return The accessibility node provider for this helper.
     */
    // @Override
    CARAPI GetAccessibilityNodeProvider(
        /* [in] */ IView* host,
        /* [out] */ IAccessibilityNodeProvider** provider);

    /**
     * Dispatches hover {@link android.view.MotionEvent}s to the virtual view hierarchy when
     * the Explore by Touch feature is enabled.
     * <p>
     * This method should be called by overriding
     * {@link View#dispatchHoverEvent}:
     *
     * <pre>&#64;Override
     * public Boolean dispatchHoverEvent(MotionEvent event) {
     *   if (mHelper.dispatchHoverEvent(this, event) {
     *     return TRUE;
     *   }
     *   return super.dispatchHoverEvent(event);
     * }
     * </pre>
     *
     * @param event The hover event to dispatch to the virtual view hierarchy.
     * @return Whether the hover event was handled.
     */
    CARAPI DispatchHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Populates an event of the specified type with information about an item
     * and attempts to send it up through the view hierarchy.
     * <p>
     * You should call this method after performing a user action that normally
     * fires an accessibility event, such as clicking on an item.
     *
     * <pre>public void performItemClick(T item) {
     *   ...
     *   sendEventForVirtualViewId(item.id, AccessibilityEvent.TYPE_VIEW_CLICKED);
     * }
     * </pre>
     *
     * @param virtualViewId The virtual view id for which to send an event.
     * @param eventType The type of event to send.
     * @return TRUE if the event was sent successfully.
     */
    CARAPI SendEventForVirtualView(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 eventType,
        /* [out] */ Boolean* result);

    /**
     * Notifies the accessibility framework that the properties of the parent
     * view have changed.
     * <p>
     * You <b>must</b> call this method after adding or removing items from the
     * parent view.
     */
    CARAPI InvalidateRoot();

    /**
     * Notifies the accessibility framework that the properties of a particular
     * item have changed.
     * <p>
     * You <b>must</b> call this method after changing any of the properties set
     * in {@link #onPopulateNodeForVirtualView}.
     *
     * @param virtualViewId The virtual view id to invalidate.
     */
    CARAPI InvalidateVirtualView(
        /* [in] */ Int32 virtualViewId);

    /**
     * Returns the virtual view id for the currently focused item,
     *
     * @return A virtual view id, or {@link #INVALID_ID} if no item is
     *         currently focused.
     */
    CARAPI GetFocusedVirtualView(
        /* [out] */ Int32* result);

protected:
    /**
     * Provides a mapping between view-relative coordinates and logical
     * items.
     *
     * @param x The view-relative x coordinate
     * @param y The view-relative y coordinate
     * @return virtual view identifier for the logical item under
     *         coordinates (x,y)
     */
    virtual CARAPI_(Int32) GetVirtualViewAt(
        /* [in] */ Float x,
        /* [in] */ Float y) = 0;

    /**
     * Populates a list with the view's visible items. The ordering of items
     * within {@code virtualViewIds} specifies order of accessibility focus
     * traversal.
     *
     * @param virtualViewIds The list to populate with visible items
     */
    virtual CARAPI_(void) GetVisibleVirtualViews(
        /* [in] */ IList/*<Integer>*/* virtualViewIds) = 0;

    /**
     * Populates an {@link AccessibilityEvent} with information about the
     * specified item.
     * <p>
     * Implementations <b>must</b> populate the following required fields:
     * <ul>
     * <li>event text, see {@link AccessibilityEvent#getText} or
     * {@link AccessibilityEvent#setContentDescription}
     * </ul>
     * <p>
     * The helper class automatically populates the following fields with
     * default values, but implementations may optionally override them:
     * <ul>
     * <li>item class name, set to android.view.View, see
     * {@link AccessibilityEvent#setClassName}
     * </ul>
     * <p>
     * The following required fields are automatically populated by the
     * helper class and may not be overridden:
     * <ul>
     * <li>package name, set to the package of the host view's
     * {@link Context}, see {@link AccessibilityEvent#setPackageName}
     * <li>event source, set to the host view and virtual view identifier,
     * see {@link AccessibilityRecord#setSource(View, Int32)}
     * </ul>
     *
     * @param virtualViewId The virtual view id for the item for which to
     *            populate the event
     * @param event The event to populate
     */
    virtual CARAPI_(void) OnPopulateEventForVirtualView(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ IAccessibilityEvent* event) = 0;

    /**
     * Populates an {@link AccessibilityNodeInfo} with information
     * about the specified item.
     * <p>
     * Implementations <b>must</b> populate the following required fields:
     * <ul>
     * <li>event text, see {@link AccessibilityNodeInfo#setText} or
     * {@link AccessibilityNodeInfo#setContentDescription}
     * <li>bounds in parent coordinates, see
     * {@link AccessibilityNodeInfo#setBoundsInParent}
     * </ul>
     * <p>
     * The helper class automatically populates the following fields with
     * default values, but implementations may optionally override them:
     * <ul>
     * <li>enabled state, set to TRUE, see
     * {@link AccessibilityNodeInfo#setEnabled}
     * <li>item class name, identical to the class name set by
     * {@link #onPopulateEventForVirtualView}, see
     * {@link AccessibilityNodeInfo#setClassName}
     * </ul>
     * <p>
     * The following required fields are automatically populated by the
     * helper class and may not be overridden:
     * <ul>
     * <li>package name, identical to the package name set by
     * {@link #onPopulateEventForVirtualView}, see
     * {@link AccessibilityNodeInfo#setPackageName}
     * <li>node source, identical to the event source set in
     * {@link #onPopulateEventForVirtualView}, see
     * {@link AccessibilityNodeInfo#setSource(View, Int32)}
     * <li>parent view, set to the host view, see
     * {@link AccessibilityNodeInfo#setParent(View)}
     * <li>visibility, computed based on parent-relative bounds, see
     * {@link AccessibilityNodeInfo#setVisibleToUser}
     * <li>accessibility focus, computed based on internal helper state, see
     * {@link AccessibilityNodeInfo#setAccessibilityFocused}
     * <li>bounds in screen coordinates, computed based on host view bounds,
     * see {@link AccessibilityNodeInfo#setBoundsInScreen}
     * </ul>
     * <p>
     * Additionally, the helper class automatically handles accessibility
     * focus management by adding the appropriate
     * {@link AccessibilityNodeInfo#ACTION_ACCESSIBILITY_FOCUS} or
     * {@link AccessibilityNodeInfo#ACTION_CLEAR_ACCESSIBILITY_FOCUS}
     * action. Implementations must <b>never</b> manually add these actions.
     * <p>
     * The helper class also automatically modifies parent- and
     * screen-relative bounds to reflect the portion of the item visible
     * within its parent.
     *
     * @param virtualViewId The virtual view identifier of the item for
     *            which to populate the node
     * @param node The node to populate
     */
    virtual CARAPI_(void) OnPopulateNodeForVirtualView(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ IAccessibilityNodeInfo* node) = 0;

    /**
     * Performs the specified accessibility action on the item associated
     * with the virtual view identifier. See
     * {@link AccessibilityNodeInfo#performAction(Int32, Bundle)} for
     * more information.
     * <p>
     * Implementations <b>must</b> handle any actions added manually in
     * {@link #onPopulateNodeForVirtualView}.
     * <p>
     * The helper class automatically handles focus management resulting
     * from {@link AccessibilityNodeInfo#ACTION_ACCESSIBILITY_FOCUS}
     * and
     * {@link AccessibilityNodeInfo#ACTION_CLEAR_ACCESSIBILITY_FOCUS}
     * actions.
     *
     * @param virtualViewId The virtual view identifier of the item on which
     *            to perform the action
     * @param action The accessibility action to perform
     * @param arguments (Optional) A bundle with additional arguments, or
     *            null
     * @return TRUE if the action was performed
     */
    virtual CARAPI_(Boolean) OnPerformActionForVirtualView(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments) = 0;

private:
    /**
     * Sets the currently hovered item, sending hover accessibility events as
     * necessary to maintain the correct state.
     *
     * @param virtualViewId The virtual view id for the item currently being
     *            hovered, or {@link #INVALID_ID} if no item is hovered within
     *            the parent view.
     */
    CARAPI_(void) UpdateHoveredVirtualView(
        /* [in] */ Int32 virtualViewId);

    /**
     * Constructs and returns an {@link AccessibilityEvent} for the specified
     * virtual view id, which includes the host view ({@link View#NO_ID}).
     *
     * @param virtualViewId The virtual view id for the item for which to
     *            construct an event.
     * @param eventType The type of event to construct.
     * @return An {@link AccessibilityEvent} populated with information about
     *         the specified item.
     */
    CARAPI CreateEvent(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 eventType,
        /* [out] */ IAccessibilityEvent** event);

    /**
     * Constructs and returns an {@link AccessibilityEvent} for the host node.
     *
     * @param eventType The type of event to construct.
     * @return An {@link AccessibilityEvent} populated with information about
     *         the specified item.
     */
    CARAPI_(AutoPtr<IAccessibilityEvent>) CreateEventForHost(
        /* [in] */ Int32 eventType);

    /**
     * Constructs and returns an {@link AccessibilityEvent} populated with
     * information about the specified item.
     *
     * @param virtualViewId The virtual view id for the item for which to
     *            construct an event.
     * @param eventType The type of event to construct.
     * @return An {@link AccessibilityEvent} populated with information about
     *         the specified item.
     */
    CARAPI CreateEventForChild(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 eventType,
        /* [out] */ IAccessibilityEvent** result);

    /**
     * Constructs and returns an {@link android.view.accessibility.AccessibilityNodeInfo} for the
     * specified virtual view id, which includes the host view
     * ({@link View#NO_ID}).
     *
     * @param virtualViewId The virtual view id for the item for which to
     *            construct a node.
     * @return An {@link android.view.accessibility.AccessibilityNodeInfo} populated with information
     *         about the specified item.
     */
    CARAPI CreateNode(
        /* [in] */ Int32 virtualViewId,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Constructs and returns an {@link AccessibilityNodeInfo} for the
     * host view populated with its virtual descendants.
     *
     * @return An {@link AccessibilityNodeInfo} for the parent node.
     */
    CARAPI_(AutoPtr<IAccessibilityNodeInfo>) CreateNodeForHost();

    /**
     * Constructs and returns an {@link AccessibilityNodeInfo} for the
     * specified item. Automatically manages accessibility focus actions.
     * <p>
     * Allows the implementing class to specify most node properties, but
     * overrides the following:
     * <ul>
     * <li>{@link AccessibilityNodeInfo#setPackageName}
     * <li>{@link AccessibilityNodeInfo#setClassName}
     * <li>{@link AccessibilityNodeInfo#setParent(View)}
     * <li>{@link AccessibilityNodeInfo#setSource(View, Int32)}
     * <li>{@link AccessibilityNodeInfo#setVisibleToUser}
     * <li>{@link AccessibilityNodeInfo#setBoundsInScreen(Rect)}
     * </ul>
     * <p>
     * Uses the bounds of the parent view and the parent-relative bounding
     * rectangle specified by
     * {@link AccessibilityNodeInfo#getBoundsInParent} to automatically
     * update the following properties:
     * <ul>
     * <li>{@link AccessibilityNodeInfo#setVisibleToUser}
     * <li>{@link AccessibilityNodeInfo#setBoundsInParent}
     * </ul>
     *
     * @param virtualViewId The virtual view id for item for which to construct
     *            a node.
     * @return An {@link AccessibilityNodeInfo} for the specified item.
     */
    CARAPI CreateNodeForChild(
        /* [in] */ Int32 virtualViewId,
        /* [out] */ IAccessibilityNodeInfo** info);

    CARAPI_(Boolean) PerformAction(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

    CARAPI_(Boolean) PerformActionForHost(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

    CARAPI_(Boolean) PerformActionForChild(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

    CARAPI_(Boolean) ManageFocusForChild(
        /* [in] */ Int32 virtualViewId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

    /**
     * Computes whether the specified {@link Rect} intersects with the visible
     * portion of its parent {@link View}. Modifies {@code localRect} to contain
     * only the visible portion.
     *
     * @param localRect A rectangle in local (parent) coordinates.
     * @return Whether the specified {@link Rect} is visible on the screen.
     */
    CARAPI_(Boolean) IntersectVisibleToUser(
        /* [in] */ IRect* localRect);

    /**
     * Returns whether this virtual view is accessibility focused.
     *
     * @return True if the view is accessibility focused.
     */
    CARAPI_(Boolean) IsAccessibilityFocused(
        /* [in] */ Int32 virtualViewId);

    /**
     * Attempts to give accessibility focus to a virtual view.
     * <p>
     * A virtual view will not actually take focus if
     * {@link AccessibilityManager#isEnabled()} returns FALSE,
     * {@link AccessibilityManager#isTouchExplorationEnabled()} returns FALSE,
     * or the view already has accessibility focus.
     *
     * @param virtualViewId The id of the virtual view on which to place
     *            accessibility focus.
     * @return Whether this virtual view actually took accessibility focus.
     */
    CARAPI_(Boolean) RequestAccessibilityFocus(
        /* [in] */ Int32 virtualViewId);

    /**
     * Attempts to clear accessibility focus from a virtual view.
     *
     * @param virtualViewId The id of the virtual view from which to clear
     *            accessibility focus.
     * @return Whether this virtual view actually cleared accessibility focus.
     */
    CARAPI_(Boolean) ClearAccessibilityFocus(
        /* [in] */ Int32 virtualViewId);

private:
    /** Default class name used for virtual views. */
    static const Int32 INVALID_ID;
    static const String DEFAULT_CLASS_NAME;
    static const String TAG;

    // Temporary, reusable data structures.
    AutoPtr<IRect> mTempScreenRect;
    AutoPtr<IRect> mTempParentRect;
    AutoPtr<IRect> mTempVisibleRect;
    AutoPtr<ArrayOf<Int32> > mTempGlobalRect;

    /** View's context **/
    AutoPtr<IContext> mContext;

    /** System accessibility manager, used to check state and send events. */
    AutoPtr<IAccessibilityManager> mManager;

    /** View whose internal structure is exposed through this helper. */
    AutoPtr<IView> mView;

    /** Node provider that handles creating nodes and performing actions. */
    AutoPtr<ExploreByTouchNodeProvider> mNodeProvider;

    /** Virtual view id for the currently focused logical item. */
    Int32 mFocusedVirtualViewId;

    /** Virtual view id for the currently hovered logical item. */
    Int32 mHoveredVirtualViewId;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_INTERNAL_EXPLOREBYTOUCHHELPER_H__
