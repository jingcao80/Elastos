
#ifndef __ELASTOS_DROID_SERVER_HDMI_HOTPLUGDETECTIONACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_HOTPLUGDETECTIONACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

using Elastos::Utility::IBitSet;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that handles hot-plug detection mechanism.
 * Hot-plug event is initiated by timer after device discovery action.
 *
 * <p>Check all devices every 15 secs except for system audio.
 * If system audio is on, check hot-plug for audio system every 5 secs.
 * For other devices, keep 15 secs period.
 */
// Seq #3
class HotplugDetectionAction
    : public HdmiCecFeatureAction
    , public IHotplugDetectionAction
{
private:
    class AllDevicesDevicePollingCallback
        : public Object
        , public IHdmiControlServiceDevicePollingCallback
    {
    public:
        CAR_INTERFACE_DECL()

        AllDevicesDevicePollingCallback(
            /* [in] */ HotplugDetectionAction* host);

        //@Override
        CARAPI OnPollingFinished(
            /* [in] */ IList* ackedAddress);

    private:
        HotplugDetectionAction* mHost;
    };

    class AudioSystemDevicePollingCallback
        : public Object
        , public IHdmiControlServiceDevicePollingCallback
    {
    public:
        CAR_INTERFACE_DECL()

        AudioSystemDevicePollingCallback(
            /* [in] */ HotplugDetectionAction* host);

        //@Override
        CARAPI OnPollingFinished(
            /* [in] */ IList* ackedAddress);

    private:
        HotplugDetectionAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    HotplugDetectionAction();

    /**
     * Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source);

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

    /**
     * Start device polling immediately.
     */
    CARAPI PollAllDevicesNow();

private:
    // This method is called every 5 seconds.
    CARAPI PollDevices();

    CARAPI PollAllDevices();

    CARAPI PollAudioSystem();

    CARAPI CheckHotplug(
        /* [in] */ IList* ackedAddress,
        /* [in] */ Boolean audioOnly);

    static CARAPI_(AutoPtr<IBitSet>) InfoListToBitSet(
        /* [in] */ IList* infoList,
        /* [in] */ Boolean audioOnly);

    static CARAPI_(AutoPtr<IBitSet>) AddressListToBitSet(
        /* [in] */ IList* list);

    // A - B = A & ~B
    static CARAPI_(AutoPtr<IBitSet>) Complement(
        /* [in] */ IBitSet* first,
        /* [in] */ IBitSet* second);

    CARAPI AddDevice(
        /* [in] */ Int32 addedAddress);

    CARAPI RemoveDevice(
        /* [in] */ Int32 removedAddress);

    CARAPI MayChangeRoutingPath(
        /* [in] */ Int32 address);

    CARAPI MayCancelDeviceSelect(
        /* [in] */ Int32 address);

    CARAPI MayCancelOneTouchRecord(
        /* [in] */ Int32 address);

    CARAPI MayDisableSystemAudioAndARC(
        /* [in] */ Int32 address);

private:
    static const String TAG;

    static const Int32 POLLING_INTERVAL_MS;

    static const Int32 TIMEOUT_COUNT;

    // State in which waits for next polling
    static const Int32 STATE_WAIT_FOR_NEXT_POLLING;

    // All addresses except for broadcast (unregistered address).
    static const Int32 NUM_OF_ADDRESS;

    Int32 mTimeoutCount;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HOTPLUGDETECTIONACTION_H__
