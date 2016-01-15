#include "elastos/droid/speech/tts/SilencePlaybackQueueItem.h"
//#include "elastos/droid/os/CConditionVariable.h"

using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CAR_INTERFACE_IMPL(SilencePlaybackQueueItem, Object, IRunnable);

SilencePlaybackQueueItem::SilencePlaybackQueueItem()
{}

SilencePlaybackQueueItem::~SilencePlaybackQueueItem()
{}

ECode SilencePlaybackQueueItem::constructor()
{
    return NOERROR;
}

ECode SilencePlaybackQueueItem::constructor(
    /* [in] */ IUtteranceProgressDispatcher* dispatcher,
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ Int64 silenceDurationMs)
{
    mSilenceDurationMs = silenceDurationMs;
//    CConditionVariable::New((IConditionVariable**)&mCondVar);
    PlaybackQueueItem::constructor(dispatcher, callerIdentity);

    return NOERROR;
}

ECode SilencePlaybackQueueItem::Run()
{
    GetDispatcher()->DispatchOnStart();
    Boolean wasStopped = FALSE;
    if (mSilenceDurationMs > 0) {
//      wasStopped = mCondVar->block(mSilenceDurationMs);
    }
    if (wasStopped) {
        GetDispatcher()->DispatchOnStop();
    } else {
        GetDispatcher()->DispatchOnSuccess();
    }

    return NOERROR;
}

ECode SilencePlaybackQueueItem::Stop(
    /* [in] */ Int32 errorCode)
{
//    mCondVar->Open();
    return NOERROR;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
