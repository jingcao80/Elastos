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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/systemui/media/CRingtonePlayer.h"
#include "elastos/droid/systemui/media/CMyRingtonePlayer.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Media::CRingtone;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Core::AutoLock;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

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
    {
        AutoLock syncLock(mHost->mClients);
        mHost->mClients->Remove(mToken);
    }
    return mRingtone->Stop();
}

//-------------------------------------------
//    CRingtonePlayer
//-------------------------------------------
const String CRingtonePlayer::TAG("CRingtonePlayer");
const Boolean CRingtonePlayer::LOGD = FALSE;

CAR_OBJECT_IMPL(CRingtonePlayer)

CRingtonePlayer::CRingtonePlayer()
{
    CHashMap::New((IHashMap**)&mClients);
    mAsyncPlayer = new NotificationPlayer(TAG);
    CMyRingtonePlayer::New(this, (IIRingtonePlayer**)&mCallback);
}

ECode CRingtonePlayer::Start()
{
    mAsyncPlayer->SetUsesWakeLock(mContext);
    mAudioService = IIAudioService::Probe(ServiceManager::GetService(IContext::AUDIO_SERVICE));
    ECode ec = mAudioService->SetRingtonePlayer(mCallback);
    if (FAILED(ec)) {
        Logger::E(TAG, "Problem registering RingtonePlayer: %08x", ec);
        return E_REMOTE_EXCEPTION;
    }
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