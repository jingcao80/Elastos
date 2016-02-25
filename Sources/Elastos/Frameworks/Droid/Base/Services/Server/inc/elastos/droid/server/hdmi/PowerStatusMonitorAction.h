
#ifndef __ELASTOS_DROID_SERVER_HDMI_POWERSTATUSMONITORACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_POWERSTATUSMONITORACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

using Elastos::Utility::IList;
using Elastos::Droid::Utility::ISparseInt32Array;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Action that check each device's power status.
 */
class PowerStatusMonitorAction
    : public HdmiCecFeatureAction
{
public:
    PowerStatusMonitorAction();

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

private:
    CARAPI HandleReportPowerStatus(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    CARAPI HandleTimeout();

    CARAPI ResetPowerStatus(
        /* [in] */ IList* deviceInfos);

    CARAPI QueryPowerStatus();

    CARAPI UpdatePowerStatus(
        /* [in] */ Int32 logicalAddress,
        /* [in] */ Int32 newStatus,
        /* [in] */ Boolean remove);

private:
    static const String TAG;

    // State that waits for <Report Power Status> once sending <Give Device Power Status>
    // to all external devices.
    static const Int32 STATE_WAIT_FOR_REPORT_POWER_STATUS;
    // State that waits for next monitoring
    static const Int32 STATE_WAIT_FOR_NEXT_MONITORING;

    static const Int32 INVALID_POWER_STATUS;

    // Monitoring interval (60s)
    static const Int32 MONITIROING_INTERNAL_MS;

    // Timeout once sending <Give Device Power Status>
    static const Int32 REPORT_POWER_STATUS_TIMEOUT_MS;

    // Container for current power status of all external devices.
    // The key is a logical address a device and the value is current power status of it
    // Whenever the action receives <Report Power Status> from a device,
    // it removes an entry of the given device.
    // If this is non-empty when timeout for STATE_WAIT_FOR_REPORT_POWER_STATUS happens,
    // updates power status of all remaining devices into POWER_STATUS_UNKNOWN.
    AutoPtr<ISparseInt32Array> mPowerStatus;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_POWERSTATUSMONITORACTION_H__
