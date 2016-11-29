#ifndef __ELASTOS_DROID_DIALER_VOICEMAIL_VOICEMAILPLAYBACKFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_VOICEMAIL_VOICEMAILPLAYBACKFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/dialerbind/analytics/AnalyticsFragment.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::DialerBind::Analytics::AnalyticsFragment;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::IRunnable;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::IScheduledExecutorService;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Voicemail {

/**
 * Displays and plays back a single voicemail.
 * <p>
 * When the Activity containing this Fragment is created, voicemail playback
 * will begin immediately. The Activity is expected to be started via an intent
 * containing a suitable voicemail uri to playback.
 * <p>
 * This class is not thread-safe, it is thread-confined. All calls to all public
 * methods on this class are expected to come from the main ui thread.
 */
// @NotThreadSafe
class VoicemailPlaybackFragment
    : public AnalyticsFragment
{
private:
    /**
     * An object that can provide us with an Activity.
     * <p>
     * Fragments suffer the drawback that the Activity they belong to may sometimes be null. This
     * can happen if the Fragment is detached, for example. In that situation a call to
     * {@link Fragment#getString(int)} will throw and {@link IllegalStateException}. Also, calling
     * {@link Fragment#getActivity()} is dangerous - it may sometimes return null. And thus blindly
     * calling a method on the result of getActivity() is dangerous too.
     * <p>
     * To work around this, I have made the {@link PlaybackViewImpl} class static, so that it does
     * not have access to any Fragment methods directly. Instead it uses an application Context for
     * things like accessing strings, accessing system services. It only uses the Activity when it
     * absolutely needs it - and does so through this class. This makes it easy to see where we have
     * to check for null properly.
     */
    class ActivityReference
        : public Objec
    {
    public:
        ActivityReference(
            /* [in] */ VoicemailPlaybackFragment* host);

        /** Gets this Fragment's Activity: <b>may be null</b>. */
        CARAPI_(AutoPtr<IActivity>) Get();

    private:
        VoicemailPlaybackFragment* mHost;
    };

    /**  Methods required by the PlaybackView for the VoicemailPlaybackPresenter. */
    class PlaybackViewImpl
        : public Object
        , public IVoicemailPlaybackPresenterPlaybackView
    {
    public:
        CAR_INTERFACE_DECL()

        PlaybackViewImpl(
            /* [in] */ VoicemailPlaybackFragment* host,
            /* [in] */ ActivityReference* activityReference,
            /* [in] */ IContext* applicationContext,
            /* [in] */ IView* playbackLayout);

        // @Override
        CARAPI Finish();

        // @Override
        CARAPI RunOnUiThread(
            /* [in] */ IRunnable* runnable);

        // @Override
        CARAPI GetDataSourceContext(
            /* [out] */ IContext** context);

        // @Override
        CARAPI SetRateDecreaseButtonListener(
            /* [in] */ IViewOnClickListener* listener);

        // @Override
        CARAPI SetRateIncreaseButtonListener(
            /* [in] */ IViewOnClickListener* listener);

        // @Override
        CARAPI SetStartStopListener(
            /* [in] */ IViewOnClickListener* listener);

        // @Override
        CARAPI SetSpeakerphoneListener(
            /* [in] */ IViewOnClickListener* listener);

        // @Override
        CARAPI SetRateDisplay(
            /* [in] */ Float rate,
            /* [in] */ Int32 stringResourceId);

        // @Override
        CARAPI SetPositionSeekListener(
            /* [in] */ ISeekBarOnSeekBarChangeListener* listener);

        // @Override
        CARAPI PlaybackStarted();

        // @Override
        CARAPI PlaybackStopped();

        // @Override
        CARAPI EnableProximitySensor();

        // @Override
        CARAPI DisableProximitySensor();

        // @Override
        CARAPI RegisterContentObserver(
            /* [in] */ IUri* uri,
            /* [in] */ IContentObserver* observer);

        // @Override
        CARAPI UnregisterContentObserver(
            /* [in] */ IContentObserver* observer);

        // @Override
        CARAPI SetClipPosition(
            /* [in] */ Int32 clipPositionInMillis,
            /* [in] */ Int32 clipLengthInMillis);

        // @Override
        CARAPI SetIsBuffering();

        // @Override
        CARAPI SetIsFetchingContent();

        // @Override
        CARAPI SetFetchContentTimeout();

        // @Override
        CARAPI GetDesiredClipPosition(
            /* [out] */ Int32* position);

        // @Override
        CARAPI DisableUiElements();

        // @Override
        CARAPI PlaybackError(
            /* [in] */ ECode ec);

        // @Override
        CARAPI EnableUiElements();

        // @Override
        CARAPI SendFetchVoicemailRequest(
            /* [in] */ IUri* voicemailUri);

        // @Override
        CARAPI QueryHasContent(
            /* [in] */ IUri* voicemailUri,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI IsSpeakerPhoneOn(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SetSpeakerPhoneOn(
            /* [in] */ Boolean on);

        // @Override
        CARAPI SetVolumeControlStream(
            /* [in] */ Int32 streamType);

    private:
        CARAPI_(String) GetString(
            /* [in] */ Int32 resId);

        CARAPI_(AutoPtr<IAudioManager>) GetAudioManager();

    private:
        VoicemailPlaybackFragment* mHost;
        AutoPtr<ActivityReference> mActivityReference;
        AutoPtr<IContext> mApplicationContext;
        AutoPtr<ISeekBar> mPlaybackSeek;
        AutoPtr<IImageButton> mStartStopButton;
        AutoPtr<IImageButton> mPlaybackSpeakerphone;
        AutoPtr<IImageButton> mRateDecreaseButton;
        AutoPtr<IImageButton> mRateIncreaseButton;
        AutoPtr<TextViewWithMessagesController> mTextController;
    };

    /**
     * Controls a TextView with dynamically changing text.
     * <p>
     * There are two methods here of interest,
     * {@link TextViewWithMessagesController#setPermanentText(String)} and
     * {@link TextViewWithMessagesController#setTemporaryText(String, long, TimeUnit)}.  The
     * former is used to set the text on the text view immediately, and is used in our case for
     * the countdown of duration remaining during voicemail playback.  The second is used to
     * temporarily replace this countdown with a message, in our case faster voicemail speed or
     * slower voicemail speed, before returning to the countdown display.
     * <p>
     * All the methods on this class must be called from the ui thread.
     */
    class TextViewWithMessagesController
        : public Object
    {
    private:
        class MyRunnable
            : public Runnable
        {
        public:
            MyRunnable(
                /* [in] */ TextViewWithMessagesController* host);

            // @Override
            CARAPI Run();

        private:
            TextViewWithMessagesController* mHost;
        };

    public:
        TextViewWithMessagesController(
            /* [in] */ VoicemailPlaybackFragment* host,
            /* [in] */ ITextView* permanentTextView,
            /* [in] */ ITextView* temporaryTextView);

        CARAPI_(void) SetPermanentText(
            /* [in] */ const String& text);

        CARAPI_(void) SetTemporaryText(
            /* [in] */ const String& text,
            /* [in] */ Int64 duration,
            /* [in] */ ITimeUnit* units);

    private:
        VoicemailPlaybackFragment* mHost;
        static const Float VISIBLE; // = 1;
        static const Float INVISIBLE; // = 0;
        static const Int64 SHORT_ANIMATION_MS; // = 200;
        static const Int64 LONG_ANIMATION_MS; // = 400;
        const Object mLock;
        AutoPtr<ITextView> mPermanentTextView;
        AutoPtr<ITextView> mTemporaryTextView;
        // @GuardedBy("mLock")
        AutoPtr<IRunnable> mRunnable;
    };

    class

public:
    CAR_INTERFACE_DECL();

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnPause();

private:
    CARAPI_(AutoPtr<IVoicemailPlaybackPresenterPlaybackView>) CreatePlaybackViewImpl();

    /* synchronized */
    // TODO:
    // static CARAPI_(AutoPtr<IMediaPlayerProxy>) GetMediaPlayerInstance();

    /* synchronized */
    static CARAPI_(AutoPtr<IScheduledExecutorService>) GetScheduledExecutorServiceInstance();

    /* synchronized */
    static CARAPI_(void) ShutdownMediaPlayer();

    /**
     * Formats a number of milliseconds as something that looks like {@code 00:05}.
     * <p>
     * We always use four digits, two for minutes two for seconds.  In the very unlikely event
     * that the voicemail duration exceeds 99 minutes, the display is capped at 99 minutes.
     */
    static CARAPI_(String) FormatAsMinutesAndSeconds(
        /* [in] */ Int32 millis);

private:
    static Object mLock;
    static const String TAG;
    static const Int32 NUMBER_OF_THREADS_IN_POOL = 2;
    static const AutoPtr<ArrayOf<String> > HAS_CONTENT_PROJECTION;

    AutoPtr<IVoicemailPlaybackPresenter> mPresenter;
    static Int32 mMediaPlayerRefCount; // = 0;
    // TODO:
    // static AutoPtr<IMediaPlayerProxy> mMediaPlayerInstance;
    static AutoPtr<IScheduledExecutorService> mScheduledExecutorService;
    AutoPtr<IView> mPlaybackLayout;
};

} // Voicemail
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_VOICEMAIL_VOICEMAILPLAYBACKFRAGMENT_H__
