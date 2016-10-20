
#include "Elastos.CoreLibrary.Security.h"
#include "CEngine.h"
#include "Services.h"
#include "core/CString.h"
#include "utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Fortress {

//---------------------------------------------------
// CEngine::SpiAndProvider
//---------------------------------------------------
CAR_INTERFACE_IMPL(CEngine::SpiAndProvider, Object, ISpiAndProvider)

ECode CEngine::SpiAndProvider::SetSpi(
    /* [in] */ IInterface* spi)
{
    mSpi = spi;
    return NOERROR;
}

ECode CEngine::SpiAndProvider::GetSpi(
    /* [out] */ IInterface** spi)
{
    VALIDATE_NOT_NULL(spi);
    *spi = mSpi;
    REFCOUNT_ADD(*spi);
    return NOERROR;
}

ECode CEngine::SpiAndProvider::SetProvider(
    /* [in] */ IProvider* provider)
{
    mProvider = provider;
    return NOERROR;
}

ECode CEngine::SpiAndProvider::GetProvider(
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = mProvider;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}


//---------------------------------------------------
// CEngine
//---------------------------------------------------
INIT_PROI_4 AutoPtr<ISecurityAccess> CEngine::sDoor;

CAR_INTERFACE_IMPL(CEngine, Object, IEngine)

CAR_OBJECT_IMPL(CEngine)

ECode CEngine::constructor(
    /* [in] */ const String& serviceName)
{
    mServiceName = serviceName;
    return NOERROR;
}

ECode CEngine::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IInterface* param,
    /* [out] */ ISpiAndProvider** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;

    if (algorithm.IsNull()) {
        Logger::E("CEngine", "Null algorithm name");
        return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }
    AutoPtr<IArrayList> services;
    GetServices(algorithm, (IArrayList**)&services);
    if (services == NULL) {
        Logger::E("CEngine", "%s %s implementation not found", mServiceName.string(), algorithm.string());
        return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }
    AutoPtr<IInterface> item;
    services->Get(0, (IInterface**)&item);
    AutoPtr<IProviderService> service = IProviderService::Probe(item);
    AutoPtr<IInterface> obj;
    service->NewInstance(param, (IInterface**)&obj);
    AutoPtr<IProvider> pro;
    service->GetProvider((IProvider**)&pro);
    AutoPtr<SpiAndProvider> sap = new SpiAndProvider(obj, pro);
    *instance = sap;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode CEngine::GetInstance(
    /* [in] */ IProviderService* service,
    /* [in] */ const String& param,
    /* [out] */ ISpiAndProvider** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;

    AutoPtr<ICharSequence> strObj;
    CString::New(param, (ICharSequence**)&strObj);
    AutoPtr<IInterface> obj;
    service->NewInstance(strObj, (IInterface**)&obj);
    AutoPtr<IProvider> pro;
    service->GetProvider((IProvider**)&pro);
    AutoPtr<SpiAndProvider> sap =  new SpiAndProvider(obj, pro);
    *instance = sap;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode CEngine::GetServices(
    /* [in] */ const String& algorithm,
    /* [out] */ IArrayList** services)
{
    VALIDATE_NOT_NULL(services)
    *services = NULL;

    Int32 newCacheVersion;
    Services::GetCacheVersion(&newCacheVersion);
    AutoPtr<ServiceCacheEntry> cacheEntry = mServiceCache;
    String algoUC = algorithm.ToUpperCase();
    if (cacheEntry != NULL
            && cacheEntry->mAlgorithm.EqualsIgnoreCase(algoUC)
            && newCacheVersion == cacheEntry->mCacheVersion) {
        *services = cacheEntry->mServices;
        REFCOUNT_ADD(*services);
        return NOERROR;
    }
    String name = mServiceName + "." + algoUC;
    AutoPtr<IArrayList> allServices;
    Logger::E("leliang", "CEngine::GetServices, name:%s", name.string());
    Services::GetServices(name, (IArrayList**)&allServices);
    mServiceCache = new ServiceCacheEntry(algoUC, newCacheVersion, allServices);
    *services = allServices;
    REFCOUNT_ADD(*services);
    return NOERROR;
}

ECode CEngine::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [in] */ IInterface* param,
    /* [out] */ IInterface** instance)
{
    VALIDATE_NOT_NULL(instance)
    *instance = NULL;

    if (algorithm.IsNull()) {
        Logger::E("CEngine", "algorithm == null");
        return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }
    AutoPtr<IProviderService> service;
    provider->GetService(mServiceName, algorithm, (IProviderService**)&service);
    if (service == NULL) {
        Logger::E("CEngine", "%s %s implementation not found", mServiceName.string(), algorithm.string());
        return E_NO_SUCH_ALGORITHM_EXCEPTION;
    }
    return service->NewInstance(param, instance);
}

} // namespace Fortress
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
