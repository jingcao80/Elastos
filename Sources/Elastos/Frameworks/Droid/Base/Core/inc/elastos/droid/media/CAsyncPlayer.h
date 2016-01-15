
#ifndef __ELASTOS_DROID_MEDIA_CASYNCPLAYER_H__
#define __ELASTOS_DROID_MEDIA_CASYNCPLAYER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Media_CAsyncPlayer.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Core::Thread;
using Elastos::Utility::ILinkedList;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Plays a series of audio URIs, but does all the hard work on another thread
 * so that any slowness with preparing or loading doesn't block the calling thread.
 */
CarClass(CAsyncPlayer)
    , public Object
    , public IAsyncPlayer
{
private:
    class Command
         : public Object
    {
    public:
        CARAPI ToString(
            /* [out] */ String* result);

    public:
        Int32 mCode;
        AutoPtr<IContext> mContext;
        AutoPtr<IUri> mUri;
        Boolean mLooping;
        Int32 mStream;
        Int64 mRequestTime;
    };

    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ CAsyncPlayer* owner);

        CARAPI Run();

    private:
        CAsyncPlayer* mOwner;
    };

public:
    CAsyncPlayer();

    virtual ~CAsyncPlayer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Construct an AsyncPlayer object.
     *
     * @param tag a string to use for debugging
     */
    CARAPI constructor(
        /* [in] */ const String& tag);

    /**
     * Start playing the sound.  It will actually start playing at some
     * point in the future.  There are no guarantees about latency here.
     * Calling this before another audio file is done playing will stop
     * that one and start the new one.
     *
     * @param context Your application's context.
     * @param uri The URI to play.  (see {@link MediaPlayer#setDataSource(Context, Uri)})
     * @param looping Whether the audio should loop forever.
     *          (see {@link MediaPlayer#setLooping(boolean)})
     * @param stream the AudioStream to use.
     *          (see {@link MediaPlayer#setAudioStreamType(int)})
     */
    CARAPI Play(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean looping,
        /* [in] */ Int32 stream);

    /**
     * Stop a previously played sound.  It can't be played again or unpaused
     * at this point.  Calling this multiple times has no ill effects.
     */
    CARAPI Stop();

    /**
     * We want to hold a wake lock while we do the prepare and play.  The stop probably is
     * optional, but it won't hurt to have it too.  The problem is that if you start a sound
     * while you're holding a wake lock (e.g. an alarm starting a notification), you want the
     * sound to play, but if the CPU turns off before mThread gets to work, it won't.  The
     * simplest way to deal with this is to make it so there is a wake lock held while the
     * thread is starting or running.  You're going to need the WAKE_LOCK permission if you're
     * going to call this.
     *
     * This must be called before the first time play is called.
     *
     * @hide
     */
    CARAPI SetUsesWakeLock(
        /* [in] */ IContext* context);

private:
    CARAPI_(void) StartSound(
        /* [in] */ Command* cmd);

    CARAPI_(void) EnqueueLocked(
        /* [in] */ Command* cmd);

    CARAPI_(void) AcquireWakeLock();

    CARAPI_(void) ReleaseWakeLock();

private:
    static const Int32 PLAY;
    static const Int32 STOP;
    static const Boolean mDebug;

    AutoPtr<ILinkedList> mCmdQueue;

    String mTag;
    AutoPtr<MyThread> mThread;
    AutoPtr<IMediaPlayer> mPlayer;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    // The current state according to the caller.  Reality lags behind
    // because of the asynchronous nature of this class.
    Int32 mState;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CASYNCPLAYER_H__
