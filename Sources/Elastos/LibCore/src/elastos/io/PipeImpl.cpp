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

#include "PipeImpl.h"
#include "SocketChannelImpl.h"
#include "IoUtils.h"
#include "CLibcore.h"
#include "OsConstants.h"

using Libcore::IO::IoUtils;
using Libcore::IO::CLibcore;
using Elastos::IO::Channels::ISocketChannel;
using Elastos::IO::Channels::EIID_ISinkChannel;
using Elastos::IO::Channels::EIID_ISourceChannel;
using Elastos::Droid::System::OsConstants;

namespace Elastos{
namespace IO{

//==========================================================
//       PipeImpl::PipeSourceChannel
//==========================================================
CAR_INTERFACE_IMPL(PipeImpl::PipeSourceChannel, Object, IFileDescriptorChannel)

PipeImpl::PipeSourceChannel::PipeSourceChannel()
{}

CARAPI PipeImpl::PipeSourceChannel::constructor(
    /* [in] */ ISelectorProvider* provider,
    /* [in] */ IFileDescriptor* fd)
{
    FAIL_RETURN(SourceChannel::constructor(provider))
    mFd = fd;
    mChannel = (ISocketChannel*) new SocketChannelImpl(provider, fd);
    return NOERROR;
}

ECode PipeImpl::PipeSourceChannel::ImplCloseSelectableChannel()
{
    return ICloseable::Probe(mChannel)->Close();
}

ECode PipeImpl::PipeSourceChannel::ImplConfigureBlocking(
    /* [in] */ Boolean blocking)
{
    AutoPtr<IFileDescriptor> descriptor;
    FAIL_RETURN(GetFD((IFileDescriptor**)&descriptor))
    return IoUtils::SetBlocking(descriptor, blocking);
}

ECode PipeImpl::PipeSourceChannel::Read(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* nRead)
{
    return mChannel->Read(buffer, nRead);
}

ECode PipeImpl::PipeSourceChannel::Read(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [out] */ Int64* nRead)
{
    return mChannel->Read(buffers, nRead);
}

ECode PipeImpl::PipeSourceChannel::Read(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int64* nRead)
{
    return mChannel->Read(buffers, offset, length, nRead);
}

ECode PipeImpl::PipeSourceChannel::GetFD(
    /* [out] */ IFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)

    *descriptor = mFd;
    REFCOUNT_ADD(*descriptor);
    return NOERROR;
}

//==========================================================
//       PipeImpl::PipeSinkChannel
//==========================================================
CAR_INTERFACE_IMPL(PipeImpl::PipeSinkChannel, Object, IFileDescriptorChannel)

PipeImpl::PipeSinkChannel::PipeSinkChannel()
{}

CARAPI PipeImpl::PipeSinkChannel::constructor(
    /* [in] */ ISelectorProvider* provider,
    /* [in] */ IFileDescriptor* fd)
{
    FAIL_RETURN(SinkChannel::constructor(provider))
    mFd = fd;
    mChannel = (ISocketChannel*) new SocketChannelImpl(provider, fd);
    return NOERROR;
}

ECode PipeImpl::PipeSinkChannel::ImplCloseSelectableChannel()
{
    return ICloseable::Probe(mChannel)->Close();
}

ECode PipeImpl::PipeSinkChannel::ImplConfigureBlocking(
    /* [in] */ Boolean blocking)
{
    AutoPtr<IFileDescriptor> desc;
    FAIL_RETURN(GetFD((IFileDescriptor**)&desc))
    return IoUtils::SetBlocking(desc, blocking);
}

ECode PipeImpl::PipeSinkChannel::Write(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ Int32* nWrite)
{
    return mChannel->Write(buffer, nWrite);
}

ECode PipeImpl::PipeSinkChannel::Write(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [out] */ Int64* nWrite)
{
    return mChannel->Write(buffers, nWrite);
}

ECode PipeImpl::PipeSinkChannel::Write(
    /* [in] */ ArrayOf<IByteBuffer*>* buffers,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int64* nWrite)
{
    return mChannel->Write(buffers, offset, length, nWrite);
}

ECode PipeImpl::PipeSinkChannel::GetFD(
    /* [out] */ IFileDescriptor** descriptor)
{
    VALIDATE_NOT_NULL(descriptor)
    *descriptor = mFd.Get();
    REFCOUNT_ADD(*descriptor);

    return NOERROR;
}

//==========================================================
//       PipeImpl
//==========================================================
PipeImpl::PipeImpl(
    /* [in] */ ISelectorProvider* selectorProvider)
{
    // try {
    AutoPtr<IFileDescriptor> fd1;
    CFileDescriptor::New((IFileDescriptor**)&fd1);
    AutoPtr<IFileDescriptor> fd2;
    CFileDescriptor::New((IFileDescriptor**)&fd2);
    CLibcore::sOs->Socketpair(OsConstants::_AF_UNIX, OsConstants::_SOCK_STREAM, 0, fd1, fd2);

    // It doesn't matter which file descriptor we use for which end;
    // they're guaranteed to be indistinguishable.
    mSink = new PipeSinkChannel();
    mSink->constructor(selectorProvider, fd1);
    mSource = new PipeSourceChannel();
    mSource->constructor(selectorProvider, fd2);
    // } catch (ErrnoException errnoException) {
    //     throw errnoException.rethrowAsIOException();
    // }
}

ECode PipeImpl::Sink(
    /* [out] */ ISinkChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    *channel = ISinkChannel::Probe(mSink);
    REFCOUNT_ADD(*channel)
    return NOERROR;
}

ECode PipeImpl::Source(
    /* [out] */ ISourceChannel** channel)
{
    VALIDATE_NOT_NULL(channel)

    *channel = ISourceChannel::Probe(mSource);
    REFCOUNT_ADD(*channel)
    return NOERROR;
}

} // namespace IO
} // namespace Elastoss
