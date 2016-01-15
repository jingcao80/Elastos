#include "CInputMethodManagerServiceUserSwitchObserver.h"
#include "elastos/Mutex.h"

namespace Elastos {
namespace Droid {
namespace Server {

ECode CInputMethodManagerServiceUserSwitchObserver::constructor(
    /* [in] */ Handle32 inputMethodManagerService)
{
    mHost = (CInputMethodManagerService*)inputMethodManagerService;
    return NOERROR;
}

ECode CInputMethodManagerServiceUserSwitchObserver::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IRemoteCallback* reply)
{
    {
        AutoLock lock(mHost->mMethodMapLock);
        mHost->SwitchUserLocked(newUserId);
    }

    if (reply != NULL) {
        // try {
            reply->SendResult(NULL);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CInputMethodManagerServiceUserSwitchObserver::OnUserSwitchComplete(
    /* [in] */ Int32 newUserId)
{
    return NOERROR;
}

} // namespace Elastos;
} // namespace Droid;
} // namespace Server;