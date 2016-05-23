
#include "PipedInputStream.h"
#include "AutoLock.h"
#include "CThread.h"
#include "Arrays.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::CThread;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(PipedInputStream, InputStream, IPipedInputStream)

const Int32 PipedInputStream::PIPE_SIZE = 1024;

ECode PipedInputStream::constructor(
    /* [in] */ IPipedOutputStream* ipos)
{
    VALIDATE_NOT_NULL(ipos)

    return Connect(ipos);
}

ECode PipedInputStream::constructor(Int32 pipeSize)
{
    if (pipeSize <= 0) {
        // throw new IllegalArgumentException("pipe size " + pipeSize + " too small");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mBuffer = ArrayOf<Byte>::Alloc(pipeSize);
    return NOERROR;
}

ECode PipedInputStream::constructor(
    /* [in] */ IPipedOutputStream* ipos,
    /* [in] */ Int32 pipeSize)
{
    VALIDATE_NOT_NULL(ipos)

    FAIL_RETURN(constructor(pipeSize));
    return Connect(ipos);
}

ECode PipedInputStream::Available(
    /* [out] */ Int32* rev)
{
    VALIDATE_NOT_NULL(rev)

    {    AutoLock syncLock(this);
        if (NULL == mBuffer || -1 == mIn) {
            *rev = 0;
        } else {
            *rev = (mIn <= mOut ? mBuffer->GetLength() - mOut + mIn : mIn - mOut);
        }
    }
    return NOERROR;
}

ECode PipedInputStream::Close()
{
    {    AutoLock syncLock(this);
        mBuffer = NULL;
        FAIL_RETURN(NotifyAll());
    }
    return NOERROR;
}

ECode PipedInputStream::Connect(
    /* [in] */ IPipedOutputStream* src)
{
    VALIDATE_NOT_NULL(src)

    return src->Connect((IPipedInputStream*)Probe(EIID_IPipedInputStream));
}

 ECode PipedInputStream::EstablishConnection()
 {
    {    AutoLock syncLock(this);
        if (mIsConnected) {
            // throw new IOException("Pipe already connected");
            return E_IO_EXCEPTION;
        }
        if (NULL == mBuffer) { // We may already have allocated the buffer.
            mBuffer = ArrayOf<Byte>::Alloc(PIPE_SIZE);
        }
        mIsConnected = true;
    }
    return NOERROR;
}

ECode PipedInputStream::Read(
    /* [out] */ Int32* rev)
{
    VALIDATE_NOT_NULL(rev)

    {    AutoLock syncLock(this);
        if (!mIsConnected) {
            // throw new IOException("Not connected");
            return E_IO_EXCEPTION;
        }
        if (NULL == mBuffer) {
            // throw new IOException("InputStream is closed");
            return E_IO_EXCEPTION;
        }

        /**
         * Set the last thread to be reading on this PipedInputStream. If
         * lastReader dies while someone is waiting to write an IOException of
         * "Pipe broken" will be thrown in receive()
         */
        mLastReader = CThread::GetCurrentThread();
//            try {
        Int32 attempts = 3;
        while (-1 == mIn) {
            // Are we at end of stream?
            if (mIsClosed) {
                *rev = -1;
                return NOERROR;
            }
            Boolean is_alive;
            if ((attempts-- <= 0) && (mLastWriter != NULL) && (mLastWriter->IsAlive(&is_alive), !is_alive)) {
                // throw new IOException("Pipe broken");
                return E_IO_EXCEPTION;
            }
            // Notify callers of receive()
            FAIL_RETURN(NotifyAll());
            Wait(1000);
        }
//            } catch (InterruptedException e) {
//                IoUtils.throwInterruptedIoException();
//            }

        Int32 result = (*mBuffer)[mOut++] & 0xff;
        if (mOut == mBuffer->GetLength()) {
            mOut = 0;
        }
        if (mOut == mIn) {
            // empty buffer
            mIn = -1;
            mOut = 0;
        }

        // let blocked writers write to the newly available buffer space
        FAIL_RETURN(NotifyAll());

        *rev = result;
    }
    return NOERROR;
}

ECode PipedInputStream::Read(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* rev)
{
    VALIDATE_NOT_NULL(bytes)

    {    AutoLock syncLock(this);
        FAIL_RETURN(Arrays::CheckOffsetAndCount(bytes->GetLength(), byteOffset, byteCount));
        if (0 == byteCount) {
            *rev = 0;
            return NOERROR;
        }

        if (!mIsConnected) {
            // throw new IOException("Not connected");
            return E_IO_EXCEPTION;
        }

        if (NULL == mBuffer) {
            // throw new IOException("InputStream is closed");
            return E_IO_EXCEPTION;
        }

        /*
         * Set the last thread to be reading on this PipedInputStream. If
         * lastReader dies while someone is waiting to write an IOException of
         * "Pipe broken" will be thrown in receive()
         */
        mLastReader = CThread::GetCurrentThread();
//            try {
        Int32 attempts = 3;
        while (-1 == mIn) {
            // Are we at end of stream?
            if (mIsClosed) {
                *rev = -1;
                return NOERROR;
            }
            Boolean is_alive;
            if ((attempts-- <= 0) && (mLastWriter != NULL) && (mLastWriter->IsAlive(&is_alive), !is_alive)) {
                // throw new IOException("Pipe broken");
                return E_IO_EXCEPTION;
            }
            // Notify callers of receive()
            FAIL_RETURN(NotifyAll());
            Wait(1000);
        }
//            } catch (InterruptedException e) {
//                IoUtils.throwInterruptedIoException();
//            }

        Int32 totalCopied = 0;

        // copy bytes from out thru the end of buffer
        if (mOut >= mIn) {
            Int32 leftInBuffer = mBuffer->GetLength() - mOut;
            Int32 length = leftInBuffer < byteCount ? leftInBuffer : byteCount;
            bytes->Copy(byteOffset, mBuffer, mOut, length);
            mOut += length;
            if (mBuffer->GetLength() == mOut) {
                mOut = 0;
            }
            if (mOut == mIn) {
                // empty buffer
                mIn = -1;
                mOut = 0;
            }
            totalCopied += length;
        }

        // copy bytes from out thru in
        if (totalCopied < byteCount && mIn != -1) {
            Int32 leftInBuffer = mIn - mOut;
            Int32 leftToCopy = byteCount - totalCopied;
            Int32 length = leftToCopy < leftInBuffer ? leftToCopy : leftInBuffer;
            bytes->Copy(byteOffset, mBuffer, mOut, length);
            mOut += length;
            if (mOut == mIn) {
                // empty buffer
                mIn = -1;
                mOut = 0;
            }
            totalCopied += length;
        }

        // let blocked writers write to the newly available buffer space
        FAIL_RETURN(NotifyAll());

        *rev = totalCopied;
    }
    return NOERROR;
}

ECode PipedInputStream::Done()
{
    {    AutoLock syncLock(this);
        mIsClosed = true;
        FAIL_RETURN(NotifyAll());
    }
    return NOERROR;
}

ECode PipedInputStream::Receive(
    /* [in] */ Int32 oneByte)
{
    {    AutoLock syncLock(this);
        if (NULL == mBuffer || mIsClosed) {
            // throw new IOException("Pipe is closed");
            return E_IO_EXCEPTION;
        }

        /*
         * Set the last thread to be writing on this PipedInputStream. If
         * lastWriter dies while someone is waiting to read an IOException of
         * "Pipe broken" will be thrown in read()
         */
        mLastWriter = CThread::GetCurrentThread();
            // try {
            while (mBuffer != NULL && mOut == mIn) {
                Boolean is_alive;
                if ((mLastWriter != NULL) && (mLastWriter->IsAlive(&is_alive), !is_alive)) {
                    // throw new IOException("Pipe broken");
                    return E_IO_EXCEPTION;
                }

                FAIL_RETURN(NotifyAll());
                Wait(1000);
            }
            // } catch (InterruptedException e) {
            //     IoUtils.throwInterruptedIoException();
            // }
        if (mBuffer == NULL) {
            // throw new IOException("Pipe is closed");
            return E_IO_EXCEPTION;
        }
        if (mIn == -1) {
            mIn = 0;
        }
        Boolean is_alive;
        if ((mLastWriter != NULL) && (mLastWriter->IsAlive(&is_alive), !is_alive)) {
            // throw new IOException("Pipe broken");
            return E_IO_EXCEPTION;
        }

        (*mBuffer)[mIn++] = (byte) oneByte;
        if (mIn == mBuffer->GetLength()) {
            mIn = 0;
        }

        // let blocked readers read the newly available data
        FAIL_RETURN(NotifyAll());
    }
    return NOERROR;
}

ECode PipedInputStream::IsConnected(
    /* [out] */ Boolean* rev)
{
    *rev = mIsConnected;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos

