
#ifndef __ELASTOS_DROID_SERVER_ACCOUNTS_CCONFIRMCREDENTIALSASUSERSESSION_H__
#define __ELASTOS_DROID_SERVER_ACCOUNTS_CCONFIRMCREDENTIALSASUSERSESSION_H__

#include "_Elastos_Droid_Server_Accounts_CConfirmCredentialsAsUserSession.h"
#include "elastos/droid/server/accounts/CAccountManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accounts {

CarClass(CConfirmCredentialsAsUserSession)
    , public CAccountManagerService::ConfirmCredentialsAsUserSession
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Accounts
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_ACCOUNTS_CCONFIRMCREDENTIALSASUSERSESSION_H__
