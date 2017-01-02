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

#ifndef __ELASTOS_IO_NIOUTILS_H__
#define __ELASTOS_IO_NIOUTILS_H__

#include "Buffer.h"
#include "ByteBuffer.h"

using Elastos::IO::Channels::IFileChannel;

namespace Elastos {
namespace IO {
/*
 * @ hilde internal use only
 */
class NioUtils
{
public:
    static CARAPI FreeDirectBuffer(
        /* [in] */ IByteBuffer* buffer);

    static AutoPtr<IFileDescriptor> GetFD(
        /* [in] */ IFileChannel* fc);

    static AutoPtr<IFileChannel> NewFileChannel(
        /* [in] */ ICloseable *stream,
        /* [in] */ IFileDescriptor *fd,
        /* [in] */ Int32 mode);

    static AutoPtr<ArrayOf<Byte> > GetUnsafeArray(
        /* [in] */ IByteBuffer* b);

    static Int32 GetUnsafeArrayOffset(
        /* [in] */ IByteBuffer* b);
private:
    NioUtils() {}
};

} // namespace IO
} // namespace Elastos

#endif
