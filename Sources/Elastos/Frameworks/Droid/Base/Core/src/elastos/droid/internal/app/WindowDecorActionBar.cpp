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

#include "elastos/droid/internal/app/WindowDecorActionBar.h"
#include "elastos/droid/animation/CAnimatorSet.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/app/CActionBarLayoutParams.h"
#include "elastos/droid/internal/app/CNavItemSelectedListener.h"
#include "elastos/droid/internal/view/menu/CMenuBuilder.h"
#include "elastos/droid/internal/view/menu/CMenuPopupHelper.h"
#include "elastos/droid/internal/view/ActionBarPolicy.h"
#include "elastos/droid/internal/widget/CScrollingTabContainerView.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/view/CContextThemeWrapper.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/CMenuInflater.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::App::CActionBarLayoutParams;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::App::EIID_IActionBarTab;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Internal::View::ActionBarPolicy;
using Elastos::Droid::Internal::View::IActionBarPolicy;
using Elastos::Droid::Internal::View::Menu::CMenuBuilder;
using Elastos::Droid::Internal::View::Menu::CMenuPopupHelper;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::IMenuPopupHelper;
using Elastos::Droid::Internal::Widget::CScrollingTabContainerView;
using Elastos::Droid::Internal::Widget::EIID_IActionBarVisibilityCallback;
using Elastos::Droid::Internal::Widget::IAbsActionBarView;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::CContextThemeWrapper;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IContextThemeWrapper;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::CMenuInflater;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IToolbar;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(WindowDecorActionBar::ActionModeImpl::CallbackWrapper, Object, IMenuBuilderCallback)

WindowDecorActionBar::ActionModeImpl::CallbackWrapper::CallbackWrapper(
    /* [in] */ WindowDecorActionBar::ActionModeImpl* host)
    : mHost(host)
{}

ECode WindowDecorActionBar::ActionModeImpl::CallbackWrapper::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    return mHost->OnMenuItemSelected(menu, item, result);
}

ECode WindowDecorActionBar::ActionModeImpl::CallbackWrapper::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    return mHost->OnMenuModeChange(menu);
}

WindowDecorActionBar::ActionModeImpl::ActionModeImpl(
    /* [in] */ IActionModeCallback* callback,
    /* [in] */ WindowDecorActionBar* host)
    : mCallback(callback)
    , mHost(host)
{
    AutoPtr<IContext> context;
    mHost->GetThemedContext((IContext**)&context);
    CMenuBuilder::New(context, (IMenuBuilder**)&mMenu);
    mMenu->SetDefaultShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM);
    AutoPtr<CallbackWrapper> callbackWrapper = new CallbackWrapper(this);
    mMenu->SetCallback(callbackWrapper);
}

ECode WindowDecorActionBar::ActionModeImpl::GetMenuInflater(
    /* [out] */ IMenuInflater** menuInflater)
{
    AutoPtr<IContext> context;
    mHost->GetThemedContext((IContext**)&context);
    return CMenuInflater::New(context, menuInflater);
}

ECode WindowDecorActionBar::ActionModeImpl::GetMenu(
    /* [out] */ IMenu** menu)
{
    VALIDATE_NOT_NULL(menu)
    *menu = IMenu::Probe(mMenu);
    REFCOUNT_ADD(*menu)
    return NOERROR;
}

ECode WindowDecorActionBar::ActionModeImpl::Finish()
{
    if (mHost->mActionMode.Get() != this) {
        // Not the active action mode - no-op
        return NOERROR;
    }

    // If this change in state is going to cause the action bar
    // to be hidden, defer the onDestroy callback until the animation
    // is finished and associated relayout is about to happen. This lets
    // apps better anticipate visibility and layout behavior.
    if (!CheckShowingFlags(mHost->mHiddenByApp, mHost->mHiddenBySystem, FALSE)) {
        // With the current state but the action bar hidden, our
        // overall showing state is going to be FALSE.
        mHost->mDeferredDestroyActionMode = this;
        mHost->mDeferredModeDestroyCallback = mCallback;
    }
    else {
        mCallback->OnDestroyActionMode(this);
    }
    mCallback = NULL;
    mHost->AnimateToMode(FALSE);

    // Clear out the context mode views after the animation finishes
    mHost->mContextView->CloseMode();
    AutoPtr<IViewGroup> viewGroup;
    mHost->mDecorToolbar->GetViewGroup((IViewGroup**)&viewGroup);
    IView::Probe(viewGroup)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
    mHost->mOverlayLayout->SetHideOnContentScrollEnabled(mHost->mHideOnContentScroll);

    mHost->mActionMode = NULL;
    return NOERROR;
}

ECode WindowDecorActionBar::ActionModeImpl::Invalidate()
{
    mMenu->StopDispatchingItemsChanged();
    Boolean res;
    mCallback->OnPrepareActionMode(this, IMenu::Probe(mMenu), &res);
    mMenu->StartDispatchingItemsChanged();
    return NOERROR;
}

ECode WindowDecorActionBar::ActionModeImpl::DispatchOnCreate(
    /* [out] */ Boolean* result)
{
    mMenu->StopDispatchingItemsChanged();
    Boolean res;
    mCallback->OnCreateActionMode(this, IMenu::Probe(mMenu), &res);
    mMenu->StartDispatchingItemsChanged();
    return NOERROR;
}

ECode WindowDecorActionBar::ActionModeImpl::SetCustomView(
    /* [in] */ IView* view)
{
    mHost->mContextView->SetCustomView(view);
    mCustomView = NULL;
    IWeakReferenceSource::Probe(view)->GetWeakReference((IWeakReference**)&mCustomView);
    return NOERROR;
}

ECode WindowDecorActionBar::ActionModeImpl::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    return mHost->mContextView->SetSubtitle(subtitle);
}

ECode WindowDecorActionBar::ActionModeImpl::SetTitle(
    /* [in] */ ICharSequence* title)
{
    return mHost->mContextView->SetTitle(title);
}

ECode WindowDecorActionBar::ActionModeImpl::SetTitle(
    /* [in] */ Int32 resId)
{
    AutoPtr<IResources> resources;
    mHost->mContext->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> text;
    resources->GetText(resId, (ICharSequence**)&text);
    return SetTitle(text);
}

ECode WindowDecorActionBar::ActionModeImpl::SetSubtitle(
    /* [in] */ Int32 resId)
{
    AutoPtr<IResources> resources;
    mHost->mContext->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> text;
    resources->GetText(resId, (ICharSequence**)&text);
    return SetSubtitle(text);
}

ECode WindowDecorActionBar::ActionModeImpl::GetTitle(
    /* [out] */ ICharSequence** title)
{
    return mHost->mContextView->GetTitle(title);
}

ECode WindowDecorActionBar::ActionModeImpl::GetSubtitle(
    /* [out] */ ICharSequence** title)
{
    return mHost->mContextView->GetSubtitle(title);
}

ECode WindowDecorActionBar::ActionModeImpl::SetTitleOptionalHint(
    /* [in] */ Boolean titleOptional)
{
    ActionMode::SetTitleOptionalHint(titleOptional);
    return mHost->mContextView->SetTitleOptional(titleOptional);
}

ECode WindowDecorActionBar::ActionModeImpl::IsTitleOptional(
    /* [out] */ Boolean* result)
{
    return mHost->mContextView->IsTitleOptional(result);
}

ECode WindowDecorActionBar::ActionModeImpl::GetCustomView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = NULL;
    if (mCustomView != NULL) {
        mCustomView->Resolve(EIID_IView, (IInterface**)view);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::ActionModeImpl::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    if (mCallback != NULL) {
        return mCallback->OnActionItemClicked(this, item, result);
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
}

ECode WindowDecorActionBar::ActionModeImpl::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    return NOERROR;
}

ECode WindowDecorActionBar::ActionModeImpl::OnSubMenuSelected(
    /* [in] */ ISubMenuBuilder* subMenu,
    /* [out] */ Boolean* result)
{
    if (mCallback == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean hasVisibleItems;
    IMenu::Probe(subMenu)->HasVisibleItems(&hasVisibleItems);
    if (!hasVisibleItems) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IContext> context;
    mHost->GetThemedContext((IContext**)&context);
    AutoPtr<IMenuPopupHelper> helper;
    CMenuPopupHelper::New(context, IMenuBuilder::Probe(subMenu), (IMenuPopupHelper**)&helper);
    helper->Show();
    *result = TRUE;
    return NOERROR;
}

ECode WindowDecorActionBar::ActionModeImpl::OnCloseSubMenu(
    /* [in] */ ISubMenuBuilder* menu)
{
    return NOERROR;
}

ECode WindowDecorActionBar::ActionModeImpl::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    if (mCallback == NULL) {
        return NOERROR;
    }
    Invalidate();
    Boolean res;
    IAbsActionBarView::Probe(mHost->mContextView)->ShowOverflowMenu(&res);
    return NOERROR;
}

CAR_INTERFACE_IMPL(WindowDecorActionBar::TabImpl, Object, IActionBarTab)

WindowDecorActionBar::TabImpl::TabImpl(
    /* [in] */ WindowDecorActionBar* host)
    : mPosition(-1)
    , mHost(host)
{}

ECode WindowDecorActionBar::TabImpl::GetTag(
    /* [out] */ IInterface** tag)
{
    VALIDATE_NOT_NULL(tag)
    *tag = mTag;
    REFCOUNT_ADD(*tag)
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::SetTag(
    /* [in] */ IInterface* tag)
{
    mTag = tag;
    return NOERROR;
}

AutoPtr<IActionBarTabListener> WindowDecorActionBar::TabImpl::GetCallback()
{
    return mCallback;
}

ECode WindowDecorActionBar::TabImpl::SetTabListener(
    /* [in] */ IActionBarTabListener* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::GetCustomView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = mCustomView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::SetCustomView(
    /* [in] */ IView* view)
{
    mCustomView = view;
    if (mPosition >= 0) {
        mHost->mTabScrollView->UpdateTab(mPosition);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::SetCustomView(
    /* [in] */ Int32 layoutResId)
{
    AutoPtr<IContext> context;
    mHost->GetThemedContext((IContext**)&context);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(layoutResId, NULL, (IView**)&view);
    return SetCustomView(view);
}

ECode WindowDecorActionBar::TabImpl::GetIcon(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = mIcon;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::GetPosition(
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)
    *position = mPosition;
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::SetPosition(
    /* [in] */ Int32 position)
{
    mPosition = position;
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::GetText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text)
    *text = mText;
    REFCOUNT_ADD(*text)
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    if (mPosition >= 0) {
        mHost->mTabScrollView->UpdateTab(mPosition);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::SetIcon(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> drawable;
    mHost->mContext->GetDrawable(resId, (IDrawable**)&drawable);
    return SetIcon(drawable);
}

ECode WindowDecorActionBar::TabImpl::SetText(
    /* [in] */ ICharSequence* text)
{
    mText = text;
    if (mPosition >= 0) {
        mHost->mTabScrollView->UpdateTab(mPosition);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::SetText(
    /* [in] */ Int32 resId)
{
    AutoPtr<IResources> resources;
    mHost->mContext->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> text;
    resources->GetText(resId, (ICharSequence**)&text);
    return SetText(text);
}

ECode WindowDecorActionBar::TabImpl::Select()
{
    return mHost->SelectTab(this);
}

ECode WindowDecorActionBar::TabImpl::SetContentDescription(
    /* [in] */ Int32 resId)
{
    AutoPtr<IResources> resources;
    mHost->mContext->GetResources((IResources**)&resources);
    AutoPtr<ICharSequence> text;
    resources->GetText(resId, (ICharSequence**)&text);
    return SetContentDescription(text);
}

ECode WindowDecorActionBar::TabImpl::SetContentDescription(
    /* [in] */ ICharSequence* contentDesc)
{
    mContentDesc = contentDesc;
    if (mPosition >= 0) {
        mHost->mTabScrollView->UpdateTab(mPosition);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::TabImpl::GetContentDescription(
    /* [out] */ ICharSequence** contentDesc)
{
    VALIDATE_NOT_NULL(contentDesc)
    *contentDesc = mContentDesc;
    REFCOUNT_ADD(*contentDesc)
    return NOERROR;
}

WindowDecorActionBar::HideListener::HideListener(
    /* [in] */ WindowDecorActionBar* host)
    : mHost(host)
{}

ECode WindowDecorActionBar::HideListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mContentAnimations && mHost->mContentView != NULL) {
        mHost->mContentView->SetTranslationY(0);
        IView::Probe(mHost->mContainerView)->SetTranslationY(0);
    }
    if (mHost->mSplitView != NULL && mHost->mContextDisplayMode == CONTEXT_DISPLAY_SPLIT) {
        IView::Probe(mHost->mSplitView)->SetVisibility(IView::GONE);
    }
    IView::Probe(mHost->mContainerView)->SetVisibility(IView::GONE);
    mHost->mContainerView->SetTransitioning(FALSE);
    mHost->mCurrentShowAnim = NULL;
    mHost->CompleteDeferredDestroyActionMode();
    if (mHost->mOverlayLayout != NULL) {
        IView::Probe(mHost->mOverlayLayout)->RequestApplyInsets();
    }
    return NOERROR;
}

WindowDecorActionBar::ShowListener::ShowListener(
    /* [in] */ WindowDecorActionBar* host)
    : mHost(host)
{}

ECode WindowDecorActionBar::ShowListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCurrentShowAnim = NULL;
    IView::Probe(mHost->mContainerView)->RequestLayout();
    return NOERROR;
}

CAR_INTERFACE_IMPL(WindowDecorActionBar::UpdateListener, Object, IAnimatorUpdateListener)

WindowDecorActionBar::UpdateListener::UpdateListener(
    /* [in] */ WindowDecorActionBar* host)
    : mHost(host)
{}

ECode WindowDecorActionBar::UpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IViewParent> parent;
    IView::Probe(mHost->mContainerView)->GetParent((IViewParent**)&parent);
    return IView::Probe(parent)->Invalidate();
}

CAR_INTERFACE_IMPL(WindowDecorActionBar::ActionBarVisibilityCallback, Object, IActionBarVisibilityCallback)

WindowDecorActionBar::ActionBarVisibilityCallback::ActionBarVisibilityCallback(
    /* [in] */ WindowDecorActionBar* host)
    : mHost(host)
{}

ECode WindowDecorActionBar::ActionBarVisibilityCallback::OnWindowVisibilityChanged(
    /* [in] */  Int32 visibility)
{
    return mHost->OnWindowVisibilityChanged(visibility);
}

ECode WindowDecorActionBar::ActionBarVisibilityCallback::ShowForSystem()
{
    return mHost->ShowForSystem();
}

ECode WindowDecorActionBar::ActionBarVisibilityCallback::HideForSystem()
{
    return mHost->HideForSystem();
}

ECode WindowDecorActionBar::ActionBarVisibilityCallback::EnableContentAnimations(
    /* [in] */  Boolean enabled)
{
    return mHost->EnableContentAnimations(enabled);
}

ECode WindowDecorActionBar::ActionBarVisibilityCallback::OnContentScrollStarted()
{
    return mHost->OnContentScrollStarted();
}

ECode WindowDecorActionBar::ActionBarVisibilityCallback::OnContentScrollStopped()
{
    return mHost->OnContentScrollStopped();
}

const String WindowDecorActionBar::TAG("WindowDecorActionBar");
const Int32 WindowDecorActionBar::CONTEXT_DISPLAY_NORMAL = 0;
const Int32 WindowDecorActionBar::CONTEXT_DISPLAY_SPLIT = 1;
const Int32 WindowDecorActionBar::INVALID_POSITION = -1;

CAR_INTERFACE_IMPL(WindowDecorActionBar, ActionBar, IWindowDecorActionBar)

WindowDecorActionBar::WindowDecorActionBar()
    : mHideOnContentScroll(FALSE)
    , mContext(NULL)
    , mThemedContext(NULL)
    , mHoldThemedContext(FALSE)
    , mActivity(NULL)
    , mDialog(NULL)
    , mSavedTabPosition(INVALID_POSITION)
    , mDisplayHomeAsUpSet(FALSE)
    , mLastMenuVisibility(FALSE)
    , mContextDisplayMode(0)
    , mHasEmbeddedTabs(FALSE)
    , mCurWindowVisibility(IView::VISIBLE)
    , mContentAnimations(TRUE)
    , mHiddenByApp(FALSE)
    , mHiddenBySystem(FALSE)
    , mShowingForMode(FALSE)
    , mNowShowing(TRUE)
    , mShowHideAnimationEnabled(FALSE)
{
    CArrayList::New((IArrayList**)&mTabs);
    CArrayList::New((IArrayList**)&mMenuVisibilityListeners);
    mHideListener = new HideListener(this);
    mShowListener = new ShowListener(this);
    mUpdateListener = new UpdateListener(this);
}

WindowDecorActionBar::~WindowDecorActionBar()
{
    if (mHoldThemedContext && mThemedContext) {
        REFCOUNT_RELEASE(mThemedContext)
    }
}

ECode WindowDecorActionBar::constructor(
    /* [in] */ IActivity* activity)
{
    mActivity = activity;
    AutoPtr<IWindow> window;
    activity->GetWindow((IWindow**)&window);
    AutoPtr<IView> decor;
    window->GetDecorView((IView**)&decor);
    Boolean overlayMode;
    window->HasFeature(IWindow::FEATURE_ACTION_BAR_OVERLAY, &overlayMode);
    FAIL_RETURN(Init(decor))
    if (!overlayMode) {
        decor->FindViewById(R::id::content, (IView**)&mContentView);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::constructor(
    /* [in] */ IDialog* dialog)
{
    mDialog = dialog;
    AutoPtr<IWindow> window;
    dialog->GetWindow((IWindow**)&window);
    AutoPtr<IView> decor;
    window->GetDecorView((IView**)&decor);
    return Init(decor);
}

ECode WindowDecorActionBar::constructor(
    /* [in] */ IView* layout)
{
    Boolean UNUSED(isInEditMode) = FALSE;
    assert((layout->IsInEditMode(&isInEditMode), isInEditMode));
    return Init(layout);
}

ECode WindowDecorActionBar::Init(
    /* [in] */ IView* decor)
{
    AutoPtr<IView> view;
    decor->FindViewById(R::id::decor_content_parent, (IView**)&view);
    mOverlayLayout = IActionBarOverlayLayout::Probe(view);
    if (mOverlayLayout != NULL) {
        AutoPtr<IActionBarVisibilityCallback> cb = new ActionBarVisibilityCallback(this);
        mOverlayLayout->SetActionBarVisibilityCallback(cb);
    }
    view = NULL;
    decor->FindViewById(R::id::action_bar, (IView**)&view);
    FAIL_RETURN(GetDecorToolbar(view, (IDecorToolbar**)&mDecorToolbar))
    view = NULL;
    decor->FindViewById(R::id::action_context_bar, (IView**)&view);
    mContextView = IActionBarContextView::Probe(view);
    view = NULL;
    decor->FindViewById(R::id::action_bar_container, (IView**)&view);
    mContainerView = IActionBarContainer::Probe(view);
    view = NULL;
    decor->FindViewById(R::id::split_action_bar, (IView**)&view);
    mSplitView = IActionBarContainer::Probe(view);

    if (mDecorToolbar == NULL || mContextView == NULL || mContainerView == NULL) {
        Logger::E(TAG, "Can only be used with a compatible window decor layout");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IContext> ctx;
    mDecorToolbar->GetContext((IContext**)&ctx);
    mContext = ctx;

    Boolean isSplit;
    mDecorToolbar->IsSplit(&isSplit);
    mContextDisplayMode = isSplit ? CONTEXT_DISPLAY_SPLIT : CONTEXT_DISPLAY_NORMAL;

    // This was initially read from the action bar style
    Int32 current;
    mDecorToolbar->GetDisplayOptions(&current);
    Boolean homeAsUp = (current & DISPLAY_HOME_AS_UP) != 0;
    if (homeAsUp) {
        mDisplayHomeAsUpSet = TRUE;
    }

    AutoPtr<IActionBarPolicy> abp = ActionBarPolicy::Get(mContext);
    Boolean enableHomeButtonByDefault;
    abp->EnableHomeButtonByDefault(&enableHomeButtonByDefault);
    SetHomeButtonEnabled(enableHomeButtonByDefault || homeAsUp);
    Boolean hasEmbeddedTabs;
    abp->HasEmbeddedTabs(&hasEmbeddedTabs);
    SetHasEmbeddedTabs(hasEmbeddedTabs);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ActionBar);
    AutoPtr<ITypedArray> a;
    mContext->ObtainStyledAttributes(NULL, attrIds, R::attr::actionBarStyle, 0, (ITypedArray**)&a);
    Boolean value;
    if (a->GetBoolean(R::styleable::ActionBar_hideOnContentScroll, FALSE, &value), value) {
        SetHideOnContentScrollEnabled(TRUE);
    }
    Int32 elevation;
    a->GetDimensionPixelSize(R::styleable::ActionBar_elevation, 0, &elevation);
    if (elevation != 0) {
        SetElevation(elevation);
    }
    a->Recycle();
    return NOERROR;
}

ECode WindowDecorActionBar::GetDecorToolbar(
    /* [in] */ IView* view,
    /* [out] */ IDecorToolbar** decorToolbar)
{
    VALIDATE_NOT_NULL(decorToolbar)
    if (IDecorToolbar::Probe(view)) {
        *decorToolbar = IDecorToolbar::Probe(view);
        REFCOUNT_ADD(*decorToolbar)
        return NOERROR;
    }
    else if (IToolbar::Probe(view)) {
        return IToolbar::Probe(view)->GetWrapper(decorToolbar);
    }
    else {
        Logger::E(TAG, "Can't make a decor toolbar out of ");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
}

ECode WindowDecorActionBar::SetElevation(
    /* [in] */ Float elevation)
{
    IView::Probe(mContainerView)->SetElevation(elevation);
    if (IView::Probe(mSplitView) != NULL) {
        IView::Probe(mSplitView)->SetElevation(elevation);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::GetElevation(
    /* [out] */ Float* elevation)
{
    return IView::Probe(mContainerView)->GetElevation(elevation);
}

ECode WindowDecorActionBar::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    Boolean hasEmbeddedTabs;
    ActionBarPolicy::Get(mContext)->HasEmbeddedTabs(&hasEmbeddedTabs);
    SetHasEmbeddedTabs(hasEmbeddedTabs);
    return NOERROR;
}

void WindowDecorActionBar::SetHasEmbeddedTabs(
    /* [in] */ Boolean hasEmbeddedTabs)
{
    mHasEmbeddedTabs = hasEmbeddedTabs;
    // Switch tab layout configuration if needed
    if (!mHasEmbeddedTabs) {
        mDecorToolbar->SetEmbeddedTabView(NULL);
        mContainerView->SetTabContainer(mTabScrollView);
    }
    else {
        mContainerView->SetTabContainer(NULL);
        mDecorToolbar->SetEmbeddedTabView(mTabScrollView);
    }
    Int32 mode;
    GetNavigationMode(&mode);
    Boolean isInTabMode = mode == NAVIGATION_MODE_TABS;
    if (mTabScrollView != NULL) {
        if (isInTabMode) {
            IView::Probe(mTabScrollView)->SetVisibility(IView::VISIBLE);
            if (mOverlayLayout != NULL) {
                IView::Probe(mOverlayLayout)->RequestApplyInsets();
            }
        }
        else {
            IView::Probe(mTabScrollView)->SetVisibility(IView::GONE);
        }
    }
    mDecorToolbar->SetCollapsible(!mHasEmbeddedTabs && isInTabMode);
    mOverlayLayout->SetHasNonEmbeddedTabs(!mHasEmbeddedTabs && isInTabMode);
}

void WindowDecorActionBar::EnsureTabsExist()
{
    if (mTabScrollView != NULL) {
        return;
    }

    AutoPtr<IScrollingTabContainerView> tabScroller;
    CScrollingTabContainerView::New(mContext, (IScrollingTabContainerView**)&tabScroller);

    if (mHasEmbeddedTabs) {
        IView::Probe(tabScroller)->SetVisibility(IView::VISIBLE);
        mDecorToolbar->SetEmbeddedTabView(tabScroller);
    }
    else {
        Int32 mode;
        GetNavigationMode(&mode);
        if (mode == NAVIGATION_MODE_TABS) {
            IView::Probe(tabScroller)->SetVisibility(IView::VISIBLE);
            if (mOverlayLayout != NULL) {
                IView::Probe(mOverlayLayout)->RequestApplyInsets();
            }
        }
        else {
            IView::Probe(tabScroller)->SetVisibility(IView::GONE);
        }
        mContainerView->SetTabContainer(tabScroller);
    }
    mTabScrollView = tabScroller;
}

void WindowDecorActionBar::CompleteDeferredDestroyActionMode()
{
    if (mDeferredModeDestroyCallback != NULL) {
        mDeferredModeDestroyCallback->OnDestroyActionMode(mDeferredDestroyActionMode);
        mDeferredDestroyActionMode = NULL;
        mDeferredModeDestroyCallback = NULL;
    }
}

ECode WindowDecorActionBar::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    mCurWindowVisibility = visibility;
    return NOERROR;
}

ECode WindowDecorActionBar::SetShowHideAnimationEnabled(
    /* [in] */ Boolean enabled)
{
    mShowHideAnimationEnabled = enabled;
    if (!enabled && mCurrentShowAnim != NULL) {
        mCurrentShowAnim->End();
    }
    return NOERROR;
}

ECode WindowDecorActionBar::AddOnMenuVisibilityListener(
    /* [in] */ IActionBarOnMenuVisibilityListener* listener)
{
    mMenuVisibilityListeners->Add(listener);
    return NOERROR;
}

ECode WindowDecorActionBar::RemoveOnMenuVisibilityListener(
    /* [in] */ IActionBarOnMenuVisibilityListener* listener)
{
    mMenuVisibilityListeners->Remove(listener);
    return NOERROR;
}

ECode WindowDecorActionBar::DispatchMenuVisibilityChanged(
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

ECode WindowDecorActionBar::SetCustomView(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    GetThemedContext((IContext**)&context);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IViewGroup> viewGroup;
    mDecorToolbar->GetViewGroup((IViewGroup**)&viewGroup);
    AutoPtr<IView> view;
    inflater->Inflate(resId, viewGroup, FALSE, (IView**)&view);
    return SetCustomView(view);
}

ECode WindowDecorActionBar::SetDisplayUseLogoEnabled(
    /* [in] */ Boolean useLogo)
{
    return SetDisplayOptions(useLogo ? DISPLAY_USE_LOGO : 0, DISPLAY_USE_LOGO);
}

ECode WindowDecorActionBar::SetDisplayShowHomeEnabled(
    /* [in] */ Boolean showHome)
{
    return SetDisplayOptions(showHome ? DISPLAY_SHOW_HOME : 0, DISPLAY_SHOW_HOME);
}

ECode WindowDecorActionBar::SetDisplayHomeAsUpEnabled(
    /* [in] */ Boolean showHomeAsUp)
{
    return SetDisplayOptions(showHomeAsUp ? DISPLAY_HOME_AS_UP : 0, DISPLAY_HOME_AS_UP);
}

ECode WindowDecorActionBar::SetDisplayShowTitleEnabled(
    /* [in] */ Boolean showTitle)
{
    return SetDisplayOptions(showTitle ? DISPLAY_SHOW_TITLE : 0, DISPLAY_SHOW_TITLE);
}

ECode WindowDecorActionBar::SetDisplayShowCustomEnabled(
    /* [in] */ Boolean showCustom)
{
    return SetDisplayOptions(showCustom ? DISPLAY_SHOW_CUSTOM : 0, DISPLAY_SHOW_CUSTOM);
}

ECode WindowDecorActionBar::SetHomeButtonEnabled(
    /* [in] */ Boolean enable)
{
    return mDecorToolbar->SetHomeButtonEnabled(enable);
}

ECode WindowDecorActionBar::SetTitle(
    /* [in] */ Int32 resId)
{
    AutoPtr<ICharSequence> text;
    mContext->GetText(resId, (ICharSequence**)&text);
    return SetTitle(text);
}

ECode WindowDecorActionBar::SetSubtitle(
    /* [in] */ Int32 resId)
{
    AutoPtr<ICharSequence> text;
    mContext->GetText(resId, (ICharSequence**)&text);
    return SetSubtitle(text);
}

ECode WindowDecorActionBar::SetSelectedNavigationItem(
    /* [in] */ Int32 position)
{
    Int32 mode;
    mDecorToolbar->GetNavigationMode(&mode);
    switch (mode) {
    case NAVIGATION_MODE_TABS:
        {
            AutoPtr<IInterface> item;
            mTabs->Get(position, (IInterface**)&item);
            SelectTab(IActionBarTab::Probe(item));
            break;
        }
    case NAVIGATION_MODE_LIST:
        mDecorToolbar->SetDropdownSelectedPosition(position);
        break;
    default:
        Logger::E(TAG, "setSelectedNavigationIndex not valid for current navigation mode");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode WindowDecorActionBar::RemoveAllTabs()
{
    return CleanupTabs();
}

ECode WindowDecorActionBar::CleanupTabs()
{
    if (mSelectedTab != NULL) {
        SelectTab(NULL);
    }
    mTabs->Clear();
    if (mTabScrollView != NULL) {
        mTabScrollView->RemoveAllTabs();
    }
    mSavedTabPosition = INVALID_POSITION;
    return NOERROR;
}

ECode WindowDecorActionBar::SetTitle(
    /* [in] */ ICharSequence* title)
{
    return mDecorToolbar->SetTitle(title);
}

ECode WindowDecorActionBar::SetWindowTitle(
    /* [in] */ ICharSequence* title)
{
    return mDecorToolbar->SetWindowTitle(title);
}

ECode WindowDecorActionBar::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    return mDecorToolbar->SetSubtitle(subtitle);
}

ECode WindowDecorActionBar::SetDisplayOptions(
    /* [in] */ Int32 options)
{
    if ((options & DISPLAY_HOME_AS_UP) != 0) {
        mDisplayHomeAsUpSet = TRUE;
    }
    return mDecorToolbar->SetDisplayOptions(options);
}

ECode WindowDecorActionBar::SetDisplayOptions(
    /* [in] */ Int32 options,
    /* [in] */ Int32 mask)
{
    Int32 current;
    mDecorToolbar->GetDisplayOptions(&current);
    if ((mask & DISPLAY_HOME_AS_UP) != 0) {
        mDisplayHomeAsUpSet = TRUE;
    }
    return mDecorToolbar->SetDisplayOptions((options & mask) | (current & ~mask));
}

ECode WindowDecorActionBar::SetBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    return mContainerView->SetPrimaryBackground(d);
}

ECode WindowDecorActionBar::SetStackedBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    return mContainerView->SetStackedBackground(d);
}

ECode WindowDecorActionBar::SetSplitBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    if (mSplitView != NULL) {
        mSplitView->SetSplitBackground(d);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::GetCustomView(
    /* [out] */ IView** view)
{
    return mDecorToolbar->GetCustomView(view);
}

ECode WindowDecorActionBar::GetTitle(
    /* [out] */ ICharSequence** cs)
{
    return mDecorToolbar->GetTitle(cs);
}

ECode WindowDecorActionBar::GetSubtitle(
    /* [out] */ ICharSequence** cs)
{
    return mDecorToolbar->GetSubtitle(cs);
}

ECode WindowDecorActionBar::GetNavigationMode(
    /* [out] */ Int32* result)
{
    return mDecorToolbar->GetNavigationMode(result);
}

ECode WindowDecorActionBar::GetDisplayOptions(
    /* [out] */ Int32* result)
{
    return mDecorToolbar->GetDisplayOptions(result);
}

ECode WindowDecorActionBar::StartActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** actionMode)
{
    VALIDATE_NOT_NULL(actionMode)
    if (mActionMode != NULL) {
        mActionMode->Finish();
    }

    mOverlayLayout->SetHideOnContentScrollEnabled(FALSE);
    mContextView->KillMode();
    AutoPtr<ActionModeImpl> mode = new ActionModeImpl(callback, this);
    Boolean result;
    mode->DispatchOnCreate(&result);
    if (result) {
        mode->Invalidate();
        mContextView->InitForMode(mode);
        AnimateToMode(TRUE);
        if (mSplitView != NULL && mContextDisplayMode == CONTEXT_DISPLAY_SPLIT) {
            // TODO animate this
            Int32 res;
            if (IView::Probe(mSplitView)->GetVisibility(&res), res != IView::VISIBLE) {
                IView::Probe(mSplitView)->SetVisibility(IView::VISIBLE);
                if (mOverlayLayout != NULL) {
                    IView::Probe(mOverlayLayout)->RequestApplyInsets();
                }
            }
        }
        IView::Probe(mContextView)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
        mActionMode = mode;
        *actionMode = mode;
        REFCOUNT_ADD(*actionMode)
        return NOERROR;
    }
    *actionMode = NULL;
    return NOERROR;
}

ECode WindowDecorActionBar::ConfigureTab(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Int32 position)
{
    TabImpl* tabi = (TabImpl*)tab;
    AutoPtr<IActionBarTabListener> callback = tabi->GetCallback();

    if (callback == NULL) {
        Logger::E(TAG, "Action Bar Tab must have a Callback");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    tabi->SetPosition(position);
    mTabs->Add(position, tab);

    Int32 count;
    mTabs->GetSize(&count);
    for (Int32 i = position + 1; i < count; i++) {
        AutoPtr<IInterface> item;
        mTabs->Get(i, (IInterface**)&item);
        ((TabImpl*)IActionBarTab::Probe(item))->SetPosition(i);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::AddTab(
    /* [in] */ IActionBarTab* tab)
{
    Boolean isEmpty;
    mTabs->IsEmpty(&isEmpty);
    return AddTab(tab, isEmpty);
}

ECode WindowDecorActionBar::AddTab(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Int32 position)
{
    Boolean isEmpty;
    mTabs->IsEmpty(&isEmpty);
    return AddTab(tab, position, isEmpty);
}

ECode WindowDecorActionBar::AddTab(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Boolean setSelected)
{
    EnsureTabsExist();
    mTabScrollView->AddTab(tab, setSelected);
    Int32 size;
    mTabs->GetSize(&size);
    FAIL_RETURN(ConfigureTab(tab, size));
    if (setSelected) {
        SelectTab(tab);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::AddTab(
    /* [in] */ IActionBarTab* tab,
    /* [in] */ Int32 position,
    /* [in] */ Boolean setSelected)
{
    EnsureTabsExist();
    mTabScrollView->AddTab(tab, position, setSelected);
    FAIL_RETURN(ConfigureTab(tab, position));
    if (setSelected) {
        SelectTab(tab);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::NewTab(
    /* [out] */ IActionBarTab** tab)
{
    VALIDATE_NOT_NULL(tab)
    *tab = new TabImpl(this);
    REFCOUNT_ADD(*tab)
    return NOERROR;
}

ECode WindowDecorActionBar::RemoveTab(
    /* [in] */ IActionBarTab* tab)
{
    Int32 position;
    tab->GetPosition(&position);
    return RemoveTabAt(position);
}

ECode WindowDecorActionBar::RemoveTabAt(
    /* [in] */ Int32 position)
{
    if (mTabScrollView == NULL) {
        // No tabs around to remove
        return NOERROR;
    }

    Int32 pos;
    Int32 selectedTabPosition = mSelectedTab != NULL
        ? (mSelectedTab->GetPosition(&pos), pos) : mSavedTabPosition;
    mTabScrollView->RemoveTabAt(position);
    AutoPtr<IInterface> oldValue;
    mTabs->Remove(position, (IInterface**)&oldValue);
    TabImpl* removedTab = (TabImpl*)IActionBarTab::Probe(oldValue);
    if (removedTab != NULL) {
        removedTab->SetPosition(-1);
    }

    Int32 newTabCount;
    mTabs->GetSize(&newTabCount);
    for (Int32 i = position; i < newTabCount; i++) {
        AutoPtr<IInterface> item;
        mTabs->Get(i, (IInterface**)&item);
        ((TabImpl*)IActionBarTab::Probe(item))->SetPosition(i);
    }

    if (selectedTabPosition == position) {
        AutoPtr<IActionBarTab> tab;
        Boolean isEmpty;
        mTabs->IsEmpty(&isEmpty);
        if (!isEmpty) {
            AutoPtr<IInterface> item;
            mTabs->Get(Elastos::Core::Math::Max(0, position - 1), (IInterface**)&item);
            tab = IActionBarTab::Probe(item);
        }
        SelectTab(tab);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::SelectTab(
    /* [in] */ IActionBarTab* tab)
{
    Int32 mode;
    GetNavigationMode(&mode);
    if (mode != NAVIGATION_MODE_TABS) {
        Int32 position;
        mSavedTabPosition = tab != NULL ? (tab->GetPosition(&position), position) : INVALID_POSITION;
        return NOERROR;
    }

    AutoPtr<IViewGroup> viewGroup;
    mDecorToolbar->GetViewGroup((IViewGroup**)&viewGroup);
    Boolean isInEditMode;
    IView::Probe(viewGroup)->IsInEditMode(&isInEditMode);
    AutoPtr<IFragmentTransaction> trans;
    if (isInEditMode) {
        AutoPtr<IFragmentManager> fm;
        mActivity->GetFragmentManager((IFragmentManager**)&fm);
        fm->BeginTransaction((IFragmentTransaction**)&trans);
        trans->DisallowAddToBackStack();
    }

    if (((IActionBarTab*)mSelectedTab) == tab) {
        if (mSelectedTab != NULL) {
            mSelectedTab->GetCallback()->OnTabReselected(mSelectedTab, trans);
            Int32 position;
            tab->GetPosition(&position);
            mTabScrollView->AnimateToTab(position);
        }
    }
    else {
        Int32 position;
        mTabScrollView->SetTabSelected(tab != NULL ? (tab->GetPosition(&position), position)
            : IActionBarTab::INVALID_POSITION);
        if (mSelectedTab != NULL) {
            mSelectedTab->GetCallback()->OnTabUnselected(mSelectedTab, trans);
        }
        mSelectedTab = (TabImpl*)tab;
        if (mSelectedTab != NULL) {
            mSelectedTab->GetCallback()->OnTabSelected(mSelectedTab, trans);
        }
    }

    Boolean isEmpty;
    if (trans != NULL && !(trans->IsEmpty(&isEmpty), isEmpty)) {
        Int32 res;
        trans->Commit(&res);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::GetSelectedTab(
    /* [out] */ IActionBarTab** tab)
{
    VALIDATE_NOT_NULL(tab)
    *tab = mSelectedTab;
    REFCOUNT_ADD(*tab)
    return NOERROR;
}

ECode WindowDecorActionBar::GetHeight(
    /* [out] */ Int32* height)
{
    return IView::Probe(mContainerView)->GetHeight(height);
}

ECode WindowDecorActionBar::EnableContentAnimations(
    /* [in] */ Boolean enabled)
{
    mContentAnimations = enabled;
    return NOERROR;
}

ECode WindowDecorActionBar::Show()
{
    if (mHiddenByApp) {
        mHiddenByApp = FALSE;
        UpdateVisibility(FALSE);
    }
    return NOERROR;
}

void WindowDecorActionBar::ShowForActionMode()
{
    if (!mShowingForMode) {
        mShowingForMode = TRUE;
        if (mOverlayLayout != NULL) {
            mOverlayLayout->SetShowingForActionMode(TRUE);
        }
        UpdateVisibility(FALSE);
    }
}

ECode WindowDecorActionBar::ShowForSystem()
{
    if (mHiddenBySystem) {
        mHiddenBySystem = FALSE;
        UpdateVisibility(TRUE);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::Hide()
{
    if (!mHiddenByApp) {
        mHiddenByApp = TRUE;
        UpdateVisibility(FALSE);
    }
    return NOERROR;
}

void WindowDecorActionBar::HideForActionMode()
{
    if (mShowingForMode) {
        mShowingForMode = FALSE;
        if (mOverlayLayout != NULL) {
            mOverlayLayout->SetShowingForActionMode(FALSE);
        }
        UpdateVisibility(FALSE);
    }
    return;
}

ECode WindowDecorActionBar::HideForSystem()
{
    if (!mHiddenBySystem) {
        mHiddenBySystem = TRUE;
        UpdateVisibility(TRUE);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::SetHideOnContentScrollEnabled(
    /* [in] */ Boolean hideOnContentScroll)
{
    Boolean res;
    if (hideOnContentScroll && !(mOverlayLayout->IsInOverlayMode(&res), res)) {
        Logger::E(TAG, "Action bar must be in overlay mode "
            "(IWindow::FEATURE_OVERLAY_ACTION_BAR) to enable hide on content scroll");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mHideOnContentScroll = hideOnContentScroll;
    return mOverlayLayout->SetHideOnContentScrollEnabled(hideOnContentScroll);
}

ECode WindowDecorActionBar::IsHideOnContentScrollEnabled(
    /* [out] */ Boolean* result)
{
    return mOverlayLayout->IsHideOnContentScrollEnabled(result);
}

ECode WindowDecorActionBar::GetHideOffset(
    /* [out] */ Int32* result)
{
    return mOverlayLayout->GetActionBarHideOffset(result);
}

ECode WindowDecorActionBar::SetHideOffset(
    /* [in] */ Int32 offset)
{
    Boolean res;
    if (offset != 0 && !(mOverlayLayout->IsInOverlayMode(&res), res)) {
        Logger::E(TAG, "Action bar must be in overlay mode "
            "(IWindow::FEATURE_OVERLAY_ACTION_BAR) to set a non-zero hide offset");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return mOverlayLayout->SetActionBarHideOffset(offset);
}

Boolean WindowDecorActionBar::CheckShowingFlags(
    /* [in] */ Boolean hiddenByApp,
    /* [in] */ Boolean hiddenBySystem,
    /* [in] */ Boolean showingForMode)
{
    if (showingForMode) {
        return TRUE;
    }
    else if (hiddenByApp || hiddenBySystem) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

void WindowDecorActionBar::UpdateVisibility(
    /* [in] */ Boolean fromSystem)
{
    // Based on the current state, should we be hidden or shown?
    Boolean shown = CheckShowingFlags(mHiddenByApp, mHiddenBySystem, mShowingForMode);

    if (shown) {
        if (!mNowShowing) {
            mNowShowing = TRUE;
            DoShow(fromSystem);
        }
    }
    else {
        if (mNowShowing) {
            mNowShowing = FALSE;
            DoHide(fromSystem);
        }
    }
}

ECode WindowDecorActionBar::DoShow(
    /* [in] */ Boolean fromSystem)
{
    if (mCurrentShowAnim != NULL) {
        mCurrentShowAnim->End();
    }
    IView* containerView = IView::Probe(mContainerView);
    containerView->SetVisibility(IView::VISIBLE);
    IView* splitView = IView::Probe(mSplitView);

    if (mCurWindowVisibility == IView::VISIBLE && (mShowHideAnimationEnabled || fromSystem)) {
        containerView->SetTranslationY(0); // because we're about to ask its window loc
        Int32 h;
        containerView->GetHeight(&h);
        Float startingY = -h;
        if (fromSystem) {
            AutoPtr<ArrayOf<Int32> > topLeft = ArrayOf<Int32>::Alloc(2);
            (*topLeft)[0] = 0;
            (*topLeft)[1] = 0;
            containerView->GetLocationInWindow(topLeft);
            startingY -= (*topLeft)[1];
        }
        containerView->SetTranslationY(startingY);
        AutoPtr<IAnimator> anim;
        CAnimatorSet::New((IAnimator**)&anim);
        AutoPtr<IValueAnimator> a = IValueAnimator::Probe(ObjectAnimator::OfFloat(
            containerView, Elastos::Droid::View::View::TRANSLATION_Y, 0));
        a->AddUpdateListener(mUpdateListener);
        AutoPtr<IAnimatorSetBuilder> b;
        IAnimatorSet::Probe(anim)->Play(IAnimator::Probe(a), (IAnimatorSetBuilder**)&b);
        if (mContentAnimations && mContentView != NULL) {
            AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
            (*values)[0] = startingY;
            (*values)[1] = 0;
            b->With(IAnimator::Probe(ObjectAnimator::OfFloat(
                mContentView, Elastos::Droid::View::View::TRANSLATION_Y, values)));
        }
        if (mSplitView != NULL && mContextDisplayMode == CONTEXT_DISPLAY_SPLIT) {
            splitView->GetHeight(&h);
            splitView->SetTranslationY(h);
            splitView->SetVisibility(IView::VISIBLE);
            AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
            (*values)[0] = startingY;
            b->With(IAnimator::Probe(ObjectAnimator::OfFloat(
                mSplitView, Elastos::Droid::View::View::TRANSLATION_Y, values)));
        }
        AutoPtr<Elastos::Droid::View::Animation::IInterpolator> interpolator;
        AnimationUtils::LoadInterpolator(mContext, R::interpolator::decelerate_cubic,
                (Elastos::Droid::View::Animation::IInterpolator**)&interpolator);
        anim->SetInterpolator(ITimeInterpolator::Probe(interpolator));
        anim->SetDuration(250);
        // If this is being shown from the system, add a small delay.
        // This is because we will also be animating in the status bar,
        // and these two elements can't be done in lock-step.  So we give
        // a little time for the status bar to start its animation before
        // the action bar animates.  (This corresponds to the corresponding
        // case when hiding, where the status bar has a small delay before
        // starting.)
        anim->AddListener(mShowListener);
        mCurrentShowAnim = anim;
        anim->Start();
    }
    else {
        containerView->SetAlpha(1);
        containerView->SetTranslationY(0);
        if (mContentAnimations && mContentView != NULL) {
            mContentView->SetTranslationY(0);
        }
        if (mSplitView != NULL && mContextDisplayMode == CONTEXT_DISPLAY_SPLIT) {
            splitView->SetAlpha(1);
            splitView->SetTranslationY(0);
            splitView->SetVisibility(IView::VISIBLE);
        }
        mShowListener->OnAnimationEnd(NULL);
    }
    if (mOverlayLayout != NULL) {
        IView::Probe(mOverlayLayout)->RequestApplyInsets();
    }
    return NOERROR;
}

ECode WindowDecorActionBar::DoHide(
    /* [in] */ Boolean fromSystem)
{
    if (mCurrentShowAnim != NULL) {
        mCurrentShowAnim->End();
    }

    IView* containerView = IView::Probe(mContainerView);
    if (mCurWindowVisibility == IView::VISIBLE && (mShowHideAnimationEnabled || fromSystem)) {
        containerView->SetAlpha(1);
        mContainerView->SetTransitioning(TRUE);

        AutoPtr<IAnimator> anim;
        CAnimatorSet::New((IAnimator**)&anim);
        Int32 h;
        containerView->GetHeight(&h);
        Float endingY = -h;
        if (fromSystem) {
            AutoPtr<ArrayOf<Int32> > topLeft = ArrayOf<Int32>::Alloc(2);
            (*topLeft)[0] = 0;
            (*topLeft)[1] = 0;
            containerView->GetLocationInWindow(topLeft);
            endingY -= (*topLeft)[1];
        }

        AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(1);
        (*values)[0] = endingY;
        AutoPtr<IValueAnimator> a = IValueAnimator::Probe(ObjectAnimator::OfFloat(
            containerView, Elastos::Droid::View::View::TRANSLATION_Y, values));
        a->AddUpdateListener(mUpdateListener);
        AutoPtr<IAnimatorSetBuilder> b;
        IAnimatorSet::Probe(anim)->Play(IAnimator::Probe(a), (IAnimatorSetBuilder**)&b);
        IView* splitView = IView::Probe(mSplitView);
        Int32 res;
        if (mSplitView != NULL && (splitView->GetVisibility(&res),
            res == IView::VISIBLE)) {
            splitView->SetAlpha(1);
            splitView->GetHeight(&h);
            (*values)[0] = h;
            b->With(IAnimator::Probe(ObjectAnimator::OfFloat(
                mSplitView, Elastos::Droid::View::View::TRANSLATION_Y, values)));
        }
        AutoPtr<Elastos::Droid::View::Animation::IInterpolator> interpolator;
        AnimationUtils::LoadInterpolator(mContext, R::interpolator::accelerate_cubic,
                (Elastos::Droid::View::Animation::IInterpolator**)&interpolator);
        anim->SetInterpolator(ITimeInterpolator::Probe(interpolator));
        anim->SetDuration(250);
        anim->AddListener(mHideListener);
        mCurrentShowAnim = anim;
        anim->Start();
    }
    else {
        mHideListener->OnAnimationEnd(NULL);
    }
    return NOERROR;
}

ECode WindowDecorActionBar::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 height;
    GetHeight(&height);
    // Take into account the case where the bar has a 0 height due to not being measured yet.
    Int32 offset;
    *result = mNowShowing && (height == 0 || (GetHideOffset(&offset), offset) < height);
    return NOERROR;
}

void WindowDecorActionBar::AnimateToMode(
    /* [in] */ Boolean toActionMode)
{
    if (toActionMode) {
        ShowForActionMode();
    }
    else {
        HideForActionMode();
    }

    mDecorToolbar->AnimateToVisibility(toActionMode ? IView::GONE : IView::VISIBLE);
    IAbsActionBarView::Probe(mContextView)->AnimateToVisibility(toActionMode ? IView::VISIBLE : IView::GONE);
    // mTabScrollView's visibility is not affected by action mode.
}

ECode WindowDecorActionBar::GetThemedContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    if (mThemedContext == NULL) {
        AutoPtr<ITypedValue> outValue;
        CTypedValue::New((ITypedValue**)&outValue);
        AutoPtr<IResourcesTheme> currentTheme;
        mContext->GetTheme((IResourcesTheme**)&currentTheme);
        Boolean res;
        currentTheme->ResolveAttribute(R::attr::actionBarWidgetTheme, outValue, TRUE, &res);
        Int32 targetThemeRes;
        outValue->GetResourceId(&targetThemeRes);

        Int32 resId;
        if (targetThemeRes != 0 && (mContext->GetThemeResId(&resId), resId != targetThemeRes)) {
            CContextThemeWrapper::New(mContext, targetThemeRes, FALSE/* do not hold */, (IContext**)&mThemedContext);
            mHoldThemedContext = TRUE;
        }
        else {
            mThemedContext = mContext;
            mHoldThemedContext = FALSE;
        }
    }
    *context = mThemedContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode WindowDecorActionBar::IsTitleTruncated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean res;
    *result = mDecorToolbar != NULL && (mDecorToolbar->IsTitleTruncated(&res), res);
    return NOERROR;
}

ECode WindowDecorActionBar::SetHomeAsUpIndicator(
    /* [in] */ IDrawable* indicator)
{
    return mDecorToolbar->SetNavigationIcon(indicator);
}

ECode WindowDecorActionBar::SetHomeAsUpIndicator(
    /* [in] */ Int32 resId)
{
    return mDecorToolbar->SetNavigationIcon(resId);
}

ECode WindowDecorActionBar::SetHomeActionContentDescription(
    /* [in] */ ICharSequence* description)
{
    return mDecorToolbar->SetNavigationContentDescription(description);
}

ECode WindowDecorActionBar::SetHomeActionContentDescription(
    /* [in] */ Int32 resId)
{
    return mDecorToolbar->SetNavigationContentDescription(resId);
}

ECode WindowDecorActionBar::OnContentScrollStarted()
{
    if (mCurrentShowAnim != NULL) {
        mCurrentShowAnim->Cancel();
        mCurrentShowAnim = NULL;
    }
    return NOERROR;
}

ECode WindowDecorActionBar::OnContentScrollStopped()
{
    return NOERROR;
}

ECode WindowDecorActionBar::CollapseActionView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean res;
    if (mDecorToolbar != NULL && (mDecorToolbar->HasExpandedActionView(&res), res)) {
        mDecorToolbar->CollapseActionView();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode WindowDecorActionBar::SetCustomView(
    /* [in] */ IView* view)
{
    return mDecorToolbar->SetCustomView(view);
}

ECode WindowDecorActionBar::SetCustomView(
    /* [in] */ IView* view,
    /* [in] */ IActionBarLayoutParams* layoutParams)
{
    view->SetLayoutParams(IViewGroupLayoutParams::Probe(layoutParams));
    return mDecorToolbar->SetCustomView(view);
}

ECode WindowDecorActionBar::SetListNavigationCallbacks(
    /* [in] */ ISpinnerAdapter* adapter,
    /* [in] */ IActionBarOnNavigationListener* callback)
{
    AutoPtr<IAdapterViewOnItemSelectedListener> listener;
    CNavItemSelectedListener::New(callback, (IAdapterViewOnItemSelectedListener**)&listener);
    return mDecorToolbar->SetDropdownParams(adapter, listener);
}

ECode WindowDecorActionBar::GetSelectedNavigationIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    Int32 mode;
    mDecorToolbar->GetNavigationMode(&mode);
    switch (mode) {
        case NAVIGATION_MODE_TABS:
            *index = -1;
            if (mSelectedTab != NULL) {
                mSelectedTab->GetPosition(index);
            }
            return NOERROR;
        case NAVIGATION_MODE_LIST:
            return mDecorToolbar->GetDropdownSelectedPosition(index);
        default:
            *index = -1;
            return NOERROR;
    }
}

ECode WindowDecorActionBar::GetNavigationItemCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 mode;
    mDecorToolbar->GetNavigationMode(&mode);
    switch (mode) {
        case NAVIGATION_MODE_TABS:
            return mTabs->GetSize(count);
        case NAVIGATION_MODE_LIST:
            return mDecorToolbar->GetDropdownItemCount(count);
        default:
            *count = 0;
            return NOERROR;
    }
}

ECode WindowDecorActionBar::GetTabCount(
    /* [out] */ Int32* count)
{
    return mTabs->GetSize(count);
}

ECode WindowDecorActionBar::SetNavigationMode(
    /* [in] */ Int32 mode)
{
    Int32 oldMode;
    mDecorToolbar->GetNavigationMode(&oldMode);
    switch (oldMode) {
        case NAVIGATION_MODE_TABS:
            GetSelectedNavigationIndex(&mSavedTabPosition);
            SelectTab(NULL);
            IView::Probe(mTabScrollView)->SetVisibility(IView::GONE);
            break;
    }
    if (oldMode != mode && !mHasEmbeddedTabs) {
        if (mOverlayLayout != NULL) {
            IView::Probe(mOverlayLayout)->RequestFitSystemWindows();
        }
    }
    mDecorToolbar->SetNavigationMode(mode);
    switch (mode) {
        case NAVIGATION_MODE_TABS:
            EnsureTabsExist();
            IView::Probe(mTabScrollView)->SetVisibility(IView::VISIBLE);
            if (mSavedTabPosition != INVALID_POSITION) {
                SetSelectedNavigationItem(mSavedTabPosition);
                mSavedTabPosition = INVALID_POSITION;
            }
            break;
    }
    mDecorToolbar->SetCollapsible(mode == NAVIGATION_MODE_TABS && !mHasEmbeddedTabs);
    mOverlayLayout->SetHasNonEmbeddedTabs(mode == NAVIGATION_MODE_TABS && !mHasEmbeddedTabs);
    return NOERROR;
}

ECode WindowDecorActionBar::GetTabAt(
    /* [in] */ Int32 index,
    /* [out] */ IActionBarTab** tab)
{
    VALIDATE_NOT_NULL(tab)
    AutoPtr<IInterface> item;
    mTabs->Get(index, (IInterface**)&item);
    *tab = IActionBarTab::Probe(item);
    REFCOUNT_ADD(*tab)
    return NOERROR;
}

ECode WindowDecorActionBar::SetIcon(
    /* [in] */ Int32 resId)
{
    return mDecorToolbar->SetIcon(resId);
}

ECode WindowDecorActionBar::SetIcon(
    /* [in] */ IDrawable* icon)
{
    return mDecorToolbar->SetIcon(icon);
}

ECode WindowDecorActionBar::HasIcon(
    /* [out] */ Boolean* result)
{
    return mDecorToolbar->HasIcon(result);
}

ECode WindowDecorActionBar::SetLogo(
    /* [in] */ Int32 resId)
{
    return mDecorToolbar->SetLogo(resId);
}

ECode WindowDecorActionBar::SetLogo(
    /* [in] */ IDrawable* logo)
{
    return mDecorToolbar->SetLogo(logo);
}

ECode WindowDecorActionBar::HasLogo(
    /* [out] */ Boolean* result)
{
    return mDecorToolbar->HasLogo(result);
}

ECode WindowDecorActionBar::SetDefaultDisplayHomeAsUpEnabled(
    /* [in] */ Boolean enable)
{
    if (!mDisplayHomeAsUpSet) {
        SetDisplayHomeAsUpEnabled(enable);
    }
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
