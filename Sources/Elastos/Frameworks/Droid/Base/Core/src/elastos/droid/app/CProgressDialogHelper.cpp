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