
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccFileNotFound.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                           IccFileNotFound
//=====================================================================
CAR_INTERFACE_IMPL(IccFileNotFound, IccException, IIccFileNotFound);

IccFileNotFound::IccFileNotFound()
{
}

IccFileNotFound::IccFileNotFound(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // super(s);
}

IccFileNotFound::IccFileNotFound(
    /* [in] */ Int32 ef)
{
    // ==================before translated======================
    // super("ICC EF Not Found 0x" + Integer.toHexString(ef));
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


