#include "elastos/droid/media/audiofx/CVirtualizerSettings.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CVirtualizerSettings, Object, IVirtualizerSettings)

CAR_OBJECT_IMPL(CVirtualizerSettings)

CVirtualizerSettings::CVirtualizerSettings()
    : mStrength(0)
{
}

ECode CVirtualizerSettings::constructor()
{
    return NOERROR;
}

/**
 * Settings class constructor from a key=value; pairs formatted string. The string is
 * typically returned by Settings.toString() method.
 * @throws IllegalArgumentException if the string is not correctly formatted.
 */
ECode CVirtualizerSettings::constructor(
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
    if (!key.Equals("Virtualizer")) {
       // throw new IllegalArgumentException("invalid settings for Virtualizer: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    key = (*values)[index++];
    if (!key.Equals("strength")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mStrength = StringUtils::ParseInt16((*values)[index++]);

     // } catch (NumberFormatException nfe) {
     //    throw new IllegalArgumentException("invalid value for key: " + key);
     // }

    return NOERROR;
}

ECode CVirtualizerSettings::GetStrength(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStrength;
    return NOERROR;
}

ECode CVirtualizerSettings::SetStrength(
    /* [in] */ Int16 result)
{
    mStrength = result;
    return NOERROR;
}

ECode CVirtualizerSettings::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb("Virtualizer");
    sb += ";strength=";
    sb += mStrength;

    *result = sb.ToString();
    return NOERROR;
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
