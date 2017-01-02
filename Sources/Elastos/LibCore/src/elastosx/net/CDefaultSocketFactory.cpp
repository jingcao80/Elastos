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

#include "CDefaultSocketFactory.h"
#include "CSocket.h"

using Elastos::Net::CSocket;

namespace Elastosx {
namespace Net {

CAR_OBJECT_IMPL(CDefaultSocketFactory)

ECode CDefaultSocketFactory::constructor()
{
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend((CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend(host, port, (CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localHost,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend(host, port, localHost, localPort, (CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* host,
    /* [in] */ Int32 port,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend(host, port, (CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultSocketFactory::CreateSocket(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int32 port,
    /* [in] */ IInetAddress* localAddress,
    /* [in] */ Int32 localPort,
    /* [out] */ ISocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CSocket> ss;
    FAIL_RETURN(CSocket::NewByFriend(address, port, localAddress, localPort, (CSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

} // namespace Net
} // namespace Elastosx

