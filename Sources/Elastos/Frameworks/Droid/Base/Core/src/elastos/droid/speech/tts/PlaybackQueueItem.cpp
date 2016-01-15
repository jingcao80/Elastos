#include "elastos/droid/speech/tts/PlaybackQueueItem.h"

using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CAR_INTERFACE_IMPL(PlaybackQueueItem, Object, IRunnable);

PlaybackQueueItem::PlaybackQueueItem()
{}

PlaybackQueueItem::~PlaybackQueueItem()
{}

ECode PlaybackQueueItem::constructor(
    /* [in] */ IUtteranceProgressDispatcher* dispatcher,
    /* [in] */ IInterface* callerIdentity)
{
    mDispatcher = dispatcher;
    mCallerIdentity = callerIdentity;

    return NOERROR;
}

AutoPtr<IInterface> PlaybackQueueItem::GetCallerIdentity()
{
    return mCallerIdentity;
}

AutoPtr<IUtteranceProgressDispatcher> PlaybackQueueItem::GetDispatcher()
{
    return mDispatcher;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
