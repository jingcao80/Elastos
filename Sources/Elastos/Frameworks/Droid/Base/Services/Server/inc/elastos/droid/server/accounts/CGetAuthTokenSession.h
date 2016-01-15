
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CGETAUTHTOKENSESSION_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_CGETAUTHTOKENSESSION_H__

#include "_Elastos_Droid_Server_Accounts_CGetAuthTokenSession.h"
#include "accounts/AccountManagerServiceSession.h"

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Accounts::IAccountManagerResponse;
using Elastos::Droid::Accounts::IIAccountManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CarClass(CGetAuthTokenSession), public AccountManagerServiceSession
{
public:
    CGetAuthTokenSession();

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
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ IBundle* loginOptions,
        /* [in] */ Boolean notifyOnAuthFailure,
        /* [in] */ Boolean permissionGranted,
        /* [in] */ Boolean customTokens,
        /* [in] */ IIAccountManager* host);

protected:
    CARAPI_(String) ToDebugString(
        /* [in]  */ Int64 now);

private:
    AutoPtr<IAccount> mAccount;
    String mAuthTokenType;
    AutoPtr<IBundle> mLoginOptions;
    Boolean mNotifyOnAuthFailure;
    Boolean mPermissionGranted;
    Boolean mCustomTokens;
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_CGETAUTHTOKENSESSION_H__
