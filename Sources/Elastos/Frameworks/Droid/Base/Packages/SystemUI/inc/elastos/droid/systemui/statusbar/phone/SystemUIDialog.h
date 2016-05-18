
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_SYSTEMUIDIALOG_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_SYSTEMUIDIALOG_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/app/AlertDialog.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Base class for dialogs that should appear over panels and keyguard.
 */
class SystemUIDialog
    : public AlertDialog
    , public ISystemUIDialog
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetShowForAllUsers(
        /* [in] */ Boolean show);

    using AlertDialog::SetMessage;

    CARAPI SetMessage(
        /* [in] */ Int32 resId);

    CARAPI SetPositiveButton(
        /* [in] */ Int32 resId,
        /* [in] */ IDialogInterfaceOnClickListener* onClick);

    CARAPI SetNegativeButton(
        /* [in] */ Int32 resId,
        /* [in] */ IDialogInterfaceOnClickListener* onClick);

protected:
    virtual CARAPI_(AutoPtr<ICharSequence>) GetSimpleName();

private:
    AutoPtr<IContext> mContext;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_SYSTEMUIDIALOG_H__
