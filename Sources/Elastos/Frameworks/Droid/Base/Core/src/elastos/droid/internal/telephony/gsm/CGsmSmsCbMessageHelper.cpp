#include "elastos/droid/internal/telephony/gsm/CGsmSmsCbMessageHelper.h"
#include "elastos/droid/internal/telephony/gsm/CGsmSmsCbMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGsmSmsCbMessageHelper, Singleton, IGsmSmsCbMessageHelper)

CAR_SINGLETON_IMPL(CGsmSmsCbMessageHelper)

ECode CGsmSmsCbMessageHelper::CreateSmsCbMessage(
    /* [in] */ ISmsCbHeader* header,
    /* [in] */ ISmsCbLocation* location,
    /* [in] */ IArrayOf* pdus,
    /* [out] */ ISmsCbMessage** result)
{
    return CGsmSmsCbMessage::CreateSmsCbMessage(header, location, pdus, result);
}

ECode CGsmSmsCbMessageHelper::CreateSmsCbMessage(
    /* [in] */ ISmsCbLocation* location,
    /* [in] */ IArrayOf* pdus,
    /* [out] */ ISmsCbMessage** result)
{
    return CGsmSmsCbMessage::CreateSmsCbMessage(location, pdus, result);
}


} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos