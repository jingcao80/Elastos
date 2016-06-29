
#include "elastos/apps/dialer/calllog/CPhoneAccountUtils.h"
#include "elastos/apps/dialer/calllog/PhoneAccountUtils.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CPhoneAccountUtils, Singleton, IPhoneAccountUtils);

CAR_SINGLETON_IMPL(CPhoneAccountUtils);

ECode CPhoneAccountUtils::GetAccount(
    /* [in] */ String componentString,
    /* [in] */ String accountId,
    /* [out] */ IPhoneAccountHandle** handle)
{
    VALIDATE_NOT_NULL(handle);
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
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IDrawable> result = PhoneAccountUtils::GetAccountIcon(
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
    VALIDATE_NOT_NULL(lable);
    *lable = PhoneAccountUtils::GetAccountLabel(
            context, phoneAccount);

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
