
#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGERAMSRESPONSE_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGERAMSRESPONSE_H__

#include "_Elastos_Droid_Accounts_CAccountManagerAmsResponse.h"
#include "elastos/droid/accounts/CAccountManager.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAccountManagerAmsResponse)
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
    AutoPtr<CAccountManager::AmsTask> mHost;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNTMANAGERAMSRESPONSE_H__
