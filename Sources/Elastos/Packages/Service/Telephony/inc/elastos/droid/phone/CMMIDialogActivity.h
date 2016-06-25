#ifndef  __ELASTOS_DROID_PHONE_CMMIDIALOGACTIVITY_H__
#define  __ELASTOS_DROID_PHONE_CMMIDIALOGACTIVITY_H__

#include "_Elastos_Droid_Phone_CMMIDialogActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ICallManager;
using Elastos::Droid::Internal::Telephony::IMmiCode;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Used to display a dialog from within the Telephony service when running an USSD code
 */
CarClass(CMMIDialogActivity)
    , public Activity
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CMMIDialogActivity::MyHandler")

        MyHandler(
            /* [in] */ CMMIDialogActivity* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CMMIDialogActivity* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CMMIDialogActivity();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) ShowMMIDialog();

    /**
     * Handles an MMI_COMPLETE event, which is triggered by telephony
     */
    CARAPI_(void) OnMMIComplete(
        /* [in] */ IMmiCode* mmiCode);

    /**
     * Handles an MMI_CANCEL event, which is triggered by the button
     * (labeled either "OK" or "Cancel") on the "MMI Started" dialog.
     * @see PhoneUtils#cancelMmiCode(Phone)
     */
    CARAPI_(void) OnMMICancel();

    CARAPI_(void) DismissDialogsAndFinish();

private:
    static const String TAG;

    AutoPtr<IDialog> mMMIDialog;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ICallManager> mCM;
    AutoPtr<IPhone> mPhone;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CMMIDIALOGACTIVITY_H__