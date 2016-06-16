#ifndef  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__
#define  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Utility class to look up carrier logo resource IDs.
 */
class TimeConsumingPreferenceActivity
    : public PreferenceActivity
    , public ITimeConsumingPreferenceListener
    , public IDialogInterfaceOnCancelListener
{
private:
    class DismissOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("TimeConsumingPreferenceActivity::DismissOnClickListener")

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);
    };

    class DismissAndFinishOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("TimeConsumingPreferenceActivity::DismissAndFinishOnClickListener")

        CAR_INTERFACE_DECL()

        DismissAndFinishOnClickListener(
            TimeConsumingPreferenceActivity* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        TimeConsumingPreferenceActivity* mHost;
    };

public:
    TO_STRING_IMPL("TimeConsumingPreferenceActivity")

    CAR_INTERFACE_DECL()

    TimeConsumingPreferenceActivity();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnStarted(
        /* [in] */ IPreference* preference,
        /* [in] */ Boolean reading);

    //@Override
    CARAPI OnFinished(
        /* [in] */ IPreference* preference,
        /* [in] */ Boolean reading);

    //@Override
    CARAPI OnError(
        /* [in] */ IPreference* preference,
        /* [in] */ Int32 error);

    //@Override
    CARAPI OnException(
        /* [in] */ IPreference* preference,
        /* [in] */ ICommandException* exception);

    //@Override
    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    CARAPI DumpState();

protected:
    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 id,
        /* [out] */ IDialog** dialog);

private:
    CARAPI_(void) DismissDialogSafely(
        /* [in] */ Int32 id)

protected:
    Boolean mIsForeground;

private:
    static const String LOG_TAG;
    const Boolean DBG;

    AutoPtr<IDialogInterfaceOnClickListener> mDismiss;
    AutoPtr<IDialogInterfaceOnClickListener> mDismissAndFinish;

    static const Int32 BUSY_READING_DIALOG;
    static const Int32 BUSY_SAVING_DIALOG;

    static const Int32 EXCEPTION_ERROR;
    static const Int32 RESPONSE_ERROR;
    static const Int32 RADIO_OFF_ERROR;
    static const Int32 FDN_CHECK_FAILURE;

    AutoPtr<IArrayList> mBusyList;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CARRIERLOGO_H__