
#include "elastos/droid/launcher2/CInstallShortcutReceiverHelper.h"
#include "elastos/droid/launcher2/InstallShortcutReceiver.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CInstallShortcutReceiverHelper)

CAR_INTERFACE_IMPL(CInstallShortcutReceiverHelper, Singleton, IInstallShortcutReceiverHelper)

ECode CInstallShortcutReceiverHelper::EnableInstallQueue()
{
    return InstallShortcutReceiver::EnableInstallQueue();
}

ECode CInstallShortcutReceiverHelper::DisableAndFlushInstallQueue(
    /* [in] */ IContext* context)
{
    return InstallShortcutReceiver::DisableAndFlushInstallQueue(context);
}

ECode CInstallShortcutReceiverHelper::FlushInstallQueue(
    /* [in] */ IContext* context)
{
    return InstallShortcutReceiver::FlushInstallQueue(context);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos