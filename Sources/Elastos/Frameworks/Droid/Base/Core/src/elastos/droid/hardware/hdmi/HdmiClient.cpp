
#include "elastos/droid/hardware/hdmi/HdmiClient.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL_2(HdmiClient::MyListener, Object, IIHdmiVendorCommandListener, IBinder)

HdmiClient::MyListener::MyListener(
    /* [in] */ IHdmiControlManagerVendorCommandListener* listener)
    : mListener(listener)
{
}

HdmiClient::MyListener::OnReceived(
    /* [in] */ Int32 srcAddress,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Boolean hasVendorId)
{
    return mListener->OnReceived(srcAddress, params, hasVendorId);
}

ECode HdmiClient::MyListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

const String HdmiClient::TAG("HdmiClient");

CAR_INTERFACE_IMPL(HdmiClient, Object, IHdmiClient)

HdmiClient::HdmiClient()
{
}

ECode HdmiClient::constructor()
{
    return NOERROR;
}

ECode HdmiClient::constructor(
    /* [in] */ IIHdmiControlService* service)
{
    mService = service;
    return NOERROR;
}

ECode HdmiClient::GetActiveSource(
    /* [out] */ IHdmiDeviceInfo** source)
{
    VALIDATE_NOT_NULL(source);
    *source = NULL;

    //try {
    ECode ec = mService->GetActiveSource(source);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "getActiveSource threw exception %d", ec);
        *source = NULL;
    }
    //}
    return NOERROR;
}

ECode HdmiClient::SendKeyEvent(
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isPressed)
{
    //try {
    Int32 type;
    GetDeviceType(&type);
    ECode ec = mService->SendKeyEvent(type, keyCode, isPressed);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "sendKeyEvent threw exception %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiClient::SendVendorCommand(
    /* [in] */ Int32 targetAddress,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Boolean hasVendorId)
{
    //try {
    Int32 type;
    GetDeviceType(&type);
    ECode ec = mService->SendVendorCommand(type, targetAddress, params, hasVendorId);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "failed to send vendor command: %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiClient::AddVendorCommandListener(
    /* [in] */ IHdmiControlManagerVendorCommandListener* listener)
{
    if (listener == NULL) {
        //throw new IllegalArgumentException("listener cannot be null");
        Slogger::E(TAG, "listener cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //try {
    Int32 type;
    GetDeviceType(&type);
    ECode ec = mService->AddVendorCommandListener(GetListenerWrapper(listener), type);
    //} catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "failed to add vendor command listener: %d", ec);
    }
    //}
    return NOERROR;
}

AutoPtr<IIHdmiVendorCommandListener> HdmiClient::GetListenerWrapper(
    /* [in] */ IHdmiControlManagerVendorCommandListener* listener)
{
    AutoPtr<IIHdmiVendorCommandListener> lisen = new MyListener(listener);
    return lisen;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
