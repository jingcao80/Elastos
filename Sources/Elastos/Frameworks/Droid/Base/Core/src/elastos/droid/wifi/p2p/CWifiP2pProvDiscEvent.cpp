#include "elastos/droid/wifi/p2p/CWifiP2pProvDiscEvent.h"
#include "elastos/droid/wifi/p2p/CWifiP2pDevice.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

const String TAG("WifiP2pProvDiscEvent");

CAR_INTERFACE_IMPL(CWifiP2pProvDiscEvent, Object, IWifiP2pProvDiscEvent)

CAR_OBJECT_IMPL(CWifiP2pProvDiscEvent)

CWifiP2pProvDiscEvent::CWifiP2pProvDiscEvent()
    : mEvent(0)
{
}

CWifiP2pProvDiscEvent::~CWifiP2pProvDiscEvent()
{
}

ECode CWifiP2pProvDiscEvent::constructor()
{
    return CWifiP2pDevice::New((IWifiP2pDevice**)&mDevice);
}

ECode CWifiP2pProvDiscEvent::constructor(
    /* [in] */ const String& string)
{
    AutoPtr< ArrayOf<String> > tokens;
    StringUtils::Split(string, String(" "), (ArrayOf<String>**)&tokens);

    if (tokens == NULL || tokens->GetLength() < 2) {
        Slogger::D("CWifiP2pProvDiscEvent", "E_ILLEGAL_ARGUMENT_EXCEPTION: Malformed mEvent %s",
            string.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((*tokens)[0].EndWith("PBC-REQ")) mEvent = PBC_REQ;
    else if ((*tokens)[0].EndWith("PBC-RESP")) mEvent = PBC_RSP;
    else if ((*tokens)[0].EndWith("ENTER-PIN")) mEvent = ENTER_PIN;
    else if ((*tokens)[0].EndWith("SHOW-PIN")) mEvent = SHOW_PIN;
    else {
        Slogger::D("CWifiP2pProvDiscEvent", "E_ILLEGAL_ARGUMENT_EXCEPTION: Malformed mEvent %s",
            string.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CWifiP2pDevice::New((IWifiP2pDevice**)&mDevice);
    mDevice->SetDeviceAddress((*tokens)[1]);

    if (mEvent == SHOW_PIN) {
        mPin = (*tokens)[2];
    }
    return NOERROR;
}

ECode CWifiP2pProvDiscEvent::GetEvent(
    /* [out] */ Int32* event)
{
    VALIDATE_NOT_NULL(event);
    *event = mEvent;
    return NOERROR;
}

ECode CWifiP2pProvDiscEvent::SetEvent(
    /* [in] */ Int32 event)
{
    mEvent = event;
    return NOERROR;
}

ECode CWifiP2pProvDiscEvent::GetDevice(
    /* [out] */ IWifiP2pDevice** device)
{
    VALIDATE_NOT_NULL(device);
    *device = mDevice;
    REFCOUNT_ADD(*device);
    return NOERROR;
}

ECode CWifiP2pProvDiscEvent::SetDevice(
    /* [in] */ IWifiP2pDevice* device)
{
    mDevice = device;
    return NOERROR;
}

ECode CWifiP2pProvDiscEvent::GetPin(
    /* [out] */ String* pin)
{
    VALIDATE_NOT_NULL(pin);
    *pin = mPin;
    return NOERROR;
}

ECode CWifiP2pProvDiscEvent::SetPin(
    /* [in] */ const String& pin)
{
    mPin = pin;
    return NOERROR;
}

ECode CWifiP2pProvDiscEvent::ToString(
    /* [out] */ String* string)
{
    StringBuilder sbuf;
    sbuf += "\n event: ";
    sbuf += mEvent;
    sbuf += "\n pin: ";
    sbuf += mPin;
    sbuf.ToString(string);
    return NOERROR;
}

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
