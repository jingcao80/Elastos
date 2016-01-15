
#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTAUTHENTICATORTRANSPORT_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTAUTHENTICATORTRANSPORT_H__

#include "_Elastos_Droid_Accounts_CAccountAuthenticatorTransport.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Accounts {

class AbstractAccountAuthenticator;

CarClass(CAccountAuthenticatorTransport)
{
public:
    CARAPI AddAccount(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ const String& accountType,
        /* [in] */ const String& authTokenType,
        /* [in] */ const ArrayOf<String>& requiredFeatures,
        /* [in] */ IBundle* options);

    CARAPI ConfirmCredentials(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* options);

    CARAPI GetAuthTokenLabel(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ const String& authTokenType);

    CARAPI GetAuthToken(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ IBundle* options);

    CARAPI UpdateCredentials(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authTokenType,
        /* [in] */ IBundle* options);

    CARAPI EditProperties(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ const String& accountType);

    CARAPI HasFeatures(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account,
        /* [in] */ const ArrayOf<String>& features);

    CARAPI GetAccountRemovalAllowed(
        /* [in] */ IIAccountAuthenticatorResponse* response,
        /* [in] */ IAccount* account);

    CARAPI constructor(
        /* [in] */ Handle32 accounts);

private:
    AbstractAccountAuthenticator* mHost;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNTAUTHENTICATORTRANSPORT_H__
