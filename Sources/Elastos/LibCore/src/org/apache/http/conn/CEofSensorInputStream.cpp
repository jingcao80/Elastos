
#include "org/apache/http/conn/CEofSensorInputStream.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {

CAR_INTERFACE_IMPL_2(CEofSensorInputStream, InputStream, IEofSensorInputStream, IConnectionReleaseTrigger)

CAR_OBJECT_IMPL(CEofSensorInputStream)

ECode CEofSensorInputStream::IsReadAllowed(
    /* [out] */ Boolean* isReadAllowed)
{
    VALIDATE_NOT_NULL(isReadAllowed)
    if (mSelfClosed) {
        Logger::E("CEofSensorInputStream", "Attempted read on closed stream.");
        *isReadAllowed = FALSE;
        return E_IO_EXCEPTION;
    }
    *isReadAllowed = (mWrappedStream != NULL);
    return NOERROR;
}

ECode CEofSensorInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;
    Int32 l = -1;

    Boolean isReadAllowed;
    if (IsReadAllowed(&isReadAllowed), isReadAllowed) {
        // try {
        ECode ec = mWrappedStream->Read(&l);
        if (FAILED(ec)) {
            CheckAbort();
            return ec;
        }
        ec = CheckEOF(l);
        if (FAILED(ec)) {
            CheckAbort();
            return ec;
        }
        // } catch (IOException ex) {
        //     checkAbort();
        //     throw ex;
        // }
    }

    *value = l;
    return NOERROR;
}

ECode CEofSensorInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    Int32 l = -1;

    Boolean isReadAllowed;
    if (IsReadAllowed(&isReadAllowed), isReadAllowed) {
        // try {
        ECode ec = mWrappedStream->Read(buffer, offset, length, &l);
        if (FAILED(ec)) {
            CheckAbort();
            return ec;
        }
        ec = CheckEOF(l);
        if (FAILED(ec)) {
            CheckAbort();
            return ec;
        }
        // } catch (IOException ex) {
        //     checkAbort();
        //     throw ex;
        // }
    }

    *number = l;
    return NOERROR;
}

ECode CEofSensorInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    Int32 l = -1;

    Boolean isReadAllowed;
    if (IsReadAllowed(&isReadAllowed), isReadAllowed) {
        // try {
        ECode ec = mWrappedStream->Read(buffer, &l);
        if (FAILED(ec)) {
            CheckAbort();
            return ec;
        }
        ec = CheckEOF(l);
        if (FAILED(ec)) {
            CheckAbort();
            return ec;
        }
        // } catch (IOException ex) {
        //     checkAbort();
        //     throw ex;
        // }
    }

    *number = l;
    return NOERROR;
}

ECode CEofSensorInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    Int32 a = 0; // not -1

    Boolean isReadAllowed;
    if (IsReadAllowed(&isReadAllowed), isReadAllowed) {
        // try {
        ECode ec = mWrappedStream->Available(&a);
        if (FAILED(ec)) {
            CheckAbort();
            return ec;
        }
        // no checkEOF() here, available() can't trigger EOF
        // } catch (IOException ex) {
        //     checkAbort();
        //     throw ex;
        // }
    }

    *number = a;
    return NOERROR;
}

ECode CEofSensorInputStream::Close()
{
    // tolerate multiple calls to close()
    mSelfClosed = TRUE;
    return CheckClose();
}

ECode CEofSensorInputStream::CheckEOF(
    /* [in] */ Int32 eof)
{
    if ((mWrappedStream != NULL) && (eof < 0)) {
        // try {
        Boolean scws = TRUE; // should close wrapped stream?
        if (mEofWatcher != NULL) mEofWatcher->EofDetected(mWrappedStream, &scws);
        if (scws) ICloseable::Probe(mWrappedStream)->Close();
        // } finally {
        //     wrappedStream = null;
        // }
        mWrappedStream = NULL;
    }
    return NOERROR;
}

ECode CEofSensorInputStream::CheckClose()
{
    if (mWrappedStream != NULL) {
        // try {
        Boolean scws = TRUE; // should close wrapped stream?
        if (mEofWatcher != NULL) mEofWatcher->StreamClosed(mWrappedStream, &scws);
        if (scws) ICloseable::Probe(mWrappedStream)->Close();
        // } finally {
        //     wrappedStream = null;
        // }
        mWrappedStream = NULL;
    }
    return NOERROR;
}

ECode CEofSensorInputStream::CheckAbort()
{
    if (mWrappedStream != NULL) {
        // try {
        Boolean scws = TRUE; // should close wrapped stream?
        if (mEofWatcher != NULL) mEofWatcher->StreamAbort(mWrappedStream, &scws);
        if (scws) ICloseable::Probe(mWrappedStream)->Close();
        // } finally {
        //     wrappedStream = null;
        // }
        mWrappedStream = NULL;
    }
    return NOERROR;
}

ECode CEofSensorInputStream::ReleaseConnection()
{
    return Close();
}

ECode CEofSensorInputStream::AbortConnection()
{
    // tolerate multiple calls
    mSelfClosed = TRUE;
    return CheckAbort();
}

ECode CEofSensorInputStream::constructor(
    /* [in] */ IInputStream* in,
    /* [in] */ IEofSensorWatcher* watcher)
{
    if (in == NULL) {
        Logger::E("CEofSensorInputStream", "Wrapped stream may not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mWrappedStream = in;
    mSelfClosed = FALSE;
    mEofWatcher = watcher;
    return NOERROR;
}

} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
