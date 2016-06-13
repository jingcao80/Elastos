
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccVmNotSupportedException.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                      IccVmNotSupportedException
//=====================================================================
CAR_INTERFACE_IMPL(IccVmNotSupportedException, IccException, IIccVmNotSupportedException);

IccVmNotSupportedException::IccVmNotSupportedException()
{
}

IccVmNotSupportedException::IccVmNotSupportedException(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // super(s);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
