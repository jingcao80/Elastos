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

#ifndef __ELASTOS_DROID_NET_CSSLSESSIONCACHEHELPER_H__
#define __ELASTOS_DROID_NET_CSSLSESSIONCACHEHELPER_H__

#include "_Elastos_Droid_Net_CSSLSessionCacheHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

using Elastosx::Net::Ssl::ISSLContext;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * File-based cache of established SSL sessions.  When re-establishing a
 * connection to the same server, using an SSL session cache can save some time,
 * power, and bandwidth by skipping directly to an encrypted stream.
 * This is a persistent cache which can span executions of the application.
 *
 * @see SSLCertificateSocketFactory
 */
CarClass(CSSLSessionCacheHelper)
    , public Singleton
    , public ISSLSessionCacheHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Installs a {@link SSLSessionCache} on a {@link SSLContext}. The cache will
     * be used on all socket factories created by this context (including factories
     * created before this call).
     *
     * @param cache the cache instance to install, or {@code null} to uninstall any
     *         existing cache.
     * @param context the context to install it on.
     * @throws IllegalArgumentException if the context does not support a session
     *         cache.
     *
     * @hide candidate for public API
     */
    CARAPI Install(
        /* [in] */ ISSLSessionCache* cache,
        /* [in] */ ISSLContext* context);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CSSLSESSIONCACHEHELPER_H__
