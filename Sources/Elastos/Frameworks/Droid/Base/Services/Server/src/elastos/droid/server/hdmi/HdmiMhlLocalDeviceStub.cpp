
#include "elastos/droid/server/hdmi/HdmiMhlLocalDeviceStub.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const AutoPtr<IHdmiDeviceInfo> HdmiMhlLocalDeviceStub::INFO = InitINFO();

HdmiMhlLocalDeviceStub::HdmiMhlLocalDeviceStub()
    : mPortId(0)
{}

ECode HdmiMhlLocalDeviceStub::constructor(
    /* [in] */ HdmiControlService* service,
    /* [in] */ Int32 portId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mService = service;
        mPortId = portId;

#endif
}

ECode HdmiMhlLocalDeviceStub::OnDeviceRemoved()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode HdmiMhlLocalDeviceStub::GetInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return INFO;

#endif
}

ECode HdmiMhlLocalDeviceStub::SetBusMode(
    /* [in] */ Int32 cbusmode)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode HdmiMhlLocalDeviceStub::OnBusOvercurrentDetected(
    /* [in] */ Boolean on)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode HdmiMhlLocalDeviceStub::SetDeviceStatusChange(
    /* [in] */ Int32 adopterId,
    /* [in] */ Int32 deviceId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode HdmiMhlLocalDeviceStub::GetPortId(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mPortId;

#endif
}

ECode HdmiMhlLocalDeviceStub::TurnOn(
    /* [in] */ IIHdmiControlCallback* callback)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

ECode HdmiMhlLocalDeviceStub::SendKeyEvent(
    /* [in] */ Int32 keycode,
    /* [in] */ Boolean isPressed)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below

#endif
}

AutoPtr<IHdmiDeviceInfo> HdmiMhlLocalDeviceStub::InitINFO()
{
    AutoPtr<IHdmiDeviceInfo> rev;
#if 0 // TODO: Translate codes below
    = new HdmiDeviceInfo(
            Constants.INVALID_PHYSICAL_ADDRESS, Constants.INVALID_PORT_ID, -1, -1)
#endif
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
