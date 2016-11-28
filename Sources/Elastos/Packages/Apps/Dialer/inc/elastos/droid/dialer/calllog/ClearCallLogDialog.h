
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CLEARCALLLOGDIALOG_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CLEARCALLLOGDIALOG_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/os/AsyncTask.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Dialer::Service::ICachedNumberLookupService;

namespace Elastos {
namespace Droid {
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
            /* [in] */ IContext* context,
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
        AutoPtr<IContext> mContext;
        AutoPtr<IContentResolver> mResolver;
        AutoPtr<IProgressDialog> mProgressDialog;
        ClearCallLogDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /** Preferred way to show this dialog */
    static CARAPI_(void) Show(
        /* [in] */ IFragmentManager* fragmentManager);

    // @Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);

private:
    static AutoPtr<ICachedNumberLookupService> mCachedNumberLookupService;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CLEARCALLLOGDIALOG_H__

