
#include "PipedWriter.h"
#include "AutoLock.h"
#include "PipedReader.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(PipedWriter, Writer, IPipedWriter)

ECode PipedWriter::constructor()
{
    return NOERROR;
}

ECode PipedWriter::constructor(
    /* [in] */ IPipedReader* destination)
{
    Writer::constructor(ISynchronize::Probe(destination));
    FAIL_RETURN(Connect(destination));
    return NOERROR;
}

ECode PipedWriter::Close()
{
    AutoPtr<IPipedReader> reader = mDestination;
    if (reader != NULL) {
        ((PipedReader*)reader.Get())->Done();
        mIsClosed = TRUE;
        mDestination = NULL;
    }
    return NOERROR;
}

ECode PipedWriter::Connect(
    /* [in] */ IPipedReader* reader)
{
    if (NULL == reader) {
        // throw new NullPointerException("reader == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    {    AutoLock syncLock(reader);
        if (mDestination != NULL) {
            // throw new IOException("Pipe already connected");
            return E_IO_EXCEPTION;
        }
        reader->EstablishConnection();
        this->mLock = ISynchronize::Probe(reader);
        this->mDestination = reader;
    }
    return NOERROR;
}

ECode PipedWriter::Flush()
{
    AutoPtr<IPipedReader> reader = mDestination;
    if (mIsClosed) {
        // throw new IOException("Pipe is closed");
        return E_IO_EXCEPTION;
    }
    if (NULL == reader) {
        return NOERROR;
    }

    {    AutoLock syncLock(reader);
        Boolean is_closed;
        ((PipedReader*)(reader.Get()))->IsClosed(&is_closed);
        if (is_closed) {
            // throw new IOException("Pipe is broken");
            return E_IO_EXCEPTION;
        }
        FAIL_RETURN(ISynchronize::Probe(reader)->NotifyAll());
    }
    return NOERROR;
}

ECode PipedWriter::Write(
    /* [in] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    AutoPtr<IPipedReader> reader = mDestination;
    if (NULL == reader) {
        // throw new IOException("Pipe not connected");
        return E_IO_EXCEPTION;
    }
    return ((PipedReader*)(reader.Get()))->Receive(buffer, offset, count);
}

ECode PipedWriter::Write(
    /* [in] */ Int32 c)
{
    AutoPtr<IPipedReader> reader = mDestination;
    if (NULL == reader) {
        // throw new IOException("Pipe not connected");
        return E_IO_EXCEPTION;
    }
    return ((PipedReader*)(reader.Get()))->Receive((Char32) c);
}

} // namespace IO
} // namespace Elastos
