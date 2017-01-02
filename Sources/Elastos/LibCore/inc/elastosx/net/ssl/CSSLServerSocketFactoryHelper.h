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

#ifndef __ELASTOSX_NET_SSL_CSSLSERVERSOCKETFACTORYHELPER_H__
#define __ELASTOSX_NET_SSL_CSSLSERVERSOCKETFACTORYHELPER_H__

#include "_Elastosx_Net_Ssl_CSSLServerSocketFactoryHelper.h"
#include "Singleton.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CSSLServerSocketFactoryHelper)
    , public Singleton
    , public ISSLServerSocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the default {@code SSLServerSocketFactory} instance. The default
     * implementation is defined by the security property
     * "ssl.ServerSocketFactory.provider".
     *
     * @return the default {@code SSLServerSocketFactory} instance.
     */
    CARAPI GetDefault(
        /* [out] */ IServerSocketFactory** factory);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLSERVERSOCKETFACTORYHELPER_H__
