#include "elastos/droid/systemui/media/CRingtonePlayer.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CBinderHelper.h"
#include "elastos/droid/media/CRingtone.h"

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Media::CRingtone;
using Elastos::Droid::Media::EIID_IIRingtonePlayer;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

const String CRingtonePlayer::TAG("RingtonePlayer");
const Boolean CRingtonePlayer::LOGD = FALSE;

//-------------------------------------------
//    CRingtonePlayer::Client
//-------------------------------------------
CAR_INTERFACE_IMPL(CRingtonePlayer::Client, IProxyDeathRecipient);

CRingtonePlayer::Client::Client(
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ IUserHandle* user,
    /* [in] */ Int32 streamType,
    /* [in] */ CRingtonePlayer* host)
    : mToken(token)
    , mHost(host)
{
    AutoPtr<IContext> ctx;
    mHost->GetContextForUser(user, (IContext**)&ctx);
    CRingtone::New(ctx, FALSE, (IRingtone**)&mRingtone);
    mRingtone->SetStreamType(streamType);
    mRingtone->SetUri(uri);
}

ECode CRingtonePlayer::Client::ProxyDied()
{
    if (LOGD) Slogger::D(TAG, "binderDied() token = %p", mToken.Get());
    {
        AutoLock lock(mLock);
        mHost->mClients.Erase(mToken);
    }
    return mRingtone->Stop();
}

//-------------------------------------------
//    CRingtonePlayer::MyRingtonePlayer
//-------------------------------------------
CAR_INTERFACE_IMPL(CRingtonePlayer::MyRingtonePlayer, IIRingtonePlayer);

ECode CRingtonePlayer::MyRingtonePlayer::Play( // throws RemoteException
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 streamType)
{
    if (LOGD) {
        // Slogger::D(TAG, "play(token=" + token + ", uri=" + uri + ", uid="
        //         + Binder.getCallingUid() + ")");
    }
    HashMap<AutoPtr<IBinder>, AutoPtr<Client> >::Iterator it;
    AutoPtr<Client> client;
    {
        AutoLock lock(mLock);
        it = mHost->mClients.Find(token);
        if (it == mHost->mClients.End()) {
            AutoPtr<IBinderHelper> binderHelper;
            CBinderHelper::AcquireSingleton((IBinderHelper**)&binderHelper);
            AutoPtr<IUserHandle> user;
            binderHelper->GetCallingUserHandle((IUserHandle**)&user);
            client = new Client(token, uri, user, streamType, mHost);

            IProxy* proxy = (IProxy*)token->Probe(EIID_IProxy);
            if (proxy != NULL) proxy->LinkToDeath(client, 0);

            mHost->mClients[token] = client;
        }
        else {
            client = it->mSecond;
        }
    }
    return client->mRingtone->Play();
}

ECode CRingtonePlayer::MyRingtonePlayer::Stop(
    /* [in] */ IBinder* token)
{
    // if (LOGD) Slogger::D(TAG, "stop(token=" + token + ")");
    HashMap<AutoPtr<IBinder>, AutoPtr<Client> >::Iterator it;
    AutoPtr<Client> client;
    {
        AutoLock lock(mLock);
        it = mHost->mClients.Find(token);
        if (it != mHost->mClients.End()) {
            client = it->mSecond;
            mHost->mClients.Erase(token);
        }
    }
    if (client != NULL) {
        IProxy* proxy = (IProxy*)client->mToken->Probe(EIID_IProxy);
        assert(proxy != NULL);
        Boolean result;
        proxy->UnlinkToDeath(client, 0, &result);

        client->mRingtone->Stop();
    }
    return NOERROR;
}

ECode CRingtonePlayer::MyRingtonePlayer::IsPlaying(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    // if (LOGD) Slogger::D(TAG, "isPlaying(token=" + token + ")");
    HashMap<AutoPtr<IBinder>, AutoPtr<Client> >::Iterator it;
    {
        AutoLock lock(mLock);
        it = mHost->mClients.Find(token);
    }
    if (it != mHost->mClients.End() && (it->mSecond != NULL)) {
        it->mSecond->mRingtone->IsPlaying(result);
    } else {
        *result = FALSE;
    }
    return NOERROR;
}

/** Used for Notification sound playback. */
ECode CRingtonePlayer::MyRingtonePlayer::PlayAsync(
    /* [in] */ IUri* uri,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean looping,
    /* [in] */ Int32 streamType)
{
    // if (LOGD) Slogger::D(TAG, "playAsync(uri=" + uri + ", user=" + user + ")");
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        // throw new SecurityException("Async playback only available from system UID.");
        Slogger::E(TAG, "Async playback only available from system UID.");
        return E_SECURITY_EXCEPTION;
    }

    // return mAsyncPlayer->Play(GetContextForUser(user), uri, looping, streamType);
    return NOERROR;
}

ECode CRingtonePlayer::MyRingtonePlayer::StopAsync()
{
    // if (LOGD) Slogger::D(TAG, "stopAsync()");
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        // throw new SecurityException("Async playback only available from system UID.");
        Slogger::E(TAG, "Async playback only available from system UID.");
        return E_SECURITY_EXCEPTION;
    }
    // return mAsyncPlayer->Stop();
    return NOERROR;
}

CRingtonePlayer::CRingtonePlayer()
{}

ECode CRingtonePlayer::constructor()
{
    // CNotificationPlayer::New(TAG, (INotificationPlayer**)&mAsyncPlayer);
    mCallback = new MyRingtonePlayer(this);
    return NOERROR;
}

PInterface CRingtonePlayer::Probe(
    /* [in] */ REIID riid)
{
    return _CRingtonePlayer::Probe(riid);
}

ECode CRingtonePlayer::Start()
{
    // mAsyncPlayer->SetUsesWakeLock(mContext);

    mAudioService = (IIAudioService*)ServiceManager::GetService(IContext::AUDIO_SERVICE).Get();

    // try {
        mAudioService->SetRingtonePlayer(mCallback);
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Problem registering RingtonePlayer: " + e);
    // }
    return NOERROR;
}

ECode CRingtonePlayer::SetContext(
    /* [in] */ IContext* context)
{
    return SystemUI::SetContext(context);
}

ECode CRingtonePlayer::GetContext(
    /* [out]*/ IContext** context)
{
    VALIDATE_NOT_NULL(context);
    AutoPtr<IContext> ctx = SystemUI::GetContext();
    *context = ctx;
    REFCOUNT_ADD(*context);
    return NOERROR;
}

ECode CRingtonePlayer::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    return SystemUI::OnConfigurationChanged(newConfig);
}

void CRingtonePlayer::GetContextForUser(
    /* [in] */ IUserHandle* user,
    /* [out]*/ IContext** context)
{
    // try {
    String name;
    mContext->GetPackageName(&name);
    mContext->CreatePackageContextAsUser(name, 0, user, context);
    // } catch (NameNotFoundException e) {
    //     throw new RuntimeException(e);
    // }
}

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
