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

#ifndef __ELASTOSX_NET_SSL_DEFAULTSSLSERVERSOCKETFACTORY_H__
#define __ELASTOSX_NET_SSL_DEFAULTSSLSERVERSOCKETFACTORY_H__

#include "SSLServerSocketFactory.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastosx::Net::IServerSocketFactory;
using Elastosx::Net::Ssl::ISSLServerSocketFactory;
using Elastosx::Net::Ssl::SSLServerSocketFactory;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * Default inoperative implementation of javax.net.ssl.SSLSocketFactory
 *
 */
class DefaultSSLServerSocketFactory
    : public SSLServerSocketFactory
{
public:
    CARAPI constructor(
        /* [in] */ const String& mes);

    CARAPI GetDefaultCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites);

    CARAPI GetSupportedCipherSuites(
        /* [out, callee] */ ArrayOf<String>** suites);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [out] */ IServerSocket** sock);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [out] */ IServerSocket** sock);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [in] */ IInetAddress* iAddress,
        /* [out] */ IServerSocket** sock);

private:
    /*const*/ String mErrMessage;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_DEFAULTSSLSERVERSOCKETFACTORY_H__
