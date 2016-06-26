#ifndef  __ELASTOS_DROID_PHONE_CERRORDIALOGACTIVITY_H__
#define  __ELASTOS_DROID_PHONE_CERRORDIALOGACTIVITY_H__

#include "_Elastos_Droid_Phone_CErrorDialogActivity.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Used to display an error dialog from within the Telephony service when an outgoing call fails
 */
CarClass(CErrorDialogActivity)
    , public Activity
    , public IErrorDialogActivity
{
private:
    class MyDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnClickListener")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CErrorDialogActivity* mHost;

    };

    class MyDialogInterfaceOnClickListener2
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnClickListener2")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener2(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CErrorDialogActivity* mHost;

    };

    class MyDialogInterfaceOnClickListener3
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnClickListener3")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnClickListener3(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CErrorDialogActivity* mHost;

    };

    class MyDialogInterfaceOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnCancelListener")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnCancelListener(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        CErrorDialogActivity* mHost;
    };

    class MyDialogInterfaceOnCancelListener2
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        TO_STRING_IMPL("CErrorDialogActivity::MyDialogInterfaceOnCancelListener2")

        CAR_INTERFACE_DECL()

        MyDialogInterfaceOnCancelListener2(
            /* [in] */ CErrorDialogActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        CErrorDialogActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CErrorDialogActivity() {}

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) ShowGenericErrorDialog(
        /* [in] */ Int32 resid);

    CARAPI_(void) ShowMissingVoicemailErrorDialog();

    CARAPI_(void) AddVoiceMailNumberPanel(
        /* [in] */ IDialogInterface* dialog);

    CARAPI_(void) DontAddVoiceMailNumber();

private:
    static const String TAG;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CERRORDIALOGACTIVITY_H__