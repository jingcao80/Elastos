
#ifndef __ELASTOS_DROID_INTERNAL_APP_TOOLBARACTIONBAR_H__
#define __ELASTOS_DROID_INTERNAL_APP_TOOLBARACTIONBAR_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/ActionBar.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/view/WindowCallbackWrapper.h"

using Elastos::Droid::App::ActionBar;
using Elastos::Droid::App::IActionBarLayoutParams;
using Elastos::Droid::App::IActionBarOnMenuVisibilityListener;
using Elastos::Droid::App::IActionBarOnNavigationListener;
using Elastos::Droid::App::IActionBarTab;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::Internal::Widget::IDecorToolbar;
using Elastos::Core::Runnable;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::WindowCallbackWrapper;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Droid::Widget::IToolbar;
using Elastos::Droid::Widget::IToolbarOnMenuItemClickListener;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class ToolbarActionBar
    : public ActionBar
    , public IToolbarActionBar
{
private:
    class MenuInvalidator : public Runnable
    {
    public:
        MenuInvalidator(
            /* [in] */ ToolbarActionBar* host);

        // @Override
        CARAPI Run();

    private:
        ToolbarActionBar* mHost;
    };

    class OnMenuItemClickListener
        : public Object
        , public IToolbarOnMenuItemClickListener
    {
    public:
        OnMenuItemClickListener(
            /* [in] */ ToolbarActionBar* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

    private:
        ToolbarActionBar* mHost;
    };

    class ToolbarCallbackWrapper
        : public WindowCallbackWrapper
    {
    public:
        ToolbarCallbackWrapper(
            /* [in] */ IWindowCallback* wrapped,
            /* [in] */ ToolbarActionBar* host);

        // @Override
        CARAPI OnPreparePanel(
            /* [in] */ Int32 featureId,
            /* [in] */ IView* view,
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* result);

    private:
        ToolbarActionBar* mHost;
    };

    class ActionMenuPresenterCallback
        : public Object
        , public IMenuPresenterCallback
    {
    public:
        ActionMenuPresenterCallback(
            /* [in] */ ToolbarActionBar* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnOpenSubMenu(
            /* [in] */ IMenuBuilder* subMenu,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

    private:
        ToolbarActionBar* mHost;
        Boolean mClosingActionMenu;
    };

    class MenuBuilderCallback
        : public Object
        , public IMenuBuilderCallback
    {
    public:
        MenuBuilderCallback(
            /* [in] */ ToolbarActionBar* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnMenuItemSelected(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnMenuModeChange(
            /* [in] */ IMenuBuilder* menu);

    private:
        ToolbarActionBar* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ToolbarActionBar();

    CARAPI constructor(
        /* [in] */ IToolbar* toolbar,
        /* [in] */ ICharSequence* title,
        /* [in] */ IWindowCallback* windowCallback);

    CARAPI GetWrappedWindowCallback(
        /* [out] */ IWindowCallback** windowCallback);

    // @Override
    CARAPI SetCustomView(
        /* [in] */ IView* view);

    // @Override
    CARAPI SetCustomView(
        /* [in] */ IView* view,
        /* [in] */ IActionBarLayoutParams* layoutParams);

    // @Override
    CARAPI SetCustomView(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetIcon(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    // @Override
    CARAPI SetLogo(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetLogo(
        /* [in] */ IDrawable* logo);

    // @Override
    CARAPI SetStackedBackgroundDrawable(
        /* [in] */ IDrawable* d);

    // @Override
    CARAPI SetSplitBackgroundDrawable(
        /* [in] */ IDrawable* d);

    // @Override
    CARAPI SetHomeButtonEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI SetElevation(
        /* [in] */ Float elevation);

    // @Override
    CARAPI GetElevation(
        /* [out] */ Float* result);

    // @Override
    CARAPI GetThemedContext(
        /* [out] */ IContext** result);

    // @Override
    CARAPI IsTitleTruncated(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetHomeAsUpIndicator(
        /* [in] */ IDrawable* indicator);

    // @Override
    CARAPI SetHomeAsUpIndicator(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetHomeActionContentDescription(
        /* [in] */ ICharSequence* description);

    // @Override
    CARAPI SetDefaultDisplayHomeAsUpEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI SetHomeActionContentDescription(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetShowHideAnimationEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* config);

    // @Override
    CARAPI StartActionMode(
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** mode);;

    // @Override
    CARAPI SetListNavigationCallbacks(
        /* [in] */ ISpinnerAdapter* adapter,
        /* [in] */ IActionBarOnNavigationListener* callback);

    // @Override
    CARAPI SetSelectedNavigationItem(
        /* [in] */ Int32 position);

    // @Override
    CARAPI GetSelectedNavigationIndex(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetNavigationItemCount(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    // @Override
    CARAPI SetTitle(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetWindowTitle(
        /* [in] */ ICharSequence* title);

    // @Override
    CARAPI SetSubtitle(
        /* [in] */ ICharSequence* subtitle);

    // @Override
    CARAPI SetSubtitle(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetDisplayOptions(
        /* [in] */ /*@DisplayOptions*/ Int32 options);

    // @Override
    CARAPI SetDisplayOptions(
        /* [in] */ /*@DisplayOptions*/ Int32 options,
        /* [in] */ /*@DisplayOptions*/ Int32 mask);

    // @Override
    CARAPI SetDisplayUseLogoEnabled(
        /* [in] */ Boolean useLogo);

    // @Override
    CARAPI SetDisplayShowHomeEnabled(
        /* [in] */ Boolean showHome);

    // @Override
    CARAPI SetDisplayHomeAsUpEnabled(
        /* [in] */ Boolean showHomeAsUp);

    // @Override
    CARAPI SetDisplayShowTitleEnabled(
        /* [in] */ Boolean showTitle);

    // @Override
    CARAPI SetDisplayShowCustomEnabled(
        /* [in] */ Boolean showCustom);

    // @Override
    CARAPI SetBackgroundDrawable(
        /* [in] */ /*@Nullable*/ IDrawable* d);

    // @Override
    CARAPI GetCustomView(
        /* [out] */ IView** result);

    // @Override
    CARAPI GetTitle(
        /* [out] */ ICharSequence** result);

    // @Override
    CARAPI GetSubtitle(
        /* [out] */ ICharSequence** result);

    // @Override
    CARAPI GetNavigationMode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetNavigationMode(
        /* [in] */ /*@NavigationMode*/ Int32 mode);

    // @Override
    CARAPI GetDisplayOptions(
        /* [out] */ Int32* result);

    // @Override
    CARAPI NewTab(
        /* [out] */ IActionBarTab** tab);

    // @Override
    CARAPI AddTab(
        /* [in] */ IActionBarTab* tab);

    // @Override
    CARAPI AddTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Boolean setSelected);

    // @Override
    CARAPI AddTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Int32 position);

    // @Override
    CARAPI AddTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Int32 position,
        /* [in] */ Boolean setSelected);

    // @Override
    CARAPI RemoveTab(
        /* [in] */ IActionBarTab* tab);

    // @Override
    CARAPI RemoveTabAt(
        /* [in] */ Int32 position);

    // @Override
    CARAPI RemoveAllTabs();

    // @Override
    CARAPI SelectTab(
        /* [in] */ IActionBarTab* tab);

    // @Override
    CARAPI GetSelectedTab(
        /* [out] */ IActionBarTab** tab);

    // @Override
    CARAPI GetTabAt(
        /* [in] */ Int32 index,
        /* [out] */ IActionBarTab** tab);

    // @Override
    CARAPI GetTabCount(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetHeight(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Show();

    // @Override
    CARAPI Hide();

    // @Override
    CARAPI IsShowing(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OpenOptionsMenu(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI InvalidateOptionsMenu(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI CollapseActionView(
        /* [out] */ Boolean* result);

    CARAPI_(void) PopulateOptionsMenu();

    // @Override
    CARAPI OnMenuKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI AddOnMenuVisibilityListener(
        /* [in] */ IActionBarOnMenuVisibilityListener* listener);

    CARAPI RemoveOnMenuVisibilityListener(
        /* [in] */ IActionBarOnMenuVisibilityListener* listener);

    CARAPI DispatchMenuVisibilityChanged(
        /* [in] */ Boolean isVisible);

private:
    static const String TAG;

    AutoPtr<IToolbar> mToolbar;
    AutoPtr<IDecorToolbar> mDecorToolbar;
    Boolean mToolbarMenuPrepared;
    AutoPtr<IWindowCallback> mWindowCallback;
    Boolean mMenuCallbackSet;

    Boolean mLastMenuVisibility;
    AutoPtr<IArrayList> mMenuVisibilityListeners;
    AutoPtr<IRunnable> mMenuInvalidator;
    AutoPtr<IToolbarOnMenuItemClickListener> mMenuClicker;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_TOOLBARACTIONBAR_H__
