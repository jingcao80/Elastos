
#include "PipedReader.h"
#include "AutoLock.h"
#include "Arrays.h"
#include "CThread.h"

using Elastos::Utility::Arrays;
using Elastos::Core::CThread;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(PipedReader, Reader, IPipedReader)

const Int32 PipedReader::PIPE_SIZE = 1024;

ECode PipedReader::Close()
{
    synchronized(this) {
        mBuffer = NULL;
        mIsClosed = TRUE;
        NotifyAll();
    }
    return NOERROR;
}

ECode PipedReader::Connect(
    /* [in] */ IPipedWriter* src)
{
    VALIDATE_NOT_NULL(src)

    src->Connect(this);
    return NOERROR;
}

ECode PipedReader::EstablishConnection()
{
    synchronized(this) {
        if (mIsConnected) {
            // throw new IOException("Pipe already connected");
            return E_IO_EXCEPTION;
        }
        if (mIsClosed) {
            // throw new IOException("Pipe is closed");
            return E_IO_EXCEPTION;
        }
        if (NULL == mBuffer) { // We may already have allocated the buffer.
            mBuffer = ArrayOf<Char32>::Alloc(PIPE_SIZE);
        }
        mIsConnected = TRUE;
    }
    return NOERROR;
}

ECode PipedReader::Read(
    /* [out] */ Int32* rev)
{
    VALIDATE_NOT_NULL(rev)

    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(1);
    Int32 result;
    FAIL_RETURN(Read(chars, 0, 1, &result));
    *rev = result != -1 ? (*chars)[0] : result;
    return NOERROR;
}

 ECode PipedReader::Read(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [out] */ Int32* rev)
 {
    VALIDATE_NOT_NULL(buffer)
    VALIDATE_NOT_NULL(rev)

    synchronized(this) {
        if (!mIsConnected) {
            // throw new IOException("Pipe not connected");
            return E_IO_EXCEPTION;
        }
        if (NULL == buffer) {
            // throw new IOException("Pipe is closed");
            return E_IO_EXCEPTION;
        }
        FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), offset, count));
        if (0 == count) {
            *rev = 0;
            return NOERROR;
        }
        /**
         * Set the last thread to be reading on this PipedReader. If
         * lastReader dies while someone is waiting to write an IOException
         * of "Pipe broken" will be thrown in receive()
         */
        mLastReader = CThread::GetCurrentThread();
        // try {
            Boolean first = TRUE;
            while (-1 == mIn) {
                // Are we at end of stream?
                if (mIsClosed) {
                    *rev = -1;
                    return NOERROR;
                }
                Boolean is_alive;
                mLastWriter->IsAlive(&is_alive);
                if (!first && mLastWriter != NULL && !is_alive) {
                    // throw new IOException("Pipe broken");
                    return E_IO_EXCEPTION;
                }
                first = FALSE;
                // Notify callers of receive()
                FAIL_RETURN(NotifyAll());
                Wait(1000);
            }
        // } catch (InterruptedException e) {
            // IoUtils.throwInterruptedIoException();
        // }

        Int32 copyLength = 0;
        /* Copy chars from out to end of buffer first */
        if (mOut >= mIn) {
            copyLength = count > mBuffer->GetLength() - mOut ? mBuffer->GetLength() - mOut : count;
            buffer->Copy(offset, mBuffer, mOut, copyLength);
            mOut += copyLength;
            if (mOut == mBuffer->GetLength()) {
                mOut = 0;
            }
            if (mOut == mIn) {
                // empty buffer
                mIn = -1;
                mOut = 0;
            }
        }

        /*
         * Did the read fully succeed in the previous copy or is the buffer
         * empty?
         */
        if (copyLength == count || -1 == mIn) {
            *rev = copyLength;
            return NOERROR;
        }

        Int32 charsCopied = copyLength;
        /* Copy bytes from 0 to the number of available bytes */
        copyLength = mIn - mOut > count - copyLength ? count - copyLength : mIn - mOut;
        buffer->Copy(offset + charsCopied, mBuffer, mOut, copyLength);
        mOut += copyLength;
        if (mOut == mIn) {
            // empty buffer
            mIn = -1;
            mOut = 0;
        }
        *rev = charsCopied + copyLength;
    }
    return NOERROR;
}

ECode PipedReader::Ready(
    /* [out] */ Boolean* rev)
{
    VALIDATE_NOT_NULL(rev)

    synchronized(this) {
        if (!mIsConnected) {
            // throw new IOException("Pipe not connected");
            return E_IO_EXCEPTION;
        }
        if (NULL == mBuffer) {
            // throw new IOException("Pipe is closed");
            return E_IO_EXCEPTION;
        }
        *rev = mIn != -1;
    }
    return NOERROR;
}

ECode PipedReader::Receive(
    /* [in] */ Char32 oneChar)
{
    synchronized(this) {
        if (NULL == mBuffer) {
            // throw new IOException("Pipe is closed");
            return E_IO_EXCEPTION;
        }
        Boolean is_alive;
        mLastReader->IsAlive(&is_alive);
        if (mLastReader != NULL && !is_alive) {
            // throw new IOException("Pipe broken");
            return E_IO_EXCEPTION;
        }
        /*
        * Set the last thread to be writing on this PipedWriter. If
        * lastWriter dies while someone is waiting to read an IOException
        * of "Pipe broken" will be thrown in read()
        */
        mLastWriter = CThread::GetCurrentThread();
        // try {
            while (mBuffer != NULL && mOut == mIn) {
                FAIL_RETURN(NotifyAll());
                Wait(1000);
                mLastReader->IsAlive(&is_alive);
                if (mLastReader != NULL && !is_alive) {
                    // throw new IOException("Pipe broken");
                    return E_IO_EXCEPTION;
                }
            }
        // } catch (InterruptedException e) {
            // IoUtils.throwInterruptedIoException();
        // }
        if (NULL == mBuffer) {
            // throw new IOException("Pipe is closed");
            return E_IO_EXCEPTION;
        }
        if (-1 == mIn) {
            mIn = 0;
        }
        (*mBuffer)[mIn++] = oneChar;
        if (mIn == mBuffer->GetLength()) {
            mIn = 0;
        }
    }
    return NOERROR;
}

ECode PipedReader::Receive(
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    VALIDATE_NOT_NULL(chars)

    synchronized(this) {
        FAIL_RETURN(Arrays::CheckOffsetAndCount(chars->GetLength(), offset, count));
        if (NULL == mBuffer) {
            // throw new IOException("Pipe is closed");
            return E_IO_EXCEPTION;
        }
        Boolean is_alive;
        mLastReader->IsAlive(&is_alive);
        if (mLastReader != NULL && !is_alive) {
            // throw new IOException("Pipe broken");
            return E_IO_EXCEPTION;
        }
        /**
         * Set the last thread to be writing on this PipedWriter. If
         * lastWriter dies while someone is waiting to read an IOException
         * of "Pipe broken" will be thrown in read()
         */
        mLastWriter = CThread::GetCurrentThread();
        while (count > 0) {
            // try {
                while (mBuffer != NULL && mOut == mIn) {
                    FAIL_RETURN(NotifyAll());
                    Wait(1000);
                    mLastReader->IsAlive(&is_alive);
                    if (mLastReader != NULL && !is_alive) {
                        // throw new IOException("Pipe broken");
                        return E_IO_EXCEPTION;
                    }
                }
            // } catch (InterruptedException e) {
                // IoUtils.throwInterruptedIoException();
            // }
            if (NULL == mBuffer) {
                // throw new IOException("Pipe is closed");
                return E_IO_EXCEPTION;
            }
            if (-1 == mIn) {
                mIn = 0;
            }
            if (mIn >= mOut) {
                Int32 length = mBuffer->GetLength() - mIn;
                if (count < length) {
                    length = count;
                }
                mBuffer->Copy(mIn, chars, offset, length);
                offset += length;
                count -= length;
                mIn += length;
                if (mIn == mBuffer->GetLength()) {
                    mIn = 0;
                }
            }
            if (count > 0 && mIn != mOut) {
                Int32 length = mOut - mIn;
                if (count < length) {
                    length = count;
                }
                mBuffer->Copy(mIn, chars, offset, length);
                offset += length;
                count -= length;
                mIn += length;
            }
        }
    }
    return NOERROR;
}

ECode PipedReader::Done()
{
    synchronized(this) {
        mIsClosed = true;
        FAIL_RETURN(NotifyAll());
    }
    return NOERROR;
}

ECode PipedReader::constructor()
{
    return NOERROR;
}

ECode PipedReader::constructor(
    /* [in] */ IPipedWriter* out)
{
    VALIDATE_NOT_NULL(out)

    FAIL_RETURN(Connect(out));
    return NOERROR;
}

ECode PipedReader::constructor(
    /* [in] */ Int32 pipeSize)
{
    if (pipeSize <= 0) {
        // throw new IllegalArgumentException("pipe size " + pipeSize + " too small");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBuffer = ArrayOf<Char32>::Alloc(pipeSize);
    return NOERROR;
}

ECode PipedReader::constructor(
    /* [in] */ IPipedWriter* out,
    /* [in] */ Int32 pipeSize)
{
    VALIDATE_NOT_NULL(out)

    FAIL_RETURN(constructor(pipeSize));
    FAIL_RETURN(Connect(out));
    return NOERROR;
}

ECode PipedReader::IsClosed(
    /* [out] */ Boolean* isClosed)
{
    VALIDATE_NOT_NULL(isClosed)

    *isClosed = mIsClosed;
    return NOERROR;
}

// Add additionally
ECode PipedReader::IsConnected(
    /* [out] */ Boolean* isConnected)
{
    VALIDATE_NOT_NULL(isConnected)

    *isConnected = mIsConnected;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos

