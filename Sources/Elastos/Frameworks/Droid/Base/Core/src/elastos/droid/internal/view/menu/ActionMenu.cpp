
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/internal/view/menu/ActionMenu.h"
#include "elastos/droid/internal/view/menu/CActionMenuItem.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CComponentName.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::EIID_IMenu;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CAR_INTERFACE_IMPL_2(ActionMenu, Object, IActionMenu, IMenu)

ActionMenu::ActionMenu()
    : mContext(NULL)
    , mIsQwerty(FALSE)
{
}

ECode ActionMenu::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

AutoPtr<IContext> ActionMenu::GetContext()
{
    return mContext;
}

ECode ActionMenu::Add(
    /* [in] */ ICharSequence* title,
    /* [out] */ IMenuItem** item)
{
    return Add(0, 0, 0, title, item);
}

ECode ActionMenu::Add(
    /* [in] */ Int32 titleRes,
    /* [out] */ IMenuItem** item)
{
    return Add(0, 0, 0, titleRes, item);
}

ECode ActionMenu::Add(
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 itemId,
    /* [in] */ Int32 order,
    /* [in] */ Int32 titleRes,
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    assert(res != NULL);

    String str;
    res->GetString(titleRes, &str);

    AutoPtr<ICharSequence> title = CoreUtils::Convert(str);
    return Add(groupId, itemId, order, title, item);
}

ECode ActionMenu::Add(
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 itemId,
    /* [in] */ Int32 order,
    /* [in] */ ICharSequence* title,
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    CActionMenuItem::New(GetContext(), groupId, itemId, 0, order, title, item);
    mItems.Insert(order, IActionMenuItem::Probe(*item));
    return NOERROR;
}

ECode ActionMenu::AddIntentOptions(
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 itemId,
    /* [in] */ Int32 order,
    /* [in] */ IComponentName* caller,
    /* [in] */ ArrayOf<IIntent*>* specifics,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<IMenuItem*>* outSpecificItems,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IList> lri;
    pm->QueryIntentActivityOptions(caller, specifics, intent, 0, (IList**)&lri);
    Int32 N = 0;
    if (lri != NULL) {
        lri->GetSize(&N);
    }

    if ((flags & FLAG_APPEND_TO_GROUP) == 0) {
        RemoveGroup(groupId);
    }

    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> elem;
        lri->Get(i, (IInterface**)&elem);
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(elem);
        assert(ri != NULL);

        AutoPtr<IIntent> rintent;
        Int32 index = 0;
        ri->GetSpecificIndex(&index);
        CIntent::New(index < 0 ? intent : (*specifics)[index], (IIntent**)&rintent);

        AutoPtr<IActivityInfo> info;
        ri->GetActivityInfo((IActivityInfo**)&info);
        assert(info != NULL);

        String name;
        IPackageItemInfo::Probe(info)->GetName(&name);

        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        assert(appInfo != NULL);

        String packageName;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);

        AutoPtr<IComponentName> cName;
        CComponentName::New(packageName, name, (IComponentName**)&cName);

        rintent->SetComponent(cName);
        AutoPtr<ICharSequence> label;
        ri->LoadLabel(pm, (ICharSequence**)&label);
        AutoPtr<IMenuItem> item;
        Add(groupId, itemId, order, label, (IMenuItem**)&item);

        AutoPtr<IDrawable> icon;
        ri->LoadIcon(pm, (IDrawable**)&icon);
        item->SetIcon(icon);
        item->SetIntent(rintent);

        if (outSpecificItems != NULL && index >= 0) {
            outSpecificItems->Set(index, item);
        }
    }

    *count = N;
    return NOERROR;
}

ECode ActionMenu::AddSubMenu(
    /* [in] */ ICharSequence* title,
    /* [out] */ ISubMenu** menu)
{
    VALIDATE_NOT_NULL(menu)
    *menu = NULL;
    // TODO Implement submenus
    return NOERROR;
}

ECode ActionMenu::AddSubMenu(
    /* [in] */ Int32 titleRes,
    /* [out] */ ISubMenu** menu)
{
    VALIDATE_NOT_NULL(menu)
    *menu = NULL;
    // TODO Implement submenus
    return NOERROR;
}

ECode ActionMenu::AddSubMenu(
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 itemId,
    /* [in] */ Int32 order,
    /* [in] */ ICharSequence* title,
    /* [out] */ ISubMenu** menu)
{
    VALIDATE_NOT_NULL(menu)
    *menu = NULL;
    // TODO Implement submenus
    return NOERROR;
}

ECode ActionMenu::AddSubMenu(
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 itemId,
    /* [in] */ Int32 order,
    /* [in] */ Int32 titleRes,
    /* [out] */ ISubMenu** menu)
{
    VALIDATE_NOT_NULL(menu)
    *menu = NULL;
    // TODO Implement submenus
    return NOERROR;
}

ECode ActionMenu::Clear()
{
    mItems.Clear();
    return NOERROR;
}

ECode ActionMenu::Close()
{
    return NOERROR;
}

Int32 ActionMenu::FindItemIndex(
    /* [in] */ Int32 id)
{
    List<AutoPtr<IActionMenuItem> >::Iterator ator = mItems.Begin();
    Int32 index = 0;
    for (; ator != mItems.End(); ++ator, index++) {
        AutoPtr<IActionMenuItem> item = *ator;
        assert(item != NULL);
        Int32 _id = 0;
        IMenuItem::Probe(item)->GetItemId(&_id);
        if (_id == id) {
            return index;
        }
    }

    return -1;
}

ECode ActionMenu::FindItem(
    /* [in] */ Int32 id,
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    *item = IMenuItem::Probe(mItems[FindItemIndex(id)]);
    REFCOUNT_ADD(*item);

    return NOERROR;
}

ECode ActionMenu::GetItem(
    /* [in] */ Int32 index,
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    *item = IMenuItem::Probe(mItems[index]);
    REFCOUNT_ADD(*item);

    return NOERROR;
}

ECode ActionMenu::HasVisibleItems(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has)
    List<AutoPtr<IActionMenuItem> >::Iterator ator = mItems.Begin();
    for (; ator != mItems.End(); ++ator) {
        Boolean visible = FALSE;
        if (IMenuItem::Probe(*ator)->IsVisible(&visible), visible) {
            *has = TRUE;
            return NOERROR;
        }
    }

    *has = FALSE;
    return NOERROR;
}

AutoPtr<IActionMenuItem> ActionMenu::FindItemWithShortcut(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    // TODO Make this smarter.
    Boolean qwerty = mIsQwerty;
    List<AutoPtr<IActionMenuItem> >::Iterator ator = mItems.Begin();
    for (; ator != mItems.End(); ++ator) {
        AutoPtr<IActionMenuItem> item = *ator;
        Char32 shortcut = 0;
        if (qwerty) {
            IMenuItem::Probe(item)->GetAlphabeticShortcut(&shortcut);
        }
        else {
            IMenuItem::Probe(item)->GetNumericShortcut(&shortcut);
        }

        if (keyCode == (Int32)shortcut) {
            return item;
        }
    }

    return NULL;
}

ECode ActionMenu::IsShortcutKey(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* isShort)
{
    VALIDATE_NOT_NULL(isShort)
    *isShort = FindItemWithShortcut(keyCode, event) != NULL;
    return NOERROR;
}

ECode ActionMenu::PerformIdentifierAction(
    /* [in] */ Int32 id,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* perform)
{
    VALIDATE_NOT_NULL(perform)
    const Int32 index = FindItemIndex(id);
    if (index < 0) {
        *perform = FALSE;
        return NOERROR;
    }

    return mItems[index]->Invoke(perform);
}

ECode ActionMenu::PerformShortcut(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* perform)
{
    VALIDATE_NOT_NULL(perform)
    AutoPtr<IActionMenuItem> item = FindItemWithShortcut(keyCode, event);
    if (item == NULL) {
        *perform = FALSE;
        return NOERROR;
    }
    return item->Invoke(perform);
}

ECode ActionMenu::RemoveGroup(
    /* [in] */ Int32 groupId)
{
    List<AutoPtr<IActionMenuItem> >::Iterator ator = mItems.Begin();
    for (; ator != mItems.End();) {
        Int32 id = 0;
        IMenuItem::Probe(*ator)->GetGroupId(&id);

        if (id == groupId) {
            ator = mItems.Erase(ator);
        }
        else {
            ator++;
        }
    }

    return NOERROR;
}

ECode ActionMenu::RemoveItem(
    /* [in] */ Int32 id)
{
    mItems.Remove(FindItemIndex(id));
    return NOERROR;
}

ECode ActionMenu::SetGroupCheckable(
    /* [in] */ Int32 group,
    /* [in] */ Boolean checkable,
    /* [in] */ Boolean exclusive)
{
    List<AutoPtr<IActionMenuItem> >::Iterator ator = mItems.Begin();
    for (; ator != mItems.End(); ++ator) {
        AutoPtr<IActionMenuItem> item = *ator;
        assert(item != NULL);
        Int32 id = 0;
        IMenuItem::Probe(item)->GetGroupId(&id);

        if (id == group) {
            IMenuItem::Probe(item)->SetCheckable(checkable);
            item->SetExclusiveCheckable(exclusive);
        }
    }

    return NOERROR;
}

ECode ActionMenu::SetGroupEnabled(
    /* [in] */ Int32 group,
    /* [in] */ Boolean enabled)
{
    List<AutoPtr<IActionMenuItem> >::Iterator ator = mItems.Begin();
    for (; ator != mItems.End(); ++ator) {
        AutoPtr<IActionMenuItem> item = *ator;
        assert(item != NULL);

        Int32 id = 0;
        IMenuItem::Probe(item)->GetGroupId(&id);

        if (id == group) {
            IMenuItem::Probe(item)->SetEnabled(enabled);
        }
    }

    return NOERROR;
}

ECode ActionMenu::SetGroupVisible(
    /* [in] */ Int32 group,
    /* [in] */ Boolean visible)
{
    List<AutoPtr<IActionMenuItem> >::Iterator ator = mItems.Begin();
    for (; ator != mItems.End(); ++ator) {
        AutoPtr<IActionMenuItem> item = *ator;
        assert(item != NULL);

        Int32 id = 0;
        IMenuItem::Probe(item)->GetGroupId(&id);

        if (id == group) {
            IMenuItem::Probe(item)->SetVisible(visible);
        }
    }

    return NOERROR;
}

ECode ActionMenu::SetQwertyMode(
    /* [in] */ Boolean isQwerty)
{
    mIsQwerty = isQwerty;
    return NOERROR;
}

ECode ActionMenu::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mItems.GetSize();
    return NOERROR;
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
