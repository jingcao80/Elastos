
#include "elastos/droid/launcher2/CUninstallShortcutReceiverHelper.h"
#include "elastos/droid/launcher2/UninstallShortcutReceiver.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CUninstallShortcutReceiverHelper)

CAR_INTERFACE_IMPL(CUninstallShortcutReceiverHelper, Singleton, IUninstallShortcutReceiverHelper)

ECode CUninstallShortcutReceiverHelper::EnableUninstallQueue()
{
    return UninstallShortcutReceiver::EnableUninstallQueue();
}

ECode CUninstallShortcutReceiverHelper::DisableAndFlushUninstallQueue(
    /* [in] */ IContext* context)
{
    return UninstallShortcutReceiver::DisableAndFlushUninstallQueue(context);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos