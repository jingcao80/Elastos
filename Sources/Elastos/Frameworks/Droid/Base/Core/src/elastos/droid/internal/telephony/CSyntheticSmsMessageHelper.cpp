
#include "elastos/droid/internal/telephony/CSyntheticSmsMessageHelper.h"
#include "elastos/droid/internal/telephony/SyntheticSmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSyntheticSmsMessageHelper, Singleton, ISyntheticSmsMessageHelper)

CAR_SINGLETON_IMPL(CSyntheticSmsMessageHelper)

ECode CSyntheticSmsMessageHelper::IsSyntheticPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean res = SyntheticSmsMessage::IsSyntheticPdu(pdu);
    *result = res;
    return NOERROR;
}

ECode CSyntheticSmsMessageHelper::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ ISyntheticSmsMessage** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISyntheticSmsMessage> res = SyntheticSmsMessage::CreateFromPdu(pdu);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony