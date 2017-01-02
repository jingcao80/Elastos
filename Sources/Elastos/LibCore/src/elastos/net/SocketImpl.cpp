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
#include "SocketImpl.h"
#include "StringUtils.h"

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL_2(SocketImpl, Object, ISocketImpl, ISocketOptions)

SocketImpl::SocketImpl()
    : mStreaming(TRUE)
    , mPort(0)
    , mLocalport(0)
{
}

ECode SocketImpl::GetFD(
    /* [out] */ IFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor)

    *fileDescriptor = mFd;
    REFCOUNT_ADD(*fileDescriptor)
    return NOERROR;
}

ECode SocketImpl::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<IInetAddress> netadd;
    GetInetAddress((IInetAddress**)&netadd);
    String outstr;
    IObject* o = (IObject*)netadd->Probe(EIID_IObject);
    o->ToString(&outstr);
    *str = String("Socket[address=") + outstr +
            String(",port=") + StringUtils::ToString(mPort) +
            String(",localPort=") + StringUtils::ToString(mLocalport) + String("]");
    return NOERROR;
}

ECode SocketImpl::GetFileDescriptor(
    /* [out] */ IFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor);

    *fileDescriptor = mFd;
    REFCOUNT_ADD(*fileDescriptor);
    return NOERROR;
}

ECode SocketImpl::GetInetAddress(
    /* [out] */ IInetAddress** netAddress)
{
    VALIDATE_NOT_NULL(netAddress);

    *netAddress = mAddress;
    REFCOUNT_ADD(*netAddress);
    return NOERROR;
}

ECode SocketImpl::GetLocalPort(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);

    *num = mLocalport;
    return NOERROR;
}

ECode SocketImpl::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);

    *port = mPort;
    return NOERROR;
}

ECode SocketImpl::ShutdownInput()
{
//    throw new IOException("Method has not been implemented");
    return E_IO_EXCEPTION;
}

ECode SocketImpl::ShutdownOutput()
{
//    throw new IOException("Method has not been implemented");
    return E_IO_EXCEPTION;
}

ECode SocketImpl::SupportsUrgentData(
    /* [out] */ Boolean* isSupports)
{
    VALIDATE_NOT_NULL(isSupports);

    *isSupports = FALSE;
    return NOERROR;
}

ECode SocketImpl::SetPerformancePreferences(
    /* [in] */ Int32 connectionTime,
    /* [in] */ Int32 latency,
    /* [in] */ Int32 bandwidth)
{
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
