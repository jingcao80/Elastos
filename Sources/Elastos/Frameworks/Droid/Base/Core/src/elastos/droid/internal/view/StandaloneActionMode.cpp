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

#include "elastos/droid/internal/view/StandaloneActionMode.h"
#include "elastos/droid/internal/view/menu/CMenuBuilder.h"
#include "elastos/droid/internal/view/menu/CMenuPopupHelper.h"
#include "elastos/droid/view/CMenuInflater.h"

using Elastos::Droid::Internal::View::Menu::EIID_IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::CMenuBuilder;
using Elastos::Droid::Internal::View::Menu::CMenuPopupHelper;
using Elastos::Droid::Internal::View::Menu::IMenuPopupHelper;
using Elastos::Droid::View::CMenuInflater;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::Internal::Widget::IAbsActionBarView;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CAR_INTERFACE_IMPL(StandaloneActionMode::MenuBuilderCallback, Object, IMenuBuilderCallback)

StandaloneActionMode::MenuBuilderCallback::MenuBuilderCallback(
    /* [in] */ StandaloneActionMode* owner)
    : mOwner(owner)
{
}

ECode StandaloneActionMode::MenuBuilderCallback::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* state)
{
    return mOwner->OnMenuItemSelected(menu, item, state);
}

ECode StandaloneActionMode::MenuBuilderCallback::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    return mOwner->OnMenuModeChange(menu);
}
StandaloneActionMode::StandaloneActionMode()
    : mContext(NULL)
    , mFinished(FALSE)
    , mFocusable(FALSE)
{
}

ECode StandaloneActionMode::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IActionBarContextView* view,
    /* [in] */ IActionModeCallback* callback,
    /* [in] */ Boolean isFocusable)
{
    mContext = context;
    mContextView = view;
    mCallback = callback;

    AutoPtr<IContext> ctx;
    IView::Probe(view)->GetContext((IContext**)&ctx);
    CMenuBuilder::New(ctx, (IMenuBuilder**)&mMenu);
    mMenu->SetDefaultShowAsAction(IMenuItem::SHOW_AS_ACTION_IF_ROOM);
    AutoPtr<MenuBuilderCallback> clb = new MenuBuilderCallback(this);
    mMenu->SetCallback(clb);
    mFocusable = isFocusable;

    return NOERROR;
}

ECode StandaloneActionMode::SetTitle(
    /* [in] */ ICharSequence* title)
{
    return mContextView->SetTitle(title);
}

ECode StandaloneActionMode::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    return mContextView->SetSubtitle(subtitle);
}

ECode StandaloneActionMode::SetTitle(
    /* [in] */ Int32 resId)
{
    String str;
    mContext->GetString(resId, &str);
    AutoPtr<ICharSequence> title;
    CString::New(str, (ICharSequence**)&title);
    return SetTitle(title);
}

ECode StandaloneActionMode::SetSubtitle(
    /* [in] */ Int32 resId)
{
    String str;
    mContext->GetString(resId, &str);
    AutoPtr<ICharSequence> subTitle;
    CString::New(str, (ICharSequence**)&subTitle);
    return SetSubtitle(subTitle);
}

ECode StandaloneActionMode::SetTitleOptionalHint(
    /* [in] */ Boolean titleOptional)
{
    ActionMode::SetTitleOptionalHint(titleOptional);
    return mContextView->SetTitleOptional(titleOptional);
}

ECode StandaloneActionMode::IsTitleOptional(
    /* [out] */ Boolean* isTitleOptional)
{
    VALIDATE_NOT_NULL(isTitleOptional);
    return mContextView->IsTitleOptional(isTitleOptional);
}

ECode StandaloneActionMode::SetCustomView(
    /* [in] */ IView* view)
{
    mContextView->SetCustomView(view);
    mCustomView = NULL;
    if (view != NULL) {
        assert(IWeakReferenceSource::Probe(view) != NULL);
        IWeakReferenceSource::Probe(view)->GetWeakReference((IWeakReference**)&mCustomView);
    }
    return NOERROR;
}

ECode StandaloneActionMode::Invalidate()
{
    Boolean isPrepare = FALSE;
    return mCallback->OnPrepareActionMode(this, IMenu::Probe(mMenu), &isPrepare);
}

ECode StandaloneActionMode::Finish()
{
    if (mFinished) {
        return NOERROR;
    }
    mFinished = TRUE;

    AutoPtr<IAccessibilityEventSource> eventSource = IAccessibilityEventSource::Probe(mContextView);
    eventSource->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
    return mCallback->OnDestroyActionMode(this);
}

ECode StandaloneActionMode::GetMenu(
    /* [out] */ IMenu** menu)
{
    VALIDATE_NOT_NULL(menu);
    *menu = IMenu::Probe(mMenu);
    REFCOUNT_ADD(*menu);
    return NOERROR;
}

ECode StandaloneActionMode::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);
    return mContextView->GetTitle(title);
}

ECode StandaloneActionMode::GetSubtitle(
    /* [out] */ ICharSequence** subtitle)
{
    VALIDATE_NOT_NULL(subtitle);
    return mContextView->GetSubtitle(subtitle);
}

ECode StandaloneActionMode::GetCustomView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    if (mCustomView != NULL) {
        mCustomView->Resolve(EIID_IView, (IInterface**)view);
    }
    return NOERROR;
}

ECode StandaloneActionMode::GetMenuInflater(
    /* [out] */ IMenuInflater** menuInflater)
{
    VALIDATE_NOT_NULL(menuInflater);
    AutoPtr<IContext> context;
    IView::Probe(mContextView)->GetContext((IContext**)&context);
    return CMenuInflater::New(context, menuInflater);
}

ECode StandaloneActionMode::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* state)
{
    VALIDATE_NOT_NULL(state);
    return mCallback->OnActionItemClicked(this, item, state);
}

ECode StandaloneActionMode::OnCloseMenu(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ Boolean allMenusAreClosing)
{
    return NOERROR;
}

Boolean StandaloneActionMode::OnSubMenuSelected(
    /* [in] */ ISubMenuBuilder* subMenu)
{
    Boolean isVisible = FALSE;
    if (!(IMenu::Probe(subMenu)->HasVisibleItems(&isVisible), isVisible)) {
        return TRUE;
    }

    AutoPtr<IContext> context;
    IView::Probe(mContextView)->GetContext((IContext**)&context);
    AutoPtr<IMenuPopupHelper> menuPopupHelper;
    CMenuPopupHelper::New(context, IMenuBuilder::Probe(subMenu), (IMenuPopupHelper**)&menuPopupHelper);
    menuPopupHelper->Show();
    return TRUE;
}

ECode StandaloneActionMode::OnCloseSubMenu(
    /* [in] */ ISubMenuBuilder* menu)
{
    return NOERROR;
}

ECode StandaloneActionMode::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    Invalidate();
    Boolean isShow = FALSE;
    return IAbsActionBarView::Probe(mContextView)->ShowOverflowMenu(&isShow);
}

ECode StandaloneActionMode::IsUiFocusable(
    /* [out] */ Boolean* uiFocusable)
{
    VALIDATE_NOT_NULL(uiFocusable);
    *uiFocusable = mFocusable;
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
