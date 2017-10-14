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

#ifndef __ELASTOS_DROID_WIDGET_ACTIONMENUPRESENTER_H__
#define __ELASTOS_DROID_WIDGET_ACTIONMENUPRESENTER_H__

#include "elastos/droid/widget/ImageButton.h"
#include "elastos/droid/widget/ListPopupWindow.h"
#include "elastos/droid/internal/view/menu/BaseMenuPresenter.h"
#include "elastos/droid/internal/view/menu/MenuPopupHelper.h"
#include <elastos/core/Runnable.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::ISubUiVisibilityListener;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::Internal::View::Menu::MenuPopupHelper;
using Elastos::Droid::Internal::View::Menu::BaseMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IPopupCallback;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Internal::View::Menu::ISubMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuView;
using Elastos::Droid::Internal::View::Menu::IMenuItemView;
using Elastos::Droid::Internal::View::Menu::IMenuItemImpl;
using Elastos::Droid::Internal::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IOnGlobalLayoutListener;
using Elastos::Droid::View::IViewOnAttachStateChangeListener;

namespace Elastos {
namespace Droid {
namespace Widget {

class ActionMenuPresenter
    : public BaseMenuPresenter
    , public IActionMenuPresenter
{
public:
    class SavedState
        : public Object
        , public IParcelable
    {
    public:
        CAR_INTERFACE_DECL()

        SavedState();

        CARAPI constructor();

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

    public:
        Int32 mOpenSubMenuId;
    };

private:
    class MyForwardingListener
        : public ListPopupWindow::ForwardingListener
    {
    public:
        MyForwardingListener(
            /* [in] */ IView* host,
            /* [in] */ ActionMenuPresenter* hostEx);

        CARAPI GetPopup(
            /* [out] */ IListPopupWindow** window);

        CARAPI_(Boolean) OnForwardingStarted();

        CARAPI_(Boolean) OnForwardingStopped();

    private:
        ActionMenuPresenter* mHostEx;
    };

    class OverflowMenuButton
        : public ImageButton
        , public IActionMenuChildView
    {
    public:
        CAR_INTERFACE_DECL()

        OverflowMenuButton();

        ~OverflowMenuButton();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ ActionMenuPresenter* host);

        //@Override
        CARAPI PerformClick(
            /* [out] */ Boolean* res);

        CARAPI NeedsDividerBefore(
            /* [out] */ Boolean* rst);

        CARAPI NeedsDividerAfter(
            /* [out] */ Boolean* rst);

        CARAPI OnInitializeAccessibilityNodeInfo(
            /* [in] */ IAccessibilityNodeInfo* info);

        CARAPI_(Boolean) SetFrame(
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

    protected:
        ActionMenuPresenter*  mHost;
        AutoPtr<ArrayOf<Float> > mTempPts;
    };

    class OverflowPopup
        : public MenuPopupHelper
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ IView* anchorView,
            /* [in] */ Boolean overflowOnly,
            /* [in] */ ActionMenuPresenter* host);

        CARAPI OnDismiss();

    private:
        ActionMenuPresenter*  mHost;
    };

    class ActionButtonSubmenu
        : public MenuPopupHelper
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ ISubMenuBuilder* subMenu,
            /* [in] */ ActionMenuPresenter* host);

        CARAPI OnDismiss();

    private:
        ISubMenuBuilder* mSubMenu;
        ActionMenuPresenter*  mHost;
    };

    class PopupPresenterCallback
        : public Object
        , public IMenuPresenterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        PopupPresenterCallback(
            /* [in] */ ActionMenuPresenter* host);

        //@Override
        CARAPI OnOpenSubMenu(
            /* [in] */ IMenuBuilder* subMenu,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

    private:
        ActionMenuPresenter*  mHost;
    };

    class OpenOverflowRunnable
        : public Runnable
    {
    public:
        OpenOverflowRunnable(
            /* [in] */ OverflowPopup* popup,
            /* [in] */ ActionMenuPresenter* host);

        CARAPI Run();

    private:
        AutoPtr<OverflowPopup> mPopup;
        ActionMenuPresenter* mHost;
    };

    class ActionMenuPopupCallback
        : public Object
        , public IPopupCallback
    {
    public:
        CAR_INTERFACE_DECL()

        ActionMenuPopupCallback(
            /* [in] */ ActionMenuPresenter* host);

        CARAPI GetPopup(
            /* [out] */ IListPopupWindow** popup);
    private:
        ActionMenuPresenter* mHost;
    };

    class MySubUiVisibilityListener
        : public Object
        , public ISubUiVisibilityListener
    {
    public:
        CAR_INTERFACE_DECL()

        MySubUiVisibilityListener(
            /* [in] */ ActionMenuPresenter* host);

        CARAPI OnSubUiVisibilityChanged(
            /* [in] */ Boolean isVisible);

    private:
        ActionMenuPresenter* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ActionMenuPresenter();

    ~ActionMenuPresenter();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI InitForMenu(
        /* [in] */ IContext* context,
        /* [in] */ IMenuBuilder* menu);

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI SetWidthLimit(
        /* [in] */ Int32 width,
        /* [in] */ Boolean strict);

    CARAPI SetReserveOverflow(
        /* [in] */ Boolean reserveOverflow);

    CARAPI SetItemLimit(
        /* [in] */ Int32 itemCount);

    CARAPI SetExpandedActionViewsExclusive(
        /* [in] */ Boolean isExclusive);

    //@Override
    CARAPI GetMenuView(
        /* [in] */ IViewGroup* root,
        /* [out] */ IMenuView** view);

    //@Override
    CARAPI GetItemView(
        /* [in] */ IMenuItemImpl* item,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    //@Override
    CARAPI BindItemView(
        /* [in] */ IMenuItemImpl* item,
        /* [in] */ IMenuItemView* itemView);

    //@Override
    CARAPI ShouldIncludeItem(
        /* [in] */ Int32 childIndex,
        /* [in] */ IMenuItemImpl* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI UpdateMenuView(
        /* [in] */ Boolean cleared);

    //@Override
    CARAPI_(Boolean) FilterLeftoverView(
        /* [in] */ IViewGroup* parent,
        /* [in] */ Int32 childIndex);

    CARAPI FilterLeftoverView(
        /* [in] */ IViewGroup* parent,
        /* [in] */ Int32 childIndex,
        /* [out] */ Boolean* filterLeftoverView);

    CARAPI OnSubMenuSelected(
        /* [in] */ ISubMenuBuilder* subMenu,
        /* [out] */ Boolean* result);

    /**
     * Display the overflow menu if one is present.
     * @return TRUE if the overflow menu was shown, FALSE otherwise.
     */
    CARAPI ShowOverflowMenu(
        /* [out] */ Boolean* result);

    /**
     * Hide the overflow menu if it is currently showing.
     *
     * @return TRUE if the overflow menu was hidden, FALSE otherwise.
     */
    CARAPI HideOverflowMenu(
        /* [out] */ Boolean* result);

    /**
     * Dismiss all popup menus - overflow and submenus.
     * @return TRUE if popups were dismissed, FALSE otherwise. (This can be because none were open.)
     */
    CARAPI DismissPopupMenus(
        /* [out] */ Boolean* result);

    /**
     * Dismiss all submenu popups.
     *
     * @return TRUE if popups were dismissed, FALSE otherwise. (This can be because none were open.)
     */
    CARAPI HideSubMenus(
        /* [out] */ Boolean* result);

    /**
     * @return TRUE if the overflow menu is currently showing
     */
    CARAPI IsOverflowMenuShowing(
        /* [out] */ Boolean* result);

    CARAPI IsOverflowMenuShowPending(
        /* [out] */ Boolean* result);

    /**
     * @return TRUE if space has been reserved in the action menu for an overflow item.
     */
    CARAPI IsOverflowReserved(
        /* [out] */ Boolean* result);

    CARAPI FlagActionItems(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnCloseMenu(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ Boolean allMenusAreClosing);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** parcel);

    //@Override
    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    CARAPI SetMenuView(
        /* [in] */ IActionMenuView* menuView);

private:
    CARAPI_(AutoPtr<IView>) FindViewForItem(
        /* [in] */ IMenuItem* item);

private:
    static String TAG;

    AutoPtr<IView> mOverflowButton;
    Boolean mReserveOverflow;
    Boolean mReserveOverflowSet;
    Int32 mWidthLimit;
    Int32 mActionItemWidthLimit;
    Int32 mMaxItems;
    Boolean mMaxItemsSet;
    Boolean mStrictWidthLimit;
    Boolean mWidthLimitSet;
    Boolean mExpandedActionViewsExclusive;

    Int32 mMinCellSize;

    // Group IDs that have been added as actions - used temporarily, allocated here for reuse.
    HashMap<Int32, Boolean> mActionButtonGroups;

    AutoPtr<IView> mScrapActionButtonView;

    AutoPtr<OverflowPopup> mOverflowPopup;
    AutoPtr<ActionButtonSubmenu> mActionButtonPopup;

    AutoPtr<OpenOverflowRunnable> mPostedOpenRunnable;
    AutoPtr<ActionMenuPopupCallback> mPopupCallback;
    AutoPtr<PopupPresenterCallback> mPopupPresenterCallback;
    Int32 mOpenSubMenuId;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_WIDGET_ACTIONMENUPRESENTER_H__
