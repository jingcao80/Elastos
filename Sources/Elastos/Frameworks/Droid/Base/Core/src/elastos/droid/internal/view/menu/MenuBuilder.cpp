
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/view/menu/MenuBuilder.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/internal/view/menu/CSubMenuBuilder.h"
#include "elastos/droid/internal/view/menu/CMenuItemImpl.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/view/CKeyCharacterMap.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::CKeyCharacterMap;
using Elastos::Droid::View::EIID_IMenu;
using Elastos::Droid::View::IActionProvider;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

const String MenuBuilder::TAG("MenuBuilder");
const String MenuBuilder::PRESENTER_KEY("android:menu:presenters");
const String MenuBuilder::ACTION_VIEW_STATES_KEY("android:menu:actionviewstates");
const String MenuBuilder::EXPANDED_ACTION_VIEW_ID("android:menu:expandedactionview");

const Int32  MenuBuilder::sCategoryToOrder[6] = {
    1, /* No category */
    4, /* CONTAINER */
    5, /* SYSTEM */
    3, /* SECONDARY */
    2, /* ALTERNATIVE */
    0, /* SELECTED_ALTERNATIVE */
};

CAR_INTERFACE_IMPL_2(MenuBuilder, Object, IMenuBuilder, IMenu)

MenuBuilder::MenuBuilder()
    : mQwertyMode(FALSE)
    , mShortcutsVisible(FALSE)
    , mIsVisibleItemsStale(TRUE)
    , mDefaultShowAsAction(IMenuItem::SHOW_AS_ACTION_NEVER)
    , mPreventDispatchingItemsChanged(FALSE)
    , mItemsChangedWhileDispatchPrevented(FALSE)
    , mOptionalIconsVisible(FALSE)
    , mIsClosing(FALSE)
{
    mTempShortcutItemList = new MenuItemImplList();
    CCopyOnWriteArrayList::New((IList**)&mPresenters);
}

ECode MenuBuilder::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    context->GetResources((IResources**)&mResources);

    mItems = new MenuItemImplList();

    CArrayList::New((IArrayList**)&mVisibleItems);
    mIsVisibleItemsStale = TRUE;

    CArrayList::New((IArrayList**)&mActionItems);
    CArrayList::New((IArrayList**)&mNonActionItems);
    mIsActionItemsStale = TRUE;

    SetShortcutsVisibleInner(TRUE);
    return NOERROR;
}

ECode MenuBuilder::SetDefaultShowAsAction(
    /* [in] */ Int32 defaultShowAsAction)
{
    mDefaultShowAsAction = defaultShowAsAction;
    return NOERROR;
}

ECode MenuBuilder::AddMenuPresenter(
    /* [in] */ IMenuPresenter* presenter)
{
    return AddMenuPresenter(presenter, mContext);
}

ECode MenuBuilder::AddMenuPresenter(
    /* [in] */ IMenuPresenter* presenter,
    /* [in] */ IContext* menuContext)
{
    VALIDATE_NOT_NULL(presenter);

    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(presenter);
    assert(wrs != NULL);
    AutoPtr<IWeakReference> wr;
    wrs->GetWeakReference((IWeakReference**)&wr);
    mPresenters->Add(wr);
    presenter->InitForMenu(menuContext, this);
    mIsActionItemsStale = TRUE;
    return NOERROR;
}

ECode MenuBuilder::RemoveMenuPresenter(
    /* [in] */ IMenuPresenter* presenter)
{
    Int32 size;
    mPresenters->GetSize(&size);
    for (Int32 i = size -1 ; i >= 0; i--) {
        AutoPtr<IInterface> elem;
        mPresenters->Get(i, (IInterface**)&elem);
        AutoPtr<IMenuPresenter> item;
        IWeakReference::Probe(elem)->Resolve(EIID_IMenuPresenter, (IInterface**)&item);

        if (item == NULL || item.Get() == presenter) {
            mPresenters->Remove(elem);
        }
    }

    return NOERROR;
}

void MenuBuilder::DispatchPresenterUpdate(
    /* [in] */ Boolean cleared)
{
    Boolean isEmpty;
    if (mPresenters->IsEmpty(&isEmpty), isEmpty)
        return;

    StopDispatchingItemsChanged();

    Int32 size;
    mPresenters->GetSize(&size);
    for (Int32 i = 0; i < size;) {
        AutoPtr<IInterface> elem;
        mPresenters->Get(i, (IInterface**)&elem);
        AutoPtr<IMenuPresenter> presenter;
        IWeakReference::Probe(elem)->Resolve(EIID_IMenuPresenter, (IInterface**)&presenter);
        if (presenter == NULL) {
            mPresenters->Remove(elem);
        }
        else {
            presenter->UpdateMenuView(cleared);
            i++;
        }
    }

    StartDispatchingItemsChanged();
}

Boolean MenuBuilder::DispatchSubMenuSelected(
    /* [in] */ ISubMenuBuilder* subMenu,
    /* [in] */ IMenuPresenter* preferredPresenter)
{
    Boolean isEmpty;
    if (mPresenters->IsEmpty(&isEmpty), isEmpty)
        return FALSE;

    Boolean result = FALSE;

    // Try the preferred presenter first.
    if (preferredPresenter != NULL) {
        preferredPresenter->OnSubMenuSelected(subMenu, &result);
    }

    Int32 size;
    mPresenters->GetSize(&size);
    for (Int32 i = 0; i < size;) {
        AutoPtr<IInterface> elem;
        mPresenters->Get(i, (IInterface**)&elem);
        AutoPtr<IMenuPresenter> presenter;
        IWeakReference::Probe(elem)->Resolve(EIID_IMenuPresenter, (IInterface**)&presenter);
        if (presenter == NULL) {
            mPresenters->Remove(elem);
        }
        else if (!result) {
            presenter->OnSubMenuSelected(subMenu, &result);
            i++;
        }
        else {
            i++;
        }
    }

    return result;
}

void MenuBuilder::DispatchSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    assert(outState != NULL);
    Boolean isEmpty;
    if (mPresenters->IsEmpty(&isEmpty), isEmpty)
        return;

    AutoPtr<ISparseArray> presenterStates;
    CSparseArray::New((ISparseArray**)&presenterStates);

    Int32 size;
    mPresenters->GetSize(&size);
    for (Int32 i = 0; i < size;) {
        AutoPtr<IInterface> elem;
        mPresenters->Get(i, (IInterface**)&elem);
        AutoPtr<IMenuPresenter> presenter;
        IWeakReference::Probe(elem)->Resolve(EIID_IMenuPresenter, (IInterface**)&presenter);
        if (presenter == NULL) {
            mPresenters->Remove(elem);
        }
        else {
            i++;
            Int32 id = 0;
            presenter->GetId(&id);
            if (id > 0) {
                AutoPtr<IParcelable> state;
                presenter->OnSaveInstanceState((IParcelable**)&state);
                if (state != NULL) {
                    presenterStates->Put(id, state);
                }
            }
        }
    }

    outState->PutSparseParcelableArray(PRESENTER_KEY, presenterStates);
}

void MenuBuilder::DispatchRestoreInstanceState(
    /* [in] */ IBundle* state)
{
    assert(state != NULL);
    AutoPtr<ISparseArray> presenterStates;
    state->GetSparseParcelableArray(PRESENTER_KEY, (ISparseArray**)&presenterStates);

    Boolean isEmpty;
    if (presenterStates == NULL || (mPresenters->IsEmpty(&isEmpty), isEmpty))
        return;

    Int32 size;
    mPresenters->GetSize(&size);
    for (Int32 i = 0; i < size;) {
        AutoPtr<IInterface> elem;
        mPresenters->Get(i, (IInterface**)&elem);
        AutoPtr<IMenuPresenter> presenter;
        IWeakReference::Probe(elem)->Resolve(EIID_IMenuPresenter, (IInterface**)&presenter);
        if (presenter == NULL) {
            mPresenters->Remove(elem);
        }
        else {
            i++;

            Int32 id = 0;
            presenter->GetId(&id);
            if (id > 0) {
                AutoPtr<IInterface> item;
                presenterStates->Get(id, (IInterface**)&item);
                AutoPtr<IParcelable> parcel = IParcelable::Probe(item);
                if (parcel != NULL) {
                    presenter->OnRestoreInstanceState(parcel);
                }
            }
        }
    }
}

ECode MenuBuilder::SavePresenterStates(
    /* [in] */ IBundle* outState)
{
    DispatchSaveInstanceState(outState);
    return NOERROR;
}

ECode MenuBuilder::RestorePresenterStates(
    /* [in] */ IBundle* state)
{
    DispatchRestoreInstanceState(state);
    return NOERROR;
}

ECode MenuBuilder::SaveActionViewStates(
    /* [in] */ IBundle* outStates)
{
    assert(outStates != NULL);
    AutoPtr<ISparseArray> viewStates;

    Int32 itemCount;
    GetSize(&itemCount);
    for (Int32 i = 0; i < itemCount; i++) {
        AutoPtr<IMenuItem> item;
        GetItem(i, (IMenuItem**)&item);
        AutoPtr<IView> v;
        item->GetActionView((IView**)&v);

        Int32 id = 0;
        if (v != NULL && (v->GetId(&id), id) != IView::NO_ID) {
            if (viewStates == NULL) {
                CSparseArray::New((ISparseArray**)&viewStates);
            }

            v->SaveHierarchyState(viewStates);

            Boolean expanded = FALSE;
            if (item->IsActionViewExpanded(&expanded), expanded) {
                item->GetItemId(&id);
                outStates->PutInt32(EXPANDED_ACTION_VIEW_ID, id);
            }
        }

        Boolean has = FALSE;
        if (item->HasSubMenu(&has), has) {
            AutoPtr<ISubMenu> submenu;
            item->GetSubMenu((ISubMenu**)&submenu);
            IMenuBuilder::Probe(submenu)->SaveActionViewStates(outStates);
        }
    }

    if (viewStates != NULL) {
        String key;
        GetActionViewStatesKey(&key);
        outStates->PutSparseParcelableArray(key, viewStates);
    }

    return NOERROR;
}

ECode MenuBuilder::RestoreActionViewStates(
    /* [in] */ IBundle* states)
{
    if (states == NULL) {
        return NOERROR;
    }

    String key;
    GetActionViewStatesKey(&key);
    AutoPtr<ISparseArray> viewStates;
    states->GetSparseParcelableArray(key, (ISparseArray**)&viewStates);

    Int32 itemCount;
    GetSize(&itemCount);
    for (Int32 i = 0; i < itemCount; i++) {
        AutoPtr<IMenuItem> item;
        GetItem(i, (IMenuItem**)&item);
        AutoPtr<IView> v;
        item->GetActionView((IView**)&v);

        Int32 id = 0;
        if (v != NULL && (v->GetId(&id), id) != IView::NO_ID) {
            v->RestoreHierarchyState(viewStates);
        }

        Boolean has = FALSE;
        if (item->HasSubMenu(&has), has) {
            AutoPtr<ISubMenu> submenu;
            item->GetSubMenu((ISubMenu**)&submenu);
            IMenuBuilder::Probe(submenu)->RestoreActionViewStates(states);
        }
    }

    Int32 expandedId = 0;
    states->GetInt32(EXPANDED_ACTION_VIEW_ID, &expandedId);
    if (expandedId > 0) {
        AutoPtr<IMenuItem> itemToExpand;
        FindItem(expandedId, (IMenuItem**)&itemToExpand);
        if (itemToExpand != NULL) {
            Boolean expanded = FALSE;
            itemToExpand->ExpandActionView(&expanded);
        }
    }

    return NOERROR;
}

ECode MenuBuilder::GetActionViewStatesKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key)
    *key = ACTION_VIEW_STATES_KEY;
    return NOERROR;
}

ECode MenuBuilder::SetCallback(
    /* [in] */ IMenuBuilderCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode MenuBuilder::GetCallback(
    /* [out] */ IMenuBuilderCallback** callback)
{
    VALIDATE_NOT_NULL(callback)
    *callback = mCallback;
    REFCOUNT_ADD(*callback);
    return NOERROR;
}

AutoPtr<IMenuItem> MenuBuilder::AddInternal(
    /* [in] */ Int32 group,
    /* [in] */ Int32 id,
    /* [in] */ Int32 categoryOrder,
    /* [in] */ ICharSequence* title)
{
    Int32 ordering = GetOrdering(categoryOrder);

    AutoPtr<IMenuItemImpl> item = CreateNewMenuItem(group, id, categoryOrder,
        ordering, title, mDefaultShowAsAction);

    if (mCurrentMenuInfo != NULL) {
        // Pass along the current menu info
        item->SetMenuInfo(mCurrentMenuInfo);
    }

    mItems->Insert(FindInsertIndex(mItems, ordering), item);
    OnItemsChanged(TRUE);
    return IMenuItem::Probe(item);
}

AutoPtr<IMenuItemImpl> MenuBuilder::CreateNewMenuItem(
    /* [in] */ Int32 group,
    /* [in] */ Int32 id,
    /* [in] */ Int32 categoryOrder,
    /* [in] */ Int32 ordering,
    /* [in] */ ICharSequence* title,
    /* [in] */ Int32 defaultShowAsAction)
{
    AutoPtr<IMenuItemImpl> item;
    CMenuItemImpl::New(this, group, id, categoryOrder, ordering, title,
        defaultShowAsAction, (IMenuItemImpl**)&item);
    return item;
}

ECode MenuBuilder::Add(
    /* [in] */ ICharSequence* title,
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    AutoPtr<IMenuItem> temp = AddInternal(0, 0, 0, title);
    *item = temp;
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode MenuBuilder::Add(
    /* [in] */ Int32 titleRes,
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    String str;
    mResources->GetString(titleRes, &str);
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(str);
    AutoPtr<IMenuItem> temp =  AddInternal(0, 0, 0, csq);
    *item = temp;
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode MenuBuilder::Add(
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 itemId,
    /* [in] */ Int32 order,
    /* [in] */ ICharSequence* title,
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    AutoPtr<IMenuItem> temp = AddInternal(groupId, itemId, order, title);
    *item = temp;
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode MenuBuilder::Add(
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 itemId,
    /* [in] */ Int32 order,
    /* [in] */ Int32 titleRes,
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    String str;
    mResources->GetString(titleRes, &str);
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(str);
    AutoPtr<IMenuItem> temp = AddInternal(groupId, itemId, order, csq);
    *item = temp;
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode MenuBuilder::AddSubMenu(
    /* [in] */ ICharSequence* title,
    /* [out] */ ISubMenu** subMenu)
{
    return AddSubMenu(0, 0, 0, title, subMenu);
}

ECode MenuBuilder::AddSubMenu(
    /* [in] */ Int32 titleRes,
    /* [out] */ ISubMenu** subMenu)
{
    String str;
    mResources->GetString(titleRes, &str);
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(str);
    return AddSubMenu(0, 0, 0, csq, subMenu);
}

ECode MenuBuilder::AddSubMenu(
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 itemId,
    /* [in] */ Int32 order,
    /* [in] */ ICharSequence*  title,
    /* [out] */ ISubMenu** subMenu)
{
    VALIDATE_NOT_NULL(subMenu)
    AutoPtr<IMenuItemImpl> item = IMenuItemImpl::Probe(AddInternal(groupId, itemId, order, title));
    CSubMenuBuilder::New(mContext, this, item, subMenu);
    item->SetSubMenu(ISubMenuBuilder::Probe(*subMenu));
    return NOERROR;
}

ECode MenuBuilder::AddSubMenu(
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 itemId,
    /* [in] */ Int32 order,
    /* [in] */ Int32 titleRes,
    /* [out] */ ISubMenu** subMenu)
{
    String str;
    mResources->GetString(titleRes, &str);
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(str);
    return AddSubMenu(groupId, itemId, order, csq, subMenu);
}

ECode MenuBuilder::AddIntentOptions(
    /* [in] */ Int32 group,
    /* [in] */ Int32 id,
    /* [in] */ Int32 categoryOrder,
    /* [in] */ IComponentName* caller,
    /* [in] */ ArrayOf<IIntent*>* specifics,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ ArrayOf<IMenuItem*>* outSpecificItems,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);

    AutoPtr<IList> lri;
    pm->QueryIntentActivityOptions(caller, specifics, intent, 0, (IList**)&lri);
    Int32 N = 0;
    if (lri != NULL) {
        lri->GetSize(&N);
    }

    if ((flags & IMenu::FLAG_APPEND_TO_GROUP) == 0) {
        RemoveGroup(group);
    }

    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> elem;
        lri->Get(i, (IInterface**)&elem);
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(elem);

        Int32 specificIndex = 0;
        ri->GetSpecificIndex(&specificIndex);
        AutoPtr<IIntent> rintent;
        if (specificIndex < 0) {
            CIntent::New(intent, (IIntent**)&rintent);
        }
        else {
            CIntent::New((*specifics)[specificIndex], (IIntent**)&rintent);
        }

        AutoPtr<IActivityInfo> info;
        ri->GetActivityInfo((IActivityInfo**)&info);
        assert(info != NULL);

        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        assert(appInfo != NULL);

        String packageName, name;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);
        IPackageItemInfo::Probe(info)->GetName(&name);

        AutoPtr<IComponentName> cName;
        CComponentName::New(packageName, name, (IComponentName**)&cName);
        rintent->SetComponent(cName);

        AutoPtr<ICharSequence> label;
        ri->LoadLabel(pm, (ICharSequence**)&label);
        AutoPtr<IMenuItem> item;
        Add(group, id, categoryOrder, label, (IMenuItem**)&item);
        assert(item != NULL);

        AutoPtr<IDrawable> icon;
        ri->LoadIcon(pm, (IDrawable**)&icon);
        item->SetIcon(icon);
        item->SetIntent(rintent);

        if (outSpecificItems != NULL && specificIndex >= 0) {
            outSpecificItems->Set(specificIndex, item);
        }
    }

    *result = N;
    return NOERROR;
}

ECode MenuBuilder::RemoveItem(
    /* [in] */ Int32 id)
{
    Int32 index;
    FindItemIndex(id, &index);
    RemoveItemAtInt(index, TRUE);
    return NOERROR;
}

ECode MenuBuilder::RemoveGroup(
    /* [in] */ Int32 group)
{
    Int32 i;
    FindGroupIndex(group, &i);

    if (i >= 0) {
        const Int32 maxRemovable = mItems->GetSize() - i;
        Int32 numRemoved = 0, groupid;
        while ((numRemoved++ < maxRemovable) &&
            ((IMenuItem::Probe((*mItems)[i])->GetGroupId(&groupid), groupid) == group)) {
            // Don't force update for each one, this method will do it at the end
            RemoveItemAtInt(i, FALSE);
        }

        // Notify menu views
        OnItemsChanged(TRUE);
    }

    return NOERROR;
}

void MenuBuilder::RemoveItemAtInt(
    /* [in] */ Int32 index,
    /* [in] */ Boolean updateChildrenOnMenuViews)
{
    if ((index < 0) || (index >= (Int32)mItems->GetSize())) return;

    mItems->Remove(index);

    if (updateChildrenOnMenuViews) OnItemsChanged(TRUE);
}

ECode MenuBuilder::RemoveItemAt(
    /* [in] */ Int32 index)
{
    RemoveItemAtInt(index, TRUE);
    return NOERROR;
}

ECode MenuBuilder::ClearAll()
{
    mPreventDispatchingItemsChanged = TRUE;
    Clear();
    ClearHeader();
    mPreventDispatchingItemsChanged = FALSE;
    mItemsChangedWhileDispatchPrevented = FALSE;
    OnItemsChanged(TRUE);

    return NOERROR;
}

ECode MenuBuilder::Clear()
{
    if (mExpandedItem != NULL) {
        Boolean res;
        CollapseItemActionView(mExpandedItem, &res);
    }

    mItems->Clear();

    OnItemsChanged(TRUE);

    return NOERROR;
}

ECode MenuBuilder::SetExclusiveItemChecked(
    /* [in] */ IMenuItem* item)
{
    Int32 group;
    item->GetGroupId(&group);

    MenuItemImplList::Iterator iter;
    for (iter = mItems->Begin(); iter != mItems->End(); ++iter) {
        AutoPtr<IMenuItemImpl> curItem = *iter;
        Int32 groupid;
        IMenuItem::Probe(curItem)->GetGroupId(&groupid);
        if (groupid == group) {
            Boolean isCheckable;
            curItem->IsExclusiveCheckable(&isCheckable);
            if (!isCheckable) continue;
            IMenuItem::Probe(curItem)->IsCheckable(&isCheckable);
            if (!isCheckable) continue;

            // Check the item meant to be checked, uncheck the others (that are in the group)
            curItem->SetCheckedInt(curItem.Get() == IMenuItemImpl::Probe(item));
        }
    }
    return NOERROR;
}

ECode MenuBuilder::SetGroupCheckable(
    /* [in] */ Int32 group,
    /* [in] */ Boolean checkable,
    /* [in] */ Boolean exclusive)
{
    MenuItemImplList::Iterator iter;
    for (iter = mItems->Begin(); iter != mItems->End(); ++iter) {
        AutoPtr<IMenuItemImpl> item = *iter;
        Int32 groupid;
        IMenuItem::Probe(item)->GetGroupId(&groupid);
        if (groupid == group) {
            item->SetExclusiveCheckable(exclusive);
            IMenuItem::Probe(item)->SetCheckable(checkable);
        }
    }

    return NOERROR;
}

ECode MenuBuilder::SetGroupVisible(
    /* [in] */ Int32 group,
    /* [in] */ Boolean visible)
{
    // We handle the notification of items being changed ourselves, so we use setVisibleInt rather
    // than setVisible and at the end notify of items being changed
    Boolean changedAtLeastOneItem = FALSE;

    MenuItemImplList::Iterator iter;
    for (iter = mItems->Begin(); iter != mItems->End(); ++iter) {
        AutoPtr<IMenuItemImpl> item = *iter;
        Int32 groupid;
        IMenuItem::Probe(item)->GetGroupId(&groupid);
        if (groupid == group) {
            Boolean res = FALSE;
            item->SetVisibleInt(visible, &res);
            if (res) changedAtLeastOneItem = TRUE;
        }
    }

    if (changedAtLeastOneItem) OnItemsChanged(TRUE);

    return NOERROR;
}

ECode MenuBuilder::SetGroupEnabled(
    /* [in] */ Int32 group,
    /* [in] */ Boolean enabled)
{
    MenuItemImplList::Iterator iter;
    for (iter = mItems->Begin(); iter != mItems->End(); ++iter) {
        AutoPtr<IMenuItemImpl> item = *iter;
        Int32 groupid;
        IMenuItem::Probe(item)->GetGroupId(&groupid);
        if (groupid == group) {
            IMenuItem::Probe(item)->SetEnabled(enabled);
        }
    }

    return NOERROR;
}

ECode MenuBuilder::HasVisibleItems(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    MenuItemImplList::Iterator iter;
    for (iter = mItems->Begin(); iter != mItems->End(); ++iter) {
        AutoPtr<IMenuItemImpl> item = *iter;
        Boolean visible;
        IMenuItem::Probe(item)->IsVisible(&visible);
        if (visible) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode MenuBuilder::FindItem(
    /* [in] */ Int32 id,
    /* [out] */ IMenuItem** result)
{
    VALIDATE_NOT_NULL(result)
    MenuItemImplList::Iterator iter;
    for (iter = mItems->Begin(); iter != mItems->End(); ++iter) {
        AutoPtr<IMenuItemImpl> item = *iter;
        Int32 itemId;
        IMenuItem::Probe(item)->GetItemId(&itemId);
        Boolean hasSub;
        if (itemId == id) {
           *result = IMenuItem::Probe(item);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        else if ((IMenuItem::Probe(item)->HasSubMenu(&hasSub), hasSub)){
            if (hasSub) {
                AutoPtr<ISubMenu> subMenu;
                IMenuItem::Probe(item)->GetSubMenu((ISubMenu**)&subMenu);
                AutoPtr<IMenuItem> possibleItem;
                IMenu::Probe(subMenu)->FindItem(id, (IMenuItem**)&possibleItem);

                if (possibleItem != NULL) {
                    *result = possibleItem;
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
            }
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode MenuBuilder::FindItemIndex(
    /* [in] */ Int32 id,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    Int32 i;
    MenuItemImplList::Iterator iter;
    for (iter = mItems->Begin(), i = 0; iter != mItems->End(); ++iter, ++i) {
        AutoPtr<IMenuItemImpl> item = *iter;
        Int32 groupid;
        IMenuItem::Probe(item)->GetGroupId(&groupid);
        if (groupid == id) {
            *index = i;
            return NOERROR;
        }
    }

    *index = -1;
    return NOERROR;
}

ECode MenuBuilder::FindGroupIndex(
    /* [in] */ Int32 group,
    /* [out] */ Int32* index)
{
    return FindGroupIndex(group, 0, index);
}

ECode MenuBuilder::FindGroupIndex(
    /* [in] */ Int32 group,
    /* [in] */ Int32 start,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    //////////////is right?
    if (start < 0) {
        start = 0;
    }

    MenuItemImplList::Iterator iter;
    Int32 i;
    for(iter = mItems->Begin(), i = 0; iter != mItems->End(); ++iter, ++i) {
        if (i >= start) {
            IMenuItemImpl* item = *iter;

            Int32 groupid;
            IMenuItem::Probe(item)->GetGroupId(&groupid);
            if (groupid == group) {
                *index = i;
                return NOERROR;
            }
        }
    }

    *index = -1;
    return NOERROR;
}

ECode MenuBuilder::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mItems->GetSize();
    return NOERROR;
}

ECode MenuBuilder::GetItem(
    /* [in] */ Int32 index,
    /* [out] */ IMenuItem** item)
{
    VALIDATE_NOT_NULL(item)
    *item = IMenuItem::Probe((*mItems)[index]);
    REFCOUNT_ADD(*item)
    return NOERROR;
}

ECode MenuBuilder::IsShortcutKey(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FindItemWithShortcutForKey(keyCode, event) != NULL;
    return NOERROR;
}

ECode MenuBuilder::SetQwertyMode(
    /* [in] */ Boolean isQwerty)
{
    mQwertyMode = isQwerty;
    OnItemsChanged(FALSE);
    return NOERROR;
}

Int32 MenuBuilder::GetOrdering(
    /* [in] */ Int32 categoryOrder)
{
    const Int32 index = (categoryOrder & IMenu::CATEGORY_MASK) >> IMenu::CATEGORY_SHIFT;

    //if (index < 0 || index >= sCategoryToOrder.length) {
    if (index < 0 || index >= 6) {
        //TODO throw new IllegalArgumentException("order does not contain a valid category.");
        assert(0 && "TODO");
    }

    return (sCategoryToOrder[index] << IMenu::CATEGORY_SHIFT) | (categoryOrder & IMenu::USER_MASK);
}

Boolean MenuBuilder::IsQwertyMode()
{
    return mQwertyMode;
}

ECode MenuBuilder::SetShortcutsVisible(
    /* [in] */ Boolean shortcutsVisible)
{
    if (mShortcutsVisible == shortcutsVisible) return NOERROR;

    SetShortcutsVisibleInner(shortcutsVisible);
    OnItemsChanged(FALSE);
    return NOERROR;
}

void MenuBuilder::SetShortcutsVisibleInner(
    /* [in] */ Boolean shortcutsVisible)
{
    AutoPtr<IConfiguration> config;
    mResources->GetConfiguration((IConfiguration**)&config);
    assert(config != NULL);
    Int32 keyboard = 0;
    config->GetKeyboard(&keyboard);

    Boolean tmp = FALSE;
    mResources->GetBoolean(R::bool_::config_showMenuShortcutsWhenKeyboardPresent, &tmp);
    mShortcutsVisible = shortcutsVisible && keyboard != IConfiguration::KEYBOARD_NOKEYS && tmp;
}

ECode MenuBuilder::IsShortcutsVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mShortcutsVisible;
    return NOERROR;
}

AutoPtr<IResources> MenuBuilder::GetResources()
{
    return mResources;
}

ECode MenuBuilder::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

Boolean MenuBuilder::DispatchMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item)
{
    Boolean state = FALSE;
    return mCallback != NULL && (mCallback->OnMenuItemSelected(menu, item, &state), state);
}

ECode MenuBuilder::ChangeMenuMode()
{
    if (mCallback != NULL) {
        return mCallback->OnMenuModeChange(this);
    }

    return NOERROR;
}

Int32 MenuBuilder::FindInsertIndex(
    /* [in] */ MenuItemImplList* items,
    /* [in] */ Int32 ordering)
{
    Int32 i = items->GetSize() - 1;
    MenuItemImplList::ReverseIterator riter;
    for (riter = items->RBegin(); riter != items->REnd(); ++riter, --i) {
        AutoPtr<IMenuItemImpl> item = *riter;
        Int32 _ordering;
        item->GetOrdering(&_ordering);
        if (_ordering <= ordering) {
            return i + 1;
        }
    }

    return 0;
}

ECode MenuBuilder::PerformShortcut(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMenuItemImpl> item = FindItemWithShortcutForKey(keyCode, event);

    if (item != NULL) {
        PerformItemAction(IMenuItem::Probe(item), flags, result);
    }

    if ((flags & IMenu::FLAG_ALWAYS_PERFORM_CLOSE) != 0) {
        Close(TRUE);
    }

    return NOERROR;
}

void MenuBuilder::FindItemsWithShortcutForKey(
    /* [in, out] */ MenuItemImplList* items,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    const Boolean qwerty = IsQwertyMode();
    Int32 metaState = 0;
    event->GetMetaState(&metaState);
    AutoPtr<CKeyCharacterMap::KeyData> possibleChars = new CKeyCharacterMap::KeyData();
    // Get the chars associated with the keyCode (i.e using any chording combo)
    Boolean isKeyCodeMapped = FALSE;
    event->GetKeyData(possibleChars, &isKeyCodeMapped);
    // The delete key is not mapped to '\b' so we treat it specially
    if (!isKeyCodeMapped && (keyCode != IKeyEvent::KEYCODE_DEL)) {
        return;
    }

    // Look for an item whose shortcut is this key.
    MenuItemImplList::Iterator iter;
    for (iter = mItems->Begin(); iter != mItems->End(); ++iter) {
        AutoPtr<IMenuItem> item = IMenuItem::Probe(*iter);
        Boolean hasSub;
        item->HasSubMenu(&hasSub);
        if (hasSub) {
            AutoPtr<ISubMenu> subMenu;
            item->GetSubMenu((ISubMenu**)&subMenu);
            ((MenuBuilder*)IMenuBuilder::Probe(subMenu))->FindItemsWithShortcutForKey(items, keyCode, event);
        }

        Char32 shortcutChar = 0;
        if (qwerty) {
            item->GetAlphabeticShortcut(&shortcutChar);
        }
        else {
            item->GetNumericShortcut(&shortcutChar);
        }
        // Boolean isEnabled = FALSE;
        // if (((metaState & (IKeyEvent::META_SHIFT_ON | IKeyEvent::META_SYM_ON)) == 0) &&
        //       (shortcutChar != 0) &&
        //       (shortcutChar == (*possibleChars->mMeta)[0]
        //           || shortcutChar == (*possibleChars->mMeta)[2]
        //           || (qwerty && shortcutChar == '\b' &&
        //               keyCode == IKeyEvent::KEYCODE_DEL)) && (item->IsEnabled(&isEnabled), isEnabled)) {
        //     items->PushBack(item);
        // }
    }
}

AutoPtr<IMenuItemImpl> MenuBuilder::FindItemWithShortcutForKey(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    // Get all items that can be associated directly or indirectly with the keyCode
    MenuItemImplList * items = mTempShortcutItemList;
    items->Clear();
    FindItemsWithShortcutForKey(items, keyCode, event);

    if (items->IsEmpty()) {
        return NULL;
    }

    Int32 metaState;
    event->GetMetaState(&metaState);
    AutoPtr<CKeyCharacterMap::KeyData> possibleChars = new CKeyCharacterMap::KeyData();
    // Get the chars associated with the keyCode (i.e using any chording combo)
    Boolean isKeyCodeMapped;
    event->GetKeyData(possibleChars, &isKeyCodeMapped);

    // If we have only one element, we can safely returns it
    if (items->GetSize() == 1) {
        AutoPtr<IMenuItemImpl> item = (*items)[0];
        return item;
    }

    const Boolean qwerty = IsQwertyMode();
    // If we found more than one item associated with the key,
    // we have to return the exact match
    MenuItemImplList::Iterator iter;
    for (iter = items->Begin(); iter != items->End(); ++iter) {
        AutoPtr<IMenuItem> item = IMenuItem::Probe(*iter);
        Char32 shortcutChar;
        if (qwerty) {
            item->GetAlphabeticShortcut(&shortcutChar);
        }
        else {
            item->GetNumericShortcut(&shortcutChar);
        }

        // if ((shortcutChar == (*possibleChars->mMeta)[0] &&
        //         (metaState & IKeyEvent::META_ALT_ON) == 0)
        //     || (shortcutChar == (*possibleChars->mMeta)[2] &&
        //         (metaState & IKeyEvent::META_ALT_ON) != 0)
        //     || (qwerty && shortcutChar == '\b' &&
        //         keyCode == IKeyEvent::KEYCODE_DEL)) {
        //     return item;
        // }
    }

    return NULL;
}

ECode MenuBuilder::PerformIdentifierAction(
    /* [in] */ Int32 id,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    // Look for an item whose identifier is the id.
    AutoPtr<IMenuItem> item;
    FindItem(id, (IMenuItem**)&item);
    return PerformItemAction(item, flags, result);
}

ECode MenuBuilder::PerformItemAction(
    /* [in] */ IMenuItem* item,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    return PerformItemAction(item, NULL, flags, result);
}

ECode MenuBuilder::PerformItemAction(
    /* [in] */ IMenuItem* item,
    /* [in] */ IMenuPresenter* preferredPresenter,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMenuItemImpl> itemImpl = IMenuItemImpl::Probe(item);

    Boolean enabled = FALSE;
    if (itemImpl == NULL || (item->IsEnabled(&enabled), !enabled)) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean invoked = FALSE;
    itemImpl->Invoke(&invoked);

    AutoPtr<IActionProvider> provider;
    item->GetActionProvider((IActionProvider**)&provider);
    Boolean res = FALSE;
    Boolean providerHasSubMenu = (provider != NULL && (provider->HasSubMenu(&res), res));

    Boolean has = FALSE;
    if ((itemImpl->HasCollapsibleActionView(&has), has)) {
        Boolean tmp = FALSE;
        invoked |= (item->ExpandActionView(&tmp), tmp);
        if (invoked) Close(TRUE);
    }
    else if ((item->HasSubMenu(&has), has) || providerHasSubMenu) {
        Close(FALSE);

        if (!(item->HasSubMenu(&has), has)) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<ISubMenuBuilder> smb;
            CSubMenuBuilder::New(context, this, itemImpl, (ISubMenuBuilder**)&smb);
            itemImpl->SetSubMenu(smb);
        }

        AutoPtr<ISubMenu> tmpSubMenu;
        item->GetSubMenu((ISubMenu**)&tmpSubMenu);
        AutoPtr<ISubMenuBuilder> subMenu = ISubMenuBuilder::Probe(tmpSubMenu);
        if (providerHasSubMenu) {
            provider->OnPrepareSubMenu(ISubMenu::Probe(subMenu));
        }

        invoked |= DispatchSubMenuSelected(subMenu, preferredPresenter);
        if (!invoked) Close(TRUE);
    }
    else {
        if ((flags & IMenu::FLAG_PERFORM_NO_CLOSE) == 0) {
            Close(TRUE);
        }
    }

    *result = invoked;
    return NOERROR;
}

ECode MenuBuilder::Close(
    /* [in] */ Boolean allMenusAreClosing)
{
    if (mIsClosing) return NOERROR;

    mIsClosing = TRUE;

    Int32 size;
    mPresenters->GetSize(&size);
    for (Int32 i = 0; i < size;) {
        AutoPtr<IInterface> elem;
        mPresenters->Get(i, (IInterface**)&elem);
        AutoPtr<IMenuPresenter> presenter;
        IWeakReference::Probe(elem)->Resolve(EIID_IMenuPresenter, (IInterface**)&presenter);
        if (presenter == NULL) {
            mPresenters->Remove(elem);
        }
        else {
            i++;
            presenter->OnCloseMenu(this, allMenusAreClosing);
        }
    }

    mIsClosing = FALSE;
    return NOERROR;
}

ECode MenuBuilder::Close()
{
    Close(TRUE);
    return NOERROR;
}

ECode MenuBuilder::OnItemsChanged(
    /* [in] */ Boolean structureChanged)
{
    if (!mPreventDispatchingItemsChanged) {
        if (structureChanged) {
            mIsVisibleItemsStale = TRUE;
            mIsActionItemsStale = TRUE;
        }

        DispatchPresenterUpdate(structureChanged);
    }
    else {
        mItemsChangedWhileDispatchPrevented = TRUE;
    }
    return NOERROR;
}

ECode MenuBuilder::StopDispatchingItemsChanged()
{
    if (!mPreventDispatchingItemsChanged) {
        mPreventDispatchingItemsChanged = TRUE;
        mItemsChangedWhileDispatchPrevented = FALSE;
    }

    return NOERROR;
}

ECode MenuBuilder::StartDispatchingItemsChanged()
{
    mPreventDispatchingItemsChanged = FALSE;

    if (mItemsChangedWhileDispatchPrevented) {
        mItemsChangedWhileDispatchPrevented = FALSE;
        OnItemsChanged(TRUE);
    }

    return NOERROR;
}

ECode MenuBuilder::OnItemVisibleChanged(
    /* [in] */ IMenuItemImpl* item)
{
    // Notify of items being changed
    mIsVisibleItemsStale = TRUE;
    OnItemsChanged(TRUE);
    return NOERROR;
}

ECode MenuBuilder::OnItemActionRequestChanged(
    /* [in] */ IMenuItemImpl* item)
{
    // Notify of items being changed
    mIsActionItemsStale = TRUE;
    OnItemsChanged(TRUE);
    return NOERROR;
}

ECode MenuBuilder::GetVisibleItems(
    /* [out] */ IArrayList** items)
{
    VALIDATE_NOT_NULL(items)
    if (!mIsVisibleItemsStale) {
        *items = mVisibleItems;
        REFCOUNT_ADD(*items)
        return NOERROR;
    }

    // Refresh the visible items
    mVisibleItems->Clear();

    MenuItemImplList::Iterator iter;
    for (iter = mItems->Begin(); iter != mItems->End(); ++iter) {
        AutoPtr<IMenuItemImpl> item = *iter;
        Boolean visible;
        IMenuItem::Probe(item)->IsVisible(&visible);
        if (visible) {
            mVisibleItems->Add(item);
        }
    }

    mIsVisibleItemsStale = FALSE;
    mIsActionItemsStale = TRUE;

    *items = mVisibleItems;
    REFCOUNT_ADD(*items)
    return NOERROR;
}

ECode MenuBuilder::FlagActionItems()
{
    // Important side effect: if getVisibleItems is stale it may refresh,
    // which can affect action items staleness.
    AutoPtr<IArrayList> visibleItems;
    GetVisibleItems((IArrayList**)&visibleItems);

    if (!mIsActionItemsStale) {
        return NOERROR;
    }

    // Presenters flag action items as needed.
    Boolean flagged = FALSE;
    Int32 size;
    mPresenters->GetSize(&size);
    for (Int32 i = 0; i < size;) {
        AutoPtr<IInterface> elem;
        mPresenters->Get(i, (IInterface**)&elem);
        AutoPtr<IMenuPresenter> presenter;
        IWeakReference::Probe(elem)->Resolve(EIID_IMenuPresenter, (IInterface**)&presenter);
        if (presenter == NULL) {
            mPresenters->Remove(elem);
        }
        else {
            i++;
            Boolean flagAction = FALSE;
            flagged |= (presenter->FlagActionItems(&flagAction), flagAction);
        }
    }

    if (flagged) {
        mActionItems->Clear();
        mNonActionItems->Clear();
        Int32 itemsSize;
        visibleItems->GetSize(&itemsSize);
        for (Int32 i = 0; i < itemsSize; i++) {
            AutoPtr<IInterface> elem;
            visibleItems->Get(i, (IInterface**)&elem);
            AutoPtr<IMenuItemImpl> item = IMenuItemImpl::Probe(elem);
            Boolean isAction = FALSE;
            if (item->IsActionButton(&isAction), isAction) {
                mActionItems->Add(item);
            }
            else {
                mNonActionItems->Add(item);
            }
        }
    }
    else {
        // Nobody flagged anything, everything is a non-action item.
        // (This happens during a first pass with no action-item presenters.)
        mActionItems->Clear();
        mNonActionItems->Clear();
        visibleItems = NULL;
        GetVisibleItems((IArrayList**)&visibleItems);
        mNonActionItems->AddAll(ICollection::Probe(visibleItems));
    }
    mIsActionItemsStale = FALSE;

    return NOERROR;
}

ECode MenuBuilder::GetActionItems(
    /* [out] */ IArrayList** items)
{
    VALIDATE_NOT_NULL(items)
    FlagActionItems();
    *items = mActionItems;
    REFCOUNT_ADD(*items)
    return NOERROR;
}

ECode MenuBuilder::GetNonActionItems(
    /* [out] */ IArrayList** items)
{
    VALIDATE_NOT_NULL(items)
    FlagActionItems();
    *items = mNonActionItems;
    REFCOUNT_ADD(*items)
    return NOERROR;
}

ECode MenuBuilder::ClearHeader()
{
    mHeaderIcon = NULL;
    mHeaderTitle = NULL;
    mHeaderView = NULL;

    OnItemsChanged(FALSE);
    return NOERROR;
}

void MenuBuilder::SetHeaderInternal(
        /* [in] */ Int32 titleRes,
        /* [in] */ ICharSequence* title,
        /* [in] */ Int32 iconRes,
        /* [in] */ IDrawable* icon,
        /* [in] */ IView* view)
{
    AutoPtr<IResources> r = GetResources();

    if (view != NULL) {
        mHeaderView = view;

        // If using a custom view, then the title and icon aren't used
        mHeaderTitle = NULL;
        mHeaderIcon = NULL;
    }
    else {
        if (titleRes > 0) {
            mHeaderTitle = NULL;
            r->GetText(titleRes, (ICharSequence**)&mHeaderTitle);
        }
        else if (title != NULL) {
            mHeaderTitle = title;
        }

        if (iconRes > 0) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            mHeaderIcon = NULL;
            context->GetDrawable(iconRes, (IDrawable**)&mHeaderIcon);
        }
        else if (icon != NULL) {
            mHeaderIcon = icon;
        }

        // If using the title or icon, then a custom view isn't used
        mHeaderView = NULL;
    }

    // Notify of change
    OnItemsChanged(FALSE);
}

ECode MenuBuilder::SetHeaderTitleInt(
    /* [in] */ ICharSequence* title)
{
    SetHeaderInternal(0, title, 0, NULL, NULL);
    return NOERROR;
}

ECode MenuBuilder::SetHeaderTitleInt(
    /* [in] */ Int32 titleRes)
{
    SetHeaderInternal(titleRes, NULL, 0, NULL, NULL);
    return NOERROR;
}

ECode MenuBuilder::SetHeaderIconInt(
    /* [in] */ IDrawable* icon)
{
    SetHeaderInternal(0, NULL, 0, icon, NULL);
    return NOERROR;
}

ECode MenuBuilder::SetHeaderIconInt(
    /* [in] */ Int32 iconRes)
{
    SetHeaderInternal(0, NULL, iconRes, NULL, NULL);
    return NOERROR;
}

ECode MenuBuilder::SetHeaderViewInt(
    /* [in] */ IView* view)
{
    SetHeaderInternal(0, NULL, 0, NULL, view);
    return NOERROR;
}

ECode MenuBuilder::GetHeaderTitle(
    /* [out] */ ICharSequence** headerTitle)
{
    VALIDATE_NOT_NULL(headerTitle)
    *headerTitle = mHeaderTitle;
    REFCOUNT_ADD(*headerTitle)
    return NOERROR;
}

ECode MenuBuilder::GetHeaderIcon(
    /* [out] */ IDrawable** headerIcon)
{
    VALIDATE_NOT_NULL(headerIcon)
    *headerIcon = mHeaderIcon;
    REFCOUNT_ADD(*headerIcon)
    return NOERROR;
}

ECode MenuBuilder::GetHeaderView(
    /* [out] */ IView** headerView)
{
    VALIDATE_NOT_NULL(headerView)
    *headerView = mHeaderView;
    REFCOUNT_ADD(*headerView)
    return NOERROR;
}

ECode MenuBuilder::GetRootMenu(
    /* [out] */ IMenuBuilder** rootMenu)
{
    VALIDATE_NOT_NULL(rootMenu)
    *rootMenu = this;
    REFCOUNT_ADD(*rootMenu)
    return NOERROR;
}

ECode MenuBuilder::SetCurrentMenuInfo(
    /* [in] */ IContextMenuInfo* menuInfo)
{
    mCurrentMenuInfo = menuInfo;
    return NOERROR;
}

ECode MenuBuilder::SetOptionalIconsVisible(Boolean visible)
{
    mOptionalIconsVisible = visible;
    return NOERROR;
}

Boolean MenuBuilder::GetOptionalIconsVisible()
{
    return mOptionalIconsVisible;
}

ECode MenuBuilder::ExpandItemActionView(
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isEmpty;
    if (mPresenters->IsEmpty(&isEmpty), isEmpty) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean expanded = FALSE;

    StopDispatchingItemsChanged();

    Int32 size;
    mPresenters->GetSize(&size);
    for (Int32 i = 0; i < size;) {
        AutoPtr<IInterface> elem;
        mPresenters->Get(i, (IInterface**)&elem);
        AutoPtr<IMenuPresenter> presenter;
        IWeakReference::Probe(elem)->Resolve(EIID_IMenuPresenter, (IInterface**)&presenter);
        if (presenter == NULL) {
            mPresenters->Remove(elem);
        }
        else if ((presenter->ExpandItemActionView(
                this, item, &expanded), expanded)) {
            break;
        }
        else {
            ++i;
        }
    }

    StartDispatchingItemsChanged();

    if (expanded) {
        mExpandedItem = item;
    }

    *result = expanded;
    return NOERROR;
}

ECode MenuBuilder::CollapseItemActionView(
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean isEmpty;
    if ((mPresenters->IsEmpty(&isEmpty), isEmpty)
        || mExpandedItem.Get() != item) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean collapsed = FALSE;

    StopDispatchingItemsChanged();

    Int32 size;
    mPresenters->GetSize(&size);
    for (Int32 i = 0; i < size;) {
        AutoPtr<IInterface> elem;
        mPresenters->Get(i, (IInterface**)&elem);
        AutoPtr<IMenuPresenter> presenter;
        IWeakReference::Probe(elem)->Resolve(EIID_IMenuPresenter, (IInterface**)&presenter);
        if (presenter == NULL) {
            mPresenters->Remove(elem);
        }
        else if ((presenter->CollapseItemActionView(
                this, item, &collapsed), collapsed)) {
            break;
        }
        else {
            ++i;
        }
    }

    StartDispatchingItemsChanged();

    if (collapsed) {
        mExpandedItem = NULL;
    }
    *result = collapsed;
    return NOERROR;
}

ECode MenuBuilder::GetExpandedItem(
    /* [out] */ IMenuItemImpl** item)
{
    VALIDATE_NOT_NULL(item)
    *item = mExpandedItem;
    REFCOUNT_ADD(*item)
    return NOERROR;
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
