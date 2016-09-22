
#ifndef __ORG_APACHE_HARMONY_SECURITY_FORTRESS_SERVICES_H__
#define __ORG_APACHE_HARMONY_SECURITY_FORTRESS_SERVICES_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "core/Mutex.h"
#include "elastos/coredef.h"
#include "Object.h"
#include "utility/etl/HashMap.h"

using Elastos::Core::Mutex;
using Elastos::Security::IProvider;
using Elastos::Security::IProviderService;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Fortress {

class Services
{
public:
    static CARAPI GetProviders(
        /* [out] */ IArrayList** providers);

    static CARAPI GetProvider(
        /* [in] */ const String& name,
        /* [out] */ IProvider** provider);

    static CARAPI InsertProviderAt(
        /* [in] */ IProvider* provider,
        /* [in] */ Int32 position,
        /* [out] */ Int32* pos);

    static CARAPI RemoveProvider(
        /* [in] */ Int32 providerNumber);

    static CARAPI InitServiceInfo(
        /* [in] */ IProvider* p);

    static CARAPI IsEmpty(
        /* [out] */ Boolean* empty);

    static CARAPI GetServices(
        /* [in] */ const String& key,
        /* [out] */ IArrayList** services);

    static CARAPI GetSecureRandomService(
        /* [out] */ IProviderService** service);

    static CARAPI SetNeedRefresh();

    static CARAPI GetCacheVersion(
        /* [out] */ Int32* cacheVersion);

private:
    Services();

    /**
     * Add or append the service to the key.
     */
    static CARAPI_(void) AppendServiceLocked(
        /* [in] */ const String& key,
        /* [in] */ IProviderService* service);

    static CARAPI_(AutoPtr<IArrayList>) Init_sProviders();

    static CARAPI_(AutoPtr<Object>) Initialize();

private:
    /**
     * The HashMap that contains information about preferred implementations for
     * all serviceName.algName in the registered providers.
     * Set the initial size to 600 so we don't grow to 1024 by default because
     * initialization adds a few entries more than the growth threshold.
     */
    static HashMap< String, AutoPtr<IArrayList> > sServices;
    static Mutex sServicesLock;

    /**
     * Save default SecureRandom service as well.
     * Avoids similar provider/services iteration in SecureRandom constructor.
     */
    static AutoPtr<IProviderService> sCachedSecureRandomService;

    /**
     * Need refresh flag.
     */
    static Boolean sNeedRefresh;

    /**
     * The cacheVersion is changed on every update of service
     * information. It is used by external callers to validate their
     * own caches of Service information.
     */
    static Int32 sCacheVersion;

    /**
     * Registered providers.
     */
    static const AutoPtr<IArrayList> sProviders;

    /**
     * Hash for quick provider access by name.
     */
    static HashMap< String, AutoPtr<IProvider> > sProvidersNames;

    static AutoPtr<Object> sIsInitialized;
    static Object LOCK;
};

} // namespace Fortress
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_FORTRESS_SERVICES_H__
