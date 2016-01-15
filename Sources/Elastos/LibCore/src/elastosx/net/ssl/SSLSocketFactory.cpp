
#include "SSLSocketFactory.h"
#include "AutoLock.h"
#include <elastos/core/Thread.h>
#include "CDefaultSSLSocketFactory.h"
#include "SSLContext.h"

using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::IClassLoader;
using Elastosx::Net::Ssl::CDefaultSSLSocketFactory;

namespace Elastosx {
namespace Net {
namespace Ssl {

AutoPtr<ISocketFactory> SSLSocketFactory::sDefaultSocketFactory;
Int32 SSLSocketFactory::sLastCacheVersion = -1;
Object SSLSocketFactory::sLock;

CAR_INTERFACE_IMPL(SSLSocketFactory, SocketFactory, ISSLSocketFactory)

SSLSocketFactory::SSLSocketFactory()
{}

SSLSocketFactory::~SSLSocketFactory()
{}

ECode SSLSocketFactory::GetDefault(
        /* [out] */ ISocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    synchronized(sLock) {
        Int32 newCacheVersion;
        assert(0 && "TODO");
        //TODO: FAIL_RETURN(Services::GetCacheVersion(&newCacheVersion))
        if (sDefaultSocketFactory != NULL && sLastCacheVersion == newCacheVersion) {
            *factory = sDefaultSocketFactory;
            REFCOUNT_ADD(*factory);
            return NOERROR;
        }
        sLastCacheVersion = newCacheVersion;

        String newName;
        assert(0 && "TODO");
        //TODO: FAIL_RETURN(Security::GetProperty(String("ssl.SocketFactory.provider"), &newName))
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
    return NOERROR;
}


} // namespace Ssl
} // namespace Net
} // namespace Elastosx
