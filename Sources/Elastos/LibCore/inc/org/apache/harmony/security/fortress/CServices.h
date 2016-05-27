
#ifndef __ORG_APACHE_HARMONY_SECURITY_FORTRESS_CSERVICES_H__
#define __ORG_APACHE_HARMONY_SECURITY_FORTRESS_CSERVICES_H__

#include "_Org_Apache_Harmony_Security_Fortress_CServices.h"
#include "core/Singleton.h"
#include "core/Mutex.h"
#include "utility/etl/HashMap.h"

using Elastos::Core::Singleton;
using Elastos::Core::Mutex;
using Elastos::Core::Singleton;
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

CarClass(CServices)
    , public Singleton
    , public IServices
{
public:
    CServices();

    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetProviders(
        /* [out] */ IArrayList** providers);

    CARAPI GetProvider(
        /* [in] */ const String& name,
        /* [out] */ IProvider** provider);

    CARAPI InsertProviderAt(
        /* [in] */ IProvider* provider,
        /* [in] */ Int32 position,
        /* [out] */ Int32* pos);

    CARAPI RemoveProvider(
        /* [in] */ Int32 providerNumber);

    CARAPI InitServiceInfo(
        /* [in] */ IProvider* p);

    CARAPI IsEmpty(
        /* [out] */ Boolean* empty);

    CARAPI GetServices(
        /* [in] */ const String& key,
        /* [out] */ IArrayList** services);

    CARAPI GetSecureRandomService(
        /* [out] */ IProviderService** service);

    CARAPI SetNeedRefresh();

    CARAPI GetCacheVersion(
        /* [out] */ Int32* cacheVersion);

private:
    /**
     * Add or append the service to the key.
     */
    static CARAPI_(void) AppendServiceLocked(
        /* [in] */ const String& key,
        /* [in] */ IProviderService* service);

    static CARAPI_(AutoPtr<IArrayList>) Init_sProviders();

    CARAPI_(void) Initialize();

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

    static Boolean sIsInitialized;
};

} // namespace Fortress
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_FORTRESS_CSERVICES_H__
