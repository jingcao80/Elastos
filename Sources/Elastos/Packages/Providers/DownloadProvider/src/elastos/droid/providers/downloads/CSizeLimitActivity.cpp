
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/providers/downloads/CSizeLimitActivity.h"
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/providers/downloads/CDownloadInfo.h"
#include "elastos/droid/text/format/Formatter.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

CAR_OBJECT_IMPL(CSizeLimitActivity)

CAR_INTERFACE_IMPL_3(CSizeLimitActivity, Activity, IDialogInterfaceOnCancelListener, IDialogInterfaceOnClickListener, ISizeLimitActivity)

CSizeLimitActivity::CSizeLimitActivity()
{
    CLinkedList::New((IQueue**)&mDownloadsToShow);
}

ECode CSizeLimitActivity::OnNewIntent(
    /* [in] */ IIntent* intent)
{
    Activity::OnNewIntent(intent);
    SetIntent(intent);
    return NOERROR;
}

ECode CSizeLimitActivity::OnResume()
{
    Activity::OnResume();
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        mDownloadsToShow->Add(intent);
        SetIntent(NULL);
        ShowNextDialog();
    }
    Boolean bShow = FALSE;
    if (mDialog != NULL && !(mDialog->IsShowing(&bShow), bShow)) {
        mDialog->Show();
    }
    return NOERROR;
}

void CSizeLimitActivity::ShowNextDialog()
{
    if (mDialog != NULL) {
        return;
    }

    Boolean bIsEmp = FALSE;
    if ((mDownloadsToShow->IsEmpty(&bIsEmp), bIsEmp)) {
        Finish();
        return;
    }

    AutoPtr<IInterface> p;
    mDownloadsToShow->Poll((IInterface**)&p);
    mCurrentIntent = IIntent::Probe(p);
    mCurrentIntent->GetData((IUri**)&mCurrentUri);
    AutoPtr<IContentResolver> cr;
    GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> cursor;
    cr->Query(mCurrentUri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);

    Boolean bMF = FALSE;
    if (!(cursor->MoveToFirst(&bMF), bMF)) {
        Logger::E(Constants::TAG, "Empty cursor for URI %p", mCurrentUri.Get());
        DialogClosed();
        return;
    }
    ShowDialog(cursor);
    ICloseable::Probe(cursor)->Close();
}

void CSizeLimitActivity::ShowDialog(
    /* [in] */ ICursor* cursor)
{
    Int32 index = 0;
    cursor->GetColumnIndexOrThrow(IDownloadsImpl::COLUMN_TOTAL_BYTES, &index);
    Int32 size = 0;
    cursor->GetInt32(index, &size);
    String sizeString = Formatter::FormatFileSize(this, size);
    String queueText;
    GetString(R::string::button_queue_for_wifi, &queueText);
    AutoPtr<IBundle> ext;
    mCurrentIntent->GetExtras((IBundle**)&ext);
    Boolean isWifiRequired = FALSE;
    ext->GetBoolean(CDownloadInfo::EXTRA_IS_WIFI_REQUIRED, &isWifiRequired);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, IAlertDialog::THEME_HOLO_DARK, (IAlertDialogBuilder**)&builder);
    AutoPtr<ICharSequence> pSizeString;
    CString::New(sizeString, (ICharSequence**)&pSizeString);
    AutoPtr<ICharSequence> pQueueText;
    CString::New(queueText, (ICharSequence**)&pQueueText);
    AutoPtr<ArrayOf<IInterface*> > pArr = ArrayOf<IInterface*>::Alloc(2);
    (*pArr)[0] = pSizeString;
    (*pArr)[1] = pQueueText;
    if (isWifiRequired) {
        builder->SetTitle(R::string::wifi_required_title);
        String str;
        GetString(R::string::wifi_required_body, pArr, &str);
        builder->SetMessage(StringUtils::ParseInt32(str));
        builder->SetPositiveButton(R::string::button_queue_for_wifi, this);
        builder->SetNegativeButton(R::string::button_cancel_download, this);
    }
    else {
        builder->SetTitle(R::string::wifi_recommended_title);
        String str;
        GetString(R::string::wifi_recommended_body, pArr, &str);
        builder->SetMessage(StringUtils::ParseInt32(str));
        builder->SetPositiveButton(R::string::button_start_now, this);
        builder->SetNegativeButton(R::string::button_queue_for_wifi, this);
    }
    builder->SetOnCancelListener(this);
    AutoPtr<IAlertDialog> ad;
    builder->Show((IAlertDialog**)&ad);
    mDialog = IDialog::Probe(ad);
}

ECode CSizeLimitActivity::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    DialogClosed();
    return NOERROR;
}

void CSizeLimitActivity::DialogClosed()
{
    mDialog = NULL;
    mCurrentUri = NULL;
    ShowNextDialog();
}

ECode CSizeLimitActivity::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    AutoPtr<IBundle> ext;
    mCurrentIntent->GetExtras((IBundle**)&ext);
    Boolean isRequired = FALSE;
    ext->GetBoolean(CDownloadInfo::EXTRA_IS_WIFI_REQUIRED, &isRequired);
    if (isRequired && which == IDialogInterface::BUTTON_NEGATIVE) {
        AutoPtr<IContentResolver> cr;
        GetContentResolver((IContentResolver**)&cr);
        Int32 val = 0;
        cr->Delete(mCurrentUri, String(NULL), NULL, &val);
    }
    else if (!isRequired && which == IDialogInterface::BUTTON_POSITIVE) {
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        values->Put(IDownloadsImpl::COLUMN_BYPASS_RECOMMENDED_SIZE_LIMIT, TRUE);
        AutoPtr<IContentResolver> cr;
        GetContentResolver((IContentResolver**)&cr);
        Int32 val = 0;
        cr->Update(mCurrentUri, values , String(NULL), NULL, &val);
    }
    DialogClosed();
    return NOERROR;
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
