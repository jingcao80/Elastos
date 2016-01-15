
#include "elastos/droid/internal/view/menu/MenuPopupHelper.h"
#include "elastos/droid/internal/view/menu/CListMenuItemView.h"
#include "elastos/droid/internal/view/menu/CMenuPopupHelper.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/View.h"
// #include "elastos/droid/widget/CFrameLayout.h"
// #include "elastos/droid/widget/CListPopupWindow.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IOnGlobalLayoutListener;
using Elastos::Droid::View::EIID_IViewOnAttachStateChangeListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::EIID_IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IPopupWindow;
// using Elastos::Droid::Widget::CFrameLayout;
using Elastos::Droid::Widget::IFrameLayout;
// using Elastos::Droid::Widget::CListPopupWindow;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

const String MenuPopupHelper::TAG("MenuPopupHelper");
const Int32 MenuPopupHelper::ITEM_LAYOUT = R::layout::popup_menu_item_layout;

MenuPopupHelper::MenuAdapter::MenuAdapter(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ MenuPopupHelper* host)
    : mExpandedIndex(-1)
    , mHost(host)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(menu);
    assert(wrs != NULL && "Error: Invalid MenuBuilder, IWeakReferenceSource not implemented!");
    wrs->GetWeakReference((IWeakReference**)&mWeakAdapterMenu);
    FindExpandedIndex();
}

ECode MenuPopupHelper::MenuAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    AutoPtr<IMenuBuilder> menu;
    mWeakAdapterMenu->Resolve(EIID_IMenuBuilder, (IInterface**)&menu);
    if (menu == NULL) {
        *count = 0;
        return NOERROR;
    }

    AutoPtr<IArrayList> items;
    mHost->mOverflowOnly ? menu->GetNonActionItems((IArrayList**)&items)
        : menu->GetVisibleItems((IArrayList**)&items);
    Int32 size;
    items->GetSize(&size);
    if (mExpandedIndex < 0) {
        *count = size;
        return NOERROR;
    }
    *count = size - 1;
    return NOERROR;
}

ECode MenuPopupHelper::MenuAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    AutoPtr<IMenuBuilder> menu;
    mWeakAdapterMenu->Resolve(EIID_IMenuBuilder, (IInterface**)&menu);
    if (menu == NULL) {
        *item = NULL;
        return NOERROR;
    }

    AutoPtr<IArrayList> items;
    mHost->mOverflowOnly ? menu->GetNonActionItems((IArrayList**)&items)
        : menu->GetVisibleItems((IArrayList**)&items);
    if (mExpandedIndex >= 0 && position >= mExpandedIndex) {
        position++;
    }

    return items->Get(position, item);
}

ECode MenuPopupHelper::MenuAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    // Since a menu item's ID is optional, we'll use the position as an
    // ID for the item in the AdapterView
    *id = position;
    return NOERROR;
}

ECode MenuPopupHelper::MenuAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in, out?] */ IView* _convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    AutoPtr<IView> convertView = _convertView;
    if (convertView == NULL) {
        mHost->mInflater->Inflate(ITEM_LAYOUT, parent, FALSE, (IView**)&convertView);
    }

    AutoPtr<IMenuItemView> itemView = IMenuItemView::Probe(convertView);
    if (mHost->mForceShowIcon) {
        IListMenuItemView::Probe(convertView)->SetForceShowIcon(TRUE);
    }

    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    itemView->Initialize(IMenuItemImpl::Probe(item), 0);
    *view = convertView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

void MenuPopupHelper::MenuAdapter::FindExpandedIndex()
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

ECode MenuPopupHelper::MenuAdapter::NotifyDataSetChanged()
{
    FindExpandedIndex();
    assert(0);
    return NOERROR;
    // return BaseAdapter::NotifyDataSetChanged();
}

CAR_INTERFACE_IMPL_3(MenuPopupHelper::MyListener, Object, IAdapterViewOnItemClickListener,
    IViewOnKeyListener, IPopupWindowOnDismissListener)

ECode MenuPopupHelper::MyListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mOwner->OnItemClick(parent, v, position, id);
}

ECode MenuPopupHelper::MyListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mOwner->OnKey(v, keyCode, event, result);
}

ECode MenuPopupHelper::MyListener::OnDismiss()
{
    return mOwner->OnDismiss();
}

CAR_INTERFACE_IMPL_7(MenuPopupHelper, Object, IMenuPopupHelper, IAdapterViewOnItemClickListener, IOnGlobalLayoutListener,
    IViewOnKeyListener, IPopupWindowOnDismissListener, IViewOnAttachStateChangeListener, IMenuPresenter)

MenuPopupHelper::MenuPopupHelper()
    : mOverflowOnly(FALSE)
    , mPopupMaxWidth(0)
    , mPopupStyleAttr(0)
    , mForceShowIcon(FALSE)
    , mHasContentWidth(FALSE)
    , mContentWidth(0)
    , mDropDownGravity(IGravity::NO_GRAVITY)
{}

ECode MenuPopupHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu)
{
    return constructor(context, menu, NULL, FALSE, R::attr::popupMenuStyle);
}

ECode MenuPopupHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IView* anchorView)
{
    return constructor(context, menu, anchorView, FALSE, R::attr::popupMenuStyle);
}

ECode MenuPopupHelper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IView* anchorView,
    /* [in] */ Boolean overflowOnly,
    /* [in] */ Int32 popupStyleAttr)
{
    mContext = context;
    LayoutInflater::From(context, (ILayoutInflater**)&mInflater);
    mMenu = menu;
    assert(0);
    // mAdapter = new MenuAdapter(mMenu, this);
    mOverflowOnly = overflowOnly;
    mPopupStyleAttr = popupStyleAttr;

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    assert(res != NULL);

    AutoPtr<IDisplayMetrics> display;
    res->GetDisplayMetrics((IDisplayMetrics**)&display);
    assert(display != NULL);

    Int32 widthPixels = 0, pixelSize = 0;
    display->GetWidthPixels(&widthPixels);
    res->GetDimensionPixelSize(R::dimen::config_prefDialogWidth, &pixelSize);
    mPopupMaxWidth = Elastos::Core::Math::Max(widthPixels / 2, pixelSize);

    mAnchorView = anchorView;

    // Present the menu using our context, not the menu builder's context.
    menu->AddMenuPresenter(this, context);
    return NOERROR;
}

ECode MenuPopupHelper::SetAnchorView(
    /* [in] */ IView* anchor)
{
    mAnchorView = anchor;
    return NOERROR;
}

ECode MenuPopupHelper::SetForceShowIcon(
    /* [in] */ Boolean forceShow)
{
    mForceShowIcon = forceShow;
    return NOERROR;
}

ECode MenuPopupHelper::SetGravity(
    /* [in] */ Int32 gravity)
{
    mDropDownGravity = gravity;
    return NOERROR;
}

ECode MenuPopupHelper::Show()
{
    Boolean res;
    TryShow(&res);
    if (!res) {
        Logger::E(TAG, "MenuPopupHelper cannot be used without an anchor");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode MenuPopupHelper::GetPopup(
    /* [out] */ IListPopupWindow** popup)
{
    VALIDATE_NOT_NULL(popup)
    *popup = mPopup;
    REFCOUNT_ADD(*popup)
    return NOERROR;
}

ECode MenuPopupHelper::TryShow(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mPopup = NULL;
    assert(0);
    // CListPopupWindow::New(mContext, NULL, mPopupStyleAttr, (IListPopupWindow**)&mPopup);
    AutoPtr<MyListener> listener = new MyListener(this);
    mPopup->SetOnDismissListener(listener);
    mPopup->SetOnItemClickListener(listener);
    assert(0);
    // mPopup->SetAdapter(mAdapter);
    mPopup->SetModal(TRUE);

    AutoPtr<IView> anchor = mAnchorView;
    if (anchor != NULL) {
        Boolean addGlobalListener = mTreeObserver == NULL;
        mTreeObserver = NULL;
        anchor->GetViewTreeObserver((IViewTreeObserver**)&mTreeObserver); // Refresh to latest
        if (addGlobalListener) mTreeObserver->AddOnGlobalLayoutListener(this);
        anchor->AddOnAttachStateChangeListener(this);
        mPopup->SetAnchorView(anchor);
        mPopup->SetDropDownGravity(mDropDownGravity);
    }
    else {
        *result = FALSE;
        return NOERROR;
    }

    if (!mHasContentWidth) {
        mContentWidth = MeasureContentWidth();
        mHasContentWidth = TRUE;
    }

    mPopup->SetContentWidth(mContentWidth);
    mPopup->SetInputMethodMode(IPopupWindow::INPUT_METHOD_NOT_NEEDED);
    mPopup->Show();

    AutoPtr<IListView> listView;
    mPopup->GetListView((IListView**)&listView);
    IView::Probe(listView)->SetOnKeyListener(listener);
    *result = TRUE;
    return NOERROR;
}

ECode MenuPopupHelper::Dismiss()
{
    Boolean res;
    if (IsShowing(&res), res) {
        // hold the last reference of mPopup until Dismiss() finished
        AutoPtr<IListPopupWindow> popup = mPopup;
        mPopup->Dismiss();
    }

    return NOERROR;
}

ECode MenuPopupHelper::OnDismiss()
{
    mPopup = NULL;
    IMenu::Probe(mMenu)->Close();
    if (mTreeObserver != NULL) {
        Boolean tmp = FALSE;
        if (!(mTreeObserver->IsAlive(&tmp), tmp)) {
            mTreeObserver = NULL;
            mAnchorView->GetViewTreeObserver((IViewTreeObserver**)&mTreeObserver);
        }

        mTreeObserver->RemoveGlobalOnLayoutListener(this);
        mTreeObserver = NULL;
    }

    return mAnchorView->RemoveOnAttachStateChangeListener(this);
}

ECode MenuPopupHelper::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean tmp = FALSE;
    *result = mPopup != NULL && (mPopup->IsShowing(&tmp), tmp);
    return NOERROR;
}

ECode MenuPopupHelper::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IMenuBuilder> adapterMenu;
    mAdapter->mWeakAdapterMenu->Resolve(EIID_IMenuBuilder, (IInterface**)&adapterMenu);
    if (adapterMenu != NULL) {
        Boolean tmp;
        AutoPtr<IMenuItem> item;
        mAdapter->GetItem(position, (IInterface**)&item);
        return adapterMenu->PerformItemAction(item, 0, &tmp);
    }
    return NOERROR;
}

ECode MenuPopupHelper::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    assert(result != NULL);
    Int32 action = 0;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_UP && keyCode == IKeyEvent::KEYCODE_MENU) {
        Dismiss();
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

Int32 MenuPopupHelper::MeasureContentWidth()
{
    // Menus don't tend to be long, so this is more sane than it looks.
    Int32 maxWidth = 0;
    AutoPtr<IView> itemView;
    Int32 itemType = 0;

    AutoPtr<MenuAdapter> adapter = mAdapter;
    const Int32 widthMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
        0, Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);
    const Int32 heightMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
        0, Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);
    Int32 count = 0;
    adapter->GetCount(&count);
    for (Int32 i = 0; i < count; i++) {
        Int32 positionType = 0;
        assert(0);
        // adapter->GetItemViewType(i, &positionType);
        if (positionType != itemType) {
            itemType = positionType;
            itemView = NULL;
        }

        if (mMeasureParent == NULL) {
            assert(0);
            // CFrameLayout::New(mContext, (IFrameLayout**)&mMeasureParent);
        }
        AutoPtr<IView> temp;
        adapter->GetView(i, itemView, mMeasureParent, (IView**)&temp);
        itemView = temp;
        itemView->Measure(widthMeasureSpec, heightMeasureSpec);

        Int32 itemWidth;
        itemView->GetMeasuredWidth(&itemWidth);
        if (itemWidth >= mPopupMaxWidth) {
            return mPopupMaxWidth;
        }
        else if (itemWidth > maxWidth) {
            maxWidth = itemWidth;
        }
    }

    return maxWidth;
}

ECode MenuPopupHelper::OnGlobalLayout()
{
    Boolean isShowing;
    IsShowing(&isShowing);
    if (isShowing) {
        AutoPtr<IView> anchor = mAnchorView;
        Boolean tmp = FALSE;
        if (anchor == NULL || !(anchor->IsShown(&tmp), tmp)) {
            Dismiss();
        }
        else if (isShowing) {
            // Recompute window size and position
            return mPopup->Show();
        }
    }

    return NOERROR;
}

ECode MenuPopupHelper::OnViewAttachedToWindow(
    /* [in] */ IView* v)
{
    return NOERROR;
}

ECode MenuPopupHelper::OnViewDetachedFromWindow(
    /* [in] */ IView* v)
{
    if (mTreeObserver != NULL) {
        Boolean tmp = FALSE;
        if (!(mTreeObserver->IsAlive(&tmp), tmp)) {
            mTreeObserver = NULL;
            v->GetViewTreeObserver((IViewTreeObserver**)&mTreeObserver);
        }

        mTreeObserver->RemoveGlobalOnLayoutListener(this);
    }

    return v->RemoveOnAttachStateChangeListener(this);
}

ECode MenuPopupHelper::InitForMenu(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu)
{
    // Don't need to do anything; we added as a presenter in the constructor.
    return NOERROR;
}

ECode MenuPopupHelper::GetMenuView(
    /* [in] */ IViewGroup* root,
    /* [out] */ IMenuView** view)
{
    Logger::E(TAG, "MenuPopupHelpers manage their own views");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MenuPopupHelper::UpdateMenuView(
    /* [in] */ Boolean cleared)
{
    mHasContentWidth = FALSE;

    if (mAdapter != NULL) {
        mAdapter->NotifyDataSetChanged();
    }

    return NOERROR;
}

ECode MenuPopupHelper::SetCallback(
    /* [in] */ IMenuPresenterCallback* cb)
{
    mPresenterCallback = cb;
    return NOERROR;
}

ECode MenuPopupHelper::OnSubMenuSelected(
    /* [in] */ ISubMenuBuilder* subMenu,
    /* [out] */ Boolean* selected)
{
    assert(selected != NULL && subMenu != NULL);
    Boolean tmp = FALSE;
    if (IMenu::Probe(subMenu)->HasVisibleItems(&tmp), tmp) {
        AutoPtr<IMenuPopupHelper> subPopup;
        CMenuPopupHelper::New(mContext, IMenuBuilder::Probe(subMenu),
            mAnchorView, (IMenuPopupHelper**)&subPopup);
        IMenuPresenter::Probe(subPopup)->SetCallback(mPresenterCallback);

        Boolean preserveIconSpacing = FALSE;
        Int32 count = 0;
        IMenu::Probe(subMenu)->GetSize(&count);
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IMenuItem> childItem;
            IMenu::Probe(subMenu)->GetItem(i, (IMenuItem**)&childItem);
            assert(childItem != NULL);

            AutoPtr<IDrawable> icon;
            childItem->GetIcon((IDrawable**)&icon);
            tmp = FALSE;
            if ((childItem->IsVisible(&tmp), tmp) && icon != NULL) {
                preserveIconSpacing = TRUE;
                break;
            }
        }

        subPopup->SetForceShowIcon(preserveIconSpacing);

        Boolean tryShow = FALSE;
        if (subPopup->TryShow(&tryShow), tryShow) {
            if (mPresenterCallback != NULL) {
                mPresenterCallback->OnOpenSubMenu(IMenuBuilder::Probe(subMenu), &tmp);
            }
            *selected = TRUE;
            return NOERROR;
        }
    }

    *selected = FALSE;
    return NOERROR;
}

ECode MenuPopupHelper::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    // Only care about the (sub)menu we're presenting.
    if (menu != mMenu) return NOERROR;

    Dismiss();
    if (mPresenterCallback != NULL) {
        mPresenterCallback->OnCloseMenu(menu, allMenusAreClosing);
    }

    return NOERROR;
}

ECode MenuPopupHelper::FlagActionItems(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode MenuPopupHelper::ExpandItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode MenuPopupHelper::CollapseItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode MenuPopupHelper::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = 0;
    return NOERROR;
}

ECode MenuPopupHelper::OnSaveInstanceState(
    /* [out] */ IParcelable** parcel)
{
    VALIDATE_NOT_NULL(parcel)
    *parcel = NULL;
    return NOERROR;
}

ECode MenuPopupHelper::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    return NOERROR;
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
