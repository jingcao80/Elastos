
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccException.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                             IccException
//=====================================================================
CAR_INTERFACE_IMPL(IccException, Object, IIccException);

IccException::IccException()
{
}

IccException::IccException(
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
