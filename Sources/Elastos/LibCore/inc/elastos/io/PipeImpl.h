#ifndef __ELASTOS_IO_PIPEIMPL_H__
#define __ELASTOS_IO_PIPEIMPL_H__

#include "Pipe.h"
#include "SinkChannel.h"
#include "SourceChannel.h"

using Elastos::IO::IFileDescriptorChannel;
using Elastos::IO::Channels::ISocketChannel;
using Elastos::IO::Channels::Pipe;
using Elastos::IO::Channels::ISinkChannel;
using Elastos::IO::Channels::ISourceChannel;
using Elastos::IO::Channels::SinkChannel;
using Elastos::IO::Channels::SourceChannel;

namespace Elastos {
namespace IO {

class PipeImpl : public Pipe
{
private:
    class PipeSourceChannel
        : public SourceChannel
        , public IFileDescriptorChannel
    {
    public:
        PipeSourceChannel();

        CARAPI constructor(
            /* [in] */ ISelectorProvider* provider,
            /* [in] */ IFileDescriptor* fd);

        CAR_INTERFACE_DECL()

        CARAPI Read(
            /* [in] */ IByteBuffer* buffer,
            /* [out] */ Int32* nRead);

        CARAPI Read(
            /* [in] */ ArrayOf<IByteBuffer*>* buffers,
            /* [out] */ Int64* nRead);

        CARAPI Read(
            /* [in] */ ArrayOf<IByteBuffer*>* buffers,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 length,
            /* [out] */ Int64* nRead);

        CARAPI GetFD(
            /* [out] */ IFileDescriptor** descriptor);

    protected:
        CARAPI ImplCloseSelectableChannel();

        CARAPI ImplConfigureBlocking(
            /* [in] */ Boolean blocking);

    private:
        AutoPtr<IFileDescriptor> mFd;
        AutoPtr<ISocketChannel> mChannel;
    };

    class PipeSinkChannel
        : public SinkChannel
        , public IFileDescriptorChannel
    {
    public:
        PipeSinkChannel();

        CARAPI constructor(
            /* [in] */ ISelectorProvider* provider,
            /* [in] */ IFileDescriptor* fd);

        CAR_INTERFACE_DECL()

        CARAPI Write(
            /* [in] */ IByteBuffer* buffer,
            /* [out] */ Int32* nWrite);

        CARAPI Write(
            /* [in] */ ArrayOf<IByteBuffer*>* buffers,
            /* [out] */ Int64* nWrite);

        CARAPI Write(
            /* [in] */ ArrayOf<IByteBuffer*>* buffers,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 length,
            /* [out] */ Int64* nWrite);

        CARAPI GetFD(
            /* [out] */ IFileDescriptor** descriptor);

    protected:
        CARAPI ImplCloseSelectableChannel();

        CARAPI ImplConfigureBlocking(
            /* [out] */ Boolean blocking);

    private:
        AutoPtr<IFileDescriptor> mFd;
        AutoPtr<ISocketChannel> mChannel;
    };

public:
    PipeImpl(
        /* [in] */ ISelectorProvider* selectorProvider);

    CARAPI Sink(
        /* [out] */ ISinkChannel** channel);

    CARAPI Source(
        /* [out] */ ISourceChannel** channel);

private:
    AutoPtr<PipeSinkChannel> mSink;
    AutoPtr<PipeSourceChannel> mSource;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_PIPEIMPL_H__
