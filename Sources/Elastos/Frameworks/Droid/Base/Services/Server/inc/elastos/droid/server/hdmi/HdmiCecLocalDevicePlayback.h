
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMICECLOCALDEVICEPLAYBACK_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMICECLOCALDEVICEPLAYBACK_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecLocalDevice.h"

using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiControlService;
/**
 * Represent a logical device of type Playback residing in Android system.
 */
class HdmiCecLocalDevicePlayback
    : public HdmiCecLocalDevice
{
public:
    HdmiCecLocalDevicePlayback();

    CARAPI constructor(
        /* [in] */ IHdmiControlService* service);

    // @Override
    CARAPI Init();

    // @Override
    // @ServiceThreadOnly
    CARAPI OnAddressAllocated(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 reason);

    // @Override
    // @ServiceThreadOnly
    CARAPI GetPreferredAddress(
        /* [out] */ Int32* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI SetPreferredAddress(
        /* [in] */ Int32 addr);

    // @ServiceThreadOnly
    CARAPI OneTouchPlay(
        /* [in] */ IIHdmiControlCallback* callback);

    // @ServiceThreadOnly
    CARAPI QueryDisplayStatus(
        /* [in] */ IIHdmiControlCallback* callback);

    // @Override
    // @ServiceThreadOnly
    CARAPI OnHotplug(
        /* [in] */ Int32 portId,
        /* [in] */ Boolean connected);

    // @ServiceThreadOnly
    CARAPI MarkActiveSource();

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleActiveSource(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleSetStreamPath(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // Samsung model, we tested, sends <RoutingChange> and <RequestActiveSource> consecutively,
    // Then if there is no <ActiveSource> response, it will change the input to
    // the internal source.  To handle this, we'll set ActiveSource aggressively.
    // @Override
    // @ServiceThreadOnly
    CARAPI HandleRoutingChange(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleRoutingInformation(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI HandleRequestActiveSource(
        /* [in] */ IHdmiCecMessage* message,
        /* [out] */ Boolean* result);

    // @Override
    // @ServiceThreadOnly
    CARAPI DisableDevice(
        /* [in] */ Boolean initiatedByCec,
        /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* callback);

    // @Override
    CARAPI Dump(
        /* [in] */ IIndentingPrintWriter* pw);

private:
    // @ServiceThreadOnly
    CARAPI InvokeCallback(
        /* [in] */ IIHdmiControlCallback* callback,
        /* [in] */ Int32 result);

    CARAPI MayResetActiveSource(
        /* [in] */ Int32 physicalAddress);

    CARAPI MaySetActiveSource(
        /* [in] */ Int32 physicalAddress);

    CARAPI WakeUpIfActiveSource();

    CARAPI MaySendActiveSource();

private:
    static const String TAG;

    Boolean mIsActiveSource;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMICECLOCALDEVICEPLAYBACK_H__
