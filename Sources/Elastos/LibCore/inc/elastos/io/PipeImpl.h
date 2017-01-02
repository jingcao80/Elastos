//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
