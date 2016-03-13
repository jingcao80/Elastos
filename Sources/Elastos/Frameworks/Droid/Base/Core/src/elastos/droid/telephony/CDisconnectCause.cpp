
#include "elastos/droid/telephony/CDisconnectCause.h"
#include "elastos/droid/telephony/DisconnectCause.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CDisconnectCause, Singleton, IDisconnectCause)

CAR_SINGLETON_IMPL(CDisconnectCause)

ECode CDisconnectCause::ToString(
    /* [in] */ Int32 cause,
    /* [out] */ String* str)
{
    return DisconnectCause::ToString(cause, str);
}

} //namespace Elastos
} //namespace Droid
} //namespace Telephony