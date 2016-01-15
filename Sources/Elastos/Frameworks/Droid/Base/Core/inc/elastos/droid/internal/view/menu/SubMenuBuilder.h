
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_SUBMENUBUILDER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_SUBMENUBUILDER_H__

#include "elastos/droid/internal/view/menu/MenuBuilder.h"

using Elastos::Droid::View::ISubMenu;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

/**
 * The model for a sub menu, which is an extension of the menu.  Most methods are proxied to
 * the parent menu.
 */
class SubMenuBuilder
    : public MenuBuilder
    , public ISubMenuBuilder
    , public ISubMenu
{
public:
    using MenuBuilder::GetItem;

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IMenuBuilder* parentMenu,
        /* [in] */ IMenuItemImpl* item);

    //@override
    CARAPI SetQwertyMode(
        /* [in] */ Boolean isQwerty);

    //@override
    CARAPI IsQwertyMode(
        /* [out] */ Boolean* result);

    //@override
    CARAPI SetShortcutsVisible(
        /* [in] */ Boolean shortcutsVisible);

    //@override
    CARAPI IsShortcutsVisible(
        /* [out] */ Boolean* result);

    CARAPI GetParentMenu(
        /* [out] */ IMenu** menu);

    CARAPI GetItem(
        /* [out] */ IMenuItem** item);

    //@override
    CARAPI SetCallback(
        /* [in] */ IMenuBuilderCallback* callback);

    //@override
    CARAPI GetRootMenu(
        /* [out] */ IMenuBuilder** rootMenu);

    CARAPI_(Boolean) DispatchMenuItemSelected(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IMenuItem* item);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI SetIcon(
        /* [in] */ Int32 iconRes);

    CARAPI SetHeaderIcon(
        /* [in] */ IDrawable* icon);

    CARAPI SetHeaderIcon(
        /* [in] */ Int32 iconRes);

    CARAPI SetHeaderTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetHeaderTitle(
        /* [in] */ Int32 titleRes);

    CARAPI SetHeaderView(
        /* [in] */ IView* view);

    CARAPI ExpandItemActionView(
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    CARAPI CollapseItemActionView(
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    CARAPI GetActionViewStatesKey(
        /* [out] */ String* key);

private:
    AutoPtr<MenuBuilder> mParentMenu;
    AutoPtr<IMenuItemImpl> mItem;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INTERNAL_VIEW_MENU_SUBMENUBUILDER_H__
