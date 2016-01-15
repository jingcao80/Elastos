#include "elastos/droid/media/audiofx/CPresetReverbSettings.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CPresetReverbSettings, Object, IPresetReverbSettings)

CAR_OBJECT_IMPL(CPresetReverbSettings)

CPresetReverbSettings::CPresetReverbSettings()
    : mPreset(0)
{
}

ECode CPresetReverbSettings::constructor()
{
    return NOERROR;
}

/**
 * Settings class constructor from a key=value; pairs formatted string. The string is
 * typically returned by Settings.toString() method.
 * @throws IllegalArgumentException if the string is not correctly formatted.
 */
ECode CPresetReverbSettings::constructor(
    /* [in] */ const String& settings)
{
    AutoPtr<ArrayOf<String> > values;
    StringUtils::Split(settings, String("=;"), (ArrayOf<String>**)&values);

    Int32 tokens = values ? values->GetLength() : 0;

    if (tokens != 3) {
       // throw new IllegalArgumentException("settings: " + settings);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 index = 0;
    String key = (*values)[index++];
    if (!key.Equals("PresetReverb")) {
       // throw new IllegalArgumentException("invalid settings for PresetReverb: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    key = (*values)[index++];
    if (!key.Equals("preset")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mPreset = StringUtils::ParseInt16((*values)[index++]);

     // } catch (NumberFormatException nfe) {
     //    throw new IllegalArgumentException("invalid value for key: " + key);
     // }

    return NOERROR;
}

ECode CPresetReverbSettings::GetPreset(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPreset;
    return NOERROR;
}

ECode CPresetReverbSettings::SetPreset(
    /* [in] */ Int16 result)
{
    mPreset = result;
    return NOERROR;
}

ECode CPresetReverbSettings::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb("PresetReverb");
    sb += ";preset=";
    sb += mPreset;

    *result = sb.ToString();
    return NOERROR;
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
