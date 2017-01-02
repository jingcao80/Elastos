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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/WindowCallbackWrapper.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(WindowCallbackWrapper, Object, IWindowCallback)

WindowCallbackWrapper::WindowCallbackWrapper()
{
}

ECode WindowCallbackWrapper::constructor(
    /* [in] */ IWindowCallback* wrapped)
{
    if (wrapped == NULL) {
        // throw new IllegalArgumentException("Window callback may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(wrapped);
    wrs->GetWeakReference((IWeakReference**)&mWeakCallback);
    return NOERROR;
}

ECode WindowCallbackWrapper::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->DispatchKeyEvent(event, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->DispatchKeyShortcutEvent(event, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->DispatchTouchEvent(event, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->DispatchTrackballEvent(event, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->DispatchGenericMotionEvent(event, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->DispatchPopulateAccessibilityEvent(event, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnCreatePanelView(
    /* [in] */ Int32 featureId,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnCreatePanelView(featureId, view);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnCreatePanelMenu(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnCreatePanelMenu(featureId, menu, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnPreparePanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IView* view,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnPreparePanel(featureId, view, menu, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnMenuOpened(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnMenuOpened(featureId, menu, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnMenuItemSelected(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnMenuItemSelected(featureId, item, result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnWindowAttributesChanged(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnWindowAttributesChanged(attrs);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnContentChanged()
{
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnContentChanged();
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnWindowFocusChanged(hasFocus);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnAttachedToWindow()
{
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnAttachedToWindow();
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnDetachedFromWindow()
{
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnDetachedFromWindow();
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnPanelClosed(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu)
{
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnPanelClosed(featureId, menu);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnSearchRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnSearchRequested(result);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnWindowStartingActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** mode)
{
    VALIDATE_NOT_NULL(mode);
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnWindowStartingActionMode(callback, mode);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnActionModeStarted(
    /* [in] */ IActionMode* mode)
{
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnActionModeStarted(mode);
    }
    return NOERROR;
}

ECode WindowCallbackWrapper::OnActionModeFinished(
    /* [in] */ IActionMode* mode)
{
    AutoPtr<IWindowCallback> cb;
    mWeakCallback->Resolve(EIID_IWindowCallback, (IInterface**)&cb);
    if (cb) {
        return cb->OnActionModeFinished(mode);
    }
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
