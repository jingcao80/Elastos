
#ifndef __ELASTOS_DROID_SERVER_HDMI_DEVICEDISCOVERYACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_DEVICEDISCOVERYACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Utility::ISlog;

using Elastos::Droid::Internal::Utility::IPreconditions;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that handles device discovery sequences.
 * Device discovery is launched when TV device is woken from "Standby" state
 * or enabled "Control for Hdmi" from disabled state.
 *
 * <p>Device discovery goes through the following steps.
 * <ol>
 *   <li>Poll all non-local devices by sending &lt;Polling Message&gt;
 *   <li>Gather "Physical address" and "device type" of all acknowledged devices
 *   <li>Gather "OSD (display) name" of all acknowledge devices
 *   <li>Gather "Vendor id" of all acknowledge devices
 * </ol>
 */
class DeviceDiscoveryAction
#if 0
    : public HdmiCecFeatureAction
#else
    : public Object
#endif
{
private:
    // An internal container used to keep track of device information during
    // this action.
    class DeviceInfo
        : public Object
    {
    private:
        DeviceInfo(
            /* [in] */ Int32 logicalAddress);

        CARAPI ToHdmiDeviceInfo(
            /* [out] */ IHdmiDeviceInfo** result);

    private:
        const Int32 mLogicalAddress;

        Int32 mPhysicalAddress;

        Int32 mPortId;

        Int32 mVendorId;

        String mDisplayName;

        Int32 mDeviceType;
    };

public:
    DeviceDiscoveryAction();

    /**
     * Constructor.
     *
     * @param source an instance of {@link HdmiCecLocalDevice}.
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ IDeviceDiscoveryActionDeviceDiscoveryCallback* callback);

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

private:
    CARAPI AllocateDevices(
        /* [in] */ IList* addresses);

    CARAPI StartPhysicalAddressStage();

    CARAPI VerifyValidLogicalAddress(
        /* [in] */ Int32 address,
        /* [out] */ Boolean* result);

    CARAPI QueryPhysicalAddress(
        /* [in] */ Int32 address);

    CARAPI StartOsdNameStage();

    CARAPI QueryOsdName(
        /* [in] */ Int32 address);

    CARAPI StartVendorIdStage();

    CARAPI QueryVendorId(
        /* [in] */ Int32 address);

    CARAPI MayProcessMessageIfCached(
        /* [in] */ Int32 address,
        /* [in] */ Int32 opcode,
        /* [out] */ Boolean* result);

    CARAPI HandleReportPhysicalAddress(
        /* [in] */ IHdmiCecMessage* cmd);

    CARAPI GetPortId(
        /* [in] */ Int32 physicalAddress,
        /* [out] */ Int32* result);

    CARAPI HandleSetOsdName(
        /* [in] */ IHdmiCecMessage* cmd);

    CARAPI HandleVendorId(
        /* [in] */ IHdmiCecMessage* cmd);

    CARAPI IncreaseProcessedDeviceCount();

    CARAPI RemoveDevice(
        /* [in] */ Int32 index);

    CARAPI WrapUpAndFinish();

    CARAPI CheckAndProceedStage();

private:
    static const String TAG;

    // State in which the action is waiting for device polling.
    static const Int32 STATE_WAITING_FOR_DEVICE_POLLING;

    // State in which the action is waiting for gathering physical address of non-local devices.
    static const Int32 STATE_WAITING_FOR_PHYSICAL_ADDRESS;

    // State in which the action is waiting for gathering osd name of non-local devices.
    static const Int32 STATE_WAITING_FOR_OSD_NAME;

    // State in which the action is waiting for gathering vendor id of non-local devices.
    static const Int32 STATE_WAITING_FOR_VENDOR_ID;

    AutoPtr<IArrayList> mDevices;

    AutoPtr<IDeviceDiscoveryActionDeviceDiscoveryCallback> mCallback;

    Int32 mProcessedDeviceCount;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_DEVICEDISCOVERYACTION_H__
