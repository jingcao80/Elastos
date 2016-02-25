
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMIMHLLOCALDEVICESTUB_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMIMHLLOCALDEVICESTUB_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include <Elastos.Droid.Hardware.h>
#include "elastos/droid/server/hdmi/HdmiControlService.h"

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Stub class that models a logical mhl device hosted in this system.
 */
class HdmiMhlLocalDeviceStub
    : public Object
{
public:
    HdmiMhlLocalDeviceStub();

    CARAPI constructor(
        /* [in] */ HdmiControlService* service,
        /* [in] */ Int32 portId);

    CARAPI OnDeviceRemoved();

    CARAPI GetInfo(
        /* [out] */ IHdmiDeviceInfo** result);

    CARAPI SetBusMode(
        /* [in] */ Int32 cbusmode);

    CARAPI OnBusOvercurrentDetected(
        /* [in] */ Boolean on);

    CARAPI SetDeviceStatusChange(
        /* [in] */ Int32 adopterId,
        /* [in] */ Int32 deviceId);

    CARAPI GetPortId(
        /* [out] */ Int32* result);

    CARAPI TurnOn(
        /* [in] */ IIHdmiControlCallback* callback);

    CARAPI SendKeyEvent(
        /* [in] */ Int32 keycode,
        /* [in] */ Boolean isPressed);

private:
    static CARAPI_(AutoPtr<IHdmiDeviceInfo>) InitINFO();

private:
    static const AutoPtr<IHdmiDeviceInfo> INFO;

    AutoPtr<HdmiControlService> mService;

    Int32 mPortId;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMIMHLLOCALDEVICESTUB_H__
