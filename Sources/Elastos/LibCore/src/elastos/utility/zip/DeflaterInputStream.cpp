
#include "DeflaterInputStream.h"
#include "Math.h"
#include "CStreams.h"
#include "CDeflater.h"
#include "Arrays.h"

using Elastos::Core::Math;
using Elastos::IO::EIID_IInputStream;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace Utility {
namespace Zip {

const Int32 DeflaterInputStream::DEFAULT_BUFFER_SIZE;

CAR_INTERFACE_IMPL(DeflaterInputStream, FilterInputStream, IDeflaterInputStream)

DeflaterInputStream::DeflaterInputStream()
    : mClosed(FALSE)
    , mAvailable(TRUE)
{}

DeflaterInputStream::~DeflaterInputStream()
{
}

ECode DeflaterInputStream::constructor(
    /* [in] */ IInputStream* in)
{
    AutoPtr<IDeflater> deflater;
    CDeflater::New((IDeflater**)&deflater);
    return constructor(in, deflater, DEFAULT_BUFFER_SIZE);
}

ECode DeflaterInputStream::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ IDeflater* deflater)
{
    return constructor(in, deflater, DEFAULT_BUFFER_SIZE);
}

ECode DeflaterInputStream::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ IDeflater* deflater,
    /* [in] */ Int32 bufferSize)
{
    FAIL_RETURN(FilterInputStream::constructor(in))

    if (in == NULL || deflater == NULL) {
//        throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    if (bufferSize <= 0) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mDef = deflater;
    mBuf = ArrayOf<Byte>::Alloc(bufferSize);
    return NOERROR;
}

ECode DeflaterInputStream::Close()
{
    mClosed = TRUE;
    FAIL_RETURN(mDef->End());
    return ICloseable::Probe(mIn)->Close();
}

ECode DeflaterInputStream::Read(
    /* [out] */ Int32* value)
{
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->ReadSingleByte(this, value);
}

ECode DeflaterInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;

    FAIL_RETURN(CheckClosed());
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), byteOffset, byteCount))

    if (byteCount == 0) {
        *number = 0;
        return NOERROR;
    }

    if (!mAvailable) {
        *number = -1;
        return NOERROR;
    }

    using Elastos::Core::Math;

    Int32 count = 0, bytesRead = 0, bytesDeflated;
    Boolean finished, needs;
    while (count < byteCount && (mDef->Finished(&finished), !finished)) {
        mDef->NeedsInput(&needs);
        if (needs) {
            // read data from input stream
            mIn->Read(mBuf, &bytesRead);
            if (bytesRead == -1) {
                mDef->Finish();
            }
            else {
                mDef->SetInput(mBuf, 0, bytesRead);
            }
        }

        mDef->Deflate(buffer, byteOffset + count, byteCount - count, &bytesDeflated);
        if (bytesDeflated == -1) {
            break;
        }
        count += bytesDeflated;
    }

    if (count == 0) {
        count = -1;
        mAvailable = FALSE;
    }

    if (mDef->Finished(&finished), finished) {
        mAvailable = FALSE;
    }

    *number = count;
    return NOERROR;
}

ECode DeflaterInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    using Elastos::Core::Math;
    byteCount = Math::Min((Int64)Math::INT32_MAX_VALUE, byteCount);
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->SkipByReading(this, byteCount, number);
}

ECode DeflaterInputStream::Available(
    /* [out] */ Int32* number)
{
    FAIL_RETURN(CheckClosed());
    *number = mAvailable ? 1 : 0;
    return NOERROR;
}

ECode DeflaterInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    *supported = FALSE;
    return NOERROR;
}

ECode DeflaterInputStream::Mark(
    /* [in] */ Int32 limit)
{
    return NOERROR;
}

ECode DeflaterInputStream::Reset()
{
    return E_IO_EXCEPTION;
}

ECode DeflaterInputStream::CheckClosed()
{
    if (mClosed) {
//        throw new IOException("Stream is closed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode DeflaterInputStream::Read(
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
