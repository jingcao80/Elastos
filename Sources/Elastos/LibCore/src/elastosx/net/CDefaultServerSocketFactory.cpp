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

#include "CDefaultServerSocketFactory.h"
#include "CServerSocket.h"

using Elastos::Net::CServerSocket;

namespace Elastosx {
namespace Net {

CAR_OBJECT_IMPL(CDefaultServerSocketFactory)

ECode CDefaultServerSocketFactory::CreateServerSocket(
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CServerSocket> ss;
    FAIL_RETURN(CServerSocket::NewByFriend((CServerSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultServerSocketFactory::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CServerSocket> ss;
    FAIL_RETURN(CServerSocket::NewByFriend(port, (CServerSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultServerSocketFactory::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CServerSocket> ss;
    FAIL_RETURN(CServerSocket::NewByFriend(port, backlog, (CServerSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

ECode CDefaultServerSocketFactory::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [in] */ IInetAddress* iAddress,
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)
    *sock = NULL;
    AutoPtr<CServerSocket> ss;
    FAIL_RETURN(CServerSocket::NewByFriend(port, backlog, iAddress, (CServerSocket**)&ss))
    *sock = ss.Get();
    REFCOUNT_ADD(*sock)
    return NOERROR;
}

} // namespace Net
} // namespace Elastosx

