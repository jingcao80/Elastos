#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/audiofx/CLoudnessEnhancerSettings.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IStringTokenizer;
using Elastos::Utility::CStringTokenizer;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CLoudnessEnhancerSettings, Object, ILoudnessEnhancerSettings)

CAR_OBJECT_IMPL(CLoudnessEnhancerSettings)

CLoudnessEnhancerSettings::CLoudnessEnhancerSettings()
    : mTargetGainmB(0)
{
}

ECode CLoudnessEnhancerSettings::constructor()
{
    return NOERROR;
}

/**
 * Settings class constructor from a key=value; pairs formatted string. The string is
 * typically returned by Settings.toString() method.
 * @throws IllegalArgumentException if the string is not correctly formatted.
 */
ECode CLoudnessEnhancerSettings::constructor(
    /* [in] */ const String& settings)
{
    AutoPtr<IStringTokenizer> st;
    CStringTokenizer::New(settings, String("=;"), (IStringTokenizer**)&st);
    //int tokens = st.countTokens();
    Int32 count;
    st->CountTokens(&count);
    if (count != 3) {
        // throw new IllegalArgumentException("settings: " + settings);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    String key;
    st->GetNextToken(&key);
    if (!key.Equals("LoudnessEnhancer")) {
        // throw new IllegalArgumentException(
        //         "invalid settings for LoudnessEnhancer: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
        st->GetNextToken(&key);
        if (!key.Equals("targetGainmB")) {
            // throw new IllegalArgumentException("invalid key name: " + key);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        st->GetNextToken(&key);
        StringUtils::Parse(key, &mTargetGainmB);
    //  } catch (NumberFormatException nfe) {
    //     throw new IllegalArgumentException("invalid value for key: " + key);
    // }

    return NOERROR;
}

ECode CLoudnessEnhancerSettings::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb("LoudnessEnhancer");
    sb += ";targetGainmB=";
    sb += mTargetGainmB;

    *result = sb.ToString();
    return NOERROR;
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
