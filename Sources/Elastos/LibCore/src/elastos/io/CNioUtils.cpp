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

#include "CNioUtils.h"
#include "NioUtils.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CNioUtils, Singleton, INioUtils)

CAR_SINGLETON_IMPL(CNioUtils)

ECode CNioUtils::FreeDirectBuffer(
    /* [in] */ IByteBuffer* buffer)
{
    return NioUtils::FreeDirectBuffer(buffer);
}

ECode CNioUtils::GetFD(
    /* [in] */ IFileChannel* fc,
    /* [out] */ IFileDescriptor** outfd)
{
    VALIDATE_NOT_NULL(outfd)
    AutoPtr<IFileDescriptor> fd = NioUtils::GetFD(fc);
    *outfd = fd;
    REFCOUNT_ADD(*outfd)
    return NOERROR;
}

ECode CNioUtils::NewFileChannel(
    /* [in] */ ICloseable* ioObject,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Int32 mode,
    /* [out] */ IFileChannel** outchannel)
{
    VALIDATE_NOT_NULL(outchannel)

    AutoPtr<IFileChannel> res = NioUtils::NewFileChannel(ioObject, fd, mode);
    *outchannel = res;
    REFCOUNT_ADD(*outchannel)
    return NOERROR;
}

ECode CNioUtils::GetUnsafeArray(
    /* [in] */ IByteBuffer* b,
    /* [out, callee] */ ArrayOf<Byte>** outbyte)
{
    VALIDATE_NOT_NULL(outbyte)

    AutoPtr< ArrayOf<Byte> > res = NioUtils::GetUnsafeArray(b);
    *outbyte = res;
    REFCOUNT_ADD(*outbyte)
    return NOERROR;
}

ECode CNioUtils::GetUnsafeArrayOffset(
    /* [in] */ IByteBuffer* b,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = NioUtils::GetUnsafeArrayOffset(b);
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
