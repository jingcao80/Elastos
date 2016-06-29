
#include "elastos/apps/dialer/calllog/ClearCallLogDialog.h"
#include "elastos/apps/dialer/calllog/CClearCallLogDialog.h"
#include "elastos/apps/dialerbind/CObjectFactory.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/droid/R.h>
#include "R.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IProgressDialogHelper;
using Elastos::Droid::App::CProgressDialogHelper;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::CCalls;
using Elastos::Apps::DialerBind::CObjectFactory;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

//=================================================================
// ClearCallLogDialog::OKClickListener
//=================================================================
CAR_INTERFACE_IMPL(ClearCallLogDialog::OKClickListener, Object, IDialogInterfaceOnClickListener);

ClearCallLogDialog::OKClickListener::OKClickListener(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ ClearCallLogDialog* host)
    : mResolver(resolver)
    , mHost(host)
{}

ECode ClearCallLogDialog::OKClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IProgressDialogHelper> helper;
    CProgressDialogHelper::AcquireSingleton((IProgressDialogHelper**)&helper);
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    String str;
    mHost->GetString(R::string::clearCallLogProgress_title, &str);
    AutoPtr<IProgressDialog> progressDialog;
    helper->Show(IContext::Probe(activity), CoreUtils::Convert(str),
            CoreUtils::Convert(String("")), TRUE, FALSE, (IProgressDialog**)&progressDialog);
    AutoPtr<MyAsyncTask> task = new MyAsyncTask(
            IContext::Probe(activity), mResolver, progressDialog, mHost);
    // TODO: Once we have the API, we should configure this ProgressDialog
    // to only show up after a certain time (e.g. 150ms)
    IDialog::Probe(progressDialog)->Show();
    task->Execute((ArrayOf<IInterface*>*)NULL);

    return NOERROR;
}

//=================================================================
// ClearCallLogDialog::MyAsyncTask
//=================================================================
ClearCallLogDialog::MyAsyncTask::MyAsyncTask(
    /* [in] */ IContext* context,
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IProgressDialog* progressDialog,
    /* [in] */ ClearCallLogDialog* host)
    : mContext(context)
    , mResolver(resolver)
    , mProgressDialog(progressDialog)
    , mHost(host)
{}

ECode ClearCallLogDialog::MyAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    AutoPtr<ICalls> calls;
    CCalls::AcquireSingleton((ICalls**)&calls);
    AutoPtr<IUri> uri;
    calls->GetCONTENT_URI((IUri**)&uri);
    Int32 rowsAffected;
    mResolver->Delete(uri, String(NULL), NULL, &rowsAffected);
    if (mHost->mCachedNumberLookupService != NULL) {
        mHost->mCachedNumberLookupService->ClearAllCacheEntries(mContext);
    }
    *obj = NULL;
    return NOERROR;
}

ECode ClearCallLogDialog::MyAsyncTask::OnPostExecute(
    /* [in] */ IInterface* info)
{
    IDialogInterface::Probe(mProgressDialog)->Dismiss();
    return NOERROR;
}

//=================================================================
// ClearCallLogDialog
//=================================================================
CAR_INTERFACE_IMPL(ClearCallLogDialog, DialogFragment, IClearCallLogDialog);

AutoPtr<ICachedNumberLookupService> ClearCallLogDialog::mCachedNumberLookupService =
        CObjectFactory::NewCachedNumberLookupService();

ClearCallLogDialog::ClearCallLogDialog()
{}

void ClearCallLogDialog::Show(
    /* [in] */ IFragmentManager* fragmentManager)
{
    AutoPtr<IClearCallLogDialog> dialog;
    CClearCallLogDialog::New((IClearCallLogDialog**)&dialog);
    IDialogFragment::Probe(dialog)->Show(fragmentManager, String("deleteCallLog"));
}

ECode ClearCallLogDialog::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IContext> context;
    IContext::Probe(activity)->GetApplicationContext((IContext**)&context);

    AutoPtr<OKClickListener> okListener = new OKClickListener(resolver, this);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(activity), (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R::string::clearCallLogConfirmation_title);
    builder->SetIconAttribute(Elastos::Droid::R::attr::alertDialogIcon);
    builder->SetMessage(R::string::clearCallLogConfirmation);
    builder->SetNegativeButton(Elastos::Droid::R::string::cancel, NULL);
    builder->SetPositiveButton(Elastos::Droid::R::string::ok, okListener);
    builder->SetCancelable(TRUE);
    AutoPtr<IAlertDialog> alertDialog;
    builder->Create((IAlertDialog**)&alertDialog);
    *dialog = IDialog::Probe(alertDialog);
    REFCOUNT_ADD(*dialog);

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
