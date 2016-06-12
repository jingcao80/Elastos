
#include "elastos/droid/internal/view/menu/CActionMenuItem.h"
#include "elastos/droid/internal/widget/ToolbarWidgetWrapper.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/widget/CActionMenuPresenter.h"
#include "elastos/droid/widget/CSpinner.h"
#include "elastos/droid/widget/CToolbarLayoutParams.h"
#include "elastos/droid/R.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::App::IActionBarLayoutParams;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Internal::View::Menu::CActionMenuItem;
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
using Elastos::Droid::Widget::CToolbarLayoutParams;
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
    AutoPtr<IContext> context;
    IView::Probe(owner->mToolbar)->GetContext((IContext**)&context);
    CActionMenuItem::New(context, 0, R::id::home, 0, 0, owner->mTitle, (IActionMenuItem**)&mNavItem);
}

ECode ToolbarWidgetWrapper::InnerViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
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
{}

ECode ToolbarWidgetWrapper::InnerAnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mCanceled) {
        IView::Probe(mOwner->mToolbar)->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::InnerAnimatorListenerAdapter1::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCanceled = TRUE;
    return NOERROR;
}


//=====================================================================
//         ToolbarWidgetWrapper::InnerAnimatorListenerAdapter2
//=====================================================================
ToolbarWidgetWrapper::InnerAnimatorListenerAdapter2::InnerAnimatorListenerAdapter2(
    /* [in] */ ToolbarWidgetWrapper* owner)
    : mOwner(owner)
{}

ECode ToolbarWidgetWrapper::InnerAnimatorListenerAdapter2::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
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
    return constructor(toolbar, style, R::string::action_bar_up_description);
}

ECode ToolbarWidgetWrapper::constructor(
    /* [in] */ IToolbar* toolbar,
    /* [in] */ Boolean style,
    /* [in] */ Int32 defaultNavigationContentDescription)
{
    mToolbar = toolbar;

    toolbar->GetTitle((ICharSequence**)&mTitle);
    toolbar->GetSubtitle((ICharSequence**)&mSubtitle);
    mTitleSet = mTitle != NULL;
    AutoPtr<IContext> context;
    IView::Probe(toolbar)->GetContext((IContext**)&context);
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::ActionBar),
            ArraySize(R::styleable::ActionBar));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(NULL, attrIds, R::attr::actionBarStyle, 0, (ITypedArray**)&a);
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
        if (customNavId != 0) {
            AutoPtr<IContext> contextTmp;
            IView::Probe(mToolbar)->GetContext((IContext**)&contextTmp);
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
            AutoPtr<IContext> contextTmp;
            IView::Probe(mToolbar)->GetContext((IContext**)&contextTmp);
            mToolbar->SetTitleTextAppearance(contextTmp, titleTextStyle);
        }

        Int32 subtitleTextStyle = 0;
        a->GetResourceId(R::styleable::ActionBar_subtitleTextStyle, 0, &subtitleTextStyle);
        if (subtitleTextStyle != 0) {
            AutoPtr<IContext> contextTmp;
            IView::Probe(mToolbar)->GetContext((IContext**)&contextTmp);
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

Int32 ToolbarWidgetWrapper::DetectDisplayOptions()
{
    Int32 opts = IActionBar::DISPLAY_SHOW_TITLE | IActionBar::DISPLAY_SHOW_HOME |
            IActionBar::DISPLAY_USE_LOGO;
    AutoPtr<IDrawable> icon;
    mToolbar->GetNavigationIcon((IDrawable**)&icon);
    if (icon != NULL) {
        opts |= IActionBar::DISPLAY_HOME_AS_UP;
        mDefaultNavigationIcon = NULL;
        mToolbar->GetNavigationIcon((IDrawable**)&mDefaultNavigationIcon);
    }
    return opts;
}


ECode ToolbarWidgetWrapper::GetViewGroup(
    /* [out] */ IViewGroup** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IViewGroup::Probe(mToolbar);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetContext(
    /* [out] */ IContext** result)
{
    VALIDATE_NOT_NULL(result);
    return IView::Probe(mToolbar)->GetContext(result);
}

ECode ToolbarWidgetWrapper::IsSplit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::HasExpandedActionView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mToolbar->HasExpandedActionView(result);
}

ECode ToolbarWidgetWrapper::CollapseActionView()
{
    return mToolbar->CollapseActionView();
}

ECode ToolbarWidgetWrapper::SetWindowCallback(
    /* [in] */ IWindowCallback* cb)
{
    mWindowCallback = cb;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetWindowTitle(
    /* [in] */ ICharSequence* title)
{
    // "Real" title always trumps window title.
    if (!mTitleSet) {
        SetTitleInt(title);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetTitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    return mToolbar->GetTitle(result);
}

ECode ToolbarWidgetWrapper::SetTitle(
    /* [in] */ ICharSequence* title)
{
    mTitleSet = TRUE;
    SetTitleInt(title);
    return NOERROR;
}
void ToolbarWidgetWrapper::SetTitleInt(
    /* [in] */ ICharSequence* title)
{
    mTitle = title;
    if ((mDisplayOpts & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
        mToolbar->SetTitle(title);
    }
}

ECode ToolbarWidgetWrapper::GetSubtitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    return mToolbar->GetSubtitle(result);
}

ECode ToolbarWidgetWrapper::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    mSubtitle = subtitle;
    if ((mDisplayOpts & IActionBar::DISPLAY_SHOW_TITLE) != 0) {
        mToolbar->SetSubtitle(subtitle);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::InitProgress()
{
    Logger::I(TAG, "Progress display unsupported");
    return NOERROR;
}

ECode ToolbarWidgetWrapper::InitIndeterminateProgress()
{
    Logger::I(TAG, "Progress display unsupported");
    return NOERROR;
}

ECode ToolbarWidgetWrapper::CanSplit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetSplitView(
    /* [in] */ IViewGroup* splitView)
{
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetSplitToolbar(
    /* [in] */ Boolean split)
{
    if (split) {
        //throw new UnsupportedOperationException("Cannot split an android.widget.Toolbar");
        Logger::I(TAG, "Cannot split an Elastos.Droid.Widget.CToolbar");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetSplitWhenNarrow(
    /* [in] */ Boolean splitWhenNarrow)
{
    // Ignore.
    return NOERROR;
}

ECode ToolbarWidgetWrapper::HasIcon(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIcon != NULL;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::HasLogo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLogo != NULL;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetIcon(
    /* [in] */ Int32 resId)
{
    if (resId == 0) {
        SetIcon((IDrawable*)NULL);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IDrawable> drawable;
        context->GetDrawable(resId, (IDrawable**)&drawable);
        SetIcon(drawable);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetIcon(
    /* [in] */ IDrawable* d)
{
    mIcon = d;
    UpdateToolbarLogo();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetLogo(
    /* [in] */ Int32 resId)
{
    if (resId == 0) {
        SetLogo((IDrawable*)NULL);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IDrawable> drawable;
        context->GetDrawable(resId, (IDrawable**)&drawable);
        SetLogo(drawable);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetLogo(
    /* [in] */ IDrawable* d)
{
    mLogo = d;
    UpdateToolbarLogo();
    return NOERROR;
}

void ToolbarWidgetWrapper::UpdateToolbarLogo()
{
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

ECode ToolbarWidgetWrapper::CanShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mToolbar->CanShowOverflowMenu(result);
}

ECode ToolbarWidgetWrapper::IsOverflowMenuShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mToolbar->IsOverflowMenuShowing(result);
}

ECode ToolbarWidgetWrapper::IsOverflowMenuShowPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mToolbar->IsOverflowMenuShowPending(result);
}

ECode ToolbarWidgetWrapper::ShowOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mToolbar->ShowOverflowMenu(result);
}

ECode ToolbarWidgetWrapper::HideOverflowMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mToolbar->HideOverflowMenu(result);
}

ECode ToolbarWidgetWrapper::SetMenuPrepared()
{
    mMenuPrepared = TRUE;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuPresenterCallback* cb)
{
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
    mToolbar->DismissPopupMenus();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetDisplayOptions(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDisplayOpts;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetDisplayOptions(
    /* [in] */ Int32 newOpts)
{
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
    AutoPtr<IViewParent> viewParent;
    if (mTabView != NULL &&
        (mTabView->GetParent((IViewParent**)&viewParent), viewParent.Get() == IViewParent::Probe(mToolbar))) {
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
    *result = mTabView != NULL;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::IsTitleTruncated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mToolbar->IsTitleTruncated(result);
}

ECode ToolbarWidgetWrapper::SetCollapsible(
    /* [in] */ Boolean collapsible)
{
    return mToolbar->SetCollapsible(collapsible);
}

ECode ToolbarWidgetWrapper::SetHomeButtonEnabled(
    /* [in] */ Boolean enable)
{
    // Ignore
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetNavigationMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNavigationMode;
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetNavigationMode(
    /* [in] */ Int32 mode)
{
    Int32 oldMode = mNavigationMode;
    if (mode != oldMode) {
        switch (oldMode) {
            case IActionBar::NAVIGATION_MODE_LIST:
                {
                    AutoPtr<IViewParent> viewParent;
                    if (mSpinner != NULL &&
                        (IView::Probe(mSpinner)->GetParent((IViewParent**)&viewParent), viewParent.Get() == IViewParent::Probe(mToolbar))) {
                        IViewGroup::Probe(mToolbar)->RemoveView(IView::Probe(mSpinner));
                    }
                }
                break;
            case IActionBar::NAVIGATION_MODE_TABS:
                {
                    AutoPtr<IViewParent> viewParent;
                    if (mTabView != NULL &&
                        (IView::Probe(mTabView)->GetParent((IViewParent**)&viewParent), viewParent.Get() == IViewParent::Probe(mToolbar))) {
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
                    Logger::E(TAG, "Invalid navigation mode %d", mode);
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
        }
    }
    return NOERROR;
}

void ToolbarWidgetWrapper::EnsureSpinner()
{
    if (mSpinner == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CSpinner::New(context, NULL, R::attr::actionDropDownStyle, (ISpinner**)&mSpinner);
        AutoPtr<IViewGroupLayoutParams> lp;
        CToolbarLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT,
                IViewGroupLayoutParams::WRAP_CONTENT,
                IGravity::START | IGravity::CENTER_VERTICAL,
                (IViewGroupLayoutParams**)&lp);
        IView::Probe(mSpinner)->SetLayoutParams(lp);
    }
}


ECode ToolbarWidgetWrapper::SetDropdownParams(
    /* [in] */ ISpinnerAdapter* adapter,
    /* [in] */ IAdapterViewOnItemSelectedListener* listener)
{
    EnsureSpinner();
    IAdapterView::Probe(mSpinner)->SetAdapter(IAdapter::Probe(adapter));
    IAdapterView::Probe(mSpinner)->SetOnItemSelectedListener(listener);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetDropdownSelectedPosition(
    /* [in] */ Int32 position)
{
    if (mSpinner == NULL) {
        Logger::E(TAG, "Can't set dropdown selected position without an adapter");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    IAdapterView::Probe(mSpinner)->SetSelection(position);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetDropdownSelectedPosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (mSpinner != NULL) {
        IAdapterView::Probe(mSpinner)->GetSelectedItemPosition(result);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::GetDropdownItemCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (mSpinner != NULL) {
        IAdapterView::Probe(mSpinner)->GetCount(result);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetCustomView(
    /* [in] */ IView* view)
{
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
    *result = mCustomView;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ToolbarWidgetWrapper::AnimateToVisibility(
    /* [in] */ Int32 visibility)
{
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
    mNavIcon = icon;
    UpdateNavigationIcon();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetNavigationIcon(
    /* [in] */ Int32 resId)
{
    if (resId == 0) {
        SetNavigationIcon((IDrawable*)NULL);
    }
    else {
        AutoPtr<IContext> context;
        IView::Probe(mToolbar)->GetContext((IContext**)&context);
        AutoPtr<IDrawable> drawable;
        context->GetDrawable(resId, (IDrawable**)&drawable);
        SetNavigationIcon(drawable);
    }
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetDefaultNavigationIcon(
    /* [in] */ IDrawable* defaultNavigationIcon)
{
    if (mDefaultNavigationIcon.Get() != defaultNavigationIcon) {
        mDefaultNavigationIcon = defaultNavigationIcon;
        UpdateNavigationIcon();
    }
    return NOERROR;
}

void ToolbarWidgetWrapper::UpdateNavigationIcon()
{
    if ((mDisplayOpts & IActionBar::DISPLAY_HOME_AS_UP) != 0) {
        mToolbar->SetNavigationIcon(mNavIcon != NULL ? mNavIcon : mDefaultNavigationIcon);
    }
    else {
        mToolbar->SetNavigationIcon((IDrawable*)NULL);
    }
}

ECode ToolbarWidgetWrapper::SetNavigationContentDescription(
    /* [in] */ ICharSequence* description)
{
    mHomeDescription = description;
    UpdateHomeAccessibility();
    return NOERROR;
}

ECode ToolbarWidgetWrapper::SetNavigationContentDescription(
    /* [in] */ Int32 resId)
{
    if (resId == 0) {
        SetNavigationContentDescription((ICharSequence*)NULL);
    }
    else {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        String str;
        context->GetString(resId, &str);
        AutoPtr<ICharSequence> csq;
        CString::New(str, (ICharSequence**)&csq);
        SetNavigationContentDescription(csq);
    }
    return NOERROR;
}

void ToolbarWidgetWrapper::UpdateHomeAccessibility()
{
    if ((mDisplayOpts & IActionBar::DISPLAY_HOME_AS_UP) != 0) {
        if (TextUtils::IsEmpty(mHomeDescription)) {
            mToolbar->SetNavigationContentDescription(mDefaultNavigationContentDescription);
        }
        else {
            mToolbar->SetNavigationContentDescription(mHomeDescription);
        }
    }
}

ECode ToolbarWidgetWrapper::SaveHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    return IView::Probe(mToolbar)->SaveHierarchyState(toolbarStates);
}

ECode ToolbarWidgetWrapper::RestoreHierarchyState(
    /* [in] */ ISparseArray* toolbarStates)
{
    return IView::Probe(mToolbar)->RestoreHierarchyState(toolbarStates);
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
