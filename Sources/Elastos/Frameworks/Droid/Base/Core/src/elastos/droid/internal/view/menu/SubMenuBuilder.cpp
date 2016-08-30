
#include "elastos/droid/internal/view/menu/SubMenuBuilder.h"

using Elastos::Droid::View::EIID_ISubMenu;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CAR_INTERFACE_IMPL_2(SubMenuBuilder, MenuBuilder, ISubMenuBuilder, ISubMenu)

ECode SubMenuBuilder::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* parentMenu,
    /* [in] */ IMenuItemImpl* item)
{
    FAIL_RETURN(MenuBuilder::constructor(context))
    mParentMenu = (MenuBuilder*)parentMenu;
    mItem = item;
    return NOERROR;
}

ECode SubMenuBuilder::SetQwertyMode(
        /* [in] */ Boolean isQwerty)
{
    return mParentMenu->SetQwertyMode(isQwerty);
}

ECode SubMenuBuilder::IsQwertyMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mParentMenu->IsQwertyMode();
    return NOERROR;
}

ECode SubMenuBuilder::SetShortcutsVisible(
    /* [in] */ Boolean shortcutsVisible)
{
    return mParentMenu->SetShortcutsVisible(shortcutsVisible);
}

ECode SubMenuBuilder::IsShortcutsVisible(
    /* [out] */ Boolean* result)
{
    return mParentMenu->IsShortcutsVisible(result);
}

ECode SubMenuBuilder::GetParentMenu(
    /* [out] */ IMenu** menu)
{
    VALIDATE_NOT_NULL(menu)
    *menu = IMenu::Probe(mParentMenu);
    REFCOUNT_ADD(*menu)
    return NOERROR;
}

ECode SubMenuBuilder::GetItem(
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    *item = IMenuItem::Probe(mItem);
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode SubMenuBuilder::SetCallback(
    /* [in] */ IMenuBuilderCallback* callback)
{
    return mParentMenu->SetCallback(callback);
}

ECode SubMenuBuilder::GetRootMenu(
    /* [out] */ IMenuBuilder** rootMenu)
{
    VALIDATE_NOT_NULL(rootMenu)
    *rootMenu = mParentMenu;
    REFCOUNT_ADD(*rootMenu)
    return NOERROR;
}

Boolean SubMenuBuilder::DispatchMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item)
{
    return MenuBuilder::DispatchMenuItemSelected(menu, item) ||
         ((MenuBuilder*)mParentMenu)->DispatchMenuItemSelected(menu, item);
}

ECode SubMenuBuilder::SetIcon(
    /* [in] */ IDrawable* icon)
{
    return IMenuItem::Probe(mItem)->SetIcon(icon);
}

ECode SubMenuBuilder::SetIcon(
    /* [in] */ Int32 iconRes)
{
    return IMenuItem::Probe(mItem)->SetIcon(iconRes);
}

ECode SubMenuBuilder::SetHeaderIcon(
    /* [in] */ IDrawable* icon)
{
    return MenuBuilder::SetHeaderIconInt(icon);
}

ECode SubMenuBuilder::SetHeaderIcon(
    /* [in] */ Int32 iconRes)
{
    return MenuBuilder::SetHeaderIconInt(iconRes);
}

ECode SubMenuBuilder::SetHeaderTitle(
    /* [in] */ ICharSequence* title)
{
    return MenuBuilder::SetHeaderTitleInt(title);
}

ECode SubMenuBuilder::SetHeaderTitle(
    /* [in] */ Int32 titleRes)
{
    return MenuBuilder::SetHeaderTitleInt(titleRes);
}

ECode SubMenuBuilder::SetHeaderView(
    /* [in] */ IView* view)
{
    return MenuBuilder::SetHeaderViewInt(view);
}

ECode SubMenuBuilder::ExpandItemActionView(
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    return mParentMenu->ExpandItemActionView(item, result);
}

ECode SubMenuBuilder::CollapseItemActionView(
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    return mParentMenu->CollapseItemActionView(item, result);
}

ECode SubMenuBuilder::GetActionViewStatesKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key)
    Int32 itemId = 0;
    if (mItem != NULL) {
        IMenuItem::Probe(mItem)->GetItemId(&itemId);
    }

    if (itemId == 0) {
        *key = NULL;
        return NOERROR;
    }

    MenuBuilder::GetActionViewStatesKey(key);
    key->AppendFormat(":%d", itemId);
    return NOERROR;
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
