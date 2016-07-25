
#ifndef __ELASTOS_DROID_WIDGET_ACTIONMENUVIEW_H__
#define __ELASTOS_DROID_WIDGET_ACTIONMENUVIEW_H__


#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/widget/ActionMenuPresenter.h"

using Elastos::Droid::View::IMenu;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::IActionMenuViewLayoutParams;
using Elastos::Droid::Widget::IActionMenuViewOnMenuItemClickListener;
using Elastos::Droid::Internal::View::Menu::IMenuView;
using Elastos::Droid::Internal::View::Menu::IMenuBuilderItemInvoker;
using Elastos::Droid::Internal::View::Menu::IMenuBuilderCallback;

namespace Elastos {
namespace Droid {
namespace Widget {

class ActionMenuView
    : public LinearLayout
    , public IActionMenuView
    , public IMenuView
    , public IMenuBuilderItemInvoker
{
public:
    class ActionMenuViewLayoutParams
        : public LinearLayout::LinearLayoutLayoutParams
        , public IActionMenuViewLayoutParams
    {
    public:
        CAR_INTERFACE_DECL()

        ActionMenuViewLayoutParams();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI constructor(
            /* [in] */ IActionMenuViewLayoutParams* source);

        CARAPI constructor(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Boolean isOverflowButton);

        CARAPI SetIsOverflowButton(
            /* [in] */ Boolean isOverflow);

        CARAPI GetIsOverflowButton(
            /* [out] */ Boolean* isOverflow);

        CARAPI SetCellsUsed(
            /* [in] */ Int32 cellsUsed);

        CARAPI GetCellsUsed(
            /* [out] */ Int32* cellsUsed);

        CARAPI SetExtraPixels(
            /* [in] */ Int32 extraPixels);

        CARAPI GetExtraPixels(
            /* [out] */ Int32* extraPixels);

        CARAPI SetExpandable(
            /* [in] */ Boolean expandable);

        CARAPI GetExpandable(
            /* [out] */ Boolean* expandable);

        CARAPI SetPreventEdgeOffset(
            /* [in] */ Boolean preventEdgeOffset);

        CARAPI GetPreventEdgeOffset(
            /* [out] */ Boolean* preventEdgeOffset);

        CARAPI SetExpanded(
            /* [in] */ Boolean expanded);

        CARAPI GetExpanded(
            /* [out] */ Boolean* expanded);
    private:
        Boolean mIsOverflowButton;
        Int32 mCellsUsed;
        Int32 mExtraPixels;
        Boolean mExpandable;
        Boolean mPreventEdgeOffset;
        Boolean mExpanded;
    };

private:
    class MenuBuilderCallback
        : public Object
        , public IMenuBuilderCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MenuBuilderCallback(
            /* [in] */ ActionMenuView* host);

        CARAPI OnMenuItemSelected(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* state);

        /**
         * Called when the mode of the menu changes (for example, from icon to expanded).
         *
         * @param menu the menu that has changed modes
         */
        CARAPI OnMenuModeChange(
            /* [in] */ IMenuBuilder* menu);

    private:
        ActionMenuView* mHost;
    };

    class ActionMenuPresenterCallback
        : public Object
        , public IMenuPresenterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

        CARAPI OnOpenSubMenu(
            /* [in] */ IMenuBuilder* subMenu,
            /* [out] */ Boolean* handle);
    };

public:
    CAR_INTERFACE_DECL()

    ActionMenuView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetPopupTheme(
        /* [in] */ Int32 resId);

    CARAPI GetPopupTheme(
        /* [out] */ Int32* resId);

    CARAPI SetPresenter(
        /* [in] */ IActionMenuPresenter* presenter);

    CARAPI SetOnMenuItemClickListener(
        /* [in] */ IActionMenuViewOnMenuItemClickListener* listener);
    /**
     * Measure a child view to fit within cell-based formatting. The child's width
     * will be measured to a whole multiple of cellSize.
     *
     * <p>Sets the expandable and cellsUsed fields of LayoutParams.
     *
     * @param child Child to measure
     * @param cellSize Size of one cell
     * @param cellsRemaining Number of cells remaining that this view can expand to fill
     * @param parentHeightMeasureSpec MeasureSpec used by the parent view
     * @param parentHeightPadding Padding present in the parent view
     * @return Number of cells this child was measured to occupy
     */
    static CARAPI_(Int32) MeasureChildForCells(
        /* [in] */ IView* child,
        /* [in] */ Int32 cellSize,
        /* [in] */ Int32 cellsRemaining,
        /* [in] */ Int32 parentHeightMeasureSpec,
        /* [in] */ Int32 parentHeightPadding);

    virtual CARAPI OnDetachedFromWindow();

    CARAPI IsOverflowReserved(
        /* [out] */ Boolean* isOverflowReserved);

    CARAPI SetOverflowReserved(
        /* [in] */ Boolean reserveOverflow);

    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** lp);

    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    CARAPI GenerateOverflowButtonLayoutParams(
        /* [out] */ IActionMenuViewLayoutParams** amlp);

    CARAPI InvokeItem(
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    CARAPI GetWindowAnimations(
        /* [out] */ Int32* animations);

    CARAPI Initialize(
        /* [in] */ IMenuBuilder* menu);

        /**
         * Returns the Menu object that this ActionMenuView is currently presenting.
         *
         * <p>Applications should use this method to obtain the ActionMenuView's Menu object
         * and inflate or add content to it as necessary.</p>
         *
         * @return the Menu presented by this view
         */
    CARAPI GetMenu(
        /* [out] */ IMenu** menu);

        /**
         * Must be called before the first call to getMenu()
         * @hide
         */
    CARAPI SetMenuCallbacks(
        /* [in] */ IMenuPresenterCallback* pcb,
        /* [in] */ IMenuBuilderCallback* mcb);

        /**
         * Returns the current menu or null if one has not yet been configured.
         * @hide Internal use only for action bar integration
         */
    CARAPI PeekMenu(
        /* [out] */ IMenuBuilder** build);

        /**
         * Show the overflow items from the associated menu.
         *
         * @return true if the menu was able to be shown, false otherwise
         */
    CARAPI ShowOverflowMenu(
        /* [out] */ Boolean* showOverflowMenu);

        /**
         * Hide the overflow items from the associated menu.
         *
         * @return true if the menu was able to be hidden, false otherwise
         */
    CARAPI HideOverflowMenu(
        /* [out] */ Boolean* hideOverflowMenu);

        /**
         * Check whether the overflow menu is currently showing. This may not reflect
         * a pending show operation in progress.
         *
         * @return true if the overflow menu is currently showing
         */
    CARAPI IsOverflowMenuShowing(
        /* [out] */ Boolean* isOverflowMenuShowing);

        /** @hide */
    CARAPI IsOverflowMenuShowPending(
        /* [out] */ Boolean* isOverflowMenuShowPending);


        /**
         * Dismiss any popups associated with this menu view.
         */
    CARAPI DismissPopupMenus();

    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

        /** @hide */
    CARAPI SetExpandedActionViewsExclusive(
        /* [in] */ Boolean exclusive);


protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    //@Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    //@Override
    CARAPI_(Boolean) HasDividerBeforeChildAt(
        /* [in] */ Int32 childIndex);

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    CARAPI_(void) InitActionMenu(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(void) OnMeasureExactFormat(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(AutoPtr<IActionMenuPresenter>) GetPresenter();

public:
    static const Int32 MIN_CELL_SIZE; // dips
    static const Int32 GENERATED_ITEM_PADDING; // dips

private:
    static String TAG;

    AutoPtr<IMenuBuilder> mMenu;
    /** Context against which to inflate popup menus. */
    AutoPtr<IContext> mPopupContext;

    /** Theme resource against which to inflate popup menus. */
    Int32 mPopupTheme;
    Boolean mReserveOverflow;
    AutoPtr<IWeakReference> mPresenter;
    AutoPtr<IMenuPresenterCallback> mActionMenuPresenterCallback;
    AutoPtr<IMenuBuilderCallback> mMenuBuilderCallback;
    Boolean mFormatItems;
    Int32 mFormatItemsWidth;
    Int32 mMinCellSize;
    Int32 mGeneratedItemPadding;
    Int32 mMeasuredExtraWidth;
    Int32 mMaxItemHeight;
    AutoPtr<IActionMenuViewOnMenuItemClickListener> mOnMenuItemClickListener;

    friend class ActionMenuPresenter;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_WIDGET_ACTIONMENUVIEW_H__
