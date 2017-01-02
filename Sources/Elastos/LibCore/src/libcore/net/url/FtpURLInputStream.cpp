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

#include "Elastos.CoreLibrary.Net.h"
#include "FtpURLInputStream.h"
#include "IoUtils.h"
#include "AutoLock.h"

using Elastos::Core::AutoLock;
using Libcore::IO::IoUtils;

namespace Libcore {
namespace Net {
namespace Url {

CAR_INTERFACE_IMPL(FtpURLInputStream, InputStream, IFtpURLInputStream)

ECode FtpURLInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ ISocket* controlSocket)
{
    mIs = is;
    mControlSocket = controlSocket;
    return NOERROR;
}

ECode FtpURLInputStream::Available(
    /* [out] */ Int32* number)
{
    return mIs->Available(number);
}

ECode FtpURLInputStream::Close()
{
    IoUtils::CloseQuietly(ICloseable::Probe(mIs));
    IoUtils::CloseQuietly(mControlSocket);
    return NOERROR;
}

ECode FtpURLInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    AutoLock lock(mLock);
    return mIs->Mark(readLimit);;
}

ECode FtpURLInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    return mIs->IsMarkSupported(supported);
}

ECode FtpURLInputStream::Read(
    /* [out] */ Int32* value)
{
    return mIs->Read(value);
}

ECode FtpURLInputStream::ReadBytes(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    return mIs->Read(buffer, offset, length, number);
}

ECode FtpURLInputStream::Reset()
{
    AutoLock lock(mLock);
    return mIs->Reset();;
}

ECode FtpURLInputStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    return mIs->Skip(byteCount, number);
}

} // namespace Url
} // namespace Net
} // namespace Libcore
