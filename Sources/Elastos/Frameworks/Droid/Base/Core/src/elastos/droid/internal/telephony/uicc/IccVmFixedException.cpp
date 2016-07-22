
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccVmFixedException.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                         IccVmFixedException
//=====================================================================
CAR_INTERFACE_IMPL(IccVmFixedException, IccException, IIccVmFixedException);

IccVmFixedException::IccVmFixedException()
{
}

IccVmFixedException::IccVmFixedException(
    /* [in] */ const String& s)
    : IccException(s)
{
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
