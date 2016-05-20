
#include "CStringEncoderHelper.h"
#include "StringEncoder.h"

namespace Elastos {
namespace Sql {
namespace SQLite {
CAR_INTERFACE_IMPL(CStringEncoderHelper, Object, IStringEncoderHelper)

CAR_SINGLETON_IMPL(CStringEncoderHelper)

ECode CStringEncoderHelper::Encode(
    /* [in] */ ArrayOf<Byte> * a,
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str);
    *str = StringEncoder::Encode(a);
    return NOERROR;
}

ECode CStringEncoderHelper::Decode(
    /* [in] */ const String& s,
    /* [out, callee] */ ArrayOf<Byte> ** outarray)
{
    VALIDATE_NOT_NULL(outarray);
    AutoPtr<ArrayOf<Byte> > result = StringEncoder::Decode(s);
    *outarray = result;
    REFCOUNT_ADD(*outarray);
    return NOERROR;
}

ECode CStringEncoderHelper::EncodeX(
    /* [in] */ ArrayOf<Byte> * a,
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str);
    *str = StringEncoder::EncodeX(a);
    return NOERROR;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
