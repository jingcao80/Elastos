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

#include "elastos/droid/server/telecom/HeadsetMediaButton.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.Droid.View.h>

using Elastos::Droid::Media::Session::EIID_IMediaSessionCallback;
using Elastos::Droid::Media::Session::CMediaSession;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// HeadsetMediaButton::SubMediaSessionCallback
//=============================================================================
HeadsetMediaButton::SubMediaSessionCallback::SubMediaSessionCallback(
    /* [in] */ HeadsetMediaButton* host)
    : mHost(host)
{}

ECode HeadsetMediaButton::SubMediaSessionCallback::OnMediaButtonEvent(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IParcelable> parcelable;
    intent->GetParcelableExtra(IIntent::EXTRA_KEY_EVENT, (IParcelable**)&parcelable);
    AutoPtr<IKeyEvent> event = IKeyEvent::Probe(parcelable);
    Log::V("HeadsetMediaButton", "SessionCallback->OnMediaButton()...  event = %s.", TO_CSTR(event));
    Int32 keyCode;
    event->GetKeyCode(&keyCode);
    if ((event != NULL) && (keyCode == IKeyEvent::KEYCODE_HEADSETHOOK)) {
        Log::V("HeadsetMediaButton", "SessionCallback: HEADSETHOOK");
        Boolean consumed;
        mHost->HandleHeadsetHook(event, &consumed);
        Log::V("HeadsetMediaButton", "==> handleHeadsetHook(): consumed = %d.", consumed);
        *result = consumed;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

//=============================================================================
// HeadsetMediaButton
//=============================================================================
const Int32 HeadsetMediaButton::SHORT_PRESS = 1;
const Int32 HeadsetMediaButton::LONG_PRESS = 2;
AutoPtr<IAudioAttributes> HeadsetMediaButton::AUDIO_ATTRIBUTES = InitAUDIO_ATTRIBUTES();

ECode HeadsetMediaButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ CallsManager* callsManager)
{
    mSessionCallback = new SubMediaSessionCallback(this);

    IWeakReferenceSource* source = IWeakReferenceSource::Probe(TO_IINTERFACE(callsManager));
    source->GetWeakReference((IWeakReference**)&mCallsManager);

    // Create a MediaSession but don't enable it yet. This is a
    // replacement for MediaButtonReceiver
    CMediaSession::New(context, String("HeadsetMediaButton"), (IMediaSession**)&mSession);
    mSession->SetCallback(mSessionCallback);
    mSession->SetFlags(IMediaSession::FLAG_EXCLUSIVE_GLOBAL_PRIORITY
            | IMediaSession::FLAG_HANDLES_MEDIA_BUTTONS);
    mSession->SetPlaybackToLocal(AUDIO_ATTRIBUTES);
    return NOERROR;
}

ECode HeadsetMediaButton::HandleHeadsetHook(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 action;
    event->GetAction(&action);
    Int32 repeatCount;
    event->GetRepeatCount(&repeatCount);
    Log::D("HeadsetMediaButton", "handleHeadsetHook()...%d %d", action, repeatCount);
    Boolean isLongPress;
    event->IsLongPress(&isLongPress);
    AutoPtr<IInterface> wsObj;
    mCallsManager->Resolve(EIID_IInterface, (IInterface**)&wsObj);
    if (isLongPress) {
        if (wsObj != NULL) {
            return ((CallsManager*) IObject::Probe(wsObj.Get()))->OnMediaButton(LONG_PRESS, result);
        } else {
            Log::D("HeadsetMediaButton", "mCallsManager is NULL");
            assert(0);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    } else if (action == IKeyEvent::ACTION_UP && repeatCount == 0) {
        if (wsObj != NULL) {
            return ((CallsManager*) IObject::Probe(wsObj.Get()))->OnMediaButton(SHORT_PRESS, result);
        } else {
            Log::D("HeadsetMediaButton", "mCallsManager is NULL");
            assert(0);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode HeadsetMediaButton::OnCallAdded(
    /* [in] */ ICall* call)
{
    Boolean isActive;
    mSession->IsActive(&isActive);
    if (!isActive) {
        mSession->SetActive(TRUE);
    }
    return NOERROR;
}

ECode HeadsetMediaButton::OnCallRemoved(
    /* [in] */ ICall* call)
{
    Boolean hasAnyCalls;
    AutoPtr<IInterface> wsObj;
        mCallsManager->Resolve(EIID_IInterface, (IInterface**)&wsObj);
    if (wsObj != NULL) {
        ((CallsManager*) IObject::Probe(wsObj.Get()))->HasAnyCalls(&hasAnyCalls);
    } else {
        Log::D("HeadsetMediaButton", "mCallsManager is NULL");
        assert(0);
    }
    if (!hasAnyCalls) {
        Boolean isActive;
        mSession->IsActive(&isActive);
        if (isActive) {
            mSession->SetActive(FALSE);
        }
    }
    return NOERROR;
}

AutoPtr<IAudioAttributes> HeadsetMediaButton::InitAUDIO_ATTRIBUTES()
{
    AutoPtr<IAudioAttributes> rev;
    AutoPtr<IAudioAttributesBuilder> audioAttributesBuilder;
    CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&audioAttributesBuilder);
    audioAttributesBuilder->SetContentType(IAudioAttributes::CONTENT_TYPE_SPEECH);
    audioAttributesBuilder->SetUsage(IAudioAttributes::USAGE_VOICE_COMMUNICATION);
    audioAttributesBuilder->Build((IAudioAttributes**)&rev);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
