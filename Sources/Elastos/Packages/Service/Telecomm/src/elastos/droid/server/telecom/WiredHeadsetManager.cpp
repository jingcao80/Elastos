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

#include "elastos/droid/server/telecom/WiredHeadsetManager.h"
#include "elastos/droid/server/telecom/CWiredHeadsetManagerWiredHeadsetBroadcastReceiver.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <Elastos.Droid.Media.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Utility::CCollections;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// WiredHeadsetManager::WiredHeadsetBroadcastReceiver
//=============================================================================
CAR_INTERFACE_IMPL(WiredHeadsetManager::WiredHeadsetBroadcastReceiver, BroadcastReceiver, IWiredHeadsetManagerWiredHeadsetBroadcastReceiver)

ECode WiredHeadsetManager::WiredHeadsetBroadcastReceiver::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (WiredHeadsetManager*) IObject::Probe(host);
    return NOERROR;
}


ECode WiredHeadsetManager::WiredHeadsetBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_HEADSET_PLUG)) {
        Int32 state;
        intent->GetInt32Extra(String("state"), 0, &state);
        Boolean isPluggedIn = state == 1;
        Log::V("WiredHeadsetManager", "ACTION_HEADSET_PLUG event, plugged in: %d",
                isPluggedIn);
        mHost->OnHeadsetPluggedInChanged(isPluggedIn);
    }
    return NOERROR;
}

//=============================================================================
// WiredHeadsetManager
//=============================================================================
WiredHeadsetManager::WiredHeadsetManager()
{
    AutoPtr<IMap> map;
    CConcurrentHashMap::New(8, 0.9f, 1, (IMap**)&map);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->SetFromMap(map, (ISet**)&mListeners);
}

ECode WiredHeadsetManager::constructor(
    /* [in] */ IContext* context)
{
    CWiredHeadsetManagerWiredHeadsetBroadcastReceiver::New(TO_IINTERFACE(this), (IWiredHeadsetManagerWiredHeadsetBroadcastReceiver**)&mReceiver);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    AutoPtr<IAudioManager> audioManager = IAudioManager::Probe(obj);
    audioManager->IsWiredHeadsetOn(&mIsPluggedIn);
    // Register for misc other intent broadcasts.
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IIntent::ACTION_HEADSET_PLUG, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(IBroadcastReceiver::Probe(mReceiver), intentFilter, (IIntent**)&intent);
    return NOERROR;
}

ECode WiredHeadsetManager::AddListener(
    /* [in] */ IWiredHeadsetManagerListener* listener)
{
    mListeners->Add(listener);
    return NOERROR;
}

ECode WiredHeadsetManager::RemoveListener(
    /* [in] */ IWiredHeadsetManagerListener* listener)
{
    if (listener != NULL) {
        mListeners->Remove(listener);
    }
    return NOERROR;
}

ECode WiredHeadsetManager::IsPluggedIn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsPluggedIn;
    return NOERROR;
}

ECode WiredHeadsetManager::OnHeadsetPluggedInChanged(
    /* [in] */ Boolean isPluggedIn)
{
    if (mIsPluggedIn != isPluggedIn) {
        Log::V("WiredHeadsetManager", "onHeadsetPluggedInChanged, mIsPluggedIn: %d -> %d", mIsPluggedIn,
                isPluggedIn);
        Boolean oldIsPluggedIn = mIsPluggedIn;
        mIsPluggedIn = isPluggedIn;
        AutoPtr<IIterator> it;
        mListeners->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IWiredHeadsetManagerListener> listener = IWiredHeadsetManagerListener::Probe(obj);
            listener->OnWiredHeadsetPluggedInChanged(oldIsPluggedIn, mIsPluggedIn);
        }
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
