#ifndef __ELASTOS_DROID_SPEECH_TTS_AudioPlaybackHandler_H__
#define __ELASTOS_DROID_SPEECH_TTS_AudioPlaybackHandler_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "elastos/droid/speech/tts/PlaybackQueueItem.h"
#include <elastos/utility/etl/List.h>
//#include <elastos/Queue.h>//#include <elastos/BlockingQueue.h>

using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;
//using Elastos::Utility::Queue;//using Elastos::Utility::BlockingQueue;
using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

class AudioPlaybackHandler
    : public Object
{
private:
    /*
     * The MessageLoop is a handler like implementation that
     * processes messages from a priority queue.
     */
    class AudioPlaybackHandlerMessageLoop
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        AudioPlaybackHandlerMessageLoop();

        virtual ~AudioPlaybackHandlerMessageLoop();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ AudioPlaybackHandler* aph);
    public:
        //@Override
        CARAPI Run();

    private:
        AutoPtr<AudioPlaybackHandler> mAph;
    };

public:
    AudioPlaybackHandler();

    CARAPI_(void) Start();

    CARAPI_(void) Stop();

    CARAPI_(void) Enqueue(
        /* [in] */ PlaybackQueueItem* item);

    CARAPI_(void) StopForApp(
        /* [in] */ IInterface* callerIdentity);

    /**
     * @return false iff the queue is empty and no queue item is currently
     *        being handled, true otherwise.
     */
    CARAPI_(Boolean) IsSpeaking();

    /**
     * Shut down the audio playback thread.
     */
    CARAPI_(void) Quit();

private:
    CARAPI_(void) Stop(
        /* [in] */ PlaybackQueueItem* item);

    /*
     * Atomically clear the queue of all messages.
     */
    CARAPI_(void) RemoveAllMessages();

    /*
     * Remove all messages that originate from a given calling app.
     */
    CARAPI_(void) RemoveWorkItemsFor(
        /* [in] */ IInterface* callerIdentity);

private:
    static const String TAG;        // = "TTS.AudioPlaybackHandler";
    static const Boolean DBG;

    //private final LinkedBlockingQueue<PlaybackQueueItem> mQueue;
    //Queue< AutoPtr<PlaybackQueueItem> > mQueue;
    List< AutoPtr<PlaybackQueueItem> > mQueue;
    AutoPtr<IThread> mHandlerThread;

    /*volatile*/ AutoPtr<PlaybackQueueItem> mCurrentWorkItem;

};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_AudioPlaybackHandler_H__
