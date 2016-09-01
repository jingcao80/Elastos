
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/view/menu/ListMenuPresenter.h"
#include "elastos/droid/internal/view/menu/CMenuDialogHelper.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/ContextThemeWrapperHolder.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::IContextThemeWrapper;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ContextThemeWrapperHolder;
using Elastos::Droid::View::IContextThemeWrapperHolder;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

const String ListMenuPresenter::TAG("ListMenuPresenter");

ListMenuPresenter::MenuAdapter::MenuAdapter(
    /* [in] */ ListMenuPresenter* host)
    : mExpandedIndex(-1)
    , mHost(host)
{
    FindExpandedIndex();
}

ECode ListMenuPresenter::MenuAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    AutoPtr<IArrayList> items;
    mHost->mMenu->GetNonActionItems((IArrayList**)&items);
    Int32 size;
    items->GetSize(&size);
    *count = size - mHost->mItemIndexOffset;
    if (mExpandedIndex < 0) {
        return NOERROR;
    }

    *count -= 1;
    return NOERROR;
}

ECode ListMenuPresenter::MenuAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    AutoPtr<IArrayList> items;
    mHost->mMenu->GetNonActionItems((IArrayList**)&items);
    position += mHost->mItemIndexOffset;
    if (mExpandedIndex >= 0 && position >= mExpandedIndex) {
        position++;
    }
    return items->Get(position, item);
}

ECode ListMenuPresenter::MenuAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    // Since a menu item's ID is optional, we'll use the position as an
    // ID for the item in the AdapterView
    *id = position;
    return NOERROR;
}

ECode ListMenuPresenter::MenuAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* _convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IView> convertView = _convertView;
    if (convertView == NULL) {
        mHost->mInflater->Inflate(mHost->mItemLayoutRes, parent, FALSE, (IView**)&convertView);
    }

    AutoPtr<IMenuItemView> itemView = IMenuItemView::Probe(convertView);
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    itemView->Initialize(IMenuItemImpl::Probe(item), 0);
    *view = convertView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

void ListMenuPresenter::MenuAdapter::FindExpandedIndex()
{
    AutoPtr<IMenuItemImpl> expandedItem;
    mHost->mMenu->GetExpandedItem((IMenuItemImpl**)&expandedItem);
    if (expandedItem != NULL) {
        AutoPtr<IArrayList> items;
        mHost->mMenu->GetNonActionItems((IArrayList**)&items);
        Int32 count;
        items->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> item;
            items->Get(i, (IInterface**)&item);
            if (IMenuItemImpl::Probe(item) == expandedItem) {
                mExpandedIndex = i;
                return;
            }
        }
    }

    mExpandedIndex = -1;
}

ECode ListMenuPresenter::MenuAdapter::NotifyDataSetChanged()
{
    FindExpandedIndex();
    return BaseAdapter::NotifyDataSetChanged();
}

CAR_INTERFACE_IMPL(ListMenuPresenter::OnItemClickListener, Object, IAdapterViewOnItemClickListener)

ECode ListMenuPresenter::OnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mOwner->OnItemClick(parent, v, position, id);
}

CAR_INTERFACE_IMPL_2(ListMenuPresenter, Object, IListMenuPresenter, IMenuPresenter)

ListMenuPresenter::ListMenuPresenter()
    : mContext(NULL)
    , mHolderContext(FALSE)
    , mThemeRes(0)
    , mItemLayoutRes(0)
    , mItemIndexOffset(0)
    , mId(0)
{}

ListMenuPresenter::~ListMenuPresenter()
{
    if (mHolderContext && mContext) {
        REFCOUNT_RELEASE(mContext)
    }
}

ECode ListMenuPresenter::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ Int32 itemLayoutRes)
{
    constructor(itemLayoutRes, 0);
    mContext = ctx;
    if (IContextThemeWrapperHolder::Probe(mContext) != NULL) {
        REFCOUNT_ADD(mContext)
        mHolderContext = TRUE;
    }
    LayoutInflater::From(mContext, (ILayoutInflater**)&mInflater);
    return NOERROR;
}

ECode ListMenuPresenter::constructor(
    /* [in] */ Int32 itemLayoutRes,
    /* [in] */ Int32 themeRes)
{
    mItemLayoutRes = itemLayoutRes;
    mThemeRes = themeRes;
    return NOERROR;
}

ECode ListMenuPresenter::InitForMenu(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu)
{
    if (mThemeRes != 0) {
        if (mHolderContext && mContext) {
            REFCOUNT_RELEASE(mContext)
        }
        mHolderContext = FALSE;
        mContext = NULL;
        mInflater = NULL;
        AutoPtr<ContextThemeWrapperHolder> temp = new ContextThemeWrapperHolder();
        temp->constructor(context, mThemeRes, FALSE/* do not hold */);
        mContext = temp.Get();
        REFCOUNT_ADD(mContext)
        mHolderContext = TRUE;

        LayoutInflater::From(mContext, (ILayoutInflater**)&mInflater);
    }
    else if (mContext != NULL) {
        if (mHolderContext && mContext) {
            REFCOUNT_RELEASE(mContext)
        }
        mHolderContext = FALSE;
        mContext = context;
        if (mInflater == NULL) {
            LayoutInflater::From(mContext, (ILayoutInflater**)&mInflater);
        }
    }

    mMenu = menu;
    if (mAdapter != NULL) {
        mAdapter->NotifyDataSetChanged();
    }

    return NOERROR;
}

ECode ListMenuPresenter::GetMenuView(
    /* [in] */ IViewGroup* root,
    /* [out] */ IMenuView** view)
{
    VALIDATE_NOT_NULL(view)
    if (mMenuView == NULL) {
        AutoPtr<IView> tmpView;
        mInflater->Inflate(R::layout::expanded_menu_layout, root, FALSE, (IView**)&tmpView);
        mMenuView = IExpandedMenuView::Probe(tmpView);
        if (mAdapter == NULL) {
            mAdapter = new MenuAdapter(this);
        }

        IAdapterView::Probe(mMenuView)->SetAdapter(mAdapter);
        AutoPtr<OnItemClickListener> listener = new OnItemClickListener(this);
        IAdapterView::Probe(mMenuView)->SetOnItemClickListener(listener);
    }

    *view = IMenuView::Probe(mMenuView);
    REFCOUNT_ADD(*view);

    return NOERROR;
}

ECode ListMenuPresenter::GetAdapter(
    /* [out] */ IListAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter)
    if (mAdapter == NULL) {
        mAdapter = new MenuAdapter(this);
    }

    *adapter = mAdapter;
    REFCOUNT_ADD(*adapter)
    return NOERROR;
}

ECode ListMenuPresenter::UpdateMenuView(
    /* [in] */ Boolean cleared)
{
    if (mAdapter != NULL) mAdapter->NotifyDataSetChanged();
    return NOERROR;
}

ECode ListMenuPresenter::SetCallback(
    /* [in] */ IMenuPresenterCallback* cb)
{
    mCallback = cb;
    return NOERROR;
}

ECode ListMenuPresenter::OnSubMenuSelected(
    /* [in] */ ISubMenuBuilder* subMenu,
    /* [out] */ Boolean* selected)
{
    VALIDATE_NOT_NULL(selected)
    Boolean has = FALSE;
    if (!(IMenu::Probe(subMenu)->HasVisibleItems(&has), has)) {
        *selected = FALSE;
        return NOERROR;
    }

    // The window manager will give us a token.
    AutoPtr<IMenuDialogHelper> helper;
    CMenuDialogHelper::New(IMenuBuilder::Probe(subMenu), (IMenuDialogHelper**)&helper);
    helper->Show(NULL);
    if (mCallback != NULL) {
        Boolean tmp = FALSE;
        mCallback->OnOpenSubMenu(IMenuBuilder::Probe(subMenu), &tmp);
    }

    *selected = TRUE;
    return NOERROR;
}

ECode ListMenuPresenter::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    if (mCallback != NULL) {
        return mCallback->OnCloseMenu(menu, allMenusAreClosing);
    }

    return NOERROR;
}

Int32 ListMenuPresenter::GetItemIndexOffset()
{
    return mItemIndexOffset;
}

ECode ListMenuPresenter::SetItemIndexOffset(
    /* [in] */ Int32 offset)
{
    mItemIndexOffset = offset;
    if (mMenuView != NULL) {
        UpdateMenuView(FALSE);
    }

    return NOERROR;
}

ECode ListMenuPresenter::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> item;
    mAdapter->GetItem(position, (IInterface**)&item);
    AutoPtr<IMenuItem> menuItem = IMenuItem::Probe(item);
    Boolean res;
    return mMenu->PerformItemAction(menuItem, this, 0, &res);
}

ECode ListMenuPresenter::FlagActionItems(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ListMenuPresenter::ExpandItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ListMenuPresenter::CollapseItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ListMenuPresenter::SaveHierarchyState(
    /* [in] */ IBundle* outState)
{
    AutoPtr<ISparseArray> viewStates;
    CSparseArray::New((ISparseArray**)&viewStates);
    if (mMenuView != NULL) {
        IView::Probe(mMenuView)->SaveHierarchyState(viewStates);
    }
    return outState->PutSparseParcelableArray(VIEWS_TAG, viewStates);
}

ECode ListMenuPresenter::RestoreHierarchyState(
    /* [in] */ IBundle* inState)
{
    AutoPtr<ISparseArray> viewStates;
    inState->GetSparseParcelableArray(VIEWS_TAG, (ISparseArray**)&viewStates);
    if (viewStates != NULL) {
        IView::Probe(mMenuView)->RestoreHierarchyState(viewStates);
    }
    return NOERROR;
}

ECode ListMenuPresenter::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

ECode ListMenuPresenter::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode ListMenuPresenter::OnSaveInstanceState(
    /* [out] */ IParcelable** parcel)
{
    VALIDATE_NOT_NULL(parcel)
    if (mMenuView == NULL) {
        *parcel = NULL;
        return NOERROR;
    }

    CBundle::New(parcel);
    return SaveHierarchyState(IBundle::Probe(*parcel));
}

ECode ListMenuPresenter::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    return RestoreHierarchyState(IBundle::Probe(state));
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
