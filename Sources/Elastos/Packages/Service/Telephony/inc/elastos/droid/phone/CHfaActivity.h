#ifndef  __ELASTOS_DROID_PHONE_CHFAACTIVITY_H__
#define  __ELASTOS_DROID_PHONE_CHFAACTIVITY_H__

#include "_Elastos_Droid_Phone_CHfaActivity.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
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
    , public IHfaActivity
{
private:
    class MyDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CHfaActivity::MyDialogInterfaceOnClickListener")

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
    CAR_INTERFACE_DECL()

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
    static const String TAG = HfaActivity.class.getSimpleName();

    AutoPtr<IAlertDialog> mDialog;
    AutoPtr<IHfaLogic> mHfaLogic;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CHFAACTIVITY_H__