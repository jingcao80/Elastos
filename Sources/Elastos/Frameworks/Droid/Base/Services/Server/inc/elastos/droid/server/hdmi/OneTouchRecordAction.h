
#ifndef __ELASTOS_DROID_SERVER_HDMI_ONETOUCHRECORDACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_ONETOUCHRECORDACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action that performs one touch record.
 */
class OneTouchRecordAction
    : public HdmiCecFeatureAction
{
public:
    OneTouchRecordAction();

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* source,
        /* [in] */ Int32 recorderAddress,
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

    CARAPI GetRecorderAddress(
        /* [out] */ Int32* result);

private:
    CARAPI SendRecordOn();

    CARAPI HandleRecordStatus(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

private:
    static const String TAG;

    // Timer out for waiting <Record Status> 120s
    static const Int32 RECORD_STATUS_TIMEOUT_MS;

    // State that waits for <Record Status> once sending <Record On>
    static const Int32 STATE_WAITING_FOR_RECORD_STATUS;

    // State that describes recording in progress.
    static const Int32 STATE_RECORDING_IN_PROGRESS;

    Int32 mRecorderAddress;

    AutoPtr<ArrayOf<Byte> > mRecordSource;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_ONETOUCHRECORDACTION_H__
