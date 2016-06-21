#include "elastos/droid/internal/telephony/imsphone/CImsPhoneFactory.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

CAR_INTERFACE_IMPL(CImsPhoneFactory, Singleton, IImsPhoneFactory)
CAR_SINGLETON_IMPL(CImsPhoneFactory)

ECode CImsPhoneFactory::MakePhone(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* phoneNotifier,
    /* [in] */ IPhone* defaultPhone,
    /* [out] */ IImsPhone** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IImsPhone> imsPhone = ImsPhoneFactory::MakePhone(context, phoneNotifier, defaultPhone);
    *result = imsPhone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace ImsPhone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
