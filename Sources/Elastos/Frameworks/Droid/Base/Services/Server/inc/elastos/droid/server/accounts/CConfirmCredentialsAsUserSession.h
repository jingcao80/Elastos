
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CCONFIRMCREDENTIALSASUSERSESSION_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_CCONFIRMCREDENTIALSASUSERSESSION_H__

#include "_Elastos_Droid_Server_Accounts_CConfirmCredentialsAsUserSession.h"
#include "accounts/AccountManagerServiceSession.h"

using Elastos::Droid::Accounts::IAccountManagerResponse;
using Elastos::Droid::Accounts::IIAccountManager;
using Elastos::Droid::Accounts::IAccount;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CarClass(CConfirmCredentialsAsUserSession), public AccountManagerServiceSession
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
        /* [in] */ Boolean expectActivityLaunch,
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* options,
        /* [in] */ IIAccountManager* host);

protected:
    CARAPI_(String) ToDebugString(
        /* [in]  */ Int64 now);

private:
    AutoPtr<IAccount> mAccount;
    AutoPtr<IBundle> mOptions;
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_CCONFIRMCREDENTIALSASUSERSESSION_H__
