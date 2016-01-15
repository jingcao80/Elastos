
#include "org/apache/http/impl/io/ContentLengthOutputStream.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

ContentLengthOutputStream::ContentLengthOutputStream(
    /* [in] */ ISessionOutputBuffer* out,
    /* [in] */ Int64 contentLength)
    : OutputStream()
    , mContentLength(0)
    , mTotal(0)
    , mClosed(FALSE)
{
    if (out == NULL) {
        Logger::E("ContentLengthOutputStream", "Session output buffer may not be null");
        assert(0);
        // throw new IllegalArgumentException("Session output buffer may not be null");
    }
    if (contentLength < 0) {
        Logger::E("ContentLengthOutputStream", "Content length may not be negative");
        assert(0);
        // throw new IllegalArgumentException("Content length may not be negative");
    }
    mOut = out;
    mContentLength = contentLength;
}

ECode ContentLengthOutputStream::Close()
{
    if (!mClosed) {
        mClosed = TRUE;
        mOut->Flush();
    }
    return NOERROR;
}

ECode ContentLengthOutputStream::Flush()
{
    return mOut->Flush();
}

ECode ContentLengthOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (mClosed) {
        Logger::E("ContentLengthOutputStream", "Attempted write to closed stream.");
        return E_IO_EXCEPTION;
    }
    if (mTotal < mContentLength) {
        Int64 max = mContentLength - mTotal;
        if (len > max) {
            len = (Int32)max;
        }
        FAIL_RETURN(mOut->Write(b, off, len))
        mTotal += len;
    }
    return NOERROR;
}

ECode ContentLengthOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    return Write(buffer, 0, buffer->GetLength());
}

ECode ContentLengthOutputStream::Write(
    /* [in] */ Int32 b)
{
    if (mClosed) {
        Logger::E("ContentLengthOutputStream", "Attempted write to closed stream.");
        return E_IO_EXCEPTION;
    }
    if (mTotal < mContentLength) {
        FAIL_RETURN(mOut->Write(b))
        mTotal++;
    }
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org