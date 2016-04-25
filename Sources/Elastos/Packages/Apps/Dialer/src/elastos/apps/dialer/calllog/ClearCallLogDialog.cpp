
#include "ClearCallLogDialog.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

//=================================================================
// ClearCallLogDialog::OKClickListener
//=================================================================
CAR_INTERFACE_IMPL(ClearCallLogDialog::OKClickListener, Object, IDialogInterfaceOnClickListener)

ClearCallLogDialog::OKClickListener::OKClickListener(
    /* [in] */ ClearCallLogDialog* host)
    : mResolver(resolver)
    , mHost(host)
{}

ECode ClearCallLogDialog::OKClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IProgressDialogHelper> helper;
    CProgressDialogHelper::AcquireSingleton(&helper);
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    String str;
    mHost->GetString(R.string.clearCallLogProgress_title, &str);
    AutoPtr<IProgressDialog> progressDialog;
    helper->Show(activity, str, String(""),
            TRUE, FALSE, (IProgressDialog**)&progressDialog);
    AutoPtr<MyAsyncTask> task = new MyAsyncTask(mResolver, mHost);
    // TODO: Once we have the API, we should configure this ProgressDialog
    // to only show up after a certain time (e.g. 150ms)
    progressDialog->Show();
    IAsyncTask::Probe(task)->Execute();

    return NOERROR;
}

//=================================================================
// ClearCallLogDialog::MyAsyncTask
//=================================================================
ClearCallLogDialog::MyAsyncTask::MyAsyncTask(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IProgressDialog* progressDialog,
    /* [in] */ ClearCallLogDialog* host)
    : mResolver(resolver)
    , mProgressDialog(progressDialog)
    , mHost(host)
{}

ECode ClearCallLogDialog::MyAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** obj)
{
    VALUE_NOT_NULL(obj);
    mResolver->Delete(ICalls::CONTENT_URI, NULL, NULL);
    if (mHost->mCachedNumberLookupService != NULL) {
        mHost->mCachedNumberLookupService->ClearAllCacheEntries(context);
    }
    *obj = null;
    return NOERROR;
}

ECode ClearCallLogDialog::MyAsyncTask::OnPostExecute(
    /* [in] */ IInterface* info)
{
    mProgressDialog->Dismiss();
    return NOERROR;
}

//=================================================================
// ClearCallLogDialog
//=================================================================
CAR_INTERFACE_IMPL(ClearCallLogDialog, DialogFragment, IClearCallLogDialog)

AutoPtr<ICachedNumberLookupService> ClearCallLogDialog::mCachedNumberLookupService =
        CObjectFactory::NewCachedNumberLookupService();

ClearCallLogDialog::ClearCallLogDialog()
{}

void ClearCallLogDialog::Show(
    /* [in] */ IFragmentManager fragmentManager)
{
    AutoPtr<IClearCallLogDialog> dialog;
    CClearCallLogDialog::New((IClearCallLogDialog**)&dialog);
    IDialogFragment::Probe(dialog)->Show(fragmentManager, String("deleteCallLog"));
}

ECode ClearCallLogDialog::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALUE_NOT_NULL(dialog);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    activity->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IContext> context;
    activity->GetApplicationContext((IContext**)&context);

    AutoPtr<OKClickListener> okListener = new OKClickListener(resolver, this);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::clearCallLogConfirmation_title);
    builder->SetIconAttribute(Elastos::R::attr::alertDialogIcon);
    builder->SetMessage(R::string::clearCallLogConfirmation);
    builder->SetNegativeButton(Elastos::R::string::cancel, NULL);
    builder->SetPositiveButton(Elastos::R::string::ok, okListener);
    builder->SetCancelable(TRUE);
    builder->Create(dialog);

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
