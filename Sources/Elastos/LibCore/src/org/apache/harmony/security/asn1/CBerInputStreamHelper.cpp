
#include "CBerInputStreamHelper.h"
#include "CBerInputStream.cpp"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_SINGLETON_IMPL(CBerInputStreamHelper)

CAR_INTERFACE_IMPL(CBerInputStreamHelper, Singleton, IBerInputStreamHelper)

ECode CBerInputStreamHelper::GetLength(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ Int32* length)
{
    return CBerInputStream::GetLength(encoding, length);
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

