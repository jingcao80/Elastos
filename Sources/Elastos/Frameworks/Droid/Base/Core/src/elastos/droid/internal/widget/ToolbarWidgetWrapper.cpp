
#include "elastos/droid/internal/widget/ToolbarWidgetWrapper.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/widget/CActionMenuPresenter.h"
#include "elastos/droid/widget/CSpinner.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::App::IActionBarLayoutParams;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Internal::View::Menu::IBaseMenuPresenter;
using Elastos::Droid::Internal::Widget::EIID_IDecorToolbar;
using Elastos::Droid::Internal::Widget::EIID_IToolbarWidgetWrapper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::CActionMenuPresenter;
using Elastos::Droid::Widget::CSpinner;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::IToolbarLayoutParams;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//=====================================================================
//           ToolbarWidgetWrapper::InnerViewOnClickListener
//=====================================================================
CAR_INTERFACE_IMPL(ToolbarWidgetWrapper::InnerViewOnClickListener, Object, IViewOnClickListener)

ToolbarWidgetWrapper::InnerViewOnClickListener::InnerViewOnClickListener(
    /* [in] */ ToolbarWidgetWrapper* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ToolbarWidgetWrapper::InnerViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    VALIDATE_NOT_NULL(v);
    // ==================before translated======================
    // if (mWindowCallback != null && mMenuPrepared) {
    //     mWindowCallback.onMenuItemSelected(Window.FEATURE_OPTIONS_PANEL, mNavItem);
    // }

    if (mOwner->mWindowCallback != NULL && mOwner->mMenuPrepared) {
        Boolean resTmp;
        mOwner->mWindowCallback->OnMenuItemSelected(IWindow::FEATURE_OPTIONS_PANEL, IMenuItem::Probe(mNavItem), &resTmp);
    }
    return NOERROR;
}

//=====================================================================
//         ToolbarWidgetWrapper::InnerAnimatorListenerAdapter1
//=====================================================================
ToolbarWidgetWrapper::InnerAnimatorListenerAdapter1::InnerAnimatorListenerAdapter1(
    /* [in] */ ToolbarWidgetWrapper* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ToolbarWidgetWrapper::InnerAnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // if (!mCanceled) {
    //     mToolbar.setVisibility(View.GONE);
    // }

    if (!mCanceled) {
        IView::Probe(mOwner->mToolbar)->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::InnerAnimatorListenerAdapter1::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // mCanceled = true;

    mCanceled = TRUE;
    return NOERROR;
}

//=====================================================================
//         ToolbarWidgetWrapper::InnerAnimatorListenerAdapter2
//=====================================================================
ToolbarWidgetWrapper::InnerAnimatorListenerAdapter2::InnerAnimatorListenerAdapter2(
    /* [in] */ ToolbarWidgetWrapper* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode ToolbarWidgetWrapper::InnerAnimatorListenerAdapter2::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    VALIDATE_NOT_NULL(animation);
    // ==================before translated======================
    // mToolbar.setVisibility(View.VISIBLE);

    IView::Probe(mOwner->mToolbar)->SetVisibility(IView::VISIBLE);
    return NOERROR;
}

//=====================================================================
//                         ToolbarWidgetWrapper
//=====================================================================
const String ToolbarWidgetWrapper::TAG("ToolbarWidgetWrapper");
const Int32 ToolbarWidgetWrapper::AFFECTS_LOGO_MASK;

CAR_INTERFACE_IMPL_2(ToolbarWidgetWrapper, Object, IDecorToolbar, IToolbarWidgetWrapper)

ToolbarWidgetWrapper::ToolbarWidgetWrapper()
    : mDisplayOpts(0)
    , mTitleSet(FALSE)
    , mMenuPrepared(FALSE)
    , mNavigationMode(0)
    , mDefaultNavigationContentDescription(0)
{
}

ECode ToolbarWidgetWrapper::constructor(
    /* [in] */ IToolbar* toolbar,
    /* [in] */ Boolean style)
{
    // ==================before translated======================
    // this(toolbar, style, R.string.action_bar_up_description);

    return constructor(toolbar, style, R::string::action_bar_up_description);
}

ECode ToolbarWidgetWrapper::constructor(
    /* [in] */ IToolbar* toolbar,
    /* [in] */ Boolean style,
    /* [in] */ Int32 defaultNavigationContentDescription)
{
    // ==================before translated======================
    // mToolbar = toolbar;
    //
    // mTitle = toolbar.getTitle();
    // mSubtitle = toolbar.getSubtitle();
    // mTitleSet = mTitle != null;
    // final TypedArray a = toolbar.getContext().obtainStyledAttributes(null,
    //         R.styleable.ActionBar, R.attr.actionBarStyle, 0);
    // mDefaultNavigationIcon = a.getDrawable(R.styleable.ActionBar_homeAsUpIndicator);
    // if (style) {
    //     final CharSequence title = a.getText(R.styleable.ActionBar_title);
    //     if (!TextUtils.isEmpty(title)) {
    //         setTitle(title);
    //     }
    //
    //     final CharSequence subtitle = a.getText(R.styleable.ActionBar_subtitle);
    //     if (!TextUtils.isEmpty(subtitle)) {
    //         setSubtitle(subtitle);
    //     }
    //
    //     final Drawable logo = a.getDrawable(R.styleable.ActionBar_logo);
    //     if (logo != null) {
    //         setLogo(logo);
    //     }
    //
    //     final Drawable icon = a.getDrawable(R.styleable.ActionBar_icon);
    //     if (icon != null) {
    //         setIcon(icon);
    //     }
    //     if (mDefaultNavigationIcon != null) {
    //         setNavigationIcon(mDefaultNavigationIcon);
    //     }
    //     setDisplayOptions(a.getInt(R.styleable.ActionBar_displayOptions, 0));
    //
    //     final int customNavId = a.getResourceId(
    //             R.styleable.ActionBar_customNavigationLayout, 0);
    //     if (customNavId != 0) {
    //         setCustomView(LayoutInflater.from(mToolbar.getContext()).inflate(customNavId,
    //                 mToolbar, false));
    //         setDisplayOptions(mDisplayOpts | ActionBar.DISPLAY_SHOW_CUSTOM);
    //     }
    //
    //     final int height = a.getLayoutDimension(R.styleable.ActionBar_height, 0);
    //     if (height > 0) {
    //         final ViewGroup.LayoutParams lp = mToolbar.getLayoutParams();
    //         lp.height = height;
    //         mToolbar.setLayoutParams(lp);
    //     }
    //
    //     final int contentInsetStart = a.getDimensionPixelOffset(
    //             R.styleable.ActionBar_contentInsetStart, -1);
    //     final int contentInsetEnd = a.getDimensionPixelOffset(
    //             R.styleable.ActionBar_contentInsetEnd, -1);
    //     if (contentInsetStart >= 0 || contentInsetEnd >= 0) {
    //         mToolbar.setContentInsetsRelative(Math.max(contentInsetStart, 0),
    //                 Math.max(contentInsetEnd, 0));
    //     }
    //
    //     final int titleTextStyle = a.getResourceId(R.styleable.ActionBar_titleTextStyle, 0);
    //     if (titleTextStyle != 0) {
    //         mToolbar.setTitleTextAppearance(mToolbar.getContext(), titleTextStyle);
    //     }
    //
    //     final int subtitleTextStyle = a.getResourceId(
    //             R.styleable.ActionBar_subtitleTextStyle, 0);
    //     if (subtitleTextStyle != 0) {
    //         mToolbar.setSubtitleTextAppearance(mToolbar.getContext(), subtitleTextStyle);
    //     }
    //
    //     final int popupTheme = a.getResourceId(R.styleable.ActionBar_popupTheme, 0);
    //     if (popupTheme != 0) {
    //         mToolbar.setPopupTheme(popupTheme);
    //     }
    // } else {
    //     mDisplayOpts = detectDisplayOptions();
    // }
    // a.recycle();
    //
    // setDefaultNavigationContentDescription(defaultNavigationContentDescription);
    // mHomeDescription = mToolbar.getNavigationContentDescription();
    //
    // mToolbar.setNavigationOnClickListener(new View.OnClickListener() {
    //     final ActionMenuItem mNavItem = new ActionMenuItem(mToolbar.getContext(),
    //             0, android.R.id.home, 0, 0, mTitle);
    //     @Override
    //     public void onClick(View v) {
    //         if (mWindowCallback != null && mMenuPrepared) {
    //             mWindowCallback.onMenuItemSelected(Window.FEATURE_OPTIONS_PANEL, mNavItem);
    //         }
    //     }
    // });

    mToolbar = toolbar;
    toolbar->GetTitle((ICharSequence**)&mTitle);
    toolbar->GetSubtitle((ICharSequence**)&mSubtitle);
    mTitleSet = mTitle != NULL;

    AutoPtr<IContext> context;
    IView::Probe(toolbar)->GetContext((IContext**)&context);

    Int32 styleableActionBarSize = ArraySize(R::styleable::ActionBar);
    AutoPtr< ArrayOf<Int32> > attrsFormat = ArrayOf<Int32>::Alloc(styleableActionBarSize + 2);
    for (Int32 idx=0; idx<styleableActionBarSize; ++idx) {
        attrsFormat->Set(idx, (Int32)R::styleable::ActionBar[idx]);
    }
    attrsFormat->Set(styleableActionBarSize + 1, R::attr::actionBarStyle);
    attrsFormat->Set(styleableActionBarSize + 2, 0);
    AutoPtr<ITypedArray> a;
    IAttributeSet* attrSet = NULL;
    context->ObtainStyledAttributes(attrSet, attrsFormat, (ITypedArray**)&a);

    a->GetDrawable(R::styleable::ActionBar_homeAsUpIndicator, (IDrawable**)&mDefaultNavigationIcon);
    if (style) {
        AutoPtr<ICharSequence> title;
        a->GetText(R::styleable::ActionBar_title, (ICharSequence**)&title);
        if (!TextUtils::IsEmpty(title)) {
            SetTitle(title);
        }

        AutoPtr<ICharSequence> subtitle;
        a->GetText(R::styleable::ActionBar_subtitle, (ICharSequence**)&subtitle);
        if (!TextUtils::IsEmpty(subtitle)) {
            SetSubtitle(subtitle);
        }

        AutoPtr<IDrawable> logo;
        a->GetDrawable(R::styleable::ActionBar_logo, (IDrawable**)&logo);
        if (logo != NULL) {
            SetLogo(logo);
        }

        AutoPtr<IDrawable> icon;
        a->GetDrawable(R::styleable::ActionBar_icon, (IDrawable**)&icon);
        if (icon != NULL) {
            SetIcon(icon);
        }
        if (mDefaultNavigationIcon != NULL) {
            SetNavigationIcon(mDefaultNavigationIcon);
        }

        Int32 displayOptions = 0;
        a->GetInt32(R::styleable::ActionBar_displayOptions, 0, &displayOptions);
        SetDisplayOptions(displayOptions);

        Int32 customNavId = 0;
        a->GetResourceId(R::styleable::ActionBar_customNavigationLayout, 0, &customNavId);
        AutoPtr<IContext> contextTmp;
        IView::Probe(mToolbar)->GetContext((IContext**)&contextTmp);
        if (customNavId != 0) {
            AutoPtr<ILayoutInflater> layoutInflater;
            LayoutInflater::From(contextTmp, (ILayoutInflater**)&layoutInflater);
            AutoPtr<IView> viewTmp;
            layoutInflater->Inflate(customNavId, IViewGroup::Probe(mToolbar), FALSE, (IView**)&viewTmp);
            SetCustomView(viewTmp);
            SetDisplayOptions(mDisplayOpts | IActionBar::DISPLAY_SHOW_CUSTOM);
        }

        Int32 height = 0;
        a->GetLayoutDimension(R::styleable::ActionBar_height, 0, &height);
        if (height > 0) {
            AutoPtr<IViewGroupLayoutParams> lp;
            IView::Probe(mToolbar)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            lp->SetHeight(height);
            IView::Probe(mToolbar)->SetLayoutParams(lp);
        }

        Int32 contentInsetStart = 0;
        a->GetDimensionPixelOffset(R::styleable::ActionBar_contentInsetStart, -1, &contentInsetStart);
        Int32 contentInsetEnd = 0;
        a->GetDimensionPixelOffset(R::styleable::ActionBar_contentInsetEnd, -1, &contentInsetEnd);
        if (contentInsetStart >= 0 || contentInsetEnd >= 0) {
            mToolbar->SetContentInsetsRelative(Elastos::Core::Math::Max(contentInsetStart, 0),
                Elastos::Core::Math::Max(contentInsetEnd, 0));
        }

        Int32 titleTextStyle = 0;
        a->GetResourceId(R::styleable::ActionBar_titleTextStyle, 0, &titleTextStyle);
        if (titleTextStyle != 0) {
            mToolbar->SetTitleTextAppearance(contextTmp, titleTextStyle);
        }

        Int32 subtitleTextStyle = 0;
        a->GetResourceId(R::styleable::ActionBar_subtitleTextStyle, 0, &subtitleTextStyle);
        if (subtitleTextStyle != 0) {
            mToolbar->SetSubtitleTextAppearance(contextTmp, subtitleTextStyle);
        }

        Int32 popupTheme = 0;
        a->GetResourceId(R::styleable::ActionBar_popupTheme, 0, &popupTheme);
        if (popupTheme != 0) {
            mToolbar->SetPopupTheme(popupTheme);
        }
    }
    else {
        mDisplayOpts = DetectDisplayOptions();
    }
    a->Recycle();

    SetDefaultNavigationContentDescription(defaultNavigationContentDescription);
    mToolbar->GetNavigationContentDescription((ICharSequence**)&mHomeDescription);

    AutoPtr<IViewOnClickListener> clickListener = new InnerViewOnClickListener(this);
    mToolbar->SetNavigationOnClickListener(clickListener);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetDefaultNavigationContentDescription(
    /* [in] */ Int32 defaultNavigationContentDescription)
{
    // ==================before translated======================
    // if (defaultNavigationContentDescription == mDefaultNavigationContentDescription) {
    //     return;
    // }
    // mDefaultNavigationContentDescription = defaultNavigationContentDescription;
    // if (TextUtils.isEmpty(mToolbar.getNavigationContentDescription())) {
    //     setNavigationContentDescription(mDefaultNavigationContentDescription);
    // }

    if (defaultNavigationContentDescription == mDefaultNavigationContentDescription) {
        return NOERROR;
    }
    mDefaultNavigationContentDescription = defaultNavigationContentDescription;
    AutoPtr<ICharSequence> description;
    mToolbar->GetNavigationContentDescription((ICharSequence**)&description);
    if (TextUtils::IsEmpty(description)) {
        SetNavigationContentDescription(mDefaultNavigationContentDescription);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetViewGroup(
    /* [out] */ IViewGroup** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar;

    *result = IViewGroup::Probe(mToolbar);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetContext(
    /* [out] */ IContext** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.getContext();

    IView::Probe(mToolbar)->GetContext(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::IsSplit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::HasExpandedActionView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.hasExpandedActionView();

    mToolbar->HasExpandedActionView(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::CollapseActionView()
{
    // ==================before translated======================
    // mToolbar.collapseActionView();

    mToolbar->CollapseActionView();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetWindowCallback(
    /* [in] */ IWindowCallback* cb)
{
    VALIDATE_NOT_NULL(cb);
    // ==================before translated======================
    // mWindowCallback = cb;

    mWindowCallback = cb;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetWindowTitle(
    /* [in] */ ICharSequence* title)
{
    VALIDATE_NOT_NULL(title);
    // ==================before translated======================
    // // "Real" title always trumps window title.
    // if (!mTitleSet) {
    //     setTitleInt(title);
    // }

    if (!mTitleSet) {
        SetTitleInt(title);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetTitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.getTitle();

    mToolbar->GetTitle(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetTitle(
    /* [in] */ ICharSequence* title)
{
    VALIDATE_NOT_NULL(title);
    // ==================before translated======================
    // mTitleSet = true;
    // setTitleInt(title);

    mTitleSet = TRUE;
    SetTitleInt(title);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetSubtitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.getSubtitle();

    mToolbar->GetSubtitle(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    VALIDATE_NOT_NULL(subtitle);
    // ==================before translated======================
    // mSubtitle = subtitle;
    // if ((mDisplayOpts & ActionBar.DISPLAY_SHOW_TITLE) != 0) {
    //     mToolbar.setSubtitle(subtitle);
    // }

    mSubtitle = subtitle;
    if ((mDisplayOpts & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
        mToolbar->SetSubtitle(subtitle);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::InitProgress()
{
    // ==================before translated======================
    // Log.i(TAG, "Progress display unsupported");

    Logger::I(TAG, "Progress display unsupported");
    return NOERROR;
}

ECode ToolbarWidgetWrapper::InitIndeterminateProgress()
{
    // ==================before translated======================
    // Log.i(TAG, "Progress display unsupported");

    Logger::I(TAG, "Progress display unsupported");
    return NOERROR;
}

ECode ToolbarWidgetWrapper::CanSplit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return false;

    *result = FALSE;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetSplitView(
    /* [in] */ IViewGroup* splitView)
{
    VALIDATE_NOT_NULL(splitView);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetSplitToolbar(
    /* [in] */ Boolean split)
{
    // ==================before translated======================
    // if (split) {
    //     throw new UnsupportedOperationException("Cannot split an android.widget.Toolbar");
    // }

    if (split) {
        //throw new UnsupportedOperationException("Cannot split an android.widget.Toolbar");
        Logger::I(TAG, "Cannot split an android.widget.Toolbar");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetSplitWhenNarrow(
    /* [in] */ Boolean splitWhenNarrow)
{
    // ==================before translated======================
    // // Ignore.

    // Ignore.
    return NOERROR;
}

ECode ToolbarWidgetWrapper::HasIcon(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIcon != null;

    *result = mIcon != NULL;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::HasLogo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mLogo != null;

    *result = mLogo != NULL;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetIcon(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // setIcon(resId != 0 ? getContext().getDrawable(resId) : null);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> drawable;
    context->GetDrawable(resId, (IDrawable**)&drawable);
    SetIcon(resId != 0 ? drawable : NULL);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetIcon(
    /* [in] */ IDrawable* d)
{
    VALIDATE_NOT_NULL(d);
    // ==================before translated======================
    // mIcon = d;
    // updateToolbarLogo();

    mIcon = d;
    UpdateToolbarLogo();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetLogo(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // setLogo(resId != 0 ? getContext().getDrawable(resId) : null);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> drawable;
    context->GetDrawable(resId, (IDrawable**)&drawable);
    SetLogo(resId != 0 ? drawable : NULL);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetLogo(
    /* [in] */ IDrawable* d)
{
    VALIDATE_NOT_NULL(d);
    // ==================before translated======================
    // mLogo = d;
    // updateToolbarLogo();

    mLogo = d;
    UpdateToolbarLogo();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::CanShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.canShowOverflowMenu();

    mToolbar->CanShowOverflowMenu(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::IsOverflowMenuShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.isOverflowMenuShowing();

    mToolbar->IsOverflowMenuShowing(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::IsOverflowMenuShowPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.isOverflowMenuShowPending();

    mToolbar->IsOverflowMenuShowPending(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::ShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.showOverflowMenu();

    mToolbar->ShowOverflowMenu(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::HideOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.hideOverflowMenu();

    mToolbar->HideOverflowMenu(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetMenuPrepared()
{
    // ==================before translated======================
    // mMenuPrepared = true;

    mMenuPrepared = TRUE;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuPresenterCallback* cb)
{
    VALIDATE_NOT_NULL(menu);
    VALIDATE_NOT_NULL(cb);
    // ==================before translated======================
    // if (mActionMenuPresenter == null) {
    //     mActionMenuPresenter = new ActionMenuPresenter(mToolbar.getContext());
    //     mActionMenuPresenter.setId(com.android.internal.R.id.action_menu_presenter);
    // }
    // mActionMenuPresenter.setCallback(cb);
    // mToolbar.setMenu((MenuBuilder) menu, mActionMenuPresenter);

    if (mActionMenuPresenter == NULL) {
        AutoPtr<IContext> context;
        IView::Probe(mToolbar)->GetContext((IContext**)&context);
        CActionMenuPresenter::New(context, (IActionMenuPresenter**)&mActionMenuPresenter);
        IBaseMenuPresenter::Probe(mActionMenuPresenter)->SetId(R::id::action_menu_presenter);
    }
    IBaseMenuPresenter::Probe(mActionMenuPresenter)->SetCallback(cb);
    mToolbar->SetMenu(IMenuBuilder::Probe(menu), mActionMenuPresenter);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::DismissPopupMenus()
{
    // ==================before translated======================
    // mToolbar.dismissPopupMenus();

    mToolbar->DismissPopupMenus();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetDisplayOptions(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDisplayOpts;

    *result = mDisplayOpts;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetDisplayOptions(
    /* [in] */ Int32 newOpts)
{
    // ==================before translated======================
    // final int oldOpts = mDisplayOpts;
    // final int changed = oldOpts ^ newOpts;
    // mDisplayOpts = newOpts;
    // if (changed != 0) {
    //     if ((changed & ActionBar.DISPLAY_HOME_AS_UP) != 0) {
    //         if ((newOpts & ActionBar.DISPLAY_HOME_AS_UP) != 0) {
    //             updateHomeAccessibility();
    //         }
    //         updateNavigationIcon();
    //     }
    //
    //     if ((changed & AFFECTS_LOGO_MASK) != 0) {
    //         updateToolbarLogo();
    //     }
    //
    //     if ((changed & ActionBar.DISPLAY_SHOW_TITLE) != 0) {
    //         if ((newOpts & ActionBar.DISPLAY_SHOW_TITLE) != 0) {
    //             mToolbar.setTitle(mTitle);
    //             mToolbar.setSubtitle(mSubtitle);
    //         } else {
    //             mToolbar.setTitle(null);
    //             mToolbar.setSubtitle(null);
    //         }
    //     }
    //
    //     if ((changed & ActionBar.DISPLAY_SHOW_CUSTOM) != 0 && mCustomView != null) {
    //         if ((newOpts & ActionBar.DISPLAY_SHOW_CUSTOM) != 0) {
    //             mToolbar.addView(mCustomView);
    //         } else {
    //             mToolbar.removeView(mCustomView);
    //         }
    //     }
    // }

    Int32 oldOpts = mDisplayOpts;
    Int32 changed = oldOpts ^ newOpts;
    mDisplayOpts = newOpts;
    if (changed != 0) {
        if ((changed & IActionBar::DISPLAY_HOME_AS_UP) != 0) {
            if ((newOpts & IActionBar::DISPLAY_HOME_AS_UP) != 0) {
                UpdateHomeAccessibility();
            }
            UpdateNavigationIcon();
        }

        if ((changed & AFFECTS_LOGO_MASK) != 0) {
            UpdateToolbarLogo();
        }

        if ((changed & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
            if ((newOpts & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
                mToolbar->SetTitle(mTitle);
                mToolbar->SetSubtitle(mSubtitle);
            }
            else {
                ICharSequence* charSequenceTmp = NULL;
                mToolbar->SetTitle(charSequenceTmp);
                mToolbar->SetSubtitle(charSequenceTmp);
            }
        }

        if ((changed & IActionBar::DISPLAY_SHOW_CUSTOM) != 0 && mCustomView != NULL) {
            if ((newOpts & IActionBar::DISPLAY_SHOW_CUSTOM) != 0) {
                IViewGroup::Probe(mToolbar)->AddView(mCustomView);
            }
            else {
                IViewGroup::Probe(mToolbar)->RemoveView(mCustomView);
            }
        }
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetEmbeddedTabView(
    /* [in] */ IScrollingTabContainerView* tabView)
{
    VALIDATE_NOT_NULL(tabView);
    // ==================before translated======================
    // if (mTabView != null && mTabView.getParent() == mToolbar) {
    //     mToolbar.removeView(mTabView);
    // }
    // mTabView = tabView;
    // if (tabView != null && mNavigationMode == ActionBar.NAVIGATION_MODE_TABS) {
    //     mToolbar.addView(mTabView, 0);
    //     Toolbar.LayoutParams lp = (Toolbar.LayoutParams) mTabView.getLayoutParams();
    //     lp.width = ViewGroup.LayoutParams.WRAP_CONTENT;
    //     lp.height = ViewGroup.LayoutParams.WRAP_CONTENT;
    //     lp.gravity = Gravity.START | Gravity.BOTTOM;
    //     tabView.setAllowCollapse(true);
    // }

    AutoPtr<IViewParent> viewParent;
    mTabView->GetParent((IViewParent**)&viewParent);
    if (mTabView != NULL && TO_IINTERFACE(viewParent)== TO_IINTERFACE(mToolbar)) {
        IViewGroup::Probe(mToolbar)->RemoveView(mTabView);
    }
    mTabView = IView::Probe(tabView);
    if (tabView != NULL && mNavigationMode == IActionBar::NAVIGATION_MODE_TABS) {
        IViewGroup::Probe(mToolbar)->AddView(mTabView, 0);
        AutoPtr<IViewGroupLayoutParams> layoutParams;
        mTabView->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
        layoutParams->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
        layoutParams->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
        IActionBarLayoutParams::Probe(layoutParams)->SetGravity(IGravity::START | IGravity::BOTTOM);
        tabView->SetAllowCollapse(TRUE);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::HasEmbeddedTabs(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mTabView != null;

    *result = mTabView != NULL;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::IsTitleTruncated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToolbar.isTitleTruncated();

    mToolbar->IsTitleTruncated(result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetCollapsible(
    /* [in] */ Boolean collapsible)
{
    // ==================before translated======================
    // mToolbar.setCollapsible(collapsible);

    mToolbar->SetCollapsible(collapsible);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetHomeButtonEnabled(
    /* [in] */ Boolean enable)
{
    // ==================before translated======================
    // // Ignore

    // Ignore
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetNavigationMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mNavigationMode;

    *result = mNavigationMode;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetNavigationMode(
    /* [in] */ Int32 mode)
{
    // ==================before translated======================
    // final int oldMode = mNavigationMode;
    // if (mode != oldMode) {
    //     switch (oldMode) {
    //         case ActionBar.NAVIGATION_MODE_LIST:
    //             if (mSpinner != null && mSpinner.getParent() == mToolbar) {
    //                 mToolbar.removeView(mSpinner);
    //             }
    //             break;
    //         case ActionBar.NAVIGATION_MODE_TABS:
    //             if (mTabView != null && mTabView.getParent() == mToolbar) {
    //                 mToolbar.removeView(mTabView);
    //             }
    //             break;
    //     }
    //
    //     mNavigationMode = mode;
    //
    //     switch (mode) {
    //         case ActionBar.NAVIGATION_MODE_STANDARD:
    //             break;
    //         case ActionBar.NAVIGATION_MODE_LIST:
    //             ensureSpinner();
    //             mToolbar.addView(mSpinner, 0);
    //             break;
    //         case ActionBar.NAVIGATION_MODE_TABS:
    //             if (mTabView != null) {
    //                 mToolbar.addView(mTabView, 0);
    //                 Toolbar.LayoutParams lp = (Toolbar.LayoutParams) mTabView.getLayoutParams();
    //                 lp.width = ViewGroup.LayoutParams.WRAP_CONTENT;
    //                 lp.height = ViewGroup.LayoutParams.WRAP_CONTENT;
    //                 lp.gravity = Gravity.START | Gravity.BOTTOM;
    //             }
    //             break;
    //         default:
    //             throw new IllegalArgumentException("Invalid navigation mode " + mode);
    //     }
    // }

    Int32 oldMode = mNavigationMode;
    if (mode != oldMode) {
        switch (oldMode) {
            case IActionBar::NAVIGATION_MODE_LIST:
                {
                    AutoPtr<IViewParent> viewParent;
                    IView::Probe(mSpinner)->GetParent((IViewParent**)&viewParent);
                    if (mSpinner != NULL && TO_IINTERFACE(viewParent)== TO_IINTERFACE(mToolbar)) {
                        IViewGroup::Probe(mToolbar)->RemoveView(IView::Probe(mSpinner));
                    }
                }
                break;
            case IActionBar::NAVIGATION_MODE_TABS:
                {
                    AutoPtr<IViewParent> viewParent;
                    IView::Probe(mTabView)->GetParent((IViewParent**)&viewParent);
                    if (mTabView != NULL && TO_IINTERFACE(viewParent)== TO_IINTERFACE(mToolbar)) {
                        IViewGroup::Probe(mToolbar)->RemoveView(IView::Probe(mTabView));
                    }
                }
                break;
        }

        mNavigationMode = mode;

        switch (mode) {
            case IActionBar::NAVIGATION_MODE_STANDARD:
                break;
            case IActionBar::NAVIGATION_MODE_LIST:
                {
                    EnsureSpinner();
                    IViewGroup::Probe(mToolbar)->AddView(IView::Probe(mSpinner), 0);
                }
                break;
            case IActionBar::NAVIGATION_MODE_TABS:
                {
                    if (mTabView != NULL) {
                        IViewGroup::Probe(mToolbar)->AddView(IView::Probe(mTabView), 0);
                        AutoPtr<IViewGroupLayoutParams> layoutParams;
                        IView::Probe(mTabView)->GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
                        layoutParams->SetWidth(IViewGroupLayoutParams::WRAP_CONTENT);
                        layoutParams->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);
                        IActionBarLayoutParams::Probe(layoutParams)->SetGravity(IGravity::START | IGravity::BOTTOM);
                    }
                }
                break;
            default:
                {
                    //throw new IllegalArgumentException("Invalid navigation mode " + mode);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
        }
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetDropdownParams(
    /* [in] */ ISpinnerAdapter* adapter,
    /* [in] */ IAdapterViewOnItemSelectedListener* listener)
{
    VALIDATE_NOT_NULL(adapter);
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // ensureSpinner();
    // mSpinner.setAdapter(adapter);
    // mSpinner.setOnItemSelectedListener(listener);

    EnsureSpinner();
    IAdapterView::Probe(mSpinner)->SetAdapter(IAdapter::Probe(adapter));
    IAdapterView::Probe(mSpinner)->SetOnItemSelectedListener(listener);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetDropdownSelectedPosition(
    /* [in] */ Int32 position)
{
    // ==================before translated======================
    // if (mSpinner == null) {
    //     throw new IllegalStateException(
    //             "Can't set dropdown selected position without an adapter");
    // }
    // mSpinner.setSelection(position);

    if (mSpinner == NULL) {
        // throw new IllegalStateException("Can't set dropdown selected position without an adapter");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    IAdapterView::Probe(mSpinner)->SetSelection(position);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetDropdownSelectedPosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSpinner != null ? mSpinner.getSelectedItemPosition() : 0;

    Int32 position = 0;
    IAdapterView::Probe(mSpinner)->GetSelectedItemPosition(&position);
    *result = mSpinner != NULL ? position : 0;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetDropdownItemCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSpinner != null ? mSpinner.getCount() : 0;

    Int32 count = 0;
    IAdapterView::Probe(mSpinner)->GetCount(&count);
    *result = mSpinner != NULL ? count : 0;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetCustomView(
    /* [in] */ IView* view)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // if (mCustomView != null && (mDisplayOpts & ActionBar.DISPLAY_SHOW_CUSTOM) != 0) {
    //     mToolbar.removeView(mCustomView);
    // }
    // mCustomView = view;
    // if (view != null && (mDisplayOpts & ActionBar.DISPLAY_SHOW_CUSTOM) != 0) {
    //     mToolbar.addView(mCustomView);
    // }

    if (mCustomView != NULL && (mDisplayOpts & IActionBar::DISPLAY_SHOW_CUSTOM) != 0) {
        IViewGroup::Probe(mToolbar)->RemoveView(mCustomView);
    }
    mCustomView = view;
    if (view != NULL && (mDisplayOpts & IActionBar::DISPLAY_SHOW_CUSTOM) != 0) {
        IViewGroup::Probe(mToolbar)->AddView(mCustomView);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetCustomView(
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCustomView;

    *result = mCustomView;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::AnimateToVisibility(
    /* [in] */ Int32 visibility)
{
    // ==================before translated======================
    // if (visibility == View.GONE) {
    //     mToolbar.animate().alpha(0)
    //             .setListener(new AnimatorListenerAdapter() {
    //                 private boolean mCanceled = false;
    //                 @Override
    //                 public void onAnimationEnd(Animator animation) {
    //                     if (!mCanceled) {
    //                         mToolbar.setVisibility(View.GONE);
    //                     }
    //                 }
    //
    //                 @Override
    //                 public void onAnimationCancel(Animator animation) {
    //                     mCanceled = true;
    //                 }
    //             });
    // } else if (visibility == View.VISIBLE) {
    //     mToolbar.animate().alpha(1)
    //             .setListener(new AnimatorListenerAdapter() {
    //                 @Override
    //                 public void onAnimationStart(Animator animation) {
    //                     mToolbar.setVisibility(View.VISIBLE);
    //                 }
    //             });
    // }

    AutoPtr<IViewPropertyAnimator> viewPropertyAnimator;
    IView::Probe(mToolbar)->Animate((IViewPropertyAnimator**)&viewPropertyAnimator);
    if (visibility == IView::GONE) {
        viewPropertyAnimator->Alpha(0.0f);
        AutoPtr<IAnimatorListener> listener = new InnerAnimatorListenerAdapter1(this);
        viewPropertyAnimator->SetListener(listener);
    }
    else if (visibility == IView::VISIBLE) {
        viewPropertyAnimator->Alpha(1.0f);
        AutoPtr<IAnimatorListener> listener = new InnerAnimatorListenerAdapter2(this);
        viewPropertyAnimator->SetListener(listener);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetNavigationIcon(
    /* [in] */ IDrawable* icon)
{
    VALIDATE_NOT_NULL(icon);
    // ==================before translated======================
    // mNavIcon = icon;
    // updateNavigationIcon();

    mNavIcon = icon;
    UpdateNavigationIcon();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetNavigationIcon(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // setNavigationIcon(resId != 0 ? mToolbar.getContext().getDrawable(resId) : null);

    AutoPtr<IContext> context;
    IView::Probe(mToolbar)->GetContext((IContext**)&context);
    AutoPtr<IDrawable> drawable;
    context->GetDrawable(resId, (IDrawable**)&drawable);
    SetNavigationIcon(resId != 0 ? drawable : NULL);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetDefaultNavigationIcon(
    /* [in] */ IDrawable* defaultNavigationIcon)
{
    VALIDATE_NOT_NULL(defaultNavigationIcon);
    // ==================before translated======================
    // if (mDefaultNavigationIcon != defaultNavigationIcon) {
    //     mDefaultNavigationIcon = defaultNavigationIcon;
    //     updateNavigationIcon();
    // }

    if (TO_IINTERFACE(mDefaultNavigationIcon) != TO_IINTERFACE(defaultNavigationIcon)) {
        mDefaultNavigationIcon = defaultNavigationIcon;
        UpdateNavigationIcon();
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetNavigationContentDescription(
    /* [in] */ ICharSequence* description)
{
    VALIDATE_NOT_NULL(description);
    // ==================before translated======================
    // mHomeDescription = description;
    // updateHomeAccessibility();

    mHomeDescription = description;
    UpdateHomeAccessibility();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetNavigationContentDescription(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // setNavigationContentDescription(resId == 0 ? null : getContext().getString(resId));

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String strTmp;
    context->GetString(resId, &strTmp);
    String str = resId == 0 ? String("") : strTmp;
    AutoPtr<ICharSequence> charSequence;
    CString::New(str, (ICharSequence**)&charSequence);
    SetNavigationContentDescription(charSequence);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SaveHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    VALIDATE_NOT_NULL(toolbarStates);
    // ==================before translated======================
    // mToolbar.saveHierarchyState(toolbarStates);

    IView::Probe(mToolbar)->SaveHierarchyState(toolbarStates);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::RestoreHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    VALIDATE_NOT_NULL(toolbarStates);
    // ==================before translated======================
    // mToolbar.restoreHierarchyState(toolbarStates);

    IView::Probe(mToolbar)->RestoreHierarchyState(toolbarStates);
    return NOERROR;
}

Int32 ToolbarWidgetWrapper::DetectDisplayOptions()
{
    // ==================before translated======================
    // int opts = ActionBar.DISPLAY_SHOW_TITLE | ActionBar.DISPLAY_SHOW_HOME |
    //         ActionBar.DISPLAY_USE_LOGO;
    // if (mToolbar.getNavigationIcon() != null) {
    //     opts |= ActionBar.DISPLAY_HOME_AS_UP;
    //     mDefaultNavigationIcon = mToolbar.getNavigationIcon();
    // }
    // return opts;

    Int32 opts = IActionBar::DISPLAY_SHOW_TITLE | IActionBar::DISPLAY_SHOW_HOME | IActionBar::DISPLAY_USE_LOGO;
    AutoPtr<IDrawable> icon;
    mToolbar->GetNavigationIcon((IDrawable**)&icon);
    if (icon != NULL) {
        opts |= IActionBar::DISPLAY_HOME_AS_UP;
        mToolbar->GetNavigationIcon((IDrawable**)&mDefaultNavigationIcon);
    }
    return opts;
}

void ToolbarWidgetWrapper::SetTitleInt(
    /* [in] */ ICharSequence* title)
{
    // ==================before translated======================
    // mTitle = title;
    // if ((mDisplayOpts & ActionBar.DISPLAY_SHOW_TITLE) != 0) {
    //     mToolbar.setTitle(title);
    // }

    mTitle = title;
    if ((mDisplayOpts & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
        mToolbar->SetTitle(title);
    }
}

void ToolbarWidgetWrapper::UpdateToolbarLogo()
{
    // ==================before translated======================
    // Drawable logo = null;
    // if ((mDisplayOpts & ActionBar.DISPLAY_SHOW_HOME) != 0) {
    //     if ((mDisplayOpts & ActionBar.DISPLAY_USE_LOGO) != 0) {
    //         logo = mLogo != null ? mLogo : mIcon;
    //     } else {
    //         logo = mIcon;
    //     }
    // }
    // mToolbar.setLogo(logo);

    AutoPtr<IDrawable> logo;
    if ((mDisplayOpts & IActionBar::DISPLAY_SHOW_HOME) != 0) {
        if ((mDisplayOpts & IActionBar::DISPLAY_USE_LOGO) != 0) {
            logo = mLogo != NULL ? mLogo : mIcon;
        }
        else {
            logo = mIcon;
        }
    }
    mToolbar->SetLogo(logo);
}

void ToolbarWidgetWrapper::EnsureSpinner()
{
    // ==================before translated======================
    // if (mSpinner == null) {
    //     mSpinner = new Spinner(getContext(), null, R.attr.actionDropDownStyle);
    //     Toolbar.LayoutParams lp = new Toolbar.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT,
    //             ViewGroup.LayoutParams.WRAP_CONTENT, Gravity.START | Gravity.CENTER_VERTICAL);
    //     mSpinner.setLayoutParams(lp);
    // }

    assert(0);
    if (mSpinner == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CSpinner::New(context, NULL, R::attr::actionDropDownStyle, (ISpinner**)&mSpinner);

        AutoPtr<IToolbarLayoutParams> lp;
        //-- CToolbarLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT,
            // IGravity::START | IGravity::CENTER_VERTICAL);
        IView::Probe(mSpinner)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    }
}

void ToolbarWidgetWrapper::UpdateNavigationIcon()
{
    // ==================before translated======================
    // if ((mDisplayOpts & ActionBar.DISPLAY_HOME_AS_UP) != 0) {
    //     mToolbar.setNavigationIcon(mNavIcon != null ? mNavIcon : mDefaultNavigationIcon);
    // } else {
    //     mToolbar.setNavigationIcon(null);
    // }

    if ((mDisplayOpts & IActionBar::DISPLAY_HOME_AS_UP) != 0) {
        mToolbar->SetNavigationIcon(mNavIcon != NULL ? mNavIcon : mDefaultNavigationIcon);
    }
    else {
        IDrawable* drawable = NULL;
        mToolbar->SetNavigationIcon(drawable);
    }
}

void ToolbarWidgetWrapper::UpdateHomeAccessibility()
{
    // ==================before translated======================
    // if ((mDisplayOpts & ActionBar.DISPLAY_HOME_AS_UP) != 0) {
    //     if (TextUtils.isEmpty(mHomeDescription)) {
    //         mToolbar.setNavigationContentDescription(mDefaultNavigationContentDescription);
    //     } else {
    //         mToolbar.setNavigationContentDescription(mHomeDescription);
    //     }
    // }

    if ((mDisplayOpts & IActionBar::DISPLAY_HOME_AS_UP) != 0) {
        if (TextUtils::IsEmpty(mHomeDescription)) {
            mToolbar->SetNavigationContentDescription(mDefaultNavigationContentDescription);
        }
        else {
            mToolbar->SetNavigationContentDescription(mHomeDescription);
        }
    }
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


