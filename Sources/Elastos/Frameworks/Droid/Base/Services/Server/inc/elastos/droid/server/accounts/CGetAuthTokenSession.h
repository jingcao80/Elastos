
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CGETAUTHTOKENSESSION_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_CGETAUTHTOKENSESSION_H__

#include "_Elastos_Droid_Server_Accounts_CGetAuthTokenSession.h"
#include "elastos/droid/server/accounts/CAccountManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CarClass(CGetAuthTokenSession)
    , public CAccountManagerService::GetAuthTokenSession
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_CGETAUTHTOKENSESSION_H__
