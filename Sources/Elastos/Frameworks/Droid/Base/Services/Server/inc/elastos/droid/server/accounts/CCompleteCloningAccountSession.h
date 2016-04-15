
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CCOMPLETECLONINGACCOUNTSESSION_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_CCOMPLETECLONINGACCOUNTSESSION_H__

#include "_Elastos_Droid_Server_Accounts_CCompleteCloningAccountSession.h"
#include "elastos/droid/server/accounts/CAccountManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CarClass(CCompleteCloningAccountSession)
    , public CAccountManagerService::CompleteCloningAccountSession
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_ACCOUNTS_CCOMPLETECLONINGACCOUNTSESSION_H__
