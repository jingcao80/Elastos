
#ifndef __ORG_APACHE_HARMONY_SECURITY_FORTRESS_CENGINE_H__
#define __ORG_APACHE_HARMONY_SECURITY_FORTRESS_CENGINE_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Org_Apache_Harmony_Security_Fortress_CEngine.h"
#include "core/Object.h"

using Elastos::Core::Object;
using Elastos::Security::IProvider;
using Elastos::Security::IProviderService;
using Elastos::Utility::IArrayList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Fortress {

CarClass(CEngine)
    , public Object
    , public IEngine
{
public:
    class SpiAndProvider
        : public Object
        , public ISpiAndProvider
    {
        friend class CEngine;

    public:
        CAR_INTERFACE_DECL()

        ~SpiAndProvider();

        CARAPI SetSpi(
            /* [in] */ IInterface* spi);

        CARAPI GetSpi(
            /* [out] */ IInterface** spi);

        CARAPI SetProvider(
            /* [in] */ IProvider* provider);

        CARAPI GetProvider(
            /* [out] */ IProvider** provider);

    private:
        SpiAndProvider(
            /* [in] */ IInterface* spi,
            /* [in] */ IProvider* provider)
            : mSpi(spi)
            , mProvider(provider)
        {}

    public:
        AutoPtr<IInterface> mSpi;
        AutoPtr<IProvider> mProvider;
    };

private:
    class ServiceCacheEntry : public Object
    {
    public:
        ServiceCacheEntry(
            /* [in] */ const String& algorithm,
            /* [in] */ Int32 cacheVersion,
            /* [in] */ IArrayList* services)
            : mAlgorithm(algorithm)
            , mCacheVersion(cacheVersion)
            , mServices(services)
        {}

    public:
        /** used to test for cache hit */
        String mAlgorithm;
        /** used to test for cache validity */
        Int32 mCacheVersion;
        /** cached result */
        AutoPtr<IArrayList> mServices;
    };

public:
    CARAPI constructor(
        /* [in] */ const String& serviceName);

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IInterface* param,
        /* [out] */ ISpiAndProvider** instance);

    /**
     * Finds the appropriate service implementation and returns an
     * {@code SpiAndProvider} instance containing a reference to SPI
     * and its {@code Provider}
     */
    CARAPI GetInstance(
        /* [in] */ IProviderService* service,
        /* [in] */ const String& param,
        /* [out] */ ISpiAndProvider** instance);

    /**
     * Returns a list of all possible matches for a given algorithm.
     */
    CARAPI GetServices(
        /* [in] */ const String& algorithm,
        /* [out] */ IArrayList** services);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [in] */ IInterface* param,
        /* [out] */ IInterface** instance);

public:
    /**
     * Access to package visible api in java.security
     */
    static AutoPtr<ISecurityAccess> sDoor;

private:
    /**
     * Service name such as Cipher or SSLContext
     */
    String mServiceName;

    /**
     * Previous result for getInstance(String, Object) optimization.
     * Only this non-Provider version of getInstance is optimized
     * since the the Provider version does not require an expensive
     * Services.getService call.
     */
    AutoPtr<ServiceCacheEntry> mServiceCache;
};

} // namespace Fortress
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_FORTRESS_CENGINE_H__
