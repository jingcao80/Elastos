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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/soundtrigger/SoundTrigger.h"
#include "elastos/droid/hardware/soundtrigger/SoundTriggerModule.h"
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerModuleProperties.h"
#include <elastos/utility/logging/Logger.h>
#include <soundtrigger/SoundTrigger.h>

using Elastos::Droid::Hardware::Soundtrigger::CSoundTriggerModuleProperties;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL(SoundTrigger, Object, ISoundTrigger)

enum {
    SOUNDTRIGGER_STATUS_OK = 0,
    SOUNDTRIGGER_STATUS_ERROR = INT_MIN,
    SOUNDTRIGGER_PERMISSION_DENIED = -1,
    SOUNDTRIGGER_STATUS_NO_INIT = -19,
    SOUNDTRIGGER_STATUS_BAD_VALUE = -22,
    SOUNDTRIGGER_STATUS_DEAD_OBJECT = -32,
    SOUNDTRIGGER_INVALID_OPERATION = -38,
};

ECode SoundTrigger::NativeListModules(
    /* [in] */ IList* modules,
    /* [out] */ Int32* result)
{
//     ALOGV("listModules");

    if (modules == NULL) {
        Logger::E(String("SoundTrigger"), String("listModules NULL AudioPatch ArrayList"));
        return SOUNDTRIGGER_STATUS_BAD_VALUE;
    }
//     if (!env->IsInstanceOf(jModules, gArrayListClass)) {
//         ALOGE("listModules not an arraylist");
//         return SOUNDTRIGGER_STATUS_BAD_VALUE;
//     }

    unsigned int numModules = 0;
    struct sound_trigger_module_descriptor *nModules = NULL;

    android::status_t status = android::SoundTrigger::listModules(nModules, &numModules);
    if (status != android::NO_ERROR || numModules == 0) {
        *result = (Int32)status;
        return NOERROR;
    }

    nModules = (struct sound_trigger_module_descriptor *)
                            calloc(numModules, sizeof(struct sound_trigger_module_descriptor));

    status = android::SoundTrigger::listModules(nModules, &numModules);
    //ALOGV("listModules SoundTrigger::listModules status %d numModules %d", status, numModules);

    if (status != android::NO_ERROR) {
        numModules = 0;
    }

    for (size_t i = 0; i < numModules; i++) {
        char str[SOUND_TRIGGER_MAX_STRING_LEN];

        String implementor = String(nModules[i].properties.implementor);
        String description = String(nModules[i].properties.description);
        android::SoundTrigger::guidToString(&nModules[i].properties.uuid,
                                   str,
                                   SOUND_TRIGGER_MAX_STRING_LEN);
        String uuid = String(str);

        // ALOGV("listModules module %zu id %d description %s maxSoundModels %d",
        //       i, nModules[i].handle, nModules[i].properties.description,
        //       nModules[i].properties.max_sound_models);

        AutoPtr<ISoundTriggerModuleProperties> newModuleDesc;
        CSoundTriggerModuleProperties::New(nModules[i].handle,
                                               implementor, description, uuid,
                                               nModules[i].properties.version,
                                               nModules[i].properties.max_sound_models,
                                               nModules[i].properties.max_key_phrases,
                                               nModules[i].properties.max_users,
                                               nModules[i].properties.recognition_modes,
                                               nModules[i].properties.capture_transition,
                                               nModules[i].properties.max_buffer_ms,
                                               nModules[i].properties.concurrent_capture,
                                               nModules[i].properties.power_consumption_mw,
                                               nModules[i].properties.trigger_in_event,
                                               (ISoundTriggerModuleProperties**)&newModuleDesc);

//         env->DeleteLocalRef(implementor);
//         env->DeleteLocalRef(description);
//         env->DeleteLocalRef(uuid);
        if (newModuleDesc == NULL) {
            status = SOUNDTRIGGER_STATUS_ERROR;
            goto exit;
        }
        //env->CallBooleanMethod(jModules, gArrayListMethods.add, newModuleDesc);
        Boolean _result;
        FAIL_RETURN(modules->Add(TO_IINTERFACE(newModuleDesc), &_result));
    }

exit:
    free(nModules);
    *result = (Int32) status;
    return NOERROR;
}

ECode SoundTrigger::AttachModule(
    /* [in] */ Int32 moduleId,
    /* [in] */ ISoundTriggerStatusListener* listener,
    /* [in] */ IHandler* handler,
    /* [out] */ ISoundTriggerModule** result)
{
    VALIDATE_NOT_NULL(result);

    if (listener == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ISoundTriggerModule> _module = new SoundTriggerModule();
    FAIL_RETURN(((SoundTriggerModule*)_module.Get())->constructor(moduleId, listener, handler))
    *result = _module;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Soundtrigger
} // namespace Hardware
} // namespace Droid
} // namespace Elastos