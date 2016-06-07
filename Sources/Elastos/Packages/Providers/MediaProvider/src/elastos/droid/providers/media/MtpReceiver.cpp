#include "elastos/droid/providers/media/MtpReceiver.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

const String MtpReceiver::TAG(String("UsbReceiver"));

MtpReceiver::MtpReceiver()
{}

MtpReceiver::~MtpReceiver()
{}

CAR_INTERFACE_IMPL(MtpReceiver, BroadcastReceiver, IMtpReceiver)

ECode MtpReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        AutoPtr<IIntent> usbState;
        AutoPtr<IIntentFilter> intentFilter;
        CIntentFilter::New(IUsbManager::ACTION_USB_STATE, (IIntentFilter**)&intentFilter);
        context->RegisterReceiver(NULL, intentFilter.Get(), (IIntent**)&usbState);
        if (usbState != NULL) {
            HandleUsbState(context, usbState);
        }
    } else if (IUsbManager::ACTION_USB_STATE.Equals(action)) {
        HandleUsbState(context, intent);
    }
}

ECode MtpReceiver::HandleUsbState(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    Boolean connected = FALSE;
    extras->GetBoolean(IUsbManager::USB_CONFIGURED, &connected);
    Boolean mtpEnabled = FALSE;
    extras->GetBoolean(IUsbManager::USB_FUNCTION_MTP, &mtpEnabled);
    Boolean ptpEnabled = FALSE;
    extras->GetBoolean(IUsbManager::USB_FUNCTION_PTP, &ptpEnabled);
    // Start MTP service if USB is connected and either the MTP or PTP function is enabled
    AutoPtr<IUri> uri;
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    if (connected && (mtpEnabled || ptpEnabled)) {
        AutoPtr<IIntent> temp;
        assert(0 && "TODO");
        // CIntent::New(context, ECLSID_CMtpService, (IIntent**)&temp);
        if (ptpEnabled) {
            temp->PutBooleanExtra(IUsbManager::USB_FUNCTION_PTP, TRUE);
        }
        AutoPtr<IComponentName> cn;
        context->StartService(temp, (IComponentName**)&cn);
        // tell MediaProvider MTP is connected so it can bind to the service

        uh->Parse(String("content://media/none/mtp_connected"), (IUri**)&uri);
        context->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<IUri> oUri;
        resolver->Insert(uri.Get(), NULL, (IUri**)&oUri);
    }
    else {
        AutoPtr<IIntent> temp;
        assert(0 && "TODO");
        // CIntent::New(context, ECLSID_CMtpService, (IIntent**)&temp);
        Boolean flag = FALSE;
        context->StopService(temp, &flag);
        // tell MediaProvider MTP is disconnected so it can unbind from the service
        uh->Parse(String("content://media/none/mtp_connected"), (IUri**)&uri);
        context->GetContentResolver((IContentResolver**)&resolver);
        Int32 vol;
        resolver->Delete(uri.Get(), String(NULL), NULL, &vol);
    }
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos