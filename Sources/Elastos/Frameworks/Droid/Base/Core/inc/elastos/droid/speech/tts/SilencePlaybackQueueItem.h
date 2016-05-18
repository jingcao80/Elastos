#ifndef __ELASTOS_DROID_SPEECH_TTS_SilencePlaybackQueueItem_H__
#define __ELASTOS_DROID_SPEECH_TTS_SilencePlaybackQueueItem_H__

#include "elastos/droid/speech/tts/PlaybackQueueItem.h"
#include "elastos/droid/os/ConditionVariable.h"
//
namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

class SilencePlaybackQueueItem
    : public PlaybackQueueItem
{
public:
    CAR_INTERFACE_DECL()

    SilencePlaybackQueueItem();

    virtual ~SilencePlaybackQueueItem();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUtteranceProgressDispatcher* dispatcher,
        /* [in] */ IInterface* callerIdentity,
        /* [in] */ Int64 silenceDurationMs);

    //@Override
    CARAPI Run();

    //@Override
    CARAPI Stop(
        /* [in] */ Int32 errorCode);

private:
//    AutoPtr<IConditionVariable> mCondVar;// = new ConditionVariable();
    Int64 mSilenceDurationMs;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_SilencePlaybackQueueItem_H__
