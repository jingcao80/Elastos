
#ifndef __ELASTOS_DROID_INTERNAL_APP_WINDOWDECORACTIONBAR_H__
#define __ELASTOS_DROID_INTERNAL_APP_WINDOWDECORACTIONBAR_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/app/ActionBar.h"
#include "elastos/droid/view/ActionMode.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::App::ActionBar;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActionBarLayoutParams;
using Elastos::Droid::App::IActionBarOnMenuVisibilityListener;
using Elastos::Droid::App::IActionBarOnNavigationListener;
using Elastos::Droid::App::IActionBarTab;
using Elastos::Droid::App::IActionBarTabListener;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::ISubMenuBuilder;
using Elastos::Droid::Internal::Widget::IActionBarContainer;
using Elastos::Droid::Internal::Widget::IActionBarContextView;
using Elastos::Droid::Internal::Widget::IActionBarOverlayLayout;
using Elastos::Droid::Internal::Widget::IActionBarVisibilityCallback;
using Elastos::Droid::Internal::Widget::IDecorToolbar;
using Elastos::Droid::Internal::Widget::IScrollingTabContainerView;
using Elastos::Droid::View::ActionMode;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class WindowDecorActionBar
    : public ActionBar
    , public IWindowDecorActionBar
{
public:
    /**
     * @hide
     */
    class ActionModeImpl : public ActionMode
    {
    private:
        class CallbackWrapper
            : public Object
            , public IMenuBuilderCallback
        {
        public:
            CallbackWrapper(
                /* [in] */ ActionModeImpl* host);

            CAR_INTERFACE_DECL()

            CARAPI OnMenuItemSelected(
                /* [in] */ IMenuBuilder* menu,
                /* [in] */ IMenuItem* item,
                /* [out] */ Boolean* result);

            CARAPI OnMenuModeChange(
                /* [in] */ IMenuBuilder* menu);

        private:
            ActionModeImpl* mHost;
        };

    public:
        ActionModeImpl(
            /* [in] */ IActionModeCallback* callback,
            /* [in] */ WindowDecorActionBar* host);

        // @Override
        CARAPI GetMenuInflater(
            /* [out] */ IMenuInflater** menuInflater);

        // @Override
        CARAPI GetMenu(
            /* [out] */ IMenu** menu);

        // @Override
        CARAPI Finish();

        // @Override
        CARAPI Invalidate();

        CARAPI DispatchOnCreate(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SetCustomView(
            /* [in] */ IView* view);

        // @Override
        CARAPI SetSubtitle(
            /* [in] */ ICharSequence* subtitle);

        // @Override
        CARAPI SetTitle(
            /* [in] */ ICharSequence* title);

        // @Override
        CARAPI SetTitle(
            /* [in] */ Int32 resId);

        // @Override
        CARAPI SetSubtitle(
            /* [in] */ Int32 resId);

        // @Override
        CARAPI GetTitle(
            /* [out] */ ICharSequence** title);

        // @Override
        CARAPI GetSubtitle(
            /* [out] */ ICharSequence** title);

        // @Override
        CARAPI SetTitleOptionalHint(
            /* [in] */ Boolean titleOptional);

        // @Override
        CARAPI IsTitleOptional(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetCustomView(
            /* [out] */ IView** view);

        CARAPI OnMenuItemSelected(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

        CARAPI OnSubMenuSelected(
            /* [in] */ ISubMenuBuilder* subMenu,
            /* [out] */ Boolean* result);

        CARAPI OnCloseSubMenu(
            /* [in] */ ISubMenuBuilder* menu);

        CARAPI OnMenuModeChange(
            /* [in] */ IMenuBuilder* menu);

    private:
        AutoPtr<IActionModeCallback> mCallback;
        AutoPtr<IMenuBuilder> mMenu;
        AutoPtr<IWeakReference> mCustomView;
        WindowDecorActionBar* mHost;
    };

    /**
     * @hide
     */
    class TabImpl
        : public Object
        , public IActionBarTab
    {
    public:
        TabImpl(
            /* [in] */ WindowDecorActionBar* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI GetTag(
            /* [out] */ IInterface** tag);

        // @Override
        CARAPI SetTag(
            /* [in] */ IInterface* tag);

        CARAPI_(AutoPtr<IActionBarTabListener>) GetCallback();

        // @Override
        CARAPI SetTabListener(
            /* [in] */ IActionBarTabListener* callback);

        // @Override
        CARAPI GetCustomView(
            /* [out] */ IView** view);

        // @Override
        CARAPI SetCustomView(
            /* [in] */ IView* view);

        // @Override
        CARAPI SetCustomView(
            /* [in] */ Int32 layoutResId);

        // @Override
        CARAPI GetIcon(
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI GetPosition(
            /* [out] */ Int32* position);

        CARAPI SetPosition(
            /* [in] */ Int32 position);

        // @Override
        CARAPI GetText(
            /* [out] */ ICharSequence** text);

        // @Override
        CARAPI SetIcon(
            /* [in] */ IDrawable* icon);

        // @Override
        CARAPI SetIcon(
            /* [in] */ Int32 resId);

        // @Override
        CARAPI SetText(
            /* [in] */ ICharSequence* text);

        // @Override
        CARAPI SetText(
            /* [in] */ Int32 resId);

        // @Override
        CARAPI Select();

        // @Override
        CARAPI SetContentDescription(
            /* [in] */ Int32 resId);

        // @Override
        CARAPI SetContentDescription(
            /* [in] */ ICharSequence* contentDesc);

        // @Override
        CARAPI GetContentDescription(
            /* [out] */ ICharSequence** contentDesc);

    private:
        AutoPtr<IActionBarTabListener> mCallback;
        AutoPtr<IInterface> mTag;
        AutoPtr<IDrawable> mIcon;
        AutoPtr<ICharSequence> mText;
        AutoPtr<ICharSequence> mContentDesc;
        Int32 mPosition;
        AutoPtr<IView> mCustomView;
        WindowDecorActionBar* mHost;
    };

private:
    class HideListener : public AnimatorListenerAdapter
    {
    public:
        HideListener(
            /* [in] */ WindowDecorActionBar* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        WindowDecorActionBar* mHost;
    };

    class ShowListener : public AnimatorListenerAdapter
    {
    public:
        ShowListener(
            /* [in] */ WindowDecorActionBar* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        WindowDecorActionBar* mHost;
    };

    class UpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        UpdateListener(
            /* [in] */ WindowDecorActionBar* host);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        WindowDecorActionBar* mHost;
    };

    class ActionBarVisibilityCallback
        : public Object
        , public IActionBarVisibilityCallback
    {
    public:
        ActionBarVisibilityCallback(
            /* [in] */ WindowDecorActionBar* host);

        CAR_INTERFACE_DECL()

        CARAPI OnWindowVisibilityChanged(
            /* [in] */  Int32 visibility);

        CARAPI ShowForSystem();

        CARAPI HideForSystem();

        CARAPI EnableContentAnimations(
            /* [in] */  Boolean enabled);

        CARAPI OnContentScrollStarted();

        CARAPI OnContentScrollStopped();

    private:
        WindowDecorActionBar* mHost;
    };

public:
    WindowDecorActionBar();

    ~WindowDecorActionBar();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IActivity* activity);

    CARAPI constructor(
        /* [in] */ IDialog* dialog);

    /**
     * Only for edit mode.
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IView* layout);

    // @Override
    CARAPI SetElevation(
        /* [in] */ Float elevation);

    // @Override
    CARAPI GetElevation(
        /* [out] */ Float* elevation);

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI_(void) CompleteDeferredDestroyActionMode();

    CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    /**
     * Enables or disables animation between show/hide states.
     * If animation is disabled using this method, animations in progress
     * will be finished.
     *
     * @param enabled TRUE to animate, FALSE to not animate.
     */
    CARAPI SetShowHideAnimationEnabled(
        /* [in] */ Boolean enabled);

    CARAPI AddOnMenuVisibilityListener(
        /* [in] */ IActionBarOnMenuVisibilityListener* listener);

    CARAPI RemoveOnMenuVisibilityListener(
        /* [in] */ IActionBarOnMenuVisibilityListener* listener);

    CARAPI DispatchMenuVisibilityChanged(
        /* [in] */ Boolean isVisible);

    // @Override
    CARAPI SetCustomView(
        /* [in] */ Int32 resId);

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
    CARAPI SetHomeButtonEnabled(
        /* [in] */ Boolean enable);

    // @Override
    CARAPI SetTitle(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetSubtitle(
        /* [in] */ Int32 resId);

    CARAPI SetSelectedNavigationItem(
        /* [in] */ Int32 position);

    CARAPI RemoveAllTabs();

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    // @Override
    CARAPI SetWindowTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetSubtitle(
        /* [in] */ ICharSequence* subtitle);;

    CARAPI SetDisplayOptions(
        /* [in] */ Int32 options);

    CARAPI SetDisplayOptions(
        /* [in] */ Int32 options,
        /* [in] */ Int32 mask);

    CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* d);

    CARAPI SetStackedBackgroundDrawable(
        /* [in] */ IDrawable* d);

    CARAPI SetSplitBackgroundDrawable(
        /* [in] */ IDrawable* d);

    CARAPI GetCustomView(
        /* [out] */ IView** view);

    CARAPI GetTitle(
        /* [out] */ ICharSequence** cs);

    CARAPI GetSubtitle(
        /* [out] */ ICharSequence** cs);

    CARAPI GetNavigationMode(
        /* [out] */ Int32* result);

    CARAPI GetDisplayOptions(
        /* [out] */ Int32* result);

    CARAPI StartActionMode(
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** actionMode);

    // @Override
    CARAPI AddTab(
        /* [in] */ IActionBarTab* tab);

    // @Override
    CARAPI AddTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Int32 position);

    // @Override
    CARAPI AddTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Boolean setSelected);

    // @Override
    CARAPI AddTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Int32 position,
        /* [in] */ Boolean setSelected);

    // @Override
    CARAPI NewTab(
        /* [out] */ IActionBarTab** tab);

    // @Override
    CARAPI RemoveTab(
        /* [in] */ IActionBarTab* tab);

    // @Override
    CARAPI RemoveTabAt(
        /* [in] */ Int32 position);

    // @Override
    CARAPI SelectTab(
        /* [in] */ IActionBarTab* tab);

    // @Override
    CARAPI GetSelectedTab(
        /* [out] */ IActionBarTab** tab);

    // @Override
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    CARAPI EnableContentAnimations(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI Show();

    CARAPI ShowForSystem();

    // @Override
    CARAPI Hide();

    CARAPI HideForSystem();

    // @Override
    CARAPI SetHideOnContentScrollEnabled(
        /* [in] */ Boolean hideOnContentScroll);
    // @Override
    CARAPI IsHideOnContentScrollEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHideOffset(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetHideOffset(
        /* [in] */ Int32 offset);

    CARAPI DoShow(
        /* [in] */ Boolean fromSystem);

    CARAPI DoHide(
        /* [in] */ Boolean fromSystem);

    CARAPI IsShowing(
        /* [out] */ Boolean* result);

    CARAPI_(void) AnimateToMode(
        /* [in] */ Boolean toActionMode);

    CARAPI GetThemedContext(
        /* [out] */ IContext** context);

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
    CARAPI SetHomeActionContentDescription(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI OnContentScrollStarted();

    // @Override
    CARAPI OnContentScrollStopped();

    // @Override
    CARAPI CollapseActionView(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetCustomView(
        /* [in] */ IView* view);

    // @Override
    CARAPI SetCustomView(
        /* [in] */ IView* view,
        /* [in] */ IActionBarLayoutParams* layoutParams);

    // @Override
    CARAPI SetListNavigationCallbacks(
        /* [in] */ ISpinnerAdapter* adapter,
        /* [in] */ IActionBarOnNavigationListener* callback);

    // @Override
    CARAPI GetSelectedNavigationIndex(
        /* [out] */ Int32* index);

    // @Override
    CARAPI GetNavigationItemCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetTabCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI SetNavigationMode(
        /* [in] */ Int32 mode);

    // @Override
    CARAPI GetTabAt(
        /* [in] */ Int32 index,
        /* [out] */ IActionBarTab** tab);

    // @Override
    CARAPI SetIcon(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI HasIcon(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetLogo(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetLogo(
        /* [in] */ IDrawable* logo);

    CARAPI HasLogo(
        /* [out] */ Boolean* result);

    CARAPI SetDefaultDisplayHomeAsUpEnabled(
        /* [in] */ Boolean enable);

private:
    CARAPI Init(
        /* [in] */ IView* decor);

    CARAPI GetDecorToolbar(
        /* [in] */ IView* view,
        /* [out] */ IDecorToolbar** decorToolbar);

    CARAPI_(void) SetHasEmbeddedTabs(
        /* [in] */ Boolean hasEmbeddedTabs);

    CARAPI_(void) EnsureTabsExist();

    CARAPI CleanupTabs();

    CARAPI ConfigureTab(
        /* [in] */ IActionBarTab* tab,
        /* [in] */ Int32 position);

    CARAPI_(void) ShowForActionMode();

    CARAPI_(void) HideForActionMode();

    static CARAPI_(Boolean) CheckShowingFlags(
        /* [in] */ Boolean hiddenByApp,
        /* [in] */ Boolean hiddenBySystem,
        /* [in] */ Boolean showingForMode);

    CARAPI_(void) UpdateVisibility(
        /* [in] */ Boolean fromSystem);

public:
    AutoPtr<ActionModeImpl> mActionMode;
    AutoPtr<IActionMode> mDeferredDestroyActionMode;
    AutoPtr<IActionModeCallback> mDeferredModeDestroyCallback;
    Boolean mHideOnContentScroll;

    AutoPtr<IAnimatorListener> mHideListener;
    AutoPtr<IAnimatorListener> mShowListener;
    AutoPtr<IAnimatorUpdateListener> mUpdateListener;

private:
    static const String TAG;
    static const Int32 CONTEXT_DISPLAY_NORMAL;
    static const Int32 CONTEXT_DISPLAY_SPLIT;
    static const Int32 INVALID_POSITION ;

    IContext* mContext;
    IContext* mThemedContext;
    Boolean mHoldThemedContext;
    IActivity* mActivity;
    IDialog* mDialog;

    AutoPtr<IActionBarOverlayLayout> mOverlayLayout;
    AutoPtr<IActionBarContainer> mContainerView;
    AutoPtr<IDecorToolbar> mDecorToolbar;
    AutoPtr<IActionBarContextView> mContextView;
    AutoPtr<IActionBarContainer> mSplitView;
    AutoPtr<IView> mContentView;
    AutoPtr<IScrollingTabContainerView> mTabScrollView;

    AutoPtr<IArrayList> mTabs;

    AutoPtr<TabImpl> mSelectedTab;
    Int32 mSavedTabPosition;

    Boolean mDisplayHomeAsUpSet;

    Boolean mLastMenuVisibility;
    AutoPtr<IArrayList> mMenuVisibilityListeners;

    Int32 mContextDisplayMode;
    Boolean mHasEmbeddedTabs;

    Int32 mCurWindowVisibility;

    Boolean mContentAnimations;
    Boolean mHiddenByApp;
    Boolean mHiddenBySystem;
    Boolean mShowingForMode;

    Boolean mNowShowing;

    AutoPtr<IAnimator> mCurrentShowAnim;
    Boolean mShowHideAnimationEnabled;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_WINDOWDECORACTIONBAR_H__
