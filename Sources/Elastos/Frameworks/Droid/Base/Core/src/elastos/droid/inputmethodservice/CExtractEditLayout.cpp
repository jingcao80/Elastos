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

#include "elastos/droid/inputmethodservice/CExtractEditLayout.h"
#include "elastos/droid/internal/view/menu/CMenuBuilder.h"
#include "elastos/droid/internal/view/menu/CMenuPopupHelper.h"
#include "elastos/droid/view/CMenuInflater.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::View::CMenuInflater;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IActionMode;
using Elastos::Droid::Internal::View::Menu::CMenuBuilder;
using Elastos::Droid::Internal::View::Menu::CMenuPopupHelper;
using Elastos::Droid::Internal::View::Menu::EIID_IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::IMenuPopupHelper;

namespace Elastos {
namespace Droid {
namespace InputMethodService {


CAR_INTERFACE_IMPL(CExtractEditLayout::ExtractActionMode::MenuBuilderCallback, Object, IMenuBuilderCallback);


CExtractEditLayout::ExtractActionMode::MenuBuilderCallback::MenuBuilderCallback(
    /* [in] */ ExtractActionMode* host)
    : mHost(host)
{}

ECode CExtractEditLayout::ExtractActionMode::MenuBuilderCallback::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    return mHost->OnMenuItemSelected(menu, item, result);
}

ECode CExtractEditLayout::ExtractActionMode::MenuBuilderCallback::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    return mHost->OnMenuModeChange(menu);
}

CExtractEditLayout::ExtractActionMode::ExtractActionMode(
    /* [in] */ IActionModeCallback* cb,
    /* [in] */ CExtractEditLayout* host)
    : mHost(host)
{
    mCallback = cb;
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    CMenuBuilder::New(context, (IMenuBuilder**)&mMenu);
    mMenuCallback = new MenuBuilderCallback(this);
    mMenu->SetCallback(mMenuCallback);
}

ECode CExtractEditLayout::ExtractActionMode::SetTitle(
    /* [in] */ ICharSequence* title)
{
    // Title will not be shown.
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::SetTitle(
    /* [in] */ Int32 resId)
{
    // Title will not be shown.
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    // Subtitle will not be shown.
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::SetSubtitle(
    /* [in] */ Int32 resId)
{
    // Subtitle will not be shown.
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::IsTitleOptional(
    /* [out] */ Boolean* result)
{
    assert(result != NULL);
    *result = TRUE;
    // Not only is it optional, it will *never* be shown.
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::SetCustomView(
    /* [in] */ IView* view)
{
    // Custom view is not supported here.
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::Invalidate()
{
    mMenu->StopDispatchingItemsChanged();
    Boolean tmp = FALSE;
    mCallback->OnPrepareActionMode(this, IMenu::Probe(mMenu), &tmp);
    return mMenu->StartDispatchingItemsChanged();
}

Boolean CExtractEditLayout::ExtractActionMode::DispatchOnCreate()
{
    mMenu->StopDispatchingItemsChanged();
    Boolean ret = FALSE;
    mCallback->OnCreateActionMode(this, IMenu::Probe(mMenu), &ret);
    mMenu->StartDispatchingItemsChanged();
    return ret;
}

ECode CExtractEditLayout::ExtractActionMode::Finish()
{
    if (mHost->mActionMode.Get() != this) {
        // Not the active action mode - no-op
        return NOERROR;
    }

    mCallback->OnDestroyActionMode(this);
    mCallback = NULL;

    IView::Probe(mHost->mExtractActionButton)->SetVisibility(IView::VISIBLE);
    IView::Probe(mHost->mEditButton)->SetVisibility(IView::INVISIBLE);

    mHost->SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);

    mHost->mActionMode = NULL;
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::GetMenu(
    /* [out] */ IMenu** menu)
{
    assert(menu != NULL);
    *menu = IMenu::Probe(mMenu);
    REFCOUNT_ADD(*menu);

    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::GetTitle(
    /* [out] */ ICharSequence** title)
{
    assert(title != NULL);
    *title = NULL;
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::GetSubtitle(
    /* [out] */ ICharSequence** subtitle)
{
    assert(subtitle != NULL);
    *subtitle = NULL;
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::GetCustomView(
    /* [out] */ IView** view)
{
    assert(view != NULL);
    *view = NULL;
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::GetMenuInflater(
    /* [out] */ IMenuInflater** inflater)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    return CMenuInflater::New(context, inflater);
}

ECode CExtractEditLayout::ExtractActionMode::OnMenuItemSelected(
    /* [in] */ IMenuBuilder* menu,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    assert(result != NULL);

    if (mCallback != NULL) {
        return mCallback->OnActionItemClicked(this, item, result);
    }

    *result = FALSE;
    return NOERROR;
}

ECode CExtractEditLayout::ExtractActionMode::OnMenuModeChange(
    /* [in] */ IMenuBuilder* menu)
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(CExtractEditLayout::_OnClickListener, Object, IViewOnClickListener);
CExtractEditLayout::_OnClickListener::_OnClickListener(
    /* [in] */ CExtractEditLayout* host)
    : mHost(host)
{
}

ECode CExtractEditLayout::_OnClickListener::OnClick(
    /* [in] */ IView* clicked)
{
    if (mHost->mActionMode != NULL) {
        AutoPtr<IContext> context;
        mHost->GetContext((IContext**)&context);
        AutoPtr<IMenuPopupHelper> helper;
        CMenuPopupHelper::New(context, mHost->mActionMode->mMenu, clicked, (IMenuPopupHelper**)&helper);
        helper->Show();
    }

    return NOERROR;
}

ECode CExtractEditLayout::StartActionModeForChild(
    /* [in] */ IView* sourceView,
    /* [in] */ IActionModeCallback* cb,
    /* [out] */ IActionMode** am)
{
    VALIDATE_NOT_NULL(am);
    AutoPtr<ExtractActionMode> mode = new ExtractActionMode(cb, this);
    if (mode->DispatchOnCreate()) {
        mode->Invalidate();
        IView::Probe(mExtractActionButton)->SetVisibility(IView::INVISIBLE);
        IView::Probe(mEditButton)->SetVisibility(IView::VISIBLE);
        mActionMode = mode;
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
        *am = mode;
        REFCOUNT_ADD(*am);
        return NOERROR;
    }

    *am = NULL;
    return NOERROR;
}

ECode CExtractEditLayout::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::inputExtractAction, (IView**)&view);
    mExtractActionButton = IButton::Probe(view);
    view = NULL;
    FindViewById(R::id::inputExtractEditButton, (IView**)&view);
    mEditButton = IButton::Probe(view);
    AutoPtr<_OnClickListener> listener = new _OnClickListener(this);
    return IView::Probe(mEditButton)->SetOnClickListener(listener);
}

CAR_OBJECT_IMPL(CExtractEditLayout);
CAR_INTERFACE_IMPL(CExtractEditLayout, LinearLayout, IExtractEditLayout);
ECode CExtractEditLayout::constructor(
    /* [in] */ IContext* ctx)
{
    return LinearLayout::constructor(ctx);
}

ECode CExtractEditLayout::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(ctx, attrs);
}

ECode CExtractEditLayout::IsActionModeStarted(
    /* [out] */ Boolean* started)
{
    assert(started != NULL);
    *started = mActionMode != NULL;
    return NOERROR;
}

ECode CExtractEditLayout::FinishActionMode()
{
    if (mActionMode != NULL) {
        return mActionMode->Finish();
    }

    return NOERROR;
}

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
