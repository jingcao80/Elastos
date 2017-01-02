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

#ifndef __ELASTOS_DROID_SPEECH_TTS_CTTSENGINESHELPER_H__
#define __ELASTOS_DROID_SPEECH_TTS_CTTSENGINESHELPER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>
#include "_Elastos_Droid_Speech_Tts_CTtsEnginesHelper.h"
#include "Elastos.Droid.Speech.h"

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

CarClass(CTtsEnginesHelper)
    , public Singleton
    , public ITtsEnginesHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    /**
     * This method tries its best to return a valid {@link Locale} object from the TTS-specific
     * Locale input (returned by {@link TextToSpeech#getLanguage}
     * and {@link TextToSpeech#getDefaultLanguage}). A TTS Locale language field contains
     * a three-letter ISO 639-2/T code (where a proper Locale would use a two-letter ISO 639-1
     * code), and the country field contains a three-letter ISO 3166 country code (where a proper
     * Locale would use a two-letter ISO 3166-1 code).
     *
     * This method tries to convert three-letter language and country codes into their two-letter
     * equivalents. If it fails to do so, it keeps the value from the TTS locale.
     */
    CARAPI NormalizeTTSLocale(
        /* [in] */ ILocale* ttsLocale,
        /* [out] */ ILocale** outLocale);
};

} // namespace Tts
} // namespace Speech
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_CTTSENGINESHELPER_H__
