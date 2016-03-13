
#ifndef __ELASTOS_DROID_SERVER_HDMI_DEVICESELECTACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_DEVICESELECTACTION_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include <Elastos.Droid.Hardware.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiTvClient;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Server::Hdmi::IHdmiControlServiceSendMessageCallback;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecLocalDeviceTv;
/**
 * Handles an action that selects a logical device as a new active source.
 *
 * Triggered by {@link HdmiTvClient}, attempts to select the given target device
 * for a new active source. It does its best to wake up the target in standby mode
 * before issuing the command &gt;Set Stream path&lt;.
 */
class DeviceSelectAction
    : public HdmiCecFeatureAction
    , public IDeviceSelectAction
{
private:
    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ DeviceSelectAction* host);

        // @Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        DeviceSelectAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DeviceSelectAction();

    /**
     * Constructor.
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param target target logical device that will be a new active source
     * @param callback callback object
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDeviceTv* source,
        /* [in] */ IHdmiDeviceInfo* target,
        /* [in] */ IIHdmiControlCallback* callback);

    CARAPI GetTargetAddress(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Start(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleTimerEvent(
        /* [in] */ Int32 timeoutState);

private:
    CARAPI QueryDevicePowerStatus();

    CARAPI HandleReportPowerStatus(
        /* [in] */ Int32 powerStatus,
        /* [out] */ Boolean* result);

    CARAPI TurnOnDevice();

    CARAPI SendSetStreamPath();

    CARAPI InvokeCallback(
        /* [in] */ Int32 result);

private:
    static const String TAG;

    // Time in milliseconds we wait for the device power status to switch to 'Standby'
    static const Int32 TIMEOUT_TRANSIT_TO_STANDBY_MS;

    // Time in milliseconds we wait for the device power status to turn to 'On'.
    static const Int32 TIMEOUT_POWER_ON_MS;

    // and just send <Set Stream Path>.
    static const Int32 LOOP_COUNTER_MAX;

    // <Give Device Power Status> we have sent.
    static const Int32 STATE_WAIT_FOR_REPORT_POWER_STATUS;

    // to wake up the device again.
    static const Int32 STATE_WAIT_FOR_DEVICE_TO_TRANSIT_TO_STANDBY;

    // maximum 100 seconds (20 * 5) before we give up and just send <Set Stream Path>.
    static const Int32 STATE_WAIT_FOR_DEVICE_POWER_ON;

    AutoPtr<IHdmiDeviceInfo> mTarget;

    AutoPtr<IIHdmiControlCallback> mCallback;

    AutoPtr<IHdmiCecMessage> mGivePowerStatus;

    Int32 mPowerStatusCounter;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_DEVICESELECTACTION_H__
