
#include "CAdler32.h"
#include <zlib.h>
#include "Arrays.h"

namespace Elastos {
namespace Utility {
namespace Zip {

CAR_INTERFACE_IMPL_2(CAdler32, Object, IAdler32, IChecksum)

CAR_OBJECT_IMPL(CAdler32)

CAdler32::CAdler32()
    : mAdler(1ll)
{}

ECode CAdler32::GetValue(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mAdler;
    return NOERROR;
}

ECode CAdler32::Reset()
{
    mAdler = 1;
    return NOERROR;
}

ECode CAdler32::Update(
    /* [in] */ Int32 i)
{
    mAdler = UpdateByteImpl(i, mAdler);
    return NOERROR;
}

ECode CAdler32::Update(
    /* [in] */ ArrayOf<Byte>* buf)
{
    VALIDATE_NOT_NULL(buf)
    return Update(buf, 0, buf->GetLength());
}

ECode CAdler32::Update(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    VALIDATE_NOT_NULL(buf)
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buf->GetLength(), offset, byteCount))

    mAdler = UpdateImpl(buf, offset, byteCount, mAdler);
    return NOERROR;
}

Int64 CAdler32::UpdateImpl(
    /* [in] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [in] */ Int64 adler)
{
    return adler32(adler, reinterpret_cast<const Bytef*>(buf->GetPayload() + offset),
            byteCount);
}

Int64 CAdler32::UpdateByteImpl(
    /* [in] */ Int32 val,
    /* [in] */ Int64 adler)
{
    Bytef bytefVal = val;
    return adler32(adler, reinterpret_cast<const Bytef*>(&bytefVal), 1);
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
