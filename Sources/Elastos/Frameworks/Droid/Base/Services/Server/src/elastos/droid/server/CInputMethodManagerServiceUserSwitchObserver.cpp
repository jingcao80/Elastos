#include "elastos/droid/server/CInputMethodManagerServiceUserSwitchObserver.h"
#include "elastos/droid/server/CInputMethodManagerService.h"
#include <elastos/core/AutoLock.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::App::EIID_IIUserSwitchObserver;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_INTERFACE_IMPL_2(CInputMethodManagerServiceUserSwitchObserver, Object, IIUserSwitchObserver, IBinder)

CAR_OBJECT_IMPL(CInputMethodManagerServiceUserSwitchObserver)

ECode CInputMethodManagerServiceUserSwitchObserver::constructor(
    /* [in] */ IIInputMethodManager* inputMethodManagerService)
{
    mHost = (CInputMethodManagerService*)inputMethodManagerService;
    return NOERROR;
}

ECode CInputMethodManagerServiceUserSwitchObserver::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IIRemoteCallback* reply)
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

ECode CInputMethodManagerServiceUserSwitchObserver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace Elastos;
} // namespace Droid;
} // namespace Server;