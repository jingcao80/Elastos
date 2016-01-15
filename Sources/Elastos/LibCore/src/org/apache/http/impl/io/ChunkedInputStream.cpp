
#include "org/apache/http/impl/io/ChunkedInputStream.h"
#include "org/apache/http/impl/io/AbstractMessageParser.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/core/Math.h"
#include "elastos/core/StringUtils.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Protocol::IHTTP;
using Org::Apache::Http::Utility::CCharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

ChunkedInputStream::ChunkedInputStream(
    /* [in] */ ISessionInputBuffer* in)
    : InputStream()
    , mChunkSize(0)
    , mPos(0)
    , mBof(TRUE)
    , mEof(FALSE)
    , mClosed(FALSE)
{
    if (in == NULL) {
        Logger::E("ChunkedInputStream", "Session input buffer may not be null");
        assert(0);
        // throw new IllegalArgumentException("Session input buffer may not be null");
    }
    mIn = in;
    CCharArrayBuffer::New(16, (ICharArrayBuffer**)&mBuffer);
}

ECode ChunkedInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;
    if (mClosed) {
        Logger::E("ChunkedInputStream", "Attempted read from closed stream.");
        return E_IO_EXCEPTION;
    }
    if (mEof) {
        *value = -1;
        return NOERROR;
    }
    if (mPos >= mChunkSize) {
        NextChunk();
        if (mEof) {
            *value = -1;
            return NOERROR;
        }
    }
    mPos++;
    return mIn->Read(value);
}

ECode ChunkedInputStream::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    if (mClosed) {
        Logger::E("ChunkedInputStream", "Attempted read from closed stream.");
        return E_IO_EXCEPTION;
    }

    if (mEof) {
        *number = -1;
        return NOERROR;
    }
    if (mPos >= mChunkSize) {
        NextChunk();
        if (mEof) {
            *number = -1;
            return NOERROR;
        }
    }
    len = Elastos::Core::Math::Min(len, mChunkSize - mPos);
    Int32 count;
    FAIL_RETURN(mIn->Read(b, off, len, &count))
    mPos += count;
    *number = count;
    return NOERROR;
}

ECode ChunkedInputStream::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    return Read(b, 0, b->GetLength(), number);
}

ECode ChunkedInputStream::NextChunk()
{
    GetChunkSize(&mChunkSize);
    if (mChunkSize < 0) {
        Logger::E("ChunkedInputStream", "Negative chunk size");
        return E_MALFORMED_CHUNK_CODING_EXCEPTION;
    }
    mBof = FALSE;
    mPos = 0;
    if (mChunkSize == 0) {
        mEof = TRUE;
        ParseTrailerHeaders();
    }
    return NOERROR;
}

ECode ChunkedInputStream::GetChunkSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    // skip CRLF
    if (!mBof) {
        Int32 cr, lf;
        mIn->Read(&cr);
        mIn->Read(&lf);
        if ((cr != IHTTP::CR) || (lf != IHTTP::LF)) {
            Logger::E("ChunkedInputStream", "CRLF expected at end of chunk");
            return E_MALFORMED_CHUNK_CODING_EXCEPTION;
        }
    }
    //parse data
    mBuffer->Clear();
    Int32 i;
    mIn->ReadLine(mBuffer, &i);
    if (i == -1) {
        Logger::E("ChunkedInputStream", "Chunked stream ended unexpectedly");
        return E_MALFORMED_CHUNK_CODING_EXCEPTION;
    }
    Int32 separator;
    mBuffer->IndexOf(';', &separator);
    if (separator < 0) {
        mBuffer->GetLength(&separator);
    }
    // try {
    String str;
    mBuffer->SubstringTrimmed(0, separator, &str);
    *size = StringUtils::ParseInt32(str, 16);
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new MalformedChunkCodingException("Bad chunk header");
    // }
}

ECode ChunkedInputStream::ParseTrailerHeaders()
{
    // try {
    return AbstractMessageParser::ParseHeaders(mIn, -1, -1, NULL, (ArrayOf<IHeader*>**)&mFooters);
    // } catch (HttpException e) {
    //     IOException ioe = new MalformedChunkCodingException("Invalid footer: "
    //             + e.getMessage());
    //     ExceptionUtils.initCause(ioe, e);
    //     throw ioe;
    // }
}

ECode ChunkedInputStream::Close()
{
    ECode ec = NOERROR;
    if (!mClosed) {
        // try {
        if (!mEof) {
            ec = ExhaustInputStream(this);
        }
        mEof = TRUE;
        mClosed = TRUE;
        // } finally {
        //     eof = true;
        //     closed = true;
        // }
    }
    return ec;
}

AutoPtr< ArrayOf<IHeader*> > ChunkedInputStream::GetFooters()
{
    return mFooters->Clone();
}

ECode ChunkedInputStream::ExhaustInputStream(
    /* [in] */ IInputStream* inStream)
{
    // read and discard the remainder of the message
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1024);
    Int32 value;
    FAIL_RETURN(inStream->Read(buffer, &value))
    while (value >= 0) {
        FAIL_RETURN(inStream->Read(buffer, &value))
    }
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org