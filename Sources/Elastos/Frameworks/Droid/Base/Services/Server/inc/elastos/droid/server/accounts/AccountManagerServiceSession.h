
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_ACCOUNTMANAGERSERVICESESSION_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_ACCOUNTMANAGERSERVICESESSION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "accounts/CAccountManagerService.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Accounts::IAccountAuthenticator;
using Elastos::Droid::Accounts::IAccountManagerResponse;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

extern "C" const InterfaceID EIID_AccountManagerServiceSession;

class AccountManagerServiceSession
    : public ElRefBase
{
public:
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

    AccountManagerServiceSession();

    CARAPI_(AutoPtr<IAccountManagerResponse>) GetResponseAndClose();

    CARAPI ProxyDied();

    CARAPI Bind();

    CARAPI_(void) ScheduleTimeout();

    CARAPI_(void) CancelTimeout();

    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

    virtual CARAPI Run() = 0;

    CARAPI OnTimedOut();

    CARAPI OnResult(
        /* [in] */ IBundle* result);

    CARAPI OnRequestContinued();

    CARAPI OnError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& errorMessage);

    virtual CARAPI_(String) ToString();

    CARAPI Init(
        /* [in]  */ UserAccounts* accounts,
        /* [in]  */ IAccountManagerResponse* response,
        /* [in]  */ const String& accountType,
        /* [in]  */ Boolean expectActivityLaunch,
        /* [in]  */ Boolean stripAuthTokenFromResult,
        /* [in]  */ CAccountManagerService* host);

protected:
    CARAPI_(String) ToDebugString();

    CARAPI_(String) ToDebugString(
        /* [in]  */ Int64 now);

private:
    CARAPI_(void) Close();

    CARAPI_(void) Unbind();

    /**
     * find the component name for the authenticator and initiate a bind
     * if no authenticator or the bind fails then return false, otherwise return true
     */
    CARAPI_(Boolean) BindToAuthenticator(
        /* [in]  */ const String& authenticatorType);

public:
    AutoPtr<IAccountManagerResponse> mResponse;
    String mAccountType;
    Boolean mExpectActivityLaunch;
    Int64 mCreationTime;

    Int32 mNumResults;

    AutoPtr<IAccountAuthenticator> mAuthenticator;

protected:
    AutoPtr<UserAccounts> mAccounts;
    CAccountManagerService* mHost;

private:
    static const String TAG;
    Int32 mNumRequestContinued;
    Int32 mNumErrors;

    Boolean mStripAuthTokenFromResult;
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_ACCOUNTMANAGERSERVICESESSION_H__
