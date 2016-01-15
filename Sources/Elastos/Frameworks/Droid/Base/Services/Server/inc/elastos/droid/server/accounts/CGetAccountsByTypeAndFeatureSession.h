
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CGETACCOUNTSBYTYPEANDFEATURESESSION_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_CGETACCOUNTSBYTYPEANDFEATURESESSION_H__

#include "_Elastos_Droid_Server_Accounts_CGetAccountsByTypeAndFeatureSession.h"
#include "accounts/AccountManagerServiceSession.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Accounts::IAccountManagerResponse;
using Elastos::Droid::Accounts::IIAccountManager;
using Elastos::Droid::Accounts::IAccount;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CarClass(CGetAccountsByTypeAndFeatureSession), public AccountManagerServiceSession
{
public:
    CGetAccountsByTypeAndFeatureSession();

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI Run();

    CARAPI CheckAccount();

    CARAPI OnResult(
        /* [in] */ IBundle* result);

    CARAPI SendResult();

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
        /* [in] */ const String& type,
        /* [in] */ const ArrayOf<String>& features,
        /* [in] */ IIAccountManager* host);

protected:
    CARAPI_(String) ToDebugString(
        /* [in]  */ Int64 now);

private:
    AutoPtr< ArrayOf<String> > mFeatures;
    AutoPtr< ArrayOf<IAccount*> > mAccountsOfType;
    List<AutoPtr<IAccount> > mAccountsWithFeatures;
    Int32 mCurrentAccount;
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_CGETACCOUNTSBYTYPEANDFEATURESESSION_H__
