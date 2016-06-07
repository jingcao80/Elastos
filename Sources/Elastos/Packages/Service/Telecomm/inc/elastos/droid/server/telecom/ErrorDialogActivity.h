
#ifndef __ELASTOS_DROID_SERVER_TELECOM_ERRORDIALOGACTIVITY_H__
#define __ELASTOS_DROID_SERVER_TELECOM_ERRORDIALOGACTIVITY_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Used to display an error dialog from within the Telecom service when an outgoing call fails
 */
class ErrorDialogActivity
    : public Activity
    , public IErrorDialogActivity
{
private:
    class ClickSubDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ClickSubDialogInterfaceOnClickListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        ErrorDialogActivity* mHost;
    };

    class CancelSubDialogInterfaceOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL()

        CancelSubDialogInterfaceOnCancelListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        ErrorDialogActivity* mHost;
    };

    class PositiveSubDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        PositiveSubDialogInterfaceOnClickListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        ErrorDialogActivity* mHost;
    };

    class NegativeSubDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        NegativeSubDialogInterfaceOnClickListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        ErrorDialogActivity* mHost;
    };

    class CancelShowSubDialogInterfaceOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL()

        CancelShowSubDialogInterfaceOnCancelListener(
            /* [in] */ ErrorDialogActivity* host);

        // @Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        ErrorDialogActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI Finish();

private:
    CARAPI ShowGenericErrorDialog(
        /* [in] */ Int32 resid);

    CARAPI ShowMissingVoicemailErrorDialog();

    CARAPI AddVoiceMailNumberPanel(
        /* [in] */ IDialogInterface* dialog);

private:
    static const String TAG;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_ERRORDIALOGACTIVITY_H__
