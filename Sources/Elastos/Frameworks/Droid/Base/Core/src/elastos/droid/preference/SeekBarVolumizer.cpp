#include "elastos/droid/content/CIntentFilter.h"
// #include "elastos/droid/media/RingtoneManager.h"
#include "elastos/droid/preference/SeekBarVolumizer.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/core/StringBuilder.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
// using Elastos::Droid::Media::RingtoneManager;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::R;
using Elastos::Core::IThread;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Preference {

//**************SeekBarVolumizer::SeekBarVolumizerH***************//

const Int32 SeekBarVolumizer::SeekBarVolumizerH::UPDATE_SLIDER;

SeekBarVolumizer::SeekBarVolumizerH::SeekBarVolumizerH(
    /* [in] */ SeekBarVolumizer* owner)
    : mOwner(owner)
{
}
ECode SeekBarVolumizer::SeekBarVolumizerH::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == UPDATE_SLIDER) {
        if (mOwner->mSeekBar != NULL) {
            Int32 arg1;
            msg->GetArg1(&arg1);
            IProgressBar::Probe(mOwner->mSeekBar)->SetProgress(arg1);
            IProgressBar::Probe(mOwner->mSeekBar)->GetProgress(&(mOwner->mLastProgress));
        }
    }
    return NOERROR;
}

ECode SeekBarVolumizer::SeekBarVolumizerH::PostUpdateSlider(
    /* [in] */ Int32 volume)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(UPDATE_SLIDER, volume, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

//**************SeekBarVolumizer::SeekBarVolumizerObserver***************//

SeekBarVolumizer::SeekBarVolumizerObserver::SeekBarVolumizerObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ SeekBarVolumizer* owner)
    : mOwner(owner)
{
    ContentObserver::constructor(handler);
}

ECode SeekBarVolumizer::SeekBarVolumizerObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    FAIL_RETURN(ContentObserver::OnChange(selfChange));
    if (mOwner->mSeekBar != NULL && mOwner->mAudioManager != NULL) {
        Int32 volume;
        mOwner->mAudioManager->GetStreamVolume(mOwner->mStreamType, &volume);
        mOwner->mUiHandler->PostUpdateSlider(volume);
    }
    return NOERROR;
}
//**************SeekBarVolumizer::SeekBarVolumizerReceiver***************//

SeekBarVolumizer::SeekBarVolumizerReceiver::SeekBarVolumizerReceiver(
    /* [in] */ SeekBarVolumizer* owner)
    : mListening(FALSE)
    , mOwner(owner)
{}

void SeekBarVolumizer::SeekBarVolumizerReceiver::SetListening(
    /* [in] */ Boolean listening)
{
    if (mListening == listening) return;
    mListening = listening;
    if (listening) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New(IAudioManager::VOLUME_CHANGED_ACTION, (IIntentFilter**)&filter);
        AutoPtr<IIntent> intent;
        mOwner->mContext->RegisterReceiver(this, filter, (IIntent**)&intent);
    } else {
        mOwner->mContext->UnregisterReceiver(this);
    }
}

ECode SeekBarVolumizer::SeekBarVolumizerReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (!IAudioManager::VOLUME_CHANGED_ACTION.Equals(action)) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 streamType;
    intent->GetInt32Extra(IAudioManager::EXTRA_VOLUME_STREAM_TYPE, -1, &streamType);
    Int32 streamValue;
    intent->GetInt32Extra(IAudioManager::EXTRA_VOLUME_STREAM_VALUE, -1, &streamValue);

    if (mOwner->mSeekBar != NULL && streamType == mOwner->mStreamType && streamValue != -1) {
        mOwner->mUiHandler->PostUpdateSlider(streamValue);
    }
    return NOERROR;
}

//**************SeekBarVolumizer**************************************//

const String SeekBarVolumizer::TAG = String("SeekBarVolumizer");
const Int32 SeekBarVolumizer::MSG_SET_STREAM_VOLUME;
const Int32 SeekBarVolumizer::MSG_START_SAMPLE;
const Int32 SeekBarVolumizer::MSG_STOP_SAMPLE;
const Int32 SeekBarVolumizer::MSG_INIT_SAMPLE;
const Int32 SeekBarVolumizer::CHECK_RINGTONE_PLAYBACK_DELAY_MS;

CAR_INTERFACE_IMPL_3(SeekBarVolumizer, Object, ISeekBarVolumizer, ISeekBarOnSeekBarChangeListener, IHandlerCallback)

SeekBarVolumizer::SeekBarVolumizer()
    : mStreamType(0)
    , mMaxStreamVolume(0)
    , mOriginalStreamVolume(0)
    , mLastProgress(-1)
    , mVolumeBeforeMute(-1)
{
    mUiHandler = new SeekBarVolumizerH(this);
    mReceiver = new SeekBarVolumizerReceiver(this);
}

ECode SeekBarVolumizer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 streamType,
    /* [in] */ IUri* defaultUri,
    /* [in] */ ISeekBarVolumizerCallback* callback)
{
    mContext = context;
    AutoPtr<IInterface> object;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&object);
    mAudioManager = IAudioManager::Probe(object);

    mStreamType = streamType;
    mAudioManager->GetStreamMaxVolume(mStreamType, &mMaxStreamVolume);

    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(String("SeekBarVolumizer.CallbackHandler"), (IHandlerThread**)&thread);
    AutoPtr<IThread> t = IThread::Probe(thread);
    t->Start();

    AutoPtr<ILooper> looper;
    thread->GetLooper((ILooper**)&looper);
    assert(0);
    //CHandler lack of one   constructor as below:
    // constructor(
    //     [in] ILooper* looper,
    //     [in] IHandlerCallback* cb);
    // CHandler::New(looper, this, (IHandler**)&mHandler);
    CHandler::New(looper, this, TRUE, (IHandler**)&mHandler);
    mCallback = callback;
    mAudioManager->GetStreamVolume(mStreamType, &mOriginalStreamVolume);
    mVolumeObserver = new SeekBarVolumizerObserver(mHandler, this);

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    Settings::System::GetUriFor((*(Settings::System::VOLUME_SETTINGS))[mStreamType], (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, mVolumeObserver);
    mReceiver->SetListening(TRUE);
    if (defaultUri == NULL) {
        if (mStreamType == IAudioManager::STREAM_RING) {
            defaultUri = Settings::System::DEFAULT_RINGTONE_URI;
        } else if (mStreamType == IAudioManager::STREAM_NOTIFICATION) {
            defaultUri = Settings::System::DEFAULT_NOTIFICATION_URI;
        } else {
            defaultUri = Settings::System::DEFAULT_ALARM_ALERT_URI;
        }
    }
    mDefaultUri = defaultUri;
    Boolean result = FALSE;
    mHandler->SendEmptyMessage(MSG_INIT_SAMPLE, &result);
    return NOERROR;
}

ECode SeekBarVolumizer::SetSeekBar(
    /* [in] */ ISeekBar* seekBar)
{
    if (mSeekBar != NULL) {
        mSeekBar->SetOnSeekBarChangeListener(NULL);
    }
    mSeekBar = seekBar;
    mSeekBar->SetOnSeekBarChangeListener(NULL);
    IProgressBar::Probe(mSeekBar)->SetMax(mMaxStreamVolume);
    IProgressBar::Probe(mSeekBar)->SetProgress(mLastProgress > -1 ? mLastProgress : mOriginalStreamVolume);
     mSeekBar->SetOnSeekBarChangeListener(this);
    return NOERROR;
}

ECode SeekBarVolumizer::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_SET_STREAM_VOLUME:
            mAudioManager->SetStreamVolume(mStreamType, mLastProgress,
                    IAudioManager::FLAG_SHOW_UI_WARNINGS);
            break;
        case MSG_START_SAMPLE:
            OnStartSample();
            break;
        case MSG_STOP_SAMPLE:
            OnStopSample();
            break;
        case MSG_INIT_SAMPLE:
            OnInitSample();
            break;
        default:
            StringBuilder sb("invalid SeekBarVolumizer message: ");
            sb += what;
            Logger::E(TAG, sb.ToString());
    }
    *result = TRUE;
    return NOERROR;
}

void SeekBarVolumizer::OnInitSample()
{
    #if 0 // RingtoneManager needed
    RingtoneManager::GetRingtone(mContext, mDefaultUri, (IRingtone**)&mRingtone);
    if (mRingtone != NULL) {
        mRingtone->SetStreamType(mStreamType);
    }
    #endif
}
void SeekBarVolumizer::PostStartSample()
{
    mHandler->RemoveMessages(MSG_START_SAMPLE);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_START_SAMPLE, (IMessage**)&msg);
    Boolean result = FALSE;
    IsSamplePlaying(&result);
    mHandler->SendMessageDelayed(msg,
            result ? CHECK_RINGTONE_PLAYBACK_DELAY_MS : 0, &result);
}

void SeekBarVolumizer::OnStartSample()
{
    Boolean result = FALSE;
    IsSamplePlaying(&result);
    if (!result) {
        if (mCallback != NULL) {
            mCallback->OnSampleStarting(this);
        }
        if (mRingtone != NULL) {
            ECode ec = mRingtone->Play();
            if (FAILED(ec)) {
                Logger::W(TAG, "Error playing ringtone, stream %d%08x", mStreamType, ec);
            }
        }
    }
}

ECode SeekBarVolumizer::PostStopSample() {
    // remove pending delayed start messages
    mHandler->RemoveMessages(MSG_START_SAMPLE);
    mHandler->RemoveMessages(MSG_STOP_SAMPLE);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_STOP_SAMPLE, (IMessage**)&msg);
    Boolean result = FALSE;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

void SeekBarVolumizer::OnStopSample() {
    if (mRingtone != NULL) {
        mRingtone->Stop();
    }
}

ECode SeekBarVolumizer::Stop()
{
    PostStopSample();
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IContentObserver> observer;
    resolver->UnregisterContentObserver(observer);
    mVolumeObserver = (SeekBarVolumizerObserver*)(observer.Get());
    mSeekBar->SetOnSeekBarChangeListener(NULL);
    mReceiver->SetListening(FALSE);
    AutoPtr<ILooper> looper;
    mHandler->GetLooper((ILooper**)&looper);
    looper->QuitSafely();
    return NOERROR;
}

ECode SeekBarVolumizer::RevertVolume()
{
    return mAudioManager->SetStreamVolume(mStreamType, mOriginalStreamVolume, 0);
}

ECode SeekBarVolumizer::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromTouch)
{
    if (!fromTouch) {
        return E_NULL_POINTER_EXCEPTION;
    }

    PostSetVolume(progress);
    return NOERROR;
}

ECode SeekBarVolumizer::PostSetVolume(
    /* [in] */ Int32 progress)
{
    // Do the volume changing separately to give responsive UI
    mLastProgress = progress;
    mHandler->RemoveMessages(MSG_SET_STREAM_VOLUME);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_SET_STREAM_VOLUME, (IMessage**)&msg);
    Boolean result = FALSE;
    mHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode SeekBarVolumizer::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return NOERROR;
}

ECode SeekBarVolumizer::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    PostStartSample();
    return NOERROR;
}

ECode SeekBarVolumizer::IsSamplePlaying(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mRingtone->IsPlaying(result);
    *result = mRingtone != NULL && *result;
    return NOERROR;
}

ECode SeekBarVolumizer::StartSample()
{
    PostStartSample();
    return NOERROR;
}

ECode SeekBarVolumizer::StopSample()
{
    PostStopSample();
    return NOERROR;
}

ECode SeekBarVolumizer::GetSeekBar(
    /* [out] */ ISeekBar** seekBar)
{
    VALIDATE_NOT_NULL(seekBar)
    *seekBar = mSeekBar;
    REFCOUNT_ADD(*seekBar);
    return NOERROR;
}

ECode SeekBarVolumizer::ChangeVolumeBy(
    /* [in] */ Int32 amount)
{
    IProgressBar::Probe(mSeekBar)->IncrementProgressBy(amount);
    Int32 progress;
    IProgressBar::Probe(mSeekBar)->GetProgress(&progress);
    PostSetVolume(progress);
    PostStartSample();
    mVolumeBeforeMute = -1;
    return NOERROR;
}

ECode SeekBarVolumizer::MuteVolume()
{
    if (mVolumeBeforeMute != -1) {
        IProgressBar::Probe(mSeekBar)->SetProgress(mVolumeBeforeMute);
        PostSetVolume(mVolumeBeforeMute);
        PostStartSample();
        mVolumeBeforeMute = -1;
    }
    else {
        IProgressBar::Probe(mSeekBar)->GetProgress(&mVolumeBeforeMute);
        IProgressBar::Probe(mSeekBar)->SetProgress(0);
        PostStopSample();
        PostSetVolume(0);
    }
    return NOERROR;
}

ECode SeekBarVolumizer::OnSaveInstanceState(
    /* [in] */ IVolumePreferenceVolumeStore* volumeStore)
{
    if (mLastProgress >= 0) {
        volumeStore->SetVolume(mLastProgress);
        volumeStore->SetOriginalVolume(mOriginalStreamVolume);
    }
    return NOERROR;
}

ECode SeekBarVolumizer::OnRestoreInstanceState(
    /* [in] */ IVolumePreferenceVolumeStore* volumeStore)
{
    Int32 volumn;
    volumeStore->GetVolume(&volumn);
    if (volumn != -1) {
        volumeStore->GetOriginalVolume(&mOriginalStreamVolume);
        volumeStore->GetVolume(&mLastProgress);
        PostSetVolume(mLastProgress);
    }
    return NOERROR;
}

}
}
}