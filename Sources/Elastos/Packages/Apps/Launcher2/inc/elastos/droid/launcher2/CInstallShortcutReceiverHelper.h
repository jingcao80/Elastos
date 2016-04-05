#ifndef __ELASTOS_DROID_LAUNCHER2_CINSATLLSHORTCUTRECEIVERHELPER_H__
#define __ELASTOS_DROID_LAUNCHER2_CINSATLLSHORTCUTRECEIVERHELPER_H__

#include "_Launcher2.h"
#include "_Elastos_Droid_Launcher2_CInstallShortcutReceiverHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CInstallShortcutReceiverHelper)
    , public Singleton
    , public IInstallShortcutReceiverHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI EnableInstallQueue();

    CARAPI DisableAndFlushInstallQueue(
        /* [in] */ IContext* context);

    CARAPI FlushInstallQueue(
        /* [in] */ IContext* context);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CINSATLLSHORTCUTRECEIVERHELPER_H__