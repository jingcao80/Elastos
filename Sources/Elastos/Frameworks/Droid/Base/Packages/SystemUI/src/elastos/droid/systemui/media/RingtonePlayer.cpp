#include "elastos/droid/systemui/media/RingtonePlayer.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Media::CRingtone;
using Elastos::Droid::Media::EIID_IIRingtonePlayer;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

//-------------------------------------------
//    RingtonePlayer::Client
//-------------------------------------------
CAR_INTERFACE_IMPL(RingtonePlayer::Client, Object, IProxyDeathRecipient)

RingtonePlayer::Client::Client(
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ IUserHandle* user,
    /* [in] */ IAudioAttributes* aa,
    /* [in] */ RingtonePlayer* host)
    : mToken(token)
    , mHost(host)
{
    CRingtone::New(mHost->GetContextForUser(user), FALSE, (IRingtone**)&mRingtone);
    mRingtone->SetAudioAttributes(aa);
    mRingtone->SetUri(uri);
}

ECode RingtonePlayer::Client::ProxyDied()
{
    if (LOGD) Logger::D(TAG, "binderDied() token = %p", mToken.Get());
    synchronized (mHost->mClients) {
        mHost->mClients->Remove(mToken);
    }
    return mRingtone->Stop();
}

//-------------------------------------------
//    RingtonePlayer::MyRingtonePlayer
//-------------------------------------------
CAR_INTERFACE_IMPL_2(RingtonePlayer::MyRingtonePlayer, Object, IIRingtonePlayer, IBinder);

RingtonePlayer::MyRingtonePlayer::MyRingtonePlayer(
    /* [in] */ RingtonePlayer* host)
    : mHost(host)
{}

ECode RingtonePlayer::MyRingtonePlayer::Play( // throws RemoteException
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ IAudioAttributes* aa)
{
    if (LOGD) {
        Logger::D(TAG, "play(token=%p, uri=%p, uid=%d)", token, uri,
            Binder::GetCallingUid());
    }
    AutoPtr<Client> client;
    synchronized(mHost->mClients) {
        AutoPtr<IInterface> clientObj;
        mHost->mClients->Get(token, (IInterface**)&clientObj);
        client = (Client*)(IObject::Probe(clientObj));
        if (client == NULL) {
            const AutoPtr<IUserHandle> user = Binder::GetCallingUserHandle();
            client = new Client(token, uri, user, aa, mHost);
            IProxy* proxy = (IProxy*)token->Probe(EIID_IProxy);
            proxy->LinkToDeath(client, 0);
            mHost->mClients->Put(token, (IProxyDeathRecipient*)client);
        }
    }
    return client->mRingtone->Play();
}

ECode RingtonePlayer::MyRingtonePlayer::Stop(
    /* [in] */ IBinder* token)
{
    if (LOGD) Logger::D(TAG, "stop(token=%p)", token);
    AutoPtr<Client> client;
    synchronized(mHost->mClients) {
        AutoPtr<IInterface> obj;
        mHost->mClients->Remove(token, (IInterface**)&obj);
        client = (Client*)(IObject::Probe(obj));
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

ECode RingtonePlayer::MyRingtonePlayer::IsPlaying(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (LOGD) Logger::D(TAG, "isPlaying(token=%p)", token);
    AutoPtr<Client> client;
    synchronized(mHost->mClients) {
        AutoPtr<IInterface> obj;
        mHost->mClients->Get(token, (IInterface**)&obj);
        client = (Client*)(IObject::Probe(obj));
    }
    if (client != NULL) {
        client->mRingtone->IsPlaying(result);
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode RingtonePlayer::MyRingtonePlayer::SetVolume(
    /* [in] */ IBinder* token,
    /* [in] */ Float volume)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode RingtonePlayer::MyRingtonePlayer::PlayAsync(
    /* [in] */ IUri* uri,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean looping,
    /* [in] */ IAudioAttributes* aa)
{
    if (LOGD) Logger::D(TAG, "playAsync(uri=%p, user=%p)", uri, user);
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Logger::E(TAG, "Async playback only available from system UID.");
        return E_SECURITY_EXCEPTION;
    }
    return mHost->mAsyncPlayer->Play(mHost->GetContextForUser(user), uri, looping, aa);
}

ECode RingtonePlayer::MyRingtonePlayer::StopAsync()
{
    if (LOGD) Logger::D(TAG, "stopAsync()");
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Slogger::E(TAG, "Async playback only available from system UID.");
        return E_SECURITY_EXCEPTION;
    }
    return mHost->mAsyncPlayer->Stop();
}

ECode RingtonePlayer::MyRingtonePlayer::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

//-------------------------------------------
//    RingtonePlayer
//-------------------------------------------
const String RingtonePlayer::TAG("RingtonePlayer");
const Boolean RingtonePlayer::LOGD = FALSE;

RingtonePlayer::RingtonePlayer()
{
}

ECode RingtonePlayer::constructor()
{
    mAsyncPlayer = new NotificationPlayer(TAG);
    mCallback = new MyRingtonePlayer(this);
    return NOERROR;
}

ECode RingtonePlayer::Start()
{
    mAsyncPlayer->SetUsesWakeLock(mContext);

    mAudioService = IIAudioService::Probe(ServiceManager::GetService(IContext::AUDIO_SERVICE));

    ECode ec = mAudioService->SetRingtonePlayer(mCallback);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Problem registering RingtonePlayer: %08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<IContext> RingtonePlayer::GetContextForUser(
    /* [in] */ IUserHandle* user)
{
    String name;
    ECode ec = mContext->GetPackageName(&name);
    if (FAILED(ec)) {
        // return E_NAME_NOT_FOUND_EXCEPTION;
        assert(0);
        //TODO
        return NULL;
    }
    AutoPtr<IContext> context;
    mContext->CreatePackageContextAsUser(name, 0, user, (IContext**)&context);
    return context;
}

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos