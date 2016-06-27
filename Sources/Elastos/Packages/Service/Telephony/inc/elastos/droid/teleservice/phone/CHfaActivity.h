#ifndef  __ELASTOS_DROID_PHONE_CHFAACTIVITY_H__
#define  __ELASTOS_DROID_PHONE_CHFAACTIVITY_H__

#include "_Elastos_Droid_TeleService_Phone_CHfaActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Starts and displays status for Hands Free Activation (HFA).
 *
 * This class operates with Hands Free Activation apps. It comes up during activation
 * requests that occur outside of setup wizard and so provides its own UI.
 * It uses {@link HfaLogic} to perform the actual activation and during the process
 * displays a "performing activation..." dialog.  This will remain up until the user
 * chooses to skip the activation (still happens in the background) or the activation
 * is successful.  Upon failure, the dialog also goes away but a subsequent dialog will
 * ask the user if they would like to try again or cancel.
 */
CarClass(CHfaActivity)
    , public Activity
{
private:
    class MyDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CHfaActivity::MyDialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener(
            /* [in] */ CHfaActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* di,
            /* [in] */ Int32 which);

    private:
        CHfaActivity* mHost;
    };

    class MyDialogInterfaceOnClickListener2
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CHfaActivity::MyDialogInterfaceOnClickListener2")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener2(
            /* [in] */ CHfaActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* di,
            /* [in] */ Int32 which);

    private:
        CHfaActivity* mHost;
    };

    class MyDialogInterfaceOnClickListener3
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CHfaActivity::MyDialogInterfaceOnClickListener3")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener3(
            /* [in] */ CHfaActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* di,
            /* [in] */ Int32 which);

    private:
        CHfaActivity* mHost;
    };

    class MyHfaLogicHfaLogicCallback
        : public Object
        , public IHfaLogicHfaLogicCallback
    {
    public:
        TO_STRING_IMPL("CHfaActivity::MyHfaLogicHfaLogicCallback")

        CAR_INTERFACE_DECL()

        MyHfaLogicHfaLogicCallback(
            /* [in] */ CHfaActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnSuccess();

        //@Override
        CARAPI OnError(
            /* [in] */ const String& error);

    private:
        CHfaActivity* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnDestroy();

private:
    CARAPI_(void) StartProvisioning();

    CARAPI_(void) BuildAndShowDialog();

    CARAPI_(void) OnHfaError(
        /* [in] */ const String& errorMsg);

    CARAPI_(void) OnHfaSuccess();

    CARAPI_(void) OnUserSkip();

private:
    static const String TAG;

    AutoPtr<IAlertDialog> mDialog;
    AutoPtr<IHfaLogic> mHfaLogic;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CHFAACTIVITY_H__