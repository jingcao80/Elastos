//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/media/CMyRingtonePlayer.h"
#include <elastos/droid/os/Binder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::EIID_IIRingtonePlayer;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Media{

//-------------------------------------------
//    CMyRingtonePlayer
//-------------------------------------------
CAR_OBJECT_IMPL(CMyRingtonePlayer)

CAR_INTERFACE_IMPL_2(CMyRingtonePlayer, Object, IIRingtonePlayer, IBinder);

ECode CMyRingtonePlayer::constructor(
    /* [in] */ ISystemUI* host)
{
    mHost = (CRingtonePlayer*)host;
    return NOERROR;
}

ECode CMyRingtonePlayer::Play( // throws RemoteException
    /* [in] */ IBinder* token,
    /* [in] */ IUri* uri,
    /* [in] */ IAudioAttributes* aa)
{
    if (mHost->LOGD) {
        Logger::D(mHost->TAG, "play(token=%p, uri=%p, uid=%d)", token, uri,
            Binder::GetCallingUid());
    }
    AutoPtr<CRingtonePlayer::Client> client;
    {
        AutoLock syncLock(mHost->mClients);
        AutoPtr<IInterface> clientObj;
        mHost->mClients->Get(token, (IInterface**)&clientObj);
        client = (CRingtonePlayer::Client*)(IObject::Probe(clientObj));
        if (client == NULL) {
            const AutoPtr<IUserHandle> user = Binder::GetCallingUserHandle();
            client = new CRingtonePlayer::Client(token, uri, user, aa, mHost);
            IProxy* proxy = (IProxy*)token->Probe(EIID_IProxy);
            proxy->LinkToDeath(client, 0);
            mHost->mClients->Put(token, (IProxyDeathRecipient*)client);
        }
    }
    return client->mRingtone->Play();
}

ECode CMyRingtonePlayer::Stop(
    /* [in] */ IBinder* token)
{
    if (mHost->LOGD) Logger::D(mHost->TAG, "stop(token=%p)", token);
    AutoPtr<CRingtonePlayer::Client> client;
    {
        AutoLock syncLock(mHost->mClients);
        AutoPtr<IInterface> obj;
        mHost->mClients->Remove(token, (IInterface**)&obj);
        client = (CRingtonePlayer::Client*)(IObject::Probe(obj));
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

ECode CMyRingtonePlayer::IsPlaying(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mHost->LOGD) Logger::D(mHost->TAG, "isPlaying(token=%p)", token);
    AutoPtr<CRingtonePlayer::Client> client;
    {
        AutoLock syncLock(mHost->mClients);
        AutoPtr<IInterface> obj;
        mHost->mClients->Get(token, (IInterface**)&obj);
        client = (CRingtonePlayer::Client*)(IObject::Probe(obj));
    }
    if (client != NULL) {
        client->mRingtone->IsPlaying(result);
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode CMyRingtonePlayer::PlayAsync(
    /* [in] */ IUri* uri,
    /* [in] */ IUserHandle* user,
    /* [in] */ Boolean looping,
    /* [in] */ IAudioAttributes* aa)
{
    if (mHost->LOGD) Logger::D(mHost->TAG, "playAsync(uri=%p, user=%p)", uri, user);
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Logger::E(mHost->TAG, "Async playback only available from system UID.");
        return E_SECURITY_EXCEPTION;
    }
    return mHost->mAsyncPlayer->Play(mHost->GetContextForUser(user), uri, looping, aa);
}

ECode CMyRingtonePlayer::StopAsync()
{
    if (mHost->LOGD) Logger::D(mHost->TAG, "stopAsync()");
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Logger::E(mHost->TAG, "Async playback only available from system UID.");
        return E_SECURITY_EXCEPTION;
    }
    return mHost->mAsyncPlayer->Stop();
}

ECode CMyRingtonePlayer::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

ECode CMyRingtonePlayer::SetVolume(
    /* [in] */ IBinder* token,
    /* [in] */ Float volume)
{
    return NOERROR;
}

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
