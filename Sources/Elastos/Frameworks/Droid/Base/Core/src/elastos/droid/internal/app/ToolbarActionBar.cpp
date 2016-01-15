
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/app/ToolbarActionBar.h"
// #include "elastos/droid/app/CActionBarLayoutParams.h"
// #include "elastos/droid/internal/app/CNavItemSelectedListener.h"
#include "elastos/droid/view/LayoutInflater.h"
// #include "elastos/droid/widget/CToolbarWidgetWrapper.h"
#include <elastos/utility/logging/Logger.h>

// using Elastos::Droid::App::CActionBarLayoutParams;
using Elastos::Droid::App::IActionBarLayoutParams;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuPresenterCallback;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::EIID_IToolbarOnMenuItemClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
// using Elastos::Droid::Internal::Widget::CToolbarWidgetWrapper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

ToolbarActionBar::MenuInvalidator::MenuInvalidator(
    /* [in] */ ToolbarActionBar* host)
    : mHost(host)
{
}

ECode ToolbarActionBar::MenuInvalidator::Run()
{
    mHost->PopulateOptionsMenu();
    return NOERROR;
}

CAR_INTERFACE_IMPL(ToolbarActionBar::OnMenuItemClickListener, Object, IToolbarOnMenuItemClickListener)

ToolbarActionBar::OnMenuItemClickListener::OnMenuItemClickListener(
    /* [in] */ ToolbarActionBar* host)
    : mHost(host)
{
}

ECode ToolbarActionBar::OnMenuItemClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    return mHost->mWindowCallback->OnMenuItemSelected(
        IWindow::FEATURE_OPTIONS_PANEL, item, result);
}

ToolbarActionBar::ToolbarCallbackWrapper::ToolbarCallbackWrapper(
    /* [in] */ IWindowCallback* wrapped,
    /* [in] */ ToolbarActionBar* host)
    : mHost(host)
{
    WindowCallbackWrapper::constructor(wrapped);
}

ECode ToolbarActionBar::ToolbarCallbackWrapper::OnPreparePanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IView* view,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    WindowCallbackWrapper::OnPreparePanel(featureId, view, menu, result);
    if (*result && !mHost->mToolbarMenuPrepared) {
        mHost->mDecorToolbar->SetMenuPrepared();
        mHost->mToolbarMenuPrepared = TRUE;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(ToolbarActionBar::ActionMenuPresenterCallback, Object, IMenuPresenterCallback)

ToolbarActionBar::ActionMenuPresenterCallback::ActionMenuPresenterCallback(
    /* [in] */ ToolbarActionBar* host)
    : mHost(host)
    , mClosingActionMenu(TRUE)
{}

ECode ToolbarActionBar::ActionMenuPresenterCallback::OnOpenSubMenu(
    /* [in] */ IMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mHost->mWindowCallback != NULL) {
        Boolean res;
        mHost->mWindowCallback->OnMenuOpened(IWindow::FEATURE_ACTION_BAR, IMenu::Probe(subMenu), &res);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode ToolbarActionBar::ActionMenuPresenterCallback::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    if (mClosingActionMenu) {
        return NOERROR;
    }

    mClosingActionMenu = TRUE;
    mHost->mToolbar->DismissPopupMenus();
    if (mHost->mWindowCallback != NULL) {
        mHost->mWindowCallback->OnPanelClosed(IWindow::FEATURE_ACTION_BAR, IMenu::Probe(menu));
    }
    mClosingActionMenu = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ToolbarActionBar::MenuBuilderCallback, Object, IMenuBuilderCallback)

ToolbarActionBar::MenuBuilderCallback::MenuBuilderCallback(
    /* [in] */ ToolbarActionBar* host)
    : mHost(host)
{}


ECode ToolbarActionBar::MenuBuilderCallback::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ToolbarActionBar::MenuBuilderCallback::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    if (mHost->mWindowCallback != NULL) {
        Boolean res;
        if (mHost->mToolbar->IsOverflowMenuShowing(&res), res) {
            mHost->mWindowCallback->OnPanelClosed(IWindow::FEATURE_ACTION_BAR, IMenu::Probe(menu));
        }
        else if (mHost->mWindowCallback->OnPreparePanel(IWindow::FEATURE_OPTIONS_PANEL,
                NULL, IMenu::Probe(menu), &res), res) {
            mHost->mWindowCallback->OnMenuOpened(IWindow::FEATURE_ACTION_BAR, IMenu::Probe(menu), &res);
        }
    }
    return NOERROR;
}

const String ToolbarActionBar::TAG("ToolbarActionBar");

CAR_INTERFACE_IMPL(ToolbarActionBar, ActionBar, IToolbarActionBar)

ToolbarActionBar::ToolbarActionBar()
    : mToolbarMenuPrepared(FALSE)
    , mMenuCallbackSet(FALSE)
    , mLastMenuVisibility(FALSE)
{
    CArrayList::New((IArrayList**)&mMenuVisibilityListeners);
    mMenuInvalidator = new MenuInvalidator(this);
    mMenuClicker = new OnMenuItemClickListener(this);
}

ECode ToolbarActionBar::constructor(
    /* [in] */ IToolbar* toolbar,
    /* [in] */ ICharSequence* title,
    /* [in] */ IWindowCallback* windowCallback)
{
    mToolbar = toolbar;
    assert(0);
    // CToolbarWidgetWrapper::New(toolbar, FALSE, this, (IDecorToolbar**)&mDecorToolbar);
    mWindowCallback = new ToolbarCallbackWrapper(windowCallback, this);
    mDecorToolbar->SetWindowCallback(mWindowCallback);
    toolbar->SetOnMenuItemClickListener(mMenuClicker);
    mDecorToolbar->SetWindowTitle(title);
    return NOERROR;
}

ECode ToolbarActionBar::GetWrappedWindowCallback(
    /* [out] */ IWindowCallback** windowCallback)
{
    VALIDATE_NOT_NULL(windowCallback)
    *windowCallback = mWindowCallback;
    REFCOUNT_ADD(*windowCallback)
    return NOERROR;
}

ECode ToolbarActionBar::SetCustomView(
    /* [in] */ IView* view)
{
    AutoPtr<IActionBarLayoutParams> lp;
    assert(0);
    // CActionBarLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
    //     IViewGroupLayoutParams::WRAP_CONTENT, (IActionBarLayoutParams**)&lp);
    return SetCustomView(view, lp);
}

ECode ToolbarActionBar::SetCustomView(
    /* [in] */ IView* view,
    /* [in] */ IActionBarLayoutParams* layoutParams)
{
    view->SetLayoutParams(IViewGroupLayoutParams::Probe(layoutParams));
    return mDecorToolbar->SetCustomView(view);
}

ECode ToolbarActionBar::SetCustomView(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    IView::Probe(mToolbar)->GetContext((IContext**)&context);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(resId, IViewGroup::Probe(mToolbar), FALSE, (IView**)&view);
    return SetCustomView(view);
}

ECode ToolbarActionBar::SetIcon(
    /* [in] */ Int32 resId)
{
    return mDecorToolbar->SetIcon(resId);
}

ECode ToolbarActionBar::SetIcon(
    /* [in] */ IDrawable* icon)
{
    return mDecorToolbar->SetIcon(icon);
}

ECode ToolbarActionBar::SetLogo(
    /* [in] */ Int32 resId)
{
    return mDecorToolbar->SetLogo(resId);
}

ECode ToolbarActionBar::SetLogo(
    /* [in] */ IDrawable* logo)
{
    return mDecorToolbar->SetLogo(logo);
}

ECode ToolbarActionBar::SetStackedBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    // This space for rent (do nothing)
    return NOERROR;
}

ECode ToolbarActionBar::SetSplitBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    // This space for rent (do nothing)
    return NOERROR;
}

ECode ToolbarActionBar::SetHomeButtonEnabled(
    /* [in] */ Boolean enabled)
{
    // If the nav button on a Toolbar is present, it's enabled. No-op.
    return NOERROR;
}

ECode ToolbarActionBar::SetElevation(
    /* [in] */ Float elevation)
{
    return IView::Probe(mToolbar)->SetElevation(elevation);
}

ECode ToolbarActionBar::GetElevation(
    /* [out] */ Float* result)
{
    return IView::Probe(mToolbar)->GetElevation(result);
}

ECode ToolbarActionBar::GetThemedContext(
    /* [out] */ IContext** result)
{
    return IView::Probe(mToolbar)->GetContext(result);
}

ECode ToolbarActionBar::IsTitleTruncated(
    /* [out] */ Boolean* result)
{
    return ActionBar::IsTitleTruncated(result);
}

ECode ToolbarActionBar::SetHomeAsUpIndicator(
    /* [in] */ IDrawable* indicator)
{
    return mToolbar->SetNavigationIcon(indicator);
}

ECode ToolbarActionBar::SetHomeAsUpIndicator(
    /* [in] */ Int32 resId)
{
    return mToolbar->SetNavigationIcon(resId);
}

ECode ToolbarActionBar::SetHomeActionContentDescription(
    /* [in] */ ICharSequence* description)
{
    return mDecorToolbar->SetNavigationContentDescription(description);
}

ECode ToolbarActionBar::SetDefaultDisplayHomeAsUpEnabled(
    /* [in] */ Boolean enabled)
{
    // Do nothing
    return NOERROR;
}

ECode ToolbarActionBar::SetHomeActionContentDescription(
    /* [in] */ Int32 resId)
{
    return mDecorToolbar->SetNavigationContentDescription(resId);
}

ECode ToolbarActionBar::SetShowHideAnimationEnabled(
    /* [in] */ Boolean enabled)
{
    // This space for rent; no-op.
    return NOERROR;
}

ECode ToolbarActionBar::OnConfigurationChanged(
    /* [in] */ IConfiguration* config)
{
    return ActionBar::OnConfigurationChanged(config);
}

ECode ToolbarActionBar::StartActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = NULL;
    return NOERROR;
}

ECode ToolbarActionBar::SetListNavigationCallbacks(
    /* [in] */ ISpinnerAdapter* adapter,
    /* [in] */ IActionBarOnNavigationListener* callback)
{
    AutoPtr<IAdapterViewOnItemSelectedListener> listener;
    assert(0);
    // CNavItemSelectedListener::New(callback, (IAdapterViewOnItemSelectedListener**)&listener);
    return mDecorToolbar->SetDropdownParams(adapter, listener);
}

ECode ToolbarActionBar::SetSelectedNavigationItem(
    /* [in] */ Int32 position)
{
    Int32 mode;
    mDecorToolbar->GetNavigationMode(&mode);
    switch (mode) {
        case NAVIGATION_MODE_LIST:
            mDecorToolbar->SetDropdownSelectedPosition(position);
            break;
        default:
            Logger::E(TAG, "setSelectedNavigationIndex not valid for current navigation mode");
            return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode ToolbarActionBar::GetSelectedNavigationIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    return NOERROR;
}

ECode ToolbarActionBar::GetNavigationItemCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    return NOERROR;
}

ECode ToolbarActionBar::SetTitle(
    /* [in] */ ICharSequence* title)
{
    return mDecorToolbar->SetTitle(title);
}

ECode ToolbarActionBar::SetTitle(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    mDecorToolbar->GetContext((IContext**)&context);
    AutoPtr<ICharSequence> text;
    context->GetText(resId, (ICharSequence**)&text);
    return mDecorToolbar->SetTitle(resId != 0 ? text : NULL);
}

ECode ToolbarActionBar::SetWindowTitle(
    /* [in] */ ICharSequence* title)
{
    return mDecorToolbar->SetWindowTitle(title);
}

ECode ToolbarActionBar::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    return mDecorToolbar->SetSubtitle(subtitle);
}

ECode ToolbarActionBar::SetSubtitle(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    mDecorToolbar->GetContext((IContext**)&context);
    AutoPtr<ICharSequence> text;
    context->GetText(resId, (ICharSequence**)&text);
    return mDecorToolbar->SetSubtitle(resId != 0 ? text : NULL);
}

ECode ToolbarActionBar::SetDisplayOptions(
    /* [in] */ /*@DisplayOptions*/ Int32 options)
{
    return SetDisplayOptions(options, 0xffffffff);
}

ECode ToolbarActionBar::SetDisplayOptions(
    /* [in] */ /*@DisplayOptions*/ Int32 options,
    /* [in] */ /*@DisplayOptions*/ Int32 mask)
{
    Int32 currentOptions;
    mDecorToolbar->GetDisplayOptions(&currentOptions);
    return mDecorToolbar->SetDisplayOptions((options & mask) | (currentOptions & ~mask));
}

ECode ToolbarActionBar::SetDisplayUseLogoEnabled(
    /* [in] */ Boolean useLogo)
{
    return SetDisplayOptions(useLogo ? DISPLAY_USE_LOGO : 0, DISPLAY_USE_LOGO);
}

ECode ToolbarActionBar::SetDisplayShowHomeEnabled(
    /* [in] */ Boolean showHome)
{
    return SetDisplayOptions(showHome ? DISPLAY_SHOW_HOME : 0, DISPLAY_SHOW_HOME);
}

ECode ToolbarActionBar::SetDisplayHomeAsUpEnabled(
    /* [in] */ Boolean showHomeAsUp)
{
    return SetDisplayOptions(showHomeAsUp ? DISPLAY_HOME_AS_UP : 0, DISPLAY_HOME_AS_UP);
}

ECode ToolbarActionBar::SetDisplayShowTitleEnabled(
    /* [in] */ Boolean showTitle)
{
    return SetDisplayOptions(showTitle ? DISPLAY_SHOW_TITLE : 0, DISPLAY_SHOW_TITLE);
}

ECode ToolbarActionBar::SetDisplayShowCustomEnabled(
    /* [in] */ Boolean showCustom)
{
    return SetDisplayOptions(showCustom ? DISPLAY_SHOW_CUSTOM : 0, DISPLAY_SHOW_CUSTOM);
}

ECode ToolbarActionBar::SetBackgroundDrawable(
    /* [in] */ /*@Nullable*/ IDrawable* d)
{
    return IView::Probe(mToolbar)->SetBackground(d);
}

ECode ToolbarActionBar::GetCustomView(
    /* [out] */ IView** result)
{
    return mDecorToolbar->GetCustomView(result);
}

ECode ToolbarActionBar::GetTitle(
    /* [out] */ ICharSequence** result)
{
    return mToolbar->GetTitle(result);
}

ECode ToolbarActionBar::GetSubtitle(
    /* [out] */ ICharSequence** result)
{
    return mToolbar->GetSubtitle(result);
}

ECode ToolbarActionBar::GetNavigationMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NAVIGATION_MODE_STANDARD;
    return NOERROR;
}

ECode ToolbarActionBar::SetNavigationMode(
    /* [in] */ /*@NavigationMode*/ Int32 mode)
{
    if (mode == IActionBar::NAVIGATION_MODE_TABS) {
        Logger::E(TAG, "Tabs not supported in this configuration");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mDecorToolbar->SetNavigationMode(mode);
}

ECode ToolbarActionBar::GetDisplayOptions(
    /* [out] */ Int32* result)
{
    return mDecorToolbar->GetDisplayOptions(result);
}

ECode ToolbarActionBar::NewTab(
    /* [out] */ IActionBarTab** tab)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::AddTab(
    /* [in] */ IActionBarTab* tab)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::AddTab(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Boolean setSelected)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::AddTab(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Int32 position)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::AddTab(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Int32 position,
    /* [in] */ Boolean setSelected)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::RemoveTab(
    /* [in] */ IActionBarTab* tab)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::RemoveTabAt(
    /* [in] */ Int32 position)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::RemoveAllTabs()
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::SelectTab(
    /* [in] */ IActionBarTab* tab)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::GetSelectedTab(
    /* [out] */ IActionBarTab** tab)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::GetTabAt(
    /* [in] */ Int32 index,
    /* [out] */ IActionBarTab** tab)
{
    Logger::E(TAG, "Tabs are not supported in toolbar action bars");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ToolbarActionBar::GetTabCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    return NOERROR;
}

ECode ToolbarActionBar::GetHeight(
    /* [out] */ Int32* result)
{
    return IView::Probe(mToolbar)->GetHeight(result);
}

ECode ToolbarActionBar::Show()
{
    // TODO: Consider a better transition for this.
    // Right now use no automatic transition so that the app can supply one if desired.
    return IView::Probe(mToolbar)->SetVisibility(IView::VISIBLE);
}

ECode ToolbarActionBar::Hide()
{
    // TODO: Consider a better transition for this.
    // Right now use no automatic transition so that the app can supply one if desired.
    return IView::Probe(mToolbar)->SetVisibility(IView::GONE);
}

ECode ToolbarActionBar::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 v;
    *result = (IView::Probe(mToolbar)->GetVisibility(&v), v) == IView::VISIBLE;
    return NOERROR;
}

ECode ToolbarActionBar::OpenOptionsMenu(
    /* [out] */ Boolean* result)
{
    return mToolbar->ShowOverflowMenu(result);
}

ECode ToolbarActionBar::InvalidateOptionsMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean res;
    IView::Probe(mToolbar)->RemoveCallbacks(mMenuInvalidator, &res);
    IView::Probe(mToolbar)->PostOnAnimation(mMenuInvalidator);
    *result = TRUE;
    return NOERROR;
}

ECode ToolbarActionBar::CollapseActionView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean res;
    if (mToolbar->HasExpandedActionView(&res), res) {
        mToolbar->CollapseActionView();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

void ToolbarActionBar::PopulateOptionsMenu()
{
    if (!mMenuCallbackSet) {
        AutoPtr<ActionMenuPresenterCallback> callback1 = new ActionMenuPresenterCallback(this);
        AutoPtr<MenuBuilderCallback> callback2 = new MenuBuilderCallback(this);
        mToolbar->SetMenuCallbacks(callback1, callback2);
        mMenuCallbackSet = TRUE;
    }
    AutoPtr<IMenu> menu;
    mToolbar->GetMenu((IMenu**)&menu);
    AutoPtr<IMenuBuilder> mb = IMenuBuilder::Probe(menu);
    if (mb != NULL) {
        mb->StopDispatchingItemsChanged();
    }
    // try {
        menu->Clear();
        Boolean res1, res2;
        if (!(mWindowCallback->OnCreatePanelMenu(IWindow::FEATURE_OPTIONS_PANEL, menu, &res1), res1) ||
            !(mWindowCallback->OnPreparePanel(IWindow::FEATURE_OPTIONS_PANEL, NULL, menu, &res2), res2)) {
            menu->Clear();
        }
    // } finally {
        if (mb != NULL) {
            mb->StartDispatchingItemsChanged();
        }
    // }
}

ECode ToolbarActionBar::OnMenuKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action;
    event->GetAction(&action);
    if (action == IKeyEvent::ACTION_UP) {
        Boolean res;
        OpenOptionsMenu(&res);
    }
    *result = TRUE;
    return NOERROR;
}

ECode ToolbarActionBar::AddOnMenuVisibilityListener(
    /* [in] */ IActionBarOnMenuVisibilityListener* listener)
{
    return mMenuVisibilityListeners->Add(listener);
}

ECode ToolbarActionBar::RemoveOnMenuVisibilityListener(
    /* [in] */ IActionBarOnMenuVisibilityListener* listener)
{
    return mMenuVisibilityListeners->Remove(listener);
}

ECode ToolbarActionBar::DispatchMenuVisibilityChanged(
    /* [in] */ Boolean isVisible)
{
    if (isVisible == mLastMenuVisibility) {
        return NOERROR;
    }
    mLastMenuVisibility = isVisible;

    Int32 count;
    mMenuVisibilityListeners->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> item;
        mMenuVisibilityListeners->Get(i, (IInterface**)&item);
        IActionBarOnMenuVisibilityListener::Probe(item)->OnMenuVisibilityChanged(isVisible);
    }
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
