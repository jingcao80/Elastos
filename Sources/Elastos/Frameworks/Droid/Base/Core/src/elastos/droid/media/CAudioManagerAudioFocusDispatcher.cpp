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

#include "elastos/droid/media/CAudioManagerAudioFocusDispatcher.h"
#include "elastos/droid/media/CAudioManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL_2(CAudioManagerAudioFocusDispatcher, Object, IIAudioFocusDispatcher, IBinder)

CAR_OBJECT_IMPL(CAudioManagerAudioFocusDispatcher)

CAudioManagerAudioFocusDispatcher::CAudioManagerAudioFocusDispatcher()
{
}

CAudioManagerAudioFocusDispatcher::~CAudioManagerAudioFocusDispatcher()
{
}

ECode CAudioManagerAudioFocusDispatcher::constructor(
    /* [in] */ IAudioManager* owner)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(owner);
    assert(wrs != NULL);
    wrs->GetWeakReference((IWeakReference**)&mOwnerWeak);
    return NOERROR;
}

ECode CAudioManagerAudioFocusDispatcher::DispatchAudioFocusChange(
    /* [in] */ Int32 focusChange,
    /* [in] */ const String& id)
{
    AutoPtr<IAudioManager> am;
    mOwnerWeak->Resolve(EIID_IAudioManager, (IInterface**)&am);
    if (am == NULL)
        return NOERROR;

    AutoPtr<ICharSequence> seq;
    CString::New(id, (ICharSequence**)&seq);

    AutoPtr<IHandler> handler = ((CAudioManager*)am.Get())->mAudioFocusEventHandlerDelegate->GetHandler();
    AutoPtr<IMessage> msg;
    handler->ObtainMessage(focusChange, seq, (IMessage**)&msg);
    Boolean result;
    return handler->SendMessage(msg, &result);
}

ECode CAudioManagerAudioFocusDispatcher::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("CAudioManagerAudioFocusDispatcher");
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
