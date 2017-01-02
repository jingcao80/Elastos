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

#include "SSLServerSocket.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(SSLServerSocket, ServerSocket, ISSLServerSocket)

SSLServerSocket::SSLServerSocket()
{}

ECode SSLServerSocket::constructor()
{
    return NOERROR;
}

ECode SSLServerSocket::constructor(
    /* [in] */ Int32 port)
{
    return ServerSocket::constructor(port);
}

ECode SSLServerSocket::constructor(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog)
{
    return ServerSocket::constructor(port, backlog);
}

ECode SSLServerSocket::constructor(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [in] */ IInetAddress* address)
{
    return ServerSocket::constructor(port, backlog, address);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
