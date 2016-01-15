
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CADDACOUNTSESSION_H__
#define __CAddAcountSession_H__

#include "_Elastos_Droid_Server_Accounts_CAddAcountSession.h"
#include "accounts/AccountManagerServiceSession.h"

using Elastos::Droid::Accounts::IAccountManagerResponse;
using Elastos::Droid::Accounts::IIAccountManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CarClass(CAddAcountSession), public AccountManagerServiceSession
{
public:
    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();
    
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI Run();

    CARAPI OnResult(
        /* [in] */ IBundle* result);

    CARAPI OnRequestContinued();

    CARAPI OnError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& errorMessage);

    CARAPI ProxyDied();

    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

    CARAPI constructor(
        /* [in] */ Handle32 accounts,
        /* [in] */ IAccountManagerResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ const String& authTokenType,
        /* [in] */ const ArrayOf<String>& requiredFeatures,
        /* [in] */ IBundle* options,
        /* [in] */ IIAccountManager* host);

protected:
    CARAPI_(String) ToDebugString(
        /* [in]  */ Int64 now);

private:
    String mAuthTokenType;
    AutoPtr< ArrayOf<String> > mRequiredFeatures;
    AutoPtr<IBundle> mOptions;
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__CAddAcountSession_H__
