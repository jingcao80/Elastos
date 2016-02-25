#ifndef __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEUSERSWITCHOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CINPUTMETHODMANAGERSERVICEUSERSWITCHOBSERVER_H__

#include "_Elastos_Droid_Server_CInputMethodManagerServiceUserSwitchObserver.h"
#include <elastos/core/Object.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Internal.h>

using Elastos::Droid::Os::IIRemoteCallback;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::App::IIUserSwitchObserver;
using Elastos::Droid::Internal::View::IIInputMethodManager;

namespace Elastos {
namespace Droid {
namespace Server {

class CInputMethodManagerService;

CarClass(CInputMethodManagerServiceUserSwitchObserver)
    , public Object
    , public IIUserSwitchObserver
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIInputMethodManager* inputMethodManagerService);

    CARAPI OnUserSwitching(
        /* [in] */ Int32 newUserId,
        /* [in] */ IIRemoteCallback* reply);

    CARAPI OnUserSwitchComplete(
        /* [in] */ Int32 newUserId);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CInputMethodManagerService* mHost;
};

} // namespace Elastos;
} // namespace Droid;
} // namespace Server;
#endif
