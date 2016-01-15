#ifndef __ELASTOS_DROID_APP_CWALLPAPERMANAGERHELPER_H__
#define __ELASTOS_DROID_APP_CWALLPAPERMANAGERHELPER_H__

#include "_Elastos_Droid_App_CWallpaperManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CWallpaperManagerHelper)
    , public Singleton
    , public IWallpaperManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Retrieve a WallpaperManager associated with the given Context.
     */
    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IWallpaperManager** manager);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CWALLPAPERMANAGERHELPER_H__
