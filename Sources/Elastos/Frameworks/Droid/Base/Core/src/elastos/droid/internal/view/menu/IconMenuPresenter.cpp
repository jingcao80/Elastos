
#include "elastos/droid/internal/view/menu/IconMenuPresenter.h"
#include "elastos/droid/internal/view/menu/MenuBuilder.h"
#include "elastos/droid/internal/view/menu/IconMenuView.h"
#include "elastos/droid/internal/view/menu/CMenuDialogHelper.h"
#include "elastos/droid/internal/view/menu/IconMenuItemView.h"
// #include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Os::CBundle;
// using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

String IconMenuPresenter::VIEWS_TAG = String("android:menu:icon");
String IconMenuPresenter::OPEN_SUBMENU_KEY = String("android:menu:icon:submenu");

CAR_INTERFACE_IMPL(IconMenuPresenter::SubMenuPresenterCallback, Object, IMenuPresenterCallback)

IconMenuPresenter::SubMenuPresenterCallback::SubMenuPresenterCallback(
    /* [in] */ IconMenuPresenter* host)
    : mHost(host)
{
}

ECode IconMenuPresenter::SubMenuPresenterCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    mHost->mOpenSubMenuId = 0;
    if (mHost->mOpenSubMenu != NULL) {
        mHost->mOpenSubMenu->Dismiss();
        mHost->mOpenSubMenu = NULL;
    }

    return NOERROR;
}

ECode IconMenuPresenter::SubMenuPresenterCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (subMenu != NULL) {
        AutoPtr<IMenuItem> item;
        ISubMenu::Probe(subMenu)->GetItem((IMenuItem**)&item);
        assert(item != NULL);

        item->GetItemId(&(mHost->mOpenSubMenuId));
    }

    *result = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(IconMenuPresenter, BaseMenuPresenter, IIconMenuPresenter)

IconMenuPresenter::IconMenuPresenter()
    : mOpenSubMenuId(0)
    , mMaxItems(-1)
{
    mSubMenuPresenterCallback = new SubMenuPresenterCallback(this);
}

ECode IconMenuPresenter::constructor(
    /* [in] */ IContext* context)
{
    AutoPtr<IContext> themeContext;
    assert(0);
    // CContextThemeWrapper::New(context, R::style::Theme_IconMenu, (IContextThemeWrapper**)&themeContext);
    return BaseMenuPresenter::constructor(themeContext, R::layout::icon_menu_layout,
            R::layout::icon_menu_item_layout);
}

ECode IconMenuPresenter::InitForMenu(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu)
{
    BaseMenuPresenter::InitForMenu(context, menu);
    mMaxItems = -1;
    return NOERROR;
}

ECode IconMenuPresenter::BindItemView(
    /* [in] */ IMenuItemImpl* item,
    /* [in] */ IMenuItemView* itemView)
{
    AutoPtr<IIconMenuItemView> view = IIconMenuItemView::Probe(itemView);
    view->SetItemData(item);

    AutoPtr<IDrawable> drawable;
    IMenuItem::Probe(item)->GetIcon((IDrawable**)&drawable);
    AutoPtr<ICharSequence> csq;
    item->GetTitleForItemView(IMenuItemView::Probe(itemView), (ICharSequence**)&csq);
    IconMenuItemView* vTemp = (IconMenuItemView*)view.Get();
    vTemp->Initialize(csq, drawable);
    Boolean visiable = FALSE;
    IMenuItem::Probe(item)->IsVisible(&visiable);
    vTemp->SetVisibility(visiable ? IView::VISIBLE : IView::GONE);

    Boolean enabled = FALSE;
    vTemp->IsEnabled(&enabled);
    vTemp->SetEnabled(enabled);

    AutoPtr<IViewGroupLayoutParams> params =
        IViewGroupLayoutParams::Probe(vTemp->GetTextAppropriateLayoutParams());
    vTemp->SetLayoutParams(params);
    return NOERROR;
}

ECode IconMenuPresenter::ShouldIncludeItem(
    /* [in] */ Int32 childIndex,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> itemsToShow;
    mMenu->GetNonActionItems((IArrayList**)&itemsToShow);
    Int32 size;
    itemsToShow->GetSize(&size);
    Boolean fits = (size == mMaxItems && childIndex < mMaxItems) ||
            childIndex < mMaxItems - 1;

    Boolean tmp = FALSE;
    *result = fits && !(item->IsActionButton(&tmp), tmp);
    return NOERROR;
}

void IconMenuPresenter::AddItemView(
    /* [in] */ IView* itemView,
    /* [in] */ Int32 childIndex)
{
    AutoPtr<IIconMenuItemView> v = IIconMenuItemView::Probe(itemView);
    AutoPtr<IIconMenuView> parent = IIconMenuView::Probe(mMenuView);

    IconMenuItemView* vTemp = (IconMenuItemView*)v.Get();
    vTemp->SetIconMenuView(parent);
    vTemp->SetItemInvoker(IMenuBuilderItemInvoker::Probe(parent));
    vTemp->SetBackgroundDrawable(((IconMenuView*)parent.Get())->GetItemBackgroundDrawable());
    BaseMenuPresenter::AddItemView(itemView, childIndex);
}

ECode IconMenuPresenter::OnSubMenuSelected(
    /* [in] */ ISubMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean tmp = FALSE;
    if (!(IMenu::Probe(subMenu)->HasVisibleItems(&tmp), tmp)) {
        *result = FALSE;
        return NOERROR;
    }

    // The window manager will give us a token.
    AutoPtr<IMenuDialogHelper> helper;
    CMenuDialogHelper::New(IMenuBuilder::Probe(subMenu), (IMenuDialogHelper**)&helper);
    helper->SetPresenterCallback(mSubMenuPresenterCallback);
    helper->Show(NULL);
    mOpenSubMenu = helper;

    AutoPtr<IMenuItem> item;
    ISubMenu::Probe(subMenu)->GetItem((IMenuItem**)&item);
    item->GetItemId(&mOpenSubMenuId);
    BaseMenuPresenter::OnSubMenuSelected(subMenu, &tmp);

    *result = TRUE;
    return NOERROR;
}

ECode IconMenuPresenter::UpdateMenuView(
    /* [in] */ Boolean cleared)
{
    AutoPtr<IconMenuView> menuView = (IconMenuView*)IIconMenuView::Probe(mMenuView);
    if (mMaxItems < 0) {
        mMaxItems = menuView->GetMaxItems();
    }

    AutoPtr<IArrayList> itemsToShow;
    mMenu->GetNonActionItems((IArrayList**)&itemsToShow);
    Int32 size;
    itemsToShow->GetSize(&size);
    Boolean needsMore = size > mMaxItems;
    BaseMenuPresenter::UpdateMenuView(cleared);

    AutoPtr<IViewParent> parent;
    if (needsMore && (mMoreView == NULL || (IView::Probe(mMoreView)->GetParent((IViewParent**)&parent),
        parent.Get()) != IViewParent::Probe(menuView))) {
        if (mMoreView == NULL) {
            mMoreView = menuView->CreateMoreItemView();

            AutoPtr<IDrawable> drawable = menuView->GetItemBackgroundDrawable();
            IView::Probe(mMoreView)->SetBackgroundDrawable(drawable);
        }

        menuView->AddView(IView::Probe(mMoreView));
    }
    else if (!needsMore && mMoreView != NULL) {
        IViewManager::Probe(menuView)->RemoveView(IView::Probe(mMoreView));
    }

    menuView->SetNumActualItemsShown(needsMore ? mMaxItems - 1 : size);
    return NOERROR;
}

Boolean IconMenuPresenter::FilterLeftoverView(
    /* [in] */ IViewGroup* parent,
    /* [in] */ Int32 childIndex)
{
    AutoPtr<IView> view;
    parent->GetChildAt(childIndex, (IView**)&view);
    if (view.Get() != IView::Probe(mMoreView)) {
        return BaseMenuPresenter::FilterLeftoverView(parent, childIndex);
    }

    return FALSE;
}

ECode IconMenuPresenter::GetNumActualItemsShown(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    *num = ((IconMenuView*)IIconMenuView::Probe(mMenuView))->GetNumActualItemsShown();
    return NOERROR;
}

ECode IconMenuPresenter::SaveHierarchyState(
    /* [in] */ IBundle* outState)
{
    AutoPtr<ISparseArray> viewStates;
    CSparseArray::New((ISparseArray**)&viewStates);

    if (mMenuView != NULL) {
        IView::Probe(mMenuView)->SaveHierarchyState(viewStates);
    }
    outState->PutSparseParcelableArray(VIEWS_TAG, viewStates);
    return NOERROR;
}

ECode IconMenuPresenter::RestoreHierarchyState(
    /* [in] */ IBundle* inState)
{
    AutoPtr<ISparseArray> viewStates;
    inState->GetSparseParcelableArray(VIEWS_TAG, (ISparseArray**)&viewStates);

    if (viewStates != NULL) {
        IView::Probe(mMenuView)->RestoreHierarchyState(viewStates);
    }

    Int32 subMenuId;
    inState->GetInt32(OPEN_SUBMENU_KEY, 0, &subMenuId);
    if (subMenuId > 0 && mMenu != NULL) {
        AutoPtr<IMenuItem> item;
        IMenu::Probe(mMenu)->FindItem(subMenuId, (IMenuItem**)&item);
        if (item != NULL) {
            AutoPtr<ISubMenu> sm;
            item->GetSubMenu((ISubMenu**)&sm);
            Boolean rst;
            OnSubMenuSelected(ISubMenuBuilder::Probe(sm), &rst);
        }
    }
    return NOERROR;
}

ECode IconMenuPresenter::OnSaveInstanceState(
    /* [out] */ IParcelable** parcel)
{
    VALIDATE_NOT_NULL(parcel)
    if (mMenuView == NULL) {
        *parcel = NULL;
        return NOERROR;
    }

    AutoPtr<IBundle> state;
    CBundle::New((IBundle**)&state);
    SaveHierarchyState(state);
    if (mOpenSubMenuId > 0) {
        state->PutInt32(OPEN_SUBMENU_KEY, mOpenSubMenuId);
    }

    *parcel = IParcelable::Probe(state);
    REFCOUNT_ADD(*parcel);

    return NOERROR;
}

ECode IconMenuPresenter::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    return RestoreHierarchyState(IBundle::Probe(state));
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
