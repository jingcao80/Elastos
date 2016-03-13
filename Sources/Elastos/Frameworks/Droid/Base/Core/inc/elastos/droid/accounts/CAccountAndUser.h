
#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTANDUSER_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTANDUSER_H__

#include "_Elastos_Droid_Accounts_CAccountAndUser.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAccountAndUser)
    , public Object
    , public IAccountAndUser
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IAccount *account,
        /* [in] */ Int32 userId);

    CARAPI Equals(
        /* [in] */ IInterface *o,
        /* [out] */ Boolean *isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hash);

    CARAPI ToString(
        /* [out] */ String *info);

    CARAPI GetAccount(
        /* [out] */ IAccount **account);

    CARAPI GetUserId(
        /* [out] */ Int32 *id);

public:
    AutoPtr<IAccount> mAccount;
    Int32 mUserId;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNTANDUSER_H__
