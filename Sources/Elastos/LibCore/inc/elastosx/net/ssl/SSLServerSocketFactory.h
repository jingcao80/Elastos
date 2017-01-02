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

#ifndef __ELASTOSX_NET_SSL_SSLSERVERSOCKETFACTORY_H__
#define __ELASTOSX_NET_SSL_SSLSERVERSOCKETFACTORY_H__

#include "ServerSocketFactory.h"

using Elastosx::Net::IServerSocketFactory;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The factory for SSL server sockets.
 */
class SSLServerSocketFactory
    : public ServerSocketFactory
    , public ISSLServerSocketFactory
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the default {@code SSLServerSocketFactory} instance. The default
     * implementation is defined by the security property
     * "ssl.ServerSocketFactory.provider".
     *
     * @return the default {@code SSLServerSocketFactory} instance.
     */
    static CARAPI GetDefault(
        /* [out] */ IServerSocketFactory** factory);

private:
    // TODO EXPORT CONTROL

    // The default SSL socket factory
    static AutoPtr<IServerSocketFactory> sDefaultServerSocketFactory;

    static String sDefaultName;

    static Int32 sLastCacheVersion;

    static Object sLock;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_SSLSERVERSOCKETFACTORY_H__
