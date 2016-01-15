#include "elastos/droid/media/audiofx/CEnvironmentalReverbSettings.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CEnvironmentalReverbSettings, Object, IEnvironmentalReverbSettings)

CAR_OBJECT_IMPL(CEnvironmentalReverbSettings)

CEnvironmentalReverbSettings::CEnvironmentalReverbSettings()
    : mRoomLevel(0)
    , mRoomHFLevel(0)
    , mDecayTime(0)
    , mDecayHFRatio(0)
    , mReflectionsLevel(0)
    , mReflectionsDelay(0)
    , mReverbLevel(0)
    , mReverbDelay(0)
    , mDiffusion(0)
    , mDensity(0)
{
}

ECode CEnvironmentalReverbSettings::constructor()
{
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::constructor(
    /* [in] */ const String& settings)
{
    AutoPtr<ArrayOf<String> > values;
    StringUtils::Split(settings, String("=;"), (ArrayOf<String>**)&values);

    Int32 tokens = values ? values->GetLength() : 0;

    if (tokens != 21) {
       // throw new IllegalArgumentException("settings: " + settings);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 index = 0;
    String key = (*values)[index++];
    if (!key.Equals("EnvironmentalReverb")) {
       // throw new IllegalArgumentException("invalid settings for EnvironmentalReverb: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // try {
    key = (*values)[index++];
    if (!key.Equals("roomLevel")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRoomLevel = StringUtils::ParseInt32((*values)[index++]);

    if (!key.Equals("roomHFLevel")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRoomHFLevel = StringUtils::ParseInt32((*values)[index++]);

    if (!key.Equals("decayTime")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDecayTime = StringUtils::ParseInt32((*values)[index++]);

    if (!key.Equals("decayHFRatio")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDecayHFRatio = StringUtils::ParseInt32((*values)[index++]);

    if (!key.Equals("reflectionsLevel")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mReflectionsLevel = StringUtils::ParseInt32((*values)[index++]);

    if (!key.Equals("reflectionsDelay")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mReflectionsDelay = StringUtils::ParseInt32((*values)[index++]);

    if (!key.Equals("reverbLevel")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mReverbLevel = StringUtils::ParseInt32((*values)[index++]);

    if (!key.Equals("reverbDelay")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mReverbDelay = StringUtils::ParseInt32((*values)[index++]);

    if (!key.Equals("diffusion")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDiffusion = StringUtils::ParseInt32((*values)[index++]);

    if (!key.Equals("density")) {
       // throw new IllegalArgumentException("invalid key name: " + key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mDensity = StringUtils::ParseInt32((*values)[index++]);

     // } catch (NumberFormatException nfe) {
     //    throw new IllegalArgumentException("invalid value for key: " + key);
     // }

    return NOERROR;
}

ECode CEnvironmentalReverbSettings::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb("EnvironmentalReverb");
    sb += ";roomLevel=";
    sb += mRoomLevel;
    sb += ";roomHFLevel=";
    sb += mRoomHFLevel;
    sb += ";decayTime=";
    sb += mDecayTime;
    sb += ";decayHFRatio=";
    sb += mDecayHFRatio;
    sb += ";reflectionsLevel=";
    sb += mReflectionsLevel;
    sb += ";reflectionsDelay=";
    sb += mReflectionsDelay;
    sb += ";reverbLevel=";
    sb += mReverbLevel;
    sb += ";reverbDelay=";
    sb += mReverbDelay;
    sb += ";diffusion=";
    sb += mDiffusion;
    sb += ";density=";
    sb += mDensity;

    *result = sb.ToString();
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetRoomLevel(
    /* [out] */ Int16* roomLevel)
{
    VALIDATE_NOT_NULL(roomLevel);
    *roomLevel = mRoomLevel;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::SetRoomLevel(
    /* [in] */ Int16 roomLevel)
{
    mRoomLevel = roomLevel;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetRoomHFLevel(
    /* [out] */ Int16* roomHFLevel)
{
    VALIDATE_NOT_NULL(roomHFLevel);
    *roomHFLevel = mRoomHFLevel;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::SetRoomHFLevel(
    /* [in] */ Int16 roomHFLevel)
{
    mRoomHFLevel = roomHFLevel;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetDecayTime(
    /* [out] */ Int32* decayTime)
{
    VALIDATE_NOT_NULL(decayTime);
    *decayTime = mDecayTime;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::SetDecayTime(
    /* [in] */ Int32 decayTime)
{
    mDecayTime = decayTime;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetDecayHFRatio(
    /* [out] */ Int16* decayHFRatio)
{
    VALIDATE_NOT_NULL(decayHFRatio);
    *decayHFRatio = mDecayHFRatio;
    return NOERROR;
}
ECode CEnvironmentalReverbSettings::SetDecayHFRatio(
    /* [in] */ Int16 decayHFRatio)
{
    mDecayHFRatio = decayHFRatio;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetReflectionsLevel(
    /* [out] */ Int16* reflectionsLevel)
{
    VALIDATE_NOT_NULL(reflectionsLevel);
    *reflectionsLevel = mReflectionsLevel;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::SetReflectionsLevel(
    /* [in] */ Int16 reflectionsLevel)
{
    mReflectionsLevel = reflectionsLevel;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetReflectionsDelay(
    /* [out] */ Int32* reflectionsDelay)
{
    VALIDATE_NOT_NULL(reflectionsDelay);
    *reflectionsDelay = mReflectionsDelay;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::SetReflectionsDelay(
    /* [in] */ Int32 reflectionsDelay)
{
    mReflectionsDelay = reflectionsDelay;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetReverbLevel(
    /* [out] */ Int16* reverbLevel)
{
    VALIDATE_NOT_NULL(reverbLevel);
    *reverbLevel = mReverbLevel;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::SetReverbLevel(
    /* [in] */ Int16 reverbLevel)
{
    mReverbLevel = reverbLevel;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetReverbDelay(
    /* [out] */ Int32* reverbDelay)
{
    VALIDATE_NOT_NULL(reverbDelay);
    *reverbDelay = mReverbDelay;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::SetReverbDelay(
    /* [in] */ Int32 reverbDelay)
{
    mReverbDelay = reverbDelay;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetDiffusion(
    /* [out] */ Int16* diffusion)
{
    VALIDATE_NOT_NULL(diffusion);
    *diffusion = mDiffusion;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::SetDiffusion(
    /* [in] */ Int16 diffusion)
{
    mDiffusion = diffusion;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::GetDensity(
    /* [out] */ Int16* density)
{
    VALIDATE_NOT_NULL(density);
    *density = mDensity;
    return NOERROR;
}

ECode CEnvironmentalReverbSettings::SetDensity(
    /* [in] */ Int16 density)
{
    mDensity = density;
    return NOERROR;
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
