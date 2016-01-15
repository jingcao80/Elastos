
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_ICONMENUVIEW_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_ICONMENUVIEW_H__

#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::View;
using Elastos::Droid::View::ViewGroup;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

class IconMenuView
    : public ViewGroup
    , public IIconMenuView
    , public IMenuBuilderItemInvoker
    , public IMenuView
    , public IRunnable
{
    friend class IconMenuPresenter;
public:
    class SavedState
        : public View::BaseSavedState
        , public IIconMenuViewSavedState
    {
    public:
        SavedState();

        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* superState,
            /* [in] */ Int32 focusedPosition);

        CARAPI SetFocusedPosition(
            /* [in] */ Int32 focusedPosition);

        CARAPI GetFocusedPosition(
            /* [out] */ Int32* focusedPosition);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel *source);

        CARAPI WriteToParcel(
            /* [in] */ IParcel *dest);

    private:
        Int32 mFocusedPosition;
    };

    /**
     * Layout parameters specific to IconMenuView (stores the left, top, right, bottom from the
     * measure pass).
     */
    class LayoutParams
        : public ViewGroup::MarginLayoutParams
        , public IIconMenuViewLayoutParams
    {
    public:
        LayoutParams();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SetLeft(
            /* [in] */ Int32 left);

        CARAPI GetLeft(
            /* [out] */ Int32* left);

        CARAPI SetTop(
            /* [in] */ Int32 top);

        CARAPI GetTop(
            /* [out] */ Int32* top);

        CARAPI SetRight(
            /* [in] */ Int32 right);

        CARAPI GetRight(
            /* [out] */ Int32* right);

        CARAPI SetBottom(
            /* [in] */ Int32 bottom);

        CARAPI GetBottom(
            /* [out] */ Int32* bottom);

        CARAPI SetDesiredWidth(
            /* [in] */ Int32 desiredWidth);

        CARAPI GetDesiredWidth(
            /* [out] */ Int32* desiredWidth);

        CARAPI SetMaxNumItemsOnRow(
            /* [in] */ Int32 maxNumItemsOnRow);

        CARAPI GetMaxNumItemsOnRow(
            /* [out] */ Int32* maxNumItemsOnRow);

    public:
        Int32 mLeft;
        Int32 mTop;
        Int32 mRight;
        Int32 mBottom;
        Int32 mDesiredWidth;
        Int32 mMaxNumItemsOnRow;
    };

private:
    class OnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnClickListener(
            /* [in] */ IconMenuView* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        IconMenuView* mHost;
    };

public:
    IconMenuView();

    CAR_INTERFACE_DECL()

    /**
     * Instantiates the IconMenuView that is linked with the provided MenuBuilder.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(Int32) GetMaxItems();

    CARAPI Initialize(
        /* [in] */ IMenuBuilder* menu);

    CARAPI InvokeItem(
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* state);

    //@override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    /**
     * Marks as having stale children.
     */
    CARAPI_(void) MarkStaleChildren();

    /**
     * @return The number of actual items shown (those that are backed by an
     *         {@link MenuView.ItemView} implementation--eg: excludes More
     *         item).
     */
    CARAPI_(Int32) GetNumActualItemsShown();

    CARAPI_(void) SetNumActualItemsShown(
        /* [in] */ Int32 count);

    CARAPI GetWindowAnimations(
        /* [out] */ Int32* animations);

    /**
     * Returns the number of items per row.
     * <p>
     * This should only be used for testing.
     *
     * @return The length of the array is the number of rows. A value at a
     *         position is the number of items in that row.
     * @hide
     */
    CARAPI GetLayout(
        /* [out, callee] */ ArrayOf<Int32>** layout);

    /**
     * Returns the number of rows in the layout.
     * <p>
     * This should only be used for testing.
     *
     * @return The length of the array is the number of rows. A value at a
     *         position is the number of items in that row.
     * @hide
     */
    CARAPI GetLayoutNumRows(
        /* [out] */ Int32* num);

    //@override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    /**
     * When this method is invoked if the menu is currently not being
     * longpressed, it means that the longpress has just been reached (so we set
     * longpress flag, and start cycling). If it is being longpressed, we cycle
     * to the next mode.
     */
    CARAPI Run();

protected:
    //@override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //@override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    //@override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@override
    CARAPI OnAttachedToWindow();

    //@override
    CARAPI OnDetachedFromWindow();

    //@override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    //@override
    CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    CARAPI_(AutoPtr<IDrawable>) GetItemBackgroundDrawable();

    /**
     * Creates the item view for the 'More' button which is used to switch to
     * the expanded menu view. This button is a special case since it does not
     * have a MenuItemData backing it.
     * @return The CIconMenuItemView for the 'More' button
     */
    CARAPI_(AutoPtr<IIconMenuItemView>) CreateMoreItemView();

private:
    /**
     * Figures out the layout for the menu items.
     *
     * @param width The available width for the icon menu.
     */
    CARAPI_(void) LayoutItems(
        /* [in] */ Int32 width);

    /**
     * Figures out the layout for the menu items by equally distributing, and
     * adding any excess items equally to lower rows.
     *
     * @param numRows The total number of rows for the menu view
     * @param numItems The total number of items (across all rows) contained in
     *            the menu view
     * @return int[] Where the value of index i contains the number of items for row i
     */
    CARAPI_(void) LayoutItemsUsingGravity(
        /* [in] */ Int32 numRows,
        /* [in] */ Int32 numItems);

    /**
     * Checks whether each item's title is fully visible using the current
     * layout.
     *
     * @return True if the items fit (each item's text is fully visible), false
     *         otherwise.
     */
    CARAPI_(Boolean) DoItemsFit();

    /**
     * The positioning algorithm that gets called from onMeasure.  It
     * just computes positions for each child, and then stores them in the child's layout params.
     * @param menuWidth The width of this menu to assume for positioning
     * @param menuHeight The height of this menu to assume for positioning
     */
    CARAPI_(void) PositionChildren(
        /* [in] */ Int32 menuWidth,
        /* [in] */ Int32 menuHeight);

    /**
     * Sets the shortcut caption mode for IconMenuView. This mode will
     * continuously cycle between a child's shortcut and its title.
     *
     * @param cycleShortcutAndNormal Whether to go into cycling shortcut mode,
     *        or to go back to normal.
     */
    CARAPI_(void) SetCycleShortcutCaptionMode(
        /* [in] */ Boolean cycleShortcutAndNormal);

    /**
     * Iterates children and sets the desired shortcut mode. Only
     * {@link #setCycleShortcutCaptionMode(boolean)} and {@link #run()} should call
     * this.
     *
     * @param shortcut Whether to show shortcut or the title.
     */
    CARAPI_(void) SetChildrenCaptionMode(
        /* [in] */ Boolean shortcut);

    /**
     * For each item, calculates the most dense row that fully shows the item's
     * title.
     *
     * @param width The available width of the icon menu.
     */
    CARAPI_(void) CalculateItemFittingMetadata(
        /* [in] */ Int32 width);

private:
    static const Int32 ITEM_CAPTION_CYCLE_DELAY = 1000;

    AutoPtr<IMenuBuilder> mMenu;

    /** Height of each row */
    Int32 mRowHeight;
    /** Maximum number of rows to be shown */
    Int32 mMaxRows;
    /** Maximum number of items to show in the icon menu. */
    Int32 mMaxItems;
    /** Maximum number of items per row */
    Int32 mMaxItemsPerRow;
    /** Actual number of items (the 'More' view does not count as an item) shown */
    Int32 mNumActualItemsShown;

    /** Divider that is drawn between all rows */
    AutoPtr<IDrawable> mHorizontalDivider;
    /** Height of the horizontal divider */
    Int32 mHorizontalDividerHeight;

    /** Set of horizontal divider positions where the horizontal divider will be drawn */
    List<AutoPtr<IRect> > mHorizontalDividerRects;

    /** Divider that is drawn between all columns */
    AutoPtr<IDrawable> mVerticalDivider;
    /** Width of the vertical divider */
    Int32 mVerticalDividerWidth;
    /** Set of vertical divider positions where the vertical divider will be drawn */
    List<AutoPtr<IRect> > mVerticalDividerRects;

    /** Icon for the 'More' button */
    AutoPtr<IDrawable> mMoreIcon;

    /** Background of each item (should contain the selected and focused states) */
    AutoPtr<IDrawable> mItemBackground;

    /** Default animations for this menu */
    Int32 mAnimations;

    /**
     * Whether this IconMenuView has stale children and needs to update them.
     * Set true by {@link #markStaleChildren()} and reset to false by
     * {@link #onMeasure(int, int)}
     */
    Boolean mHasStaleChildren;

    /**
     * Longpress on MENU (while this is shown) switches to shortcut caption
     * mode. When the user releases the longpress, we do not want to pass the
     * key-up event up since that will dismiss the menu.
     */
    Boolean mMenuBeingLongpressed;

    /**
     * While {@link #mMenuBeingLongpressed}, we toggle the children's caption
     * mode between each's title and its shortcut. This is the last caption mode
     * we broadcasted to children.
     */
    Boolean mLastChildrenCaptionMode;

    /**
     * The layout to use for menu items. Each index is the row number (0 is the
     * top-most). Each value contains the number of items in that row.
     * <p>
     * The length of this array should not be used to get the number of rows in
     * the current layout, instead use {@link #mLayoutNumRows}.
     */
    AutoPtr<ArrayOf<Int32> > mLayout;

    /**
     * The number of rows in the current layout.
     */
    Int32 mLayoutNumRows;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_MENU_ICONMENUVIEW_H__
