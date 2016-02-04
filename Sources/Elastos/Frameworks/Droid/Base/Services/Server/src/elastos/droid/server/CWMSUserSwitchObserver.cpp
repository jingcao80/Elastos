
#include "CWMSUserSwitchObserver.h"
#include "CWallpaperManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {

ECode CWMSUserSwitchObserver::constructor(
    /* [in] */ Handle32 owner)
{
    mOwner = (CWallpaperManagerService*)owner;
    return NOERROR;
}

ECode CWMSUserSwitchObserver::OnUserSwitching(
    /* [in] */ Int32 newUserId,
    /* [in] */ IIRemoteCallback* reply)
{
    mOwner->SwitchUser(newUserId, reply);
    return NOERROR;
}

ECode CWMSUserSwitchObserver::OnUserSwitchComplete(
    /* [in] */ Int32 newUserId)
{
    return NOERROR;
}

ECode CWMSUserSwitchObserver::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}


} // Server
} // Droid
} // Elastos
