#include "elastos/droid/server/CAppWidgetBackupBridge.h"

namespace Elastos {
namespace Droid {
namespace Server {

AutoPtr<IWidgetBackupProvider> CAppWidgetBackupBridge::sAppWidgetService;

CAR_SINGLETON_IMPL(CAppWidgetBackupBridge)

CAR_INTERFACE_IMPL(CAppWidgetBackupBridge, Singleton, IAppWidgetBackupBridge)

ECode CAppWidgetBackupBridge::constructor()
{
    return NOERROR;
}

ECode CAppWidgetBackupBridge::Register(
    /* [in] */ IWidgetBackupProvider* instance)
{
    sAppWidgetService = instance;
    return NOERROR;
}

ECode CAppWidgetBackupBridge::GetWidgetParticipants(
    /* [in] */ Int32 userId,
    /* [out] */ IList** wp)
{
    VALIDATE_NOT_NULL(wp)
    if (sAppWidgetService != NULL) {
        sAppWidgetService->GetWidgetParticipants(userId, wp);
    }
    else {
        *wp = NULL;
    }
    return NOERROR;
}

ECode CAppWidgetBackupBridge::GetWidgetState(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<Byte>** ws)
{
    VALIDATE_NOT_NULL(ws)
    if (sAppWidgetService != NULL) {
        sAppWidgetService->GetWidgetState(packageName, userId, ws);
    }
    else {
        *ws = NULL;
    }
    return NOERROR;
}

ECode CAppWidgetBackupBridge::RestoreStarting(
    /* [in] */ Int32 userId)
{
    if (sAppWidgetService != NULL) {
        sAppWidgetService->RestoreStarting(userId);
    }
    return NOERROR;
}

ECode CAppWidgetBackupBridge::RestoreWidgetState(
    /* [in] */ const String& packageName,
    /* [in] */ ArrayOf<Byte>* restoredState,
    /* [in] */ Int32 userId)
{
    if (sAppWidgetService != NULL) {
        sAppWidgetService->RestoreWidgetState(packageName, restoredState, userId);
    }
    return NOERROR;
}

ECode CAppWidgetBackupBridge::RestoreFinished(
    /* [in] */ Int32 userId)
{
    if (sAppWidgetService != NULL) {
        sAppWidgetService->RestoreFinished(userId);
    }
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos