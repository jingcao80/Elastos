
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUITEMIMPL_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUITEMIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IActionProvider;
using Elastos::Droid::View::IContextMenuInfo;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IOnActionExpandListener;
using Elastos::Droid::View::IOnMenuItemClickListener;
using Elastos::Droid::View::ISubMenu;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IVisibilityListener;
using Elastos::Core::IRunnable;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

class MenuBuilder;

/**
 * @hide
 */
class MenuItemImpl
    : public Object
    , public IMenuItemImpl
    , public IMenuItem
{
private:
    class VisibilityListener
        : public Object
        , public IVisibilityListener
    {
    public:
        CAR_INTERFACE_DECL()

        VisibilityListener(
            /* [in] */ MenuItemImpl* host);

        CARAPI OnActionProviderVisibilityChanged(
            /* [in] */ Boolean isVisible);

    private:
        MenuItemImpl* mHost;
    };

public:
    MenuItemImpl();

    CAR_INTERFACE_DECL()

    /**
     * Instantiates this menu item.
     *
     * @param menu
     * @param group Item ordering grouping control. The item will be added after
     *            all other items whose order is <= this number, and before any
     *            that are larger than it. This can also be used to define
     *            groups of items for batch state changes. Normally use 0.
     * @param id Unique item ID. Use 0 if you do not need a unique ID.
     * @param categoryOrder The ordering for this item.
     * @param title The text to display for the item.
     */
    CARAPI constructor(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ Int32 group,
        /* [in] */ Int32 id,
        /* [in] */ Int32 categoryOrder,
        /* [in] */ Int32 ordering,
        /* [in] */ ICharSequence* title,
        /* [in] */ Int32 showAsAction);

    /**
     * Invokes the item by calling various listeners or callbacks.
     *
     * @return true if the invocation was handled, false otherwise
     */
    CARAPI Invoke(
        /* [out] */ Boolean* handled);

    CARAPI IsEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetGroupId(
        /* [out] */ Int32* groupId);

    CARAPI GetItemId(
        /* [out] */ Int32* id);

    CARAPI GetOrder(
        /* [out] */ Int32* order);

    CARAPI GetOrdering(
        /* [out] */ Int32* ordering);

    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    CARAPI SetIntent(
        /* [in] */ IIntent* intent);

    CARAPI_(AutoPtr<IRunnable>) GetCallback();

    CARAPI SetCallback(
        /* [in] */ IRunnable* callback);

    CARAPI GetAlphabeticShortcut(
        /* [out] */ Char32* shortcut);

    CARAPI SetAlphabeticShortcut(
        /* [in] */ Char32 alphaChar);

    CARAPI GetNumericShortcut(
        /* [out] */ Char32* shortcut);

    CARAPI SetNumericShortcut(
        /* [in] */ Char32 numericChar);

    CARAPI SetShortcut(
        /* [in] */ Char32 numericChar,
        /* [in] */ Char32 alphaChar);

    /**
     * @return The active shortcut (based on QWERTY-mode of the menu).
     */
    CARAPI GetShortcut(
        /* [out] */ Char32* c);

    /**
     * @return The label to show for the shortcut. This includes the chording
     *         key (for example 'Menu+a'). Also, any non-human readable
     *         characters should be human readable (for example 'Menu+enter').
     */
    CARAPI GetShortcutLabel(
        /* [out] */ String* label);

    /**
     * @return Whether this menu item should be showing shortcuts (depends on
     *         whether the menu should show shortcuts and whether this item has
     *         a shortcut defined)
     */
    CARAPI ShouldShowShortcut(
        /* [out] */ Boolean* has);

    CARAPI GetSubMenu(
        /* [out] */ ISubMenu** subMenu);

    CARAPI HasSubMenu(
        /* [out] */ Boolean* has);

    CARAPI SetSubMenu(
        /* [in] */ ISubMenuBuilder* subMenu);

    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    /**
     * Gets the title for a particular {@link ItemView}
     *
     * @param itemView The ItemView that is receiving the title
     * @return Either the title or condensed title based on what the ItemView
     *         prefers
     */
    CARAPI GetTitleForItemView(
        /* [in] */ IMenuItemView* itemView,
        /* [out] */ ICharSequence** title);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetTitle(
        /* [in] */ Int32 title);

    CARAPI GetTitleCondensed(
        /* [out] */ ICharSequence** title);

    CARAPI SetTitleCondensed(
        /* [in] */ ICharSequence* title);

    CARAPI GetIcon(
        /* [out] */ IDrawable** drawable);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI SetIcon(
        /* [in] */ Int32 iconResId);

    CARAPI IsCheckable(
        /* [out] */ Boolean* checkable);

    CARAPI SetCheckable(
        /* [in] */ Boolean checkable);

    CARAPI SetExclusiveCheckable(
        /* [in] */ Boolean exclusive);

    CARAPI IsExclusiveCheckable(
        /* [out] */ Boolean* checkable);

    CARAPI IsChecked(
        /* [out] */ Boolean* checked) ;

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    CARAPI SetCheckedInt(
        /* [in] */ Boolean checked);

    CARAPI IsVisible(
        /* [out] */ Boolean* isVisible);

    /**
     * Changes the visibility of the item. This method DOES NOT notify the
     * parent menu of a change in this item, so this should only be called from
     * methods that will eventually trigger this change.  If unsure, use {@link #setVisible(Boolean)}
     * instead.
     *
     * @param shown Whether to show (true) or hide (false).
     * @return Whether the item's shown state was changed
     */
    CARAPI SetVisibleInt(
        /* [in] */ Boolean show,
        /* [out] */ Boolean* isVisible);

    CARAPI SetVisible(
        /* [in] */ Boolean shown);

    CARAPI SetOnMenuItemClickListener(
        /* [in] */ IOnMenuItemClickListener* clickListener);

    //@Override
    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI SetMenuInfo(
        /* [in] */ IContextMenuInfo* menuInfo);

    CARAPI GetMenuInfo(
        /* [out] */ IContextMenuInfo** menuInfo);

    CARAPI ActionFormatChanged();

    /**
     * @return Whether the given menu type should show icons for menu items.
     */
    CARAPI ShouldShowIcon(
        /* [out] */ Boolean* shown);

    CARAPI IsActionButton(
        /* [out] */ Boolean* action);

    CARAPI RequestsActionButton(
        /* [out] */ Boolean* result);

    CARAPI RequiresActionButton(
        /* [out] */ Boolean* result);

    CARAPI SetIsActionButton(
        /* [in] */ Boolean isActionButton);

    CARAPI ShowsTextAsAction(
        /* [out] */ Boolean* shows);

    CARAPI SetShowAsAction(
        /* [in] */ Int32 actionEnum);

    CARAPI SetActionView(
        /* [in] */ IView* view);

    CARAPI SetActionView(
        /* [in] */ Int32 resId);

    CARAPI GetActionView(
        /* [out] */ IView** view);

    CARAPI GetActionProvider(
        /* [out] */ IActionProvider** provider);

    CARAPI SetActionProvider(
        /* [in] */ IActionProvider* actionProvider);

    CARAPI SetShowAsActionFlags(
        /* [in] */ Int32 actionEnum);

    CARAPI ExpandActionView(
        /* [out] */ Boolean* expand);

    CARAPI CollapseActionView(
        /* [out] */ Boolean* collapse);

    CARAPI SetOnActionExpandListener(
        /* [out] */ IOnActionExpandListener* listener);

    CARAPI HasCollapsibleActionView(
        /* [out] */ Boolean* has);

    CARAPI SetActionViewExpanded(
        /* [in] */ Boolean isExpanded);

    CARAPI IsActionViewExpanded(
        /* [out] */ Boolean* expanded);

private:
    static const String TAG;
    static const Int32 SHOW_AS_ACTION_MASK;

    static const Int32 CHECKABLE;
    static const Int32 CHECKED;
    static const Int32 EXCLUSIVE;
    static const Int32 HIDDEN;
    static const Int32 ENABLED;
    static const Int32 IS_ACTION;
    /** Used for the icon resource ID if this item does not have an icon */
    static const Int32 NO_ICON;

    static String sLanguage;
    static String sPrependShortcutLabel;
    static String sEnterShortcutLabel;
    static String sDeleteShortcutLabel;
    static String sSpaceShortcutLabel;

    Int32 mId;
    Int32 mGroup;
    Int32 mCategoryOrder;
    Int32 mOrdering;
    AutoPtr<ICharSequence> mTitle;
    AutoPtr<ICharSequence> mTitleCondensed;
    AutoPtr<IIntent> mIntent;
    Char32 mShortcutNumericChar;
    Char32 mShortcutAlphabeticChar;

    /** The icon's drawable which is only created as needed */
    AutoPtr<IDrawable> mIconDrawable;
    /**
     * The icon's resource ID which is used to get the Drawable when it is
     * needed (if the Drawable isn't already obtained--only one of the two is
     * needed).
     */
    Int32 mIconResId;

    /** The menu to which this item belongs */
    MenuBuilder* mMenu;    // mMenu has this's reference
    /** If this item should launch a sub menu, this is the sub menu to launch */
    AutoPtr<ISubMenuBuilder> mSubMenu;

    AutoPtr<IRunnable> mItemCallback;
    AutoPtr<IOnMenuItemClickListener> mClickListener;

    Int32 mFlags;

    Int32 mShowAsAction/* = SHOW_AS_ACTION_NEVER*/;

    AutoPtr<IView> mActionView;
    AutoPtr<IActionProvider> mActionProvider;
    AutoPtr<IOnActionExpandListener> mOnActionExpandListener;
    Boolean mIsActionViewExpanded;

    /**
     * Current use case is for context menu: Extra information linked to the
     * View that added this item to the context menu.
     */
    AutoPtr<IContextMenuInfo> mMenuInfo;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif    //__ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUITEMIMPL_H__
