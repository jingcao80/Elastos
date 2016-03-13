
#ifndef __ELASTOS_DROID_SERVER_HDMI_TIMERRECORDINGACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_TIMERRECORDINGACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that performs timer recording.
 */
class TimerRecordingAction
    : public HdmiCecFeatureAction
    , public ITimerRecordingAction
{
public:
    CAR_INTERFACE_DECL()

    TimerRecordingAction();

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 recorderAddress,
        /* [in] */ Int32 sourceType,
        /* [in] */ ArrayOf<Byte>* recordSource);

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
    CARAPI SendTimerMessage();

    CARAPI HandleTimerStatus(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    CARAPI HandleFeatureAbort(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    // Convert byte array to int.
    static CARAPI BytesToInt32(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Int32* result);

private:
    static const String TAG;

    // Timer out for waiting <Timer Status> 120s.
    static const Int32 TIMER_STATUS_TIMEOUT_MS;

    // State that waits for <Timer Status> once sending <Set XXX Timer>
    static const Int32 STATE_WAITING_FOR_TIMER_STATUS;

    Int32 mRecorderAddress;

    Int32 mSourceType;

    AutoPtr<ArrayOf<Byte> > mRecordSource;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_TIMERRECORDINGACTION_H__
