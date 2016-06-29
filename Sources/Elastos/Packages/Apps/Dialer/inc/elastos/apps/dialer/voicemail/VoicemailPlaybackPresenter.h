#ifndef __ELASTOS_APPS_DIALER_VOICEMAIL_VOICEMAILPLAYBACKPRESENTER_H__
#define __ELASTOS_APPS_DIALER_VOICEMAIL_VOICEMAILPLAYBACKPRESENTER_H__

#include "_Elastos.Apps.Dialer.h"
#include <elastos/core/Object.h>
#include <elastos/core/Runnable.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/os/AsyncTask.h>
#include "Elastos.Droid.Concurrent.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Droid::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IMediaPlayerOnErrorListener;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Core::Runnable;
using Elastos::Core::IRunnable;
using Elastos::Utility::Concurrent::IScheduledExecutorService;
using Elastos::Apps::Dialer::Util::IAsyncTaskExecutor;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Voicemail {

/**
 * Contains the controlling logic for a voicemail playback ui.
 * <p>
 * Specifically right now this class is used to control the
 * {@link com.android.dialer.voicemail.VoicemailPlaybackFragment}.
 * <p>
 * This class is not thread safe. The thread policy for this class is
 * thread-confinement, all calls into this class from outside must be done from
 * the main ui thread.
 */
// @NotThreadSafe
// @VisibleForTesting
class VoicemailPlaybackPresenter
    : public Object
    , public IVoicemailPlaybackPresenter
{
private:
    // @ThreadSafe
    class FetchResultHandler
        : public ContentObserver
        , public IRunnable
    {
    private:
        class FetchResultHandlerAsyncTask
            : public AsyncTask
        {
        public:
            FetchResultHandlerAsyncTask(
                /* [in]  */ FetchResultHandler* host);

            CARAPI DoInBackground(
                /* [in] */ ArrayOf<IInterface*>* params,
                /* [out] */ IInterface** result);

            CARAPI OnPostExecute(
                /* [in] */ IInterface* result);

        private:
            FetchResultHandler* mHost;
        };

    public:
        CAR_INTERFACE_DECL();

        FetchResultHandler(
            /* [in]  */ VoicemailPlaybackPresenter* host);

        CARAPI constructor(
            /* [in]  */ IHandler* handler);

        CARAPI_(AutoPtr<IRunnable>) GetTimeoutRunnable();

        // @Override
        CARAPI Run();

        CARAPI_(void) Destroy();

        // @Override
        CARAPI OnChange(
            /* [in]  */ Boolean selfChange);

    private:
        VoicemailPlaybackPresenter* mHost;
        AutoPtr<IAtomicBoolean> mResultStillPending;
        AutoPtr<IHandler> mHandler;
    };

    class MediaPlayerErrorListener
        : public Object
        , public IMediaPlayerOnErrorListener
    {
    private:
        class MediaPlayerErrorListenerRunnable
            : public Runnable
        {
        public:
            MediaPlayerErrorListenerRunnable(
                /* [in]  */ MediaPlayerErrorListener* host);

            // @Override
            CARAPI Run();

        private:
            MediaPlayerErrorListener* mHost;
        };

    public:
        CAR_INTERFACE_DECL();

        MediaPlayerErrorListener(
            /* [in]  */ VoicemailPlaybackPresenter* host);

        // @Override
        CARAPI OnError(
            /* [in]  */ IMediaPlayer* mp,
            /* [in]  */ Int32 what,
            /* [in]  */ Int32 extra,
            /* [out]  */ Boolean* result);
    private:
        VoicemailPlaybackPresenter* mHost;
    };

    class MediaPlayerCompletionListener
        : public Object
        , public IMediaPlayerOnCompletionListener
    {
    private:
        class MediaPlayerCompletionListenerRunnable
            : public Runnable
        {
        public:
            MediaPlayerCompletionListenerRunnable(
                /* [in]  */ MediaPlayerCompletionListener* host,
                /* [in]  */ IMediaPlayer* mp);

            // @Override
            CARAPI Run();

        private:
            MediaPlayerCompletionListener* mHost;
            AutoPtr<IMediaPlayer> mMp;
        };

    public:
        CAR_INTERFACE_DECL();

        MediaPlayerCompletionListener(
            /* [in]  */ VoicemailPlaybackPresenter* host);

        // @Override
        CARAPI OnCompletion(
            /* [in]  */ IMediaPlayer* mp);

    private:
        VoicemailPlaybackPresenter* mHost;
    };

    /**
     * Listens to clicks on the rate increase and decrease buttons.
     * <p>
     * This class is not thread-safe, but all interactions with it will happen on the ui thread.
     */
    class RateChangeListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        RateChangeListener(
            /* [in]  */ VoicemailPlaybackPresenter* host,
            /* [in]  */ Boolean increase);

        // @Override
        CARAPI OnClick(
            /* [in]  */ IView* v);
    private:
        VoicemailPlaybackPresenter* mHost;
        Boolean mIncrease;
    };

    class AsyncPrepareTask
        : public AsyncTask
    {
    public:
        AsyncPrepareTask(
            /* [in]  */ VoicemailPlaybackPresenter* host,
            /* [in]  */ Int32 clipPositionInMillis);

        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        VoicemailPlaybackPresenter* mHost;
        Int32 mClipPositionInMillis;
    };

    class PlaybackPositionListener
        : public Object
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        CAR_INTERFACE_DECL();

        PlaybackPositionListener(
            /* [in]  */ VoicemailPlaybackPresenter* host);

        // @Override
        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* arg0);

        // @Override
        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* arg0);

        // @Override
        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);
    private:
        VoicemailPlaybackPresenter* mHost;
        Boolean mShouldResumePlaybackAfterSeeking;
    };

    class SpeakerphoneListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        SpeakerphoneListener(
            /* [in]  */ VoicemailPlaybackPresenter* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        VoicemailPlaybackPresenter* mHost;
    };


    class StartStopButtonListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        StartStopButtonListener(
            /* [in]  */ VoicemailPlaybackPresenter* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        VoicemailPlaybackPresenter* mHost;
    };

    /**
     * Controls the animation of the playback slider.
     */
    // @ThreadSafe
    class PositionUpdater
        : public Runnable
    {
    private:
        class PositionUpdaterRunnable
            : public Runnable
        {
        public:
            PositionUpdaterRunnable(
                /* [in]  */ PositionUpdater* host);

            // @Override
            CARAPI Run();
        private:
            PositionUpdater* mHost;
        };

    public:

        PositionUpdater(
            /* [in] */ VoicemailPlaybackPresenter* host,
            /* [in] */ IScheduledExecutorService* executorService,
            /* [in] */ Int32 periodMillis);

        // @Override
        CARAPI Run();

        CARAPI_(void) StartUpdating(
            /* [in] */ Int32 beginPosition,
            /* [in] */ Int32 endPosition);

        CARAPI_(void) StopUpdating();
    private:
        VoicemailPlaybackPresenter* mHost;

        AutoPtr<IScheduledExecutorService> mExecutorService;
        Int32 mPeriodMillis;
        Object mLock;
        // @GuardedBy("mLock")
        // TODO:
        // AutoPtr<IScheduledFuture> mScheduledFuture;
        AutoPtr<IRunnable> mSetClipPostitionRunnable;
    };

    class CheckThatWeHaveContentAsyncTask
        : public AsyncTask
    {
    public:
        CheckThatWeHaveContentAsyncTask(
            /* [in]  */ VoicemailPlaybackPresenter* host);

        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        VoicemailPlaybackPresenter* mHost;
    };

    class SuccessfullyFetchedContentAsyncTask
        : public AsyncTask
    {
    public:
        SuccessfullyFetchedContentAsyncTask(
            /* [in]  */ VoicemailPlaybackPresenter* host);

        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        VoicemailPlaybackPresenter* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    VoicemailPlaybackPresenter();

    // TODO:
    // CARAPI constructor(
    //     /* [in] */ IVoicemailPlaybackPresenterPlaybackView* view,
    //     /* [in] */ IMediaPlayerProxy* player,
    //     /* [in] */ IUri* voicemailUri,
    //     /* [in] */ IScheduledExecutorService* executorService,
    //     /* [in] */ Boolean startPlayingImmediately,
    //     /* [in] */ IAsyncTaskExecutor* asyncTaskExecutor,
    //     /* [in] */ IPowerManagerWakeLock* wakeLock);

    CARAPI OnCreate(
        /* [in] */ IBundle* bundle);

    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI OnDestroy();

    CARAPI CreateRateDecreaseListener(
        /* [out] */ IViewOnClickListener** listener);

    CARAPI CreateRateIncreaseListener(
        /* [out] */ IViewOnClickListener** listener);

    CARAPI HandleCompletion(
        /* [in] */ IMediaPlayer* mediaPlayer);

    CARAPI OnPause();

private:
    /**
     * Checks to see if we have content available for this voicemail.
     * <p>
     * This method will be called once, after the fragment has been created, before we know if the
     * voicemail we've been asked to play has any content available.
     * <p>
     * This method will notify the user through the ui that we are fetching the content, then check
     * to see if the content field in the db is set. If set, we proceed to
     * {@link #postSuccessfullyFetchedContent()} method. If not set, we will make a request to fetch
     * the content asynchronously via {@link #makeRequestForContent()}.
     */
    CARAPI_(void) CheckThatWeHaveContent();

    /**
     * Makes a broadcast request to ask that a voicemail source fetch this content.
     * <p>
     * This method <b>must be called on the ui thread</b>.
     * <p>
     * This method will be called when we realise that we don't have content for this voicemail. It
     * will trigger a broadcast to request that the content be downloaded. It will add a listener to
     * the content resolver so that it will be notified when the has_content field changes. It will
     * also set a timer. If the has_content field changes to true within the allowed time, we will
     * proceed to {@link #postSuccessfullyFetchedContent()}. If the has_content field does not
     * become true within the allowed time, we will update the ui to reflect the fact that content
     * was not available.
     */
    CARAPI_(void) MakeRequestForContent();

    /**
     * Prepares the voicemail content for playback.
     * <p>
     * This method will be called once we know that our voicemail has content (according to the
     * content provider). This method will try to prepare the data source through the media player.
     * If preparing the media player works, we will call through to
     * {@link #postSuccessfulPrepareActions()}. If preparing the media player fails (perhaps the
     * file the content provider points to is actually missing, perhaps it is of an unknown file
     * format that we can't play, who knows) then we will show an error on the ui.
     */
    CARAPI_(void) PostSuccessfullyFetchedContent();

    /**
     * Enables the ui, and optionally starts playback immediately.
     * <p>
     * This will be called once we have successfully prepared the media player, and will optionally
     * playback immediately.
     */
    CARAPI_(void) PostSuccessfulPrepareActions();

    CARAPI_(void) ResetPrepareStartPlaying(
        /* [in] */ Int32 clipPositionInMillis);

    CARAPI_(void) HandleError(
        /* [in] */ ECode ec);

    CARAPI_(void) StopPlaybackAtPosition(
        /* [in] */ Int32 clipPosition,
        /* [in] */ Int32 duration);

    CARAPI_(void) ChangeRate(
        /* [in] */ Float rate,
        /* [in] */ Int32 stringResourceId);

    static CARAPI_(Int32) Constrain(
        /* [in] */ Int32 amount,
        /* [in] */ Int32 low,
        /* [in] */ Int32 high);

private:
    /** The stream used to playback voicemail. */
    static const Int32 PLAYBACK_STREAM; // = AudioManager.STREAM_VOICE_CALL;

    /** Update rate for the slider, 30fps. */
    static const Int32 SLIDER_UPDATE_PERIOD_MILLIS; // = 1000 / 30;
    /** Time our ui will wait for content to be fetched before reporting not available. */
    static const Int64 FETCH_CONTENT_TIMEOUT_MS; // = 20000;
    /**
     * If present in the saved instance bundle, we should not resume playback on
     * create.
     */
    static const String PAUSED_STATE_KEY;
    /**
     * If present in the saved instance bundle, indicates where to set the
     * playback slider.
     */
    static const String CLIP_POSITION_KEY;

    /** The preset variable-speed rates.  Each is greater than the previous by 25%. */
    static const Float PRESET_RATES[];
    /** The string resource ids corresponding to the names given to the above preset rates. */
    static const Int32 PRESET_NAMES[];

    /**
     * Pointer into the {@link VoicemailPlaybackPresenter#PRESET_RATES} array.
     * <p>
     * This doesn't need to be synchronized, it's used only by the {@link RateChangeListener}
     * which in turn is only executed on the ui thread.  This can't be encapsulated inside the
     * rate change listener since multiple rate change listeners must share the same value.
     */
    Int32 mRateIndex; // = 2;

    /**
     * The most recently calculated duration.
     * <p>
     * We cache this in a field since we don't want to keep requesting it from the player, as
     * this can easily lead to throwing {@link IllegalStateException} (any time the player is
     * released, it's illegal to ask for the duration).
     */
    AutoPtr<IAtomicInteger32> mDuration; // = new AtomicInteger(0);

    AutoPtr<IVoicemailPlaybackPresenterPlaybackView> mView;
    // TODO:
    // AutoPtr<IMediaPlayerProxy> mPlayer;
    AutoPtr<PositionUpdater> mPositionUpdater;

    /** Voicemail uri to play. */
    AutoPtr<IUri> mVoicemailUri;
    /** Start playing in onCreate iff this is true. */
    Boolean mStartPlayingImmediately;
    /** Used to run async tasks that need to interact with the ui. */
    AutoPtr<IAsyncTaskExecutor> mAsyncTaskExecutor;

    /**
     * Used to handle the result of a successful or time-out fetch result.
     * <p>
     * This variable is thread-contained, accessed only on the ui thread.
     */
    AutoPtr<FetchResultHandler> mFetchResultHandler;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<AsyncTask> mPrepareTask;
};

} // Voicemail
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_VOICEMAIL_VOICEMAILPLAYBACKPRESENTER_H__
