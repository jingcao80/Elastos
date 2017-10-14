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

#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUBUILDER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUBUILDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
// #include "elastos/droid/widget/BaseAdapter.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IContextMenuInfo;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ISubMenu;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::ICopyOnWriteArrayList;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

typedef List<AutoPtr<IMenuItemImpl> > MenuItemImplList;

class MenuBuilder
    : public Object
    , public IMenuBuilder
    , public IMenu
{
public:
    CAR_INTERFACE_DECL()

    MenuBuilder();

    ~MenuBuilder();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetDefaultShowAsAction(
        /* [in] */ Int32 defaultShowAsAction);

    CARAPI AddMenuPresenter(
        /* [in] */ IMenuPresenter* presenter);

    CARAPI AddMenuPresenter(
        /* [in] */ IMenuPresenter* presenter,
        /* [in] */ IContext* menuContext);

    CARAPI RemoveMenuPresenter(
        /* [in] */ IMenuPresenter* presenter);

    CARAPI SavePresenterStates(
        /* [in] */ IBundle* outState);

    CARAPI RestorePresenterStates(
        /* [in] */ IBundle* state);

    CARAPI SaveActionViewStates(
        /* [in] */ IBundle* outStates);

    CARAPI RestoreActionViewStates(
        /* [in] */ IBundle* states);

    CARAPI SetCallback(
        /* [in] */ IMenuBuilderCallback* callback);

    CARAPI GetCallback(
        /* [out] */ IMenuBuilderCallback** callback);

    /**
     * Add a new item to the menu. This item displays the given title for its
     * label.
     *
     * @param title The text to display for the item.
     * @return The newly added menu item.
     */
    CARAPI Add(
        /* [in] */ ICharSequence* title,
        /* [out] */ IMenuItem** item);

    /**
     * Add a new item to the menu. This item displays the given title for its
     * label.
     *
     * @param titleRes Resource identifier of title string.
     * @return The newly added menu item.
     */
    CARAPI Add(
        /* [in] */ Int32 titleRes,
        /* [out] */ IMenuItem** item);

    /**
     * Add a new item to the menu. This item displays the given title for its
     * label.
     *
     * @param groupId The group identifier that this item should be part of.
     *        This can be used to define groups of items for batch state
     *        changes. Normally use {@link #NONE} if an item should not be in a
     *        group.
     * @param itemId Unique item ID. Use {@link #NONE} if you do not need a
     *        unique ID.
     * @param order The order for the item. Use {@link #NONE} if you do not care
     *        about the order. See {@link MenuItem#getOrder()}.
     * @param title The text to display for the item.
     * @return The newly added menu item.
     */
    CARAPI Add(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ ICharSequence* title,
        /* [out] */ IMenuItem** item);

    /**
     * Variation on {@link #add(int, int, int, CharSequence)} that takes a
     * string resource identifier instead of the string itself.
     *
     * @param groupId The group identifier that this item should be part of.
     *        This can also be used to define groups of items for batch state
     *        changes. Normally use {@link #NONE} if an item should not be in a
     *        group.
     * @param itemId Unique item ID. Use {@link #NONE} if you do not need a
     *        unique ID.
     * @param order The order for the item. Use {@link #NONE} if you do not care
     *        about the order. See {@link MenuItem#getOrder()}.
     * @param titleRes Resource identifier of title string.
     * @return The newly added menu item.
     */
    CARAPI Add(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ Int32 titleRes,
        /* [out] */ IMenuItem** item);

    /**
     * Add a new sub-menu to the menu. This item displays the given title for
     * its label. To modify other attributes on the submenu's menu item, use
     * {@link SubMenu#getItem()}.
     *
     * @param title The text to display for the item.
     * @return The newly added sub-menu
     */
    CARAPI AddSubMenu(
        /* [in] */ ICharSequence* title,
        /* [out] */ ISubMenu** subMenu);

    /**
     * Add a new sub-menu to the menu. This item displays the given title for
     * its label. To modify other attributes on the submenu's menu item, use
     * {@link SubMenu#getItem()}.
     *
     * @param titleRes Resource identifier of title string.
     * @return The newly added sub-menu
     */
    CARAPI AddSubMenu(
        /* [in] */ Int32 titleRes,
        /* [out] */ ISubMenu** subMenu);

    /**
     * Add a new sub-menu to the menu. This item displays the given
     * <var>title</var> for its label. To modify other attributes on the
     * submenu's menu item, use {@link SubMenu#getItem()}.
     *<p>
     * Note that you can only have one level of sub-menus, i.e. you cannnot add
     * a subMenu to a subMenu: An {@link UnsupportedOperationException} will be
     * thrown if you try.
     *
     * @param groupId The group identifier that this item should be part of.
     *        This can also be used to define groups of items for batch state
     *        changes. Normally use {@link #NONE} if an item should not be in a
     *        group.
     * @param itemId Unique item ID. Use {@link #NONE} if you do not need a
     *        unique ID.
     * @param order The order for the item. Use {@link #NONE} if you do not care
     *        about the order. See {@link MenuItem#getOrder()}.
     * @param title The text to display for the item.
     * @return The newly added sub-menu
     */
    CARAPI AddSubMenu(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ ICharSequence* title,
        /* [out] */ ISubMenu** subMenu);

    /**
     * Variation on {@link #addSubMenu(int, int, int, CharSequence)} that takes
     * a string resource identifier for the title instead of the string itself.
     *
     * @param groupId The group identifier that this item should be part of.
     *        This can also be used to define groups of items for batch state
     *        changes. Normally use {@link #NONE} if an item should not be in a group.
     * @param itemId Unique item ID. Use {@link #NONE} if you do not need a unique ID.
     * @param order The order for the item. Use {@link #NONE} if you do not care about the
     *        order. See {@link MenuItem#getOrder()}.
     * @param titleRes Resource identifier of title string.
     * @return The newly added sub-menu
     */
    CARAPI AddSubMenu(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ Int32 titleRes,
        /* [out] */ ISubMenu** subMenu);

    /**
     * Add a group of menu items corresponding to actions that can be performed
     * for a particular Intent. The Intent is most often configured with a null
     * action, the data that the current activity is working with, and includes
     * either the {@link Intent#CATEGORY_ALTERNATIVE} or
     * {@link Intent#CATEGORY_SELECTED_ALTERNATIVE} to find activities that have
     * said they would like to be included as optional action. You can, however,
     * use any Intent you want.
     *
     * <p>
     * See {@link android.content.pm.PackageManager#queryIntentActivityOptions}
     * for more * details on the <var>caller</var>, <var>specifics</var>, and
     * <var>intent</var> arguments. The list returned by that function is used
     * to populate the resulting menu items.
     *
     * <p>
     * All of the menu items of possible options for the intent will be added
     * with the given group and id. You can use the group to control ordering of
     * the items in relation to other items in the menu. Normally this function
     * will automatically remove any existing items in the menu in the same
     * group and place a divider above and below the added items; this behavior
     * can be modified with the <var>flags</var> parameter. For each of the
     * generated items {@link MenuItem#setIntent} is called to associate the
     * appropriate Intent with the item; this means the activity will
     * automatically be started for you without having to do anything else.
     *
     * @param groupId The group identifier that the items should be part of.
     *        This can also be used to define groups of items for batch state
     *        changes. Normally use {@link #NONE} if the items should not be in
     *        a group.
     * @param itemId Unique item ID. Use {@link #NONE} if you do not need a
     *        unique ID.
     * @param order The order for the items. Use {@link #NONE} if you do not
     *        care about the order. See {@link MenuItem#getOrder()}.
     * @param caller The current activity component name as defined by
     *        queryIntentActivityOptions().
     * @param specifics Specific items to place first as defined by
     *        queryIntentActivityOptions().
     * @param intent Intent describing the kinds of items to populate in the
     *        list as defined by queryIntentActivityOptions().
     * @param flags Additional options controlling how the items are added.
     * @param outSpecificItems Optional array in which to place the menu items
     *        that were generated for each of the <var>specifics</var> that were
     *        requested. Entries may be null if no activity was found for that
     *        specific action.
     * @return The number of menu items that were added.
     *
     * @see #FLAG_APPEND_TO_GROUP
     * @see MenuItem#setIntent
     * @see android.content.pm.PackageManager#queryIntentActivityOptions
     */
    CARAPI AddIntentOptions(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ IComponentName* caller,
        /* [in] */ ArrayOf<IIntent*>* specifics,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ ArrayOf<IMenuItem*>* outSpecificItems,
        /* [out] */ Int32* result);

    /**
     * Remove the item with the given identifier.
     *
     * @param id The item to be removed.  If there is no item with this
     *           identifier, nothing happens.
     */
    CARAPI RemoveItem(
        /* [in] */ Int32 id);

    /**
     * Remove all items in the given group.
     *
     * @param groupId The group to be removed.  If there are no items in this
     *           group, nothing happens.
     */
    CARAPI RemoveGroup(
        /* [in] */ Int32 groupId);

    CARAPI RemoveItemAt(
        /* [in] */ Int32 index);

    CARAPI ClearAll();

    /**
     * Remove all existing items from the menu, leaving it empty as if it had
     * just been created.
     */
    CARAPI Clear();

    /**
     * Control whether a particular group of items can show a check mark.  This
     * is similar to calling {@link MenuItem#setCheckable} on all of the menu items
     * with the given group identifier, but in addition you can control whether
     * this group contains a mutually-exclusive set items.  This should be called
     * after the items of the group have been added to the menu.
     *
     * @param group The group of items to operate on.
     * @param checkable Set to true to allow a check mark, false to
     *                  disallow.  The default is false.
     * @param exclusive If set to true, only one item in this group can be
     *                  checked at a time; checking an item will automatically
     *                  uncheck all others in the group.  If set to false, each
     *                  item can be checked independently of the others.
     *
     * @see MenuItem#setCheckable
     * @see MenuItem#setChecked
     */
    CARAPI SetGroupCheckable(
        /* [in] */ Int32 group,
        /* [in] */ Boolean checkable,
        /* [in] */ Boolean exclusive);

    /**
     * Show or hide all menu items that are in the given group.
     *
     * @param group The group of items to operate on.
     * @param visible If true the items are visible, else they are hidden.
     *
     * @see MenuItem#setVisible
     */
    CARAPI SetGroupVisible(
        /* [in] */ Int32 group,
        /* [in] */ Boolean visible);

    /**
     * Enable or disable all menu items that are in the given group.
     *
     * @param group The group of items to operate on.
     * @param enabled If true the items will be enabled, else they will be disabled.
     *
     * @see MenuItem#setEnabled
     */
    CARAPI SetGroupEnabled(
        /* [in] */ Int32 group,
        /* [in] */ Boolean enabled);

    /**
     * Return whether the menu currently has item items that are visible.
     *
     * @return True if there is one or more item visible,
     *         else false.
     */
    CARAPI HasVisibleItems(
        /* [out] */ Boolean* result);

    /**
     * Return the menu item with a particular identifier.
     *
     * @param id The identifier to find.
     *
     * @return The menu item object, or null if there is no item with
     *         this identifier.
     */
    CARAPI FindItem(
        /* [in] */ Int32 id,
        /* [out] */ IMenuItem** item);

    CARAPI FindItemIndex(
        /* [in] */ Int32 id,
        /* [out] */ Int32* index);

    CARAPI FindGroupIndex(
        /* [in] */ Int32 group,
        /* [out] */ Int32* index);

    CARAPI FindGroupIndex(
        /* [in] */ Int32 group,
        /* [in] */ Int32 start,
        /* [out] */ Int32* index);

    /**
     * Get the number of items in the menu.  Note that this will change any
     * times items are added or removed from the menu.
     *
     * @return The item count.
     */
    CARAPI GetSize(
        /* [out] */ Int32* size);

    /**
     * Gets the menu item at the given index.
     *
     * @param index The index of the menu item to return.
     * @return The menu item.
     * @exception IndexOutOfBoundsException
     *                when {@code index < 0 || >= size()}
     */
    CARAPI GetItem(
        /* [in] */ Int32 index,
        /* [out] */ IMenuItem** item);

    /**
     * Is a keypress one of the defined shortcut keys for this window.
     * @param keyCode the key code from {@link KeyEvent} to check.
     * @param event the {@link KeyEvent} to use to help check.
     */
    CARAPI IsShortcutKey(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Control whether the menu should be running in qwerty mode (alphabetic
     * shortcuts) or 12-key mode (numeric shortcuts).
     *
     * @param isQwerty If true the menu will use alphabetic shortcuts; else it
     *                 will use numeric shortcuts.
     */
    CARAPI SetQwertyMode(
        /* [in] */ Boolean isQwerty);

    /**
     * Sets whether the shortcuts should be visible on menus.  Devices without hardware
     * key input will never make shortcuts visible even if this method is passed 'true'.
     *
     * @param shortcutsVisible Whether shortcuts should be visible (if true and a
     *            menu item does not have a shortcut defined, that item will
     *            still NOT show a shortcut)
     */
    CARAPI SetShortcutsVisible(
        /* [in] */ Boolean shortcutsVisible);

    /**
     * @return Whether shortcuts should be visible on menus.
     */
    CARAPI IsShortcutsVisible(
        /* [out] */ Boolean* result);

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI ChangeMenuMode();

    /**
     * Execute the menu item action associated with the given shortcut
     * character.
     *
     * @param keyCode The keycode of the shortcut key.
     * @param event Key event message.
     * @param flags Additional option flags or 0.
     *
     * @return If the given shortcut exists and is shown, returns
     *         true; else returns false.
     *
     * @see #FLAG_PERFORM_NO_CLOSE
     */
    CARAPI PerformShortcut(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* result);

    /**
     * Execute the menu item action associated with the given menu identifier.
     *
     * @param id Identifier associated with the menu item.
     * @param flags Additional option flags or 0.
     *
     * @return If the given identifier exists and is shown, returns
     *         true; else returns false.
     *
     * @see #FLAG_PERFORM_NO_CLOSE
     */
    CARAPI PerformIdentifierAction(
        /* [in] */ Int32 id,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* result);

    CARAPI PerformItemAction(
        /* [in] */ IMenuItem* item,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* result);

    CARAPI PerformItemAction(
        /* [in] */ IMenuItem* item,
        /* [in] */ IMenuPresenter* preferredPresenter,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* result);

    /**
     * Closes the visible menu.
     *
     * @param allMenusAreClosing Whether the menus are completely closing (true),
     *            or whether there is another menu coming in this menu's place
     *            (false). For example, if the menu is closing because a
     *            sub menu is about to be shown, <var>allMenusAreClosing</var>
     *            is false.
     */
    CARAPI Close(
        /* [in] */ Boolean allMenusAreClosing);

    /** {@inheritDoc} */
    CARAPI Close();

    /**
     * Called when an item is added or removed.
     *
     * @param structureChanged true if the menu structure changed,
     *                         false if only item properties changed.
     *                         (Visibility is a structural property since it affects layout.)
     */
    CARAPI OnItemsChanged(
        /* [in] */ Boolean cleared);

    CARAPI StopDispatchingItemsChanged();

    CARAPI StartDispatchingItemsChanged();

    CARAPI GetVisibleItems(
        /* [out] */ IArrayList** items);

    CARAPI FlagActionItems();

    CARAPI GetActionItems(
        /* [out] */ IArrayList** items);

    CARAPI GetNonActionItems(
        /* [out] */ IArrayList** items);

    CARAPI ClearHeader();

    CARAPI GetHeaderTitle(
        /* [out] */ ICharSequence** headerTitle);

    CARAPI GetHeaderIcon(
        /* [out] */ IDrawable** headerIcon);

    CARAPI GetHeaderView(
        /* [out] */ IView** headerView);

    /**
     * Gets the root menu (if this is a submenu, find its root menu).
     * @return The root menu.
     */
    CARAPI GetRootMenu(
        /* [out] */ IMenuBuilder** rootMenu);

    /**
     * Sets the current menu info that is set on all items added to this menu
     * (until this is called again with different menu info, in which case that
     * one will be added to all subsequent item additions).
     *
     * @param menuInfo The extra menu information to add.
     */
    CARAPI SetCurrentMenuInfo(
        /* [in] */ IContextMenuInfo* menuInfo);

    CARAPI ExpandItemActionView(
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    CARAPI CollapseItemActionView(
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    CARAPI GetExpandedItem(
        /* [out] */ IMenuItemImpl** item);


public:
    // not public API
    virtual CARAPI SetExclusiveItemChecked(
        /* [in] */ IMenuItem* item);

    /**
     * @return whether the menu shortcuts are in qwerty mode or not
     */
    virtual CARAPI IsQwertyMode(
        /* [out] */ Boolean* qwertyMode);

    virtual CARAPI_(AutoPtr<IResources>) GetResources();

    virtual CARAPI_(Boolean) DispatchMenuItemSelected(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IMenuItem* item);

    /*
     * This function will return all the menu and sub-menu items that can
     * be directly (the shortcut directly corresponds) and indirectly
     * (the ALT-enabled char corresponds to the shortcut) associated
     * with the keyCode.
     */
    virtual CARAPI_(void) FindItemsWithShortcutForKey(
        /* [in, out] */ MenuItemImplList* list,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    /*
     * We want to return the menu item associated with the key, but if there is no
     * ambiguity (i.e. there is only one menu item corresponding to the key) we want
     * to return it even if it's not an exact match; this allow the user to
     * _not_ use the ALT key for example, making the use of shortcuts slightly more
     * user-friendly. An example is on the G1, '!' and '1' are on the same key, and
     * in Gmail, Menu+1 will trigger Menu+! (the actual shortcut).
     *
     * On the other hand, if two (or more) shortcuts corresponds to the same key,
     * we have to only return the exact match.
     */
    virtual CARAPI_(AutoPtr<IMenuItemImpl>) FindItemWithShortcutForKey(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    /**
     * Called by {@link CMenuItemImpl*} when its visible flag is changed.
     * @param item The item that has gone through a visibility change.
     */
    virtual CARAPI OnItemVisibleChanged(
        /* [in] */ IMenuItemImpl* item);

    /**
     * Called by {@link MenuItemImpl} when its action request status is changed.
     * @param item The item that has gone through a change in action request status.
     */
    virtual CARAPI OnItemActionRequestChanged(
        /* [in] */ IMenuItemImpl* item);

    virtual CARAPI SetOptionalIconsVisible(
        /* [in] */ Boolean visible);

    virtual CARAPI_(Boolean) GetOptionalIconsVisible();

protected:
    /**
     * Sets the header's title. This replaces the header view. Called by the
     * builder-style methods of subclasses.
     *
     * @param title The new title.
     * @return This MenuBuilder so additional setters can be called.
     */
    virtual CARAPI SetHeaderTitleInt(
        /* [in] */ ICharSequence* title);

    /**
     * Sets the header's title. This replaces the header view. Called by the
     * builder-style methods of subclasses.
     *
     * @param titleRes The new title (as a resource ID).
     * @return This MenuBuilder so additional setters can be called.
     */
    virtual CARAPI SetHeaderTitleInt(
        /* [in] */ Int32 titleRes);

    /**
     * Sets the header's icon. This replaces the header view. Called by the
     * builder-style methods of subclasses.
     *
     * @param icon The new icon.
     * @return This MenuBuilder so additional setters can be called.
     */
    virtual CARAPI SetHeaderIconInt(
        /* [in] */ IDrawable* icon);

    /**
     * Sets the header's icon. This replaces the header view. Called by the
     * builder-style methods of subclasses.
     *
     * @param iconRes The new icon (as a resource ID).
     * @return This MenuBuilder so additional setters can be called.
     */
    virtual CARAPI SetHeaderIconInt(
        /* [in] */ Int32 iconRes);

    /**
     * Sets the header's view. This replaces the title and icon. Called by the
     * builder-style methods of subclasses.
     *
     * @param view The new view.
     * @return This MenuBuilder so additional setters can be called.
     */
    virtual CARAPI SetHeaderViewInt(
        /* [in] */ IView* view);

    virtual CARAPI GetActionViewStatesKey(
        /* [out] */ String* key);

private:
    /**
     * Adds an item to the menu.  The other add methods funnel to this.
     */
    CARAPI_(AutoPtr<IMenuItem>) AddInternal(
        /* [in] */ Int32 group,
        /* [in] */ Int32 id,
        /* [in] */ Int32 categoryOrder,
        /* [in] */ ICharSequence* title);

    // Layoutlib overrides this method to return its custom implementation of MenuItemImpl
    CARAPI_(AutoPtr<IMenuItemImpl>) CreateNewMenuItem(
        /* [in] */ Int32 group,
        /* [in] */ Int32 id,
        /* [in] */ Int32 categoryOrder,
        /* [in] */ Int32 ordering,
        /* [in] */ ICharSequence* title,
        /* [in] */ Int32 defaultShowAsAction);

    /**
     * Remove the item at the given index and optionally forces menu views to
     * update.
     *
     * @param index The index of the item to be removed. If this index is
     *            invalid an exception is thrown.
     * @param updateChildrenOnMenuViews Whether to force update on menu views.
     *            Please make sure you eventually call this after your batch of
     *            removals.
     */
    CARAPI_(void) RemoveItemAtInt(
        /* [in] */ Int32 index,
        /* [in] */ Boolean updateChildrenOnMenuViews);

    /**
     * Returns the ordering across all items. This will grab the category from
     * the upper bits, find out how to order the category with respect to other
     * categories, and combine it with the lower bits.
     *
     * @param categoryOrder The category order for a particular item (if it has
     *            not been or/add with a category, the default category is
     *            assumed).
     * @return An ordering Int32eger that can be used to order this item across
     *         all the items (even from other categories).
     */
    static CARAPI_(Int32) GetOrdering(
        /* [in] */ Int32 categoryOrder);

    static CARAPI_(Int32) FindInsertIndex(
        /* [in] */ MenuItemImplList* items,
        /* [in] */ Int32 ordering);

    CARAPI_(void) SetHeaderInternal(
        /* [in] */ Int32 titleRes,
        /* [in] */ ICharSequence* title,
        /* [in] */ Int32 iconRes,
        /* [in] */ IDrawable* icon,
        /* [in] */ IView* view);

private:
    CARAPI_(void) DispatchPresenterUpdate(
        /* [in] */ Boolean cleared);

    CARAPI_(Boolean) DispatchSubMenuSelected(
        /* [in] */ ISubMenuBuilder* subMenu,
        /* [in] */ IMenuPresenter* preferredPresenter);

    CARAPI_(void) DispatchSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI_(void) DispatchRestoreInstanceState(
        /* [in] */ IBundle* state);

    CARAPI_(void) SetShortcutsVisibleInner(
        /* [in] */ Boolean shortcutsVisible);

public:
    /** Header title for menu types that have a header (context and submenus) */
    AutoPtr<ICharSequence> mHeaderTitle;
    /** Header icon for menu types that have a header and support icons (context) */
    AutoPtr<IDrawable> mHeaderIcon;
    /** Header custom view for menu types that have a header and support custom views (context) */
    AutoPtr<IView> mHeaderView;

private:
    static const String TAG;
    static const String PRESENTER_KEY;
    static const String ACTION_VIEW_STATES_KEY;
    static const String EXPANDED_ACTION_VIEW_ID;

    static const Int32 sCategoryToOrder[6];

    IContext* mContext;
    Boolean mHolderContext;
    AutoPtr<IResources> mResources;

    /**
     * Whether the shortcuts should be qwerty-accessible. Use isQwertyMode()
     * instead of accessing this directly.
     */
    Boolean mQwertyMode;

    /**
     * Whether the shortcuts should be visible on menus. Use isShortcutsVisible()
     * instead of accessing this directly.
     */
    Boolean mShortcutsVisible;

    /**
     * Callback that will receive the various menu-related events generated by
     * this class. Use getCallback to get a reference to the callback.
     */
    AutoPtr<IMenuBuilderCallback> mCallback;

    /** Contains all of the items for this menu */
    AutoPtr<MenuItemImplList> mItems;

    /** Contains only the items that are currently visible.  This will be created/refreshed from
     * {@link #getVisibleItems()} */
    AutoPtr<IArrayList> mVisibleItems;
    /**
     * Whether or not the items (or any one item's shown state) has changed since it was last
     * fetched from {@link #getVisibleItems()}
     */
    Boolean mIsVisibleItemsStale;

    /**
     * Contains only the items that should appear in the Action Bar, if present.
     */
    AutoPtr<IArrayList> mActionItems;
    /**
     * Contains items that should NOT appear in the Action Bar, if present.
     */
    AutoPtr<IArrayList> mNonActionItems;

    /**
     * Whether or not the items (or any one item's action state) has changed since it was
     * last fetched.
     */
    Boolean mIsActionItemsStale;

    /**
     * Default value for how added items should show in the action list.
     */
    Int32 mDefaultShowAsAction;

    /**
     * Current use case is Context Menus: As Views populate the context menu, each one has
     * extra information that should be passed along.  This is the current menu info that
     * should be set on all items added to this menu.
     */
    AutoPtr<IContextMenuInfo> mCurrentMenuInfo;

    /**
     * Contains the state of the View hierarchy for all menu views when the menu
     * was frozen.
     */
    AutoPtr<ISparseArray> mFrozenViewStates;

    /**
     * Prevents onItemsChanged from doing its junk, useful for batching commands
     * that may individually call onItemsChanged.
     */
    Boolean mPreventDispatchingItemsChanged;

    Boolean mItemsChangedWhileDispatchPrevented;

    Boolean mOptionalIconsVisible;

    Boolean mIsClosing;

    AutoPtr<MenuItemImplList> mTempShortcutItemList;

    // In order to use convenient
    // AutoPtr<ICopyOnWriteArrayList> mPresenters;
    AutoPtr<IList> mPresenters; // weak-ref of IMenuPresenter

    /**
     * Currently expanded menu item; must be collapsed when we clear.
     */
    AutoPtr<IMenuItemImpl> mExpandedItem;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif    //__ELASTOS_DROID_INTERNAL_VIEW_MENU_MENUBUILDER_H__
