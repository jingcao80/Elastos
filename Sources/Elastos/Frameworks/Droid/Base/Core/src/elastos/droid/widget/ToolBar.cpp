#include "elastos/droid/widget/ToolBar.h"
#include "elastos/droid/widget/CImageView.h"
#include "elastos/droid/widget/CImageButton.h"
#include "elastos/droid/widget/CTextView.h"
#include "elastos/droid/widget/CActionMenuView.h"
#include "elastos/droid/view/Gravity.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/view/CMenuInflater.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Math.h>

using Elastos::Droid::App::IActionBarLayoutParams;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuPresenter;
using Elastos::Droid::View::Gravity;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::CMenuInflater;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ICollapsibleActionView;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::CImageButton;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::CActionMenuView;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ILayout;
using Elastos::Droid::Text::TextUtilsTruncateAt_END;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Widget {

/////////////////////////////////////////////////////////////
//              ToolBar::LayoutParams
/////////////////////////////////////////////////////////////

const Int32 ToolBar::LayoutParams::CUSTOM;
const Int32 ToolBar::LayoutParams::SYSTEM;
const Int32 ToolBar::LayoutParams::EXPANDED;

CAR_INTERFACE_IMPL(ToolBar::LayoutParams, ActionBarLayoutParams, IToolBarLayoutParams)

ToolBar::LayoutParams::LayoutParams()
    : mViewType(CUSTOM)
{}

ECode ToolBar::LayoutParams::constructor(
/* [in] */ IContext* ctx,
/* [in] */ IAttributeSet* attrs)
{
    return ActionBarLayoutParams::constructor(ctx, attrs);
}

ECode ToolBar::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    ActionBarLayoutParams::constructor(width, height);
    mGravity = IGravity::CENTER_VERTICAL | IGravity::START;
    return NOERROR;
}

ECode ToolBar::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 gravity)
{
    ActionBarLayoutParams::constructor(width, height);
    mGravity = gravity;
    return NOERROR;
}

ECode ToolBar::LayoutParams::constructor(
    /* [in] */ Int32 gravity)
{
    return constructor(IViewGroupLayoutParams::WRAP_CONTENT,
        IViewGroupLayoutParams::MATCH_PARENT, gravity);
}

ECode ToolBar::LayoutParams::constructor(
    /* [in] */ IToolBarLayoutParams* source)
{
    ActionBarLayoutParams::constructor(IActionBarLayoutParams::Probe(source));
    Int32 viewTpye;
    assert(0);
    /*source->GetViewType(viewTpye);
    SetViewType(viewTpye);*/
    return NOERROR;
}

ECode ToolBar::LayoutParams::constructor(
    /* [in] */ IActionBarLayoutParams* source)
{
    return ActionBarLayoutParams::constructor(source);
}

ECode ToolBar::LayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    ActionBarLayoutParams::constructor(IViewGroupLayoutParams::Probe(source));
    // ActionBar.LayoutParams doesn't have a MarginLayoutParams constructor.
    // Fake it here and copy over the relevant data.
    CopyMarginsFrom(source);
    return NOERROR;
}

ECode ToolBar::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ActionBarLayoutParams::constructor(source);
}

ECode ToolBar::LayoutParams::SetViewType(
    /* [in] */ Int32 type)
{
    mViewType = type;
    return  NOERROR;
}

ECode ToolBar::LayoutParams::GetViewType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mViewType;
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//              ToolBar::SavedState
/////////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ToolBar::SavedState, View::BaseSavedState, IToolBarSavedState)

ToolBar::SavedState::SavedState()
    : mExpandedMenuItemId(0)
    , mIsOverflowOpen(FALSE)
{}

ECode ToolBar::SavedState::constructor()
{
    return NOERROR;
}

ECode ToolBar::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return BaseSavedState::constructor(superState);
}

ECode ToolBar::SavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    BaseSavedState::WriteToParcel(dest);
    dest->WriteInt32(mExpandedMenuItemId);
    dest->WriteInt32(mIsOverflowOpen ? 1 : 0);
    return NOERROR;
}

ECode ToolBar::SavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    BaseSavedState::ReadFromParcel(source);
    source->ReadInt32(&mExpandedMenuItemId);
    Int32 res;
    source->ReadInt32(&res);
    mIsOverflowOpen = res != 0;
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//              ToolBar::MenuItemClickListener
/////////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ToolBar::MenuItemClickListener, Object, IActionMenuViewOnMenuItemClickListener)

ToolBar::MenuItemClickListener::MenuItemClickListener(
    /* [in] */ ToolBar* host)
    : mHost(host)
{}

ECode ToolBar::MenuItemClickListener::OnMenuItemClick(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    if (mHost->mOnMenuItemClickListener != NULL) {
        mHost->mOnMenuItemClickListener->OnMenuItemClick(item, rst);
    }
    *rst = FALSE;
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//       ToolBar::ExpandedActionViewMenuPresenter
/////////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ToolBar::ExpandedActionViewMenuPresenter, Object, IMenuPresenter)

ToolBar::ExpandedActionViewMenuPresenter::ExpandedActionViewMenuPresenter(
    /* [in] */ ToolBar* host)
    : mHost(host)
{}

ECode ToolBar::ExpandedActionViewMenuPresenter::InitForMenu(
    /* [in] */ IContext* ctx,
    /* [in] */ IMenuBuilder* menu)
{
    if (mMenu != NULL && mCurrentExpandedItem != NULL) {
        Boolean res;
        mMenu->CollapseItemActionView(mCurrentExpandedItem, &res);
    }
    mMenu = menu;
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::GetMenuView(
    /* [in] */ IViewGroup* root,
    /* [out] */ IMenuView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = NULL;
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::UpdateMenuView(
    /* [in] */ Boolean cleared)
{
    if (mCurrentExpandedItem != NULL) {
        Boolean found = FALSE;

        if (mMenu != NULL) {
            Int32 count;
            mMenu->GetSize(&count);
            for (Int32 i = 0; i < count; i++) {
                AutoPtr<IMenuItem> item;
                mMenu->GetItem(i, (IMenuItem**)&item);
                if (IMenuItem::Probe(mCurrentExpandedItem) == item) {
                    found = TRUE;
                    break;
                }
            }
        }

        if (!found) {
            // The item we had expanded disappeared. Collapse.
            Boolean res;
            CollapseItemActionView(mMenu, mCurrentExpandedItem, &res);
        }
    }
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::SetCallback(
    /* [in] */ IMenuPresenterCallback* cb)
{
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::OnSubMenuSelected(
    /* [in] */ ISubMenuBuilder* subMenu,
    /* [out] */ Boolean* handled)
{
    VALIDATE_NOT_NULL(handled)
    *handled = FALSE;
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::FlagActionItems(
    /* [out] */ Boolean* shown)
{
    VALIDATE_NOT_NULL(shown)
    *shown = FALSE;
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::ExpandItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* expanded)
{
    VALIDATE_NOT_NULL(expanded)
    mHost->EnsureCollapseButtonView();
    AutoPtr<IViewParent> parent;
    IView::Probe(mHost->mCollapseButtonView)->GetParent((IViewParent**)&parent);
    if (IView::Probe(parent) != (IView*)mHost->Probe(EIID_IView)) {
        mHost->AddView(IView::Probe(mHost->mCollapseButtonView));
    }
    item->GetActionView((IView**)&mHost->mExpandedActionView);
    mCurrentExpandedItem = item;

    parent = NULL;
    mHost->mExpandedActionView->GetParent((IViewParent**)&parent);
    if (IView::Probe(parent) != (IView*)mHost->Probe(EIID_IView)) {
        AutoPtr<IToolBarLayoutParams> lp;
        mHost->GenerateDefaultLayoutParams((IToolBarLayoutParams**)&lp);
        IActionBarLayoutParams::Probe(lp)->SetGravity(
            IGravity::START | (mHost->mButtonGravity & IGravity::VERTICAL_GRAVITY_MASK));
        assert(0);
        //lp->SetViewType(LayoutParams::EXPANDED);
        IView::Probe(mHost->mExpandedActionView)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
        mHost->AddView(IView::Probe(mHost->mExpandedActionView));
    }

    mHost->SetChildVisibilityForExpandedActionView(TRUE);
    mHost->RequestLayout();
    item->SetActionViewExpanded(TRUE);

    if (ICollapsibleActionView::Probe(mHost->mExpandedActionView)) {
        ICollapsibleActionView::Probe(mHost->mExpandedActionView)->OnActionViewExpanded();
    }

    *expanded = TRUE;
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::CollapseItemActionView(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* collapsed)
{
    VALIDATE_NOT_NULL(collapsed)
    // Do this before detaching the actionview from the hierarchy, in case
    // it needs to dismiss the soft keyboard, etc.
    if (ICollapsibleActionView::Probe(mHost->mExpandedActionView)) {
        ICollapsibleActionView::Probe(mHost->mExpandedActionView)->OnActionViewCollapsed();
    }

    mHost->RemoveView(IView::Probe(mHost->mExpandedActionView));
    mHost->RemoveView(IView::Probe(mHost->mCollapseButtonView));
    mHost->mExpandedActionView = NULL;

    mHost->SetChildVisibilityForExpandedActionView(FALSE);
    mCurrentExpandedItem = NULL;
    mHost->RequestLayout();
    item->SetActionViewExpanded(FALSE);

    *collapsed = TRUE;
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = 0;
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::OnSaveInstanceState(
    /* [out] */ IParcelable** pa)
{
    VALIDATE_NOT_NULL(pa)
    *pa = NULL;
    return NOERROR;
}

ECode ToolBar::ExpandedActionViewMenuPresenter::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//              ToolBar::ToolBarRunnable
/////////////////////////////////////////////////////////////

ToolBar::ToolBarRunnable::ToolBarRunnable(
            /* [in] */ ToolBar* host)
    : mHost(host)
{}

ECode ToolBar::ToolBarRunnable::Run()
{
    Boolean isRun;
    mHost->ShowOverflowMenu(&isRun);
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//             ToolBar::CollapseOnClickListener
/////////////////////////////////////////////////////////////

CAR_INTERFACE_IMPL(ToolBar::CollapseOnClickListener, Object, IViewOnClickListener)

ToolBar::CollapseOnClickListener::CollapseOnClickListener(
    /* [in] */ ToolBar* host)
    : mHost(host)
{}

ECode ToolBar::CollapseOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->CollapseActionView();
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//                      ToolBar
/////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(ToolBar, ViewGroup, IToolBar)

const String ToolBar::TAG("Toolbar");

ToolBar::ToolBar()
    : ViewGroup()
    , mPopupTheme(0)
    , mTitleTextAppearance(0)
    , mSubtitleTextAppearance(0)
    , mNavButtonStyle(0)
    , mButtonGravity(0)
    , mMaxButtonHeight(0)
    , mTitleMarginStart(0)
    , mTitleMarginEnd(0)
    , mTitleMarginTop(0)
    , mTitleMarginBottom(0)
    , mGravity(IGravity::START | IGravity::CENTER_VERTICAL)
    , mTitleTextColor(0)
    , mSubtitleTextColor(0)
    , mEatingTouch(FALSE)
    , mCollapsible(FALSE)
{
    mContentInsets = new RtlSpacingHelper();
    CArrayList::New((IArrayList**)&mTempViews);
    mTempMargins = ArrayOf<Int32>::Alloc(2);
}

ECode ToolBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32 *>(R::styleable::Toolbar),
        ArraySize(R::styleable::Toolbar));
    AutoPtr<ITypedArray> a;
    ASSERT_SUCCEEDED(context->ObtainStyledAttributes(
            attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));
    a->GetResourceId(R::styleable::Toolbar_titleTextAppearance, 0, &mTitleTextAppearance);
    a->GetResourceId(R::styleable::Toolbar_subtitleTextAppearance, 0, &mSubtitleTextAppearance);
    a->GetResourceId(R::styleable::Toolbar_navigationButtonStyle, 0, &mNavButtonStyle);
    a->GetInteger(R::styleable::Toolbar_gravity, mGravity, &mGravity);
    a->GetInteger(R::styleable::Toolbar_buttonGravity, IGravity::TOP, &mButtonGravity);
    a->GetDimensionPixelOffset(R::styleable::Toolbar_titleMargins, 0, &mTitleMarginBottom);
    mTitleMarginStart = mTitleMarginEnd = mTitleMarginTop = mTitleMarginBottom;
    Int32 marginStart, marginEnd, marginTop, marginBottom;
    a->GetDimensionPixelOffset(R::styleable::Toolbar_titleMarginStart, -1, &marginStart);
    if (marginStart >= 0) {
        mTitleMarginStart = marginStart;
    }
    a->GetDimensionPixelOffset(R::styleable::Toolbar_titleMarginEnd, -1, &marginEnd);
    if (marginEnd >= 0) {
        mTitleMarginEnd = marginEnd;
    }
    a->GetDimensionPixelOffset(R::styleable::Toolbar_titleMarginTop, -1, &marginTop);
    if (marginTop >= 0) {
        mTitleMarginTop = marginTop;
    }
    a->GetDimensionPixelOffset(R::styleable::Toolbar_titleMarginBottom, -1, &marginBottom);
    if (marginBottom >= 0) {
        mTitleMarginBottom = marginBottom;
    }
    a->GetDimensionPixelSize(R::styleable::Toolbar_maxButtonHeight, -1, &mMaxButtonHeight);
    Int32 contentInsetStart, contentInsetEnd, contentInsetLeft, contentInsetRight;
    a->GetDimensionPixelOffset(R::styleable::Toolbar_contentInsetStart,
                    RtlSpacingHelper::UNDEFINED, &contentInsetStart);
    a->GetDimensionPixelOffset(R::styleable::Toolbar_contentInsetEnd,
                    RtlSpacingHelper::UNDEFINED, &contentInsetEnd);
    a->GetDimensionPixelSize(R::styleable::Toolbar_contentInsetStart, 0, &contentInsetLeft);
    a->GetDimensionPixelSize(R::styleable::Toolbar_contentInsetStart, 0, &contentInsetRight);

    mContentInsets->SetAbsolute(contentInsetLeft, contentInsetRight);

    if (contentInsetStart != RtlSpacingHelper::UNDEFINED ||
            contentInsetEnd != RtlSpacingHelper::UNDEFINED) {
        mContentInsets->SetRelative(contentInsetStart, contentInsetEnd);
    }

    a->GetDrawable(R::styleable::Toolbar_collapseIcon, (IDrawable**)&mCollapseIcon);
    AutoPtr<ICharSequence> title;
    a->GetText(R::styleable::Toolbar_title, (ICharSequence**)&title);
    if (!TextUtils::IsEmpty(title)) {
        SetTitle(title);
    }
    AutoPtr<ICharSequence> subtitle;
    a->GetText(R::styleable::Toolbar_subtitle, (ICharSequence**)&subtitle);
    if (!TextUtils::IsEmpty(subtitle)) {
        SetSubtitle(subtitle);
    }

    // Set the default context, since setPopupTheme() may be a no-op.
    mPopupContext = mContext;
    Int32 theme;
    a->GetResourceId(R::styleable::Toolbar_popupTheme, 0, &theme);
    SetPopupTheme(theme);

    AutoPtr<IDrawable> navIcon;
    a->GetDrawable(R::styleable::Toolbar_navigationIcon, (IDrawable**)&navIcon);

    if (navIcon != NULL) {
        SetNavigationIcon(navIcon);
    }

    AutoPtr<ICharSequence> navDesc;
    a->GetText(R::styleable::Toolbar_navigationContentDescription, (ICharSequence**)&navDesc);
    if (!TextUtils::IsEmpty(navDesc)) {
        SetNavigationContentDescription(navDesc);
    }
    a->Recycle();
    return NOERROR;
}

ECode ToolBar::SetPopupTheme(
    /* [in] */ Int32 resId)
{
    if (mPopupTheme != resId) {
        mPopupTheme = resId;
        if (resId == 0) {
            mPopupContext = mContext;
        } else {
            CContextThemeWrapper::New(mContext, resId, (IContext**)&mPopupContext);
        }
    }
    return NOERROR;
}

ECode ToolBar::GetPopupTheme(
    /* [out] */ Int32* theme)
{
    VALIDATE_NOT_NULL(theme)
    *theme = mPopupTheme;
    return NOERROR;
}

ECode ToolBar::OnRtlPropertiesChanged(
    /* [in] */ Int32 layoutDirection)
{
    ViewGroup::OnRtlPropertiesChanged(layoutDirection);
    mContentInsets->SetDirection(layoutDirection == IView::LAYOUT_DIRECTION_RTL);
    return NOERROR;
}

ECode ToolBar::SetLogo(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IDrawable> logo;
    ctx->GetDrawable(resId, (IDrawable**)&logo);
    SetLogo(logo);
    return NOERROR;
}

ECode ToolBar::CanShowOverflowMenu(
    /* [out] */ Boolean* menu)
{
    VALIDATE_NOT_NULL(menu)
    Int32 visibility;
    GetVisibility(&visibility);
    Boolean isOverflowReserved;
    *menu = visibility && mMenuView != NULL
        && (mMenuView->IsOverflowReserved(&isOverflowReserved), isOverflowReserved);
    return NOERROR;
}

ECode ToolBar::IsOverflowMenuShowing(
    /* [out] */ Boolean* isShowing)
{
    VALIDATE_NOT_NULL(isShowing)
    Boolean isOverflowMenuShowing;
    *isShowing = mMenuView != NULL
        && (mMenuView->IsOverflowMenuShowing(&isOverflowMenuShowing), isOverflowMenuShowing);
    return NOERROR;
}

ECode ToolBar::IsOverflowMenuShowPending(
    /* [out] */ Boolean* is)
{
    VALIDATE_NOT_NULL(is)
    Boolean isOverflowMenuShowPending;
    *is = mMenuView != NULL
        && (mMenuView->IsOverflowMenuShowPending(&isOverflowMenuShowPending), isOverflowMenuShowPending);
    return NOERROR;
}

ECode ToolBar::ShowOverflowMenu(
    /* [out] */ Boolean* isShowing)
{
    VALIDATE_NOT_NULL(isShowing)
    Boolean showOverflowMenu;
    *isShowing = mMenuView != NULL
        && (mMenuView->ShowOverflowMenu(&showOverflowMenu), showOverflowMenu);
    return NOERROR;
}

ECode ToolBar::HideOverflowMenu(
    /* [out] */ Boolean* isHide)
{
    VALIDATE_NOT_NULL(isHide)
    Boolean hideOverflowMenu;
    *isHide = mMenuView != NULL
        && (mMenuView->HideOverflowMenu(&hideOverflowMenu), hideOverflowMenu);
    return NOERROR;
}

ECode ToolBar::SetMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IActionMenuPresenter* outerPresenter)
{
    if (menu == NULL && mMenuView == NULL) {
        return NOERROR;
    }

    EnsureMenuView();
    AutoPtr<IMenuBuilder> oldMenu;
    mMenuView->PeekMenu((IMenuBuilder**)&oldMenu);
    if (IMenuBuilder::Probe(oldMenu) == IMenuBuilder::Probe(menu)) {
        return NOERROR;
    }

    if (oldMenu != NULL) {
        oldMenu->RemoveMenuPresenter(IMenuPresenter::Probe(mOuterActionMenuPresenter));
        oldMenu->RemoveMenuPresenter(mExpandedMenuPresenter);
    }

    if (mExpandedMenuPresenter == NULL) {
        mExpandedMenuPresenter = new ExpandedActionViewMenuPresenter(this);
    }

    outerPresenter->SetExpandedActionViewsExclusive(TRUE);
    if (menu != NULL) {
        menu->AddMenuPresenter(IMenuPresenter::Probe(outerPresenter), mPopupContext);
        menu->AddMenuPresenter(mExpandedMenuPresenter, mPopupContext);
    } else {
        IMenuPresenter::Probe(outerPresenter)->InitForMenu(mPopupContext, NULL);
        mExpandedMenuPresenter->InitForMenu(mPopupContext, NULL);
        IMenuPresenter::Probe(outerPresenter)->UpdateMenuView(TRUE);
        mExpandedMenuPresenter->UpdateMenuView(TRUE);
    }
    mMenuView->SetPopupTheme(mPopupTheme);
    mMenuView->SetPresenter(outerPresenter);
    mOuterActionMenuPresenter = outerPresenter;
    return NOERROR;
}

ECode ToolBar::DismissPopupMenus()
{
    if (mMenuView != NULL) {
        mMenuView->DismissPopupMenus();
    }
    return NOERROR;
}

ECode ToolBar::IsTitleTruncated(
    /* [out] */ Boolean* is)
{
    VALIDATE_NOT_NULL(is)
    if (mTitleTextView == NULL) {
        *is = FALSE;
        return NOERROR;
    }

    AutoPtr<ILayout> titleLayout;
    mTitleTextView->GetLayout((ILayout**)&titleLayout);
    if (titleLayout == NULL) {
        *is = FALSE;
        return NOERROR;
    }

    Int32 lineCount;
    titleLayout->GetLineCount(&lineCount);
    for (Int32 i = 0; i < lineCount; i++) {
        Int32 count;
        titleLayout->GetEllipsisCount(i, &count);
        if (count > 0) {
            *is = TRUE;
            return NOERROR;
        }
    }
    *is = FALSE;
    return NOERROR;
}

ECode ToolBar::SetLogo(
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IViewParent> parent;
    IView::Probe(mLogoView)->GetParent((IViewParent**)&parent);
    if (drawable != NULL) {
        EnsureLogoView();
        if (parent == NULL) {
            AddSystemView(IView::Probe(mLogoView));
            UpdateChildVisibilityForExpandedActionView(IView::Probe(mLogoView));
        }
    } else if (mLogoView != NULL && parent != NULL) {
        RemoveView(IView::Probe(mLogoView));
    }
    if (mLogoView != NULL) {
        mLogoView->SetImageDrawable(drawable);
    }
    return NOERROR;
}

ECode ToolBar::GetLogo(
    /* [out] */ IDrawable** logo)
{
    VALIDATE_NOT_NULL(logo)
    if (mLogoView != NULL) {
        mLogoView->GetDrawable(logo);
    } else {
        *logo = NULL;
    }
    return NOERROR;
}

ECode ToolBar::SetLogoDescription(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ICharSequence> text;
    ctx->GetText(resId, (ICharSequence**)&text);
    return SetLogoDescription(text);
}

ECode ToolBar::SetLogoDescription(
    /* [in] */ ICharSequence* description)
{
    if (!TextUtils::IsEmpty(description)) {
        EnsureLogoView();
    }
    if (mLogoView != NULL) {
        IView::Probe(mLogoView)->SetContentDescription(description);
    }
    return NOERROR;
}

ECode ToolBar::GetLogoDescription(
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(description)
    if (mLogoView != NULL) {
        IView::Probe(mLogoView)->GetContentDescription(description);
    } else {
        *description = NULL;
    }
    return NOERROR;
}

ECode ToolBar::HasExpandedActionView(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has)
    *has = mExpandedMenuPresenter != NULL &&
        mExpandedMenuPresenter->mCurrentExpandedItem != NULL;
    return NOERROR;
}

ECode ToolBar::CollapseActionView()
{
    AutoPtr<IMenuItemImpl> item = mExpandedMenuPresenter == NULL ? NULL :
            mExpandedMenuPresenter->mCurrentExpandedItem;
    if (item != NULL) {
        Boolean res;
        IMenuItem::Probe(item)->CollapseActionView(&res);
    }
    return NOERROR;
}

ECode ToolBar::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    *title = mTitleText;
    REFCOUNT_ADD(*title)
    return NOERROR;
}

ECode ToolBar::SetTitle(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ICharSequence> title;
    ctx->GetText(resId, (ICharSequence**)&title);
    return SetTitle(title);
}

ECode ToolBar::SetTitle(
    /* [in] */ ICharSequence* title)
{
    AutoPtr<IViewParent> pView;
    if (!TextUtils::IsEmpty(title)) {
        if (mTitleTextView == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            CTextView::New(context, (ITextView**)&mTitleTextView);

            mTitleTextView->SetSingleLine();
            mTitleTextView->SetEllipsize(TextUtilsTruncateAt_END);
            if (mTitleTextAppearance != 0) {
                mTitleTextView->SetTextAppearance(context, mTitleTextAppearance);
            }
            if (mTitleTextColor != 0) {
                mTitleTextView->SetTextColor(mTitleTextColor);
            }
        }
        AutoPtr<IViewParent> parent;
        IView::Probe(mTitleTextView)->GetParent((IViewParent**)&parent);
        if (parent == NULL) {
            AddSystemView(IView::Probe(mTitleTextView));
            UpdateChildVisibilityForExpandedActionView(IView::Probe(mTitleTextView));
        }
    } else if (mTitleTextView != NULL
        && (IView::Probe(mTitleTextView)->GetParent((IViewParent**)&pView), pView) != NULL) {
        RemoveView(IView::Probe(mTitleTextView));
    }
    if (mTitleTextView != NULL) {
        mTitleTextView->SetText(title);
    }
    mTitleText = title;
    return NOERROR;
}

ECode ToolBar::GetSubtitle(
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(description)
    *description = mSubtitleText;
    return NOERROR;
}

ECode ToolBar::SetSubtitle(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ICharSequence> title;
    ctx->GetText(resId, (ICharSequence**)&title);
    return SetSubtitle(title);
}

ECode ToolBar::SetSubtitle(
    /* [in] */ ICharSequence* title)
{
    AutoPtr<IViewParent> pView;
    if (!TextUtils::IsEmpty(title)) {
        if (mSubtitleTextView == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            CTextView::New(context, (ITextView**)&mSubtitleTextView);
            mSubtitleTextView->SetSingleLine();
            mSubtitleTextView->SetEllipsize(TextUtilsTruncateAt_END);
            if (mSubtitleTextAppearance != 0) {
                mSubtitleTextView->SetTextAppearance(context, mSubtitleTextAppearance);
            }
            if (mSubtitleTextColor != 0) {
                mSubtitleTextView->SetTextColor(mSubtitleTextColor);
            }
        }
        AutoPtr<IViewParent> parent;
        IView::Probe(mSubtitleTextView)->GetParent((IViewParent**)&parent);
        if (parent == NULL) {
            AddSystemView(IView::Probe(mSubtitleTextView));
            UpdateChildVisibilityForExpandedActionView(IView::Probe(mSubtitleTextView));
        }
    } else if (mSubtitleTextView != NULL
        && (IView::Probe(mSubtitleTextView)->GetParent((IViewParent**)&pView), pView) != NULL) {
        RemoveView(IView::Probe(mSubtitleTextView));
    }
    if (mSubtitleTextView != NULL) {
        mSubtitleTextView->SetText(title);
    }
    mSubtitleText = title;
    return NOERROR;
}

ECode ToolBar::SetTitleTextAppearance(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId)
{
    mTitleTextAppearance = resId;
    if (mTitleTextView != NULL) {
        mTitleTextView->SetTextAppearance(context, resId);
    }
    return NOERROR;
}

ECode ToolBar::SetSubtitleTextAppearance(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resId)
{
    mSubtitleTextAppearance = resId;
    if (mSubtitleTextView != NULL) {
        mSubtitleTextView->SetTextAppearance(context, resId);
    }
    return NOERROR;
}

ECode ToolBar::SetTitleTextColor(
    /* [in] */ Int32 color)
{
    mTitleTextColor = color;
    if (mTitleTextView != NULL) {
        mTitleTextView->SetTextColor(color);
    }
    return NOERROR;
}

ECode ToolBar::SetSubtitleTextColor(
    /* [in] */ Int32 color)
{
    mSubtitleTextColor = color;
    if (mSubtitleTextView != NULL) {
        mSubtitleTextView->SetTextColor(color);
    }
    return NOERROR;
}

ECode ToolBar::GetNavigationContentDescription(
    /* [out] */ ICharSequence** description)
{
    VALIDATE_NOT_NULL(description)
    if (mNavButtonView != NULL) {
        IView::Probe(mNavButtonView)->GetContentDescription(description);
    } else {
        *description = NULL;
    }
    return NOERROR;
}

ECode ToolBar::SetNavigationContentDescription(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ICharSequence> title;
    ctx->GetText(resId, (ICharSequence**)&title);
    if (resId != 0) {
        SetNavigationContentDescription(title);
    } else {
        SetNavigationContentDescription((ICharSequence*)NULL);
    }
    return NOERROR;
}

ECode ToolBar::SetNavigationContentDescription(
    /* [in] */ ICharSequence* description)
{
    if (!TextUtils::IsEmpty(description)) {
        EnsureNavButtonView();
    }
    if (mNavButtonView != NULL) {
        IView::Probe(mNavButtonView)->SetContentDescription(description);
    }
    return NOERROR;
}

ECode ToolBar::SetNavigationIcon(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IDrawable> icon;
    ctx->GetDrawable(resId, (IDrawable**)&icon);
    SetNavigationIcon(icon);
    return NOERROR;
}

ECode ToolBar::SetNavigationIcon(
    /* [in] */ IDrawable* icon)
{
    AutoPtr<IViewParent> pView;
    if (icon != NULL) {
        EnsureNavButtonView();
        AutoPtr<IViewParent> parent;
        IView::Probe(mNavButtonView)->GetParent((IViewParent**)&parent);
        if (parent == NULL) {
            AddSystemView(IView::Probe(mNavButtonView));
            UpdateChildVisibilityForExpandedActionView(IView::Probe(mNavButtonView));
        }
    } else if (mNavButtonView != NULL
        && (IView::Probe(mNavButtonView)->GetParent((IViewParent**)&pView), pView) != NULL) {
        RemoveView(IView::Probe(mNavButtonView));
    }
    if (mNavButtonView != NULL) {
        IImageView::Probe(mNavButtonView)->SetImageDrawable(icon);
    }
    return NOERROR;
}

ECode ToolBar::GetNavigationIcon(
    /* [out] */ IDrawable** icon)
{
    VALIDATE_NOT_NULL(icon)
    if (mNavButtonView != NULL) {
        IImageView::Probe(mNavButtonView)->GetDrawable(icon);
    } else {
        *icon = NULL;
    }
    return NOERROR;
}

ECode ToolBar::SetNavigationOnClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    EnsureNavButtonView();
    IView::Probe(mNavButtonView)->SetOnClickListener(listener);
    return NOERROR;
}

ECode ToolBar::GetMenu(
    /* [out] */ IMenu** menu)
{
    VALIDATE_NOT_NULL(menu)
    EnsureMenu();
    return mMenuView->GetMenu(menu);
}

ECode ToolBar::InflateMenu(
    /* [in] */ Int32 resId)
{
    AutoPtr<IMenu> menu;
    GetMenu((IMenu**)&menu);
    GetMenuInflater()->Inflate(resId, menu);
    return NOERROR;
}

ECode ToolBar::SetOnMenuItemClickListener(
    /* [in] */ IToolBarOnMenuItemClickListener* listener)
{
    mOnMenuItemClickListener = listener;
    return NOERROR;
}

ECode ToolBar::SetContentInsetsRelative(
    /* [in] */ Int32 contentInsetStart,
    /* [in] */ Int32 contentInsetEnd)
{
    mContentInsets->SetRelative(contentInsetStart, contentInsetEnd);
    return NOERROR;
}

ECode ToolBar::GetContentInsetStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start)
    *start = mContentInsets->GetStart();
    return NOERROR;
}

ECode ToolBar::GetContentInsetEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end)
    *end = mContentInsets->GetEnd();
    return NOERROR;
}

ECode ToolBar::SetContentInsetsAbsolute(
    /* [in] */ Int32 contentInsetLeft,
    /* [in] */ Int32 contentInsetRight)
{
    mContentInsets->SetAbsolute(contentInsetLeft, contentInsetRight);
    return NOERROR;
}

ECode ToolBar::GetContentInsetLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left)
    *left = mContentInsets->GetLeft();
    return NOERROR;
}

ECode ToolBar::GetContentInsetRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right)
    *right = mContentInsets->GetRight();
    return NOERROR;
}

ECode ToolBar::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    // Toolbars always eat touch events, but should still respect the touch event dispatch
    // contract. If the normal View implementation doesn't want the events, we'll just silently
    // eat the rest of the gesture without reporting the events to the default implementation

    VALIDATE_NOT_NULL(res)
    Int32 action;
    event->GetActionMasked(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        mEatingTouch = FALSE;
    }

    if (!mEatingTouch) {
        Boolean handled;
        ViewGroup::OnTouchEvent(event, &handled);
        if (action == IMotionEvent::ACTION_DOWN && !handled) {
            mEatingTouch = TRUE;
        }
    }

    if (action == IMotionEvent::ACTION_UP || action == IMotionEvent::ACTION_CANCEL) {
        mEatingTouch = FALSE;
    }

    *res = TRUE;
    return NOERROR;
}

void ToolBar::OnSetLayoutParams(
    /* [in] */ IView* child,
    /* [in] */ IViewGroupLayoutParams* layoutParams)
{
    /*
     * Apps may set ActionBar.LayoutParams on their action bar custom views when
     * a Toolbar is actually acting in the role of the action bar. Perform a quick
     * switch with Toolbar.LayoutParams whenever this happens. This does leave open
     * one potential gotcha: if an app retains the ActionBar.LayoutParams reference
     * and attempts to keep making changes to it before layout those changes won't
     * be reflected in the final results.
     */
    if (!CheckLayoutParams(layoutParams)) {
        AutoPtr<IToolBarLayoutParams> lp;
        GenerateLayoutParams(layoutParams, (IToolBarLayoutParams**)&lp);
        child->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    }
}

ECode ToolBar::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IToolBarLayoutParams** layoutParams)
{
    VALIDATE_NOT_NULL(layoutParams)
    AutoPtr<LayoutParams> result = new LayoutParams();
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    result->constructor(ctx, attrs);
    *layoutParams = IToolBarLayoutParams::Probe(result);
    REFCOUNT_ADD(*layoutParams)
    return NOERROR;
}

ECode ToolBar::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p,
    /* [out] */ IToolBarLayoutParams** layoutParams)
{
    VALIDATE_NOT_NULL(layoutParams)
    if (IToolBarLayoutParams::Probe(p)) {
            AutoPtr<LayoutParams> result = new LayoutParams();
            result->constructor(IToolBarLayoutParams::Probe(p));
            *layoutParams = IToolBarLayoutParams::Probe(result);
            REFCOUNT_ADD(*layoutParams)
            return NOERROR;
        } else if (IActionBarLayoutParams::Probe(p)) {
            AutoPtr<LayoutParams> result = new LayoutParams();
            result->constructor(IActionBarLayoutParams::Probe(p));
            *layoutParams = IToolBarLayoutParams::Probe(result);
            REFCOUNT_ADD(*layoutParams)
            return NOERROR;
        } else if (IViewGroupMarginLayoutParams::Probe(p)) {
            AutoPtr<LayoutParams> result = new LayoutParams();
            result->constructor(IViewGroupMarginLayoutParams::Probe(p));
            *layoutParams = IToolBarLayoutParams::Probe(result);
            REFCOUNT_ADD(*layoutParams)
            return NOERROR;
        } else {
            AutoPtr<LayoutParams> result = new LayoutParams();
            result->constructor(p);
            *layoutParams = IToolBarLayoutParams::Probe(result);
            REFCOUNT_ADD(*layoutParams)
            return NOERROR;
        }
}

ECode ToolBar::GetWrapper(
    /* [out] */ IDecorToolbar** bar)
{
    VALIDATE_NOT_NULL(bar)
    if (mWrapper == NULL) {
        assert(0);
        //mWrapper = new ToolbarWidgetWrapper(this, true);
    }
    *bar = IDecorToolbar::Probe(mWrapper);
    REFCOUNT_ADD(*bar)
    return NOERROR;
}

ECode ToolBar::SetCollapsible(
    /* [in] */ Boolean collapsible)
{
    mCollapsible = collapsible;
    RequestLayout();
    return NOERROR;
}

ECode ToolBar::SetMenuCallbacks(
    /* [in] */ IMenuPresenterCallback* pcb,
    /* [in] */ IMenuBuilderCallback* mcb)
{
    mActionMenuPresenterCallback = pcb;
    mMenuBuilderCallback = mcb;
    return NOERROR;
}

AutoPtr<IParcelable> ToolBar::OnSaveInstanceState()
{
    assert(0);
    /*SavedState state = new SavedState(super.onSaveInstanceState());

    if (mExpandedMenuPresenter != null && mExpandedMenuPresenter.mCurrentExpandedItem != null) {
        state.expandedMenuItemId = mExpandedMenuPresenter.mCurrentExpandedItem.getItemId();
    }

    state.isOverflowOpen = isOverflowMenuShowing();

    return state;*/
}

void ToolBar::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    assert(0);
    /*final SavedState ss = (SavedState) state;
    super.onRestoreInstanceState(ss.getSuperState());

    final Menu menu = mMenuView != null ? mMenuView.peekMenu() : null;
    if (ss.expandedMenuItemId != 0 && mExpandedMenuPresenter != null && menu != null) {
        final MenuItem item = menu.findItem(ss.expandedMenuItemId);
        if (item != null) {
            item.expandActionView();
        }
    }

    if (ss.isOverflowOpen) {
        postShowOverflowMenu();
    }*/
}

ECode ToolBar::OnDetachedFromWindow()
{
    Boolean res;
    ViewGroup::OnDetachedFromWindow();
    RemoveCallbacks(mShowOverflowMenuRunnable, &res);
    return NOERROR;
}

void ToolBar::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 width = 0;
    Int32 height = 0;
    Int32 childState = 0;

    AutoPtr< ArrayOf<Int32> > collapsingMargins = mTempMargins;
    Int32 marginStartIndex;
    Int32 marginEndIndex;
    Boolean isLayoutRtl;
    if (IsLayoutRtl(&isLayoutRtl), isLayoutRtl) {
        marginStartIndex = 1;
        marginEndIndex = 0;
    } else {
        marginStartIndex = 0;
        marginEndIndex = 1;
    }

    // System views measure first.

    Int32 navWidth = 0;
    if (ShouldLayout(IView::Probe(mNavButtonView))) {
        MeasureChildConstrained(IView::Probe(mNavButtonView), widthMeasureSpec,
            width, heightMeasureSpec, 0, mMaxButtonHeight);
        Int32 measureWidth, measureHeight, state;
        IView::Probe(mNavButtonView)->GetMeasuredWidth(&measureWidth);
        navWidth = measureWidth + GetHorizontalMargins(IView::Probe(mNavButtonView));
        IView::Probe(mNavButtonView)->GetMeasuredHeight(&measureHeight);
        height = Elastos::Core::Math::Max(height, measureHeight + GetVerticalMargins(IView::Probe(mNavButtonView)));
        IView::Probe(mNavButtonView)->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);
    }

    if (ShouldLayout(IView::Probe(mCollapseButtonView))) {
        MeasureChildConstrained(IView::Probe(mCollapseButtonView), widthMeasureSpec, width,
                heightMeasureSpec, 0, mMaxButtonHeight);
        Int32 measureWidth, measureHeight, state;
        IView::Probe(mCollapseButtonView)->GetMeasuredWidth(&measureWidth);
        navWidth = measureWidth + GetHorizontalMargins(IView::Probe(mCollapseButtonView));
        IView::Probe(mCollapseButtonView)->GetMeasuredHeight(&measureHeight);
        height = Elastos::Core::Math::Max(height, measureHeight + GetVerticalMargins(IView::Probe(mCollapseButtonView)));
        IView::Probe(mCollapseButtonView)->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);
    }

    Int32 contentInsetStart;
    GetContentInsetStart(&contentInsetStart);
    width += Elastos::Core::Math::Max(contentInsetStart, navWidth);
    (*collapsingMargins)[marginStartIndex] = Elastos::Core::Math::Max(0, contentInsetStart - navWidth);

    Int32 menuWidth = 0;
    if (ShouldLayout(IView::Probe(mMenuView))) {
        MeasureChildConstrained(IView::Probe(mMenuView), widthMeasureSpec, width, heightMeasureSpec, 0,
                mMaxButtonHeight);
        Int32 measureWidth, measureHeight, state;
        IView::Probe(mMenuView)->GetMeasuredWidth(&measureWidth);
        menuWidth = measureWidth + GetHorizontalMargins(IView::Probe(mMenuView));
        IView::Probe(mMenuView)->GetMeasuredHeight(&measureHeight);
        height = Elastos::Core::Math::Max(height, measureHeight + GetVerticalMargins(IView::Probe(mMenuView)));
        IView::Probe(mMenuView)->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);
    }

    Int32 contentInsetEnd;
    GetContentInsetEnd(&contentInsetEnd);
    width += Elastos::Core::Math::Max(contentInsetEnd, menuWidth);
    (*collapsingMargins)[marginEndIndex] = Elastos::Core::Math::Max(0, contentInsetEnd - menuWidth);

    if (ShouldLayout(mExpandedActionView)) {
        width += MeasureChildCollapseMargins(mExpandedActionView, widthMeasureSpec, width,
                heightMeasureSpec, 0, collapsingMargins);
        Int32 measureHeight, state;
        mExpandedActionView->GetMeasuredHeight(&measureHeight);
        height = Elastos::Core::Math::Max(height, measureHeight + GetVerticalMargins(mExpandedActionView));
        mExpandedActionView->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);
    }

    if (ShouldLayout(IView::Probe(mLogoView))) {
        width += MeasureChildCollapseMargins(IView::Probe(mLogoView), widthMeasureSpec, width,
                heightMeasureSpec, 0, collapsingMargins);
        Int32 measureHeight, state;
        IView::Probe(mLogoView)->GetMeasuredHeight(&measureHeight);
        height = Elastos::Core::Math::Max(height, measureHeight + GetVerticalMargins(IView::Probe(mLogoView)));
        IView::Probe(mLogoView)->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);
    }

    Int32 childCount;
    GetChildCount(&childCount);
    AutoPtr<IView> child;
    for (Int32 i = 0; i < childCount; i++) {
        child = NULL;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> temp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&temp);
        AutoPtr<IToolBarLayoutParams> lp = IToolBarLayoutParams::Probe(temp);
        Int32 viewTpye;
        assert(0);
        //lp->GetViewType(&viewTpye);
        if (viewTpye != LayoutParams::CUSTOM || !ShouldLayout(child)) {
            // We already got all system views above. Skip them and GONE views.
            continue;
        }

        width += MeasureChildCollapseMargins(child, widthMeasureSpec, width,
                heightMeasureSpec, 0, collapsingMargins);
        Int32 measureHeight, state;
        child->GetMeasuredHeight(&measureHeight);
        height = Elastos::Core::Math::Max(height, measureHeight + GetVerticalMargins(child));
        child->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);
    }

    Int32 titleWidth = 0;
    Int32 titleHeight = 0;
    Int32 titleVertMargins = mTitleMarginTop + mTitleMarginBottom;
    Int32 titleHorizMargins = mTitleMarginStart + mTitleMarginEnd;
    if (ShouldLayout(IView::Probe(mTitleTextView))) {
        titleWidth = MeasureChildCollapseMargins(IView::Probe(mTitleTextView), widthMeasureSpec,
                width + titleHorizMargins, heightMeasureSpec, titleVertMargins, collapsingMargins);
        Int32 measureWidth, measureHeight, state;
        IView::Probe(mTitleTextView)->GetMeasuredWidth(&measureWidth);
        titleWidth = measureWidth + GetHorizontalMargins(IView::Probe(mTitleTextView));
        IView::Probe(mTitleTextView)->GetMeasuredHeight(&measureHeight);
        titleHeight = measureHeight + GetVerticalMargins(IView::Probe(mTitleTextView));
        IView::Probe(mTitleTextView)->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);
    }
    if (ShouldLayout(IView::Probe(mSubtitleTextView))) {
        titleWidth = Elastos::Core::Math::Max(titleWidth,
            MeasureChildCollapseMargins(IView::Probe(mSubtitleTextView), widthMeasureSpec,
            width + titleHorizMargins, heightMeasureSpec, titleHeight + titleVertMargins, collapsingMargins));
        Int32 measureHeight, state;
        IView::Probe(mSubtitleTextView)->GetMeasuredHeight(&measureHeight);
        titleHeight += measureHeight + GetVerticalMargins(IView::Probe(mSubtitleTextView));
        IView::Probe(mSubtitleTextView)->GetMeasuredState(&state);
        childState = CombineMeasuredStates(childState, state);
    }

    width += titleWidth;
    height = Elastos::Core::Math::Max(height, titleHeight);

    // Measurement already took padding into account for available space for the children,
    // add it in for the final size.
    Int32 paddingleft, paddingRight, paddingTop, paddingBottom;
    GetPaddingLeft(&paddingleft);
    GetPaddingRight(&paddingRight);
    width += paddingleft + paddingRight;
    GetPaddingTop(&paddingTop);
    GetPaddingBottom(&paddingBottom);
    height += paddingTop + paddingBottom;

    Int32 measuredWidth = ResolveSizeAndState(
            Elastos::Core::Math::Max(width, GetSuggestedMinimumWidth()),
            widthMeasureSpec, childState & MEASURED_STATE_MASK);
    Int32 measuredHeight = ResolveSizeAndState(
            Elastos::Core::Math::Max(height, GetSuggestedMinimumHeight()),
            heightMeasureSpec, childState << MEASURED_HEIGHT_STATE_SHIFT);

    SetMeasuredDimension(measuredWidth, ShouldCollapse() ? 0 : measuredHeight);
}

ECode ToolBar::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 direction;
    GetLayoutDirection(&direction);
    Boolean isRtl = direction == IView::LAYOUT_DIRECTION_RTL;
    Int32 width, height, paddingLeft, paddingRight, paddingTop, paddingBottom;
    GetWidth(&width);
    GetHeight(&height);
    GetPaddingLeft(&paddingLeft);
    GetPaddingRight(&paddingRight);
    GetPaddingTop(&paddingTop);
    GetPaddingBottom(&paddingBottom);
    Int32 left = paddingLeft;
    Int32 right = width - paddingRight;

    AutoPtr< ArrayOf<Int32> > collapsingMargins = mTempMargins;
    (*collapsingMargins)[0] = (*collapsingMargins)[1] = 0;

    // Align views within the minimum toolbar height, if set.
    Int32 alignmentHeight;
    GetMinimumHeight(&alignmentHeight);

    if (ShouldLayout(IView::Probe(mNavButtonView))) {
        if (isRtl) {
            right = LayoutChildRight(IView::Probe(mNavButtonView), right, collapsingMargins,
                    alignmentHeight);
        } else {
            left = LayoutChildLeft(IView::Probe(mNavButtonView), left, collapsingMargins,
                    alignmentHeight);
        }
    }

    if (ShouldLayout(IView::Probe(mCollapseButtonView))) {
        if (isRtl) {
            right = LayoutChildRight(IView::Probe(mCollapseButtonView), right, collapsingMargins,
                    alignmentHeight);
        } else {
            left = LayoutChildLeft(IView::Probe(mCollapseButtonView), left, collapsingMargins,
                    alignmentHeight);
        }
    }

    if (ShouldLayout(IView::Probe(mMenuView))) {
        if (isRtl) {
            left = LayoutChildLeft(IView::Probe(mMenuView), left, collapsingMargins,
                    alignmentHeight);
        } else {
            right = LayoutChildRight(IView::Probe(mMenuView), right, collapsingMargins,
                    alignmentHeight);
        }
    }

    Int32 insetLeft, insetRight;
    GetContentInsetLeft(&insetLeft);
    GetContentInsetRight(&insetRight);
    (*collapsingMargins)[0] = Elastos::Core::Math::Max(0, insetLeft - left);
    (*collapsingMargins)[1] = Elastos::Core::Math::Max(0, insetRight - (width - paddingRight - right));
    left = Elastos::Core::Math::Max(left, insetLeft);
    right = Elastos::Core::Math::Min(right, width - paddingRight - insetRight);

    if (ShouldLayout(mExpandedActionView)) {
        if (isRtl) {
            right = LayoutChildRight(mExpandedActionView, right, collapsingMargins,
                    alignmentHeight);
        } else {
            left = LayoutChildLeft(mExpandedActionView, left, collapsingMargins,
                    alignmentHeight);
        }
    }

    if (ShouldLayout(IView::Probe(mLogoView))) {
        if (isRtl) {
            right = LayoutChildRight(IView::Probe(mLogoView), right, collapsingMargins,
                    alignmentHeight);
        } else {
            left = LayoutChildLeft(IView::Probe(mLogoView), left, collapsingMargins,
                    alignmentHeight);
        }
    }

    Boolean layoutTitle = ShouldLayout(IView::Probe(mTitleTextView));
    Boolean layoutSubtitle = ShouldLayout(IView::Probe(mSubtitleTextView));
    Int32 titleHeight = 0;
    if (layoutTitle) {
        AutoPtr<IViewGroupLayoutParams> temp;
        IView::Probe(mTitleTextView)->GetLayoutParams((IViewGroupLayoutParams**)&temp);
        AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
        Int32 height, top, bottom;
        IView::Probe(mTitleTextView)->GetMeasuredHeight(&height);
        lp->GetTopMargin(&top);
        lp->GetBottomMargin(&bottom);
        titleHeight += top + height + bottom;
    }
    if (layoutSubtitle) {
        AutoPtr<IViewGroupLayoutParams> temp;
        IView::Probe(mSubtitleTextView)->GetLayoutParams((IViewGroupLayoutParams**)&temp);
        AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
        Int32 height, top, bottom;
        IView::Probe(mSubtitleTextView)->GetMeasuredHeight(&height);
        lp->GetTopMargin(&top);
        lp->GetBottomMargin(&bottom);
        titleHeight += top + height + bottom;
    }

    if (layoutTitle || layoutSubtitle) {
        Int32 titleTop;
        AutoPtr<IView> topChild = layoutTitle ? IView::Probe(mTitleTextView) : IView::Probe(mSubtitleTextView);
        AutoPtr<IView> bottomChild = layoutSubtitle ? IView::Probe(mSubtitleTextView) : IView::Probe(mTitleTextView);
        AutoPtr<IViewGroupLayoutParams> topTemp;
        IView::Probe(topChild)->GetLayoutParams((IViewGroupLayoutParams**)&topTemp);
        AutoPtr<IViewGroupMarginLayoutParams> toplp = IViewGroupMarginLayoutParams::Probe(topTemp);
        AutoPtr<IViewGroupLayoutParams> bottomTemp;
        IView::Probe(bottomChild)->GetLayoutParams((IViewGroupLayoutParams**)&bottomTemp);
        AutoPtr<IViewGroupMarginLayoutParams> bottomlp = IViewGroupMarginLayoutParams::Probe(bottomTemp);
        Int32 titleWidth, subWidth;
        IView::Probe(mTitleTextView)->GetMeasuredWidth(&titleWidth);
        IView::Probe(mSubtitleTextView)->GetMeasuredWidth(&subWidth);
        Boolean titleHasWidth = (layoutTitle && titleWidth > 0)
                || (layoutSubtitle && subWidth > 0);

        switch (mGravity & IGravity::VERTICAL_GRAVITY_MASK) {
            case IGravity::TOP: {
                Int32 top, topMargin;
                GetPaddingTop(&top);
                toplp->GetTopMargin(&topMargin);
                titleTop = top + topMargin + mTitleMarginTop;
                break;
            }
            default:
            case IGravity::CENTER_VERTICAL: {
                Int32 space = height - paddingTop - paddingBottom;
                Int32 spaceAbove = (space - titleHeight) / 2;
                Int32 top, bottom, secondBottom;
                toplp->GetTopMargin(&top);
                toplp->GetBottomMargin(&secondBottom);
                bottomlp->GetBottomMargin(&bottom);
                if (spaceAbove < top + mTitleMarginTop) {
                    spaceAbove = top + mTitleMarginTop;
                } else {
                    Int32 spaceBelow = height - paddingBottom - titleHeight -
                            spaceAbove - paddingTop;
                    if (spaceBelow < bottom + mTitleMarginBottom) {
                        spaceAbove = Elastos::Core::Math::Max(0, spaceAbove -
                                (secondBottom + mTitleMarginBottom - spaceBelow));
                    }
                }
                titleTop = paddingTop + spaceAbove;
                break;
            }
            case IGravity::BOTTOM: {
                Int32 bottom;
                bottomlp->GetBottomMargin(&bottom);
                titleTop = height - paddingBottom - bottom - mTitleMarginBottom - titleHeight;
                break;
            }
        }
        if (isRtl) {
            Int32 rd = (titleHasWidth ? mTitleMarginStart : 0) - (*collapsingMargins)[1];
            right -= Elastos::Core::Math::Max(0, rd);
            (*collapsingMargins)[1] = Elastos::Core::Math::Max(0, -rd);
            Int32 titleRight = right;
            Int32 subtitleRight = right;

            if (layoutTitle) {
                AutoPtr<IViewGroupLayoutParams> temp;
                IView::Probe(mTitleTextView)->GetLayoutParams((IViewGroupLayoutParams**)&temp);
                AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
                Int32 width, height;
                IView::Probe(mTitleTextView)->GetMeasuredWidth(&width);
                Int32 titleLeft = titleRight - width;
                IView::Probe(mTitleTextView)->GetMeasuredHeight(&height);
                Int32 titleBottom = titleTop + height;
                IView::Probe(mTitleTextView)->Layout(titleLeft, titleTop, titleRight, titleBottom);
                titleRight = titleLeft - mTitleMarginEnd;
                Int32 bottom;
                lp->GetBottomMargin(&bottom);
                titleTop = titleBottom + bottom;
            }
            if (layoutSubtitle) {
                AutoPtr<IViewGroupLayoutParams> temp;
                IView::Probe(mSubtitleTextView)->GetLayoutParams((IViewGroupLayoutParams**)&temp);
                AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
                Int32 top;
                lp->GetTopMargin(&top);
                titleTop += top;
                Int32 width, height;
                IView::Probe(mSubtitleTextView)->GetMeasuredWidth(&width);
                Int32 subtitleLeft = subtitleRight - width;
                IView::Probe(mSubtitleTextView)->GetMeasuredHeight(&height);
                Int32 subtitleBottom = titleTop + height;
                IView::Probe(mSubtitleTextView)->Layout(subtitleLeft, titleTop, subtitleRight, subtitleBottom);
                subtitleRight = subtitleRight - mTitleMarginEnd;
                Int32 bottom;
                lp->GetBottomMargin(&bottom);
                titleTop = subtitleBottom + bottom;
            }
            if (titleHasWidth) {
                right = Elastos::Core::Math::Min(titleRight, subtitleRight);
            }
        } else {
            Int32 ld = (titleHasWidth ? mTitleMarginStart : 0) - (*collapsingMargins)[0];
            left += Elastos::Core::Math::Max(0, ld);
            (*collapsingMargins)[0] = Elastos::Core::Math::Max(0, -ld);
            Int32 titleLeft = left;
            Int32 subtitleLeft = left;

            if (layoutTitle) {
                AutoPtr<IViewGroupLayoutParams> temp;
                IView::Probe(mTitleTextView)->GetLayoutParams((IViewGroupLayoutParams**)&temp);
                AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
                Int32 width, height;
                IView::Probe(mTitleTextView)->GetMeasuredWidth(&width);
                Int32 titleRight = titleLeft + width;
                IView::Probe(mTitleTextView)->GetMeasuredHeight(&height);
                Int32 titleBottom = titleTop + height;
                IView::Probe(mTitleTextView)->Layout(titleLeft, titleTop, titleRight, titleBottom);
                titleLeft = titleRight + mTitleMarginEnd;
                Int32 bottom;
                lp->GetBottomMargin(&bottom);
                titleTop = titleBottom + bottom;
            }
            if (layoutSubtitle) {
                AutoPtr<IViewGroupLayoutParams> temp;
                IView::Probe(mSubtitleTextView)->GetLayoutParams((IViewGroupLayoutParams**)&temp);
                AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
                Int32 top, height;
                lp->GetTopMargin(&top);
                titleTop += top;
                IView::Probe(mSubtitleTextView)->GetMeasuredWidth(&top);
                Int32 subtitleRight = subtitleLeft + top;
                IView::Probe(mSubtitleTextView)->GetMeasuredHeight(&height);
                Int32 subtitleBottom = titleTop + height;
                IView::Probe(mSubtitleTextView)->Layout(subtitleLeft, titleTop, subtitleRight, subtitleBottom);
                subtitleLeft = subtitleRight + mTitleMarginEnd;
                Int32 bottom;
                lp->GetBottomMargin(&bottom);
                titleTop = subtitleBottom + bottom;
            }
            if (titleHasWidth) {
                left = Elastos::Core::Math::Max(titleLeft, subtitleLeft);
            }
        }
    }

    // Get all remaining children sorted for layout. This is all prepared
    // such that absolute layout direction can be used below.

    AddCustomViewsWithGravity(IList::Probe(mTempViews), IGravity::LEFT);
    Int32 leftViewsCount;
    mTempViews->GetSize(&leftViewsCount);
    AutoPtr<IInterface> obj;
    for (Int32 i = 0; i < leftViewsCount; i++) {
        obj = NULL;
        mTempViews->Get(i, (IInterface**)&obj);
        left = LayoutChildLeft(IView::Probe(obj), left, collapsingMargins, alignmentHeight);
    }

    AddCustomViewsWithGravity(IList::Probe(mTempViews), IGravity::RIGHT);
    Int32 rightViewsCount;
    mTempViews->GetSize(&rightViewsCount);
    for (Int32 i = 0; i < rightViewsCount; i++) {
        obj = NULL;
        mTempViews->Get(i, (IInterface**)&obj);
        right = LayoutChildRight(IView::Probe(obj), right, collapsingMargins, alignmentHeight);
    }

    // Centered views try to center with respect to the whole bar, but views pinned
    // to the left or right can push the mass of centered views to one side or the other.
    AddCustomViewsWithGravity(IList::Probe(mTempViews), IGravity::CENTER_HORIZONTAL);
    Int32 centerViewsWidth = GetViewListMeasuredWidth(IList::Probe(mTempViews), collapsingMargins);
    Int32 parentCenter = paddingLeft + (width - paddingLeft - paddingRight) / 2;
    Int32 halfCenterViewsWidth = centerViewsWidth / 2;
    Int32 centerLeft = parentCenter - halfCenterViewsWidth;
    Int32 centerRight = centerLeft + centerViewsWidth;
    if (centerLeft < left) {
        centerLeft = left;
    } else if (centerRight > right) {
        centerLeft -= centerRight - right;
    }

    Int32 centerViewsCount;
    mTempViews->GetSize(&centerViewsCount);
    for (Int32 i = 0; i < centerViewsCount; i++) {
        obj = NULL;
        mTempViews->Get(i, (IInterface**)&obj);
        centerLeft = LayoutChildLeft(IView::Probe(obj), centerLeft, collapsingMargins, alignmentHeight);
    }

    mTempViews->Clear();
    return NOERROR;
}

ECode ToolBar::GenerateDefaultLayoutParams(
    /* [out] */ IToolBarLayoutParams** params)
{
    VALIDATE_NOT_NULL(params)
    AutoPtr<LayoutParams> obj = new LayoutParams();
    obj->constructor(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
    *params = obj;
    REFCOUNT_ADD(*params)
    return NOERROR;
}

Boolean ToolBar::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return ViewGroup::CheckLayoutParams(p)
        && IToolBarLayoutParams::Probe(p);
}

ECode ToolBar::EnsureLogoView()
{
    if (mLogoView == NULL) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        CImageView::New(ctx, (IImageView**)&mLogoView);
    }
    return NOERROR;
}

ECode ToolBar::EnsureMenu()
{
    EnsureMenuView();
    AutoPtr<IMenuBuilder> menu;
    mMenuView->PeekMenu((IMenuBuilder**)&menu);
    if (menu == NULL) {
        // Initialize a new menu for the first time.
        AutoPtr<IMenu> temp;
        mMenuView->GetMenu((IMenu**)&temp);
        AutoPtr<IMenuBuilder> menu = IMenuBuilder::Probe(temp);
        if (mExpandedMenuPresenter == NULL) {
            mExpandedMenuPresenter = new ExpandedActionViewMenuPresenter(this);
        }
        mMenuView->SetExpandedActionViewsExclusive(TRUE);
        menu->AddMenuPresenter(mExpandedMenuPresenter, mPopupContext);
    }
    return NOERROR;
}

ECode ToolBar::EnsureMenuView()
{
    if (mMenuView == NULL) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        assert(0);
        //CActionMenuView::New(ctx, (IActionMenuView**)&mMenuView);
        mMenuView->SetPopupTheme(mPopupTheme);
        mMenuView->SetOnMenuItemClickListener(mMenuViewItemClickListener);
        mMenuView->SetMenuCallbacks(mActionMenuPresenterCallback, mMenuBuilderCallback);
        AutoPtr<IToolBarLayoutParams> lp;
        GenerateDefaultLayoutParams((IToolBarLayoutParams**)&lp);
        IActionBarLayoutParams::Probe(lp)->SetGravity(
            IGravity::END | (mButtonGravity & IGravity::VERTICAL_GRAVITY_MASK));
        IView::Probe(mMenuView)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
        AddSystemView(IView::Probe(mMenuView));
    }
    return NOERROR;
}

AutoPtr<IMenuInflater> ToolBar::GetMenuInflater()
{
    AutoPtr<IMenuInflater> temp;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    CMenuInflater::New(ctx, (IMenuInflater**)&temp);
    return temp;
}

ECode ToolBar::EnsureNavButtonView()
{
    if (mNavButtonView == NULL) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        assert(0);
        //CImageButton::New(ctx, NULL, 0, mNavButtonStyle, (IImageButton**)&mNavButtonView);
        AutoPtr<IToolBarLayoutParams> lp;
        GenerateDefaultLayoutParams((IToolBarLayoutParams**)&lp);
        IActionBarLayoutParams::Probe(lp)->SetGravity(
            IGravity::START | (mButtonGravity & IGravity::VERTICAL_GRAVITY_MASK));
        IView::Probe(mNavButtonView)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    }
    return NOERROR;
}

ECode ToolBar::EnsureCollapseButtonView()
{
    if (mCollapseButtonView == NULL) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        assert(0);
        //CImageButton::New(ctx, NULL, 0, mNavButtonStyle, (IImageButton**)&mCollapseButtonView);
        IImageView::Probe(mCollapseButtonView)->SetImageDrawable(mCollapseIcon);
        AutoPtr<IToolBarLayoutParams> lp;
        GenerateDefaultLayoutParams((IToolBarLayoutParams**)&lp);
        IActionBarLayoutParams::Probe(lp)->SetGravity(
            IGravity::START | (mButtonGravity & IGravity::VERTICAL_GRAVITY_MASK));
        //lp->SetViewType(LayoutParams::EXPANDED);
        IView::Probe(mCollapseButtonView)->SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
        AutoPtr<IViewOnClickListener> listener = new CollapseOnClickListener(this);
        IView::Probe(mCollapseButtonView)->SetOnClickListener(listener);
    }
    return NOERROR;
}

ECode ToolBar::AddSystemView(
    /* [in] */ IView* v)
{
    AutoPtr<IViewGroupLayoutParams> vlp;
    v->GetLayoutParams((IViewGroupLayoutParams**)&vlp);

    AutoPtr<IToolBarLayoutParams> lp;
    if (vlp == NULL) {
        GenerateDefaultLayoutParams((IToolBarLayoutParams**)&lp);
    } else if (!CheckLayoutParams(vlp)) {
        GenerateLayoutParams(vlp, (IToolBarLayoutParams**)&lp);
    } else {
        lp = IToolBarLayoutParams::Probe(vlp);
    }
    assert(0);
    //lp->SetViewType(LayoutParams::SYSTEM);
    AddView(v, IViewGroupLayoutParams::Probe(lp));
}

ECode ToolBar::PostShowOverflowMenu()
{
    Boolean res;
    RemoveCallbacks(mShowOverflowMenuRunnable, &res);
    Post(mShowOverflowMenuRunnable, &res);
    return NOERROR;
}

ECode ToolBar::MeasureChildConstrained(
    /* [in] */ IView* child,
    /* [in] */ Int32 parentWidthSpec,
    /* [in] */ Int32 widthUsed,
    /* [in] */ Int32 parentHeightSpec,
    /* [in] */ Int32 heightUsed,
    /* [in] */ Int32 heightConstraint)
{
    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(params);
    Int32 width, height, leftMargin, rightMargin, topMargin, bottomMargin;
    params->GetWidth(&width);
    params->GetHeight(&height);
    lp->GetLeftMargin(&leftMargin);
    lp->GetRightMargin(&rightMargin);
    lp->GetTopMargin(&topMargin);
    lp->GetBottomMargin(&bottomMargin);
    Int32 childWidthSpec = GetChildMeasureSpec(parentWidthSpec,
            mPaddingLeft + mPaddingRight + leftMargin + rightMargin
                    + widthUsed, width);
    Int32 childHeightSpec = GetChildMeasureSpec(parentHeightSpec,
            mPaddingTop + mPaddingBottom + topMargin + bottomMargin
                    + heightUsed, height);

    Int32 childHeightMode = MeasureSpec::GetMode(childHeightSpec);
    if (childHeightMode != MeasureSpec::EXACTLY && heightConstraint >= 0) {
        Int32 size = childHeightMode != MeasureSpec::UNSPECIFIED ?
               Elastos::Core::Math::Min(MeasureSpec::GetSize(childHeightSpec), heightConstraint) :
                heightConstraint;
        childHeightSpec = MeasureSpec::MakeMeasureSpec(size, MeasureSpec::EXACTLY);
    }
    child->Measure(childWidthSpec, childHeightSpec);
    return NOERROR;
}

Int32 ToolBar::MeasureChildCollapseMargins(
    /* [in] */ IView* child,
    /* [in] */ Int32 parentWidthMeasureSpec,
    /* [in] */ Int32 widthUsed,
    /* [in] */ Int32 parentHeightMeasureSpec,
    /* [in] */ Int32 heightUsed,
    /* [in] */ ArrayOf<Int32> * collapsingMargins)
{
    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(params);

    Int32 width, height, leftMargin, rightMargin, topMargin, bottomMargin;
    params->GetWidth(&width);
    params->GetHeight(&height);
    lp->GetLeftMargin(&leftMargin);
    lp->GetRightMargin(&rightMargin);
    lp->GetTopMargin(&topMargin);
    lp->GetBottomMargin(&bottomMargin);

    Int32 leftDiff = leftMargin - (*collapsingMargins)[0];
    Int32 rightDiff = rightMargin - (*collapsingMargins)[1];
    Int32 left = Elastos::Core::Math::Max(0, leftDiff);
    Int32 right = Elastos::Core::Math::Max(0, rightDiff);
    Int32 hMargins = left + right;
    (*collapsingMargins)[0] = Elastos::Core::Math::Max(0, -leftDiff);
    (*collapsingMargins)[1] = Elastos::Core::Math::Max(0, -rightDiff);

    Int32 childWidthMeasureSpec = GetChildMeasureSpec(parentWidthMeasureSpec,
            mPaddingLeft + mPaddingRight + hMargins + widthUsed, width);
    Int32 childHeightMeasureSpec = GetChildMeasureSpec(parentHeightMeasureSpec,
            mPaddingTop + mPaddingBottom + topMargin + bottomMargin
                    + heightUsed, height);

    child->Measure(childWidthMeasureSpec, childHeightMeasureSpec);
    Int32 childWidth;
    child->GetMeasuredWidth(&childWidth);
    return childWidth + hMargins;
}

Boolean ToolBar::ShouldCollapse()
{
    if (!mCollapsible) return FALSE;

    Int32 childCount;
    GetChildCount(&childCount);
    AutoPtr<IView> child;
    for (Int32 i = 0; i < childCount; i++) {
        child = NULL;
        GetChildAt(i, (IView**)&child);
        Int32 width, height;
        if (ShouldLayout(child)
            && (child->GetMeasuredWidth(&width), width) > 0
            && (child->GetMeasuredHeight(&height), height) > 0) {
            return FALSE;
        }
    }
    return TRUE;
}

Int32 ToolBar::GetViewListMeasuredWidth(
    /* [in] */ IList* views,
    /* [in] */ ArrayOf<Int32> * collapsingMargins)
{
    Int32 collapseLeft = (*collapsingMargins)[0];
    Int32 collapseRight = (*collapsingMargins)[1];
    Int32 width = 0;
    Int32 count;
    views->GetSize(&count);
    AutoPtr<IInterface> obj;
    for (Int32 i = 0; i < count; i++) {
        obj = NULL;
        views->Get(i, (IInterface**)&obj);
        AutoPtr<IView> v = IView::Probe(obj);
        AutoPtr<IViewGroupLayoutParams> temp;
        v->GetLayoutParams((IViewGroupLayoutParams**)&temp);
        AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
        Int32 left, right;
        lp->GetLeftMargin(&left);
        lp->GetRightMargin(&right);
        Int32 l = left - collapseLeft;
        Int32 r = right - collapseRight;
        Int32 leftMargin = Elastos::Core::Math::Max(0, l);
        Int32 rightMargin = Elastos::Core::Math::Max(0, r);
        collapseLeft = Elastos::Core::Math::Max(0, -l);
        collapseRight = Elastos::Core::Math::Max(0, -r);
        Int32 width;
        v->GetMeasuredWidth(&width);
        width += leftMargin + width + rightMargin;
    }
    return width;
}

Int32 ToolBar::LayoutChildLeft(
    /* [in] */IView* child,
    /* [in] */ Int32 left,
    /* [in] */ ArrayOf<Int32> * collapsingMargins,
    /* [in] */ Int32 alignmentHeight)
{
    AutoPtr<IViewGroupLayoutParams> temp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&temp);
    AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
    Int32 leftMargin, right;
    lp->GetLeftMargin(&leftMargin);
    Int32 l = leftMargin - (*collapsingMargins)[0];
    left += Elastos::Core::Math::Max(0, l);
    (*collapsingMargins)[0] = Elastos::Core::Math::Max(0, -l);
    Int32 top = GetChildTop(child, alignmentHeight);
    Int32 childWidth, height;
    child->GetMeasuredWidth(&childWidth);
    child->GetMeasuredHeight(&height);
    child->Layout(left, top, left + childWidth, top + height);
    lp->GetRightMargin(&right);
    left += childWidth + right;
    return left;
}

Int32 ToolBar::LayoutChildRight(
    /* [in] */ IView* child,
    /* [in] */ Int32 right,
    /* [in] */ ArrayOf<Int32> * collapsingMargins,
    /* [in] */ Int32 alignmentHeight)
{
    AutoPtr<IViewGroupLayoutParams> temp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&temp);
    AutoPtr<IViewGroupMarginLayoutParams> lp = IViewGroupMarginLayoutParams::Probe(temp);
    Int32 leftMargin, rightMargin;
    lp->GetRightMargin(&rightMargin);

    Int32 r = rightMargin - (*collapsingMargins)[1];
    right -= Elastos::Core::Math::Max(0, r);
    (*collapsingMargins)[1] = Elastos::Core::Math::Max(0, -r);
    Int32 top = GetChildTop(child, alignmentHeight);
    Int32 childWidth, height;
    child->GetMeasuredWidth(&childWidth);
    child->GetMeasuredHeight(&height);
    child->Layout(right - childWidth, top, right, top + height);
    lp->GetLeftMargin(&leftMargin);
    right -= childWidth + leftMargin;
    return right;
}

Int32 ToolBar::GetChildTop(
    /* [in] */ IView* child,
    /* [in] */ Int32 alignmentHeight)
{
    AutoPtr<IViewGroupLayoutParams> temp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&temp);
    AutoPtr<IActionBarLayoutParams> lp = IActionBarLayoutParams::Probe(temp);

    Int32 childHeight;
    child->GetMeasuredHeight(&childHeight);
    Int32 alignmentOffset = alignmentHeight > 0 ? (childHeight - alignmentHeight) / 2 : 0;
    Int32 gravity;
    lp->GetGravity(&gravity);
    switch (GetChildVerticalGravity(gravity)) {
        case IGravity::TOP: {
            Int32 top;
            GetPaddingTop(&top);
            return top - alignmentOffset;
        }

        case IGravity::BOTTOM: {
            Int32 height, bottom, margin;
            GetHeight(&height);
            GetPaddingBottom(&bottom);
            IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&margin);
            return height - bottom - childHeight - margin - alignmentOffset;
        }

        default:
        case IGravity::CENTER_VERTICAL: {
            Int32 paddingTop;
            GetPaddingTop(&paddingTop);
            Int32 paddingBottom;
            GetPaddingBottom(&paddingBottom);
            Int32 height;
            GetHeight(&height);
            Int32 space = height - paddingTop - paddingBottom;
            Int32 spaceAbove = (space - childHeight) / 2;
            Int32 top, bottom;
            IViewGroupMarginLayoutParams::Probe(lp)->GetTopMargin(&top);
            IViewGroupMarginLayoutParams::Probe(lp)->GetBottomMargin(&bottom);
            if (spaceAbove < top) {
                spaceAbove = top;
            } else {
                Int32 spaceBelow = height - paddingBottom - childHeight -
                        spaceAbove - paddingTop;
                if (spaceBelow < bottom) {
                    spaceAbove = Elastos::Core::Math::Max(0, spaceAbove - (bottom - spaceBelow));
                }
            }
            return paddingTop + spaceAbove;
        }
    }
}

Int32 ToolBar::GetChildVerticalGravity(
    /* [in] */ Int32 gravity)
{
    Int32 vgrav = gravity & IGravity::VERTICAL_GRAVITY_MASK;
    switch (vgrav) {
        case IGravity::TOP:
        case IGravity::BOTTOM:
        case IGravity::CENTER_VERTICAL:
            return vgrav;
        default:
            return mGravity & IGravity::VERTICAL_GRAVITY_MASK;
    }
}

void ToolBar::AddCustomViewsWithGravity(
    /* [in] */ IList* views,
    /* [in] */ Int32 gravity)
{
    Int32 direction;
    GetLayoutDirection(&direction);
    Boolean isRtl = direction == LAYOUT_DIRECTION_RTL;
    Int32 childCount;
    GetChildCount(&childCount);
    Int32 absGrav;
    Gravity::GetAbsoluteGravity(gravity, direction);
    views->Clear();
    AutoPtr<IView> child;
    if (isRtl) {
        for (Int32 i = childCount - 1; i >= 0; i--) {
            child = NULL;
            GetChildAt(i, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            Int32 viewTpye, gravity;
            assert(0);
            //IToolBarLayoutParams::Probe(lp)->GetViewType(&viewTpye);
            IActionBarLayoutParams::Probe(lp)->GetGravity(&gravity);
            if (viewTpye == LayoutParams::CUSTOM && ShouldLayout(child) &&
                    GetChildHorizontalGravity(gravity) == absGrav) {
                views->Add(child);
            }
        }
    } else {
        for (Int32 i = 0; i < childCount; i++) {
            child = NULL;
            GetChildAt(i, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
            Int32 viewTpye, gravity;
            assert(0);
            //IToolBarLayoutParams::Probe(lp)->GetViewType(&viewTpye);
            IActionBarLayoutParams::Probe(lp)->GetGravity(&gravity);
            if (viewTpye == LayoutParams::CUSTOM && ShouldLayout(child) &&
                    GetChildHorizontalGravity(gravity) == absGrav) {
                views->Add(child);
            }
        }
    }
}

Int32 ToolBar::GetChildHorizontalGravity(
    /* [in] */ Int32 gravity)
{
    Int32 ld;
    GetLayoutDirection(&ld);
    Int32 absGrav;
    Gravity::GetAbsoluteGravity(gravity, ld, &absGrav);
    Int32 hGrav = absGrav & IGravity::HORIZONTAL_GRAVITY_MASK;
    switch (hGrav) {
        case IGravity::LEFT:
        case IGravity::RIGHT:
        case IGravity::CENTER_HORIZONTAL:
            return hGrav;
        default:
            return ld == LAYOUT_DIRECTION_RTL ? IGravity::RIGHT : IGravity::LEFT;
    }
}

Boolean ToolBar::ShouldLayout(
    /* [in] */ IView* view)
{
    AutoPtr<IViewParent> parent;
    Int32 visibility;
    return view != NULL
        && (view->GetParent((IViewParent**)&parent), IView::Probe(parent))
        == (IView*)this->Probe(EIID_IView)
        && (view->GetVisibility(&visibility), visibility) != IView::GONE;
}

Int32 ToolBar::GetHorizontalMargins(
    /* [in] */ IView* v)
{
    AutoPtr<IViewGroupLayoutParams> temp;
    v->GetLayoutParams((IViewGroupLayoutParams**)&temp);
    AutoPtr<IViewGroupMarginLayoutParams> mlp = IViewGroupMarginLayoutParams::Probe(temp);
    Int32 start, end;
    mlp->GetMarginStart(&start);
    mlp->GetMarginEnd(&end);
    return start + end;
}

Int32 ToolBar::GetVerticalMargins(
    /* [in] */ IView* v)
{
    AutoPtr<IViewGroupLayoutParams> temp;
    v->GetLayoutParams((IViewGroupLayoutParams**)&temp);
    AutoPtr<IViewGroupMarginLayoutParams> mlp = IViewGroupMarginLayoutParams::Probe(temp);
    Int32 top, bottom;
    mlp->GetTopMargin(&top);
    mlp->GetBottomMargin(&bottom);
    return top + bottom;
}

Boolean ToolBar::IsCustomView(
    /* [in] */ IView* child)
{
    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    Int32 viewTpye;
    assert(0);
    //IToolBarLayoutParams::Probe(params)->GetViewType(&viewTpye);
    return viewTpye == LayoutParams::CUSTOM;
}

ECode ToolBar::SetChildVisibilityForExpandedActionView(
    /* [in] */ Boolean expand)
{
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        Int32 viewTpye;
        assert(0);
        //IToolBarLayoutParams::Probe(params)->GetViewType(&viewTpye);
        if (viewTpye != LayoutParams::EXPANDED && IView::Probe(mMenuView) != child) {
            child->SetVisibility(expand ? IView::GONE : IView::VISIBLE);
        }
    }
    return NOERROR;
}

ECode ToolBar::UpdateChildVisibilityForExpandedActionView(
    /* [in] */ IView* child)
{
    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    Int32 viewTpye;
    assert(0);
    //IToolBarLayoutParams::Probe(params)->GetViewType(&viewTpye);
    if (viewTpye != LayoutParams::EXPANDED && IView::Probe(mMenuView) != child) {
        child->SetVisibility(mExpandedActionView != NULL ? IView::GONE : IView::VISIBLE);
    }
}

} // namespace Widget
} // namespace Droid
} // namaspace Elastos