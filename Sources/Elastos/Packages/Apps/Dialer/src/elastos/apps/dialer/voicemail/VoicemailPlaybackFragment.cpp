
#include "voicemail/VoicemailPlaybackFragment.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Voicemail {

//=================================================================
// VoicemailPlaybackFragment::ActivityReference
//=================================================================
VoicemailPlaybackFragment::ActivityReference::ActivityReference(
    /* [in] */ VoicemailPlaybackFragment* host)
    : mHost(host)
{}

AutoPtr<IActivity> VoicemailPlaybackFragment::ActivityReference::Get()
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    return activity;
}

//=================================================================
// VoicemailPlaybackFragment::PlaybackViewImpl
//=================================================================
CAR_INTERFACE_IMPL(VoicemailPlaybackFragment::PlaybackViewImpl, Object, IVoicemailPlaybackPresenterPlaybackView);

VoicemailPlaybackFragment::PlaybackViewImpl::PlaybackViewImpl(
    /* [in] */ VoicemailPlaybackFragment* host,
    /* [in] */ ActivityReference* activityReference,
    /* [in] */ IContext* applicationContext,
    /* [in] */ IView* playbackLayout)
    : mHost(host)
{
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)preconditions);
    preconditions->CheckNotNull(activityReference);
    preconditions->CheckNotNull(applicationContext);
    preconditions->CheckNotNull(playbackLayout);
    mActivityReference = activityReference;
    mApplicationContext = applicationContext;

    AutoPtr<IView> view;
    playbackLayout->FindViewById(R::id::playback_seek, (IView**)&view);
    mPlaybackSeek = ISeekBar::Probe(view);
    view = NULL;
    playbackLayout->FindViewById(R::id::playback_start_stop, (IView**)&view);
    mStartStopButton = IImageButton::Probe(view);
    view = NULL;
    playbackLayout->FindViewById(R::id::playback_speakerphone, (IView**)&view);
    mPlaybackSpeakerphone = IImageButton::Probe(view);
    view = NULL;
    playbackLayout->FindViewById(R::id::rate_decrease_button, (IView**)&view);
    mRateDecreaseButton = IImageButton::Probe(view);
    view = NULL;
    playbackLayout->FindViewById(R::id::rate_increase_button, (IView**)&view);
    mRateIncreaseButton = IImageButton::Probe(view);

    AutoPtr<IView> positionText;
    playbackLayout->FindViewById(R::id::playback_position_text, (IView**)&positionText);
    AutoPtr<IView> speedText;
    playbackLayout->FindViewById(R::id::playback_speed_text, (IView**)&speedText);
    CTextViewWithMessagesController::New(ITextView::Probe(positionText),
            ITextView::Probe(speedText), (ITextViewWithMessagesController**)&mTextController);
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::Finish()
{
    AutoPtr<IActivity> activity;
    mActivityReference->Get((IActivity**)&activity);
    if (activity != NULL) {
        activity->Finish();
    }
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::RunOnUiThread(
    /* [in] */ IRunnable* runnable)
{
    AutoPtr<IActivity> activity;
    mActivityReference->Get((IActivity**)&activity);
    if (activity != NULL) {
        activity->RunOnUiThread(runnable);
    }
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::GetDataSourceContext(
    /* [out] */ IContext** context)
{
    VALUE_NOT_NULL(context);
    *context = mApplicationContext;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetRateDecreaseButtonListener(
    /* [in] */ IViewOnClickListener* listener)
{
    return mRateDecreaseButton->SetOnClickListener(listener);
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetRateIncreaseButtonListener(
    /* [in] */ IViewOnClickListener* listener)
{
    return mRateIncreaseButton->SetOnClickListener(listener);
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetStartStopListener(
    /* [in] */ IViewOnClickListener* listener)
{
    return mStartStopButton->SetOnClickListener(listener);
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetSpeakerphoneListener(
    /* [in] */ IViewOnClickListener* listener)
{
    return mPlaybackSpeakerphone->SetOnClickListener(listener);
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetRateDisplay(
    /* [in] */ Float rate,
    /* [in] */ Int32 stringResourceId)
{
    String str;
    mApplicationContext->GetString(stringResourceId, &str);
    mTextController->SetTemporaryText(str, 1, ITimeUnit::SECONDS);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetPositionSeekListener(
    /* [in] */ ISeekBarOnSeekBarChangeListener* listener)
{
    return mPlaybackSeek->SetOnSeekBarChangeListener(listener);
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::PlaybackStarted()
{
    return mStartStopButton->SetImageResource(R::drawable::ic_hold_pause);
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::PlaybackStopped()
{
    return mStartStopButton->SetImageResource(R::drawable::ic_play);
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::EnableProximitySensor()
{
    // Only change the state if the activity is still around.
    AutoPtr<IActivity> activity;
    mActivityReference->Get((IActivity**)&activity);
    if (activity != NULL && IProximitySensorAware::Probe(activity) != NULL) {
        IProximitySensorAware::Probe(activity)->EnableProximitySensor();
    }
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::DisableProximitySensor()
{
    // Only change the state if the activity is still around.
    AutoPtr<IActivity> activity;
    mActivityReference->Get((IActivity**)&activity);
    if (activity != NULL && IProximitySensorAware::Probe(activity) != NULL) {
        IProximitySensorAware::Probe(activity)->DisableProximitySensor(TRUE);
    }
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::RegisterContentObserver(
    /* [in] */ IUri* uri,
    /* [in] */ IContentObserver* observer)
{
    AutoPtr<IContentObserver> observer;
    mApplicationContext->GetContentResolver((IContentObserver**)&observer);
    observer->RegisterContentObserver(uri, FALSE, observer);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::UnregisterContentObserver(
    /* [in] */ IContentObserver* observer)
{
    AutoPtr<IContentObserver> observer;
    mApplicationContext->GetContentResolver((IContentObserver**)&observer);
    observer->UnregisterContentObserver();
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetClipPosition(
    /* [in] */ Int32 clipPositionInMillis,
    /* [in] */ Int32 clipLengthInMillis)
{
    Int32 seekBarPosition = Math::Max(0, clipPositionInMillis);
    Int32 seekBarMax = Math::Max(seekBarPosition, clipLengthInMillis);
    Int32 max;
    IProgressBar::Probe(mPlaybackSeek)->GetMax(&max);
    if (max != seekBarMax) {
        IProgressBar::Probe(mPlaybackSeek)->SetMax(seekBarMax);
    }
    mPlaybackSeek->SetProgress(seekBarPosition);
    mTextController->SetPermanentText(
            mHost->FormatAsMinutesAndSeconds(seekBarMax - seekBarPosition));

    return NOERROR;
}

String VoicemailPlaybackFragment::PlaybackViewImpl::GetString(
    /* [in] */ Int32 resId)
{
    String str;
    mApplicationContext->GetString(resId, &str);
    return str;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetIsBuffering()
{
    DisableUiElements();
    mTextController->SetPermanentText(GetString(R::string::voicemail_buffering));
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetIsFetchingContent()
{
    DisableUiElements();
    mTextController->SetPermanentText(GetString(R::string::voicemail_fetching_content));
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetFetchContentTimeout()
{
    DisableUiElements();
    mTextController.setPermanentText(GetString(R::string::voicemail_fetching_timout));
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::GetDesiredClipPosition(
    /* [out] */ Int32* position)
{
    VALUE_NOT_NULL(position);
    mPlaybackSeek->GetProgress(position);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::DisableUiElements()
{
    mRateIncreaseButton->SetEnabled(FALSE);
    mRateDecreaseButton->SetEnabled(FALSE);
    mStartStopButton->SetEnabled(FALSE);
    mPlaybackSpeakerphone->SetEnabled(FALSE);
    mPlaybackSeek->SetProgress(0);
    mPlaybackSeek->SetEnabled(FALSE);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::PlaybackError(
    /* [in] */ ECode ec)
{
    DisableUiElements();
    mTextController->SetPermanentText(GetString(R::string::voicemail_playback_error));
    Logger::E(TAG, "Could not play voicemail %x", ec);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::EnableUiElements()
{
    mRateIncreaseButton->SetEnabled(TRUE);
    mRateDecreaseButton->SetEnabled(TRUE);
    mStartStopButton->SetEnabled(TRUE);
    mPlaybackSpeakerphone->SetEnabled(TRUE);
    mPlaybackSeek->SetEnabled(TRUE);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SendFetchVoicemailRequest(
    /* [in] */ IUri* voicemailUri)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IVoicemailContract::ACTION_FETCH_VOICEMAIL,
            voicemailUri, (IIntent**)&intent);
    mApplicationContext->SendBroadcast(intent);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::QueryHasContent(
    /* [in] */ IUri* voicemailUri,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    AutoPtr<IContentResolver> contentResolver;
    mApplicationContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ICursor> cursor;
    contentResolver->Query(voicemailUri, HAS_CONTENT_PROJECTION,
            NULL, NULL, NULL, (ICursor**)&cursor);
    // try {
    Boolean succeeded;
    if (cursor != NULL && cursor->MoveToNext(&succeeded), succeeded) {
        Int32 columnIndex;
        ECode ec = cursor->GetColumnIndexOrThrow(
                IVoicemailContractVoicemails::HAS_CONTENT, &columnIndex);
        if (FAILED(ec)) {
            *result = FALSE;
            goto exit;
        }
        Int32 value;
        cursor->GetInt32(columnIndex, &value);
        *result = value == 1;
        goto exit;
    }
    // } finally {
    //     MoreCloseables.closeQuietly(cursor);
    // }
exit:
    assert(0 && "TODO");
    // MoreCloseables.closeQuietly(cursor);
    return NOERROR;
}

AutoPtr<IAudioManager> VoicemailPlaybackFragment::PlaybackViewImpl::GetAudioManager()
{
    AutoPtr<IInterface> service;
    mApplicationContext->GetSystemService(
            IContext::AUDIO_SERVICE, (IInterface**)&service);
    return IAudioManager::Probe(service);
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::IsSpeakerPhoneOn(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    AutoPtr<IAudioManager> manager = GetAudioManager();
    manager->IsSpeakerphoneOn(result);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetSpeakerPhoneOn(
    /* [in] */ Boolean on)
{
    VALUE_NOT_NULL(result);
    AutoPtr<IAudioManager> manager = GetAudioManager();
    manager->SetSpeakerphoneOn(on);
    if (on) {
        mPlaybackSpeakerphone->SetImageResource(R::drawable::ic_speakerphone_on);
        // Speaker is now on, tapping button will turn it off.
        String str;
        mApplicationContext->GetString(R::string::voicemail_speaker_off, &str);
        mPlaybackSpeakerphone->SetContentDescription(str);
    }
    else {
        mPlaybackSpeakerphone->SetImageResource(R::drawable::ic_speakerphone_off);
        // Speaker is now off, tapping button will turn it on.
        String str;
        mApplicationContext->GetString(R::string::voicemail_speaker_on, &str);
        mPlaybackSpeakerphone->SetContentDescription(str);
    }
    return NOERROR;
}

ECode VoicemailPlaybackFragment::PlaybackViewImpl::SetVolumeControlStream(
    /* [in] */ Int32 streamType)
{
    AutoPtr<IActivity> activity;
    mActivityReference->Get((IActivity**)&activity);
    if (activity != NULL) {
        activity->SetVolumeControlStream(streamType);
    }
    return NOERROR;
}

//=================================================================
// VoicemailPlaybackFragment::TextViewWithMessagesController
//=================================================================
VoicemailPlaybackFragment::TextViewWithMessagesController::TextViewWithMessagesController(
    /* [in] */ VoicemailPlaybackFragment* host,
    /* [in] */ ITextView* permanentTextView,
    /* [in] */ ITextView* temporaryTextView)
    : mHost(host)
    , mPermanentTextView(permanentTextView)
    , mTemporaryTextView(temporaryTextView)
{}

void VoicemailPlaybackFragment::TextViewWithMessagesController::SetPermanentText(
    /* [in] */ const String& text)
{
    mPermanentTextView->SetText(text);
}

void VoicemailPlaybackFragment::TextViewWithMessagesController::SetTemporaryText(
    /* [in] */ const String& text,
    /* [in] */ Int64 duration,
    /* [in] */ ITimeUnit* units)
{
    synchronized (mLock) {
        mTemporaryTextView->SetText(text);
        AutoPtr<IViewPropertyAnimator> animator;
        IView::Probe(mTemporaryTextView)->Animate((IViewPropertyAnimator**)&animator);
        animator->Alpha(IView::VISIBLE)
        animator->SetDuration(SHORT_ANIMATION_MS);
        animator = NULL;
        IView::Probe(mPermanentTextView)->Animate((IViewPropertyAnimator**)&animator);
        animator->Alpha(IView::INVISIBLE)
        animator->SetDuration(SHORT_ANIMATION_MS);
        mRunnable = (IRunnable*)new TextViewWithMessagesController::MyRunnable(this);
        Int64 millis;
        units->ToMillis(duration, &millis);
        mTemporaryTextView->PostDelayed(mRunnable, millis);
    }
}

//========================================================================
// VoicemailPlaybackFragment::TextViewWithMessagesController::MyRunnable
//========================================================================
const Float VoicemailPlaybackFragment::TextViewWithMessagesController::VISIBLE = 1;
const Float VoicemailPlaybackFragment::TextViewWithMessagesController::INVISIBLE = 0;
const Int64 VoicemailPlaybackFragment::TextViewWithMessagesController::SHORT_ANIMATION_MS = 200;
const Int64 VoicemailPlaybackFragment::TextViewWithMessagesController::LONG_ANIMATION_MS = 400;

VoicemailPlaybackFragment::TextViewWithMessagesController::MyRunnable::MyRunnable(
    /* [in] */ TextViewWithMessagesController* host)
    : mHost(host)
{}

ECode VoicemailPlaybackFragment::TextViewWithMessagesController::MyRunnable::Run()
{
    synchronized (mHost->mLock) {
        // We check for (mRunnable == this) becuase if not true, then another
        // setTemporaryText call has taken place in the meantime, and this
        // one is now defunct and needs to take no action.
        if (mHost->mRunnable == this) {
            mHost->mRunnable = NULL;
            AutoPtr<IViewPropertyAnimator> animator;
            IView::Probe(mHost->mTemporaryTextView)->Animate(
                    (IViewPropertyAnimator**)&animator);
            animator->Alpha(IView::INVISIBLE)
            animator->SetDuration(LONG_ANIMATION_MS);
            animator = NULL;
            IView::Probe(mHost->mPermanentTextView)->Animate(
                    (IViewPropertyAnimator**)&animator);
            animator->Alpha(IView::VISIBLE)
            animator->SetDuration(LONG_ANIMATION_MS);
        }
    }
    return NOERROR;
}

//=================================================================
// VoicemailPlaybackFragment
//=================================================================
Object VoicemailPlaybackFragment::mLock;
const String VoicemailPlaybackFragment::TAG("VoicemailPlayback");
const Int32 VoicemailPlaybackFragment::NUMBER_OF_THREADS_IN_POOL = 2;
const String VoicemailPlaybackFragment::HAS_CONTENT_PROJECTION[] = {
    IVoicemailContractVoicemails::HAS_CONTENT,
};

Int32 VoicemailPlaybackFragment::mMediaPlayerRefCount; // = 0;
AutoPtr<IMediaPlayerProxy> VoicemailPlaybackFragment::mMediaPlayerInstance;
AutoPtr<IScheduledExecutorService> VoicemailPlaybackFragment::mScheduledExecutorService;

CAR_INTERFACE_IMPL(VoicemailPlaybackFragment, AnalyticsFragment, IVoicemailPlaybackFragment)

ECode VoicemailPlaybackFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IView** view)
{
    VALUE_NOT_NULL(view);
    inflater->inflate(R::layout::playback_layout, NULL, (IView**)&mPlaybackLayout);
    *view = mPlaybackLayout;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    AnalyticsFragment::OnActivityCreated(savedInstanceState);
    AutoPtr<IBundle> arguments;
    GetArguments((IBundle**)&arguments);

    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)preconditions);
    preconditions->CheckNotNull(arguments, String("fragment must be started with arguments"));

    AutoPtr<IUri> voicemailUri;
    arguments->GetParcelable(EXTRA_VOICEMAIL_URI, (IUri**)&voicemailUri);
    preconditions->CheckNotNull(voicemailUri,  String("fragment must contain EXTRA_VOICEMAIL_URI"));
    Boolean startPlayback;
    arguments->GetBoolean(EXTRA_VOICEMAIL_START_PLAYBACK, FALSE, &startPlayback);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> service;
    IContext::Probe(activity)->GetSystemService(
            IContext::POWER_SERVICE, (IInterface**)&service);
    IPowerManager* powerManager = IPowerManager::Probe(service);
    AutoPtr<IPowerManagerWakeLock> wakeLock;
    powerManager->NewWakeLock(IPowerManager::SCREEN_DIM_WAKE_LOCK,
            String("VoicemailPlaybackFragment"), (IPowerManagerWakeLock**)&wakeLock);
    CVoicemailPlaybackPresenter::New(CreatePlaybackViewImpl(),
            GetMediaPlayerInstance(), voicemailUri,
            GetScheduledExecutorServiceInstance(), startPlayback,
            AsyncTaskExecutors::CreateAsyncTaskExecutor(), wakeLock,
            (IVoicemailPlaybackPresenter**)&mPresenter);
    mPresenter->OnCreate(savedInstanceState);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    mPresenter->OnSaveInstanceState(outState);
    AnalyticsFragment::OnSaveInstanceState(outState);
    return NOERROR;
}

ECode VoicemailPlaybackFragment::OnDestroy()
{
    ShutdownMediaPlayer();
    mPresenter->OnDestroy();
    AnalyticsFragment::OnDestroy();
    return NOERROR;
}

ECode VoicemailPlaybackFragment::OnPause()
{
    mPresenter->OnPause();
    AnalyticsFragment::OnPause();
    return NOERROR;
}

AutoPtr<IVoicemailPlaybackPresenterPlaybackView> VoicemailPlaybackFragment::CreatePlaybackViewImpl()
{
    AutoPtr<IActivity> activity;
    GetActivity(&activity);
    AutoPtr<IContext> context;
    activity->GetApplicationContext((IContext**)&context);
    AutoPtr<PlaybackViewImpl> pv = new PlaybackViewImpl(this,
            new ActivityReference(this), context, mPlaybackLayout);
    return (IVoicemailPlaybackPresenterPlaybackView*)pv;
}

AutoPtr<IMediaPlayerProxy> VoicemailPlaybackFragment::GetMediaPlayerInstance()
{
    synchronized(mLock) {
        ++mMediaPlayerRefCount;
        if (mMediaPlayerInstance == NULL) {
            assert(0 && "TODO");
            // mMediaPlayerInstance = VariableSpeed.createVariableSpeed(
            //         GetScheduledExecutorServiceInstance());
        }
        return mMediaPlayerInstance;
    }
}

AutoPtr<IScheduledExecutorService> VoicemailPlaybackFragment::GetScheduledExecutorServiceInstance()
{
    synchronized(mLock) {
        if (mScheduledExecutorService == NULL) {
            AutoPtr<IExecutors> executors;
            CExecutors::AcquireSingleton((IExecutors**)&executors);
            executors->NewScheduledThreadPool(NUMBER_OF_THREADS_IN_POOL,
                    (IScheduledExecutorService**)&mScheduledExecutorService);
        }
        return mScheduledExecutorService;
    }
}

void VoicemailPlaybackFragment::ShutdownMediaPlayer()
{
    synchronized(mLock) {
        --mMediaPlayerRefCount;
        if (mMediaPlayerRefCount > 0) {
            return;
        }
        if (mScheduledExecutorService != NULL) {
            mScheduledExecutorService->Shutdown();
            mScheduledExecutorService = NULL;
        }
        if (mMediaPlayerInstance != NULL) {
            mMediaPlayerInstance->Release();
            mMediaPlayerInstance = NULL;
        }
    }
}

String VoicemailPlaybackFragment::FormatAsMinutesAndSeconds(
    /* [in] */ Int32 millis)
{
    Int32 seconds = millis / 1000;
    Int32 minutes = seconds / 60;
    seconds -= minutes * 60;
    if (minutes > 99) {
        minutes = 99;
    }

    AutoPtr<ArrayOf<IInterface> > args = ArrayOf<IInterface>::Alloc(2);
    args->Set(0, CoreUtils::Convert(minutes));
    args->Set(1, CoreUtils::Convert(seconds));
    return StringUtils::Format("%02d:%02d", args);
}

} // Voicemail
} // Dialer
} // Apps
} // Elastos
