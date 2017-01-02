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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFO_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFO_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_Accessibility_CAccessibilityNodeInfo.h"
#include "elastos/droid/utility/CInt64Array.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::CInt64Array;
using Elastos::Droid::Utility::IInt64Array;
using Elastos::Droid::Utility::Pools;
using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityNodeInfo)
    , public Object
    , public IAccessibilityNodeInfo
    , public IParcelable
{
    friend class CAccessibilityNodeInfoAccessibilityAction;

public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityNodeInfo();

    ~CAccessibilityNodeInfo();

    CARAPI constructor();

    /**
     * Gets the accessibility view id which identifies a View in the view three.
     *
     * @param accessibilityNodeId The id of an {@link AccessibilityNodeInfo}.
     * @return The accessibility view id part of the node id.
     *
     * @hide
     */
    static CARAPI_(Int32) GetAccessibilityViewId(
        /* [in] */ Int64 accessibilityNodeId);

    /**
     * Gets the virtual descendant id which identifies an imaginary view in a
     * containing View.
     *
     * @param accessibilityNodeId The id of an {@link AccessibilityNodeInfo}.
     * @return The virtual view id part of the node id.
     *
     * @hide
     */
    static CARAPI_(Int32) GetVirtualDescendantId(
        /* [in] */ Int64 accessibilityNodeId);

    /**
     * Makes a node id by shifting the <code>virtualDescendantId</code>
     * by {@link #VIRTUAL_DESCENDANT_ID_SHIFT} and taking
     * the bitwise or with the <code>accessibilityViewId</code>.
     *
     * @param accessibilityViewId A View accessibility id.
     * @param virtualDescendantId A virtual descendant id.
     * @return The node id.
     *
     * @hide
     */
    static CARAPI_(Int64) MakeNodeId(
        /* [in] */ Int32 accessibilityViewId,
        /* [in] */ Int32 virtualDescendantId);

    /**
     * Sets the source.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param source The info source.
     */
    CARAPI SetSource(
        /* [in] */ IView* source);

    /**
     * Sets the source to be a virtual descendant of the given <code>root</code>.
     * If <code>virtualDescendantId</code> is {@link View#NO_ID} the root
     * is set as the source.
     * <p>
     * A virtual descendant is an imaginary View that is reported as a part of the view
     * hierarchy for accessibility purposes. This enables custom views that draw complex
     * content to report themselves as a tree of virtual views, thus conveying their
     * logical structure.
     * </p>
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param root The root of the virtual subtree.
     * @param virtualDescendantId The id of the virtual descendant.
     */
    CARAPI SetSource(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId);

    /**
     * Find the view that has the specified focus type. The search starts from
     * the view represented by this node info.
     *
     * @param focus The focus to find. One of {@link #FOCUS_INPUT} or
     *         {@link #FOCUS_ACCESSIBILITY}.
     * @return The node info of the focused view or null.
     *
     * @see #FOCUS_INPUT
     * @see #FOCUS_ACCESSIBILITY
     */
    CARAPI FindFocus(
        /* [in] */ Int32 focus,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Searches for the nearest view in the specified direction that can take
     * the input focus.
     *
     * @param direction The direction. Can be one of:
     *     {@link View#FOCUS_DOWN},
     *     {@link View#FOCUS_UP},
     *     {@link View#FOCUS_LEFT},
     *     {@link View#FOCUS_RIGHT},
     *     {@link View#FOCUS_FORWARD},
     *     {@link View#FOCUS_BACKWARD}.
     *
     * @return The node info for the view that can take accessibility focus.
     */
    CARAPI FocusSearch(
        /* [in] */ Int32 direction,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Gets the id of the window from which the info comes from.
     *
     * @return The window id.
     */
    CARAPI GetWindowId(
        /* [out] */ Int32* id);

    /**
     * Refreshes this info with the latest state of the view it represents.
     * <p>
     * <strong>Note:</strong> If this method returns false this info is obsolete
     * since it represents a view that is no longer in the view tree and should
     * be recycled.
     * </p>
     *
     * @param bypassCache Whether to bypass the cache.
     * @return Whether the refresh succeeded.
     *
     * @hide
     */
    CARAPI Refresh(
        /* [in] */ Boolean bypassCache,
        /* [out] */ Boolean* result);

    /**
     * Refreshes this info with the latest state of the view it represents.
     * <p>
     * <strong>Note:</strong> If this method returns false this info is obsolete
     * since it represents a view that is no longer in the view tree and should
     * be recycled.
     * </p>
     * @return Whether the refresh succeeded.
     */
    CARAPI Refresh(
        /* [out] */ Boolean* result);

    /**
     * Returns the array containing the IDs of this node's children.
     *
     * @hide
     */
    CARAPI GetChildNodeIds(
        /* [out] */ IInt64Array** ids);

    /**
     * Returns the id of the child at the specified index.
     *
     * @throws IndexOutOfBoundsException when index &lt; 0 || index &gt;=
     *             getChildCount()
     * @hide
     */
    CARAPI GetChildId(
        /* [in] */ Int32 index,
        /* [out] */ Int64* id);

    /**
     * Gets the number of children.
     *
     * @return The child count.
     */
    CARAPI GetChildCount(
        /* [out] */ Int32* count);

    /**
     * Get the child at given index.
     * <p>
     *   <strong>Note:</strong> It is a client responsibility to recycle the
     *     received info by calling {@link AccessibilityNodeInfo#recycle()}
     *     to avoid creating of multiple instances.
     * </p>
     *
     * @param index The child index.
     * @return The child node.
     *
     * @throws IllegalStateException If called outside of an AccessibilityService.
     *
     */
    CARAPI GetChild(
        /* [in] */ Int32 index,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Adds a child.
     * <p>
     * <strong>Note:</strong> Cannot be called from an
     * {@link android.accessibilityservice.AccessibilityService}.
     * This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param child The child.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI AddChild(
        /* [in] */ IView* child);

     /**
     * Unchecked version of {@link #addChild(View)} that does not verify
     * uniqueness. For framework use only.
     *
     * @hide
     */
    CARAPI AddChildUnchecked(
        /* [in] */ IView* child);

    /**
     * Removes a child. If the child was not previously added to the node,
     * calling this method has no effect.
     * <p>
     * <strong>Note:</strong> Cannot be called from an
     * {@link android.accessibilityservice.AccessibilityService}.
     * This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param child The child.
     * @return true if the child was present
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI RemoveChild(
        /* [in] */ IView* child,
        /* [out] */ Boolean* result);

    /**
     * Adds a virtual child which is a descendant of the given <code>root</code>.
     * If <code>virtualDescendantId</code> is {@link View#NO_ID} the root
     * is added as a child.
     * <p>
     * A virtual descendant is an imaginary View that is reported as a part of the view
     * hierarchy for accessibility purposes. This enables custom views that draw complex
     * content to report them selves as a tree of virtual views, thus conveying their
     * logical structure.
     * </p>
     *
     * @param root The root of the virtual subtree.
     * @param virtualDescendantId The id of the virtual child.
     */
    CARAPI AddChild(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId);

    /**
     * Removes a virtual child which is a descendant of the given
     * <code>root</code>. If the child was not previously added to the node,
     * calling this method has no effect.
     *
     * @param root The root of the virtual subtree.
     * @param virtualDescendantId The id of the virtual child.
     * @return true if the child was present
     * @see #addChild(View, int)
     */
    CARAPI RemoveChild(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId,
        /* [out] */ Boolean* result);

    /**
     * Gets the actions that can be performed on the node.
     */
    CARAPI GetActionList(
        /* [out] */ IList** list);

    /**
     * Gets the actions that can be performed on the node.
     *
     * @return The bit mask of with actions.
     *
     * @see AccessibilityNodeInfo#ACTION_FOCUS
     * @see AccessibilityNodeInfo#ACTION_CLEAR_FOCUS
     * @see AccessibilityNodeInfo#ACTION_SELECT
     * @see AccessibilityNodeInfo#ACTION_CLEAR_SELECTION
     * @see AccessibilityNodeInfo#ACTION_ACCESSIBILITY_FOCUS
     * @see AccessibilityNodeInfo#ACTION_CLEAR_ACCESSIBILITY_FOCUS
     * @see AccessibilityNodeInfo#ACTION_CLICK
     * @see AccessibilityNodeInfo#ACTION_LONG_CLICK
     * @see AccessibilityNodeInfo#ACTION_NEXT_AT_MOVEMENT_GRANULARITY
     * @see AccessibilityNodeInfo#ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY
     * @see AccessibilityNodeInfo#ACTION_NEXT_HTML_ELEMENT
     * @see AccessibilityNodeInfo#ACTION_PREVIOUS_HTML_ELEMENT
     * @see AccessibilityNodeInfo#ACTION_SCROLL_FORWARD
     * @see AccessibilityNodeInfo#ACTION_SCROLL_BACKWARD
     *
     * @deprecated Use {@link #getActionList()}.
     */
    CARAPI GetActions(
        /* [out] */ Int32* actions);

    /**
     * Adds an action that can be performed on the node.
     * <p>
     * To add a standard action use the static constants on {@link AccessibilityAction}.
     * To add a custom action create a new {@link AccessibilityAction} by passing in a
     * resource id from your application as the action id and an optional label that
     * describes the action. To override one of the standard actions use as the action
     * id of a standard action id such as {@link #ACTION_CLICK} and an optional label that
     * describes the action.
     * </p>
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param action The action.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI AddAction(
        /* [in] */ IAccessibilityNodeInfoAccessibilityAction* action);

    /**
     * Adds an action that can be performed on the node.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param action The action.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     * @throws IllegalArgumentException If the argument is not one of the standard actions.
     *
     * @deprecated This has been deprecated for {@link #addAction(AccessibilityAction)}
     */
    CARAPI AddAction(
        /* [in] */ Int32 action);

    /**
     * Removes an action that can be performed on the node. If the action was
     * not already added to the node, calling this method has no effect.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param action The action to be removed.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     * @deprecated Use {@link #removeAction(AccessibilityAction)}
     */
    CARAPI RemoveAction(
        /* [in] */ Int32 action);

    /**
     * Removes an action that can be performed on the node. If the action was
     * not already added to the node, calling this method has no effect.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param action The action to be removed.
     * @return The action removed from the list of actions.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI RemoveAction(
        /* [in] */ IAccessibilityNodeInfoAccessibilityAction* action,
        /* [out] */ Boolean* result);

    /**
     * Sets the maximum text length, or -1 for no limit.
     * <p>
     * Typically used to indicate that an editable text field has a limit on
     * the number of characters entered.
     * <p>
     * <strong>Note:</strong> Cannot be called from an
     * {@link android.accessibilityservice.AccessibilityService}.
     * This class is made immutable before being delivered to an AccessibilityService.
     *
     * @param max The maximum text length.
     * @see #getMaxTextLength()
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetMaxTextLength(
        /* [in] */ Int32 max);

    /**
     * Returns the maximum text length for this node.
     *
     * @return The maximum text length, or -1 for no limit.
     * @see #setMaxTextLength(int)
     */
    CARAPI GetMaxTextLength(
        /* [out] */ Int32* max);

    /**
     * Sets the movement granularities for traversing the text of this node.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param granularities The bit mask with granularities.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetMovementGranularities(
        /* [in] */ Int32 granularities);

    /**
     * Gets the movement granularities for traversing the text of this node.
     *
     * @return The bit mask with granularities.
     */
    CARAPI GetMovementGranularities(
        /* [out] */ Int32* granularities);

    /**
     * Performs an action on the node.
     * <p>
     *   <strong>Note:</strong> An action can be performed only if the request is made
     *   from an {@link android.accessibilityservice.AccessibilityService}.
     * </p>
     *
     * @param action The action to perform.
     * @return True if the action was performed.
     *
     * @throws IllegalStateException If called outside of an AccessibilityService.
     */
    CARAPI PerformAction(
        /* [in] */ Int32 action,
        /* [out] */ Boolean* result);

    /**
     * Performs an action on the node.
     * <p>
     *   <strong>Note:</strong> An action can be performed only if the request is made
     *   from an {@link android.accessibilityservice.AccessibilityService}.
     * </p>
     *
     * @param action The action to perform.
     * @param arguments A bundle with additional arguments.
     * @return True if the action was performed.
     *
     * @throws IllegalStateException If called outside of an AccessibilityService.
     */
    CARAPI PerformAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* result);

    /**
     * Finds {@link AccessibilityNodeInfo}s by text. The match is case
     * insensitive containment. The search is relative to this info i.e.
     * this info is the root of the traversed tree.
     *
     * <p>
     *   <strong>Note:</strong> It is a client responsibility to recycle the
     *     received info by calling {@link AccessibilityNodeInfo#recycle()}
     *     to avoid creating of multiple instances.
     * </p>
     *
     * @param text The searched text.
     * @return A list of node info.
     */
    CARAPI FindAccessibilityNodeInfosByText(
        /* [in] */ const String& text,
        /* [out] */ IList** list);

    /**
     * Finds {@link AccessibilityNodeInfo}s by the fully qualified view id's resource
     * name where a fully qualified id is of the from "package:id/id_resource_name".
     * For example, if the target application's package is "foo.bar" and the id
     * resource name is "baz", the fully qualified resource id is "foo.bar:id/baz".
     *
     * <p>
     *   <strong>Note:</strong> It is a client responsibility to recycle the
     *     received info by calling {@link AccessibilityNodeInfo#recycle()}
     *     to avoid creating of multiple instances.
     * </p>
     * <p>
     *   <strong>Note:</strong> The primary usage of this API is for UI test automation
     *   and in order to report the fully qualified view id if an {@link AccessibilityNodeInfo}
     *   the client has to set the {@link AccessibilityServiceInfo#FLAG_REPORT_VIEW_IDS}
     *   flag when configuring his {@link android.accessibilityservice.AccessibilityService}.
     * </p>
     *
     * @param viewId The fully qualified resource name of the view id to find.
     * @return A list of node info.
     */
    CARAPI FindAccessibilityNodeInfosByViewId(
        /* [in] */ const String& viewId,
        /* [out] */ IList** list);

    /**
     * Gets the window to which this node belongs.
     *
     * @return The window.
     *
     * @see android.accessibilityservice.AccessibilityService#getWindows()
     */
    CARAPI GetWindow(
        /* [out] */ IAccessibilityWindowInfo** info);

    /**
     * Gets the parent.
     * <p>
     *   <strong>Note:</strong> It is a client responsibility to recycle the
     *     received info by calling {@link AccessibilityNodeInfo#recycle()}
     *     to avoid creating of multiple instances.
     * </p>
     *
     * @return The parent.
     */
    CARAPI GetParent(
        /* [out] */ IAccessibilityNodeInfo** parent);

    /**
     * @return The parent node id.
     *
     * @hide
     */
    CARAPI GetParentNodeId(
        /* [out] */ Int64* id);

    /**
     * Sets the parent.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param parent The parent.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetParent(
        /* [in] */ IView* parent);

    /**
     * Sets the parent to be a virtual descendant of the given <code>root</code>.
     * If <code>virtualDescendantId</code> equals to {@link View#NO_ID} the root
     * is set as the parent.
     * <p>
     * A virtual descendant is an imaginary View that is reported as a part of the view
     * hierarchy for accessibility purposes. This enables custom views that draw complex
     * content to report them selves as a tree of virtual views, thus conveying their
     * logical structure.
     * </p>
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param root The root of the virtual subtree.
     * @param virtualDescendantId The id of the virtual descendant.
     */
    CARAPI SetParent(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId);

    /**
     * Gets the node bounds in parent coordinates.
     *
     * @param outBounds The output node bounds.
     */
    CARAPI GetBoundsInParent(
        /* [in] */ IRect* outBounds);

    /**
     * Sets the node bounds in parent coordinates.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param bounds The node bounds.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetBoundsInParent(
        /* [in] */ IRect* bounds);

    /**
     * Gets the node bounds in screen coordinates.
     *
     * @param outBounds The output node bounds.
     */
    CARAPI GetBoundsInScreen(
        /* [in] */ IRect* outBounds);

    /**
     * Sets the node bounds in screen coordinates.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param bounds The node bounds.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetBoundsInScreen(
        /* [in] */ IRect* bounds);

    /**
     * Gets whether this node is checkable.
     *
     * @return True if the node is checkable.
     */
    CARAPI IsCheckable(
        /* [out] */ Boolean* checkable);

    /**
     * Sets whether this node is checkable.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param checkable True if the node is checkable.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetCheckable(
        /* [in] */ Boolean checkable);

    /**
     * Gets whether this node is checked.
     *
     * @return True if the node is checked.
     */
    CARAPI IsChecked(
        /* [out] */ Boolean* checked);

    /**
     * Sets whether this node is checked.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param checked True if the node is checked.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    /**
     * Gets whether this node is focusable.
     *
     * @return True if the node is focusable.
     */
    CARAPI IsFocusable(
        /* [out] */ Boolean* focusable);

    /**
     * Sets whether this node is focusable.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param focusable True if the node is focusable.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetFocusable(
        /* [in] */ Boolean focusable);

    /**
     * Gets whether this node is focused.
     *
     * @return True if the node is focused.
     */
    CARAPI IsFocused(
        /* [out] */ Boolean* focused);

    /**
     * Sets whether this node is focused.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param focused True if the node is focused.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetFocused(
        /* [in] */ Boolean focused);

    /**
     * Sets whether this node is visible to the user.
     *
     * @return Whether the node is visible to the user.
     */
    CARAPI IsVisibleToUser(
        /* [out] */ Boolean* visible);

    /**
     * Sets whether this node is visible to the user.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param visibleToUser Whether the node is visible to the user.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetVisibleToUser(
        /* [in] */ Boolean visible);

    /**
     * Gets whether this node is accessibility focused.
     *
     * @return True if the node is accessibility focused.
     */
    CARAPI IsAccessibilityFocused(
        /* [out] */ Boolean* focused);

    /**
     * Sets whether this node is accessibility focused.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param focused True if the node is accessibility focused.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetAccessibilityFocused(
        /* [in] */ Boolean focusable);

    /**
     * Gets whether this node is selected.
     *
     * @return True if the node is selected.
     */
    CARAPI IsSelected(
        /* [out] */ Boolean* selected);

    /**
     * Sets whether this node is selected.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param selected True if the node is selected.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetSelected(
        /* [in] */ Boolean selected);

    /**
     * Gets whether this node is clickable.
     *
     * @return True if the node is clickable.
     */
    CARAPI IsClickable(
        /* [out] */ Boolean* clickable);

    /**
     * Sets whether this node is clickable.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param clickable True if the node is clickable.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetClickable(
        /* [in] */ Boolean clickable);

    /**
     * Gets whether this node is long clickable.
     *
     * @return True if the node is long clickable.
     */
    CARAPI IsLongClickable(
        /* [out] */ Boolean* longClickable);

    /**
     * Sets whether this node is long clickable.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param longClickable True if the node is long clickable.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetLongClickable(
        /* [in] */ Boolean longClickable);

    /**
     * Gets whether this node is enabled.
     *
     * @return True if the node is enabled.
     */
    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    /**
     * Sets whether this node is enabled.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param enabled True if the node is enabled.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Gets whether this node is a password.
     *
     * @return True if the node is a password.
     */
    CARAPI IsPassword(
        /* [out] */ Boolean* isPassword);

    /**
     * Sets whether this node is a password.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param password True if the node is a password.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetPassword(
        /* [in] */ Boolean isPassword);

    /**
     * Gets if the node is scrollable.
     *
     * @return True if the node is scrollable, false otherwise.
     */
    CARAPI IsScrollable(
        /* [out] */ Boolean* scrollable);

    /**
     * Sets if the node is scrollable.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param scrollable True if the node is scrollable, false otherwise.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetScrollable(
        /* [in] */ Boolean scrollable);

    /**
     * Gets if the node is editable.
     *
     * @return True if the node is editable, false otherwise.
     */
    CARAPI IsEditable(
        /* [out] */ Boolean* editable);

    /**
     * Sets whether this node is editable.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param editable True if the node is editable.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetEditable(
        /* [in] */ Boolean editable);

    /**
     * Gets the collection info if the node is a collection. A collection
     * child is always a collection item.
     *
     * @return The collection info.
     */
    CARAPI GetCollectionInfo(
        /* [out] */ IAccessibilityNodeInfoCollectionInfo** info);

    /**
     * Sets the collection info if the node is a collection. A collection
     * child is always a collection item.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param collectionInfo The collection info.
     */
    CARAPI SetCollectionInfo(
        /* [in] */ IAccessibilityNodeInfoCollectionInfo* collectionInfo);

    /**
     * Gets the collection item info if the node is a collection item. A collection
     * item is always a child of a collection.
     *
     * @return The collection item info.
     */
    CARAPI GetCollectionItemInfo(
        /* [out] */ IAccessibilityNodeInfoCollectionItemInfo** info);

    /**
     * Sets the collection item info if the node is a collection item. A collection
     * item is always a child of a collection.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     */
    CARAPI SetCollectionItemInfo(
        /* [in] */ IAccessibilityNodeInfoCollectionItemInfo* collectionItemInfo);

    /**
     * Gets the range info if this node is a range.
     *
     * @return The range.
     */
    CARAPI GetRangeInfo(
        /* [out] */ IAccessibilityNodeInfoRangeInfo** info);

    /**
     * Sets the range info if this node is a range.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param rangeInfo The range info.
     */
    CARAPI SetRangeInfo(
        /* [in] */ IAccessibilityNodeInfoRangeInfo* rangeInfo);

    /**
     * Gets if the content of this node is invalid. For example,
     * a date is not well-formed.
     *
     * @return If the node content is invalid.
     */
    CARAPI IsContentInvalid(
        /* [out] */ Boolean* result);

    /**
     * Sets if the content of this node is invalid. For example,
     * a date is not well-formed.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param contentInvalid If the node content is invalid.
     */
    CARAPI SetContentInvalid(
        /* [in] */ Boolean contentInvalid);

    /**
     * Gets the node's live region mode.
     * <p>
     * A live region is a node that contains information that is important for
     * the user and when it changes the user should be notified. For example,
     * in a login screen with a TextView that displays an "incorrect password"
     * notification, that view should be marked as a live region with mode
     * {@link View#ACCESSIBILITY_LIVE_REGION_POLITE}.
     * <p>
     * It is the responsibility of the accessibility service to monitor
     * {@link AccessibilityEvent#TYPE_WINDOW_CONTENT_CHANGED} events indicating
     * changes to live region nodes and their children.
     *
     * @return The live region mode, or
     *         {@link View#ACCESSIBILITY_LIVE_REGION_NONE} if the view is not a
     *         live region.
     * @see android.view.View#getAccessibilityLiveRegion()
     */
    CARAPI GetLiveRegion(
        /* [out] */ Int32* region);

    /**
     * Sets the node's live region mode.
     * <p>
     * <strong>Note:</strong> Cannot be called from an
     * {@link android.accessibilityservice.AccessibilityService}. This class is
     * made immutable before being delivered to an AccessibilityService.
     *
     * @param mode The live region mode, or
     *        {@link View#ACCESSIBILITY_LIVE_REGION_NONE} if the view is not a
     *        live region.
     * @see android.view.View#setAccessibilityLiveRegion(int)
     */
    CARAPI SetLiveRegion(
        /* [in] */ Int32 mode);

    /**
     * Gets if the node is a multi line editable text.
     *
     * @return True if the node is multi line.
     */
    CARAPI IsMultiLine(
        /* [out] */ Boolean* result);

    /**
     * Sets if the node is a multi line editable text.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param multiLine True if the node is multi line.
     */
    CARAPI SetMultiLine(
        /* [in] */ Boolean multiLine);

    /**
     * Gets if this node opens a popup or a dialog.
     *
     * @return If the the node opens a popup.
     */
    CARAPI CanOpenPopup(
        /* [out] */ Boolean* result);

    /**
     * Sets if this node opens a popup or a dialog.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param opensPopup If the the node opens a popup.
     */
    CARAPI SetCanOpenPopup(
        /* [in] */ Boolean opensPopup);

    /**
     * Gets if the node can be dismissed.
     *
     * @return If the node can be dismissed.
     */
    CARAPI IsDismissable(
        /* [out] */ Boolean* dismissable);

    /**
     * Sets if the node can be dismissed.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param dismissable If the node can be dismissed.
     */
    CARAPI SetDismissable(
        /* [in] */ Boolean dismissable);

    /**
     * Gets the package this node comes from.
     *
     * @return The package name.
     */
    CARAPI GetPackageName(
        /* [out] */ ICharSequence** packageName);

    /**
     * Sets the package this node comes from.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param packageName The package name.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetPackageName(
        /* [in] */ ICharSequence* packageName);

    /**
     * Gets the class this node comes from.
     *
     * @return The class name.
     */
    CARAPI GetClassName(
        /* [out] */ ICharSequence** className);

    /**
     * Sets the class this node comes from.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param className The class name.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetClassName(
        /* [in] */ ICharSequence* className);

    /**
     * Gets the text of this node.
     *
     * @return The text.
     */
    CARAPI GetText(
        /* [out] */ ICharSequence** text);

    /**
     * Sets the text of this node.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param text The text.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetText(
        /* [in] */ ICharSequence* text);

    /**
     * Sets the error text of this node.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param error The error text.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetError(
        /* [in] */ ICharSequence* error);

    /**
     * Gets the error text of this node.
     *
     * @return The error text.
     */
    CARAPI GetError(
        /* [out] */ ICharSequence** error);

    /**
     * Gets the content description of this node.
     *
     * @return The content description.
     */
    CARAPI GetContentDescription(
        /* [out] */ ICharSequence** description);

    /**
     * Sets the content description of this node.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param contentDescription The content description.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetContentDescription(
        /* [in] */ ICharSequence* description);

    /**
     * Sets the view for which the view represented by this info serves as a
     * label for accessibility purposes.
     *
     * @param labeled The view for which this info serves as a label.
     */
    CARAPI SetLabelFor(
        /* [in] */ IView* labeled);

    /**
     * Sets the view for which the view represented by this info serves as a
     * label for accessibility purposes. If <code>virtualDescendantId</code>
     * is {@link View#NO_ID} the root is set as the labeled.
     * <p>
     * A virtual descendant is an imaginary View that is reported as a part of the view
     * hierarchy for accessibility purposes. This enables custom views that draw complex
     * content to report themselves as a tree of virtual views, thus conveying their
     * logical structure.
     * </p>
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param root The root whose virtual descendant serves as a label.
     * @param virtualDescendantId The id of the virtual descendant.
     */
    CARAPI SetLabelFor(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId);

    /**
     * Gets the node info for which the view represented by this info serves as
     * a label for accessibility purposes.
     * <p>
     *   <strong>Note:</strong> It is a client responsibility to recycle the
     *     received info by calling {@link AccessibilityNodeInfo#recycle()}
     *     to avoid creating of multiple instances.
     * </p>
     *
     * @return The labeled info.
     */
    CARAPI GetLabelFor(
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Sets the view which serves as the label of the view represented by
     * this info for accessibility purposes.
     *
     * @param label The view that labels this node's source.
     */
    CARAPI SetLabeledBy(
        /* [in] */ IView* label);

    /**
     * Sets the view which serves as the label of the view represented by
     * this info for accessibility purposes. If <code>virtualDescendantId</code>
     * is {@link View#NO_ID} the root is set as the label.
     * <p>
     * A virtual descendant is an imaginary View that is reported as a part of the view
     * hierarchy for accessibility purposes. This enables custom views that draw complex
     * content to report themselves as a tree of virtual views, thus conveying their
     * logical structure.
     * </p>
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param root The root whose virtual descendant labels this node's source.
     * @param virtualDescendantId The id of the virtual descendant.
     */
    CARAPI SetLabeledBy(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId);

    /**
     * Gets the node info which serves as the label of the view represented by
     * this info for accessibility purposes.
     * <p>
     *   <strong>Note:</strong> It is a client responsibility to recycle the
     *     received info by calling {@link AccessibilityNodeInfo#recycle()}
     *     to avoid creating of multiple instances.
     * </p>
     *
     * @return The label.
     */
    CARAPI GetLabeledBy(
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Sets the fully qualified resource name of the source view's id.
     *
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param viewIdResName The id resource name.
     */
    CARAPI SetViewIdResourceName(
        /* [in] */ const String& viewIdResName);

    /**
     * Gets the fully qualified resource name of the source view's id.
     *
     * <p>
     *   <strong>Note:</strong> The primary usage of this API is for UI test automation
     *   and in order to report the source view id of an {@link AccessibilityNodeInfo} the
     *   client has to set the {@link AccessibilityServiceInfo#FLAG_REPORT_VIEW_IDS}
     *   flag when configuring his {@link android.accessibilityservice.AccessibilityService}.
     * </p>
     * @return The id resource name.
     */
    CARAPI GetViewIdResourceName(
        /* [out] */ String* viewIdResName);

    /**
     * Gets the text selection start.
     *
     * @return The text selection start if there is selection or -1.
     */
    CARAPI GetTextSelectionStart(
        /* [out] */ Int32* start);

    /**
     * Gets the text selection end.
     *
     * @return The text selection end if there is selection or -1.
     */
    CARAPI GetTextSelectionEnd(
        /* [out] */ Int32* end);

    /**
     * Sets the text selection start and end.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an AccessibilityService.
     * </p>
     *
     * @param start The text selection start.
     * @param end The text selection end.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetTextSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Gets the input type of the source as defined by {@link InputType}.
     *
     * @return The input type.
     */
    CARAPI GetInputType(
        /* [out] */ Int32* type);

    /**
     * Sets the input type of the source as defined by {@link InputType}.
     * <p>
     *   <strong>Note:</strong> Cannot be called from an
     *   {@link android.accessibilityservice.AccessibilityService}.
     *   This class is made immutable before being delivered to an
     *   AccessibilityService.
     * </p>
     *
     * @param inputType The input type.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetInputType(
        /* [in] */ Int32 inputType);

    /**
     * Gets an optional bundle with extra data. The bundle
     * is lazily created and never <code>null</code>.
     * <p>
     * <strong>Note:</strong> It is recommended to use the package
     * name of your application as a prefix for the keys to avoid
     * collisions which may confuse an accessibility service if the
     * same key has different meaning when emitted from different
     * applications.
     * </p>
     *
     * @return The bundle.
     */
    CARAPI GetExtras(
        /* [out] */ IBundle** bundle);

    /**
     * Sets the unique id of the IAccessibilityServiceConnection over which
     * this instance can send requests to the system.
     *
     * @param connectionId The connection id.
     *
     * @hide
     */
    CARAPI SetConnectionId(
        /* [in] */ Int32 connectionId);

    /**
     * Gets the id of the source node.
     *
     * @return The id.
     *
     * @hide
     */
    CARAPI GetSourceNodeId(
        /* [out] */ Int64* id);

    /**
     * Sets if this instance is sealed.
     *
     * @param sealed Whether is sealed.
     *
     * @hide
     */
    CARAPI SetSealed(
        /* [in] */ Boolean sealed);

    /**
     * Gets if this instance is sealed.
     *
     * @return Whether is sealed.
     *
     * @hide
     */
    CARAPI IsSealed(
        /* [out] */ Boolean* sealed);

    static CARAPI Obtain(
        /* [in] */ IView* source,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Returns a cached instance if such is available otherwise a new one
     * and sets the source.
     *
     * @param root The root of the virtual subtree.
     * @param virtualDescendantId The id of the virtual descendant.
     * @return An instance.
     *
     * @see #setSource(View, int)
     */
    static CARAPI Obtain(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId,
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Returns a cached instance if such is available otherwise a new one.
     *
     * @return An instance.
     */
    static CARAPI Obtain(
        /* [out] */ IAccessibilityNodeInfo** info);

    /**
     * Returns a cached instance if such is available or a new one is
     * create. The returned instance is initialized from the given
     * <code>info</code>.
     *
     * @param info The other info.
     * @return An instance.
     */
    static CARAPI Obtain(
        /* [in] */ IAccessibilityNodeInfo* info,
        /* [out] */ IAccessibilityNodeInfo** infoClone);

    CARAPI Recycle();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ToString(
        /* [out] */ String* s);

protected:
    /**
     * Enforces that this instance is sealed.
     *
     * @throws IllegalStateException If this instance is not sealed.
     *
     * @hide
     */
    CARAPI EnforceSealed();

    /**
     * Enforces that this instance is not sealed.
     *
     * @throws IllegalStateException If this instance is sealed.
     *
     * @hide
     */
    CARAPI EnforceNotSealed();

private:
    CARAPI AddChildInternal(
        /* [in] */ IView* root,
        /* [in] */ Int32 virtualDescendantId,
        /* [in] */ Boolean checked);

    /**
     * Gets the value of a boolean property.
     *
     * @param property The property.
     * @return The value.
     */
    CARAPI_(Boolean) GetBooleanProperty(
        /* [in] */ Int32 property);

    /**
     * Sets a boolean property.
     *
     * @param property The property.
     * @param value The value.
     *
     * @throws IllegalStateException If called from an AccessibilityService.
     */
    CARAPI SetBooleanProperty(
        /* [in] */ Int32 property,
        /* [in] */ Boolean value);

    CARAPI EnforceValidFocusDirection(
        /* [in] */ Int32 direction);

    CARAPI EnforceValidFocusType(
        /* [in] */ Int32 focusType);

    /**
     * Initializes this instance from another one.
     *
     * @param other The other instance.
     */
    // @SuppressWarnings("unchecked")
    CARAPI_(void) Init(
        /* [in] */ IAccessibilityNodeInfo* other);

    /**
     * Clears the state of this instance.
     */
    CARAPI_(void) Clear();

    static CARAPI_(Boolean) IsDefaultLegacyStandardAction(
        /* [in] */ IAccessibilityNodeInfoAccessibilityAction* action);

    static CARAPI_(AutoPtr<IAccessibilityNodeInfoAccessibilityAction>) GetActionSingleton(
        /* [in] */ Int32 actionId);

    CARAPI_(void) AddLegacyStandardActions(
        /* [in] */ Int32 actionMask);

    /**
     * Gets the human readable action symbolic name.
     *
     * @param action The action.
     * @return The symbolic name.
     */
    static CARAPI_(String) GetActionSymbolicName(
        /* [in] */ Int32 action);

    /**
     * Gets the human readable movement granularity symbolic name.
     *
     * @param granularity The granularity.
     * @return The symbolic name.
     */
    static CARAPI_(String) GetMovementGranularitySymbolicName(
        /* [in] */ Int32 granularity);

    CARAPI_(Boolean) CanPerformRequestOverConnection(
        /* [in] */ Int64 accessibilityNodeId);

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 LAST_LEGACY_STANDARD_ACTION;

    /**
     * Mask to see if the value is larger than the largest ACTION_ constant
     */
    static const Int32 ACTION_TYPE_MASK;

    // Boolean attributes.

    static const Int32 BOOLEAN_PROPERTY_CHECKABLE;

    static const Int32 BOOLEAN_PROPERTY_CHECKED;

    static const Int32 BOOLEAN_PROPERTY_FOCUSABLE;

    static const Int32 BOOLEAN_PROPERTY_FOCUSED;

    static const Int32 BOOLEAN_PROPERTY_SELECTED;

    static const Int32 BOOLEAN_PROPERTY_CLICKABLE;

    static const Int32 BOOLEAN_PROPERTY_LONG_CLICKABLE;

    static const Int32 BOOLEAN_PROPERTY_ENABLED;

    static const Int32 BOOLEAN_PROPERTY_PASSWORD;

    static const Int32 BOOLEAN_PROPERTY_SCROLLABLE;

    static const Int32 BOOLEAN_PROPERTY_ACCESSIBILITY_FOCUSED;

    static const Int32 BOOLEAN_PROPERTY_VISIBLE_TO_USER;

    static const Int32 BOOLEAN_PROPERTY_EDITABLE;

    static const Int32 BOOLEAN_PROPERTY_OPENS_POPUP;

    static const Int32 BOOLEAN_PROPERTY_DISMISSABLE;

    static const Int32 BOOLEAN_PROPERTY_MULTI_LINE;

    static const Int32 BOOLEAN_PROPERTY_CONTENT_INVALID;

    /**
     * Bits that provide the id of a virtual descendant of a view.
     */
    static const Int64 VIRTUAL_DESCENDANT_ID_MASK;

    /**
     * Bit shift of {@link #VIRTUAL_DESCENDANT_ID_MASK} to get to the id for a
     * virtual descendant of a view. Such a descendant does not exist in the view
     * hierarchy and is only reported via the accessibility APIs.
     */
    static const Int32 VIRTUAL_DESCENDANT_ID_SHIFT;

    // Housekeeping.
    static const Int32 MAX_POOL_SIZE;
    static AutoPtr<Pools::SynchronizedPool<IAccessibilityNodeInfo> > sPool;

    Boolean mSealed;

    // Data.
    Int32 mWindowId;
    Int64 mSourceNodeId;
    Int64 mParentNodeId;
    Int64 mLabelForId;
    Int64 mLabeledById;

    Int32 mBooleanProperties;
    AutoPtr<IRect> mBoundsInParent;
    AutoPtr<IRect> mBoundsInScreen;

    AutoPtr<ICharSequence> mPackageName;
    AutoPtr<ICharSequence> mClassName;
    AutoPtr<ICharSequence> mText;
    AutoPtr<ICharSequence> mError;
    AutoPtr<ICharSequence> mContentDescription;
    String mViewIdResourceName;

    AutoPtr<IInt64Array> mChildNodeIds;
    AutoPtr<IArrayList> mActions;
    Int32 mMaxTextLength;
    Int32 mMovementGranularities;

    Int32 mTextSelectionStart;
    Int32 mTextSelectionEnd;
    Int32 mInputType;
    Int32 mLiveRegion;
    AutoPtr<IBundle> mExtras;

    Int32 mConnectionId;
    AutoPtr<IAccessibilityNodeInfoRangeInfo> mRangeInfo;
    AutoPtr<IAccessibilityNodeInfoCollectionInfo> mCollectionInfo;
    AutoPtr<IAccessibilityNodeInfoCollectionItemInfo> mCollectionItemInfo;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYNODEINFO_H__
