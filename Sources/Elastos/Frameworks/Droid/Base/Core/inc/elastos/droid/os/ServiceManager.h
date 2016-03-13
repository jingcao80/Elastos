#ifndef __ELASTOS_DROID_OS_SERVICEMANAGER_H__
#define __ELASTOS_DROID_OS_SERVICEMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Os {

class ECO_PUBLIC ServiceManager
{
public:
    /**
     * Returns a reference to a service with the given name.
     *
     * @param name the name of the service to get
     * @return a reference to the service, or <code>null</code> if the service doesn't exist
     */
    static CARAPI_(AutoPtr<IInterface>) GetService(
        /* [in] */ const String& name);

    /**
     * Place a new @a service called @a name into the service
     * manager.
     *
     * @param name the name of the new service
     * @param service the service object
     */
    static CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ IInterface* service);

    /**
     * Place a new @a service called @a name into the service
     * manager.
     *
     * @param name the name of the new service
     * @param service the service object
     * @param allowIsolated set to true to allow isolated sandboxed processes
     * to access this service
     */
    static CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ IInterface* service,
        /* [in] */ Boolean allowIsolated);

    /**
     * Retrieve an existing service called @a name from the
     * service manager.  Non-blocking.
     */
    static CARAPI_(AutoPtr<IInterface>) CheckService(
        /* [in] */ const String& name);

    /**
     * Return a list of all currently running services.
     */
    static CARAPI_(AutoPtr< ArrayOf<String> >) ListServices();

private:
    static CARAPI_(AutoPtr<IServiceManager>) GetIServiceManager();

private:
    static const String TAG;

    static AutoPtr<IServiceManager> sServiceManager;

private:
    ServiceManager();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_SERVICEMANAGER_H__
