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

#ifndef __ELASTOS_DROID_WIDGET_LISTVIEW_H__
#define __ELASTOS_DROID_WIDGET_LISTVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/AbsListView.h"

using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Widget {

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IListAdapter;

/**
 * A view that shows items in a vertically scrolling list. The items
 * come from the {@link ListAdapter} associated with this view.
 *
 * <p>See the <a href="{@docRoot}resources/tutorials/views/hello-listview.html">List View
 * tutorial</a>.</p>
 *
 * @attr ref android.R.styleable#ListView_entries
 * @attr ref android.R.styleable#ListView_divider
 * @attr ref android.R.styleable#ListView_dividerHeight
 * @attr ref android.R.styleable#ListView_choiceMode
 * @attr ref android.R.styleable#ListView_headerDividersEnabled
 * @attr ref android.R.styleable#ListView_footerDividersEnabled
 */
// @RemoteView
class ECO_PUBLIC ListView
    : public AbsListView
    , public IListView
{
public:
    /**
     * A class that represents a fixed view in a list, for example a header at the top
     * or a footer at the bottom.
     */
    class FixedViewInfo
        : public Object
        , public IFixedViewInfo
    {
    public:
        CAR_INTERFACE_DECL()

        FixedViewInfo();

        CARAPI SetView(
            /* [in] */ IView* view);

        CARAPI GetView(
            /* [out] */ IView** view);

        CARAPI SetData(
            /* [in] */ IInterface* data);

        CARAPI GetData(
            /* [out] */ IInterface** data);

        CARAPI SetSelectable(
            /* [in] */ Boolean selectable);

        CARAPI GetSelectable(
            /* [out] */ Boolean* selectable);

    public:
        /** The view to add to the list */
        AutoPtr<IView> mView;
        /** The data backing the view. This is returned from {@link ListAdapter#GetItem(Int32)}. */
        AutoPtr<IInterface> mData;
        /** <code>TRUE</code> if the fixed view should be selectable in the list */
        Boolean mIsSelectable;
    };

private:
    class ECO_LOCAL FocusSelector
        : public Runnable
    {
    public:
        FocusSelector(
            /* [in] */ ListView* host);

        CARAPI_(AutoPtr<FocusSelector>) Setup(
            /* [in] */ Int32 position,
            /* [in] */ Int32 top);

        CARAPI Run();

    private:
        ListView* mHost;
        Int32 mPosition;
        Int32 mPositionTop;
    };

    /**
     * Holds results of focus aware arrow scrolling.
     */
    class ECO_LOCAL ArrowScrollFocusResult
        : public Object
    {
        friend class ListView;
    public:
        ArrowScrollFocusResult();

        CARAPI_(Int32) GetSelectedPosition();

        CARAPI_(Int32) GetAmountToScroll();

    protected:
        CARAPI_(void) Populate(
            /* [in] */ Int32 selectedPosition,
            /* [in] */ Int32 amountToScroll);

    private:
        Int32 mSelectedPosition;
        Int32 mAmountToScroll;
    };

public:
    CAR_INTERFACE_DECL()

    ListView();

    ~ListView();

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

    CARAPI GetMaxScrollAmount(
        /* [out] */ Int32* amount);

    /**
     * Add a fixed view to appear at the top of the list. If this method is
     * called more than once, the views will appear in the order they were
     * added. Views added using this call can take focus if they want.
     * <p>
     * Note: When first introduced, this method could only be called before
     * setting the adapter with {@link #setAdapter(ListAdapter)}. Starting with
     * {@link android.os.Build.VERSION_CODES#KITKAT}, this method may be
     * called at any time. If the ListView's adapter does not extend
     * {@link HeaderViewListAdapter}, it will be wrapped with a supporting
     * instance of {@link WrapperListAdapter}.
     *
     * @param v The view to add.
     * @param data Data to associate with this view
     * @param isSelectable whether the item is selectable
     */
    CARAPI AddHeaderView(
        /* [in] */ IView* v,
        /* [in] */ IInterface* data,
        /* [in] */ Boolean isSelectable);

    /**
     * Add a fixed view to appear at the top of the list. If addHeaderView is
     * called more than once, the views will appear in the order they were
     * added. Views added using this call can take focus if they want.
     * <p>
     * Note: When first introduced, this method could only be called before
     * setting the adapter with {@link #setAdapter(ListAdapter)}. Starting with
     * {@link android.os.Build.VERSION_CODES#KITKAT}, this method may be
     * called at any time. If the ListView's adapter does not extend
     * {@link HeaderViewListAdapter}, it will be wrapped with a supporting
     * instance of {@link WrapperListAdapter}.
     *
     * @param v The view to add.
     */
    CARAPI AddHeaderView(
        /* [in] */ IView* v);

    //@Override
    virtual CARAPI_(Int32) GetHeaderViewsCount();

    /**
     * Removes a previously-added header view.
     *
     * @param v The view to remove
     * @return TRUE if the view was removed, FALSE if the view was not a header
     *         view
     */
    CARAPI RemoveHeaderView(
        /* [in] */ IView* v,
        /* [out] */ Boolean* res);

    /**
     * Add a fixed view to appear at the bottom of the list. If addFooterView is
     * called more than once, the views will appear in the order they were
     * added. Views added using this call can take focus if they want.
     * <p>
     * Note: When first introduced, this method could only be called before
     * setting the adapter with {@link #setAdapter(ListAdapter)}. Starting with
     * {@link android.os.Build.VERSION_CODES#KITKAT}, this method may be
     * called at any time. If the ListView's adapter does not extend
     * {@link HeaderViewListAdapter}, it will be wrapped with a supporting
     * instance of {@link WrapperListAdapter}.
     *
     * @param v The view to add.
     * @param data Data to associate with this view
     * @param isSelectable true if the footer view can be selected
     */
    CARAPI AddFooterView(
        /* [in] */ IView* v,
        /* [in] */ IInterface* data,
        /* [in] */ Boolean isSelectable);

    /**
     * Add a fixed view to appear at the bottom of the list. If addFooterView is
     * called more than once, the views will appear in the order they were
     * added. Views added using this call can take focus if they want.
     * <p>
     * Note: When first introduced, this method could only be called before
     * setting the adapter with {@link #setAdapter(ListAdapter)}. Starting with
     * {@link android.os.Build.VERSION_CODES#KITKAT}, this method may be
     * called at any time. If the ListView's adapter does not extend
     * {@link HeaderViewListAdapter}, it will be wrapped with a supporting
     * instance of {@link WrapperListAdapter}.
     *
     * @param v The view to add.
     */
    CARAPI AddFooterView(
        /* [in] */ IView* v);

    //@Override
    virtual CARAPI_(Int32) GetFooterViewsCount();

    /**
     * Removes a previously-added footer view.
     *
     * @param v The view to remove
     * @return
     * TRUE if the view was removed, FALSE if the view was not a footer view
     */
    CARAPI RemoveFooterView(
        /* [in] */ IView* v,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI GetAdapter(
        /* [out] */ IAdapter** adapter);

     /**
     * Sets up this AbsListView to use a remote views adapter which connects to a RemoteViewsService
     * through the specified intent.
     * @param intent the intent used to identify the RemoteViewsService for the adapter to connect to.
     */
    CARAPI SetRemoteViewsAdapter(
        /* [in] */ IIntent* intent);

    //@Override
    CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    //@Override
    CARAPI RequestChildRectangleOnScreen(
        /* [in] */ IView* childView,
        /* [in] */ IRect* r,
        /* [in] */ Boolean immediate,
        /* [out] */ Boolean* res);

    /**
     * Smoothly scroll to the specified adapter position. The view will
     * scroll such that the indicated position is displayed.
     * @param position Scroll to this adapter position.
     */
    CARAPI SmoothScrollToPosition(
        /* [in] */ Int32 position);

    /**
     * Smoothly scroll to the specified adapter position offset. The view will
     * scroll such that the indicated position is displayed.
     * @param offset The amount to offset from the adapter position to scroll to.
     */
    virtual CARAPI SmoothScrollByOffset(
        /* [in] */ Int32 offset);

    //@Override
    CARAPI SetSelection(
        /* [in] */ Int32 position);

    CARAPI SetSelectionAfterHeaderView();

    //@Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 repeatCount,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    CARAPI SetItemsCanFocus(
        /* [in] */ Boolean itemsCanFocus);

    CARAPI GetItemsCanFocus(
        /* [out] */ Boolean* canFocus);

    //@Override
    CARAPI IsOpaque(
        /* [out] */ Boolean* res);

    //@Override
    CARAPI SetCacheColorHint(
        /* [in] */ Int32 color);

    CARAPI GetDivider(
        /* [out] */ IDrawable** divider);

    CARAPI SetDivider(
        /* [in] */ IDrawable* divider);

    /**
     * @return Returns the height of the divider that will be drawn between each item in the list.
     */
    CARAPI GetDividerHeight(
        /* [out] */ Int32* height);

    CARAPI SetDividerHeight(
        /* [in] */ Int32 height);

    CARAPI SetHeaderDividersEnabled(
        /* [in] */ Boolean headerDividersEnabled);

    CARAPI AreHeaderDividersEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetFooterDividersEnabled(
        /* [in] */ Boolean footerDividersEnabled);

    CARAPI AreFooterDividersEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetOverscrollHeader(
        /* [in] */ IDrawable* header);

    CARAPI GetOverscrollHeader(
        /* [out] */ IDrawable** overScrollHeader);

    CARAPI SetOverscrollFooter(
        /* [in] */ IDrawable* footer);

    CARAPI GetOverscrollFooter(
        /* [out] */ IDrawable** overScrollFooter);

    CARAPI GetCheckItemIds(
        /* [out, callee] */ ArrayOf<Int64>** itemIds);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfoForItem(
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * Find a position that can be selected (i.e., is not a separator).
     *
     * @param position The starting position to look at.
     * @param lookDown Whether to look down for other positions.
     * @return The next selectable position starting at position and then searching either up or
     *         down. Returns {@link #INVALID_POSITION} if nothing can be found.
     */
    //@Override
    virtual CARAPI_(Int32) LookForSelectablePosition(
        /* [in] */ Int32 position,
        /* [in] */ Boolean lookDown);

    virtual CARAPI_(Int32) MeasureHeightOfChildren(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 startPosition,
        /* [in] */ Int32 endPosition,
        /* [in] */ Int32 maxHeight,
        /* [in] */ Int32 disallowPartialChildPosition);

protected:
    //@Override
    virtual CARAPI_(void) ResetList();

    //@Override
    virtual CARAPI FillGap(
        /* [in] */ Boolean down);

    //@Override
    virtual CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    //@Override
    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI_(Boolean) RecycleOnMeasure();

    //@Override
    virtual CARAPI_(Int32) FindMotionRow(
        /* [in] */ Int32 y);

    //@Override
    virtual CARAPI LayoutChildren();

    //@Override
    virtual CARAPI_(Boolean) CanAnimate();

    //@Override
    virtual CARAPI SetSelectionInt(
        /* [in] */ Int32 position);

    /**
     * Find a position that can be selected (i.e., is not a separator). If there
     * are no selectable positions in the specified direction from the starting
     * position, searches in the opposite direction from the starting position
     * to the current position.
     *
     * @param current the current position
     * @param position the starting position
     * @param lookDown whether to look down for other positions
     * @return the next selectable position, or {@link #INVALID_POSITION} if
     *         nothing can be found
     */
    virtual CARAPI_(Int32) LookForSelectablePositionAfter(
        /* [in] */ Int32 current,
        /* [in] */ Int32 position,
        /* [in] */ Boolean lookDown);

    virtual CARAPI_(Boolean) PageScroll(
        /* [in] */ Int32 direction);

    virtual CARAPI_(Boolean) FullScroll(
        /* [in] */ Int32 direction);

    virtual CARAPI_(Boolean) ArrowScroll(
        /* [in] */ Int32 direction);

    virtual CARAPI_(void) DrawOverscrollHeader(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IDrawable* drawable,
        /* [in] */ IRect* bounds);

    virtual CARAPI_(void) DrawOverscrollFooter(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IDrawable* drawable,
        /* [in] */ IRect* bounds);

    //@Override
    virtual CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI_(void) DrawDivider(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* bounds,
        /* [in] */ Int32 childIndex);

    //@Override
    virtual CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean gainFocus,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    //@Override
    virtual CARAPI OnFinishInflate();

    //@Override
    virtual CARAPI_(AutoPtr<IView>) FindViewTraversal(
        /* [in] */ Int32 id);

    virtual CARAPI_(AutoPtr<IView>) FindViewInHeadersOrFooters(
        /* [in] */ IArrayList* where,
        /* [in] */ Int32 id);

    //@Override
    virtual CARAPI_(AutoPtr<IView>) FindViewWithTagTraversal(
        /* [in] */ IInterface* tag);

    virtual CARAPI_(AutoPtr<IView>) FindViewWithTagInHeadersOrFooters(
        /* [in] */ IArrayList* where,
        /* [in] */ IInterface* tag);

    virtual CARAPI_(Boolean) DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime);

    virtual CARAPI_(AutoPtr<IView>) FindViewByPredicateTraversal(
        /* [in] */ IPredicate* predicate,
        /* [in] */ IView* childToSkip);

    virtual CARAPI_(AutoPtr<IView>) FindViewByPredicateInHeadersOrFooters(
        /* [in] */ IArrayList* where,
        /* [in] */ IPredicate* predicate,
        /* [in] */ IView* childToSkip);

    // @Override
    virtual CARAPI_(Int32) GetHeightForPosition(
        /* [in] */ Int32 position);

private:
    ECO_LOCAL CARAPI_(void) AdjustViewsUpOrDown();

    ECO_LOCAL CARAPI_(void) RemoveFixedViewInfo(
        /* [in] */ IView* v,
        /* [in] */ IArrayList* where);

    ECO_LOCAL CARAPI_(void) ClearRecycledState(
        /* [in] */ IArrayList* infos);

    ECO_LOCAL CARAPI_(Boolean) ShowingTopFadingEdge();

    ECO_LOCAL CARAPI_(Boolean) ShowingBottomFadingEdge();

    ECO_LOCAL CARAPI_(AutoPtr<IView>) FillDown(
        /* [in] */ Int32 pos,
        /* [in] */ Int32 nextTop);

    ECO_LOCAL CARAPI_(AutoPtr<IView>) FillUp(
        /* [in] */ Int32 pos,
        /* [in] */ Int32 nextBottom);

    ECO_LOCAL CARAPI_(AutoPtr<IView>) FillFromTop(
        /* [in] */ Int32 nextTop);

    ECO_LOCAL CARAPI_(AutoPtr<IView>) FillFromMiddle(
        /* [in] */ Int32 childrenTop,
        /* [in] */ Int32 childrenBottom);

    ECO_LOCAL CARAPI_(void) FillAboveAndBelow(
        /* [in] */ IView* sel,
        /* [in] */ Int32 position);

    ECO_LOCAL CARAPI_(AutoPtr<IView>) FillFromSelection(
        /* [in] */ Int32 selectedTop,
        /* [in] */ Int32 childrenTop,
        /* [in] */ Int32 childrenBottom);

    ECO_LOCAL CARAPI_(Int32) GetBottomSelectionPixel(
        /* [in] */ Int32 childrenBottom,
        /* [in] */ Int32 fadingEdgeLength,
        /* [in] */ Int32 selectedPosition);

    ECO_LOCAL CARAPI_(Int32) GetTopSelectionPixel(
        /* [in] */ Int32 childrenTop,
        /* [in] */ Int32 fadingEdgeLength,
        /* [in] */ Int32 selectedPosition);

    ECO_LOCAL CARAPI_(AutoPtr<IView>) MoveSelection(
        /* [in] */ IView* oldSel,
        /* [in] */ IView* newSel,
        /* [in] */ Int32 delta,
        /* [in] */ Int32 childrenTop,
        /* [in] */ Int32 childrenBottom);

    ECO_LOCAL CARAPI_(void) MeasureScrapChild(
        /* [in] */ IView* child,
        /* [in] */ Int32 position,
        /* [in] */ Int32 widthMeasureSpec);

    ECO_LOCAL CARAPI_(AutoPtr<IView>) FillSpecific(
        /* [in] */ Int32 position,
        /* [in] */ Int32 top);

    ECO_LOCAL CARAPI_(void) CorrectTooHigh(
        /* [in] */ Int32 childCount);

    ECO_LOCAL CARAPI_(void) CorrectTooLow(
        /* [in] */ Int32 childCount);

    ECO_LOCAL CARAPI_(Boolean) IsDirectChildHeaderOrFooter(
        /* [in] */ IView* child);

    ECO_LOCAL CARAPI_(AutoPtr<IView>) MakeAndAddView(
        /* [in] */ Int32 position,
        /* [in] */ Int32 y,
        /* [in] */ Boolean flow,
        /* [in] */ Int32 childrenLeft,
        /* [in] */ Boolean selected);

    ECO_LOCAL CARAPI_(void) SetupChild(
        /* [in] */ IView* child,
        /* [in] */ Int32 position,
        /* [in] */ Int32 y,
        /* [in] */ Boolean flowDown,
        /* [in] */ Int32 childrenLeft,
        /* [in] */ Boolean selected,
        /* [in] */ Boolean recycled);

    ECO_LOCAL CARAPI_(Boolean) CommonKey(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event);

    ECO_LOCAL CARAPI HandleHorizontalFocusWithinListItem(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* rst);

    /**
     * Used by {@link #arrowScrollImpl(int)} to help determine the next selected position
     * to move to. This return a position in the direction given if the selected item
     * is fully visible.
     *
     * @param selectedView Current selected view to move from
     * @param selectedPos Current selected position to move from
     * @param direction Direction to move in
     * @return Desired selected position after moving in the given direction
     */
    ECO_LOCAL CARAPI_(Int32) NextSelectedPositionForDirection(
        /* [in] */ IView* selectedView,
        /* [in] */ Int32 selectedPos,
        /* [in] */ Int32 direction);

    ECO_LOCAL CARAPI_(Boolean) ArrowScrollImpl(
        /* [in] */ Int32 direction);

    ECO_LOCAL CARAPI HandleNewSelectionChange(
        /* [in] */ IView* selectedView,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 newSelectedPosition,
        /* [in] */ Boolean newFocusAssigned);

    ECO_LOCAL CARAPI_(void) MeasureAndAdjustDown(
        /* [in] */ IView* child,
        /* [in] */ Int32 childIndex,
        /* [in] */ Int32 numChildren);

    ECO_LOCAL CARAPI_(void) MeasureItem(
        /* [in] */ IView* child);

    ECO_LOCAL CARAPI_(void) RelayoutMeasuredItem(
        /* [in] */ IView* child);

    ECO_LOCAL CARAPI_(Int32) GetArrowScrollPreviewLength();

    ECO_LOCAL CARAPI_(Int32) AmountToScroll(
        /* [in] */ Int32 direction,
        /* [in] */ Int32 nextSelectedPosition);

    ECO_LOCAL CARAPI_(Int32) LookForSelectablePositionOnScreen(
        /* [in] */ Int32 direction);

    ECO_LOCAL CARAPI_(AutoPtr<ArrowScrollFocusResult>) ArrowScrollFocused(
        /* [in] */ Int32 direction);

    ECO_LOCAL CARAPI PositionOfNewFocus(
        /* [in] */ IView* newFocus,
        /* [out] */ Int32* focus);

    ECO_LOCAL CARAPI_(Boolean) IsViewAncestorOf(
        /* [in] */ IView* child,
        /* [in] */ IView* parent);

    ECO_LOCAL CARAPI_(Int32) AmountToScrollToNewFocus(
        /* [in] */ Int32 direction,
        /* [in] */ IView* newFocus,
        /* [in] */ Int32 positionOfNewFocus);

    ECO_LOCAL CARAPI_(Int32) DistanceToView(
        /* [in] */ IView* descendant);

    ECO_LOCAL CARAPI_(void) ScrollListItemsBy(
        /* [in] */ Int32 amount);

    ECO_LOCAL CARAPI_(AutoPtr<IView>) AddViewAbove(
        /* [in] */ IView* theView,
        /* [in] */ Int32 position);

    ECO_LOCAL CARAPI_(AutoPtr<IView>) AddViewBelow(
        /* [in] */ IView* theView,
        /* [in] */ Int32 position);

    ECO_LOCAL CARAPI_(Int32) GetMaxScrollAmount();

    ECO_LOCAL CARAPI_(Boolean) ShouldAdjustHeightForDivider(
        /* [in] */ Int32 itemIndex);

public:
    /**
    * Used to indicate a no preference for a position type.
    */
    static const Int32 NO_POSITION;

protected:
    AutoPtr<IDrawable> mDivider;
    Int32 mDividerHeight;

    AutoPtr<IDrawable> mOverScrollHeader;
    AutoPtr<IDrawable> mOverScrollFooter;

private:
    /**
     * When arrow scrolling, ListView will never scroll more than this factor
     * times the height of the list.
     */
    ECO_LOCAL static const Float MAX_SCROLL_FACTOR;

    /**
     * When arrow scrolling, need a certain amount of pixels to preview next
     * items.  This is usually the fading edge, but if that is small enough,
     * we want to make sure we preview at least this many pixels.
     */
    ECO_LOCAL static const Int32 MIN_SCROLL_PREVIEW_PIXELS;

    ECO_LOCAL static const String LISTVIEW_NAME;

    AutoPtr<IArrayList> mHeaderViewInfos;
    AutoPtr<IArrayList> mFooterViewInfos;

    Boolean mIsCacheColorOpaque;
    Boolean mDividerIsOpaque;

    Boolean mHeaderDividersEnabled;
    Boolean mFooterDividersEnabled;

    Boolean mAreAllItemsSelectable;

    Boolean mItemsCanFocus;

    // used for temporary calculations.
    AutoPtr<IRect> mTempRect;
    AutoPtr<IPaint> mDividerPaint;

    // the single allocated result per list view; kinda cheesey but avoids
    // allocating these thingies too often.
    AutoPtr<ArrowScrollFocusResult> mArrowScrollFocusResult;

    // Keeps focused children visible through resizes
    AutoPtr<FocusSelector> mFocusSelector;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WIDGET_LISTVIEW_H__
