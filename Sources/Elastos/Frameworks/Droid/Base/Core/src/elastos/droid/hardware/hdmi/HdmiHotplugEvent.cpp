
#include "elastos/droid/hardware/hdmi/HdmiHotplugEvent.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL_2(HdmiHotplugEvent, Object, IHdmiHotplugEvent, IParcelable)

HdmiHotplugEvent::HdmiHotplugEvent()
    : mPort(0)
    , mConnected(FALSE)
{
}

ECode HdmiHotplugEvent::constructor()
{
    return NOERROR;
}

ECode HdmiHotplugEvent::constructor(
    /* [in] */ Int32 port,
    /* [in] */ Boolean connected)
{
    mPort = port;
    mConnected = connected;
    return NOERROR;
}

ECode HdmiHotplugEvent::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    *port = mPort;
    return NOERROR;
}

ECode HdmiHotplugEvent::IsConnected(
    /* [out] */ Boolean* port)
{
    VALIDATE_NOT_NULL(port);

    *port = mConnected;
    return NOERROR;
}

ECode HdmiHotplugEvent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mPort);
    source->ReadBoolean(&mConnected);

    return NOERROR;
}

ECode HdmiHotplugEvent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mPort);
    dest->WriteBoolean(mConnected);

    return NOERROR;
}


} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
