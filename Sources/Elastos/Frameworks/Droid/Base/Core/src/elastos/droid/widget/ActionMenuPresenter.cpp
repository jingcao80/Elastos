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

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/widget/ActionMenuPresenter.h"
#include "elastos/droid/internal/view/menu/MenuItemImpl.h"
#include "elastos/droid/widget/ActionMenuView.h"
#include "elastos/droid/widget/CActionMenuView.h"
#include "elastos/droid/widget/ListPopupWindow.h"
#include "elastos/droid/internal/view/ActionBarPolicy.h"
#include "elastos/droid/internal/transition/ActionBarTransition.h"
#include "elastos/droid/widget/CActionMenuPresenterSavedState.h"
#include "elastos/droid/view/ActionProvider.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Internal::View::IActionBarPolicy;
using Elastos::Droid::Internal::View::ActionBarPolicy;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Internal::View::Menu::MenuItemImpl;
using Elastos::Droid::Internal::View::Menu::IActionMenuItemView;
using Elastos::Droid::Internal::View::Menu::ISubMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuBuilderItemInvoker;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuPresenterCallback;
using Elastos::Droid::Internal::View::Menu::EIID_IPopupCallback;
using Elastos::Droid::Internal::Transition::ActionBarTransition;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::ActionProvider;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityEvent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::ISoundEffectConstants;
using Elastos::Droid::View::EIID_ISubUiVisibilityListener;

using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Widget {

String ActionMenuPresenter::TAG("ActionMenuPresenter");

ActionMenuPresenter::MyForwardingListener::MyForwardingListener(
    /* [in] */ IView* host,
    /* [in] */ ActionMenuPresenter* hostEx)
{
    ForwardingListener::constructor(host);
    mHostEx = hostEx;
}

ECode ActionMenuPresenter::MyForwardingListener::GetPopup(
    /* [out] */ IListPopupWindow** window)
{
    if (mHostEx->mOverflowPopup == NULL) {
        *window = NULL;
        return NOERROR;
    }

    return mHostEx->mOverflowPopup->GetPopup(window);
}

Boolean ActionMenuPresenter::MyForwardingListener::OnForwardingStarted()
{
    Boolean result;
    mHostEx->ShowOverflowMenu(&result);
    return TRUE;
}

Boolean ActionMenuPresenter::MyForwardingListener::OnForwardingStopped()
{
    // Displaying the popup occurs asynchronously, so wait for
    // the runnable to finish before deciding whether to stop
    // forwarding.
    if (mHostEx->mPostedOpenRunnable != NULL) {
        return FALSE;
    }

    Boolean result;
    mHostEx->HideOverflowMenu(&result);
    return TRUE;
}

CAR_INTERFACE_IMPL(ActionMenuPresenter::OverflowMenuButton, ImageButton, IActionMenuChildView)

ActionMenuPresenter::OverflowMenuButton::OverflowMenuButton()
{
}

ActionMenuPresenter::OverflowMenuButton::~OverflowMenuButton()
{
}

ECode ActionMenuPresenter::OverflowMenuButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ActionMenuPresenter* host)
{
    ImageButton::constructor(context, NULL, R::attr::actionOverflowButtonStyle);
    mTempPts = ArrayOf<Float>::Alloc(2);
    mHost = host;
    SetClickable(TRUE);
    SetFocusable(TRUE);
    SetVisibility(IView::VISIBLE);
    SetEnabled(TRUE);
    AutoPtr<MyForwardingListener> listener = new MyForwardingListener(this, mHost);
    SetOnTouchListener(listener);
    return NOERROR;
}

//@Override
ECode ActionMenuPresenter::OverflowMenuButton::PerformClick(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    ImageButton::PerformClick(res);
    if (*res) {
        return NOERROR;
    }

    PlaySoundEffect(ISoundEffectConstants::CLICK);
    mHost->ShowOverflowMenu(res);
    *res = TRUE;
    return NOERROR;
}

ECode ActionMenuPresenter::OverflowMenuButton::NeedsDividerBefore(
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = FALSE;
    return NOERROR;
}

ECode ActionMenuPresenter::OverflowMenuButton::NeedsDividerAfter(
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = FALSE;
    return NOERROR;
}

ECode ActionMenuPresenter::OverflowMenuButton::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ImageButton::OnInitializeAccessibilityNodeInfo(info);
    info->SetCanOpenPopup(TRUE);
    return NOERROR;
}

Boolean ActionMenuPresenter::OverflowMenuButton::SetFrame(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Boolean changed = ImageButton::SetFrame(left, top, right, bottom);

    // Set up the hotspot bounds to be centered on the image.
    AutoPtr<IDrawable> d;
    GetDrawable((IDrawable**)&d);
    AutoPtr<IDrawable> bg;
    GetBackground((IDrawable**)&bg);
    if (d != NULL && bg != NULL) {
        AutoPtr<IRect> bounds;
        d->GetBounds((IRect**)&bounds);
        Int32 tmp;
        bounds->GetCenterX(&tmp);

        (*mTempPts)[0] = tmp;
        AutoPtr<IMatrix> matrix;
        GetImageMatrix((IMatrix**)&matrix);
        matrix->MapPoints(mTempPts);
        Int32 w, h;
        GetWidth(&w);
        GetHeight(&h);
        Int32 offset =  (Int32) (*mTempPts)[0] - w / 2;
        bg->SetHotspotBounds(offset, 0, w + offset, h);
    }

    return changed;
}

ECode ActionMenuPresenter::OverflowPopup::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IView* anchorView,
    /* [in] */ Boolean overflowOnly,
    /* [in] */ ActionMenuPresenter* host)
{
    MenuPopupHelper::constructor(context, menu, anchorView, overflowOnly, R::attr::actionOverflowMenuStyle);
    mHost = host;
    SetGravity(IGravity::END);
    SetCallback(mHost->mPopupPresenterCallback);
    return NOERROR;
}

ECode ActionMenuPresenter::OverflowPopup::OnDismiss()
{
    MenuPopupHelper::OnDismiss();
    mHost->mMenu->Close(TRUE);
    mHost->mOverflowPopup = NULL;
    return NOERROR;
}

ECode ActionMenuPresenter::ActionButtonSubmenu::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ISubMenuBuilder* subMenu,
    /* [in] */ ActionMenuPresenter* host)
{
    MenuPopupHelper::constructor(context, IMenuBuilder::Probe(subMenu), NULL, FALSE, R::attr::actionOverflowMenuStyle);
    mSubMenu = subMenu;
    mHost = host;

    AutoPtr<IMenuItem> menuItem;
    subMenu->GetItem((IMenuItem**)&menuItem);
    AutoPtr<IMenuItemImpl> item = IMenuItemImpl::Probe(menuItem);
    Boolean isActionButton;
    if (item->IsActionButton(&isActionButton), !isActionButton) {
        // Give a reasonable anchor to nested submenus.
        SetAnchorView(mHost->mOverflowButton == NULL ? IView::Probe(mHost->mMenuView) : mHost->mOverflowButton.Get());
    }

    SetCallback(mHost->mPopupPresenterCallback);

    Boolean preserveIconSpacing = FALSE;
    Int32 count;
    IMenu::Probe(subMenu)->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IMenuItem> childItem;
        IMenu::Probe(subMenu)->GetItem(i, (IMenuItem**)&childItem);
        Boolean isVisible;
        if (childItem->IsVisible(&isVisible), isVisible) {
            AutoPtr<IDrawable> icon;
            childItem->GetIcon((IDrawable**)&icon);
            if (icon != NULL) {
                preserveIconSpacing = TRUE;
                break;
            }
        }
    }
    SetForceShowIcon(preserveIconSpacing);
    return NOERROR;
}

ECode ActionMenuPresenter::ActionButtonSubmenu::OnDismiss()
{
    MenuPopupHelper::OnDismiss();
    mHost->mActionButtonPopup = NULL;
    mHost->mOpenSubMenuId = 0;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ActionMenuPresenter::PopupPresenterCallback, Object, IMenuPresenterCallback)

ActionMenuPresenter::PopupPresenterCallback::PopupPresenterCallback(
    /* [in] */ ActionMenuPresenter* host)
{
    mHost = host;
}

ECode ActionMenuPresenter::PopupPresenterCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    if (subMenu == NULL) {
        return NOERROR;
    }

    AutoPtr<ISubMenuBuilder> sm = ISubMenuBuilder::Probe(subMenu);
    if (sm != NULL) {
        AutoPtr<IMenuItem> item;
        sm->GetItem((IMenuItem**)&item);
        item->GetItemId(&(mHost->mOpenSubMenuId));
    }


    AutoPtr<IMenuPresenterCallback> cb;
    mHost->GetCallback((IMenuPresenterCallback**)&cb);
    if (cb != NULL)
    {
        return cb->OnOpenSubMenu(subMenu, result);
    }
    return NOERROR;
}

ECode ActionMenuPresenter::PopupPresenterCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    if (ISubMenuBuilder::Probe(menu) != NULL) {
        AutoPtr<IMenuBuilder> root;
        IMenuBuilder::Probe(menu)->GetRootMenu((IMenuBuilder**)&root);
        root->Close(FALSE);
    }

    AutoPtr<IMenuPresenterCallback> cb;
    mHost->GetCallback((IMenuPresenterCallback**)&cb);
    if (cb != NULL)
    {
        return cb->OnCloseMenu(menu, allMenusAreClosing);
    }
    return NOERROR;
}

ActionMenuPresenter::OpenOverflowRunnable::OpenOverflowRunnable(
    /* [in] */ OverflowPopup* popup,
    /* [in] */ ActionMenuPresenter* host)
    : mPopup(popup)
    , mHost(host)
{}

ECode ActionMenuPresenter::OpenOverflowRunnable::Run()
{
    mHost->mMenu->ChangeMenuMode();
    IView* menuView = IView::Probe(mHost->mMenuView);
    AutoPtr<IBinder> token;
    Boolean tryShow = FALSE;
    if (menuView != NULL && (menuView->GetWindowToken((IBinder**)&token), token) != NULL && (mPopup->TryShow(&tryShow), tryShow)) {
        mHost->mOverflowPopup = mPopup;
    }
    mHost->mPostedOpenRunnable = NULL;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ActionMenuPresenter::ActionMenuPopupCallback, Object, IPopupCallback)

ActionMenuPresenter::ActionMenuPopupCallback::ActionMenuPopupCallback(
    /* [in] */ ActionMenuPresenter* host)
    : mHost(host)
{}

ECode ActionMenuPresenter::ActionMenuPopupCallback::GetPopup(
    /* [out] */ IListPopupWindow** popup)
{
    VALIDATE_NOT_NULL(popup)

    if (mHost->mActionButtonPopup != NULL) {
        return mHost->mActionButtonPopup->GetPopup(popup);
    }

    *popup = NULL;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ActionMenuPresenter::SavedState, Object, IParcelable)

ActionMenuPresenter::SavedState::SavedState()
{}

ECode ActionMenuPresenter::SavedState::constructor()
{
    mOpenSubMenuId = 0;
    return NOERROR;
}

ECode ActionMenuPresenter::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInt32(&mOpenSubMenuId);
}

ECode ActionMenuPresenter::SavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInt32(mOpenSubMenuId);
}

CAR_INTERFACE_IMPL(ActionMenuPresenter::MySubUiVisibilityListener, Object, ISubUiVisibilityListener)

ActionMenuPresenter::MySubUiVisibilityListener::MySubUiVisibilityListener(
    /* [in] */ ActionMenuPresenter* host)
    : mHost(host)
{}

ECode ActionMenuPresenter::MySubUiVisibilityListener::OnSubUiVisibilityChanged(
    /* [in] */ Boolean isVisible)
{
    if (isVisible) {
        // Not a submenu, but treat it like one.
        Boolean tmp = FALSE;
        mHost->BaseMenuPresenter::OnSubMenuSelected(NULL, &tmp);
    } else {
        mHost->mMenu->Close(FALSE);
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(ActionMenuPresenter, BaseMenuPresenter, IActionMenuPresenter)

ActionMenuPresenter::ActionMenuPresenter()
    : mReserveOverflow(FALSE)
    , mReserveOverflowSet(FALSE)
    , mWidthLimit(0)
    , mActionItemWidthLimit(0)
    , mMaxItems(0)
    , mMaxItemsSet(FALSE)
    , mStrictWidthLimit(FALSE)
    , mWidthLimitSet(FALSE)
    , mExpandedActionViewsExclusive(FALSE)
    , mMinCellSize(0)
{
}

ActionMenuPresenter::~ActionMenuPresenter()
{
}

ECode ActionMenuPresenter::constructor(
    /* [in] */ IContext* context)
{
    mPopupPresenterCallback = new PopupPresenterCallback(this);
    return BaseMenuPresenter::constructor(context, R::layout::action_menu_layout,
            R::layout::action_menu_item_layout);
}

//@Override
ECode ActionMenuPresenter::InitForMenu(
    /* [in] */ IContext* context,
    /* [in] */ IMenuBuilder* menu)
{
    BaseMenuPresenter::InitForMenu(context, menu);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<IActionBarPolicy> abp = ActionBarPolicy::Get(context);
    if (!mReserveOverflowSet) {
        abp->ShowsOverflowMenuButton(&mReserveOverflow);
    }

    if (!mWidthLimitSet) {
        abp->GetEmbeddedMenuWidthLimit(&mWidthLimit);
    }

    // Measure for initial configuration
    if (!mMaxItemsSet) {
        abp->GetMaxActionButtons(&mMaxItems);
    }

    Int32 width = mWidthLimit;
    if (mReserveOverflow) {
        if (mOverflowButton == NULL) {
            AutoPtr<OverflowMenuButton> omb = new OverflowMenuButton();
            omb->constructor(mSystemContext, this);
            mOverflowButton = (IView*)omb.Get();
            const Int32 spec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(0, Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);
            mOverflowButton->Measure(spec, spec);
        }

        Int32 v = 0;
        mOverflowButton->GetMeasuredWidth(&v);
        width -= v;
    } else {
        mOverflowButton = NULL;
    }

    mActionItemWidthLimit = width;

    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density = 0.f;
    metrics->GetDensity(&density);
    mMinCellSize = (Int32) (ActionMenuView::MIN_CELL_SIZE * density);

    // Drop a scrap view as it may no longer reflect the proper context/config.
    mScrapActionButtonView = NULL;
    return NOERROR;
}

ECode ActionMenuPresenter::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    if (!mMaxItemsSet) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        res->GetInteger(R::integer::max_action_buttons, &mMaxItems);
    }

    if (mMenu != NULL) {
        mMenu->OnItemsChanged(TRUE);
    }

    return NOERROR;
}

ECode ActionMenuPresenter::SetWidthLimit(
    /* [in] */ Int32 width,
    /* [in] */ Boolean strict)
{
    mWidthLimit = width;
    mStrictWidthLimit = strict;
    mWidthLimitSet = TRUE;
    return NOERROR;
}

ECode ActionMenuPresenter::SetReserveOverflow(
    /* [in] */ Boolean reserveOverflow)
{
    mReserveOverflow = reserveOverflow;
    mReserveOverflowSet = TRUE;
    return NOERROR;
}

ECode ActionMenuPresenter::SetItemLimit(
    /* [in] */ Int32 itemCount)
{
    mMaxItems = itemCount;
    mMaxItemsSet = TRUE;
    return NOERROR;
}

ECode ActionMenuPresenter::SetExpandedActionViewsExclusive(
    /* [in] */ Boolean isExclusive)
{
    mExpandedActionViewsExclusive = isExclusive;
    return NOERROR;
}

ECode ActionMenuPresenter::GetMenuView(
    /* [in] */ IViewGroup* root,
    /* [out] */ IMenuView** view)
{
    assert(view != NULL);
    BaseMenuPresenter::GetMenuView(root, view);
    AutoPtr<IActionMenuView> temp = IActionMenuView::Probe(*view);
    temp->SetPresenter(this);
    return NOERROR;
}

ECode ActionMenuPresenter::GetItemView(
    /* [in] */ IMenuItemImpl* item,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    assert(view != NULL && item != NULL);
    AutoPtr<IView> actionView;
    IMenuItem::Probe(item)->GetActionView((IView**)&actionView);

    Boolean has = FALSE;
    if (actionView == NULL || (item->HasCollapsibleActionView(&has), has)) {
        BaseMenuPresenter::GetItemView(item, convertView, parent, (IView**)&actionView);
    }

    Boolean expanded = FALSE;
    actionView->SetVisibility((IMenuItem::Probe(item)->IsActionViewExpanded(&expanded), expanded) ? IView::GONE : IView::VISIBLE);

    AutoPtr<IActionMenuView> menuParent = IActionMenuView::Probe(parent);
    AutoPtr<IViewGroupLayoutParams> lp;
    actionView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    ActionMenuView* impl = (ActionMenuView*)menuParent.Get();
    if (!impl->CheckLayoutParams(lp)) {
        actionView->SetLayoutParams(impl->GenerateLayoutParams(lp));
    }

    *view = actionView;
    REFCOUNT_ADD(*view);

    return NOERROR;
}

ECode ActionMenuPresenter::BindItemView(
    /* [in] */ IMenuItemImpl* item,
    /* [in] */ IMenuItemView* itemView)
{
    itemView->Initialize(item, 0);

    AutoPtr<IMenuBuilderItemInvoker> menuView = IMenuBuilderItemInvoker::Probe(mMenuView);
    AutoPtr<IActionMenuItemView> actionItemView = IActionMenuItemView::Probe(itemView);
    actionItemView->SetItemInvoker(menuView);

    if (mPopupCallback == NULL) {
        mPopupCallback = new ActionMenuPopupCallback(this);
    }
    return actionItemView->SetPopupCallback(mPopupCallback);
}

ECode ActionMenuPresenter::ShouldIncludeItem(
    /* [in] */ Int32 childIndex,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    assert(item != NULL);
    return item->IsActionButton(result);
}

ECode ActionMenuPresenter::UpdateMenuView(
    /* [in] */ Boolean cleared)
{
    AutoPtr<IView> mv = IView::Probe(mMenuView);
    AutoPtr<IViewParent> tmp;
    mv->GetParent((IViewParent**)&tmp);
    AutoPtr<IViewGroup> menuViewParent = IViewGroup::Probe(tmp);
    if (menuViewParent != NULL) {
        ActionBarTransition::BeginDelayedTransition(menuViewParent);
    }

    BaseMenuPresenter::UpdateMenuView(cleared);
    mv->RequestLayout();

    if (mMenu != NULL) {
        AutoPtr<IArrayList> actionItems;
        mMenu->GetActionItems((IArrayList**)&actionItems);
        AutoPtr<IIterator> it;
        actionItems->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext)
        {
            AutoPtr<IInterface> tmp;
            it->GetNext((IInterface**)&tmp);
            AutoPtr<IMenuItemImpl> impl = IMenuItemImpl::Probe(tmp);
            AutoPtr<IActionProvider> provider;
            IMenuItem::Probe(impl)->GetActionProvider((IActionProvider**)&provider);
            if (provider != NULL) {
                AutoPtr<ISubUiVisibilityListener> l = new MySubUiVisibilityListener(this);
                provider->SetSubUiVisibilityListener(l);
            }
        }
    }

    AutoPtr<IArrayList> nonActionItems;
    if (mMenu != NULL) {
        mMenu->GetNonActionItems((IArrayList**)&nonActionItems);
    }

    Boolean hasOverflow = FALSE;
    if (mReserveOverflow && nonActionItems != NULL) {
        Int32 count;
        nonActionItems->GetSize(&count);
        if (count == 1) {
            AutoPtr<IInterface> tmp;
            nonActionItems->Get(0, (IInterface**)&tmp);
            AutoPtr<IMenuItem> impl = IMenuItem::Probe(tmp);
            hasOverflow = !(impl->IsActionViewExpanded(&hasOverflow), hasOverflow);
        } else {
            hasOverflow = count > 0;
        }
    }

    if (mOverflowButton == NULL) {
        AutoPtr<OverflowMenuButton> omb = new OverflowMenuButton();
        omb->constructor(mSystemContext, this);
        mOverflowButton = (IView*)omb.Get();
    }

    AutoPtr<IViewParent> par;
    mOverflowButton->GetParent((IViewParent**)&par);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(par);
    if (hasOverflow) {
        if (parent.Get() != IViewGroup::Probe(mMenuView)) {
            if (parent != NULL) {
                assert(IViewManager::Probe(parent) != NULL);
                IViewManager::Probe(parent)->RemoveView(mOverflowButton);
            }

            AutoPtr<IActionMenuView> menuView = IActionMenuView::Probe(mMenuView);
            AutoPtr<IActionMenuViewLayoutParams> lp;
            menuView->GenerateOverflowButtonLayoutParams((IActionMenuViewLayoutParams**)&lp);
            IViewGroup::Probe(menuView)->AddView(mOverflowButton, IViewGroupLayoutParams::Probe(lp));
        }
    } else if (mOverflowButton != NULL && parent.Get() == IViewGroup::Probe(mMenuView)) {
        assert(IViewManager::Probe(parent) != NULL);
        IViewManager::Probe(parent)->RemoveView(mOverflowButton);
    }

    AutoPtr<IActionMenuView> menuView = IActionMenuView::Probe(mMenuView);
    menuView->SetOverflowReserved(mReserveOverflow);

    return NOERROR;
}

Boolean ActionMenuPresenter::FilterLeftoverView(
    /* [in] */ IViewGroup* parent,
    /* [in] */ Int32 childIndex)
{
    assert(parent != NULL);
    AutoPtr<IView> child;
    parent->GetChildAt(childIndex, (IView**)&child);
    if (child == mOverflowButton) return FALSE;
    return BaseMenuPresenter::FilterLeftoverView(parent, childIndex);
}

ECode ActionMenuPresenter::FilterLeftoverView(
    /* [in] */ IViewGroup* parent,
    /* [in] */ Int32 childIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FilterLeftoverView(parent, childIndex);
    return NOERROR;
}

ECode ActionMenuPresenter::OnSubMenuSelected(
    /* [in] */ ISubMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(subMenu != NULL);

    if (IMenu::Probe(subMenu)->HasVisibleItems(result), !(*result)) {
        return NOERROR;
    }

    AutoPtr<ISubMenuBuilder> topSubMenu = subMenu;
    AutoPtr<IMenu> parentMenu;
    while ((topSubMenu->GetParentMenu((IMenu**)&parentMenu), parentMenu.Get()) != IMenu::Probe(mMenu)) {
        topSubMenu = ISubMenuBuilder::Probe(parentMenu);
        parentMenu = NULL;
    }

    AutoPtr<IMenuItem> item;
    (ISubMenu::Probe(topSubMenu))->GetItem((IMenuItem**)&item);
    AutoPtr<IView> anchor = FindViewForItem(item);
    if (anchor == NULL) {
        if (mOverflowButton == NULL) {
            *result = FALSE;
            return NOERROR;
        }

        anchor = mOverflowButton;
    }

    item = NULL;
    ISubMenu::Probe(subMenu)->GetItem((IMenuItem**)&item);
    assert(item != NULL);

    item->GetItemId(&mOpenSubMenuId);
    mActionButtonPopup = new ActionButtonSubmenu();
    mActionButtonPopup->constructor(mContext, subMenu, this);
    mActionButtonPopup->SetAnchorView(anchor);
    mActionButtonPopup->Show();

    BaseMenuPresenter::OnSubMenuSelected(subMenu, result);

    *result = TRUE;
    return NOERROR;
}

AutoPtr<IView> ActionMenuPresenter::FindViewForItem(
    /* [in] */ IMenuItem* item)
{
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(mMenuView);
    if (parent == NULL) return NULL;

    Int32 count = 0;
    parent->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        parent->GetChildAt(i, (IView**)&child);


        if (IMenuItemView::Probe(child) != NULL)
        {
            AutoPtr<IMenuItemImpl> itemTemp;
            IMenuItemView::Probe(child)->GetItemData((IMenuItemImpl**)&itemTemp);
            AutoPtr<IMenuItem> tmp = IMenuItem::Probe(itemTemp);
            if(tmp.Get() == item)
            {
                return child;
            }
        }
    }

    return NULL;
}

ECode ActionMenuPresenter::ShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    Boolean overflowMenuShowing;
    if (mReserveOverflow && (IsOverflowMenuShowing(&overflowMenuShowing), !overflowMenuShowing) &&
            mMenu != NULL && mMenuView != NULL && mPostedOpenRunnable == NULL) {
        AutoPtr<IArrayList> nonActionItems;
        mMenu->GetNonActionItems((IArrayList**)&nonActionItems);
        Boolean empty;
        if (nonActionItems->IsEmpty(&empty), !empty) {
            AutoPtr<OverflowPopup> popup = new OverflowPopup();
            popup->constructor(mContext, mMenu, mOverflowButton, TRUE, this);
            mPostedOpenRunnable = new OpenOverflowRunnable(popup, this);
            // Post this for later; we might still need a layout for the anchor to be right.
            Boolean tmp = FALSE;
            IView::Probe(mMenuView)->Post(mPostedOpenRunnable, &tmp);

            // ActionMenuPresenter uses null as a callback argument here
            // to indicate overflow is opening.
            BaseMenuPresenter::OnSubMenuSelected(NULL, &tmp);

            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode ActionMenuPresenter::HideOverflowMenu(
    /* [out] */ Boolean* result)
{
    if (mPostedOpenRunnable != NULL && mMenuView != NULL) {
        Boolean tmp = FALSE;
        IView::Probe(mMenuView)->RemoveCallbacks(mPostedOpenRunnable, &tmp);

        mPostedOpenRunnable = NULL;
        *result = TRUE;
        return NOERROR;
    }

    if (mOverflowPopup != NULL) {
        mOverflowPopup->Dismiss();
        *result = TRUE;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode ActionMenuPresenter::DismissPopupMenus(
    /* [out] */ Boolean* result)
{
    HideOverflowMenu(result);
    Boolean tmp;
    HideSubMenus(&tmp);
    *result |= tmp;
    return NOERROR;
}

ECode ActionMenuPresenter::HideSubMenus(
    /* [out] */ Boolean* result)
{
    if (mActionButtonPopup != NULL) {
        mActionButtonPopup->Dismiss();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode ActionMenuPresenter::IsOverflowMenuShowing(
    /* [out] */ Boolean* result)
{
    Boolean isShowing;
    *result = mOverflowPopup != NULL && (mOverflowPopup->IsShowing(&isShowing), isShowing);
    return NOERROR;
}

ECode ActionMenuPresenter::IsOverflowMenuShowPending(
    /* [out] */ Boolean* result)
{
    Boolean overflowMenuShowing;
    *result = mPostedOpenRunnable != NULL || (IsOverflowMenuShowing(&overflowMenuShowing), overflowMenuShowing);
    return NOERROR;
}

ECode ActionMenuPresenter::IsOverflowReserved(
    /* [out] */ Boolean* result)
{
    *result = mReserveOverflow;
    return NOERROR;
}

ECode ActionMenuPresenter::FlagActionItems(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> visibleItems;
    mMenu->GetVisibleItems((IArrayList**)&visibleItems);
    AutoPtr<IIterator> it;
    visibleItems->GetIterator((IIterator**)&it);
    Int32 maxActions = mMaxItems;
    Int32 widthLimit = mActionItemWidthLimit;
    const Int32 querySpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(0, Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED);
    AutoPtr<IViewGroup> parent = IViewGroup::Probe(mMenuView);

    Int32 requiredItems = 0;
    Int32 requestedItems = 0;
    Int32 firstActionWidth = 0;
    Boolean hasOverflow = FALSE;
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IMenuItemImpl> item = IMenuItemImpl::Probe(next);

        Boolean tmp = FALSE;
        if (item->RequiresActionButton(&tmp), tmp) {
            requiredItems++;
        } else if (item->RequestsActionButton(&tmp), tmp) {
            requestedItems++;
        } else {
            hasOverflow = TRUE;
        }

        if (mExpandedActionViewsExclusive && (IMenuItem::Probe(item)->IsActionViewExpanded(&tmp), tmp)) {
            // Overflow everything if we have an expanded action view and we're
            // space constrained.
            maxActions = 0;
        }
    }

    // Reserve a spot for the overflow item if needed.
    if (mReserveOverflow &&
            (hasOverflow || requiredItems + requestedItems > maxActions)) {
        maxActions--;
    }
    maxActions -= requiredItems;

    mActionButtonGroups.Clear();

    Int32 cellSize = 0;
    Int32 cellsRemaining = 0;
    if (mStrictWidthLimit) {
        cellsRemaining = widthLimit / mMinCellSize;
        const Int32 cellSizeRemaining = widthLimit % mMinCellSize;
        cellSize = mMinCellSize + cellSizeRemaining / cellsRemaining;
    }

    // Flag as many more requested items as will fit.
    it = NULL;
    visibleItems->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IMenuItemImpl> item = IMenuItemImpl::Probe(next);

        Boolean tmp = FALSE;
        if (item->RequiresActionButton(&tmp), tmp) {
            AutoPtr<IView> v;
            GetItemView(item, mScrapActionButtonView, parent, (IView**)&v);
            if (mScrapActionButtonView == NULL) {
                mScrapActionButtonView = v;
            }

            if (mStrictWidthLimit) {
                cellsRemaining -= ActionMenuView::MeasureChildForCells(v,
                        cellSize, cellsRemaining, querySpec, 0);
            } else {
                v->Measure(querySpec, querySpec);
            }

            Int32 measuredWidth = 0;
            v->GetMeasuredWidth(&measuredWidth);

            widthLimit -= measuredWidth;
            if (firstActionWidth == 0) {
                firstActionWidth = measuredWidth;
            }

            Int32 groupId = 0;
            IMenuItem::Probe(item)->GetGroupId(&groupId);
            if (groupId != 0) {
                mActionButtonGroups[groupId] = TRUE;
            }

            item->SetIsActionButton(TRUE);
        } else if (item->RequestsActionButton(&tmp), tmp) {
            // Items in a group with other items that already have an action slot
            // can break the max actions rule, but not the width limit.
            Int32 groupId = 0;
            IMenuItem::Probe(item)->GetGroupId(&groupId);
            Boolean inGroup = mActionButtonGroups[groupId];
            Boolean isAction = (maxActions > 0 || inGroup) && widthLimit > 0 &&
                    (!mStrictWidthLimit || cellsRemaining > 0);

            if (isAction) {
                AutoPtr<IView> v;
                GetItemView(item, mScrapActionButtonView, parent, (IView**)&v);
                if (mScrapActionButtonView == NULL) {
                    mScrapActionButtonView = v;
                }

                if (mStrictWidthLimit) {
                    // const Int32 cells = ActionMenuView::MeasureChildForCells(v,
                    //         cellSize, cellsRemaining, querySpec, 0);
                    // cellsRemaining -= cells;
                    // if (cells == 0) {
                    //     isAction = FALSE;
                    // }
                } else {
                    v->Measure(querySpec, querySpec);
                }

                Int32 measuredWidth = 0;
                v->GetMeasuredWidth(&measuredWidth);
                widthLimit -= measuredWidth;
                if (firstActionWidth == 0) {
                    firstActionWidth = measuredWidth;
                }

                if (mStrictWidthLimit) {
                    isAction &= widthLimit >= 0;
                } else {
                    // Did this push the entire first item past the limit?
                    isAction &= widthLimit + firstActionWidth > 0;
                }
            }

            if (isAction && groupId != 0) {
                mActionButtonGroups[groupId] = TRUE;
            } else if (inGroup) {
                // We broke the width limit. Demote the whole group, they all overflow now.
                mActionButtonGroups[groupId] = FALSE;
                AutoPtr<IIterator> it2;
                visibleItems->GetIterator((IIterator**)&it2);
                Boolean hasNext;
                while(it2->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> next;
                    it2->GetNext((IInterface**)&next);
                    AutoPtr<IMenuItemImpl> areYouMyGroupie = IMenuItemImpl::Probe(next);

                    Int32 id = 0;
                    if ((IMenuItem::Probe(areYouMyGroupie)->GetGroupId(&id), id) == groupId) {
                        // Give back the action slot
                        Boolean tmp = FALSE;
                        if (areYouMyGroupie->IsActionButton(&tmp), tmp) maxActions++;

                        areYouMyGroupie->SetIsActionButton(FALSE);
                    }
                }
            }

            if (isAction) maxActions--;

            item->SetIsActionButton(isAction);
        } else {
            // Neither requires nor requests an action button.
            item->SetIsActionButton(FALSE);
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode ActionMenuPresenter::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    Boolean tmp;
    DismissPopupMenus(&tmp);
    BaseMenuPresenter::OnCloseMenu(menu, allMenusAreClosing);
    return NOERROR;
}

ECode ActionMenuPresenter::OnSaveInstanceState(
    /* [out] */ IParcelable** parcel)
{
    VALIDATE_NOT_NULL(parcel)

    CActionMenuPresenterSavedState::New(parcel);
    ((SavedState*)(*parcel))->mOpenSubMenuId = mOpenSubMenuId;
    return NOERROR;
}

ECode ActionMenuPresenter::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    SavedState* saved = (SavedState*)state;

    if (saved->mOpenSubMenuId > 0) {
        AutoPtr<IMenuItem> item;
        IMenu::Probe(mMenu)->FindItem(saved->mOpenSubMenuId, (IMenuItem**)&item);
        if (item != NULL) {
            AutoPtr<ISubMenu> mTemp;
            item->GetSubMenu((ISubMenu**)&mTemp);
            AutoPtr<ISubMenuBuilder> subMenu = ISubMenuBuilder::Probe(mTemp);
            Boolean temp;
            OnSubMenuSelected(subMenu, &temp);
        }
    }
    return NOERROR;
}

ECode ActionMenuPresenter::SetMenuView(
    /* [in] */ IActionMenuView* menuView)
{
    mMenuView = IMenuView::Probe(menuView);
    return menuView->Initialize(mMenu);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
