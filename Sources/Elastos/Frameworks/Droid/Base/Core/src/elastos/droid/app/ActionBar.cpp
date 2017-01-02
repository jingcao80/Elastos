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

#include "elastos/droid/app/ActionBar.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(ActionBar, Object, IActionBar)

ECode ActionBar::SetHomeButtonEnabled(
    /* [in] */ Boolean enabled)
{
    return NOERROR;
}

ECode ActionBar::GetThemedContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = NULL;
    return NOERROR;
}

ECode ActionBar::IsTitleTruncated(
    /* [out] */ Boolean* bval)
{
    VALIDATE_NOT_NULL(bval)
    *bval = FALSE;
    return NOERROR;
}

ECode ActionBar::SetHomeAsUpIndicator(
    /* [in] */ IDrawable* indicator)
{
    return NOERROR;
}

ECode ActionBar::SetHomeAsUpIndicator(
    /* [in] */ Int32 resId)
{
    return NOERROR;
}

ECode ActionBar::SetHomeActionContentDescription(
    /* [in] */ ICharSequence* description)
{
    return NOERROR;
}

ECode ActionBar::SetHomeActionContentDescription(
    /* [in] */ Int32 resId)
{
    return NOERROR;
}

ECode ActionBar::SetHideOnContentScrollEnabled(
    /* [in] */ Boolean hideOnContentScroll)
{
    if (hideOnContentScroll) {
        // throw new UnsupportedOperationException("Hide on content scroll is not supported in " +
        //         "this action bar configuration.")
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode ActionBar::IsHideOnContentScrollEnabled(
    /* [out] */ Boolean* bval)
{
    VALIDATE_NOT_NULL(bval)
    *bval = FALSE;
    return NOERROR;
}

ECode ActionBar::GetHideOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)
    *offset = 0;
    return NOERROR;
}

ECode ActionBar::SetHideOffset(
    /* [in] */ Int32 offset)
{
    if (offset != 0) {
        // throw new UnsupportedOperationException("Setting an explicit action bar hide offset " +
        //         "is not supported in this action bar configuration.")
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode ActionBar::SetElevation(
    /* [in] */ Float elevation)
{
    if (elevation != 0) {
        // throw new UnsupportedOperationException("Setting a non-zero elevation is " +
        //         "not supported in this action bar configuration.")
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode ActionBar::GetElevation(
    /* [out] */ Float* fval)
{
    VALIDATE_NOT_NULL(fval)
    *fval = 0;
    return NOERROR;
}

ECode ActionBar::SetDefaultDisplayHomeAsUpEnabled(
    /* [in] */ Boolean enabled)
{
    return NOERROR;
}

ECode ActionBar::SetShowHideAnimationEnabled(
    /* [in] */ Boolean enabled)
{
    return NOERROR;
}

ECode ActionBar::OnConfigurationChanged(
    /* [in] */ IConfiguration* config)
{
    return NOERROR;
}

ECode ActionBar::DispatchMenuVisibilityChanged(
    /* [in] */ Boolean visible)
{
    return NOERROR;
}

ECode ActionBar::StartActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = NULL;
    return NOERROR;
}

ECode ActionBar::OpenOptionsMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ActionBar::InvalidateOptionsMenu(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ActionBar::OnMenuKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ActionBar::CollapseActionView(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode ActionBar::SetWindowTitle(
    /* [in] */ ICharSequence* title)
{
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos