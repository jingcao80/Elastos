#include "elastos/droid/internal/telephony/sip/CSipPhoneFactory.h"
#include "elastos/droid/internal/telephony/sip/SipPhoneFactory.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Sip {

CAR_INTERFACE_IMPL(CSipPhoneFactory, Singleton, ISipPhoneFactory)
CAR_SINGLETON_IMPL(CSipPhoneFactory)

ECode CSipPhoneFactory::MakePhone(
    /* [in] */ const String& sipUri,
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* phoneNotifier,
    /* [out] */ ISipPhone** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISipPhone> sipPhone = SipPhoneFactory::MakePhone(sipUri, context, phoneNotifier);
    *result = sipPhone;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Sip
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
