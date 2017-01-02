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

#ifndef __ELASTOS_IO_CNIOUTILSHELPER_H__
#define __ELASTOS_IO_CNIOUTILSHELPER_H__

#include "_Elastos_IO_CNioUtils.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::IO::Channels::IFileChannel;

namespace Elastos {
namespace IO {

CarClass(CNioUtils)
    , public Singleton
    , public INioUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FreeDirectBuffer(
        /* [in] */ IByteBuffer* buffer);

    /**
     * Returns the int file descriptor from within the given FileChannel 'fc'.
     */
    CARAPI GetFD(
        /* [in] */ IFileChannel* fc,
        /* [out] */ IFileDescriptor** outfd);

    /**
     * Helps bridge between io and nio.
     */
    CARAPI NewFileChannel(
        /* [in] */ ICloseable* ioObject,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 mode,
        /* [out] */ IFileChannel** outchannel);

    /**
     * Exposes the array backing a non-direct ByteBuffer, even if the ByteBuffer is read-only.
     * Normally, attempting to access the array backing a read-only buffer throws.
     */
    CARAPI GetUnsafeArray(
        /* [in] */ IByteBuffer* b,
        /* [out, callee] */ ArrayOf<Byte>** outbyte);

    /**
     * Exposes the array offset for the array backing a non-direct ByteBuffer,
     * even if the ByteBuffer is read-only.
     */
    CARAPI GetUnsafeArrayOffset(
        /* [in] */ IByteBuffer* b,
        /* [out] */ Int32* value);
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CNIOUTILSHELPER_H__
