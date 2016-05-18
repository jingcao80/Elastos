
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
