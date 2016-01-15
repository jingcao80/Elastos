
#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGERFUTURERESPONSE_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGERFUTURERESPONSE_H__

#include "_Elastos_Droid_Accounts_CAccountManagerFutureResponse.h"
#include "elastos/droid/accounts/CAccountManager.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAccountManagerFutureResponse)
{
public:
    CARAPI OnResult(
        /* [in] */ IBundle* value);

    CARAPI OnError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& errorMessage);

    CARAPI constructor (
        /* [in] */ Handle32 host);

private:
    AutoPtr<CAccountManager::BaseFutureTask> mHost;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGERFUTURERESPONSE_H__
