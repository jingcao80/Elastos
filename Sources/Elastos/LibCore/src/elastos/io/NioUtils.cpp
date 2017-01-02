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

#include "NioUtils.h"
#include "DirectByteBuffer.h"
#include "MappedByteBuffer.h"
#include "FileChannelImpl.h"
#include "ByteArrayBuffer.h"

namespace Elastos {
namespace IO {

ECode NioUtils::FreeDirectBuffer(
    /* [in] */ IByteBuffer* buffer)
{
    IDirectByteBuffer* dbb = IDirectByteBuffer::Probe(buffer);
    if (dbb == NULL) {
        return NOERROR;
    }
    return ((DirectByteBuffer*) dbb)->Free();
}

AutoPtr<IFileDescriptor> NioUtils::GetFD(
    /* [in] */ IFileChannel* fc)
{
    AutoPtr<IFileDescriptor> outfd;
    IFileChannelImpl::Probe(fc)->GetFD((IFileDescriptor**)&outfd);
    return outfd;
}

AutoPtr<IFileChannel> NioUtils::NewFileChannel(
    /* [in] */ ICloseable *stream,
    /* [in] */ IFileDescriptor *fd,
    /* [in] */ Int32 mode)
{
    AutoPtr<IFileChannel> res = (IFileChannel *) new FileChannelImpl(stream, fd, mode);
    return res;
}

AutoPtr<ArrayOf<Byte> > NioUtils::GetUnsafeArray(
    /* [in] */ IByteBuffer* b)
{
    return ((ByteArrayBuffer*)b)->mBackingArray;
}

Int32 NioUtils::GetUnsafeArrayOffset(
    /* [in] */ IByteBuffer* b)
{
    return ((ByteArrayBuffer*)b)->mArrayOffset;
}

} // namespace IO
} // namespace Elastos
