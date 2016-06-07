
#include "elastos/droid/server/telecom/AsyncRingtonePlayer.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/ThreadUtil.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Core::IThread;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// AsyncRingtonePlayer::SubHandler
//=============================================================================
AsyncRingtonePlayer::SubHandler::SubHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ AsyncRingtonePlayer* host)
    : Handler(looper)
    , mHost(host)
{}

ECode AsyncRingtonePlayer::SubHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    Int32 msgArg2;
    msg->GetArg2(&msgArg2);
    if (msgWhat == EVENT_PLAY) {
        mHost->HandlePlay(IUri::Probe(msgObj), (Float) msgArg1 / 100.0f, msgArg2);
    }
    else if (msgWhat == EVENT_REPEAT) {
        mHost->HandleRepeat();
    }
    else if (msgWhat == EVENT_STOP) {
        mHost->HandleStop();
    }
    else if (msgWhat == EVENT_INCREASE_VOLUME) {
        mHost->mCurrentIncrementVolume += mHost->mIncrementAmount;
        Log::D("AsyncRingtonePlayer", "Increasing ringtone volume to %d%%",
                Elastos::Core::Math::Round(mHost->mCurrentIncrementVolume * 100.0f));
        mHost->mRingtone->SetVolume(mHost->mCurrentIncrementVolume);
        if (mHost->mCurrentIncrementVolume < 1.0f) {
            Boolean bNoUse;
            SendEmptyMessageDelayed(EVENT_INCREASE_VOLUME, 1000, &bNoUse);
        }
    }
    return NOERROR;
}

//=============================================================================
// AsyncRingtonePlayer
//=============================================================================
const Int32 AsyncRingtonePlayer::EVENT_PLAY = 1;
const Int32 AsyncRingtonePlayer::EVENT_STOP = 2;
const Int32 AsyncRingtonePlayer::EVENT_REPEAT = 3;
const Int32 AsyncRingtonePlayer::EVENT_INCREASE_VOLUME = 4;
const Int32 AsyncRingtonePlayer::RESTART_RINGER_MILLIS = 3000;

AsyncRingtonePlayer::AsyncRingtonePlayer()
    : mIncrementAmount(0.0)
    , mCurrentIncrementVolume(0.0)
    , mPhoneId(0)
{}

ECode AsyncRingtonePlayer::SetPhoneId(
    /* [in] */ Int32 phoneId)
{
    mPhoneId = phoneId;
    return NOERROR;
}

ECode AsyncRingtonePlayer::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode AsyncRingtonePlayer::Play(
    /* [in] */ IUri* ringtone,
    /* [in] */ Float incStartVolume,
    /* [in] */ Int32 incRampUpTime)
{
    Log::D("AsyncRingtonePlayer", "Posting play.");
    PostMessage(EVENT_PLAY, TRUE /* shouldCreateHandler */, ringtone,
            Elastos::Core::Math::Round(incStartVolume * 100.0f), incRampUpTime);
    return NOERROR;
}

ECode AsyncRingtonePlayer::Stop()
{
    Log::D("AsyncRingtonePlayer", "Posting stop.");
    PostMessage(EVENT_STOP, FALSE /* shouldCreateHandler */, NULL, 0, 0);
    return NOERROR;
}

ECode AsyncRingtonePlayer::PostMessage(
    /* [in] */ Int32 messageCode,
    /* [in] */ Boolean shouldCreateHandler,
    /* [in] */ IUri* ringtone,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    {
        AutoLock lock(this);
        if (mHandler == NULL && shouldCreateHandler) {
            GetNewHandler((IHandler**)&mHandler);
        }
        if (mHandler == NULL) {
            Log::D("AsyncRingtonePlayer", "Message %d skipped because there is no handler.", messageCode);
        } else {
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(messageCode, arg1, arg2, ringtone, (IMessage**)&msg);
            msg->SendToTarget();
        }
    }
    return NOERROR;
}

ECode AsyncRingtonePlayer::GetNewHandler(
    /* [out] */ IHandler** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPreconditions> preconditionsHelper;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditionsHelper);
    preconditionsHelper->CheckState(mHandler == NULL);
    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(String("ringtone-player"), (IHandlerThread**)&thread);
    IThread::Probe(thread)->Start();
    AutoPtr<ILooper> looper;
    thread->GetLooper((ILooper**)&looper);
    *result = new SubHandler(looper, this);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode AsyncRingtonePlayer::HandlePlay(
    /* [in] */ IUri* ringtoneUri,
    /* [in] */ Float incStartVolume,
    /* [in] */ Int32 incRampUpTime)
{
        // don't bother with any of this if there is an EVENT_STOP waiting.
    Boolean hasMessages;
    mHandler->HasMessages(EVENT_STOP, &hasMessages);
    if (hasMessages) {
        return NOERROR;
    }
    ThreadUtil::CheckNotOnMainThread();
    Log::I("AsyncRingtonePlayer", "Play ringtone.");
    if (mRingtone == NULL) {
        GetRingtone(ringtoneUri, (IRingtone**)&mRingtone);
        // Cancel everything if there is no ringtone.
        if (mRingtone == NULL) {
            HandleStop();
            return NOERROR;
        }
    }
    if (incRampUpTime > 0) {
        Log::D("AsyncRingtonePlayer", "Starting ringtone volume at %d%%", Elastos::Core::Math::Round(incStartVolume * 100.0f));
        mRingtone->SetVolume(incStartVolume);
        mIncrementAmount = (1.0f - incStartVolume) / (Float) incRampUpTime;
        mCurrentIncrementVolume = incStartVolume;
        Boolean bNoUse;
        mHandler->SendEmptyMessageDelayed(EVENT_INCREASE_VOLUME, 1000, &bNoUse);
    } else {
        mRingtone->SetVolume(1.0f);
    }
    HandleRepeat();
    return NOERROR;
}

ECode AsyncRingtonePlayer::HandleRepeat()
{
    if (mRingtone == NULL) {
        return NOERROR;
    }
    Boolean isPlaying;
    mRingtone->IsPlaying(&isPlaying);
    if (isPlaying) {
        Log::D("AsyncRingtonePlayer", "Ringtone already playing.");
    } else {
        mRingtone->Play();
        Log::I("AsyncRingtonePlayer", "Repeat ringtone.");
    }
    // Repost event to restart ringer in {@link RESTART_RINGER_MILLIS}.
    {
        AutoLock lock(this);
        Boolean hasMessages;
        mHandler->HasMessages(EVENT_REPEAT, &hasMessages);
        if (!hasMessages) {
            Boolean bNoUse;
            mHandler->SendEmptyMessageDelayed(EVENT_REPEAT, RESTART_RINGER_MILLIS, &bNoUse);
        }
    }
    return NOERROR;
}

ECode AsyncRingtonePlayer::HandleStop()
{
    ThreadUtil::CheckNotOnMainThread();
    Log::I("AsyncRingtonePlayer", "Stop ringtone.");
    if (mRingtone != NULL) {
        Log::D("AsyncRingtonePlayer", "Ringtone.stop() invoked.");
        mRingtone->Stop();
        mRingtone = NULL;
    }
    {
        AutoLock lock(this);
        // At the time that STOP is handled, there should be no need for repeat messages in the
        // queue.
        mHandler->RemoveMessages(EVENT_REPEAT);
        mHandler->RemoveMessages(EVENT_INCREASE_VOLUME);
        Boolean hasMessages;
        mHandler->HasMessages(EVENT_PLAY, &hasMessages);
        if (hasMessages) {
            Log::V("AsyncRingtonePlayer", "Keeping alive ringtone thread for subsequent play request.");
        } else {
            mHandler->RemoveMessages(EVENT_STOP);
            AutoPtr<ILooper> looper;
            mHandler->GetLooper((ILooper**)&looper);
            looper->QuitSafely();
            mHandler = NULL;
            Log::V("AsyncRingtonePlayer", "Handler cleared.");
        }
    }
    return NOERROR;
}

ECode AsyncRingtonePlayer::GetRingtone(
    /* [in] */ IUri* ringtoneUri,
    /* [out] */ IRingtone** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    if (ringtoneUri == NULL) {
        Boolean isMultiSimEnabled;
        AutoPtr<ITelephonyManagerHelper> telephonyManagerHelper;
        CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&telephonyManagerHelper);
        AutoPtr<ITelephonyManager> tm;
        telephonyManagerHelper->GetDefault((ITelephonyManager**)&tm);
        tm->IsMultiSimEnabled(&isMultiSimEnabled);
        if (isMultiSimEnabled) {
            helper->GetActualRingtoneUriBySubId(mContext, mPhoneId, (IUri**)&ringtoneUri);
            if (ringtoneUri == NULL) {
                *result = NULL;
                return NOERROR;
            }
        } else {
            AutoPtr<ISettingsSystem> helper;
            CSettingsSystem::AcquireSingleton((ISettingsSystem**)&helper);
            helper->GetDEFAULT_RINGTONE_URI((IUri**)&ringtoneUri);
        }
    }
    AutoPtr<IRingtone> ringtone;
    helper->GetRingtone(mContext, ringtoneUri, (IRingtone**)&ringtone);
    if (ringtone != NULL) {
        ringtone->SetStreamType(IAudioManager::STREAM_RING);
    }
    *result = ringtone;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
