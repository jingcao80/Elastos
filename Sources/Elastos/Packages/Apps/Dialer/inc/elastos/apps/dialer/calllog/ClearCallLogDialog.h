
#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CLEARCALLLOGDIALOG_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CLEARCALLLOGDIALOG_H__

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

/**
 * Dialog that clears the call log after confirming with the user
 */
class ClearCallLogDialog
    : public DialogFragment
    , public IClearCallLogDialog
{
private:
    class OKClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        OKClickListener(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ ClearCallLogDialog* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        AutoPtr<IContentResolver> mResolver;
        ClearCallLogDialog* mHost;
    };

    class MyAsyncTask
        : public AsyncTask
    {
    public:
        MyAsyncTask(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ IProgressDialog* progressDialog,
            /* [in] */ ClearCallLogDialog* host);

    protected:
        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** obj);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* info);
    private:
        AutoPtr<IContentResolver> mResolver;
        AutoPtr<IProgressDialog> mProgressDialog;
        ClearCallLogDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ClearCallLogDialog();

    /** Preferred way to show this dialog */
    static CARAPI_(void) Show(
        /* [in] */ IFragmentManager fragmentManager);

    // @Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);

private:
    static AutoPtr<ICachedNumberLookupService> mCachedNumberLookupService;
};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CLEARCALLLOGDIALOG_H__

