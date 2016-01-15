
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/internal/utility/SizedInputStream.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CStreams;
using Libcore::IO::IStreams;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(SizedInputStream, InputStream, ISizedInputStream)

ECode SizedInputStream::constructor(
    /* [in] */ IInputStream* wrapped,
    /* [in] */ Int64 length)
{
    mWrapped = wrapped;
    mLength = length;
    return NOERROR;
}

// @Override
ECode SizedInputStream::Close()
{
    FAIL_RETURN(InputStream::Close())
    return ICloseable::Probe(mWrapped)->Close();
}

// @Override
ECode SizedInputStream::Read(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->ReadSingleByte(this, num);
}

// @Override
ECode SizedInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num)
    if (mLength <= 0) {
        *num = -1;
        return NOERROR;
    }
    else if (byteCount > mLength) {
        byteCount = (Int32)mLength;
    }

    Int32 n;
    mWrapped->Read(buffer, byteOffset, byteCount, &n);
    if (n == -1) {
        if (mLength > 0) {
            Logger::E("SizedInputStream", "Unexpected EOF; expected %lld more bytes", mLength);
            return E_IO_EXCEPTION;
        }
    }
    else {
        mLength -= n;
    }
    *num = n;
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
