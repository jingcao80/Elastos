

#include "CPhoneAccountUtils.h"
#include "PhoneAccountUtils.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CPhoneAccountUtils, Singleton, IPhoneAccountUtils)

CAR_SINGLETON_IMPL(CPhoneAccountUtils)

ECode CPhoneAccountUtils::GetAccount(
    /* [in] */ String componentString,
    /* [in] */ String accountId,
    /* [out] */ IPhoneAccountHandle* handle)
{
    VALUE_NOT_NULL(handle);
    AutoPtr<IPhoneAccountHandle> result = PhoneAccountUtils::GetAccount(
            componentString, accountId);
    *handle = result;
    REFCOUNT_ADD(*handle);

    return NOERROR;
}

ECode CPhoneAccountUtils::GetAccountIcon(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneAccountHandle* phoneAccount,
    /* [out] */ IDrawable** drawable)
{
    VALUE_NOT_NULL(drawable);
    AutoPtr<IPhoneAccountHandle> result = PhoneAccountUtils::GetAccountIcon(
            context, phoneAccount);
    *drawable = result;
    REFCOUNT_ADD(*drawable);

    return NOERROR;
}

ECode CPhoneAccountUtils::GetAccountLabel(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneAccountHandle* phoneAccount,
    /* [out] */ String* lable)
{
    VALUE_NOT_NULL(lable);
    AutoPtr<IPhoneAccountHandle> result = PhoneAccountUtils::GetAccountLabel(
            context, phoneAccount);
    *lable = result;

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
