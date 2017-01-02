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

#include "Elastos.CoreLibrary.IO.h"
#include "DatagramSocketImpl.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL_2(DatagramSocketImpl, Object, IDatagramSocketImpl, ISocketOptions)

DatagramSocketImpl::DatagramSocketImpl()
    : mLocalPort(-1)
{}

ECode DatagramSocketImpl::GetFileDescriptor(
    /* [out] */ IFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor);
    *fileDescriptor = mFd;
    REFCOUNT_ADD(*fileDescriptor);
    return NOERROR;
}

ECode DatagramSocketImpl::GetLocalPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);
    *port = mLocalPort;
    return NOERROR;
}

ECode DatagramSocketImpl::Connect(
    /* [in] */ IInetAddress* inetAddr,
    /* [in] */ Int32 port)
{
    // do nothing
    return NOERROR;
}

ECode DatagramSocketImpl::Disconnect()
{
    // do nothing
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
