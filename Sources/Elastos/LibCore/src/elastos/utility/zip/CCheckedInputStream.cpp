
#include "CCheckedInputStream.h"
#include "Math.h"
#include "CStreams.h"

using Elastos::Core::Math;
using Elastos::IO::EIID_IInputStream;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace Utility {
namespace Zip {

CAR_INTERFACE_IMPL(CCheckedInputStream, FilterInputStream, ICheckedInputStream)

CAR_OBJECT_IMPL(CCheckedInputStream)

ECode CCheckedInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ IChecksum* csum)
{
    FAIL_RETURN(FilterInputStream::constructor(is))
    mCheck = csum;
    return NOERROR;
}

ECode CCheckedInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    FAIL_RETURN(mIn->Read(value));
    if (*value != -1) {
        mCheck->Update(*value);
    }
    return NOERROR;
}

ECode CCheckedInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 off,
    /* [in] */ Int32 nbytes,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(buffer);

    FAIL_RETURN(mIn->Read(buffer, off, nbytes, number));
    if (*number != -1) {
        mCheck->Update(buffer, off, *number);
    }
    return NOERROR;
}

ECode CCheckedInputStream::GetChecksum(
    /* [out] */ IChecksum** checksum)
{
    VALIDATE_NOT_NULL(checksum);

    *checksum = mCheck;
    REFCOUNT_ADD(*checksum);
    return NOERROR;
}

ECode CCheckedInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->SkipByReading(THIS_PROBE(IInputStream), byteCount, number);
}

ECode CCheckedInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(buffer);
    return Read(buffer, 0, buffer->GetLength(), number);
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
