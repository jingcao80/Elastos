
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_TOOLBARWIDGETWRAPPER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_TOOLBARWIDGETWRAPPER_H__

#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::View::Menu::IActionMenuItem;
using Elastos::Droid::Internal::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::Internal::View::Menu::IBaseMenuPresenter;
using Elastos::Droid::Internal::Widget::IDecorToolbar;
using Elastos::Droid::Internal::Widget::IScrollingTabContainerView;
using Elastos::Droid::Internal::Widget::IToolbarWidgetWrapper;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::Widget::IActionMenuPresenter;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ISpinnerAdapter;
using Elastos::Droid::Widget::IToolbar;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class ToolbarWidgetWrapper
    : public Object
    , public IDecorToolbar
    , public IToolbarWidgetWrapper
{
private:
    class InnerViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerViewOnClickListener(
            /* [in] */ ToolbarWidgetWrapper* owner);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    public:
        /*const*/ AutoPtr<IActionMenuItem> mNavItem;

    private:
        ToolbarWidgetWrapper* mOwner;
    };

    class InnerAnimatorListenerAdapter1
        : public AnimatorListenerAdapter
    {
    public:
        InnerAnimatorListenerAdapter1(
            /* [in] */ ToolbarWidgetWrapper* owner);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        // @Override
        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);

    private:
        Boolean mCanceled;
        ToolbarWidgetWrapper* mOwner;
    };

    class InnerAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        InnerAnimatorListenerAdapter2(
            /* [in] */ ToolbarWidgetWrapper* owner);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        ToolbarWidgetWrapper* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    ToolbarWidgetWrapper();

    ~ToolbarWidgetWrapper();

    CARAPI constructor(
        /* [in] */ IToolbar* toolbar,
        /* [in] */ Boolean style);

    CARAPI constructor(
        /* [in] */ IToolbar* toolbar,
        /* [in] */ Boolean style,
        /* [in] */ Int32 defaultNavigationContentDescription);

    // @Override
    CARAPI SetDefaultNavigationContentDescription(
        /* [in] */ Int32 defaultNavigationContentDescription);

    // @Override
    CARAPI GetViewGroup(
        /* [out] */ IViewGroup** result);

    // @Override
    CARAPI GetContext(
        /* [out] */ IContext** result);

    // @Override
    CARAPI IsSplit(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HasExpandedActionView(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI CollapseActionView();

    // @Override
    CARAPI SetWindowCallback(
        /* [in] */ IWindowCallback* cb);

    // @Override
    CARAPI SetWindowTitle(
        /* [in] */ ICharSequence* title);

    // @Override
    CARAPI GetTitle(
        /* [out] */ ICharSequence** result);

    // @Override
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    // @Override
    CARAPI GetSubtitle(
        /* [out] */ ICharSequence** result);

    // @Override
    CARAPI SetSubtitle(
        /* [in] */ ICharSequence* subtitle);

    // @Override
    CARAPI InitProgress();

    // @Override
    CARAPI InitIndeterminateProgress();

    // @Override
    CARAPI CanSplit(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetSplitView(
        /* [in] */ IViewGroup* splitView);

    // @Override
    CARAPI SetSplitToolbar(
        /* [in] */ Boolean split);

    // @Override
    CARAPI SetSplitWhenNarrow(
        /* [in] */ Boolean splitWhenNarrow);

    // @Override
    CARAPI HasIcon(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HasLogo(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetIcon(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetIcon(
        /* [in] */ IDrawable* d);

    // @Override
    CARAPI SetLogo(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetLogo(
        /* [in] */ IDrawable* d);

    // @Override
    CARAPI CanShowOverflowMenu(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsOverflowMenuShowing(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsOverflowMenuShowPending(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ShowOverflowMenu(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HideOverflowMenu(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetMenuPrepared();

    // @Override
    CARAPI SetMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuPresenterCallback* cb);

    // @Override
    CARAPI DismissPopupMenus();

    // @Override
    CARAPI GetDisplayOptions(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetDisplayOptions(
        /* [in] */ Int32 newOpts);

    // @Override
    CARAPI SetEmbeddedTabView(
        /* [in] */ IScrollingTabContainerView* tabView);

    // @Override
    CARAPI HasEmbeddedTabs(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsTitleTruncated(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetCollapsible(
        /* [in] */ Boolean collapsible);

    // @Override
    CARAPI SetHomeButtonEnabled(
        /* [in] */ Boolean enable);

    // @Override
    CARAPI GetNavigationMode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetNavigationMode(
        /* [in] */ Int32 mode);

    // @Override
    CARAPI SetDropdownParams(
        /* [in] */ ISpinnerAdapter* adapter,
        /* [in] */ IAdapterViewOnItemSelectedListener* listener);

    // @Override
    CARAPI SetDropdownSelectedPosition(
        /* [in] */ Int32 position);

    // @Override
    CARAPI GetDropdownSelectedPosition(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetDropdownItemCount(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetCustomView(
        /* [in] */ IView* view);

    // @Override
    CARAPI GetCustomView(
        /* [out] */ IView** result);

    // @Override
    CARAPI AnimateToVisibility(
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI SetNavigationIcon(
        /* [in] */ IDrawable* icon);

    // @Override
    CARAPI SetNavigationIcon(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetDefaultNavigationIcon(
        /* [in] */ IDrawable* defaultNavigationIcon);

    // @Override
    CARAPI SetNavigationContentDescription(
        /* [in] */ ICharSequence* description);

    // @Override
    CARAPI SetNavigationContentDescription(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SaveHierarchyState(
        /* [in] */ ISparseArray* toolbarStates);

    // @Override
    CARAPI RestoreHierarchyState(
        /* [in] */ ISparseArray* toolbarStates);

private:
    CARAPI_(Int32) DetectDisplayOptions();

    CARAPI_(void) SetTitleInt(
        /* [in] */ ICharSequence* title);

    CARAPI_(void) UpdateToolbarLogo();

    CARAPI_(void) EnsureSpinner();

    CARAPI_(void) UpdateNavigationIcon();

    CARAPI_(void) UpdateHomeAccessibility();

private:
    static const String TAG;
    static const Int32 AFFECTS_LOGO_MASK = IActionBar::DISPLAY_SHOW_HOME | IActionBar::DISPLAY_USE_LOGO;
    AutoPtr<IToolbar> mToolbar;
    Int32 mDisplayOpts;
    AutoPtr<IView> mTabView;
    AutoPtr<ISpinner> mSpinner;
    AutoPtr<IView> mCustomView;
    AutoPtr<IDrawable> mIcon;
    AutoPtr<IDrawable> mLogo;
    AutoPtr<IDrawable> mNavIcon;
    Boolean mTitleSet;
    AutoPtr<ICharSequence> mTitle;
    AutoPtr<ICharSequence> mSubtitle;
    AutoPtr<ICharSequence> mHomeDescription;
    AutoPtr<IWindowCallback> mWindowCallback;
    Boolean mMenuPrepared;
    AutoPtr<IBaseMenuPresenter> mActionMenuPresenter;
    Int32 mNavigationMode;
    Int32 mDefaultNavigationContentDescription;
    AutoPtr<IDrawable> mDefaultNavigationIcon;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_TOOLBARWIDGETWRAPPER_H__

