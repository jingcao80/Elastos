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

#ifndef __ELASTOSX_NET_SSL_CSSLSOCKETFACTORYHELPER_H__
#define __ELASTOSX_NET_SSL_CSSLSOCKETFACTORYHELPER_H__

#include "_Elastosx_Net_Ssl_CSSLSocketFactoryHelper.h"
#include "Singleton.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CSSLSocketFactoryHelper)
    , public Singleton
    , public ISSLSocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the default {@code SSLSocketFactory} instance. The default is
     * defined by the security property {@code 'ssl.SocketFactory.provider'}.
     *
     * @return the default ssl socket factory instance.
     */
    ECode GetDefault(
        /* [out] */ ISocketFactory** factory);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLSOCKETFACTORYHELPER_H__
