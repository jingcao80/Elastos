#ifndef __ELASTOS_DROID_SPEECH_TTS_SynthesisRequest_H__
#define __ELASTOS_DROID_SPEECH_TTS_SynthesisRequest_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include "Elastos.Droid.Speech.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/**
 * Contains data required by engines to synthesize speech. This data is:
 * <ul>
 *   <li>The text to synthesize</li>
 *   <li>The synthesis locale, represented as a language, country and a variant.
 *   The language is an ISO 639-3 letter language code, and the country is an
 *   ISO 3166 alpha 3 code. The variant is not specified.</li>
 *   <li>The name of the voice requested for this synthesis. May be empty if
 *   the client uses {@link TextToSpeech#setLanguage} instead of
 *   {@link TextToSpeech#setVoice}</li>
 *   <li>The synthesis speech rate, with 100 being the normal, and
 *   higher values representing higher speech rates.</li>
 *   <li>The voice pitch, with 100 being the default pitch.</li>
 * </ul>
 *
 * Any additional parameters sent to the text to speech service are passed in
 * uninterpreted, see the {@code params} argument in {@link TextToSpeech#speak}
 * and {@link TextToSpeech#synthesizeToFile}.
 */
//final
class SynthesisRequest
    : public Object
    , public ISynthesisRequest
{
public:
    CAR_INTERFACE_DECL();

    SynthesisRequest();

    virtual ~SynthesisRequest();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& text,
        /* [in] */ IBundle* params);

    CARAPI constructor(
        /* [in] */ ICharSequence* text,
        /* [in] */ IBundle* params);

    /**
     * Gets the text which should be synthesized.
     */
    CARAPI GetText(
        /* [out] */ String* ret);

    /**
     * Gets the text which should be synthesized.
     */
    CARAPI GetCharSequenceText(
        /* [out] */ ICharSequence** ret);

    /**
     * Gets the name of the voice to use.
     */
    CARAPI GetVoiceName(
        /* [out] */ String* ret);

    /**
     * Gets the ISO 3-letter language code for the language to use.
     */
    CARAPI GetLanguage(
        /* [out] */ String* ret);

    /**
     * Gets the ISO 3-letter country code for the language to use.
     */
    CARAPI GetCountry(
        /* [out] */ String* ret);

    /**
     * Gets the language variant to use.
     */
    CARAPI GetVariant(
        /* [out] */ String* ret);

    /**
     * Gets the speech rate to use. The normal rate is 100.
     */
    CARAPI GetSpeechRate(
        /* [out] */ Int32* ret);

    /**
     * Gets the pitch to use. The normal pitch is 100.
     */
    CARAPI GetPitch(
        /* [out] */ Int32* ret);

    /**
     * Gets the additional params, if any.
     */
    CARAPI GetParams(
        /* [out] */ IBundle** ret);

    /**
     * Gets the request caller Uid.
     */
    CARAPI GetCallerUid(
        /* [out] */ Int32* ret);

    /**
     * Sets the locale for the request.
     */
    CARAPI SetLanguage(
        /* [in] */ const String& language,
        /* [in] */ const String& country,
        /* [in] */ const String& variant);
    /**
     * Sets the voice name for the request.
     */
    CARAPI SetVoiceName(
        /* [in] */ const String& voiceName);

    /**
     * Sets the speech rate.
     */
    CARAPI SetSpeechRate(
        /* [in] */ Int32 speechRate);

    /**
     * Sets the pitch.
     */
    CARAPI SetPitch(
        /* [in] */ Int32 pitch);

    /**
     * Sets Caller Uid
     */
    CARAPI SetCallerUid(
        /* [in] */ Int32 uid);

private:
    AutoPtr<ICharSequence> mText;
    String mVoiceName;

    AutoPtr<IBundle> mParams;
    String mLanguage;
    String mCountry;
    String mVariant;
    Int32 mSpeechRate;
    Int32 mPitch;
    Int32 mCallerUid;
};

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_TTS_SynthesisRequest_H__
