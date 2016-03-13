
#include "elastos/droid/accounts/CAccountAndUser.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Accounts {

CAR_OBJECT_IMPL(CAccountAndUser)

CAR_INTERFACE_IMPL(CAccountAndUser, Object, IAccountAndUser)

ECode CAccountAndUser::Equals(
    /* [in] */ IInterface *o,
    /* [out] */ Boolean *isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    if (THIS_PROBE(IInterface) == o) {
        *isEqual = TRUE;
        return NOERROR;
    }
    if (!IAccountAndUser::Probe(o)) {
        *isEqual = FALSE;
        return NOERROR;
    }
    const AutoPtr<CAccountAndUser> other = (CAccountAndUser*)IAccountAndUser::Probe(o);
    Boolean equal;
    mAccount->Equals(other->mAccount.Get(), &equal);
    *isEqual = equal && mUserId == other->mUserId;
    return NOERROR;
}

ECode CAccountAndUser::GetHashCode(
    /* [out] */ Int32 *hash)
{
    VALIDATE_NOT_NULL(hash)
    IObject::Probe(mAccount.Get())->GetHashCode(hash);
    *hash += mUserId;
    return NOERROR;
}

ECode CAccountAndUser::ToString(
    /* [out] */ String *info)
{
    VALIDATE_NOT_NULL(info)
    mAccount->ToString(info);
    *info += " u";
    info->AppendFormat("%d", mUserId);
    return NOERROR;
}

ECode CAccountAndUser::GetAccount(
    /* [out] */ IAccount **account)
{
    VALIDATE_NOT_NULL(account)
    *account = mAccount;
    REFCOUNT_ADD(*account)
    return NOERROR;
}

ECode CAccountAndUser::GetUserId(
    /* [out] */ Int32 *id)
{
    VALIDATE_NOT_NULL(id)
    *id = mUserId;
    return NOERROR;
}

ECode CAccountAndUser::constructor(
    /* [in] */ IAccount *account,
    /* [in] */ Int32 userId)
{
    mAccount = account;
    mUserId = userId;
    return NOERROR;
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
