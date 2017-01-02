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

#include "elastos/droid/service/voice/CAlwaysOnHotwordDetector.h"
#include "elastos/droid/service/voice/CAlwaysOnHotwordDetectorEventPayload.h"
#include "elastos/droid/service/voice/CAlwaysOnHotwordDetectorSoundTriggerListener.h"
#include "elastos/droid/service/voice/CVoiceInteractionService.h"
#include "elastos/droid/service/voice/CVoiceInteractionServiceInfo.h"
#include "elastos/droid/service/voice/CVoiceInteractionSessionRequest.h"
#include "elastos/droid/service/voice/CVoiceInteractionSessionCaller.h"
#include "elastos/droid/service/voice/CVoiceInteractionSessionInsets.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

CAR_OBJECT_IMPL(CAlwaysOnHotwordDetector)
CAR_OBJECT_IMPL(CAlwaysOnHotwordDetectorEventPayload)
CAR_OBJECT_IMPL(CAlwaysOnHotwordDetectorSoundTriggerListener)
CAR_OBJECT_IMPL(CVoiceInteractionService)
CAR_OBJECT_IMPL(CVoiceInteractionServiceInfo)
CAR_OBJECT_IMPL(CVoiceInteractionSessionRequest)
CAR_OBJECT_IMPL(CVoiceInteractionSessionCaller)
CAR_OBJECT_IMPL(CVoiceInteractionSessionInsets)

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos