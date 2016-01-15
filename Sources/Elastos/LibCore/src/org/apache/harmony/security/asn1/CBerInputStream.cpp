
#include "CBerInputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ECode CBerInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    return BerInputStream::constructor(encoded);
}

ECode CBerInputStream::constructor(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 expectedLength)
{
    return BerInputStream::constructor(encoded, offset, expectedLength);
}

ECode CBerInputStream::constructor(
    /* [in] */ IInputStream* is)
{
    return BerInputStream::constructor(is);
}

ECode CBerInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ Int32 initialSize)
{
    return BerInputStream::constructor(is, initialSize);
}

CAR_OBJECT_IMPL(CBerInputStream)

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

