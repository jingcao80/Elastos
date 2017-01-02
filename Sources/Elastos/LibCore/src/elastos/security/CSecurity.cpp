//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "core/AutoLock.h"
#include "core/CString.h"
#include "security/CSecurity.h"
#include "utility/CProperties.h"
#include "utility/CHashMap.h"
#include "utility/CHashSet.h"
#include "utility/CArrayList.h"
#include "utility/logging/Logger.h"
#include "org/apache/harmony/security/fortress/CEngine.h"
#include "org/apache/harmony/security/fortress/Services.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::AutoLock;
using Elastos::Security::IProviderService;
using Elastos::Security::IProvider;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CProperties;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::Services;
using Org::Apache::Harmony::Security::Fortress::EIID_ISecurityAccess;

namespace Elastos {
namespace Security {

//----------------------------------------------------
// CSecurity::SecurityDoor
//----------------------------------------------------
CAR_INTERFACE_IMPL(CSecurity::SecurityDoor, Object, ISecurityAccess)

//ECode CSecurity::SecurityDoor::Aggregate(
//    /* [in] */ AggregateType type,
//    /* [in] */ IInterface* object)
//{
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CSecurity::SecurityDoor::GetDomain(
//    /* [out] */ IInterface** object)
//{
//    return E_NOT_IMPLEMENTED;
//}
//
//ECode CSecurity::SecurityDoor::GetClassID(
//    /* [out] */ ClassID* clsid)
//{
//    return E_NOT_IMPLEMENTED;
//}

ECode CSecurity::SecurityDoor::RenumProviders()
{
    return CSecurity::RenumProviders();
}

ECode CSecurity::SecurityDoor::GetAliases(
    /* [in] */ IProviderService* s,
    /* [out] */ IList** aliases)
{
    return s->GetAliases(aliases);
}

ECode CSecurity::SecurityDoor::GetService(
    /* [in] */ IProvider* p,
    /* [in] */ const String& type,
    /* [out] */ IProviderService** service)
{
    return p->GetService(type, service);
}


//----------------------------------------------------
// CSecurity::StaticInitializer
//----------------------------------------------------
CSecurity::StaticInitializer::StaticInitializer()
{
    // static initialization
    // - load security properties files
    // - load statically registered providers
    // - if no provider description file found then load default providers

    Logger::E("CSecurity", "TODO leliang file:%s", __FILE__);
    Boolean loaded = FALSE;
    // try {
    // TODO:
    // InputStream configStream = Security.class.getResourceAsStream("security.properties");
    // InputStream input = new BufferedInputStream(configStream);
    // secprops.load(input);
    // loaded = true;
    // configStream.close();
    // } catch (Exception ex) {
    //     System.logE("Could not load 'security.properties'", ex);
    // }
    if (!loaded) {
        CSecurity::RegisterDefaultProviders();
    }
    CEngine::sDoor = new SecurityDoor();
}


//----------------------------------------------------
// CSecurity
//----------------------------------------------------
INIT_PROI_5 const AutoPtr<IProperties> CSecurity::sSecprops = CSecurity::Init_sSecprops();
INIT_PROI_5 const CSecurity::StaticInitializer CSecurity::sInitializer;

CAR_INTERFACE_IMPL(CSecurity, Singleton, ISecurity)

CAR_SINGLETON_IMPL(CSecurity)

AutoPtr<IProperties> CSecurity::Init_sSecprops()
{
    AutoPtr<IProperties> prop;
    CProperties::New((IProperties**)&prop);
    return prop;
}

// Register default providers
void CSecurity::RegisterDefaultProviders()
{
    String old;
    sSecprops->SetProperty(String("security.provider.1"), String("Org.Conscrypt.COpenSSLProvider"), &old);
    //sSecprops->SetProperty(String("security.provider.1"), String("com.android.org.conscrypt.OpenSSLProvider"), &old);
    //sSecprops->SetProperty(String("security.provider.2"), String("com.android.org.bouncycastle.jce.provider.BouncyCastleProvider"), &old);
    //sSecprops->SetProperty(String("security.provider.3"), String("org.apache.harmony.security.provider.crypto.CryptoProvider"), &old);
    //sSecprops->SetProperty(String("security.provider.4"), String("com.android.org.conscrypt.JSSEProvider"), &old);
}

/**
 * Returns value for the specified algorithm with the specified name.
 *
 * @param algName
 *            the name of the algorithm.
 * @param propName
 *            the name of the property.
 * @return value of the property.
 * @deprecated Use {@link AlgorithmParameters} and {@link KeyFactory}
 *             instead.
 */
ECode CSecurity::GetAlgorithmProperty(
    /* [in] */ const String& algName,
    /* [in] */ const String& propName,
    /* [out] */ String* algProp)
{
    VALIDATE_NOT_NULL(algProp)
    if (algName.IsNull() || propName.IsNull()) {
        *algProp = NULL;
        return NOERROR;
    }
    String prop = String("Alg.") + propName + "." + algName;
    AutoPtr< ArrayOf<IProvider*> > providers;
    GetProviders((ArrayOf<IProvider*>**)&providers);
    for (Int32 i = 0; i < providers->GetLength(); ++i) {
        IProvider* provider = (*providers)[i];
        AutoPtr<IEnumeration> e;
        IProperties::Probe(provider)->PropertyNames((IEnumeration**)&e);
        Boolean hasMore;
        while (e->HasMoreElements(&hasMore), hasMore) {
            AutoPtr<IInterface> inter;
            e->GetNextElement((PInterface*)&inter);
            String propertyName;
            ICharSequence::Probe(inter)->ToString(&propertyName);
            if (propertyName.EqualsIgnoreCase(prop)) {
                return IProperties::Probe(provider)->GetProperty(propertyName, algProp);
            }
        }
    }
    *algProp = NULL;
    return NOERROR;
}

/**
 * Insert the given {@code Provider} at the specified {@code position}. The
 * positions define the preference order in which providers are searched for
 * requested algorithms.
 *
 * @param provider
 *            the provider to insert.
 * @param position
 *            the position (starting from 1).
 * @return the actual position or {@code -1} if the given {@code provider}
 *         was already in the list. The actual position may be different
 *         from the desired position.
 */
ECode CSecurity::InsertProviderAt(
    /* [in] */ IProvider* provider,
    /* [in] */ Int32 position,
    /* [out] */ Int32* pos)
{
    if (NULL == provider) {
        return E_NULL_POINTER_EXCEPTION;
    }

    // check that provider is not already
    // installed, else return -1; if (position <1) or (position > max
    // position) position = max position + 1; insert provider, shift up
    // one position for next providers; Note: The position is 1-based
    VALIDATE_NOT_NULL(pos)
    AutoLock lock(this);
    String name;
    provider->GetName(&name);
    AutoPtr<IProvider> pro;
    GetProvider(name, (IProvider**)&pro);
    if (pro != NULL) {
        *pos = -1;
        return NOERROR;
    }
    Int32 result;
    Services::InsertProviderAt(provider, position, &result);
    RenumProviders();
    *pos = result;
    return NOERROR;
}

/**
 * Adds the given {@code provider} to the collection of providers at the
 * next available position.
 *
 * @param provider
 *            the provider to be added.
 * @return the actual position or {@code -1} if the given {@code provider}
 *         was already in the list.
 */
ECode CSecurity::AddProvider(
    /* [in] */ IProvider* provider,
    /* [out] */ Int32* pos)
{
    return InsertProviderAt(provider, 0, pos);
}

/**
 * Removes the {@code Provider} with the specified name form the collection
 * of providers. If the the {@code Provider} with the specified name is
 * removed, all provider at a greater position are shifted down one
 * position.
 *
 * <p>Returns silently if {@code name} is {@code null} or no provider with the
 * specified name is installed.
 *
 * @param name
 *            the name of the provider to remove.
 */
ECode CSecurity::RemoveProvider(
    /* [in] */ const String& name)
{
    AutoLock lock(this);
    // It is not clear from spec.:
    // 1. if name is null, should we checkSecurityAccess or not?
    //    throw SecurityException or not?
    // 2. as 1 but provider is not installed
    // 3. behavior if name is empty string?

    AutoPtr<IProvider> p;
    if (name.IsNullOrEmpty()) {
        return NOERROR;
    }
    GetProvider(name, (IProvider**)&p);
    if (p == NULL) {
        return NOERROR;
    }
    Int32 number;
    p->GetProviderNumber(&number);
    Services::RemoveProvider(number);
    RenumProviders();
    p->SetProviderNumber(-1);
    return NOERROR;
}

/**
 * Returns an array containing all installed providers. The providers are
 * ordered according their preference order.
 *
 * @return an array containing all installed providers.
 */
ECode CSecurity::GetProviders(
    /* [out, callee] */ ArrayOf<IProvider*>** providers)
{
    VALIDATE_NOT_NULL(providers)
    AutoLock lock(this);
    AutoPtr<IArrayList> pros;
    Services::GetProviders((IArrayList**)&pros);
    return pros->ToArray((ArrayOf<IInterface*>**)providers);
}

ECode CSecurity::GetProvider(
    /* [in] */ const String& name,
    /* [out] */ IProvider** provider)
{
    VALIDATE_NOT_NULL(provider)
    AutoLock lock(this);
    return Services::GetProvider(name, provider);
}

/**
 * Returns the array of providers which meet the user supplied string
 * filter. The specified filter must be supplied in one of two formats:
 * <nl>
 * <li> CRYPTO_SERVICE_NAME.ALGORITHM_OR_TYPE
 * <p>
 * (for example: "MessageDigest.SHA")
 * <li> CRYPTO_SERVICE_NAME.ALGORITHM_OR_TYPE
 * ATTR_NAME:ATTR_VALUE
 * <p>
 * (for example: "Signature.MD2withRSA KeySize:512")
 * </nl>
 *
 * @param filter
 *            case-insensitive filter.
 * @return the providers which meet the user supplied string filter {@code
 *         filter}. A {@code null} value signifies that none of the
 *         installed providers meets the filter specification.
 * @throws InvalidParameterException
 *             if an unusable filter is supplied.
 * @throws NullPointerException
 *             if {@code filter} is {@code null}.
 */
ECode CSecurity::GetProviders(
    /* [in] */ const String& filter,
    /* [out, callee] */ ArrayOf<IProvider*>** providers)
{
    VALIDATE_NOT_NULL(providers)
    if (filter.IsNull()) {
        Logger::E("CSecurity", "filter == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (filter.GetLength() == 0) {
        return E_INVALID_PARAMETER_EXCEPTION;
    }
    AutoPtr<IMap> hm;
    CHashMap::New((IMap**)&hm);
    Int32 i = filter.IndexOf(':');
    if ((i == filter.GetLength() - 1) || (i == 0)) {
        return E_INVALID_PARAMETER_EXCEPTION;
    }
    AutoPtr<ICharSequence> key, value;
    AutoPtr<IInterface> old;
    if (i < 1) {
        CString::New(filter, (ICharSequence**)&key);
        CString::New(String(""), (ICharSequence**)&value);
        hm->Put(key, value, (PInterface*)&old);
    }
    else {
        AutoPtr<CString> swKey, swValue;
        CString::NewByFriend(filter, (CString**)&swKey);
        swKey->SubSequence(0, i, (ICharSequence**)&key);
        CString::NewByFriend(filter, (CString**)&swValue);
        swValue->SubSequence(i + 1, filter.GetLength(), (ICharSequence**)&value);
        hm->Put(key, value, (PInterface*)&old);
    }
    return GetProviders(hm, providers);
}

/**
 * Returns the array of providers which meet the user supplied set of
 * filters. The filter must be supplied in one of two formats:
 * <nl>
 * <li> CRYPTO_SERVICE_NAME.ALGORITHM_OR_TYPE
 * <p>
 * for example: "MessageDigest.SHA" The value associated with the key must
 * be an empty string. <li> CRYPTO_SERVICE_NAME.ALGORITHM_OR_TYPE
 * ATTR_NAME:ATTR_VALUE
 * <p>
 * for example: "Signature.MD2withRSA KeySize:512" where "KeySize:512" is
 * the value of the filter map entry.
 * </nl>
 *
 * @param filter
 *            case-insensitive filter.
 * @return the providers which meet the user supplied string filter {@code
 *         filter}. A {@code null} value signifies that none of the
 *         installed providers meets the filter specification.
 * @throws InvalidParameterException
 *             if an unusable filter is supplied.
 * @throws NullPointerException
 *             if {@code filter} is {@code null}.
 */
ECode CSecurity::GetProviders(
    /* [in] */ IMap* filter,
    /* [out, callee] */ ArrayOf<IProvider*>** providers)
{
    VALIDATE_NOT_NULL(providers)
    AutoLock lock(this);
    if (filter == NULL) {
        Logger::E("CSecurity", "filter == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean empty;
    if ((filter->IsEmpty(&empty), empty)) {
        *providers = NULL;
        return NOERROR;
    }

    AutoPtr<IArrayList> pros;
    Services::GetProviders((IArrayList**)&pros);
    AutoPtr<IArrayList> result;
    CArrayList::New(ICollection::Probe(pros), (IArrayList**)&result);
    AutoPtr<ISet> keys;
    filter->GetEntrySet((ISet**)&keys);
    AutoPtr<IIterator> it;
    keys->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> outface;
        it->GetNext((IInterface**)&outface);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);
        AutoPtr<IInterface> itkey, itvalue;
        entry->GetKey((IInterface**)&itkey);
        String key, val;
        ICharSequence::Probe(itkey)->ToString(&key);
        entry->GetValue((IInterface**)&itvalue);
        ICharSequence::Probe(itvalue)->ToString(&val);
        String attribute;
        Int32 i = key.IndexOf(' ');
        Int32 j = key.IndexOf('.');
        if (j == -1) {
            return E_INVALID_PARAMETER_EXCEPTION;
        }
        if (i == -1) {
            // <crypto_service>.<algorithm_or_type>
            if (val.GetLength() != 0) {
                return E_INVALID_PARAMETER_EXCEPTION;
            }
        }
        else {
            // <crypto_service>.<algorithm_or_type> <attribute_name>
            if (val.GetLength() == 0) {
                return E_INVALID_PARAMETER_EXCEPTION;
            }
            attribute = key.Substring(i + 1);
            if (attribute.Trim().GetLength() == 0) {
                return E_INVALID_PARAMETER_EXCEPTION;
            }
            key = key.Substring(0, i);
        }
        String serv = key.Substring(0, j);
        String alg = key.Substring(j + 1);
        if (serv.GetLength() == 0 || alg.GetLength() == 0) {
            return E_INVALID_PARAMETER_EXCEPTION;
        }
        FilterProviders(result, serv, alg, attribute, val);
    }
    Int32 size;
    result->GetSize(&size);
    if (size > 0) {
        return result->ToArray((ArrayOf<IInterface*>**)providers);
    }
    *providers = NULL;
    return NOERROR;
}

void CSecurity::FilterProviders(
    /* [in] */ IArrayList* providers,
    /* [in] */ const String& service,
    /* [in] */ const String& algorithm,
    /* [in] */ const String& attribute,
    /* [in] */ const String& attrValue)
{
    AutoPtr<IIterator> it;
    providers->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        Boolean result;
        if (IProvider::Probe(p)->ImplementsAlg(service, algorithm, attribute, attrValue, &result), !result) {
            it->Remove();
        }
    }
}


/**
 * Returns the value of the security property named by the argument.
 *
 * @param key
 *            the name of the requested security property.
 * @return the value of the security property.
 */
ECode CSecurity::GetProperty(
    /* [in] */ const String& key,
    /* [out] */ String* prop)
{
    VALIDATE_NOT_NULL(prop)
    if (key.IsNull()) {
        Logger::E("CSecurity", "key == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    String property;
    sSecprops->GetProperty(key, &property);
    if (!property.IsNull()) {
        property = property.Trim();
    }
    *prop = property;
    return NOERROR;
}

/**
 * Sets the value of the specified security property.
 */
ECode CSecurity::SetProperty(
    /* [in] */ const String& key,
    /* [in] */ const String& value)
{
    Services::SetNeedRefresh();
    String old;
    return sSecprops->SetProperty(key, value, &old);
}

/**
 * Returns a {@code Set} of all registered algorithms for the specified
 * cryptographic service. {@code "Signature"}, {@code "Cipher"} and {@code
 * "KeyStore"} are examples for such kind of services.
 *
 * @param serviceName
 *            the case-insensitive name of the service.
 * @return a {@code Set} of all registered algorithms for the specified
 *         cryptographic service, or an empty {@code Set} if {@code
 *         serviceName} is {@code null} or if no registered provider
 *         provides the requested service.
 */
ECode CSecurity::GetAlgorithms(
    /* [in] */ const String& serviceName,
    /* [out] */ ISet** algs)
{
    VALIDATE_NOT_NULL(algs)
    AutoPtr<ISet> result;
    CHashSet::New((ISet**)&result);
    // compatibility with RI
    if (serviceName.IsNull()) {
        *algs = result;
        REFCOUNT_ADD(*algs)
        return NOERROR;
    }
    AutoPtr<ArrayOf<IProvider*> > providers;
    GetProviders((ArrayOf<IProvider*>**)&providers);
    for (Int32 i = 0; i < providers->GetLength(); ++i) {
        AutoPtr<ISet> set;
        (*providers)[i]->GetServices((ISet**)&set);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Boolean next;
        while (it->HasNext(&next), next) {
            AutoPtr<IInterface> elm;
            it->GetNext((IInterface**)&elm);
            AutoPtr<IProviderService> service = IProviderService::Probe(elm);
            String type, algorithm;
            service->GetType(&type);
            if (type.EqualsIgnoreCase(serviceName)) {
                service->GetAlgorithm(&algorithm);
                AutoPtr<ICharSequence> cs;
                CString::New(algorithm, (ICharSequence**)&cs);
                Boolean ret;
                result->Add(cs, &ret);
            }
        }
    }
    *algs = result;
    REFCOUNT_ADD(*algs)
    return NOERROR;
}

/**
 *
 * Update sequence numbers of all providers.
 *
 */
ECode CSecurity::RenumProviders()
{
    AutoPtr<IArrayList> providers;
    FAIL_RETURN(Services::GetProviders((IArrayList**)&providers));
    Int32 size;
    providers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> provider;
        providers->Get(i, (IInterface**)&provider);
        IProvider::Probe(provider)->SetProviderNumber(i + 1);
    }
    return NOERROR;
}

}
}
