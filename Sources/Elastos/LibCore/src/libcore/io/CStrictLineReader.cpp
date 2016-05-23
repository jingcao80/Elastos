#include "CStrictLineReader.h"
#include "charset/Charset.h"
#include "StringUtils.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::IO::Charset::Charset;
using Elastos::IO::EIID_ICloseable;

namespace Libcore {
namespace IO {

const Byte CStrictLineReader::CR = (Byte)'\r';
const Byte CStrictLineReader::LF = (Byte)'\n';

CStrictLineReader::MyOutputStream::MyOutputStream(
    /* [in] */ Int32 size,
    /* [in] */ CStrictLineReader* host)
{
    ByteArrayOutputStream::constructor(size);
    mHost = host;
}

ECode CStrictLineReader::MyOutputStream::ToString(
    /* [out] */ String* str)
{
    Int32 length = (mCount > 0 && (*mHost->mBuf)[mCount - 1] == CR) ? mCount - 1 : mCount;
    AutoPtr<ArrayOf<Char32> > strBuf = ArrayOf<Char32>::Alloc(mHost->mBuf->GetLength());
    for (Int32 i = 0; i < mHost->mBuf->GetLength(); i++) {
        (*strBuf)[i] = (*mHost->mBuf)[i];
    }
    *str = String(*strBuf, 0, length);
    return NOERROR;
}

CAR_OBJECT_IMPL(CStrictLineReader)

CAR_INTERFACE_IMPL_2(CStrictLineReader, Object, IStrictLineReader, ICloseable)

ECode CStrictLineReader::constructor(
    /* [in] */ IInputStream* in)
{
    return constructor(in, 8192);
}

ECode CStrictLineReader::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ Int32 capacity)
{
    AutoPtr<ICharset> ASCII;
    FAIL_RETURN(Charset::ForName(String("US-ASCII"), (ICharset**)&ASCII))
    return constructor(in, capacity, ASCII);
}

ECode CStrictLineReader::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ ICharset* Charset)
{
    return constructor(in, 8192, Charset);

}

ECode CStrictLineReader::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ Int32 capacity,
    /* [in] */ ICharset* Charset)
{
    if (in == NULL) {
        return E_NULL_POINTER_EXCEPTION;
        // throw new NullPointerException("in == null");
    } else if (Charset == NULL) {
        return E_NULL_POINTER_EXCEPTION;
        // throw new NullPointerException("Charset == null");
    }
    if (capacity < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("capacity <= 0");
    }
    AutoPtr<ICharset> US_ASCII, UTF_8, ISO_8859_1;
    FAIL_RETURN(Charset::ForName(String("US-ASCII"), (ICharset**)&US_ASCII))
    FAIL_RETURN(Charset::ForName(String("UTF-8"), (ICharset**)&UTF_8))
    FAIL_RETURN(Charset::ForName(String("ISO-8859-1"), (ICharset**)&ISO_8859_1))

    Boolean equalsAscii, equalsUtf8, equalsIso;
    AutoPtr<IObject> obj = IObject::Probe(Charset);
    obj->Equals(US_ASCII, &equalsAscii);
    obj->Equals(UTF_8, &equalsUtf8);
    obj->Equals(ISO_8859_1, &equalsIso);

    if (!(equalsAscii || equalsUtf8 || equalsIso)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Unsupported encoding");
    }

    mIn = in;
    mCharset = Charset;
    mBuf = ArrayOf<Byte>::Alloc(capacity);
    return NOERROR;
}

/**
 * Reads the next line. A line ends with {@code "\n"} or {@code "\r\n"},
 * this end of line marker is not included in the result.
 *
 * @return the next line from the input.
 * @throws IOException for underlying {@code InputStream} errors.
 * @throws EOFException for the end of source stream.
 */
ECode CStrictLineReader::ReadLine(
    /* [out] */ String* line)
{
    // {    AutoLock syncLock(in);
    AutoLock lock(mLockIn);
    if (mBuf == NULL) {
        return E_IO_EXCEPTION;
        // throw new IOException("LineReader is closed");
    }

    // Read more data if we are at the end of the buffered data.
    // Though it's an error to read after an exception, we will let {@code fillBuf()}
    // throw again if that happens; thus we need to handle end == -1 as well as end == pos.
    if (mPos >= mEnd) {
        FillBuf();
    }
    // Try to find LF in the buffered data and return the line if successful.
    for (Int32 i = mPos; i != mEnd; ++i) {
        if ((*mBuf)[i] == LF) {
            Int32 lineEnd = (i != mPos && (*mBuf)[i - 1] == CR) ? i - 1 : i;
            AutoPtr<ArrayOf<Char32> > strBuf = ArrayOf<Char32>::Alloc(mBuf->GetLength());
            for (Int32 j = 0; j < mBuf->GetLength(); j++) {
                (*strBuf)[j] = (*mBuf)[j];
            }
            *line = String(*strBuf, mPos, lineEnd - mPos);
            mPos = i + 1;
            return NOERROR;
        }
    }

    // Let's anticipate up to 80 characters on top of those already read.
    AutoPtr<MyOutputStream> out = new MyOutputStream(mEnd - mPos + 80, this);

    while (TRUE) {
        out->Write(mBuf, mPos, mEnd - mPos);
        // Mark unterminated line in case fillBuf throws EOFException or IOException.
        mEnd = -1;
        FillBuf();
        // Try to find LF in the buffered data and return the line if successful.
        for (Int32 i = mPos; i != mEnd; ++i) {
            if ((*mBuf)[i] == LF) {
                if (i != mPos) {
                    out->Write(mBuf, mPos, i - mPos);
                }
                mPos = i + 1;
                return out->ToString(line);
            }
        }
    }
    // }
    return NOERROR;
}

/**
 * Read an {@code int} from a line containing its decimal representation.
 *
 * @return the value of the {@code int} from the next line.
 * @throws IOException for underlying {@code InputStream} errors or conversion error.
 * @throws EOFException for the end of source stream.
 */
ECode CStrictLineReader::ReadInt32(
    /* [out] */ Int32* value)
{
    String intString;
    ReadLine(&intString);
    *value = StringUtils::ParseInt32(intString);
    return NOERROR;
}

/**
 * Check whether there was an unterminated line at end of input after the line reader reported
 * end-of-input with EOFException. The value is meaningless in any other situation.
 *
 * @return true if there was an unterminated line at end of input.
 */
ECode CStrictLineReader::HasUnterminatedLine(
    /* [out] */ Boolean* value)
{
    *value = mEnd == -1;
    return NOERROR;
}

ECode CStrictLineReader::Close()
{
    AutoLock lock(mLockIn);
    ECode ec = NOERROR;
    if (mBuf != NULL) {
        mBuf = NULL;
        ec = mIn->Close();
    }
    return ec;
}

ECode CStrictLineReader::FillBuf()
{
    Int32 result;
    mIn->Read(mBuf, 0, mBuf->GetLength(), &result);
    if (result == -1) {
        return E_EOF_EXCEPTION;
    }
    mPos = 0;
    mEnd = result;
    return NOERROR;
}

} // namespace IO
} // namespace Libcore