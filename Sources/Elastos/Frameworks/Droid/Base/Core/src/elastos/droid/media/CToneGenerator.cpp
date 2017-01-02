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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/CToneGenerator.h"
#include <media/AudioSystem.h>
#include <media/ToneGenerator.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using namespace Elastos::Core;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_OBJECT_IMPL(CToneGenerator)

CAR_INTERFACE_IMPL(CToneGenerator, Object, IToneGenerator)

CToneGenerator::CToneGenerator()
    : mNativeContext(0)
{}

CToneGenerator::~CToneGenerator()
{
    NativeFinalize();
}

ECode CToneGenerator::constructor(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 volume)
{
    return NativeSetup(streamType, volume);
}

ECode CToneGenerator::StartTone(
    /* [in] */ Int32 toneType,
    /* [out] */ Boolean* result)
{
    return StartTone(toneType, -1, result);
}

ECode CToneGenerator::StartTone(
    /* [in] */ Int32 toneType,
    /* [in] */ Int32 durationMs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    // LOGV("android_media_ToneGenerator_startTone: %x\n", (int)thiz);

    android::ToneGenerator *lpToneGen = (android::ToneGenerator *)mNativeContext;
    if (lpToneGen == NULL) {
        Slogger::E("CToneGenerator", "Method called after release()");
        return E_RUNTIME_EXCEPTION;
    }

    *result = (Boolean)lpToneGen->startTone((android::ToneGenerator::tone_type)toneType, durationMs);
    return NOERROR;
}

/**
 * This method stops the tone currently playing playback.
 * @see #ToneGenerator(int, int)
 */
ECode CToneGenerator::StopTone()
{
    // LOGV("android_media_ToneGenerator_stopTone: %x\n", (int)thiz);

    android::ToneGenerator *lpToneGen = (android::ToneGenerator *)mNativeContext;

    // LOGV("ToneGenerator lpToneGen: %x\n", (unsigned int)lpToneGen);
    if (lpToneGen == NULL) {
        Slogger::E("CToneGenerator", "Method called after release()");
        return E_RUNTIME_EXCEPTION;
    }

    lpToneGen->stopTone();
    return NOERROR;
}

/**
 * Releases resources associated with this ToneGenerator object. It is good
 * practice to call this method when you're done using the ToneGenerator.
 */
ECode CToneGenerator::ReleaseResources()
{
    android::ToneGenerator *lpToneGen = (android::ToneGenerator *)mNativeContext;
    // LOGV("android_media_ToneGenerator_release lpToneGen: %x\n", (int)lpToneGen);

    if (lpToneGen) {
        delete lpToneGen;
        mNativeContext = 0;
    }
    return NOERROR;
}

ECode CToneGenerator::GetAudioSessionId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    android::ToneGenerator *lpToneGen = (android::ToneGenerator *)mNativeContext;

    // LOGV("ToneGenerator lpToneGen: %x\n", (unsigned int)lpToneGen);
    if (lpToneGen == NULL) {
        Slogger::E("CToneGenerator", "Method called after release()");
        return E_RUNTIME_EXCEPTION;
    }

    *result = lpToneGen->getSessionId();
    return NOERROR;
}

ECode CToneGenerator::NativeSetup(
    /* [in] */ Int32 streamType,
    /* [in] */ Int32 volume)
{
    android::ToneGenerator *lpToneGen = new android::ToneGenerator(
        (audio_stream_type_t)streamType, android::AudioSystem::linearToLog(volume), true);

    // LOGV("android_media_ToneGenerator_native_setup jobject: %x\n", (int)thiz);

    if (lpToneGen == NULL) {
        Slogger::E("CToneGenerator", "ToneGenerator creation failed");
        return E_OUT_OF_MEMORY_ERROR;
    }
    // LOGV("ToneGenerator lpToneGen: %x\n", (unsigned int)lpToneGen);

    if (!lpToneGen->isInited()) {
        Slogger::E("CToneGenerator", "ToneGenerator init failed.");
        // jniThrowException(env, "java/lang/RuntimeException", "Init failed");
        return E_RUNTIME_EXCEPTION;
    }

    // Stow our new C++ ToneGenerator in an opaque field in the Java object.
    mNativeContext = (Handle32)lpToneGen;

    // LOGV("ToneGenerator fields.context: %x\n", env->GetIntField(thiz, fields.context));

    return NOERROR;
}

void CToneGenerator::NativeFinalize()
{
    // LOGV("android_media_ToneGenerator_native_finalize jobject: %x\n", (int)thiz);

    android::ToneGenerator *lpToneGen = (android::ToneGenerator *)mNativeContext;

    if (lpToneGen) {
        // LOGV("delete lpToneGen: %x\n", (int)lpToneGen);
        delete lpToneGen;
        mNativeContext = 0;
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
