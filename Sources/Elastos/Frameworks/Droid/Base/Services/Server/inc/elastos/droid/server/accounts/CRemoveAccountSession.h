
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CREMOVEACCOUNTSESSION_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_CREMOVEACCOUNTSESSION_H__

#include "_Elastos_Droid_Server_Accounts_CRemoveAccountSession.h"
#include "elastos/droid/server/accounts/CAccountManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CarClass(CRemoveAccountSession)
    , public CAccountManagerService::RemoveAccountSession
{
public:
    CAR_OBJECT_DECL()
};


} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_CREMOVEACCOUNTSESSION_H__
