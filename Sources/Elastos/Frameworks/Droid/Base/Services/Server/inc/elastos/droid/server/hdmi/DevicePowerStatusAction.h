
#ifndef __ELASTOS_DROID_SERVER_HDMI_DEVICEPOWERSTATUSACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_DEVICEPOWERSTATUSACTION_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include <Elastos.Droid.Hardware.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClient;
using Elastos::Droid::Hardware::Hdmi::IHdmiPlaybackClientDisplayStatusCallback;
using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;
using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

class HdmiCecLocalDevice;
/**
 * Feature action that queries the power status of other device. This action is initiated via
 * {@link HdmiPlaybackClient#queryDisplayStatus(DisplayStatusCallback)} from the Android system
 * working as playback device to get the power status of TV device.
 * <p>
 * Package-private, accessed by {@link HdmiControlService} only.
 */
class DevicePowerStatusAction
    : public HdmiCecFeatureAction
{
public:
    DevicePowerStatusAction();

    static CARAPI Create(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 targetAddress,
        /* [in] */ IIHdmiControlCallback* callback,
        /* [out] */ DevicePowerStatusAction** result);

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
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* localDevice,
        /* [in] */ Int32 targetAddress,
        /* [in] */ IIHdmiControlCallback* callback);

    CARAPI QueryDevicePowerStatus();

    CARAPI InvokeCallback(
        /* [in] */ Int32 result);

private:
    static const String TAG;

    // State in which the action is waiting for <Report Power Status>.
    static const Int32 STATE_WAITING_FOR_REPORT_POWER_STATUS;

    Int32 mTargetAddress;

    AutoPtr<IIHdmiControlCallback> mCallback;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_DEVICEPOWERSTATUSACTION_H__
