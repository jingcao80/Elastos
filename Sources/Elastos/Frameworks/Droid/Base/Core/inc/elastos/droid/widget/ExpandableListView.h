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

#ifndef __ELASTOS_DROID_WIDGET_EXPANDABLELISTVIEW_H__
#define __ELASTOS_DROID_WIDGET_EXPANDABLELISTVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/ListView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * A view that shows items in a vertically scrolling two-level list. This
 * differs from the {@link ListView} by allowing two levels: groups which can
 * individually be expanded to show its children. The items come from the
 * {@link ExpandableListAdapter} associated with this view.
 * <p>
 * Expandable lists are able to show an indicator beside each item to display
 * the item's current state (the states are usually one of expanded group,
 * collapsed group, child, or last child). Use
 * {@link #setChildIndicator(Drawable)} or {@link #setGroupIndicator(Drawable)}
 * (or the corresponding XML attributes) to set these indicators (see the docs
 * for each method to see additional state that each Drawable can have). The
 * default style for an {@link ExpandableListView} provides indicators which
 * will be shown next to Views given to the {@link ExpandableListView}. The
 * layouts android.R.layout.simple_expandable_list_item_1 and
 * android.R.layout.simple_expandable_list_item_2 (which should be used with
 * {@link SimpleCursorTreeAdapter}) contain the preferred position information
 * for indicators.
 * <p>
 * The context menu information set by an {@link ExpandableListView} will be a
 * {@link ExpandableListContextMenuInfo} object with
 * {@link ExpandableListContextMenuInfo#packedPosition} being a packed position
 * that can be used with {@link #getPackedPositionType(Int64)} and the other
 * similar methods.
 * <p>
 * <em><b>Note:</b></em> You cannot use the value <code>wrap_content</code>
 * for the <code>android:layout_height</code> attribute of a
 * ExpandableListView in XML if the parent's size is also not strictly specified
 * (for example, if the parent were ScrollView you could not specify
 * wrap_content since it also can be any length. However, you can use
 * wrap_content if the ExpandableListView parent has a specific size, such as
 * 100 pixels.
 *
 * @attr ref android.R.styleable#ExpandableListView_groupIndicator
 * @attr ref android.R.styleable#ExpandableListView_indicatorLeft
 * @attr ref android.R.styleable#ExpandableListView_indicatorRight
 * @attr ref android.R.styleable#ExpandableListView_childIndicator
 * @attr ref android.R.styleable#ExpandableListView_childIndicatorLeft
 * @attr ref android.R.styleable#ExpandableListView_childIndicatorRight
 * @attr ref android.R.styleable#ExpandableListView_childDivider
 * @attr ref android.R.styleable#ExpandableListView_indicatorStart
 * @attr ref android.R.styleable#ExpandableListView_indicatorEnd
 * @attr ref android.R.styleable#ExpandableListView_childIndicatorStart
 * @attr ref android.R.styleable#ExpandableListView_childIndicatorEnd
 */

class ExpandableListView
    : public ListView
    , public IExpandableListView
{
public:
    /**
     * Extra menu information specific to an {@link ExpandableListView} provided
     * to the
     * {@link android.view.View.OnCreateContextMenuListener#onCreateContextMenu(ContextMenu, View, ContextMenuInfo) }
     * callback when a context menu is brought up for this AdapterView.
     */
    class ExpandableListContextMenuInfo
        : public Object
        , public IExpandableListContextMenuInfo
    {
    public:
        CAR_INTERFACE_DECL()

        ExpandableListContextMenuInfo();

        ~ExpandableListContextMenuInfo();

        CARAPI constructor(
            /* [in] */ IView* targetView,
            /* [in] */ Int64 packedPosition,
            /* [in] */ Int64 id);

        CARAPI SetTargetView(
            /* [in] */ IView* targetView);

        CARAPI GetTargetView(
            /* [out] */ IView** targetView);

        CARAPI SetPackedPosition(
            /* [in] */ Int64 packedPosition);

        CARAPI GetPackedPosition(
            /* [out] */ Int64* packedPosition);

        CARAPI SetId(
            /* [in] */ Int64 id);

        CARAPI GetId(
            /* [out] */ Int64* id);

    public:
        /**
         * The view for which the context menu is being displayed. This
         * will be one of the children Views of this {@link ExpandableListView}.
         */
        AutoPtr<IView> mTargetView;

        /**
         * The packed position in the list represented by the adapter for which
         * the context menu is being displayed. Use the methods
         * {@link ExpandableListView#getPackedPositionType},
         * {@link ExpandableListView#getPackedPositionChild}, and
         * {@link ExpandableListView#getPackedPositionGroup} to unpack this.
         */
        Int64 mPackedPosition;

        /**
         * The ID of the item (group or child) for which the context menu is
         * being displayed.
         */
        Int64 mId;
    };

    class ExpandableListViewSavedState
        : public View::BaseSavedState
        , public IExpandableListViewSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        ExpandableListViewSavedState();

        ~ExpandableListViewSavedState();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* superState,
            /* [in] */ IArrayList* expandedGroupMetadataList);

    public:
        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

        // @Override
        CARAPI ReadFromParcel(
            /* [in] */ IParcel* in);
    public:
        AutoPtr<IArrayList> mExpandedGroupMetadataList; // element is IGroupMetadata
    };

public:
    CAR_INTERFACE_DECL()

    ExpandableListView();

    ~ExpandableListView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI OnRtlPropertiesChanged(
        /* [in] */ Int32 layoutDirection);

    /**
     * Sets the drawable that will be drawn adjacent to every child in the list. This will
     * be drawn using the same height as the normal divider ({@link #setDivider(Drawable)}) or
     * if it does not have an intrinsic height, the height set by {@link #setDividerHeight(Int32)}.
     *
     * @param childDivider The drawable to use.
     */
    CARAPI SetChildDivider(
        /* [in] */ IDrawable* childDivider);

    /**
     * This overloaded method should not be used, instead use
     * {@link #setAdapter(ExpandableListAdapter)}.
     * <p>
     * {@inheritDoc}
     */
    // @Override
    CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    /**
     * This method should not be used, use {@link #getExpandableListAdapter()}.
     */
    // @Override
    CARAPI GetAdapter(
        /* [out] */ IAdapter** adapter);

    /**
     * Register a callback to be invoked when an item has been clicked and the
     * caller prefers to receive a ListView-style position instead of a group
     * and/or child position. In most cases, the caller should use
     * {@link #setOnGroupClickListener} and/or {@link #setOnChildClickListener}.
     * <p />
     * {@inheritDoc}
     */
    CARAPI SetOnItemClickListener(
        /* [in] */ IAdapterViewOnItemClickListener* l);

    /**
     * Sets the adapter that provides data to this view.
     * @param adapter The adapter that provides data to this view.
     */
    CARAPI SetAdapter(
        /* [in] */ IExpandableListAdapter* adapter);

    /**
     * Gets the adapter that provides data to this view.
     * @return The adapter that provides data to this view.
     */
    CARAPI GetExpandableListAdapter(
        /* [out] */ IExpandableListAdapter** adapter);

    CARAPI PerformItemClick(
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id,
        /* [out] */ Boolean* result);

    /**
     * Expand a group in the grouped list view
     *
     * @param groupPos the group to be expanded
     * @return True if the group was expanded, false otherwise (if the group
     *         was already expanded, this will return false)
     */
    CARAPI ExpandGroup(
        /* [in] */ Int32 groupPos,
        /* [out] */ Boolean* expanded);

    /**
     * Expand a group in the grouped list view
     *
     * @param groupPos the group to be expanded
     * @param animate true if the expanding group should be animated in
     * @return True if the group was expanded, false otherwise (if the group
     *         was already expanded, this will return false)
     */
    CARAPI ExpandGroup(
        /* [in] */ Int32 groupPos,
        /* [in] */ Boolean animate,
        /* [out] */ Boolean* expanded);

    /**
     * Collapse a group in the grouped list view
     *
     * @param groupPos position of the group to collapse
     * @return True if the group was collapsed, false otherwise (if the group
     *         was already collapsed, this will return false)
     */
    CARAPI CollapseGroup(
        /* [in] */ Int32 groupPos,
        /* [out] */ Boolean* collapse);

    CARAPI SetOnGroupCollapseListener(
        /* [in] */ IExpandableListViewOnGroupCollapseListener* onGroupCollapseListener);

    CARAPI SetOnGroupExpandListener(
        /* [in] */ IExpandableListViewOnGroupExpandListener* onGroupExpandListener);

    CARAPI SetOnGroupClickListener(
        /* [in] */ IExpandableListViewOnGroupClickListener* onGroupClickListener);

    CARAPI SetOnChildClickListener(
        /* [in] */ IExpandableListViewOnChildClickListener* onChildClickListener);

    /**
     * Converts a flat list position (the raw position of an item (child or group)
     * in the list) to an group and/or child position (represented in a
     * packed position). This is useful in situations where the caller needs to
     * use the underlying {@link ListView}'s methods. Use
     * {@link ExpandableListView#getPackedPositionType} ,
     * {@link ExpandableListView#getPackedPositionChild},
     * {@link ExpandableListView#getPackedPositionGroup} to unpack.
     *
     * @param flatListPosition The flat list position to be converted.
     * @return The group and/or child position for the given flat list position
     *         in packed position representation. #PACKED_POSITION_VALUE_NULL if
     *         the position corresponds to a header or a footer item.
     */
    CARAPI GetExpandableListPosition(
        /* [in] */ Int32 flatListPosition,
        /* [out] */ Int64* position);

    /**
     * Converts a group and/or child position to a flat list position. This is
     * useful in situations where the caller needs to use the underlying
     * {@link ListView}'s methods.
     *
     * @param packedPosition The group and/or child positions to be converted in
     *            packed position representation. Use
     *            {@link #getPackedPositionForChild(Int32, Int32)} or
     *            {@link #getPackedPositionForGroup(Int32)}.
     * @return The flat list position for the given child or group.
     */
    CARAPI GetFlatListPosition(
        /* [in] */ Int64 packedPosition,
        /* [out] */ Int32* position);

    /**
     * Gets the position of the currently selected group or child (along with
     * its type). Can return {@link #PACKED_POSITION_VALUE_NULL} if no selection.
     *
     * @return A packed position containing the currently selected group or
     *         child's position and type. #PACKED_POSITION_VALUE_NULL if no selection
     *         or if selection is on a header or a footer item.
     */
    CARAPI GetSelectedPosition(
        /* [out] */ Int64* position);

    /**
     * Gets the ID of the currently selected group or child. Can return -1 if no
     * selection.
     *
     * @return The ID of the currently selected group or child. -1 if no
     *         selection.
     */
    CARAPI GetSelectedId(
        /* [out] */ Int64* id);

    /**
     * Sets the selection to the specified group.
     * @param groupPosition The position of the group that should be selected.
     */
    CARAPI SetSelectedGroup(
        /* [in] */ Int32 groupPosition);

    /**
     * Sets the selection to the specified child. If the child is in a collapsed
     * group, the group will only be expanded and child subsequently selected if
     * shouldExpandGroup is set to true, otherwise the method will return false.
     *
     * @param groupPosition The position of the group that contains the child.
     * @param childPosition The position of the child within the group.
     * @param shouldExpandGroup Whether the child's group should be expanded if
     *            it is collapsed.
     * @return Whether the selection was successfully set on the child.
     */
    CARAPI SetSelectedChild(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [in] */ Boolean shouldExpandGroup,
        /* [out] */ Boolean* res);

    /**
     * Whether the given group is currently expanded.
     *
     * @param groupPosition The group to check.
     * @return Whether the group is currently expanded.
     */
    CARAPI IsGroupExpanded(
        /* [in] */ Int32 groupPosition,
        /* [out] */ Boolean* expanded);

    /**
     * Gets the type of a packed position. See
     * {@link #getPackedPositionForChild(Int32, Int32)}.
     *
     * @param packedPosition The packed position for which to return the type.
     * @return The type of the position contained within the packed position,
     *         either {@link #PACKED_POSITION_TYPE_CHILD}, {@link #PACKED_POSITION_TYPE_GROUP}, or
     *         {@link #PACKED_POSITION_TYPE_NULL}.
     */
    static CARAPI_(Int32) GetPackedPositionType(
        /* [in] */ Int64 packedPosition);

    /**
     * Gets the group position from a packed position. See
     * {@link #getPackedPositionForChild(Int32, Int32)}.
     *
     * @param packedPosition The packed position from which the group position
     *            will be returned.
     * @return The group position portion of the packed position. If this does
     *         not contain a group, returns -1.
     */
    static CARAPI_(Int32) GetPackedPositionGroup(
        /* [in] */ Int64 packedPosition);

    /**
     * Gets the child position from a packed position that is of
     * {@link #PACKED_POSITION_TYPE_CHILD} type (use {@link #getPackedPositionType(Int64)}).
     * To get the group that this child belongs to, use
     * {@link #getPackedPositionGroup(Int64)}. See
     * {@link #getPackedPositionForChild(Int32, Int32)}.
     *
     * @param packedPosition The packed position from which the child position
     *            will be returned.
     * @return The child position portion of the packed position. If this does
     *         not contain a child, returns -1.
     */
    static CARAPI_(Int32) GetPackedPositionChild(
        /* [in] */ Int64 packedPosition);

    /**
     * Returns the packed position representation of a child's position.
     * <p>
     * In general, a packed position should be used in
     * situations where the position given to/returned from an
     * {@link ExpandableListAdapter} or {@link ExpandableListView} method can
     * either be a child or group. The two positions are packed into a single
     * Int64 which can be unpacked using
     * {@link #getPackedPositionChild(Int64)},
     * {@link #getPackedPositionGroup(Int64)}, and
     * {@link #getPackedPositionType(Int64)}.
     *
     * @param groupPosition The child's parent group's position.
     * @param childPosition The child position within the group.
     * @return The packed position representation of the child (and parent group).
     */
    static CARAPI_(Int64) GetPackedPositionForChild(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition);

    /**
     * Returns the packed position representation of a group's position. See
     * {@link #getPackedPositionForChild(Int32, Int32)}.
     *
     * @param groupPosition The child's parent group's position.
     * @return The packed position representation of the group.
     */
    static CARAPI_(Int64) GetPackedPositionForGroup(
        /* [in] */ Int32 groupPosition);

     /**
     * Sets the indicator to be drawn next to a child.
     *
     * @param childIndicator The drawable to be used as an indicator. If the
     *            child is the last child for a group, the state
     *            {@link android.R.attr#state_last} will be set.
     */
    CARAPI SetChildIndicator(
        /* [in] */ IDrawable* childIndicator);

    /**
     * Sets the drawing bounds for the child indicator. For either, you can
     * specify {@link #CHILD_INDICATOR_INHERIT} to use inherit from the general
     * indicator's bounds.
     *
     * @see #setIndicatorBounds(Int32, Int32)
     * @param left The left position (relative to the left bounds of this View)
     *            to start drawing the indicator.
     * @param right The right position (relative to the left bounds of this
     *            View) to end the drawing of the indicator.
     */
    CARAPI SetChildIndicatorBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right);

    /**
     * Sets the relative drawing bounds for the child indicator. For either, you can
     * specify {@link #CHILD_INDICATOR_INHERIT} to use inherit from the general
     * indicator's bounds.
     *
     * @see #setIndicatorBounds(int, int)
     * @param start The start position (relative to the start bounds of this View)
     *            to start drawing the indicator.
     * @param end The end position (relative to the end bounds of this
     *            View) to end the drawing of the indicator.
     */
    CARAPI SetChildIndicatorBoundsRelative(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Sets the indicator to be drawn next to a group.
     *
     * @param groupIndicator The drawable to be used as an indicator. If the
     *            group is empty, the state {@link android.R.attr#state_empty} will be
     *            set. If the group is expanded, the state
     *            {@link android.R.attr#state_expanded} will be set.
     */
    CARAPI SetGroupIndicator(
        /* [in] */ IDrawable* groupIndicator);

    /**
     * Sets the drawing bounds for the indicators (at minimum, the group indicator
     * is affected by this; the child indicator is affected by this if the
     * child indicator bounds are set to inherit).
     *
     * @see #setChildIndicatorBounds(Int32, Int32)
     * @param left The left position (relative to the left bounds of this View)
     *            to start drawing the indicator.
     * @param right The right position (relative to the left bounds of this
     *            View) to end the drawing of the indicator.
     */
    CARAPI SetIndicatorBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 right);

    /**
     * Sets the relative drawing bounds for the indicators (at minimum, the group indicator
     * is affected by this; the child indicator is affected by this if the
     * child indicator bounds are set to inherit).
     *
     * @see #setChildIndicatorBounds(int, int)
     * @param start The start position (relative to the start bounds of this View)
     *            to start drawing the indicator.
     * @param end The end position (relative to the end bounds of this
     *            View) to end the drawing of the indicator.
     */
    CARAPI SetIndicatorBoundsRelative(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

protected:
    virtual CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI_(void) DrawDivider(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* bounds,
        /* [in] */ Int32 childIndex);

    /**
     * This will either expand/collapse groups (if a group was clicked) or pass
     * on the click to the proper child (if a child was clicked)
     *
     * @param position The flat list position. This has already been factored to
     *            remove the header/footer.
     * @param id The ListAdapter ID, not the group or child ID.
     */
    virtual CARAPI_(Boolean) HandleItemClick(
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    virtual CARAPI_(AutoPtr<IContextMenuInfo>) CreateContextMenuInfo(
        /* [in] */ IView* view,
        /* [in] */ Int32 flatListPosition,
        /* [in] */ Int64 id);

private:
    /**
     * Return true if we are in RTL compatibility mode (either before Jelly Bean MR1 or
     * RTL not supported)
     */
    CARAPI_(Boolean) IsRtlCompatibilityMode();

    /**
     * Return true if the application tag in the AndroidManifest has set "supportRtl" to true
     */
    CARAPI_(Boolean) HasRtlSupport();

    /**
     * Resolve start/end indicator. start/end indicator always takes precedence over left/right
     * indicator when defined.
     */
    CARAPI_(void) ResolveIndicator();

    /**
     * Resolve start/end child indicator. start/end child indicator always takes precedence over
     * left/right child indicator when defined.
     */
    CARAPI_(void) ResolveChildIndicator();


    /**
     * Gets the indicator for the item at the given position. If the indicator
     * is stateful, the state will be given to the indicator.
     *
     * @param pos The flat list position of the item whose indicator
     *            should be returned.
     * @return The indicator in the proper state.
     */
    CARAPI_(AutoPtr<IDrawable>) GetIndicator(
        /* [in] */ IPositionMetadata* pos);

    /**
     * @param position An absolute (including header and footer) flat list position.
     * @return true if the position corresponds to a header or a footer item.
     */
    CARAPI_(Boolean) IsHeaderOrFooterPosition(
        /* [in] */ Int32 position);

    /**
     * Converts an absolute item flat position into a group/child flat position, shifting according
     * to the number of header items.
     *
     * @param flatListPosition The absolute flat position
     * @return A group/child flat position as expected by the connector.
     */
    CARAPI_(Int32) GetFlatPositionForConnector(
        /* [in] */ Int32 flatListPosition);

    /**
     * Converts a group/child flat position into an absolute flat position, that takes into account
     * the possible headers.
     *
     * @param flatListPosition The child/group flat position
     * @return An absolute flat position.
     */
    CARAPI_(Int32) GetAbsoluteFlatPosition(
        /* [in] */ Int32 flatListPosition);

    /**
     * Gets the ID of the group or child at the given <code>position</code>.
     * This is useful since there is no ListAdapter ID -> ExpandableListAdapter
     * ID conversion mechanism (in some cases, it isn't possible).
     *
     * @param position The position of the child or group whose ID should be
     *            returned.
     */
    CARAPI_(Int64) GetChildOrGroupId(
        /* [in] */ IExpandableListPosition* position);

    static CARAPI_(AutoPtr< ArrayOf< ArrayOf<Int32>* > >) InitGROUP_STATE_SETS();

private:
    /** The mask (in packed position representation) for the child */
    static const Int64 PACKED_POSITION_MASK_CHILD;

    /** The mask (in packed position representation) for the group */
    static const Int64 PACKED_POSITION_MASK_GROUP;

    /** The mask (in packed position representation) for the type */
    static const Int64 PACKED_POSITION_MASK_TYPE;

    /** The shift amount (in packed position representation) for the group */
    static const Int64 PACKED_POSITION_SHIFT_GROUP;

    /** The shift amount (in packed position representation) for the type */
    static const Int64 PACKED_POSITION_SHIFT_TYPE;

    /** The mask (in integer child position representation) for the child */
    static const Int64 PACKED_POSITION_INT_MASK_CHILD;

    /** The mask (in integer group position representation) for the group */
    static const Int64 PACKED_POSITION_INT_MASK_GROUP;

    /** Serves as the glue/translator between a ListView and an ExpandableListView */
    AutoPtr<IExpandableListConnector> mConnector;

    /** Gives us Views through group+child positions */
    AutoPtr<IExpandableListAdapter> mAdapter;

    /** Left bound for drawing the indicator. */
    Int32 mIndicatorLeft;

    /** Right bound for drawing the indicator. */
    Int32 mIndicatorRight;

    /** Start bound for drawing the indicator. */
    Int32 mIndicatorStart;

    /** End bound for drawing the indicator. */
    Int32 mIndicatorEnd;

    /**
     * Left bound for drawing the indicator of a child. Value of
     * {@link #CHILD_INDICATOR_INHERIT} means use mIndicatorLeft.
     */
    Int32 mChildIndicatorLeft;

    /**
     * Right bound for drawing the indicator of a child. Value of
     * {@link #CHILD_INDICATOR_INHERIT} means use mIndicatorRight.
     */
    Int32 mChildIndicatorRight;

    /**
     * Start bound for drawing the indicator of a child. Value of
     * {@link #CHILD_INDICATOR_INHERIT} means use mIndicatorStart.
     */
    Int32 mChildIndicatorStart;

    /**
     * End bound for drawing the indicator of a child. Value of
     * {@link #CHILD_INDICATOR_INHERIT} means use mIndicatorEnd.
     */
    Int32 mChildIndicatorEnd;

    /**
     * Denotes an undefined value for an indicator
     */
    static const Int32 INDICATOR_UNDEFINED;

    /** The indicator drawn next to a group. */
    AutoPtr<IDrawable> mGroupIndicator;

    /** The indicator drawn next to a child. */
    AutoPtr<IDrawable> mChildIndicator;

    static const AutoPtr< ArrayOf<Int32> > EMPTY_STATE_SET;

    /** State indicating the group is expanded. */
    static const AutoPtr< ArrayOf<Int32> > GROUP_EXPANDED_STATE_SET;

    /** State indicating the group is empty (has no children). */
    static const AutoPtr< ArrayOf<Int32> > GROUP_EMPTY_STATE_SET;

    /** State indicating the group is expanded and empty (has no children). */
    static const AutoPtr< ArrayOf<Int32> > GROUP_EXPANDED_EMPTY_STATE_SET;

    /** States for the group where the 0th bit is expanded and 1st bit is empty. */
    static const AutoPtr< ArrayOf< ArrayOf<Int32>* > > GROUP_STATE_SETS;

    /** State indicating the child is the last within its group. */
    static const AutoPtr< ArrayOf<Int32> > CHILD_LAST_STATE_SET;

    /** Drawable to be used as a divider when it is adjacent to any children */
    AutoPtr<IDrawable> mChildDivider;

    // Bounds of the indicator to be drawn
    AutoPtr<IRect> mIndicatorRect;

    AutoPtr<IExpandableListViewOnGroupCollapseListener> mOnGroupCollapseListener;

    AutoPtr<IExpandableListViewOnGroupExpandListener> mOnGroupExpandListener;

    AutoPtr<IExpandableListViewOnGroupClickListener> mOnGroupClickListener;

    AutoPtr<IExpandableListViewOnChildClickListener> mOnChildClickListener;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_EXPANDABLELISTVIEW_H__
