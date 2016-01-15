#ifndef __ELASTOS_DROID_SERVICE_WALLPAPER_WALLPAPERSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SERVICE_WALLPAPER_WALLPAPERSETTINGSACTIVITY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/preference/PreferenceActivity.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::PreferenceActivity;


namespace Elastos {
namespace Droid {
namespace Service {
namespace Wallpaper {

/**
 * Base class for activities that will be used to configure the settings of
 * a wallpaper.  You should derive from this class to allow it to select the
 * proper theme of the activity depending on how it is being used.
 * @hide
 */
class WallpaperSettingsActivity
    : public PreferenceActivity
    , public IWallpaperSettingsActivity
{
public:
    CAR_INTERFACE_DECL()

    WallpaperSettingsActivity();

    CARAPI constructor();

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);
};

} // namespace Wallpaper
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_WALLPAPER_WALLPAPERSETTINGSACTIVITY_H__
