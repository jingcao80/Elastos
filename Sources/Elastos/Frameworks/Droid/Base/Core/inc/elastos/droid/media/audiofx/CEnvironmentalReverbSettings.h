
#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CENVIRONMENTALREVERBSETTINGS_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CENVIRONMENTALREVERBSETTINGS_H__

#include "_Elastos_Droid_Media_Audiofx_CEnvironmentalReverbSettings.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * The Settings class regroups all bass boost parameters. It is used in
 * conjuntion with getProperties() and setProperties() methods to backup and restore
 * all parameters in a single call.
 */
CarClass(CEnvironmentalReverbSettings)
    , public Object
    , public IEnvironmentalReverbSettings
{
public:
    CEnvironmentalReverbSettings();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Settings class constructor from a key=value; pairs formatted string. The string is
     * typically returned by Settings.toString() method.
     * @throws IllegalArgumentException if the string is not correctly formatted.
     */
    CARAPI constructor(
        /* [in] */ const String& settings);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetRoomLevel(
        /* [out] */ Int16* roomLevel);

    CARAPI SetRoomLevel(
        /* [in] */ Int16 roomLevel);

    CARAPI GetRoomHFLevel(
        /* [out] */ Int16* roomHFLevel);

    CARAPI SetRoomHFLevel(
        /* [in] */ Int16 roomHFLevel);

    CARAPI GetDecayTime(
        /* [out] */ Int32* decayTime);

    CARAPI SetDecayTime(
        /* [in] */ Int32 decayTime);

    CARAPI GetDecayHFRatio(
        /* [out] */ Int16* decayHFRatio);

    CARAPI SetDecayHFRatio(
        /* [in] */ Int16 decayHFRatio);

    CARAPI GetReflectionsLevel(
        /* [out] */ Int16* reflectionsLevel);

    CARAPI SetReflectionsLevel(
        /* [in] */ Int16 reflectionsLevel);

    CARAPI GetReflectionsDelay(
        /* [out] */ Int32* reflectionsDelay);

    CARAPI SetReflectionsDelay(
        /* [in] */ Int32 reflectionsDelay);

    CARAPI GetReverbLevel(
        /* [out] */ Int16* reverbLevel);

    CARAPI SetReverbLevel(
        /* [in] */ Int16 reverbLevel);

    CARAPI GetReverbDelay(
        /* [out] */ Int32* reverbDelay);

    CARAPI SetReverbDelay(
        /* [in] */ Int32 reverbDelay);

    CARAPI GetDiffusion(
        /* [out] */ Int16* diffusion);

    CARAPI SetDiffusion(
        /* [in] */ Int16 diffusion);

    CARAPI GetDensity(
        /* [out] */ Int16* density);

    CARAPI SetDensity(
        /* [in] */ Int16 density);

public:
    Int16 mRoomLevel;
    Int16 mRoomHFLevel;
    Int32 mDecayTime;
    Int16 mDecayHFRatio;
    Int16 mReflectionsLevel;
    Int32 mReflectionsDelay;
    Int16 mReverbLevel;
    Int32 mReverbDelay;
    Int16 mDiffusion;
    Int16 mDensity;
};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CENVIRONMENTALREVERBSETTINGS_H__
