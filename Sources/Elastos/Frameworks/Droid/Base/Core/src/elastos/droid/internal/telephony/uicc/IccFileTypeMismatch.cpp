
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccFileTypeMismatch.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                         IccFileTypeMismatch
//=====================================================================
CAR_INTERFACE_IMPL(IccFileTypeMismatch, IccException, IIccFileTypeMismatch);

IccFileTypeMismatch::IccFileTypeMismatch()
{
}

IccFileTypeMismatch::IccFileTypeMismatch(
    /* [in] */ const String& s)
    : IccException(s)
{
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
