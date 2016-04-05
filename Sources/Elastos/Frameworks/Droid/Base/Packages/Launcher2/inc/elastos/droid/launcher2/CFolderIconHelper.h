#ifndef __ELASTOS_DROID_LAUNCHER2_CFOLDERICONHELPER_H__
#define __ELASTOS_DROID_LAUNCHER2_CFOLDERICONHELPER_H__

#include "_Launcher2.h"
#include "_Elastos_Droid_Launcher2_CFolderIconHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CFolderIconHelper)
    , public Singleton
    , public IFolderIconHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI FromXml(
        /* [in] */ Int32 resId,
        /* [in] */ ILauncher* launcher,
        /* [in] */ IViewGroup* group,
        /* [in] */ IFolderInfo* folderInfo,
        /* [in] */ IIconCache* iconCache,
        /* [out] */ IFolderIcon** icon);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CFOLDERICONHELPER_H__