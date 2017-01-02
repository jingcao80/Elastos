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

#include "SSLSocketFactory.h"
#include "CDefaultSSLSocketFactory.h"
#include "AutoLock.h"
#include "Thread.h"
#include "CSecurity.h"
#include "SSLContext.h"
#include "org/apache/harmony/security/fortress/Services.h"

using Elastos::Core::AutoLock;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::IClassLoader;
using Elastos::Security::CSecurity;
using Elastos::Security::ISecurity;
using Elastosx::Net::Ssl::CDefaultSSLSocketFactory;
using Org::Apache::Harmony::Security::Fortress::Services;

namespace Elastosx {
namespace Net {
namespace Ssl {

INIT_PROI_6 AutoPtr<ISocketFactory> SSLSocketFactory::sDefaultSocketFactory;
Int32 SSLSocketFactory::sLastCacheVersion = -1;
INIT_PROI_6 Object SSLSocketFactory::sLock;

CAR_INTERFACE_IMPL(SSLSocketFactory, SocketFactory, ISSLSocketFactory)

SSLSocketFactory::SSLSocketFactory()
{}

SSLSocketFactory::~SSLSocketFactory()
{}

ECode SSLSocketFactory::GetDefault(
    /* [out] */ ISocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    {    AutoLock syncLock(sLock);
        assert(sLastCacheVersion == -1);
        Int32 newCacheVersion;
        FAIL_RETURN(Services::GetCacheVersion(&newCacheVersion));
        if (sDefaultSocketFactory != NULL && sLastCacheVersion == newCacheVersion) {
            *factory = sDefaultSocketFactory;
            REFCOUNT_ADD(*factory);
            return NOERROR;
        }
        sLastCacheVersion = newCacheVersion;

        String newName;
        AutoPtr<ISecurity> security;
        CSecurity::AcquireSingleton((ISecurity**)&security);
        FAIL_RETURN(security->GetProperty(String("ssl.SocketFactory.provider"), &newName));
        if (!newName.IsNull()) {
            /* The cache could have been invalidated, but the provider name didn't change. This
             * will be the most common state, so check for it early without resetting the default
             * SocketFactory.
             */
            if (sDefaultSocketFactory != NULL) {
                assert(0 && "TODO");
                if (0/*newName.Equals(sDefaultSocketFactory.getClass().getName())*/) {
                    *factory = sDefaultSocketFactory;
                    REFCOUNT_ADD(*factory);
                    return NOERROR;
                }
                else {
                    sDefaultSocketFactory = NULL;
                }
            }

            AutoPtr<IThread> thread = Thread::GetCurrentThread();
            AutoPtr<IClassLoader> cl;
            FAIL_RETURN(thread->GetContextClassLoader((IClassLoader**)&cl))
            assert(0 && "TODO");
            // if (cl == NULL) {
            //     cl = ClassLoader.getSystemClassLoader();
            // }
            // try {
            //     final Class<?> sfc = Class.forName(newName, true, cl);
            //     sDefaultSocketFactory = (SocketFactory) sfc.newInstance();
            // } catch (Exception e) {
            //     System.logW("Could not create " + newName + " with ClassLoader "
            //             + cl.toString() + ": " + e.getMessage());
            // }
        } else {
            sDefaultSocketFactory = NULL;
        }

        if (sDefaultSocketFactory == NULL) {
            AutoPtr<ISSLContext> context;
            ECode ec = NOERROR;
            //try {
                ec = SSLContext::GetDefault((ISSLContext**)&context);
            //} catch (NoSuchAlgorithmException e) {
                if (!SUCCEEDED(ec)) {
                    context = NULL;
                }
            //}
            if (context != NULL) {
                AutoPtr<ISSLServerSocketFactory> _factory;
                FAIL_RETURN(context->GetServerSocketFactory((ISSLServerSocketFactory**)&_factory))
                sDefaultSocketFactory = ISocketFactory::Probe(_factory);
            }
        }

        if (sDefaultSocketFactory == NULL) {
            // Use internal implementation
            CDefaultSSLSocketFactory::New(String("No SSLSocketFactory installed"), (ISocketFactory**)&sDefaultSocketFactory);
        }

        *factory = sDefaultSocketFactory;
        REFCOUNT_ADD(*factory);
        return NOERROR;
    }
}


} // namespace Ssl
} // namespace Net
} // namespace Elastosx
