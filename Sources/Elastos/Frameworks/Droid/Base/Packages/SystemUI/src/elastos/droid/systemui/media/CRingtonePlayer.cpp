#include "elastos/droid/systemui/media/CRingtonePlayer.h"
#include "elastos/droid/systemui/media/CMyRingtonePlayer.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Media::CRingtone;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

//-------------------------------------------
//    CRingtonePlayer::Client
//-------------------------------------------
CAR_INTERFACE_IMPL(CRingtonePlayer::Client, Object, IProxyDeathRecipient)

CRingtonePlayer::Client::Client(
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ IUserHandle* user,
    /* [in] */ IAudioAttributes* aa,
    /* [in] */ CRingtonePlayer* host)
    : mToken(token)
    , mHost(host)
{
    CRingtone::New(mHost->GetContextForUser(user), FALSE, (IRingtone**)&mRingtone);
    mRingtone->SetAudioAttributes(aa);
    mRingtone->SetUri(uri);
}

ECode CRingtonePlayer::Client::ProxyDied()
{
    if (LOGD) Logger::D(TAG, "binderDied() token = %p", mToken.Get());
    synchronized (mHost->mClients) {
        mHost->mClients->Remove(mToken);
    }
    return mRingtone->Stop();
}

//-------------------------------------------
//    CRingtonePlayer
//-------------------------------------------
const String CRingtonePlayer::TAG("CRingtonePlayer");
const Boolean CRingtonePlayer::LOGD = FALSE;
CRingtonePlayer::CRingtonePlayer()
{
    mAsyncPlayer = new NotificationPlayer(TAG);
    CMyRingtonePlayer::New(this, (IIRingtonePlayer**)&mCallback);
}

ECode CRingtonePlayer::Start()
{
    mAsyncPlayer->SetUsesWakeLock(mContext);
    Slogger::I(TAG, "TODO: Not Debug: AudioService");
    // mAudioService = IIAudioService::Probe(ServiceManager::GetService(IContext::AUDIO_SERVICE));

    // ECode ec = mAudioService->SetRingtonePlayer(mCallback);
    // if (FAILED(ec)) {
    //     Slogger::E(TAG, "Problem registering RingtonePlayer: %08x", ec);
    //     return E_REMOTE_EXCEPTION;
    // }
    return NOERROR;
}

AutoPtr<IContext> CRingtonePlayer::GetContextForUser(
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