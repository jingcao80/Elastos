#ifndef __ELASTOS_DROID_LAUNCHER2_CFOLDERHELPER_H__
#define __ELASTOS_DROID_LAUNCHER2_CFOLDERHELPER_H__

#include "_Launcher2.h"
#include "_Elastos_Droid_Launcher2_CFolderHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CFolderHelper)
    , public Singleton
    , public IFolderHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Creates a new UserFolder, inflated from R.layout.user_folder.
     *
     * @param context The application's context.
     *
     * @return A new UserFolder.
     */
    CARAPI FromXml(
        /* [in] */ IContext* context,
        /* [out] */ IFolder** folder);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CFOLDERHELPER_H__