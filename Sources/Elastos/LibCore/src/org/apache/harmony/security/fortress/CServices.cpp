
#include "CServices.h"
#include "CEngine.h"
#include "core/CString.h"
#include "core/ClassLoader.h"
#include "core/AutoLock.h"
#include "security/CSecurity.h"
#include "utility/CArrayList.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IClassLoader;
using Elastos::Core::ClassLoader;
using Elastos::Core::AutoLock;
using Elastos::Security::CSecurity;
using Elastos::Utility::CArrayList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Fortress {

HashMap< String, AutoPtr<IArrayList> > CServices::sServices;
Mutex CServices::sServicesLock;
AutoPtr<IProviderService> CServices::sCachedSecureRandomService;
Boolean CServices::sNeedRefresh;
Int32 CServices::sCacheVersion = 1;
const AutoPtr<IArrayList> CServices::sProviders = Init_sProviders();
HashMap< String, AutoPtr<IProvider> > CServices::sProvidersNames;
Boolean CServices::sIsInitialized = FALSE;

CAR_INTERFACE_IMPL(CServices, Singleton, IServices)

CAR_SINGLETON_IMPL(CServices)

CServices::CServices()
{
    Initialize();
}

AutoPtr<IArrayList> CServices::Init_sProviders()
{
    AutoPtr<CArrayList> list;
    CArrayList::NewByFriend((CArrayList**)&list);
    return list.Get();
}

void CServices::Initialize()
{
    if (sIsInitialized) return;

    String providerClassName;
    Int32 i = 1;
    AutoPtr<IClassLoader> cl = ClassLoader::GetSystemClassLoader();

    AutoPtr<CSecurity> security;
    CSecurity::AcquireSingletonByFriend((CSecurity**)&security);

    String key("");
    key.AppendFormat("security.provider.%d", i);
    while (security->GetProperty(key, &providerClassName), !providerClassName.IsNull()) {
        AutoPtr<IClassInfo> providerClass;
        ECode ec = cl->LoadClass(providerClassName, (IClassInfo**)&providerClass);
        if (SUCCEEDED(ec)) {
            AutoPtr<IInterface> obj;
            ec = providerClass->CreateObject((IInterface**)&obj);
            if (SUCCEEDED(ec)) {
                IProvider* p = IProvider::Probe(obj);
                assert(p != NULL);
                Boolean result;
                sProviders->Add(p, &result);
                String name;
                p->GetName(&name);
                sProvidersNames[name] = p;
                InitServiceInfo(p);
            }
        }
        key = "";
        key.AppendFormat("security.provider.%d", ++i);
    }
    CEngine::sDoor->RenumProviders();

    sIsInitialized = TRUE;
}

ECode CServices::GetProviders(
    /* [out] */ IArrayList** providers)
{
    VALIDATE_NOT_NULL(providers)
    AutoLock lock(this);
    *providers = sProviders;
    REFCOUNT_ADD(*providers);
    return NOERROR;
}

ECode CServices::GetProvider(
    /* [in] */ const String& name,
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoLock lock(this);

    if (name.IsNull()) {
        *provider = NULL;
        return NOERROR;
    }
    HashMap< String, AutoPtr<IProvider> >::Iterator it = sProvidersNames.Find(name);
    *provider = it != sProvidersNames.End() ? it->mSecond : NULL;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CServices::InsertProviderAt(
    /* [in] */ IProvider* provider,
    /* [in] */ Int32 position,
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    AutoLock lock(this);
    Int32 size;
    sProviders->GetSize(&size);
    if ((position < 1) || (position > size)) {
        position = size + 1;
    }
    sProviders->Add(position - 1, provider);
    String name;
    provider->GetName(&name);
    sProvidersNames[name] = provider;
    SetNeedRefresh();
    *pos = position;
    return NOERROR;
}

ECode CServices::RemoveProvider(
    /* [in] */ Int32 providerNumber)
{
    AutoLock lock(this);
    AutoPtr<IProvider> p;
    sProviders->Remove(providerNumber - 1, (IInterface**)&p);
    String name;
    p->GetName(&name);
    sProvidersNames.Erase(name);
    SetNeedRefresh();
    return NOERROR;
}

ECode CServices::InitServiceInfo(
    /* [in] */ IProvider* p)
{
    AutoLock lock(this);
    AutoPtr<ISet> set;
    p->GetServices((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        IProviderService* service = IProviderService::Probe(obj);
        String type;
        service->GetType(&type);
        if (sCachedSecureRandomService == NULL && type.Equals("SecureRandom")) {
            sCachedSecureRandomService = service;
        }
        String algo;
        service->GetAlgorithm(&algo);
        String key = type + "." + algo.ToUpperCase();
        AppendServiceLocked(key, service);
        AutoPtr<IList> aliases;
        CEngine::sDoor->GetAliases(service, (IList**)&aliases);
        AutoPtr<IIterator> aliasIt;
        aliases->GetIterator((IIterator**)&aliasIt);
        Boolean hasNextAlias;
        while(aliasIt->HasNext(&hasNextAlias), hasNextAlias) {
            String alias;
            AutoPtr<ICharSequence> aliasObj;
            aliasIt->GetNext((IInterface**)&aliasObj);
            aliasObj->ToString(&alias);
            key = type + "." + alias.ToUpperCase();
            AppendServiceLocked(key, service);
        }
    }
    return NOERROR;
}

void CServices::AppendServiceLocked(
    /* [in] */ const String& key,
    /* [in] */ IProviderService* service)
{
    AutoPtr<IArrayList> serviceList = sServices[key];
    if (serviceList == NULL) {
        CArrayList::New((IArrayList**)&serviceList);
        sServices[key] = serviceList;
    }
    Boolean result;
    serviceList->Add(service, &result);
}

ECode CServices::IsEmpty(
    /* [out] */ Boolean* empty)
{
    AutoLock lock(this);
    *empty = sServices.IsEmpty();
    return NOERROR;
}

ECode CServices::GetServices(
    /* [in] */ const String& key,
    /* [out] */ IArrayList** services)
{
    VALIDATE_NOT_NULL(services)
    AutoLock lock(this);
    HashMap< String, AutoPtr<IArrayList> >::Iterator it = sServices.Find(key);
    *services = it != sServices.End() ? it->mSecond : NULL;
    REFCOUNT_ADD(*services);
    return NOERROR;
}

ECode CServices::GetSecureRandomService(
    /* [out] */ IProviderService** service)
{
    VALIDATE_NOT_NULL(service)
    AutoLock lock(this);
    Int32 version;
    GetCacheVersion(&version); // used for side effect of updating cache if needed
    *service = sCachedSecureRandomService;
    REFCOUNT_ADD(*service);
    return NOERROR;
}

ECode CServices::SetNeedRefresh()
{
    AutoLock lock(this);
    sNeedRefresh = TRUE;
    return NOERROR;
}

ECode CServices::GetCacheVersion(
    /* [out] */ Int32* cacheVersion)
{
    VALIDATE_NOT_NULL(cacheVersion)
    AutoLock lock(this);
    if (sNeedRefresh) {
        sCacheVersion++;
        {
            Mutex::AutoLock lock(sServicesLock);
            sServices.Clear();
        }
        sCachedSecureRandomService = NULL;
        AutoPtr<IIterator> it;
        sProviders->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IProvider> p;
            it->GetNext((IInterface**)&p);
            InitServiceInfo(p);
        }
        sNeedRefresh = FALSE;
    }
    *cacheVersion = sCacheVersion;
    return NOERROR;
}

} // namespace Fortress
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
