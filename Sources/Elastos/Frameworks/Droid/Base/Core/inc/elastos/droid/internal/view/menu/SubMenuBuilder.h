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
    MenuBuilder* mParentMenu;
    AutoPtr<IMenuItemImpl> mItem;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INTERNAL_VIEW_MENU_SUBMENUBUILDER_H__
