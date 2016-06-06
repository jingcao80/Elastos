
#include "elastos/droid/internal/telephony/CSmsHeaderHelper.h"
#include "elastos/droid/internal/telephony/SmsHeader.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CSmsHeaderHelper, Object, ISmsHeaderHelper)

CAR_SINGLETON_IMPL(CSmsHeaderHelper)

ECode CSmsHeaderHelper::FromByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ISmsHeader** result)
{
    return SmsHeader::FromByteArray(data, result);
}

ECode CSmsHeaderHelper::ToByteArray(
    /* [in] */ ISmsHeader* smsHeader,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    return SmsHeader::ToByteArray(smsHeader, result);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
