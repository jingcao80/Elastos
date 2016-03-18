
#include "elastos/droid/server/hdmi/Constants.h"
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
    /* [in] */ IHdmiControlService* service,
    /* [in] */ Int32 portId)
{
    mService = service;
    mPortId = portId;
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::OnDeviceRemoved()
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::GetInfo(
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)

    *result = INFO;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::SetBusMode(
    /* [in] */ Int32 cbusmode)
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::OnBusOvercurrentDetected(
    /* [in] */ Boolean on)
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::SetDeviceStatusChange(
    /* [in] */ Int32 adopterId,
    /* [in] */ Int32 deviceId)
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::GetPortId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPortId;
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::TurnOn(
    /* [in] */ IIHdmiControlCallback* callback)
{
    return NOERROR;
}

ECode HdmiMhlLocalDeviceStub::SendKeyEvent(
    /* [in] */ Int32 keycode,
    /* [in] */ Boolean isPressed)
{
    return NOERROR;
}

AutoPtr<IHdmiDeviceInfo> HdmiMhlLocalDeviceStub::InitINFO()
{
    AutoPtr<IHdmiDeviceInfo> rev;
    // TODO: Waiting for CHdmiDeviceInfo
    // assert(0);
    // CHdmiDeviceInfo::New(Constants::INVALID_PHYSICAL_ADDRESS, Constants::INVALID_PORT_ID, -1, -1, (IHdmiDeviceInfo**)&rev);
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
