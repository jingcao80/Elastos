
#include "PipedOutputStream.h"
#include "AutoLock.h"
#include "PipedInputStream.h"

using Elastos::Core::ISynchronize;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(PipedOutputStream, OutputStream, IPipedOutputStream)

ECode PipedOutputStream::constructor()
{
    return NOERROR;
}

ECode PipedOutputStream::constructor(
    /* [in] */ IPipedInputStream* target)
{
    VALIDATE_NOT_NULL(target)

    FAIL_RETURN(Connect(target));
    return NOERROR;
}

ECode PipedOutputStream::Close()
{
    // Is the pipe connected?
    AutoPtr<IPipedInputStream> stream = mTarget;
    if (stream != NULL) {
        stream->Done();
        mTarget = NULL;
    }
    return NOERROR;
}

ECode PipedOutputStream::Connect(
    /* [in] */ IPipedInputStream* stream)
{
    VALIDATE_NOT_NULL(stream)

    if (NULL == stream) {
        // throw new NullPointerException("stream == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    synchronized (stream) {
        if (mTarget != NULL) {
            // throw new IOException("Already connected");
            return E_IO_EXCEPTION;
        }
        Boolean is_connected;
        stream->IsConnected(&is_connected);
        if (is_connected) {
            // throw new IOException("Pipe already connected");
            return E_IO_EXCEPTION;
        }
        stream->EstablishConnection();
        mTarget = stream;
    }
    return NOERROR;
}

ECode PipedOutputStream::Flush()
{
    AutoPtr<IPipedInputStream> stream = mTarget;
    if (NULL == stream) {
        return NOERROR;
    }

    synchronized (stream) {
        FAIL_RETURN(ISynchronize::Probe(stream)->NotifyAll());
    }
    return NOERROR;
}

ECode PipedOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr<IPipedInputStream> stream = mTarget;
    if (NULL == stream) {
        // throw new IOException("Pipe not connected");
        return E_IO_EXCEPTION;
    }
    ((PipedInputStream*)stream.Get())->Receive(oneByte);
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
