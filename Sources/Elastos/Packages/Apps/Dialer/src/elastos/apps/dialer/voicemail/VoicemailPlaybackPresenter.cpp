
#include "VoicemailPlaybackPresenter.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Voicemail {

//=================================================================================
// VoicemailPlaybackPresenter::FetchResultHandler::FetchResultHandlerAsyncTask
//=================================================================================
VoicemailPlaybackPresenter::FetchResultHandler::FetchResultHandlerAsyncTask::FetchResultHandlerAsyncTask(
    /* [in]  */ FetchResultHandler* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::FetchResultHandler::FetchResultHandlerAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALUE_NOT_NULL(*result);
    Boolean hasContent;
    mHost->mHost->mView->QueryHasContent(mVoicemailUri, &hasContent);
    *result = CoreUtils::Convert(hasContent);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode VoicemailPlaybackPresenter::FetchResultHandler::FetchResultHandlerAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    Boolean hasContent = CoreUtils::Unbox(result);
    if (hasContent) {
        Boolean getAndSetRet;
        if (mHost->mResultStillPending->GetAndSet(FALSE, &getAndSetRet), getAndSetRet) {
            mView->UnregisterContentObserver(mHost);
            PostSuccessfullyFetchedContent();
        }
    }
    return NOERROR;
}

//=================================================================================
// VoicemailPlaybackPresenter::FetchResultHandler
//=================================================================================
CAR_INTERFACE_IMPL(VoicemailPlaybackPresenter::FetchResultHandler, ContentObserver, IRunnable)

VoicemailPlaybackPresenter::FetchResultHandler::FetchResultHandler(
    /* [in]  */ VoicemailPlaybackPresenter* host)
    : mHost->(host)
{
    CAtomicBoolean::New((IAtomicBoolean**)&mResultStillPending);
}

ECode VoicemailPlaybackPresenter::FetchResultHandler::constructor(
    /* [in]  */ IHandler* handler)
{
    ContentObserver::constructor(handler);
    mHandler = handler;
    return NOERROR;
}

AutoPtr<IRunnable> VoicemailPlaybackPresenter::FetchResultHandler::GetTimeoutRunnable()
{
    return (IRunnable*)this;
}

ECode VoicemailPlaybackPresenter::FetchResultHandler::Run()
{
    Boolean result;
    if (mResultStillPending->GetAndSet(FALSE, &result), result) {
        mHost->mView->UnregisterContentObserver(this);
        mHost->mView->SetFetchContentTimeout();
    }
    return NOERROR;
}

void VoicemailPlaybackPresenter::FetchResultHandler::Destroy()
{
    Boolean result;
    if (mResultStillPending->GetAndSet(FALSE, &result), result) {
        mHost->mView->UnregisterContentObserver(this);
        mHandler->RemoveCallbacks(this);
    }
    return NOERROR;
}

ECode VoicemailPlaybackPresenter::FetchResultHandler::OnChange(
    /* [in]  */ Boolean selfChange)
{
    mHost->mAsyncTaskExecutor->Submit(ITasks::CHECK_CONTENT_AFTER_CHANGE,
                    new FetchResultHandlerAsyncTask(this));
    return NOERROR;
}

//========================================================================================
// VoicemailPlaybackPresenter::MediaPlayerErrorListener::MediaPlayerErrorListenerRunnable
//========================================================================================
VoicemailPlaybackPresenter::MediaPlayerErrorListener::MediaPlayerErrorListenerRunnable::MediaPlayerErrorListenerRunnable(
    /* [in]  */ MediaPlayerErrorListener* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::MediaPlayerErrorListener::MediaPlayerErrorListenerRunnable::Run()
{
    mHost->mHost->HandleError(E_ILLEGAL_STATE_EXCEPTION);
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::MediaPlayerErrorListener
//=========================================================================
CAR_INTERFACE_IMPL(VoicemailPlaybackPresenter::MediaPlayerErrorListener, Object, IMediaPlayerOnErrorListener)

VoicemailPlaybackPresenter::MediaPlayerErrorListener::MediaPlayerErrorListener(
    /* [in]  */ VoicemailPlaybackPresenter* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::MediaPlayerErrorListener::OnError(
    /* [in]  */ IMediaPlayer* mp,
    /* [in]  */ Int32 what,
    /* [in]  */ Int32 extra,
    /* [out]  */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    mHost->mHost->mView->RunOnUiThread(new MediaPlayerErrorListenerRunnable(this));
    *result = TRUE;
    return NOERROR;
}


//==================================================================================================
// VoicemailPlaybackPresenter::MediaPlayerCompletionListener::MediaPlayerCompletionListenerRunnable
//==================================================================================================
VoicemailPlaybackPresenter::MediaPlayerCompletionListener::MediaPlayerCompletionListenerRunnable::MediaPlayerCompletionListenerRunnable(
    /* [in]  */ MediaPlayerCompletionListener* host,
    /* [in]  */ IMediaPlayer* mp)
    : mHost(host)
    , mMp(mp)
{}

ECode VoicemailPlaybackPresenter::MediaPlayerCompletionListener::MediaPlayerCompletionListenerRunnable::Run()
{
    mHost->mHost->HandleCompletion(mMp);
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::MediaPlayerCompletionListener
//=========================================================================
CAR_INTERFACE_IMPL(VoicemailPlaybackPresenter::MediaPlayerCompletionListener, Object, IMediaPlayerOnCompletionListener)

VoicemailPlaybackPresenter::MediaPlayerCompletionListener::MediaPlayerCompletionListener(
    /* [in]  */ VoicemailPlaybackPresenter* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::MediaPlayerCompletionListener::OnCompletion(
    /* [in]  */ IMediaPlayer* mp)
{
    VALUE_NOT_NULL(result);
    mHost->mHost->mView->RunOnUiThread(
            new MediaPlayerCompletionListenerRunnable(this, mp));
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::RateChangeListener
//=========================================================================
CAR_INTERFACE_IMPL(VoicemailPlaybackPresenter::RateChangeListener, Object, IViewOnClickListener)

VoicemailPlaybackPresenter::RateChangeListener::RateChangeListener(
    /* [in]  */ VoicemailPlaybackPresenter* host,
    /* [in]  */ Boolean increase)
    : mHost(host)
    , mIncrease(increase)
{}

ECode VoicemailPlaybackPresenter::RateChangeListener::OnClick(
    /* [in]  */ IView* v)
{
    // Adjust the current rate, then clamp it to the allowed values.
    mHost->mRateIndex = Constrain(mHost->mRateIndex + (mIncrease ? 1 : -1),
            0, sizeof(PRESET_RATES)/sizeof(Float) - 1);
    // Whether or not we have actually changed the index, call changeRate().
    // This will ensure that we show the "fastest" or "slowest" text on the ui to indicate
    // to the user that it doesn't get any faster or slower.
    mHost->ChangeRate(PRESET_RATES[mRateIndex], PRESET_NAMES[mRateIndex]);
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::AsyncPrepareTask
//=========================================================================
VoicemailPlaybackPresenter::AsyncPrepareTask::AsyncPrepareTask(
    /* [in]  */ VoicemailPlaybackPresenter* host,
    /* [in]  */ Int32 clipPositionInMillis)
    : mHost(host)
    , mClipPositionInMillis(clipPositionInMillis)
{}

ECode VoicemailPlaybackPresenter::AsyncPrepareTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALUE_NOT_NULL(result);
    // try {
    ECode ec = NOERROR;
    Boolean isReadyToPlay;
    if (mHost->mPlayer->IsReadyToPlay(&isReadyToPlay), !isReadyToPlay) {
        ec = mPlayer->Reset();
        if (FAILED(ec)) goto exit;

        AutoPtr<IContext> context;
        ec = mHost->mView->GetDataSourceContext((IContext**)&context);
        if (FAILED(ec)) goto exit;
        ec = mHost->mPlayer->SetDataSource(context, mVoicemailUri);
        if (FAILED(ec)) goto exit;
        ec = mHost->mPlayer->SetAudioStreamType(PLAYBACK_STREAM);
        if (FAILED(ec)) goto exit;
        ec = mHost->mPlayer->Prepare();
        if (FAILED(ec)) goto exit;
    }
exit:
    if (SUCCEEDED(ec)) {
        *result = NULL;
    }
    else {
        *result = CoreUtils::Convert(ec);
    }
    return NOERROR;
    // } catch (Exception e) {
    //     return e;
    // }
}

ECode VoicemailPlaybackPresenter::AsyncPrepareTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    mHost->mPrepareTask = NULL;
    if (result == NULL) {
        Int32 duration;
        mPlayer->GetDuration(&duration);
        mDuration->Set(duration);
        Int32 startPosition =
            Constrain(mClipPositionInMillis, 0, duration);
        mHost->mPlayer->SeekTo(startPosition);
        mHost->mView->SetClipPosition(startPosition, duration);
        // try {
            // Can throw RejectedExecutionException
            ECode ec = mHost->mPlayer->Start();
            if (ec == (ECode)E_REJECTED_EXECUTION_EXCEPTION) {
                mHost->HandleError(ec);
            }
            mHost->mView->PlaybackStarted();
            Boolean isHeld;
            if (mHost->mWakeLock->IsHeld(&isHeld), !isHeld) {
                mHost->mWakeLock->Acquire();
            }
            // Only enable if we are not currently using the speaker phone.
            Boolean isSpeakerPhoneOn;
            if (mHost->mView->IsSpeakerPhoneOn(&isSpeakerPhoneOn), !isSpeakerPhoneOn) {
                mHost->mView->EnableProximitySensor();
            }
            // Can throw RejectedExecutionException
            ec = mPositionUpdater->StartUpdating(startPosition, duration);
            if (ec == (ECode)E_REJECTED_EXECUTION_EXCEPTION) {
                mHost->HandleError(ec);
            }
        // } catch (RejectedExecutionException e) {
        //     handleError(e);
        // }
    }
    else {
        mHost->HandleError(CoreUtils::Unbox(result));
    }
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::PlaybackPositionListener
//=========================================================================
CAR_INTERFACE_IMPL(VoicemailPlaybackPresenter::PlaybackPositionListener, Object, ISeekBarOnSeekBarChangeListener)

VoicemailPlaybackPresenter::PlaybackPositionListener::PlaybackPositionListener(
    /* [in]  */ VoicemailPlaybackPresenter* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::PlaybackPositionListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* arg0)
{
    Boolean result;
    if (mHost->mPlayer->IsPlaying(&result), result) {
        mShouldResumePlaybackAfterSeeking = TRUE;
        Int32 position;
        mHost->mPlayer->GetCurrentPosition(&position);
        Int32 value;
        mHost->mDuration->Get(&value);
        StopPlaybackAtPosition(position, value);
    }
    else {
        mShouldResumePlaybackAfterSeeking = FALSE;
    }
    return NOERROR;
}

ECode VoicemailPlaybackPresenter::PlaybackPositionListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* arg0)
{
    Boolean result;
    if (mHost->mPlayer->IsPlaying(&result), result) {
        Int32 position;
        mHost->mPlayer->GetCurrentPosition(&position);
        Int32 value;
        mHost->mDuration->Get(&value);
        StopPlaybackAtPosition(position, value);
    }
    if (mShouldResumePlaybackAfterSeeking) {
        Int32 position;
        mHost->mView->GetDesiredClipPosition(&position);
        ResetPrepareStartPlaying(position);
    }
    return NOERROR;
}

ECode VoicemailPlaybackPresenter::PlaybackPositionListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    Int32 progress;
    IProgressBar::Probe(seekBar)->GetProgress(&progress);
    Int32 max;
    IProgressBar::Probe(seekBar)->GetMax(&max);
    mHost->mView->SetClipPosition(progress, max);
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::SpeakerphoneListener
//=========================================================================
CAR_INTERFACE_IMPL(VoicemailPlaybackPresenter::SpeakerphoneListener, Object, IViewOnClickListener)

VoicemailPlaybackPresenter::SpeakerphoneListener::SpeakerphoneListener(
    /* [in]  */ VoicemailPlaybackPresenter* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::SpeakerphoneListener::OnClick(
    /* [in] */ IView* v)
{
    Boolean previousState;
    mHost->mView->IsSpeakerPhoneOn(&previousState);
    mHost->mView->SetSpeakerPhoneOn(!previousState);
    Boolean result;
    if (mHost->mPlayer->IsPlaying(&result), result && previousState) {
        // If we are currently playing and we are disabling the speaker phone, enable the
        // sensor.
        mHost->mView->EnableProximitySensor();
    }
    else {
        // If we are not currently playing, disable the sensor.
        mHost->mView->DisableProximitySensor();
    }
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::StartStopButtonListener
//=========================================================================
CAR_INTERFACE_IMPL(VoicemailPlaybackPresenter::StartStopButtonListener, Object, IViewOnClickListener)

VoicemailPlaybackPresenter::StartStopButtonListener::StartStopButtonListener(
    /* [in]  */ VoicemailPlaybackPresenter* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::StartStopButtonListener::OnClick(
    /* [in] */ IView* v)
{
    Boolean result;
    if (mHost->mPlayer->IsPlaying(&result), result) {
        Int32 position;
        mHost->mPlayer->GetCurrentPosition(&position);
        Int32 value;
        mHost->mDuration->Get(&value);
        StopPlaybackAtPosition(position, value);
    }
    else {
        Int32 position;
        mHost->mView->GetDesiredClipPosition(&position);
        ResetPrepareStartPlaying(position);
    }
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::PositionUpdater::PositionUpdaterRunnable
//=========================================================================
VoicemailPlaybackPresenter::PositionUpdater::PositionUpdaterRunnable::PositionUpdaterRunnable(
    /* [in]  */ PositionUpdater* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::PositionUpdater::PositionUpdaterRunnable::Run()
{
    Int32 currentPosition = 0;
    synchronized (mHost->mLock) {
        if (mScheduledFuture == NULL) {
            // This task has been canceled. Just stop now.
            return NOERROR;
        }
        mHost->mHost->mPlayer->GetCurrentPosition(&currentPosition);
    }
    Int32 value;
    mHost->mHost->mDuration->Get(&value);
    mHost->mHost->mView->SetClipPosition(currentPosition, value);
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::PositionUpdater
//=========================================================================
VoicemailPlaybackPresenter::PositionUpdater::PositionUpdater(
    /* [in] */ VoicemailPlaybackPresenter* host,
    /* [in] */ IScheduledExecutorService* executorService,
    /* [in] */ Int32 periodMillis)
    : mHost(host)
    , mExecutorService(executorService)
    , mPeriodMillis(periodMillis)
{
    mSetClipPostitionRunnable = new PositionUpdaterRunnable(this);
}

ECode VoicemailPlaybackPresenter::PositionUpdater::Run()
{
    mHost->mView->RunOnUiThread(mSetClipPostitionRunnable);
    return NOERROR;
}

void VoicemailPlaybackPresenter::PositionUpdater::StartUpdating(
    /* [in] */ Int32 beginPosition,
    /* [in] */ Int32 endPosition)
{
    synchronized (mLock) {
        if (mScheduledFuture != NULL) {
            mScheduledFuture->Cancel(FALSE);
            mScheduledFuture = NULL;
        }
        mExecutorService->ScheduleAtFixedRate(this, 0, mPeriodMillis,
                ITimeUnit::MILLISECONDS, (IScheduledFuture**)&mScheduledFuture);
    }
}

void VoicemailPlaybackPresenter::PositionUpdater::StopUpdating()
{
    synchronized (mLock) {
        if (mScheduledFuture != NULL) {
            mScheduledFuture->Cancel(FALSE);
            mScheduledFuture = NULL;
        }
    }
}

//=========================================================================
// VoicemailPlaybackPresenter::CheckThatWeHaveContentAsyncTask
//=========================================================================
VoicemailPlaybackPresenter::CheckThatWeHaveContentAsyncTask::CheckThatWeHaveContentAsyncTask(
    /* [in]  */ VoicemailPlaybackPresenter* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::CheckThatWeHaveContentAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALUE_NOT_NULL(result);
    Boolean hasContent;
    mHost->mView->QueryHasContent(mVoicemailUri, &hasContent);
    *result = CoreUtils::Convert(hasContent);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode VoicemailPlaybackPresenter::CheckThatWeHaveContentAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    Boolean hasContent = CoreUtils::Unbox(result);
    if (hasContent) {
        mHost->PostSuccessfullyFetchedContent();
    } else {
        mHost->MakeRequestForContent();
    }
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter::SuccessfullyFetchedContentAsyncTask
//=========================================================================
VoicemailPlaybackPresenter::SuccessfullyFetchedContentAsyncTask::SuccessfullyFetchedContentAsyncTask(
    /* [in]  */ VoicemailPlaybackPresenter* host)
    : mHost(host)
{}

ECode VoicemailPlaybackPresenter::SuccessfullyFetchedContentAsyncTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALUE_NOT_NULL(result);
    // try {
    ECode ec = NOERROR;
    ec = mHost->mPlayer->Reset();
    if (FAILED(ec)) goto exit;
    AutoPtr<IContext> context;
    ec = mHost->mView->GetDataSourceContext((IContext**)&context);
    if (FAILED(ec)) goto exit;
    ec = mHost->mPlayer->SetDataSource(context, mVoicemailUri);
    if (FAILED(ec)) goto exit;
    ec = mHost->mPlayer->SetAudioStreamType(PLAYBACK_STREAM);
    if (FAILED(ec)) goto exit;
    ec = mHost->mPlayer->Prepare();
    if (FAILED(ec)) goto exit;

    Int32 duration;
    mHost->mPlayer->GetDuration(&duration);
    ec = mHost->mDuration->Set(duration);
    if (FAILED(ec)) goto exit;
    // } catch (Exception e) {
    //     return e;
    // }

exit:
    if (SUCCEEDED(ec)) {
        *result = NULL
    }
    else {
        *result = CoreUtils::Convert(ec);
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

ECode VoicemailPlaybackPresenter::SuccessfullyFetchedContentAsyncTask::OnPostExecute(
    /* [in] */ IInterface* result)
{
    if (result == null) {
        mHost->PostSuccessfulPrepareActions();
    }
    else {
        mHost->mView->PlaybackError(exception);
    }
    return NOERROR;
}

//=========================================================================
// VoicemailPlaybackPresenter
//=========================================================================
const Int32 VoicemailPlaybackPresenter::PLAYBACK_STREAM = IAudioManager::STREAM_VOICE_CALL;

const Int32 VoicemailPlaybackPresenter::SLIDER_UPDATE_PERIOD_MILLIS = 1000 / 30;
const Int64 VoicemailPlaybackPresenter::FETCH_CONTENT_TIMEOUT_MS = 20000;

const String VoicemailPlaybackPresenter::PAUSED_STATE_KEY("VoicemailPlaybackPresenter.PAUSED_STATE_KEY");

const String VoicemailPlaybackPresenter::CLIP_POSITION_KEY("VoicemailPlaybackPresenter.CLIP_POSITION_KEY");

const Float VoicemailPlaybackPresenter::PRESET_RATES[] = {
    0.64f, 0.8f, 1.0f, 1.25f, 1.5625f
};
const Int32 VoicemailPlaybackPresenter::PRESET_NAMES[] = {
    R::string::voicemail_speed_slowest,
    R::string::voicemail_speed_slower,
    R::string::voicemail_speed_normal,
    R::string::voicemail_speed_faster,
    R::string::voicemail_speed_fastest,
};

VoicemailPlaybackPresenter::VoicemailPlaybackPresenter()
    : mRateIndex(2)
{
    CAtomicInteger32::New((IAtomicInteger**)&mDuration);
}

ECode VoicemailPlaybackPresenter::constructor(
    /* [in] */ IVoicemailPlaybackPresenterPlaybackView* view,
    /* [in] */ IMediaPlayerProxy* player,
    /* [in] */ IUri* voicemailUri,
    /* [in] */ IScheduledExecutorService* executorService,
    /* [in] */ Boolean startPlayingImmediately,
    /* [in] */ IAsyncTaskExecutor* asyncTaskExecutor,
    /* [in] */ IPowerManagerWakeLock* wakeLock)
{
    mView = view;
    mPlayer = player;
    mVoicemailUri = voicemailUri;
    mStartPlayingImmediately = startPlayingImmediately;
    mAsyncTaskExecutor = asyncTaskExecutor;
    CPositionUpdater::New(executorService, SLIDER_UPDATE_PERIOD_MILLIS,
            (IPositionUpdater**)&mPositionUpdater);
    mWakeLock = wakeLock;
    return NOERROR;
}

ECode VoicemailPlaybackPresenter::OnCreate(
    /* [in] */ IBundle* bundle)
{
    mView->SetVolumeControlStream(PLAYBACK_STREAM);
    CheckThatWeHaveContent();
    return NOERROR;
}

void VoicemailPlaybackPresenter::CheckThatWeHaveContent()
{
    mView->SetIsFetchingContent();
    mAsyncTaskExecutor->Submit(ITasks::CHECK_FOR_CONTENT,
            new CheckThatWeHaveContentAsyncTask(this));
}

void VoicemailPlaybackPresenter::MakeRequestForContent()
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)preconditions);
    preconditions->CheckState(mFetchResultHandler == NULL, "mFetchResultHandler should be null");
    mFetchResultHandler = new FetchResultHandler(this);
    mFetchResultHandler->constructor(handler);
    mView->RegisterContentObserver(mVoicemailUri, (IRunnable*)mFetchResultHandler);
    handler->PostDelayed(mFetchResultHandler->GetTimeoutRunnable(), FETCH_CONTENT_TIMEOUT_MS);
    mView->SendFetchVoicemailRequest(mVoicemailUri);
}

void VoicemailPlaybackPresenter::PostSuccessfullyFetchedContent()
{
    mView->SetIsBuffering();
    mAsyncTaskExecutor->Submit(ITasks::PREPARE_MEDIA_PLAYER,
            new SuccessfullyFetchedContentAsyncTask(this));
}

void VoicemailPlaybackPresenter::PostSuccessfulPrepareActions()
{
    mView->EnableUiElements();
    mView->SetPositionSeekListener(new PlaybackPositionListener(this));
    mView->SetStartStopListener(new StartStopButtonListener(this));
    mView->SetSpeakerphoneListener(new SpeakerphoneListener(this));
    mPlayer->SetOnErrorListener(new MediaPlayerErrorListener(this));
    mPlayer->SetOnCompletionListener(new MediaPlayerCompletionListener(this));

    Boolean result;
    mView->IsSpeakerPhoneOn(&result)
    mView->SetSpeakerPhoneOn(result);

    AutoPtr<IViewOnClickListener> listener;
    CreateRateDecreaseListener(&listener);
    mView->SetRateDecreaseButtonListener(listener);
    listener = NULL;
    CreateRateIncreaseListener(&listener);
    mView->SetRateIncreaseButtonListener(listener);

    Int32 value;
    mDuration->Get(&value);
    mView->SetClipPosition(0, value);
    mView->PlaybackStopped();
    // Always disable on stop.
    mView->DisableProximitySensor();
    if (mStartPlayingImmediately) {
        ResetPrepareStartPlaying(0);
    }
    // TODO: Now I'm ignoring the bundle, when previously I was checking for contains against
    // the PAUSED_STATE_KEY, and CLIP_POSITION_KEY.
}

ECode VoicemailPlaybackPresenter::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Int32 position;
    mView->GetDesiredClipPosition(&position);
    outState->PutInt32(CLIP_POSITION_KEY, position);
    Boolean isPlaying;
    if (mPlayer->IsPlaying(&isPlaying), !isPlaying) {
        outState->PutBoolean(PAUSED_STATE_KEY, TRUE);
    }

    return NOERROR;
}

ECode VoicemailPlaybackPresenter::OnDestroy()
{
    if (mPrepareTask != NULL) {
        mPrepareTask->Cancel(FALSE);
        mPrepareTask = NULL;
    }
    mPlayer->Release();
    if (mFetchResultHandler != NULL) {
        mFetchResultHandler->Destroy();
        mFetchResultHandler = NULL;
    }
    mPositionUpdater->StopUpdating();

    Boolean isHeld;
    if (mWakeLock->IsHeld(&isHeld), isHeld) {
        mWakeLock->Release();
    }

    return NOERROR;
}

ECode VoicemailPlaybackPresenter::CreateRateDecreaseListener(
    /* [out] */ IViewOnClickListener** listener)
{
    VALUE_NOT_NULL(listener);
    AutoPtr<RateChangeListener> result = new RateChangeListener(this, FALSE);
    *listener = (IViewOnClickListener*)result;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

ECode VoicemailPlaybackPresenter::CreateRateIncreaseListener(
    /* [out] */ IViewOnClickListener** listener)
{
    VALUE_NOT_NULL(listener);
    AutoPtr<RateChangeListener> result = new RateChangeListener(this, TRUE);
    *listener = (IViewOnClickListener*)result;
    REFCOUNT_ADD(*listener);
    return NOERROR;
}

void VoicemailPlaybackPresenter::ResetPrepareStartPlaying(
    /* [in] */ Int32 clipPositionInMillis)
{
    if (mPrepareTask != NULL) {
        mPrepareTask->Cancel(FALSE);
        mPrepareTask = NULL;
    }
    mAsyncTaskExecutor->Submit(ITasks::RESET_PREPARE_START_MEDIA_PLAYER,
            new AsyncPrepareTask(this, clipPositionInMillis), (IAsyncTask**)&mPrepareTask);
}

void VoicemailPlaybackPresenter::HandleError(
    /* [in] */ ECode ec)
{
    mView->PlaybackError(ec);
    mPositionUpdater->StopUpdating();
    mPlayer->Release();
}

ECode VoicemailPlaybackPresenter::HandleCompletion(
    /* [in] */ IMediaPlayer* mediaPlayer)
{
    Int32 duration;
    mDuration->Get(&duration);
    StopPlaybackAtPosition(0, duration);
    return NOERROR;
}

void VoicemailPlaybackPresenter::StopPlaybackAtPosition(
    /* [in] */ Int32 clipPosition,
    /* [in] */ Int32 duration)
{
    mPositionUpdater->StopUpdating();
    mView->PlaybackStopped();
    Boolean isHeld;
    if (mWakeLock->IsHeld(&isHeld), isHeld) {
        mWakeLock->Release();
    }
    // Always disable on stop.
    mView->DisableProximitySensor();
    mView->SetClipPosition(clipPosition, duration);

    Boolean isPlaying;
    if (mPlayer->IsPlaying(&isPlaying), isPlaying) {
        mPlayer->Pause();
    }
}

void VoicemailPlaybackPresenter::ChangeRate(
    /* [in] */ Float rate,
    /* [in] */ Int32 stringResourceId)
{
    ISingleThreadedMediaPlayerProxy::Probe(mPlayer)->SetVariableSpeed(rate);
    mView->SetRateDisplay(rate, stringResourceId);
}

ECode VoicemailPlaybackPresenter::OnPause()
{
    Boolean isPlaying;
    if (mPlayer->IsPlaying(&isPlaying), isPlaying) {
        Int32 position;
        mPlayer->GetCurrentPosition(&position);
        Int32 value;
        mDuration->Get(&value);
        StopPlaybackAtPosition(position, value);
    }
    if (mPrepareTask != NULL) {
        mPrepareTask->Cancel(FALSE);
        mPrepareTask = NULL;
    }

    Boolean isHeld;
    if (mWakeLock->IsHeld(&isHeld), isHeld) {
        mWakeLock->Release();
    }

    return NOERROR;
}

Int32 VoicemailPlaybackPresenter::Constrain(
    /* [in] */ Int32 amount,
    /* [in] */ Int32 low,
    /* [in] */ Int32 high)
{
    return amount < low ? low : (amount > high ? high : amount);
}

} // Voicemail
} // Dialer
} // Apps
} // Elastos
