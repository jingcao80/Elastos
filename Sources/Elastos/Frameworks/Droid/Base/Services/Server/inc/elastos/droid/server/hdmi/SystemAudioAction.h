
#ifndef __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Base feature action class for SystemAudioActionFromTv and SystemAudioActionFromAvr.
 */
class SystemAudioAction
    : public HdmiCecFeatureAction
    , public ISystemAudioAction
{
public:
    CAR_INTERFACE_DECL()

    SystemAudioAction();

    /**
     * Constructor
     *
     * @param source {@link HdmiCecLocalDevice} instance
     * @param avrAddress logical address of AVR device
     * @param targetStatus Whether to enable the system audio mode or not
     * @param callback callback interface to be notified when it's done
     * @throw IllegalArugmentException if device type of sourceAddress and avrAddress is invalid
     */
    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress,
        /* [in] */ Boolean targetStatus,
        /* [in] */ IIHdmiControlCallback* callback);

    // Seq #27
    CARAPI SendSystemAudioModeRequest();

    CARAPI SetSystemAudioMode(
        /* [in] */ Boolean mode);

    // @Override
    CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    CARAPI StartAudioStatusAction();

    CARAPI RemoveSystemAudioActionInProgress();

    // @Override
    CARAPI HandleTimerEvent(
        /* [in] */ Int32 state);

    // TODO: if IHdmiControlCallback is general to other FeatureAction,
    //       move it into FeatureAction.
    CARAPI FinishWithCallback(
        /* [in] */ Int32 returnCode);

private:
    CARAPI SendSystemAudioModeRequestInternal();

    CARAPI HandleSendSystemAudioModeRequestTimeout();

public:
    // Logical address of AV Receiver.
    Int32 mAvrLogicalAddress;

    // The target audio status of the action, whether to enable the system audio mode or not.
    Boolean mTargetAudioStatus;

private:
    static const String TAG;

    // Transient state to differentiate with STATE_NONE where the on-finished callback
    // will not be called.
    static const Int32 STATE_CHECK_ROUTING_IN_PRGRESS;

    // State in which waits for <SetSystemAudioMode>.
    static const Int32 STATE_WAIT_FOR_SET_SYSTEM_AUDIO_MODE;

    static const Int32 MAX_SEND_RETRY_COUNT;

    static const Int32 ON_TIMEOUT_MS;

    static const Int32 OFF_TIMEOUT_MS;

    AutoPtr<IIHdmiControlCallback> mCallback;

    Int32 mSendRetryCount;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOACTION_H__
