
#include "CChannels.h"
#include "ByteBuffer.h"
#include "InputStream.h"
#include "OutputStream.h"
#include "Charset.h"
#include "CStreams.h"
#include "CInputStreamReader.h"
#include "COutputStreamWriter.h"
#include "elastos/core/Math.h"

using Elastos::IO::InputStream;
using Elastos::IO::OutputStream;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::Charset::Charset;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace IO {
namespace Channels{

//==========================================================
//       CChannels::ChannelInputStream
//==========================================================

CChannels::ChannelInputStream::ChannelInputStream(
    /* [in] */ IReadableByteChannel* channel)
{
    assert(NULL != channel);
    mChannel = channel;
}

ECode CChannels::ChannelInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Int32 nRead;
    AutoPtr<IStreams> stream;
    CStreams::AcquireSingleton((IStreams**)&stream);
    stream->ReadSingleByte(this, &nRead);
    *value = nRead;
    return NOERROR;
}

ECode CChannels::ChannelInputStream::Read(
    /* [in] */ ArrayOf<Byte>* target,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)

    AutoPtr<IByteBuffer> buffer;
    ByteBuffer::Wrap(target, offset, length, (IByteBuffer**)&buffer);
    CheckBlocking(IChannel::Probe(mChannel));
    return mChannel->Read(buffer, number);
}

ECode CChannels::ChannelInputStream::Available(
    /* [out] */ Int32* number)
{
    if(IFileChannel::Probe(mChannel)) {
        AutoPtr<IFileChannel> fileChannel = IFileChannel::Probe(mChannel);
        Int64 result, size, position;
        fileChannel->GetSize(&size);
        fileChannel->GetPosition(&position);
        result = size - position;
        *number = result > Elastos::Core::Math::INT32_MAX_VALUE ? Elastos::Core::Math::INT32_MAX_VALUE : (Int32)result;
        return NOERROR;
    }
    else {
        return InputStream::Available(number);
    }
}

ECode CChannels::ChannelInputStream::Close()
{
    return ICloseable::Probe(mChannel)->Close();
}

//==========================================================
//       CChannels::ChannelOutputStream
//==========================================================

CChannels::ChannelOutputStream::ChannelOutputStream(
    /* [in] */ IWritableByteChannel* channel)
{
    assert(channel == NULL);
    mChannel = channel;
}

ECode CChannels::ChannelOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr< ArrayOf<Byte> > wrappedByte = ArrayOf<Byte>::Alloc(1);
    (*wrappedByte)[0] = (Byte) oneByte;
    return Write(wrappedByte, 0, wrappedByte->GetLength());
}

ECode CChannels::ChannelOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* source,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    AutoPtr<IByteBuffer> buffer;
    ByteBuffer::Wrap(source, offset, length, (IByteBuffer**)&buffer);
    CheckBlocking(IChannel::Probe(mChannel));
    Int32 total = 0;
    while (total < length) {
        Int32 wb = 0;
        mChannel->Write(buffer, &wb);
        total += wb;
    }
    return NOERROR;
}

ECode CChannels::ChannelOutputStream::Close()
{
    return ICloseable::Probe(mChannel)->Close();
}

//==========================================================
//       CChannels::InputStreamChannel
//==========================================================

CChannels::InputStreamChannel::InputStreamChannel(
    /* [in] */ IInputStream* inputStream)
{
    assert(inputStream == NULL);
    mInputStream = inputStream;
}

CAR_INTERFACE_IMPL(CChannels::InputStreamChannel, AbstractInterruptibleChannel, IReadableByteChannel)

ECode CChannels::InputStreamChannel::Read(
    /* [in] */ IByteBuffer* target,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Boolean isflag = FALSE;
    if (IsOpen(&isflag), !isflag) {
        // throw new ClosedChannelException();
        return E_CLOSED_CHANNEL_EXCEPTION;
    }
    Int32 bytesRemain = 0;
    IBuffer::Probe(target)->GetRemaining(&bytesRemain);
    AutoPtr< ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(bytesRemain);
    Int32 readCount = 0;
    // try {
        Begin();
        mInputStream->Read(bytes, &readCount);
    // } finally {
        End(readCount >= 0);
    // }
    if (readCount > 0) {
        target->Put(bytes, 0, readCount);
    }
    *value = readCount;
    return NOERROR;
}

ECode CChannels::InputStreamChannel::ImplCloseChannel()
{
    return mInputStream->Close();
}

//==========================================================
//       CChannels::OutputStreamChannel
//==========================================================

CChannels::OutputStreamChannel::OutputStreamChannel(
    /* [in] */ IOutputStream* outputStream)
{
    assert(outputStream == NULL);
    mOutputStream = outputStream;
}

CAR_INTERFACE_IMPL(CChannels::OutputStreamChannel, AbstractInterruptibleChannel, IWritableByteChannel)

ECode CChannels::OutputStreamChannel::Write(
    /* [in] */ IByteBuffer* source,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Boolean isflag = FALSE;
    if (IsOpen(&isflag), !isflag) {
        // throw new ClosedChannelException();
        return E_CLOSED_CHANNEL_EXCEPTION;
    }
    Int32 bytesRemain = 0;
    IBuffer::Probe(source)->GetRemaining(&bytesRemain);
    if (bytesRemain == 0) {
        *value = 0;
        return NOERROR;
    }
    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(bytesRemain);
    source->Get(buf);
    // try {
        Begin();
        FAIL_RETURN(mOutputStream->Write(buf, 0, bytesRemain));
    // } finally {
        End(bytesRemain >= 0);
    // }
    *value = bytesRemain;
    return NOERROR;
}

ECode CChannels::OutputStreamChannel::ImplCloseChannel()
{
    return mOutputStream->Close();
}

//==========================================================
//       CChannels
//==========================================================

CAR_INTERFACE_IMPL(CChannels, Singleton, IChannels)

CAR_SINGLETON_IMPL(CChannels)

ECode CChannels::NewChannel(
    /* [in] */ IInputStream* inputStream,
    /* [out] */ IReadableByteChannel** inputChannel)
{
    return _NewChannel(inputStream, inputChannel);
}

ECode CChannels::NewChannel(
    /* [in] */ IOutputStream* outputStream,
    /* [out] */ IWritableByteChannel** outputChannel)
{
    return _NewChannel(outputStream, outputChannel);
}

ECode CChannels::NewInputStream(
    /* [in] */ IReadableByteChannel* channel,
    /* [out] */ IInputStream** ppInputStream)
{
    return _NewInputStream(channel, ppInputStream);
}

ECode CChannels::NewOutputStream(
    /* [in] */ IWritableByteChannel* channel,
    /* [out] */ IOutputStream** ppOutputStream)
{
    return _NewOutputStream(channel, ppOutputStream);
}

ECode CChannels::NewReader(
    /* [in] */ IReadableByteChannel* channel,
    /* [in] */ ICharsetDecoder * decoder,
    /* [in] */ Int32 minBufferCapacity,
    /* [out] */ IReader** ppReader)
{
    return _NewReader(channel, decoder, minBufferCapacity, ppReader);
}

ECode CChannels::NewReader(
    /* [in] */ IReadableByteChannel* channel,
    /* [in] */ const String& charsetName,
    /* [out] */ IReader** ppReader)
{
    return _NewReader(channel, charsetName, ppReader);
}

ECode CChannels::NewWriter(
    /* [in] */ IWritableByteChannel* channel,
    /* [in] */ ICharsetEncoder* encoder,
    /* [in] */ Int32 minBufferCapacity,
    /* [out] */ IWriter** ppWriter)
{
    return _NewWriter(channel, encoder, minBufferCapacity, ppWriter);
}

ECode CChannels::NewWriter(
    /* [in] */ IWritableByteChannel* channel,
    /* [in] */ const String& charsetName,
    /* [out] */ IWriter** ppWriter)
{
    return _NewWriter(channel, charsetName, ppWriter);
}

ECode CChannels::CheckBlocking(
    /* [in] */ IChannel* channel)
{
    if(ISelectableChannel::Probe(channel) != NULL)
    {
        Boolean ret;
        ((ISelectableChannel*)channel)->IsBlocking(&ret);
        if(ret)
        {
            return E_ILLEGAL_BLOCKING_MODE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CChannels::_NewChannel(
    /* [in] */ IInputStream* inputStream,
    /* [out] */ IReadableByteChannel** inputChannel)
{
    VALIDATE_NOT_NULL(inputChannel)

    *inputChannel = (IReadableByteChannel*) new InputStreamChannel(inputStream);
    REFCOUNT_ADD(*inputChannel)
    return NOERROR;
}

ECode CChannels::_NewChannel(
    /* [in] */ IOutputStream* outputStream,
    /* [out] */ IWritableByteChannel** outputChannel)
{
    VALIDATE_NOT_NULL(outputChannel)

    *outputChannel = (IWritableByteChannel*) new OutputStreamChannel(outputStream);
    REFCOUNT_ADD(*outputChannel)
    return NOERROR;
}

ECode CChannels::_NewInputStream(
    /* [in] */ IReadableByteChannel* channel,
    /* [out] */ IInputStream** ppInputStream)
{
    VALIDATE_NOT_NULL(ppInputStream)

    *ppInputStream = (IInputStream*) new ChannelInputStream(channel);
    REFCOUNT_ADD(*ppInputStream)
    return NOERROR;
}

ECode CChannels::_NewOutputStream(
    /* [in] */ IWritableByteChannel* channel,
    /* [out] */ IOutputStream** ppOutputStream)
{
    VALIDATE_NOT_NULL(ppOutputStream)

    *ppOutputStream = (IOutputStream*) new ChannelOutputStream(channel);
    REFCOUNT_ADD(*ppOutputStream)
    return NOERROR;
}

ECode CChannels::_NewReader(
    /* [in] */ IReadableByteChannel* channel,
    /* [in] */ ICharsetDecoder * decoder,
    /* [in] */ Int32 minBufferCapacity,
    /* [out] */ IReader** ppReader)
{
    /*
     * This method doesn't honor minBufferCapacity. Ignoring that parameter
     * saves us from having to add a hidden constructor to InputStreamReader.
     */
    AutoPtr<IInputStream> res = (IInputStream*) new ChannelInputStream(channel);
    AutoPtr<IInputStreamReader> is;
    CInputStreamReader::New(res, decoder, (IInputStreamReader**)&is);
    *ppReader = IReader::Probe(is);
    REFCOUNT_ADD(*ppReader)
    return NOERROR;
}

ECode CChannels::_NewReader(
    /* [in] */ IReadableByteChannel* channel,
    /* [in] */ const String& charsetName,
    /* [out] */ IReader** ppReader)
{
    if (charsetName.IsNull()) {
        // throw new NullPointerException("charsetName == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ICharset> reschar;
    Elastos::IO::Charset::Charset::ForName(charsetName, (ICharset**)&reschar);
    AutoPtr<ICharsetDecoder> resdec;
    reschar->NewDecoder((ICharsetDecoder**)&resdec);
    return _NewReader(channel, resdec, -1, ppReader);
}

ECode CChannels::_NewWriter(
    /* [in] */ IWritableByteChannel* channel,
    /* [in] */ ICharsetEncoder* encoder,
    /* [in] */ Int32 minBufferCapacity,
    /* [out] */ IWriter** ppWriter)
{
    VALIDATE_NOT_NULL(ppWriter)

    AutoPtr<IOutputStream> res = (IOutputStream*) new ChannelOutputStream(channel);
    AutoPtr<IOutputStreamWriter> os;
    COutputStreamWriter::New(res, encoder, (IOutputStreamWriter**)&os);
    *ppWriter = IWriter::Probe(os);
    REFCOUNT_ADD(*ppWriter)
    return NOERROR;
}

ECode CChannels::_NewWriter(
    /* [in] */ IWritableByteChannel* channel,
    /* [in] */ const String& charsetName,
    /* [out] */ IWriter** ppWriter)
{
    if (charsetName.IsNull()) {
        // throw new NullPointerException("charsetName == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ICharset> reschar;
    Elastos::IO::Charset::Charset::ForName(charsetName, (ICharset**)&reschar);
    AutoPtr<ICharsetEncoder> resdec;
    reschar->NewEncoder((ICharsetEncoder**)&resdec);
    return _NewWriter(channel, resdec, -1, ppWriter);
}

} // namespace Channels
} // namespace IO
} // namespace Elastos
