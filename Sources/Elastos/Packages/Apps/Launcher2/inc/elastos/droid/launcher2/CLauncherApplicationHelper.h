#ifndef __ELASTOS_DROI_LAUNCHER2_CLAUNCHERAPPLICATIONHELPER_H__
#define  __ELASTOS_DROI_LAUNCHER2_CLAUNCHERAPPLICATIONHELPER_H__

#include "_Launcher2.h"
#include "_Elastos_Droid_Launcher2_CLauncherApplicationHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CLauncherApplicationHelper)
    , public Singleton
    , public ILauncherApplicationHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetSharedPreferencesKey(
        /* [out] */ String* key);

    CARAPI IsScreenLarge(
        /* [out] */ Boolean* result);

    CARAPI IsScreenLandscape(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);

    CARAPI GetScreenDensity(
        /* [out] */ Float* density);

    CARAPI GetLongPressTimeout(
        /* [out] */ Int32* timeout);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROI_LAUNCHER2_CLAUNCHERAPPLICATIONHELPER_H__