#include "elastos/droid/media/audiofx/CEqualizerSettings.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CEqualizerSettings, Object, IEqualizerSettings)

CAR_OBJECT_IMPL(CEqualizerSettings)

CEqualizerSettings::CEqualizerSettings()
    : mCurPreset(0)
    , mNumBands(0)
{
}

ECode CEqualizerSettings::constructor()
{
    return NOERROR;
}

/**
 * Settings class constructor from a key=value; pairs formatted string. The string is
 * typically returned by Settings.toString() method.
 * @throws IllegalArgumentException if the string is not correctly formatted.
 */
ECode CEqualizerSettings::constructor(
    /* [in] */ const String& settings)
{
    AutoPtr<ArrayOf<String> > values;
    StringUtils::Split(settings, String("=;"), (ArrayOf<String>**)&values);

    Int32 tokens = values ? values->GetLength() : 0;

    if (tokens < 5) {
       // throw new IllegalArgumentException("settings: " + settings);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 index = 0;
    String key = (*values)[index++];
    if (!key.Equals("Equalizer")) {
       // throw new IllegalArgumentException("invalid settings for Equalizer: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    key = (*values)[index++];
    if (!key.Equals("curPreset")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCurPreset = StringUtils::ParseInt32((*values)[index++]);

    key = (*values)[index++];
    if (!key.Equals("numBands")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mNumBands = StringUtils::ParseInt32((*values)[index++]);
    if (StringUtils::ParseInt32((*values)[index++]) != mNumBands*2) {
       // throw new IllegalArgumentException("settings: " + settings);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    mBandLevels = ArrayOf<Int16>::Alloc(mNumBands);
    for (int i = 0; i < mNumBands; i++) {
        key = (*values)[index++];
        if (!key.Equals(String("band") + StringUtils::ToString(i+1) + String("Level"))) {
           // throw new IllegalArgumentException("invalid key name: " + key);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        (*mBandLevels)[i] = StringUtils::ParseInt32((*values)[index++]);
    }

    // } catch (NumberFormatException nfe) {
    //     throw new IllegalArgumentException("invalid value for key: " + key);
    // }

    return NOERROR;
}

ECode CEqualizerSettings::GetCurPreset(
    /* [out] */ Int16* curPreset)
{
    VALIDATE_NOT_NULL(curPreset);
    *curPreset = mCurPreset;
    return NOERROR;
}

ECode CEqualizerSettings::SetCurPreset(
    /* [in] */ Int16 curPreset)
{
    mCurPreset = curPreset;
    return NOERROR;
}

ECode CEqualizerSettings::GetNumBands(
    /* [out] */ Int16* numBands)
{
    VALIDATE_NOT_NULL(numBands);
    *numBands = mNumBands;
    return NOERROR;
}

ECode CEqualizerSettings::SetNumBands(
    /* [in] */ Int16 numBands)
{
    mNumBands = numBands;
    return NOERROR;
}

ECode CEqualizerSettings::GetBandLevels(
    /* [out] */ ArrayOf<Int16>** bandLevels)
{
    VALIDATE_NOT_NULL(bandLevels);
    *bandLevels = mBandLevels;
    return NOERROR;
}

ECode CEqualizerSettings::SetBandLevels(
    /* [in] */ ArrayOf<Int16>* bandLevels)
{
    mBandLevels = bandLevels;
    return NOERROR;
}

ECode CEqualizerSettings::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb("Equalizer");
    sb += ";curPreset=";
    sb += mCurPreset;
    sb += ";numBands=";
    sb += mNumBands;

    for (int i = 0; i < mNumBands; i++) {
        sb += ";band";
        sb += i + 1;
        sb += "Level=";
        sb += (*mBandLevels)[i];
    }

    *result = sb.ToString();
    return NOERROR;
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
