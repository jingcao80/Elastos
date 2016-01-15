#ifndef __ELASTOS_DROID_SYSTEMUI_MEDIA_CNOTIFICATIONPLAYER_H__
#define __ELASTOS_DROID_SYSTEMUI_MEDIA_CNOTIFICATIONPLAYER_H__

#include "_Elastos_Droid_SystemUI_Media_CNotificationPlayer.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

CarClass(CNotificationPlayer)
{
private:
    class Command
        : public ElRefBase
    {
    public:
        Command()
            : mCode(0)
            , mLooping(FALSE)
            , mStream(0)
            , mRequestTime(0)
        {}

        CARAPI_(String) ToString();

    public:
        Int32 mCode;
        AutoPtr<IContext> mContext;
        AutoPtr<IUri> mUri;
        Boolean mLooping;
        Int32 mStream;
        Int64 mRequestTime;
    };

    class CreationAndCompletionThread
        : public ThreadBase
    {
    public:
        CreationAndCompletionThread(
            /* [in] */ Command* cmd,
            /* [in] */ CNotificationPlayer* host);

        virtual CARAPI Run();

    private:
        AutoPtr<Command> mCmd;
        CNotificationPlayer* mHost;
        Object mLock;
    };

    class CmdThread
        : public ThreadBase
    {
    public:
        CmdThread(
            /* [in] */ CNotificationPlayer* host);

        virtual CARAPI Run();

    private:
        CNotificationPlayer* mHost;
    };

public:
    CNotificationPlayer();

    /**
     * Construct a NotificationPlayer object.
     *
     * @param tag a string to use for debugging
     */
    CARAPI constructor(
        /* [in] */ const String& tag);

    CARAPI OnCompletion(
        /* [in] */ IMediaPlayer* mp);

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
    CARAPI_(void) EnqueueLocked(
        /* [in] */ Command* cmd);

    CARAPI_(void) AcquireWakeLock();

    CARAPI_(void) ReleaseWakeLock();

    CARAPI_(void) StartSound(
        /* [in] */ Command* cmd);

private:
    static const Int32 PLAY; // = 1;
    static const Int32 STOP; // = 2;
    static const Boolean mDebug; // = false;
    List< AutoPtr<Command> > mCmdQueue; // LinkedList<Command> mCmdQueue; // = new LinkedList();

    AutoPtr<ILooper> mLooper;

    String mTag;
    AutoPtr<CmdThread> mThread;
    AutoPtr<CreationAndCompletionThread> mCompletionThread;
    Object mCompletionHandlingLock; // = new Object();
    AutoPtr<IMediaPlayer> mPlayer;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<IAudioManager> mAudioManager;

    // The current state according to the caller.  Reality lags behind
    // because of the asynchronous nature of this class.
    Int32 mState; // = STOP;
    Object mCmdQueueLock;
    Object mCompletionThreadLock;
};

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_MEDIA_CNOTIFICATIONPLAYER_H__
