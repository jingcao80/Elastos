
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccFileNotFound.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

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
    : IccException(s)
{
}

IccFileNotFound::IccFileNotFound(
    /* [in] */ Int32 ef)
    : IccException(String("ICC EF Not Found 0x") + StringUtils::ToString(ef))
{
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


