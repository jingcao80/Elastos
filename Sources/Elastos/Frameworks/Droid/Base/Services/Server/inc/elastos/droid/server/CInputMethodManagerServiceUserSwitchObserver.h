#ifndef __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEUSERSWITCHOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEUSERSWITCHOBSERVER_H__

#include "_Elastos_Droid_Server_CInputMethodManagerServiceUserSwitchObserver.h"
#include "CInputMethodManagerService.h"

using Elastos::Droid::Os::IRemoteCallback;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CInputMethodManagerServiceUserSwitchObserver)
{
public:
    CARAPI constructor(
        /* [in] */ Handle32 inputMethodManagerService);

    CARAPI OnUserSwitching(
        /* [in] */ Int32 newUserId,
        /* [in] */ IRemoteCallback* reply);

    CARAPI OnUserSwitchComplete(
        /* [in] */ Int32 newUserId);

private:
    CInputMethodManagerService* mHost;
};

} // namespace Elastos;
} // namespace Droid;
} // namespace Server;
#endif
