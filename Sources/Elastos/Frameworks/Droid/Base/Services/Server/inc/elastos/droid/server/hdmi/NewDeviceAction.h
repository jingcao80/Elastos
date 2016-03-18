
#ifndef __ELASTOS_DROID_SERVER_HDMI_NEWDEVICEACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_NEWDEVICEACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevice.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that discovers the information of a newly found logical device.
 *
 * This action is created when receiving &lt;Report Physical Address&gt;, a CEC command a newly
 * connected HDMI-CEC device broadcasts to announce its advent. Additional commands are issued in
 * this action to gather more information on the device such as OSD name and device vendor ID.
 *
 * <p>The result is made in the form of {@link HdmiDeviceInfo} object, and passed to service
 * for the management through its life cycle.
 *
 * <p>Package-private, accessed by {@link HdmiControlService} only.
 */
class NewDeviceAction
    : public HdmiCecFeatureAction
    , public INewDeviceAction
{
public:
    CAR_INTERFACE_DECL()

    NewDeviceAction();

    /**
     * Constructor.
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param deviceLogicalAddress logical address of the device in interest
     * @param devicePhysicalAddress physical address of the device in interest
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 deviceLogicalAddress,
        /* [in] */ Int32 devicePhysicalAddress);

    // @Override
    CARAPI Start(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleTimerEvent(
        /* [in] */ Int32 state);

    CARAPI IsActionOf(
        /* [in] */ IHdmiCecLocalDeviceActiveSource* activeSource,
        /* [out] */ Boolean* result);

    // that contains the name of the device for display on screen.
    static const Int32 STATE_WAITING_FOR_SET_OSD_NAME;

    // <Device Vendor ID> that contains the vendor ID of the device.
    static const Int32 STATE_WAITING_FOR_DEVICE_VENDOR_ID;

private:
    CARAPI_(Boolean) MayProcessCommandIfCached(
        /* [in] */ Int32 destAddress,
        /* [in] */ Int32 opcode);

    CARAPI RequestVendorId();

    CARAPI AddDeviceInfo();

private:
    static const String TAG;

    Int32 mDeviceLogicalAddress;

    Int32 mDevicePhysicalAddress;

    Int32 mVendorId;

    String mDisplayName;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_NEWDEVICEACTION_H__
