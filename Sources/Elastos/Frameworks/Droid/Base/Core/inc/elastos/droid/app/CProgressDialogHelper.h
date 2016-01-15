
#ifndef __ELASTOS_DROID_APP_CPROGRESS_DIALOG_HELPER_H__
#define __ELASTOS_DROID_APP_CPROGRESS_DIALOG_HELPER_H__

#include "_Elastos_Droid_App_CProgressDialogHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CProgressDialogHelper)
    , public Singleton
    , public IProgressDialogHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Show(
        /* [in] */ IContext* ctx,
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* message,
        /* [out] */ IProgressDialog** pDialog);

    CARAPI Show(
        /* [in] */ IContext* ctx,
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* message,
        /* [in] */ Boolean indeterminate,
        /* [out] */ IProgressDialog** pDialog);

    CARAPI Show(
        /* [in] */ IContext* ctx,
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* message,
        /* [in] */ Boolean indeterminate,
        /* [in] */ Boolean cancelable,
        /* [out] */ IProgressDialog** pDialog);

    CARAPI Show(
        /* [in] */ IContext* ctx,
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* message,
        /* [in] */ Boolean indeterminate,
        /* [in] */ Boolean cancelable,
        /* [in] */ IDialogInterfaceOnCancelListener* cancelListener,
        /* [out] */ IProgressDialog** pDialog);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CPROGRESS_DIALOG_HELPER_H__
