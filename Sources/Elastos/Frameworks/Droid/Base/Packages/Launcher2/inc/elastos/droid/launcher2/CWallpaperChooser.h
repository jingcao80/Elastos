
#ifndef  __ELASTOS_DROID_LAUNCHER2_CWALLPAPERCHOOSER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CWALLPAPERCHOOSER_H__

#include "_Elastos_Droid_Packages_Launcher2_CWallpaperChooser.h"
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CWallpaperChooser)
    , public Activity
{
public:
    CAR_OBJECT_DECL();

    // @Override
    CARAPI OnCreate();

private:
    // @SuppressWarnings("unused")
    static const String TAG;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CWALLPAPERCHOOSER_H__
