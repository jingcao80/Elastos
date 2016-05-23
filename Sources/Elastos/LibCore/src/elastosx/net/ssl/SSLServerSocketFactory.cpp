
#include "SSLServerSocketFactory.h"
#include "CDefaultSSLServerSocketFactory.h"
#include "AutoLock.h"
#include "SSLContext.h"
#include <elastos/core/Thread.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::IClassLoader;
using Elastosx::Net::Ssl::CDefaultSSLServerSocketFactory;

namespace Elastosx {
namespace Net {
namespace Ssl {

AutoPtr<IServerSocketFactory> SSLServerSocketFactory::sDefaultServerSocketFactory;
String SSLServerSocketFactory::sDefaultName;
Int32 SSLServerSocketFactory::sLastCacheVersion = -1;
Object SSLServerSocketFactory::sLock;

CAR_INTERFACE_IMPL(SSLServerSocketFactory, ServerSocketFactory, ISSLServerSocketFactory)

ECode SSLServerSocketFactory::GetDefault(
        /* [out] */ IServerSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    {    AutoLock syncLock(sLock);
        Int32 newCacheVersion;
        assert(0 && "TODO");
        //TODO: Services::GetCacheVersion(&newCacheVersion);
        if (sLastCacheVersion != newCacheVersion) {
            sDefaultServerSocketFactory = NULL;
            sDefaultName = String(NULL);
            sLastCacheVersion = newCacheVersion;
        }
        if (sDefaultServerSocketFactory != NULL) {
            *factory = sDefaultServerSocketFactory;
            REFCOUNT_ADD(*factory);
            return NOERROR;
        }
        if (sDefaultName.IsNull()) {
            assert(0 && "TODO");
            //TODO: Security::GetProperty(String("ssl.ServerSocketFactory.provider"), &sDefaultName);
            if (!sDefaultName.IsNull()) {
                AutoPtr<IThread> thread = Thread::GetCurrentThread();
                AutoPtr<IClassLoader> cl;
                FAIL_RETURN(thread->GetContextClassLoader((IClassLoader**)&cl))
                assert(0 && "TODO");
                // if (cl == NULL) {
                //     cl = ClassLoader::getSystemClassLoader();
                // }
                // try {
                //     final Class<?> ssfc = Class.forName(defaultName, true, cl);
                //     sDefaultServerSocketFactory = (ServerSocketFactory) ssfc.newInstance();
                // } catch (Exception e) {
                // }
            }
        }
        if (sDefaultServerSocketFactory == NULL) {
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
                sDefaultServerSocketFactory = IServerSocketFactory::Probe(_factory);
            }
        }
        if (sDefaultServerSocketFactory == NULL) {
            // Use internal dummy implementation
            CDefaultSSLServerSocketFactory::New(String("No ServerSocketFactory installed"), (IServerSocketFactory**)&sDefaultServerSocketFactory);
        }
        *factory = sDefaultServerSocketFactory;
        REFCOUNT_ADD(*factory);
        return NOERROR;
    }
    return NOERROR;
}


} // namespace Ssl
} // namespace Net
} // namespace Elastosx
