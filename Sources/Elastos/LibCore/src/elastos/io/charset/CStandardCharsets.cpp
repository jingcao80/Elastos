
#include "CStandardCharsets.h"
#include "StandardCharsets.h"

namespace Elastos {
namespace IO {
namespace Charset {

CAR_INTERFACE_IMPL(CStandardCharsets, Singleton, IStandardCharsets)

CAR_SINGLETON_IMPL(CStandardCharsets)

ECode CStandardCharsets::GetISO_8859_1(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::ISO_8859_1;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUS_ASCII(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::US_ASCII;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUTF_8(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::UTF_8;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUTF_16(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::UTF_16;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUTF_16BE(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::UTF_16BE;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

ECode CStandardCharsets::GetUTF_16LE(
    /* [out] */ ICharset** outcharset)
{
    VALIDATE_NOT_NULL(outcharset)

    *outcharset = StandardCharsets::UTF_16LE;
    REFCOUNT_ADD(*outcharset)
    return NOERROR;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
