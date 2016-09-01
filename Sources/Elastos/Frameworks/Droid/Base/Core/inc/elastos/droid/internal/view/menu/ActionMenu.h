
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_ACTIONMENU_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_ACTIONMENU_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ISubMenu;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

/**
 * @hide
 */
class ActionMenu
    : public Object
    , public IActionMenu
    , public IMenu
{
public:
    ActionMenu();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI_(AutoPtr<IContext>) GetContext();

    CARAPI Add(
        /* [in] */ ICharSequence* title,
        /* [out] */ IMenuItem** item);

    CARAPI Add(
        /* [in] */ Int32 titleRes,
        /* [out] */ IMenuItem** item);

    CARAPI Add(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ Int32 titleRes,
        /* [out] */ IMenuItem** item);

    CARAPI Add(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ ICharSequence* title,
        /* [out] */ IMenuItem** item);

    CARAPI AddIntentOptions(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ IComponentName* caller,
        /* [in] */ ArrayOf<IIntent*>* specifics,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ ArrayOf<IMenuItem*>* outSpecificItems,
        /* [out] */ Int32* count);

    CARAPI AddSubMenu(
        /* [in] */ ICharSequence* title,
        /* [out] */ ISubMenu** menu);

    CARAPI AddSubMenu(
        /* [in] */ Int32 titleRes,
        /* [out] */ ISubMenu** menu);

    CARAPI AddSubMenu(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ ICharSequence* title,
        /* [out] */ ISubMenu** menu);

    CARAPI AddSubMenu(
        /* [in] */ Int32 groupId,
        /* [in] */ Int32 itemId,
        /* [in] */ Int32 order,
        /* [in] */ Int32 titleRes,
        /* [out] */ ISubMenu** menu);

    CARAPI Clear();

    CARAPI Close();

    CARAPI FindItem(
        /* [in] */ Int32 id,
        /* [out] */ IMenuItem** item);

    CARAPI GetItem(
        /* [in] */ Int32 index,
        /* [out] */ IMenuItem** item);

    CARAPI HasVisibleItems(
        /* [out] */ Boolean* has);

    CARAPI IsShortcutKey(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* isShort);

    CARAPI PerformIdentifierAction(
        /* [in] */ Int32 id,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* perform);

    CARAPI PerformShortcut(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* perform);

    CARAPI RemoveGroup(
        /* [in] */ Int32 groupId);

    CARAPI RemoveItem(
        /* [in] */ Int32 id);

    CARAPI SetGroupCheckable(
        /* [in] */ Int32 group,
        /* [in] */ Boolean checkable,
        /* [in] */ Boolean exclusive);

    CARAPI SetGroupEnabled(
        /* [in] */ Int32 group,
        /* [in] */ Boolean enabled);

    CARAPI SetGroupVisible(
        /* [in] */ Int32 group,
        /* [in] */ Boolean visible);

    CARAPI SetQwertyMode(
        /* [in] */ Boolean isQwerty);

    CARAPI GetSize(
        /* [out] */ Int32* size);

private:
    CARAPI_(Int32) FindItemIndex(
        /* [in] */ Int32 id);

    CARAPI_(AutoPtr<IActionMenuItem>) FindItemWithShortcut(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

private:
    IContext* mContext;

    Boolean mIsQwerty;

    List<AutoPtr<IActionMenuItem> > mItems;
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INTERNAL_VIEW_MENU_ACTIONMENU_H__
