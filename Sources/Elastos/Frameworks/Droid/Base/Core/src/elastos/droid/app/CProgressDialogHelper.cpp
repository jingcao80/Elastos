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

#include "elastos/droid/app/CProgressDialogHelper.h"
#include "elastos/droid/app/ProgressDialog.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CProgressDialogHelper, Singleton, IProgressDialogHelper)

CAR_SINGLETON_IMPL(CProgressDialogHelper)

ECode CProgressDialogHelper::Show(
    /* [in] */ IContext* ctx,
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* message,
    /* [out] */ IProgressDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    AutoPtr<IProgressDialog> tmp = ProgressDialog::Show(ctx, title, message);
    *dialog = tmp;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

ECode CProgressDialogHelper::Show(
    /* [in] */ IContext* ctx,
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* message,
    /* [in] */ Boolean indeterminate,
    /* [out] */ IProgressDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    AutoPtr<IProgressDialog> tmp = ProgressDialog::Show(ctx, title, message, indeterminate);
    *dialog = tmp;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

ECode CProgressDialogHelper::Show(
    /* [in] */ IContext* ctx,
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* message,
    /* [in] */ Boolean indeterminate,
    /* [in] */ Boolean cancelable,
    /* [out] */ IProgressDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    AutoPtr<IProgressDialog> tmp = ProgressDialog::Show(ctx, title, message, indeterminate, cancelable);
    *dialog = tmp;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

ECode CProgressDialogHelper::Show(
    /* [in] */ IContext* ctx,
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* message,
    /* [in] */ Boolean indeterminate,
    /* [in] */ Boolean cancelable,
    /* [in] */ IDialogInterfaceOnCancelListener* cancelListener,
    /* [out] */ IProgressDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    AutoPtr<IProgressDialog> tmp = ProgressDialog::Show(ctx, title, message, indeterminate, cancelable, cancelListener);
    *dialog = tmp;
    REFCOUNT_ADD(*dialog)
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos