
#ifndef __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOAUTOINITIATIONACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOAUTOINITIATIONACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Action to initiate system audio once AVR is detected on Device discovery action.
 */
// Seq #27
class SystemAudioAutoInitiationAction
    : public HdmiCecFeatureAction
{
public:
    SystemAudioAutoInitiationAction();

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 avrAddress);

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
    CARAPI SendGiveSystemAudioModeStatus();

    CARAPI HandleSystemAudioModeStatusMessage();

    CARAPI HandleSystemAudioModeStatusTimeout();

    CARAPI CanChangeSystemAudio(
        /* [out] */ Boolean* result);

private:
    Int32 mAvrAddress;

    // State that waits for <System Audio Mode Status> once send
    // <Give System Audio Mode Status> to AV Receiver.
    static const Int32 STATE_WAITING_FOR_SYSTEM_AUDIO_MODE_STATUS;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_SYSTEMAUDIOAUTOINITIATIONACTION_H__
