
#include "CPhoneNumberUtilsWrapperHelper.h"
#include "PhoneNumberUtilsWrapper.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CPhoneNumberUtilsWrapperHelper, Singleton, IPhoneNumberUtilsWrapperHelper)

CAR_SINGLETON_IMPL(CPhoneNumberUtilsWrapperHelper)

ECode CPhoneNumberUtilsWrapperHelper::GetINSTANCE(
    /* [out] */ IPhoneNumberUtilsWrapper** wrapper)
{
    VALUE_NOT_NULL(wrapper);
    *wrapper = PhoneNumberUtilsWrapper::INSTANCE;
    REFCOUNT_ADD(*wrapper);
    return NOERROR;
}

ECode CPhoneNumberUtilsWrapperHelper::CanPlaceCallsTo(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = PhoneNumberUtilsWrapper::CanPlaceCallsTo(number, presentation);
    return NOERROR;
}

ECode CPhoneNumberUtilsWrapperHelper::IsUnknownNumberThatCanBeLookedUp(
    /* [in] */ ICharSequence* number,
    /* [in] */ Int32 presentation,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = PhoneNumberUtilsWrapper::IsUnknownNumberThatCanBeLookedUp(number, presentation);
    return NOERROR;
}

ECode CPhoneNumberUtilsWrapperHelper::IsLegacyUnknownNumbers(
    /* [in] */ ICharSequence* number,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = PhoneNumberUtilsWrapper::IsLegacyUnknownNumbers(number);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
