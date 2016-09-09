
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/view/menu/BaseMenuPresenter.h"
#include "elastos/droid/view/LayoutInflater.h"

using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IContextThemeWrapperHolder;
using Elastos::Utility::IArrayList;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CAR_INTERFACE_IMPL_2(BaseMenuPresenter, Object, IBaseMenuPresenter, IMenuPresenter)

BaseMenuPresenter::BaseMenuPresenter()
    : mSystemContext(NULL)
    , mContext(NULL)
    , mHolderSystemContext(FALSE)
    , mHolderContext(FALSE)
    , mMenuLayoutRes(0)
    , mItemLayoutRes(0)
    , mId(0)
{
}

BaseMenuPresenter::~BaseMenuPresenter()
{
    if (mHolderSystemContext && mSystemContext != NULL) {
        REFCOUNT_RELEASE(mSystemContext)
    }

    if (mHolderContext && mContext != NULL) {
        REFCOUNT_RELEASE(mContext)
    }
    mInflater = NULL;
    mSystemInflater = NULL;
    mMenu = NULL;
    mMenuView = NULL;
}

ECode BaseMenuPresenter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 menuLayoutRes,
    /* [in] */ Int32 itemLayoutRes)
{
    mSystemContext = context;
    if (IContextThemeWrapperHolder::Probe(mSystemContext)) {
        REFCOUNT_ADD(mSystemContext)
        mHolderSystemContext = TRUE;
    }
    FAIL_RETURN(LayoutInflater::From(context, (ILayoutInflater**)&mSystemInflater))
    mMenuLayoutRes = menuLayoutRes;
    mItemLayoutRes = itemLayoutRes;
    return NOERROR;
}

ECode BaseMenuPresenter::InitForMenu(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu)
{
    mContext = context;
    if (IContextThemeWrapperHolder::Probe(mContext)) {
        REFCOUNT_ADD(mContext)
        mHolderContext = TRUE;
    }
    mInflater = NULL;
    FAIL_RETURN(LayoutInflater::From(mContext, (ILayoutInflater**)&mInflater))
    mMenu = menu;
    return NOERROR;
}

ECode BaseMenuPresenter::GetMenuView(
    /* [in] */ IViewGroup* root,
    /* [out] */ IMenuView** view)
{
    VALIDATE_NOT_NULL(view)
    if (mMenuView == NULL) {
        AutoPtr<IView> vTemp;
        mSystemInflater->Inflate(mMenuLayoutRes, root, FALSE, (IView**)&vTemp);
        mMenuView = IMenuView::Probe(vTemp);
        mMenuView->Initialize(mMenu);
        UpdateMenuView(TRUE);
    }

    *view = mMenuView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode BaseMenuPresenter::UpdateMenuView(
    /* [in] */ Boolean cleared)
{
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(mMenuView);
    if (parent == NULL) return NOERROR;

    Int32 childIndex = 0;
    if (mMenu != NULL) {
        mMenu->FlagActionItems();

        AutoPtr<IArrayList> visibleItems;
        mMenu->GetVisibleItems((IArrayList**)&visibleItems);
        Int32 itemCount;
        visibleItems->GetSize(&itemCount);
        IMenuItemView* menuItemView = NULL;
        for (Int32 i = 0; i < itemCount; i++) {
            AutoPtr<IInterface> elem;
            visibleItems->Get(i, (IInterface**)&elem);
            AutoPtr<IMenuItemImpl> item = IMenuItemImpl::Probe(elem);
            Boolean result;
            if (ShouldIncludeItem(childIndex, item, &result), result) {
                AutoPtr<IView> convertView;
                parent->GetChildAt(childIndex, (IView**)&convertView);

                AutoPtr<IMenuItemImpl> oldItem;
                menuItemView = IMenuItemView::Probe(convertView);
                if (menuItemView != NULL) {
                    menuItemView->GetItemData((IMenuItemImpl**)&oldItem);
                }

                AutoPtr<IView> itemView;
                GetItemView(item, convertView, parent, (IView**)&itemView);
                if (item != oldItem) {
                    // Don't let old states linger with new data.
                    itemView->SetPressed(FALSE);
                    itemView->JumpDrawablesToCurrentState();
                }

                if (itemView != convertView) {
                    AddItemView(itemView, childIndex);
                }
                childIndex++;
            }
        }
    }

    // Remove leftover views.
    Int32 childCount = 0;
    while (childIndex < (parent->GetChildCount(&childCount), childCount)) {
        if (!FilterLeftoverView(parent, childIndex)) {
            childIndex++;
        }
    }

    return NOERROR;
}

void BaseMenuPresenter::AddItemView(
    /* [in] */ IView* itemView,
    /* [in] */ Int32 childIndex)
{
    assert(itemView != NULL);
    AutoPtr<IViewParent> parent;
    itemView->GetParent((IViewParent**)&parent);
    AutoPtr<IViewGroup> currentParent = IViewGroup::Probe(parent);
    if (currentParent != NULL) {
        IViewManager::Probe(currentParent)->RemoveView(itemView);
    }
    IViewGroup::Probe(mMenuView)->AddView(itemView, childIndex);
}

Boolean BaseMenuPresenter::FilterLeftoverView(
    /* [in] */ IViewGroup* parent,
    /* [in] */ Int32 childIndex)
{
    assert(parent != NULL);
    parent->RemoveViewAt(childIndex);
    return TRUE;
}

ECode BaseMenuPresenter::SetCallback(
    /* [in] */ IMenuPresenterCallback* cb)
{
    mCallback = cb;
    return NOERROR;
}

ECode BaseMenuPresenter::GetCallback(
    /* [out] */ IMenuPresenterCallback** cb)
{
    VALIDATE_NOT_NULL(cb)
    *cb = mCallback;
    REFCOUNT_ADD(*cb)
    return NOERROR;
}

ECode BaseMenuPresenter::CreateItemView(
    /* [in] */ IViewGroup* parent,
    /* [out] */ IMenuItemView** itemView)
{
    VALIDATE_NOT_NULL(itemView)
    AutoPtr<IView> view;
    mSystemInflater->Inflate(mItemLayoutRes, parent, FALSE, (IView**)&view);
    *itemView = IMenuItemView::Probe(view);
    REFCOUNT_ADD(*itemView)
    return NOERROR;
}

ECode BaseMenuPresenter::GetItemView(
    /* [in] */ IMenuItemImpl* item,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IMenuItemView> itemView;
    if (IMenuItemView::Probe(convertView) != NULL) {
        itemView = IMenuItemView::Probe(convertView);
    }
    else {
        CreateItemView(parent, (IMenuItemView**)&itemView);
    }

    BindItemView(item, itemView);
    *view = IView::Probe(itemView);
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode BaseMenuPresenter::ShouldIncludeItem(
    /* [in] */ Int32 childIndex,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode BaseMenuPresenter::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    if (mCallback != NULL) {
        return mCallback->OnCloseMenu(menu, allMenusAreClosing);
    }

    return NOERROR;
}

ECode BaseMenuPresenter::OnSubMenuSelected(
    /* [in] */ ISubMenuBuilder* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mCallback != NULL) {
        return mCallback->OnOpenSubMenu(IMenuBuilder::Probe(menu), result);
    }

    *result = FALSE;
    return NOERROR;
}

ECode BaseMenuPresenter::FlagActionItems(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode BaseMenuPresenter::ExpandItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode BaseMenuPresenter::CollapseItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode BaseMenuPresenter::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode BaseMenuPresenter::SetId(
    /* [in] */ Int32 id)
{
    mId = id;
    return NOERROR;
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
