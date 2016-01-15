
#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CPRESETREVERBSETTINGS_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CPRESETREVERBSETTINGS_H__

#include "_Elastos_Droid_Media_Audiofx_CPresetReverbSettings.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * The Settings class regroups all preset reverb parameters. It is used in
 * conjuntion with getProperties() and setProperties() methods to backup and restore
 * all parameters in a single call.
 */
CarClass(CPresetReverbSettings)
    , public Object
    , public IPresetReverbSettings
{
public:
    CPresetReverbSettings();

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

    CARAPI GetPreset(
        /* [out] */ Int16* result);

    CARAPI SetPreset(
        /* [in] */ Int16 result);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    Int16 mPreset;
};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CPRESETREVERBSETTINGS_H__
