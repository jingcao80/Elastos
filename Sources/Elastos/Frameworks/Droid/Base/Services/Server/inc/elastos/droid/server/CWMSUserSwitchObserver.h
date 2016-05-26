
#ifndef __ELASTOS_DROID_SERVER_CWMSUSERSWITCHOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CWMSUSERSWITCHOBSERVER_H__

#include "_Elastos_Droid_Server_CWMSUserSwitchObserver.h"

using Elastos::Droid::Os::IIRemoteCallback;

namespace Elastos {
namespace Droid {
namespace Server {

class CWallpaperManagerService;

CarClass(CWMSUserSwitchObserver)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
            /* [in] */ Handle32 owner);

    CARAPI OnUserSwitching(
        /* [in] */ Int32 newUserId,
        /* [in] */ IIRemoteCallback* reply);

    CARAPI OnUserSwitchComplete(
        /* [in] */ Int32 newUserId);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CWallpaperManagerService* mOwner;
};

} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CWMSUSERSWITCHOBSERVER_H__
