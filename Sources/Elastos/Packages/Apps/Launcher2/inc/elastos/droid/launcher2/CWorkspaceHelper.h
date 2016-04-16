#ifndef __ELASTOS_DROID_LAUNCHER2_CWORKSPACEHELPER_H__
#define __ELASTOS_DROID_LAUNCHER2_CWORKSPACEHELPER_H__

#include "_Elastos_Droid_Launcher2_CWorkspaceHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Graphics.h"

using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CWorkspaceHelper)
    , public Singleton
    , public IWorkspaceHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCellLayoutMetrics(
        /* [in] */ ILauncher* launcher,
        /* [in] */ Int32 orientation,
        /* [out] */ IRect** rect);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CWORKSPACEHELPER_H__