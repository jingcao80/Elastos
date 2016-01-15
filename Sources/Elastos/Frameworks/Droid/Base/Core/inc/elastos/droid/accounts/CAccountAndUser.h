
#ifndef __ELASTOS_DROID_ACCOUNTS_CACCOUNTANDUSER_H__
#define __ELASTOS_DROID_ACCOUNTS_CACCOUNTANDUSER_H__

#include "_Elastos_Droid_Accounts_CAccountAndUser.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAccountAndUser)
{
public:
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

    CARAPI constructor(
        /* [in] */ IAccount *account,
        /* [in] */ Int32 userId);

public:
    AutoPtr<IAccount> mAccount;
    Int32 mUserId;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CACCOUNTANDUSER_H__
