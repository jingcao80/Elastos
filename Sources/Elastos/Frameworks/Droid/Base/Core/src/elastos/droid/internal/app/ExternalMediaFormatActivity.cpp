
#include "elastos/droid/internal/app/ExternalMediaFormatActivity.h"
#include "elastos/droid/internal/os/storage/CExternalStorageFormatter.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Internal::Os::Storage::CExternalStorageFormatter;
using Elastos::Droid::Internal::Os::Storage::IExternalStorageFormatter;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

ExternalMediaFormatActivity::StorageReceiver::StorageReceiver(
    /* [in] */ ExternalMediaFormatActivity* host)
    : mHost(host)
{}

ECode ExternalMediaFormatActivity::StorageReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Logger::D("ExternalMediaFormatActivity", "got action %s", action.string());

    if (action == IIntent::ACTION_MEDIA_REMOVED ||
        action == IIntent::ACTION_MEDIA_CHECKING ||
        action == IIntent::ACTION_MEDIA_MOUNTED ||
        action == IIntent::ACTION_MEDIA_SHARED) {
        mHost->Finish();
    }
    return NOERROR;
}

ECode ExternalMediaFormatActivity::StorageReceiver::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("StorageReceiver: ");
    (*info).AppendFormat("%p", this);
    return NOERROR;
}

CAR_INTERFACE_IMPL(ExternalMediaFormatActivity::OnClickListener, Object, IDialogInterfaceOnClickListener)

ExternalMediaFormatActivity::OnClickListener::OnClickListener(
    /* [in] */ ExternalMediaFormatActivity* host)
    : mHost(host)
{
}

ECode ExternalMediaFormatActivity::OnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->OnClick(dialog, which);
}

const Int32 ExternalMediaFormatActivity::POSITIVE_BUTTON = IDialogInterface::BUTTON_POSITIVE;

CAR_INTERFACE_IMPL_2(ExternalMediaFormatActivity, AlertActivity, IExternalMediaFormatActivity,
    IDialogInterfaceOnClickListener)

ExternalMediaFormatActivity::ExternalMediaFormatActivity()
{
    mStorageReceiver = new StorageReceiver(this);
}

ECode ExternalMediaFormatActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AlertActivity::OnCreate(savedInstanceState);

    Logger::D("ExternalMediaFormatActivity", "onCreate!");
    // Set up the "dialog"
    AutoPtr<IAlertControllerAlertParams> p = mAlertParams;
    AutoPtr<ICharSequence> charSequence;
    GetText(R::string::extmedia_format_title, (ICharSequence**)&charSequence);
    p->SetTitle(charSequence);
    charSequence = NULL;
    GetText(R::string::extmedia_format_message, (ICharSequence**)&charSequence);
    p->SetMessage(charSequence);
    charSequence = NULL;
    GetText(R::string::extmedia_format_button_format, (ICharSequence**)&charSequence);
    p->SetPositiveButtonText(charSequence);
    AutoPtr<OnClickListener> listener = new OnClickListener(this);
    p->SetPositiveButtonListener(listener);
    charSequence = NULL;
    GetText(R::string::cancel, (ICharSequence**)&charSequence);
    p->SetNegativeButtonText(charSequence);
    p->SetNegativeButtonListener(listener);
    SetupAlert();
    return NOERROR;
}

ECode ExternalMediaFormatActivity::OnResume()
{
    AlertActivity::OnResume();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_MEDIA_REMOVED);
    filter->AddAction(IIntent::ACTION_MEDIA_CHECKING);
    filter->AddAction(IIntent::ACTION_MEDIA_MOUNTED);
    filter->AddAction(IIntent::ACTION_MEDIA_SHARED);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mStorageReceiver, filter, (IIntent**)&intent);
    return NOERROR;
}

ECode ExternalMediaFormatActivity::OnPause()
{
    AlertActivity::OnPause();

    UnregisterReceiver(mStorageReceiver);
    return NOERROR;
}

ECode ExternalMediaFormatActivity::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{

    if (which == POSITIVE_BUTTON) {
        AutoPtr<IIntent> intent;
        CIntent::New(IExternalStorageFormatter::FORMAT_ONLY, (IIntent**)&intent);
        intent->SetComponent(CExternalStorageFormatter::COMPONENT_NAME);
        AutoPtr<IComponentName> component;
        StartService(intent, (IComponentName**)&component);
    }

    // No matter what, finish the activity
    Finish();
    return NOERROR;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
