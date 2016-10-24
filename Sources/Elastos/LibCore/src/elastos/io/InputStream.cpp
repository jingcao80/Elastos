
#include "elastos/io/InputStream.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>
#include "libcore/io/CStreams.h"

using Elastos::Utility::Logging::Logger;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_2(InputStream, Object, IInputStream, ICloseable)

InputStream::InputStream()
{
}

InputStream::~InputStream()
{
}

ECode InputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    return NOERROR;
}

ECode InputStream::Close()
{
    return NOERROR;
}

ECode InputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    return NOERROR;
}

ECode InputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);
    *supported = FALSE;
    return NOERROR;
}

ECode InputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    // BEGIN android-note
    // changed array notation to be consistent with the rest of harmony
    // END android-note
    return Read(buffer, 0, buffer->GetLength(), number);
}

ECode InputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    // BEGIN android-note
    // changed array notation to be consistent with the rest of harmony
    // END android-note
    // Force null check for b first!
    if (byteOffset > buffer->GetLength() || byteOffset < 0) {
        Logger::E("InputStream", "ArrayIndexOutOfBoundsException: Offset out of bounds: %d", byteOffset);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (byteCount < 0 || byteCount > buffer->GetLength() - byteOffset) {
        Logger::E("InputStream", "ArrayIndexOutOfBoundsException: Length out of bounds: %d", byteCount);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 c;
    for (Int32 i = 0; i < byteCount; i++) {
        ECode ec = Read(&c);
        if (ec == (ECode)E_IO_EXCEPTION) {
            if (i != 0) {
                *number = i;
                return NOERROR;
            }
            return ec;
        }
        if (c == -1) {
            *number = i == 0 ? -1 : i;
            return NOERROR;
        }
        (*buffer)[byteOffset + i] = (Byte)c;
    }
    *number = byteCount;
    return NOERROR;
}

ECode InputStream::Reset()
{
    return E_IO_EXCEPTION;
}

ECode InputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);

    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->SkipByReading(this, byteCount, number);
}

} // namespace IO
} // namespace Elastos
