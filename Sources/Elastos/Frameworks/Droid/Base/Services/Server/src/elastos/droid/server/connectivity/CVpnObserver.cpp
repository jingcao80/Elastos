
#include "elastos/droid/server/connectivity/CVpnObserver.h"
#include "elastos/droid/server/connectivity/Vpn.h"
#include <elastos/droid/os/Binder.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Os::Binder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

CAR_OBJECT_IMPL(CVpnObserver)

ECode CVpnObserver::constructor(
    /* [in] */ Handle32 host)
{
    mHost = (Vpn*)host;
    return NOERROR;
}

ECode CVpnObserver::InterfaceStatusChanged(
    /* [in] */ const String& interfaze,
    /* [in] */ Boolean up)
{
    Object& lockObj = mHost->mLock;
    AutoLock lock(lockObj);
    if (!up && mHost->mLegacyVpnRunner != NULL) {
        mHost->mLegacyVpnRunner->Check(interfaze);
    }
    return NOERROR;
}

ECode CVpnObserver::InterfaceRemoved(
    /* [in] */ const String& interfaze)
{
    Object& lockObj = mHost->mLock;
    AutoLock lock(lockObj);
    if (interfaze.Equals(mHost->mInterface) && mHost->NativeCheck(interfaze) == 0) {
        mHost->mStatusIntent = NULL;
        mHost->mVpnUsers = NULL;
        mHost->mInterface = NULL;
        if (mHost->mConnection != NULL) {
            mHost->mContext->UnbindService(mHost->mConnection);
            mHost->mConnection = NULL;
            mHost->AgentDisconnect();
        }
        else if (mHost->mLegacyVpnRunner != NULL) {
            mHost->mLegacyVpnRunner->Exit();
            mHost->mLegacyVpnRunner = NULL;
        }
    }
    return NOERROR;
}

ECode CVpnObserver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // Connectivity
} // Server
} // Droid
} // Elastos
