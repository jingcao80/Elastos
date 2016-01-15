
#include "BufferedWriter.h"
#include "Character.h"
#include "AutoLock.h"

using Elastos::Core::Character;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(BufferedWriter, Writer, IBufferedWriter)

//todo:
#ifdef _linux
const String BufferedWriter::mLineSeparator = String("\n"); // = AccessController.doPrivileged(new PriviAction<String>("line.separator"));
#else
const String BufferedWriter::mLineSeparator = String("\r\n");
#endif

BufferedWriter::BufferedWriter()
    : mPos(0)
{
}

BufferedWriter::~BufferedWriter()
{
}

ECode BufferedWriter::constructor(
    /* [in] */ IWriter* wout)
{
    return constructor(wout, 8192);
}

ECode BufferedWriter::constructor(
    /* [in] */ IWriter* wout,
    /* [in] */ Int32 size)
{
    if (size <= 0) {
//      throw new IllegalArgumentException("size <= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(Writer::constructor(ISynchronize::Probe(wout)))

    mOut = wout;
    mBuf = ArrayOf<Char32>::Alloc(size);
    if (mBuf == NULL) return E_OUT_OF_MEMORY_ERROR;
    return NOERROR;
}

ECode BufferedWriter::Close()
{
    AutoLock lock(mLock);

    if (IsClosed()) {
        return NOERROR;
    }

    ECode ec = FlushInternal();
    mBuf = NULL;
    ECode ec2 = ICloseable::Probe(mOut)->Close();
    if(ec2 != NOERROR && ec == NOERROR) ec = ec2;
    mOut = NULL;

    return ec;
}

ECode BufferedWriter::Flush()
{
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    FAIL_RETURN(FlushInternal());
    FAIL_RETURN(IFlushable::Probe(mOut)->Flush());

    return NOERROR;
}

ECode BufferedWriter::CheckNotClosed()
{
    if (IsClosed()) {
//      throw new IOException("BufferedWriter is closed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode BufferedWriter::FlushInternal()
{
    if (mPos > 0) {
        FAIL_RETURN(mOut->Write(mBuf, 0, mPos));
    }
    mPos = 0;

    return NOERROR;
}

Boolean BufferedWriter::IsClosed()
{
    return mOut == NULL;
}

ECode BufferedWriter::NewLine()
{
    return Write(mLineSeparator, 0, mLineSeparator.GetLength());
}

ECode BufferedWriter::Write(
    /* [in] */ ArrayOf<Char32>* cbuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    VALIDATE_NOT_NULL(cbuf)

    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    // BEGIN android-changed
    // Exception priorities (in case of multiple errors) differ from
    // RI, but are spec-compliant.
    // made implicit null check explicit, used (offset | count) < 0
    // instead of (offset < 0) || (count < 0) to safe one operation
    //assert(cbuf != NULL);

    if ((offset | count) < 0 || offset > cbuf->GetLength() - count) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    // END android-changed

    Int32 bufferLength = mBuf->GetLength();
    if (mPos == 0 && count >= bufferLength) {
        return mOut->Write(cbuf, offset, count);
    }

    Int32 available = bufferLength - mPos;
    if (count < available) {
        available = count;
    }

    if (available > 0) {
        mBuf->Copy(mPos, cbuf, offset, available);
        mPos += available;
    }

    if (mPos == bufferLength) {
        FAIL_RETURN(mOut->Write(mBuf, 0, bufferLength));
        mPos = 0;
        if (count > available) {
            offset += available;
            available = count - available;
            if (available >= bufferLength) {
                FAIL_RETURN(mOut->Write(cbuf, offset, available));
                return NOERROR;
            }

            mBuf->Copy(mPos, cbuf, offset, available);
            mPos += available;
        }
    }

    return NOERROR;
}

ECode BufferedWriter::Write(
    /* [in] */ Int32 oneChar)
{
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());

    Int32 bufferLength = mBuf->GetLength();
    if (mPos >= bufferLength) {
        FAIL_RETURN(mOut->Write(mBuf, 0, bufferLength));
        mPos = 0;
    }

    (*mBuf)[mPos++] = oneChar;

    return NOERROR;
}

ECode BufferedWriter::Write(
    /* [in] */ const String& str,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    AutoLock lock(mLock);

    FAIL_RETURN(CheckNotClosed());
    if (count <= 0) {
        return NOERROR;
    }
    if (offset < 0 || offset > str.GetLength() - count) {
        // throw new StringIndexOutOfBoundsException();
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    if (mPos == 0 && count >= mBuf->GetLength()) {
        AutoPtr< ArrayOf<Char32> > chars = str.GetChars(offset, offset + count);
        FAIL_RETURN(mOut->Write(chars, 0, count));
        return NOERROR;
    }

    Int32 available = mBuf->GetLength() - mPos;
    if (count < available) {
        available = count;
    }

    if (available > 0) {
        AutoPtr< ArrayOf<Char32> > chars = str.GetChars(offset, offset + available);
        mBuf->Copy(mPos, chars, available);
        mPos += available;
    }

    if (mPos == mBuf->GetLength()) {
        mOut->Write(mBuf, 0, mBuf->GetLength());
        mPos = 0;
        if (count > available) {
            offset += available;
            available = count - available;
            if (available >= mBuf->GetLength()) {
                AutoPtr< ArrayOf<Char32> > chars = str.GetChars(offset, offset + available);
                FAIL_RETURN(mOut->Write(chars, 0, available));
                return NOERROR;
            }
            AutoPtr< ArrayOf<Char32> > chars = str.GetChars(offset, offset + available);
            mBuf->Copy(mPos, chars, available);
            mPos += available;
        }
    }
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
