#ifndef __ELASTOS_DROID_SYSTEMUI_MEDIA_NOTIFICATIONPLAYER_H__
#define __ELASTOS_DROID_SYSTEMUI_MEDIA_NOTIFICATIONPLAYER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Core::Thread;
using Elastos::Utility::ILinkedList;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

/**
 * @hide
 * This class is provides the same interface and functionality as android.media.AsyncPlayer
 * with the following differences:
 * - whenever audio is played, audio focus is requested,
 * - whenever audio playback is stopped or the playback completed, audio focus is abandoned.
 */
class NotificationPlayer
    : public Object
    , public IMediaPlayerOnCompletionListener
{
private:
    class Command
        : public Object
    {
    public:
        Command();

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Int32 mCode;
        AutoPtr<IContext> mContext;
        AutoPtr<IUri> mUri;
        Boolean mLooping;
        AutoPtr<IAudioAttributes> mAttributes;
        Int64 mRequestTime;
    };

    class CreationAndCompletionThread
        : public Thread
    {
    public:
        CreationAndCompletionThread(
            /* [in] */ Command* cmd,
            /* [in] */ NotificationPlayer* host);

        CARAPI Run();

    private:
        AutoPtr<Command> mCmd;
        NotificationPlayer* mHost;
    };

    class CmdThread
        : public Thread
    {
    public:
        CmdThread(
            /* [in] */ NotificationPlayer* host);

        CARAPI Run();

    private:
        NotificationPlayer* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Construct a NotificationPlayer object.
     *
     * @param tag a string to use for debugging
     */
    NotificationPlayer(
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
     * @deprecated use {@link #play(Context, Uri, boolean, AudioAttributes)} instead.
     */
    // @Deprecated
    CARAPI Play(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean looping,
        /* [in] */ Int32 stream);

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
     * @param attributes the AudioAttributes to use.
     *          (see {@link MediaPlayer#setAudioAttributes(AudioAttributes)})
     */
    CARAPI Play(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean looping,
        /* [in] */ IAudioAttributes* attributes);

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
    static const Int32 PLAY;
    static const Int32 STOP;
    static const Boolean mDebug;
    AutoPtr<ILinkedList> mCmdQueue;

    AutoPtr<ILooper> mLooper;

    String mTag;
    AutoPtr<CmdThread> mThread;
    AutoPtr<CreationAndCompletionThread> mCompletionThread;
    AutoPtr<Object> mCompletionHandlingLock;
    AutoPtr<IMediaPlayer> mPlayer;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<Object> mQueueAudioFocusLock;
    AutoPtr<IAudioManager> mAudioManagerWithAudioFocus; // synchronized on mQueueAudioFocusLock

    // The current state according to the caller.  Reality lags behind
    // because of the asynchronous nature of this class.
    Int32 mState;
};

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_MEDIA_NOTIFICATIONPLAYER_H__