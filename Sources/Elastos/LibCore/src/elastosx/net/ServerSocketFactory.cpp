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

#include "ServerSocketFactory.h"
#include "CDefaultServerSocketFactory.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastosx::Net::CDefaultServerSocketFactory;

namespace Elastosx {
namespace Net {

AutoPtr<IServerSocketFactory> ServerSocketFactory::sDefaultFactory;
Object ServerSocketFactory::sLock;

CAR_INTERFACE_IMPL(ServerSocketFactory, Object, IServerSocketFactory)

ServerSocketFactory::ServerSocketFactory()
{}

ECode ServerSocketFactory::GetDefault(
    /* [out] */ IServerSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    {    AutoLock syncLock(sLock);
        if (sDefaultFactory == NULL) {
            CDefaultServerSocketFactory::New((IServerSocketFactory**)&sDefaultFactory);
        }
        *factory = sDefaultFactory;
        REFCOUNT_ADD(*factory)
    }
    return NOERROR;
}

ECode ServerSocketFactory::CreateServerSocket(
    /* [out] */ IServerSocket** sock)
{
    VALIDATE_NOT_NULL(sock)

    // follow RI's behavior
    // throw new SocketException("Unbound server sockets not implemented");
    return E_SOCKET_EXCEPTION;
}

} // namespace Net
} // namespace Elastosx

