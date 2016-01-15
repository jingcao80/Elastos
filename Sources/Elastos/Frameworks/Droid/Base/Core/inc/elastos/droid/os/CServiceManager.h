
#ifndef __ELASTOS_DROID_OS_CSERVICEMANAGER_H__
#define __ELASTOS_DROID_OS_CSERVICEMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Os_CServiceManager.h"
#include <elastos/core/Singleton.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CServiceManager)
    , public Singleton
    , public IServiceManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CServiceManager();

    virtual ~CServiceManager();

    CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ IInterface* service);

    CARAPI AddService(
        /* [in] */ const String& name,
        /* [in] */ IInterface* service,
        /* [in] */ Boolean allowIsolated);

    CARAPI GetService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** service);

    CARAPI CheckService(
        /* [in] */ const String& name,
        /* [out] */ IInterface** service);

    CARAPI ListServices(
        /* [out, callee] */ ArrayOf<String>** services);

    CARAPI SetPermissionController(
        /* [in] */ IPermissionController* controller);

    /**
     * This is only intended to be called when the process is first being brought
     * up and bound by the activity manager. There is only one thread in the process
     * at that time, so no locking is done.
     *
     * @param cache the cache of service references
     * @hide
     */
    CARAPI InitServiceCache(
        /* [in] */ IMap* services);

private:
    HashMap<String, AutoPtr<IInterface> > mServiceCache;
    Object mServiceCacheLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CSERVICEMANAGER_H__
