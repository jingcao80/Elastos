
#include "Services.h"
#include "CEngine.h"
#include "core/CString.h"
#include "core/ClassLoader.h"
#include "core/AutoLock.h"
#include "core/CPathClassLoader.h"
#include "security/CSecurity.h"
#include "utility/CArrayList.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IClassLoader;
using Elastos::Core::ClassLoader;
using Elastos::Core::CPathClassLoader;
using Elastos::Core::AutoLock;
using Elastos::Security::CSecurity;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Fortress {

INIT_PROI_5 HashMap< String, AutoPtr<IArrayList> > Services::sServices;
INIT_PROI_5 Mutex Services::sServicesLock;
INIT_PROI_5 AutoPtr<IProviderService> Services::sCachedSecureRandomService;
Boolean Services::sNeedRefresh = FALSE;
Int32 Services::sCacheVersion = 1;
INIT_PROI_5 const AutoPtr<IArrayList> Services::sProviders = Init_sProviders();
INIT_PROI_5 HashMap< String, AutoPtr<IProvider> > Services::sProvidersNames;
INIT_PROI_6 AutoPtr<Object> Services::sIsInitialized;// = Initialize();
INIT_PROI_5 Object Services::LOCK;

Services::Services()
{}

AutoPtr<IArrayList> Services::Init_sProviders()
{
    AutoPtr<CArrayList> list;
    CArrayList::NewByFriend((CArrayList**)&list);
    return list.Get();
}

AutoPtr<Object> Services::Initialize()
{
    static Boolean initialized = FALSE;
    if (initialized) {
        return NULL;
    } else {
        initialized = TRUE;
    }

    String providerClassName;
    Int32 i = 1;
    AutoPtr<IClassLoader> cl = ClassLoader::GetSystemClassLoader();
    //begin leliang
    //String classPath("/system/lib/Org.Conscrypt.eco");
    //AutoPtr<IClassLoader> cl;
    //CPathClassLoader::New(classPath, NULL, (IClassLoader**)&cl);
    //end leliang
    AutoPtr<CSecurity> security;
    CSecurity::AcquireSingletonByFriend((CSecurity**)&security);
    String key("");
    key.AppendFormat("security.provider.%d", i);
    while (security->GetProperty(key, &providerClassName), !providerClassName.IsNull()) {
        Logger::E("leliang", "line:%d, func:%s\n, className:%s, key:%s", __LINE__, "Services::Initialize", providerClassName.string(), key.string());
        AutoPtr<IClassInfo> providerClass;
        ECode ec = cl->LoadClass(providerClassName, (IClassInfo**)&providerClass);
        //Logger::E("leliang", "LoadClass ec:0x%x", ec);
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
    return NULL;
}

ECode Services::GetProviders(
    /* [out] */ IArrayList** providers)
{
    VALIDATE_NOT_NULL(providers)
    AutoLock lock(LOCK);
    *providers = sProviders;
    REFCOUNT_ADD(*providers);
    return NOERROR;
}

ECode Services::GetProvider(
    /* [in] */ const String& name,
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoLock lock(LOCK);

    if (name.IsNull()) {
        *provider = NULL;
        return NOERROR;
    }
    HashMap< String, AutoPtr<IProvider> >::Iterator it = sProvidersNames.Find(name);
    if (it != sProvidersNames.End()) {
        *provider =  it->mSecond;
        REFCOUNT_ADD(*provider);
    }
    return NOERROR;
}

ECode Services::InsertProviderAt(
    /* [in] */ IProvider* provider,
    /* [in] */ Int32 position,
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos)
    AutoLock lock(LOCK);
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

ECode Services::RemoveProvider(
    /* [in] */ Int32 providerNumber)
{
    AutoLock lock(LOCK);
    AutoPtr<IInterface> value;
    sProviders->Remove(providerNumber - 1, (IInterface**)&value);
    AutoPtr<IProvider> p = IProvider::Probe(value);
    String name;
    p->GetName(&name);
    sProvidersNames.Erase(name);
    SetNeedRefresh();
    return NOERROR;
}

ECode Services::InitServiceInfo(
    /* [in] */ IProvider* p)
{
    AutoLock lock(LOCK);
    AutoPtr<ISet> set;
    p->GetServices((ISet**)&set);
    //leliang begin debug log
    String pName;
    p->GetName(&pName);
    Logger::E("leliang", "===Services::InitServiceInfo for provider: %s===", pName.string());
    //leliang end debug log
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
        Logger::E("leliang", "===Services::InitServiceInfo service: key:%s, service:%p===", key.string(), service);
        AppendServiceLocked(key, service);
        AutoPtr<IList> aliases;
        CEngine::sDoor->GetAliases(service, (IList**)&aliases);
        AutoPtr<IIterator> aliasIt;
        aliases->GetIterator((IIterator**)&aliasIt);
        Boolean hasNextAlias;
        while(aliasIt->HasNext(&hasNextAlias), hasNextAlias) {
            String alias;
            AutoPtr<IInterface> aliasObj;
            aliasIt->GetNext((IInterface**)&aliasObj);
            ICharSequence::Probe(aliasObj)->ToString(&alias);
            key = type + "." + alias.ToUpperCase();
            //Logger::E("leliang", "===Services::InitServiceInfo alias: %s===", key.string());
            AppendServiceLocked(key, service);
        }
    }
    return NOERROR;
}

void Services::AppendServiceLocked(
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

ECode Services::IsEmpty(
    /* [out] */ Boolean* empty)
{
    AutoLock lock(LOCK);
    *empty = sServices.IsEmpty();
    return NOERROR;
}

ECode Services::GetServices(
    /* [in] */ const String& key,
    /* [out] */ IArrayList** services)
{
    VALIDATE_NOT_NULL(services)
    *services = NULL;
    AutoLock lock(LOCK);
    HashMap< String, AutoPtr<IArrayList> >::Iterator it = sServices.Find(key);
    if (it != sServices.End()) {
        *services = it->mSecond;
        //test begin leling
        //Int32 size;
        //(*services)->GetSize(&size);
        //Logger::E("leliang", "Services::GetServices, find key:%s, size:%d", key.string(), size);
        //test begin end
        REFCOUNT_ADD(*services);
    }
    return NOERROR;
}

ECode Services::GetSecureRandomService(
    /* [out] */ IProviderService** service)
{
    VALIDATE_NOT_NULL(service)
    AutoLock lock(LOCK);
    Int32 version;
    GetCacheVersion(&version); // used for side effect of updating cache if needed
    *service = sCachedSecureRandomService;
    REFCOUNT_ADD(*service);
    return NOERROR;
}

ECode Services::SetNeedRefresh()
{
    AutoLock lock(LOCK);
    sNeedRefresh = TRUE;
    return NOERROR;
}

ECode Services::GetCacheVersion(
    /* [out] */ Int32* cacheVersion)
{
    VALIDATE_NOT_NULL(cacheVersion)
    AutoLock lock(LOCK);
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
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            InitServiceInfo(IProvider::Probe(p));
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
