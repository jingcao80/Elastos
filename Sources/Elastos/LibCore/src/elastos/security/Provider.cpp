
#include "Elastos.CoreLibrary.IO.h"
#include "coredef.h"
#include "Provider.h"
#include "CProviderService.h"
#include "core/AutoLock.h"
#include "core/StringUtils.h"
#include "core/CString.h"
#include "utility/CArrayList.h"
#include "utility/CCollections.h"
#include "utility/CHashMap.h"
#include "utility/CLinkedHashMap.h"
#include "utility/CLinkedHashSet.h"
#include "utility/CProperties.h"
#include "utility/logging/Logger.h"
#include "org/apache/harmony/security/fortress/CServices.h"

using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::IString;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IProperties;
using Elastos::Utility::CProperties;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CLinkedHashMap;
using Elastos::Utility::CLinkedHashSet;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Harmony::Security::Fortress::IServices;
using Org::Apache::Harmony::Security::Fortress::CServices;

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(Provider, Properties, IProvider)

ECode Provider::constructor(
    /* [in] */ const String& name,
    /* [in] */ Double version,
    /* [in] */ const String& info)
{
    mName = name;
    mVersion = version;
    mInfo = info;
    mVersionString = StringUtils::ToString(version);
    PutProviderInfo();
    return NOERROR;
}

ECode Provider::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode Provider::GetVersion(
    /* [out] */ Double* version)
{
    VALIDATE_NOT_NULL(version);
    *version = mVersion;
    return NOERROR;
}

ECode Provider::GetInfo(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = mInfo;
    return NOERROR;
}

ECode Provider::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = mName + String(" version ") + mVersion;
    return NOERROR;
}

ECode Provider::Clear()
{
    AutoLock lock(this);

    Properties::Clear();
    if (mServiceTable != NULL) {
        mServiceTable->Clear();
    }
    if (mPropertyServiceTable != NULL) {
        mPropertyServiceTable->Clear();
    }
    if (mAliasTable != NULL) {
        mAliasTable->Clear();
    }
    if (mPropertyAliasTable != NULL) {
        mPropertyAliasTable->Clear();
    }
    mChangedProperties = NULL;
    PutProviderInfo();
    if (mProviderNumber != -1) {
        // if registered then refresh Services
        AutoPtr<IServices> services;
        CServices::AcquireSingleton((IServices**)&services);
        services->SetNeedRefresh();
    }
    ServicesChanged();

    return NOERROR;
}

ECode Provider::Load(
    /* [in] */ IInputStream* inStream)
{
    AutoLock lock(this);

    AutoPtr<IProperties> tmp;
    CProperties::New((IProperties**)&tmp);
    tmp->Load(inStream);
    MyPutAll(IMap::Probe(tmp));
    return NOERROR;
}

ECode Provider::PutAll(
    /* [in] */ IMap* t)
{
    AutoLock lock(this);

    MyPutAll(t);
    return NOERROR;
}

void Provider::MyPutAll(
    /* [in] */ IMap* t)
{
    if (mChangedProperties == NULL) {
        CLinkedHashMap::New((ILinkedHashMap**)&mChangedProperties);
    }
    AutoPtr<ISet> entries;
    t->GetEntrySet((ISet**)&entries);
    AutoPtr<IIterator> it;
    entries->GetIterator((IIterator**)&it);
    AutoPtr<IInterface> key, value;
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        key = NULL;
        entry->GetKey((IInterface**)&key);
        if (IString::Probe(key)) {
            String str;
            ICharSequence::Probe(key)->ToString(&str);
            if (str.StartWith("Provider.")) {
                // Provider service type is reserved
                continue;
            }
        }
        value = NULL;
        entry->GetValue((IInterface**)&value);
        Properties::Put(key, value);
        obj = NULL;
        mChangedProperties->Remove(key, (IInterface**)&obj);
        if (obj == NULL) {
            RemoveFromPropertyServiceTable(key);
        }
        mChangedProperties->Put(key, value);
    }
    if (mProviderNumber != -1) {
        // if registered then refresh Services
        AutoPtr<IServices> services;
        CServices::AcquireSingleton((IServices**)&services);
        services->SetNeedRefresh();
    }
}

ECode Provider::GetEntrySet(
    /* [out] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);
    AutoLock lock(this);

    AutoPtr<ISet> origEntries;
    Properties::GetEntrySet((ISet**)&origEntries);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->UnmodifiableSet(origEntries, entries);
}

ECode Provider::GetKeySet(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);

    AutoPtr<ISet> origKeys;
    Properties::GetKeySet((ISet**)&origKeys);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->UnmodifiableSet(origKeys, keys);
}

ECode Provider::GetValues(
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);

    AutoPtr<ICollection> origValues;
    Properties::GetValues((ICollection**)&origValues);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->UnmodifiableCollection(origValues, values);
}

ECode Provider::Put(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** oldValue)
{
    VALIDATE_NOT_NULL(oldValue);
    *oldValue = NULL;

    if (IString::Probe(key)) {
        String str;
        ICharSequence::Probe(key)->ToString(&str);
        if (str.StartWith("Provider.")) {
            // Provider service type is reserved
            return NOERROR;
        }
    }
    if (mProviderNumber != -1) {
        // if registered then refresh Services
        AutoPtr<IServices> services;
        CServices::AcquireSingleton((IServices**)&services);
        services->SetNeedRefresh();
    }
    AutoPtr<IInterface> obj;
    if (mChangedProperties != NULL && (mChangedProperties->Remove(key, (IInterface**)&obj), obj == NULL)) {
        RemoveFromPropertyServiceTable(key);
    }
    if (mChangedProperties == NULL) {
        CLinkedHashMap::New((ILinkedHashMap**)&mChangedProperties);
    }
    mChangedProperties->Put(key, value);
    return Properties::Put(key, value, oldValue);
}

ECode Provider::Remove(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    AutoLock lock(this);

    if (IString::Probe(key)) {
        String str;
        ICharSequence::Probe(key)->ToString(&str);
        if (str.StartWith("Provider.")) {
            // Provider service type is reserved
            return NOERROR;
        }
    }
    if (mProviderNumber != -1) {
        // if registered then refresh Services
        AutoPtr<IServices> services;
        CServices::AcquireSingleton((IServices**)&services);
        services->SetNeedRefresh();
    }
    AutoPtr<IInterface> obj;
    if (mChangedProperties != NULL && (mChangedProperties->Remove(key, (IInterface**)&obj), obj == NULL)) {
        RemoveFromPropertyServiceTable(key);
        Int32 size;
        if (mChangedProperties->GetSize(&size), size == 0) {
            mChangedProperties = NULL;
        }
    }
    return Properties::Remove(key, value);
}

ECode Provider::ImplementsAlg(
    /* [in] */ const String& serv,
    /* [in] */ const String& alg,
    /* [in] */ const String& attribute,
    /* [in] */ const String& val,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    String servAlg = serv + String(".") + alg;
    String prop = GetPropertyIgnoreCase(servAlg);
    if (prop.IsNull()) {
        String newAlg = GetPropertyIgnoreCase(String("Alg.Alias.") + servAlg);
        if (!newAlg.IsNull()) {
            servAlg = serv + String(".") + newAlg;
            prop = GetPropertyIgnoreCase(servAlg);
        }
    }
    if (!prop.IsNull()) {
        if (attribute.IsNull()) {
            *result = TRUE;
            return NOERROR;
        }
        *result = CheckAttribute(servAlg, attribute, val);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

Boolean Provider::CheckAttribute(
    /* [in] */ const String& servAlg,
    /* [in] */ const String& attribute,
    /* [in] */ const String& val)
{
    String attributeValue = GetPropertyIgnoreCase(servAlg + String(" ")  + attribute);
    if (!attributeValue.IsNull()) {
        if (attribute.EqualsIgnoreCase("KeySize")) {
            if (StringUtils::ParseInt32(attributeValue) >= StringUtils::ParseInt32(val)) {
                return TRUE;
            }
        }
        else { // other attributes
            if (attributeValue.EqualsIgnoreCase(val)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

ECode Provider::SetProviderNumber(
    /* [in] */ Int32 n)
{
    mProviderNumber = n;
    return NOERROR;
}

ECode Provider::GetProviderNumber(
    /* [out] */ Int32* n)
{
    VALIDATE_NOT_NULL(n);
    *n = mProviderNumber;
    return NOERROR;
}

ECode Provider::GetService(
    /* [in] */ const String& type,
    /* [out] */ IProviderService** retService)
{
    VALIDATE_NOT_NULL(retService)
    AutoLock lock(this);

    UpdatePropertyServiceTable();
    if (mLastServicesByType != NULL && type.Equals(mLastType)) {
        *retService = mLastServicesByType;
        REFCOUNT_ADD(*retService);
        return NOERROR;
    }
    AutoPtr<ISet> services;
    GetServices((ISet**)&services);
    AutoPtr<IIterator> it;
    services->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IProviderService> service = IProviderService::Probe(obj);
        String srvType;
        service->GetType(&srvType);
        if (type.Equals(srvType)) {
            mLastType = type;
            mLastServicesByType = service;
            *retService = service;
            REFCOUNT_ADD(*retService);
            return NOERROR;
        }
    }
    *retService = NULL;
    return NOERROR;
}

ECode Provider::GetService(
    /* [in] */ const String& type,
    /* [in] */ const String& algorithm,
    /* [out] */ IProviderService** service)
{
    VALIDATE_NOT_NULL(service);
    *service = NULL;
    AutoLock lock(this);

    if (type.IsNull()) {
        Logger::E("Provider", "type == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (algorithm.IsNull()) {
        Logger::E("Provider", "algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (type.Equals(mLastServiceName) && algorithm.EqualsIgnoreCase(mLastAlgorithm)) {
        *service = mReturnedService;
        return NOERROR;
    }

    String key = Key(type, algorithm);
    AutoPtr<ICharSequence> keyObj;
    CString::New(key, (ICharSequence**)&keyObj);
    AutoPtr<IInterface> o;
    if (mServiceTable != NULL) {
        mServiceTable->Get(keyObj, (IInterface**)&o);
    }
    if (o == NULL && mAliasTable != NULL) {
        mAliasTable->Get(keyObj, (IInterface**)&o);
    }
    if (o == NULL) {
        UpdatePropertyServiceTable();
    }
    if (o == NULL && mPropertyServiceTable != NULL) {
        mPropertyServiceTable->Get(keyObj, (IInterface**)&o);
    }
    if (o == NULL && mPropertyAliasTable != NULL) {
        mPropertyAliasTable->Get(keyObj, (IInterface**)&o);
    }

    if (o != NULL) {
        mLastServiceName = type;
        mLastAlgorithm = algorithm;
        mReturnedService = IProviderService::Probe(o);
        *service = mReturnedService;
        REFCOUNT_ADD(*service);
        return NOERROR;
    }
    *service = NULL;
    return NOERROR;
}

ECode Provider::GetServices(
    /* [out] */ ISet** services)
{
    VALIDATE_NOT_NULL(services);
    *services = NULL;

    UpdatePropertyServiceTable();
    if (mLastServicesSet != NULL) {
        *services = mLastServicesSet;
        REFCOUNT_ADD(*services);
        return NOERROR;
    }
    if (mServiceTable != NULL) {
        AutoPtr<ICollection> values;
        mServiceTable->GetValues((ICollection**)&values);
        CLinkedHashSet::New(values, (ISet**)&mLastServicesSet);
    }
    else {
        CLinkedHashSet::New((ISet**)&mLastServicesSet);
    }
    if (mPropertyServiceTable != NULL) {
        AutoPtr<ICollection> values;
        mPropertyServiceTable->GetValues((ICollection**)&values);
        mLastServicesSet->AddAll(values);
    }
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->UnmodifiableSet(mLastServicesSet, services);
    mLastServicesSet = *services;
    return NOERROR;
}

ECode Provider::PutService(
    /* [in] */ IProviderService* s)
{
    AutoLock lock(this);

    if (s == NULL) {
        Logger::E("Provider", "s == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    String type;
    s->GetType(&type);
    if (String("Provider").Equals(type)) { // Provider service type cannot be added
        return NOERROR;
    }
    ServicesChanged();
    if (mServiceTable == NULL) {
        CLinkedHashMap::New(128, (ILinkedHashMap**)&mServiceTable);
    }
    String algo;
    s->GetAlgorithm(&algo);
    AutoPtr<ICharSequence> strObj;
    CString::New(Key(type, algo), (ICharSequence**)&strObj);
    mServiceTable->Put(strObj, s);
    AutoPtr<IList> aliases;
    s->GetAliases((IList**)&aliases);
    if (aliases != NULL) {
        if (mAliasTable == NULL) {
            CLinkedHashMap::New(256, (ILinkedHashMap**)&mAliasTable);
        }
        AutoPtr<IIterator> it;
        aliases->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> strObj;
            it->GetNext((IInterface**)&strObj);
            String alias;
            ICharSequence::Probe(strObj)->ToString(&alias);
            AutoPtr<ICharSequence> keyObj;
            CString::New(Key(type, alias), (ICharSequence**)&keyObj);
            mAliasTable->Put(keyObj, s);
        }
    }
    ServiceInfoToProperties(s);
    return NOERROR;
}

ECode Provider::RemoveService(
    /* [in] */ IProviderService* s)
{
    AutoLock lock(this);

    if (s == NULL) {
        Logger::E("Provider", "s == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    ServicesChanged();
    String type, algo;
    s->GetType(&type);
    s->GetAlgorithm(&algo);
    if (mServiceTable != NULL) {
        AutoPtr<ICharSequence> strObj;
        CString::New(Key(type, algo), (ICharSequence**)&strObj);
        mServiceTable->Remove(strObj);
    }
    AutoPtr<IList> aliases;
    if (mAliasTable != NULL && (s->GetAliases((IList**)&aliases), aliases != NULL)) {
        AutoPtr<IIterator> it;
        aliases->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            String alias;
            ICharSequence::Probe(obj)->ToString(&alias);
            AutoPtr<ICharSequence> strObj;
            CString::New(Key(type, alias), (ICharSequence**)&strObj);
            mAliasTable->Remove(strObj);
        }
    }
    ServiceInfoFromProperties(s);
    return NOERROR;
}

void Provider::ServiceInfoToProperties(
    /* [in] */ IProviderService* s)
{
    String type, algo, className;
    s->GetType(&type);
    s->GetAlgorithm(&algo);
    s->GetClassName(&className);
    AutoPtr<ICharSequence> keyObj, valueObj;
    CString::New(type + String(".") + algo, (ICharSequence**)&keyObj);
    CString::New(className, (ICharSequence**)&valueObj);
    Properties::Put(keyObj, valueObj);
    AutoPtr<IList> aliases;
    s->GetAliases((IList**)&aliases);
    if (aliases != NULL) {
        AutoPtr<IIterator> it;
        aliases->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> strObj;
            it->GetNext((IInterface**)&strObj);
            String str;
            ICharSequence::Probe(strObj)->ToString(&str);
            keyObj = NULL;
            valueObj = NULL;
            CString::New(String("Alg.Alias.") + type + String(".") + str, (ICharSequence**)&keyObj);
            CString::New(algo, (ICharSequence**)&valueObj);
            Properties::Put(keyObj, valueObj);
        }
    }
    AutoPtr<IMap> attributes;
    s->GetAttributes((IMap**)&attributes);
    if (attributes != NULL) {
        AutoPtr<ISet> entrySet;
        attributes->GetKeySet((ISet**)&entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
            AutoPtr<IInterface> entryKey, entryValue;
            entry->GetKey((IInterface**)&entryKey);
            entry->GetValue((IInterface**)&entryValue);
            String entryKeyStr;
            ICharSequence::Probe(entryKey)->ToString(&entryKeyStr);
            keyObj = NULL;
            CString::New(type + String(".") + algo + String(" ") + entryKeyStr, (ICharSequence**)&keyObj);
            Properties::Put(keyObj, ICharSequence::Probe(entryValue));
        }
    }
    if (mProviderNumber != -1) {
        // if registered then refresh Services
        AutoPtr<IServices> services;
        CServices::AcquireSingleton((IServices**)&services);
        services->SetNeedRefresh();
    }
}

void Provider::ServiceInfoFromProperties(
    /* [in] */ IProviderService* s)
{
    String type, algo;
    s->GetType(&type);
    s->GetAlgorithm(&algo);
    AutoPtr<ICharSequence> keyObj;
    CString::New(type + String(".") + algo, (ICharSequence**)&keyObj);
    Properties::Remove(keyObj);
    AutoPtr<IList> aliases;
    s->GetAliases((IList**)&aliases);
    if (aliases != NULL) {
        AutoPtr<IIterator> it;
        aliases->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> strObj;
            it->GetNext((IInterface**)&strObj);
            String str;
            ICharSequence::Probe(strObj)->ToString(&str);
            keyObj = NULL;
            CString::New(String("Alg.Alias.") + type + String(".") + str, (ICharSequence**)&keyObj);
            Properties::Remove(keyObj);
        }
    }
    AutoPtr<IMap> attributes;
    s->GetAttributes((IMap**)&attributes);
    if (attributes != NULL) {
        AutoPtr<ISet> entrySet;
        attributes->GetKeySet((ISet**)&entrySet);
        AutoPtr<IIterator> it;
        entrySet->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> entry;
            it->GetNext((IInterface**)&entry);
            AutoPtr<IInterface> entryKey;
            IMapEntry::Probe(entry)->GetKey((IInterface**)&entryKey);
            String entryKeyStr;
            ICharSequence::Probe(entryKey)->ToString(&entryKeyStr);
            keyObj = NULL;
            CString::New(type + String(".") + algo + String(" ") + entryKeyStr, (ICharSequence**)&keyObj);
            Properties::Remove(keyObj);
        }
    }
    if (mProviderNumber != -1) {
        // if registered then refresh Services
        AutoPtr<IServices> services;
        CServices::AcquireSingleton((IServices**)&services);
        services->SetNeedRefresh();
    }
}

void Provider::RemoveFromPropertyServiceTable(
    /* [in] */ IInterface* key)
{
    if (key == NULL || IString::Probe(key) == NULL) {
        return;
    }
    String k;
    ICharSequence::Probe(key)->ToString(&k);
    if (k.StartWith("Provider.")) { // Provider service type is reserved
        return;
    }
    AutoPtr<IProviderService> s;
    String serviceName;
    String algorithm;
    String attribute;
    Int32 i;
    if (k.StartWith("Alg.Alias.")) { // Alg.Alias.<crypto_service>.<aliasName>=<standardName>
        String aliasName;
        String service_alias = k.Substring(10);
        i = service_alias.IndexOf('.');
        serviceName = service_alias.Substring(0, i);
        aliasName = service_alias.Substring(i + 1);
        if (mPropertyAliasTable != NULL) {
            AutoPtr<ICharSequence> keyObj;
            CString::New(Key(serviceName, aliasName), (ICharSequence**)&keyObj);
            mPropertyAliasTable->Remove(keyObj);
        }
        if (mPropertyServiceTable != NULL) {
            AutoPtr<ICharSequence> keyObj;
            CString::New(aliasName, (ICharSequence**)&keyObj);
            AutoPtr<ICollection> values;
            mPropertyServiceTable->GetValues((ICollection**)&values);
            AutoPtr<IIterator> it;
            values->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                s = IProviderService::Probe(obj);
                AutoPtr<IList> aliases;
                s->GetAliases((IList**)&aliases);
                Boolean result;
                if (aliases->Contains(keyObj, &result), result) {
                    aliases->Remove(keyObj);
                    return;
                }
            }
        }
        return;
    }
    Int32 j = k.IndexOf('.');
    if (j == -1) { // unknown format
        return;
    }

    i = k.IndexOf(' ');
    if (i == -1) { // <crypto_service>.<algorithm_or_type>=<className>
        serviceName = k.Substring(0, j);
        algorithm = k.Substring(j + 1);
        if (mPropertyServiceTable != NULL) {
            AutoPtr<ICharSequence> keyObj;
            CString::New(Key(serviceName, algorithm), (ICharSequence**)&keyObj);
            AutoPtr<IInterface> serObj;
            mPropertyServiceTable->Remove(keyObj, (IInterface**)&serObj);
            IProviderService* ser = IProviderService::Probe(serObj);
            AutoPtr<IList> aliases;
            if (ser != NULL && mPropertyAliasTable != NULL
                    && (ser->GetAliases((IList**)&aliases), aliases != NULL)) {
                AutoPtr<IIterator> it;
                aliases->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> strObj;
                    it->GetNext((IInterface**)&strObj);
                    String alias;
                    ICharSequence::Probe(strObj)->ToString(&alias);
                    AutoPtr<ICharSequence> keyObj;
                    CString::New(Key(serviceName, alias), (ICharSequence**)&keyObj);
                    mPropertyAliasTable->Remove(keyObj);
                }
            }
        }
    }
    else {
        // <crypto_service>.<algorithm_or_type>
        // <attribute_name>=<attrValue>
        attribute = k.Substring(i + 1);
        serviceName = k.Substring(0, j);
        algorithm = k.Substring(j + 1, i);
        if (mPropertyServiceTable != NULL) {
            AutoPtr<ICharSequence> keyObj;
            CString::New(Key(serviceName, algorithm), (ICharSequence**)&keyObj);
            AutoPtr<IInterface> o;
            mPropertyServiceTable->Get(keyObj, (IInterface**)&o);
            if (o != NULL) {
                s = IProviderService::Probe(o);
                AutoPtr<IMap> attributes;
                s->GetAttributes((IMap**)&attributes);
                AutoPtr<ICharSequence> keyObj;
                CString::New(attribute, (ICharSequence**)&keyObj);
                attributes->Remove(keyObj);
            }
        }
    }
}

void Provider::UpdatePropertyServiceTable()
{
    AutoPtr<IProviderService> s;
    String serviceName;
    String algorithm;
    Boolean isEmpty;
    if (mChangedProperties == NULL || (mChangedProperties->IsEmpty(&isEmpty), isEmpty)) {
        return;
    }
    AutoPtr<ISet> entrySet;
    mChangedProperties->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(obj);
        AutoPtr<IInterface> _key;
        AutoPtr<IInterface> _value;
        entry->GetKey((IInterface**)&_key);
        entry->GetValue((IInterface**)&_value);
        if (_key == NULL || _value == NULL || (IString::Probe(_key) == NULL)
                || (IString::Probe(_value) == NULL)) {
            continue;
        }
        String key, value;
        ICharSequence::Probe(_key)->ToString(&key);
        ICharSequence::Probe(_value)->ToString(&value);
        if (key.StartWith("Provider")) {
            // Provider service type is reserved
            continue;
        }
        Int32 i;
        if (key.StartWith("Alg.Alias.")) {
            // Alg.Alias.<crypto_service>.<aliasName>=<standardName>
            String aliasName;
            String service_alias = key.Substring(10);
            i = service_alias.IndexOf('.');
            serviceName = service_alias.Substring(0, i);
            aliasName = service_alias.Substring(i + 1);
            algorithm = value;
            String propertyServiceTableKey = Key(serviceName, algorithm);
            AutoPtr<IInterface> o;
            if (mPropertyServiceTable == NULL) {
                CLinkedHashMap::New(128, (ILinkedHashMap**)&mPropertyServiceTable);
            }
            else {
                AutoPtr<ICharSequence> keyObj;
                CString::New(propertyServiceTableKey, (ICharSequence**)&keyObj);
                mPropertyServiceTable->Get(keyObj, (IInterface**)&o);
            }
            if (o != NULL) {
                s = IProviderService::Probe(o);
                s->AddAlias(aliasName);
                if (mPropertyAliasTable == NULL) {
                    CLinkedHashMap::New(256, (ILinkedHashMap**)&mPropertyAliasTable);
                }
                AutoPtr<ICharSequence> keyObj;
                CString::New(Key(serviceName, aliasName), (ICharSequence**)&keyObj);
                mPropertyAliasTable->Put(keyObj, s);
            }
            else {
                AutoPtr<ICharSequence> keyObj;
                CString::New(serviceName + "." + algorithm, (ICharSequence**)&keyObj);
                AutoPtr<IInterface> valueObj;
                mChangedProperties->Get(keyObj, (IInterface**)&valueObj);
                String className;
                ICharSequence::Probe(valueObj)->ToString(&className);
                if (!className.IsNull()) {
                    AutoPtr<IList> l;
                    CArrayList::New((IList**)&l);
                    AutoPtr<ICharSequence> aliasNameObj;
                    CString::New(aliasName, (ICharSequence**)&aliasNameObj);
                    l->Add(aliasNameObj);
                    AutoPtr<IMap> attributes;
                    CHashMap::New((IMap**)&attributes);
                    s = NULL;
                    CProviderService::New(this, serviceName, algorithm, className,
                            l, attributes, (IProviderService**)&s);
                    keyObj = NULL;
                    CString::New(propertyServiceTableKey, (ICharSequence**)&keyObj);
                    mPropertyServiceTable->Put(keyObj, s);
                    if (mPropertyAliasTable == NULL) {
                        CLinkedHashMap::New(256, (ILinkedHashMap**)&mPropertyAliasTable);
                    }
                    keyObj = NULL;
                    CString::New(Key(serviceName, aliasName), (ICharSequence**)&keyObj);
                    mPropertyAliasTable->Put(keyObj, s);
                }
            }
            continue;
        }
        Int32 j = key.IndexOf('.');
        if (j == -1) { // unknown format
            continue;
        }
        i = key.IndexOf(' ');
        if (i == -1) { // <crypto_service>.<algorithm_or_type>=<className>
            serviceName = key.Substring(0, j);
            algorithm = key.Substring(j + 1);
            String propertyServiceTableKey = Key(serviceName, algorithm);
            AutoPtr<IInterface> o;
            if (mPropertyServiceTable != NULL) {
                AutoPtr<ICharSequence> keyObj;
                CString::New(propertyServiceTableKey, (ICharSequence**)&keyObj);
                mPropertyServiceTable->Get(keyObj, (IInterface**)&o);
            }
            if (o != NULL) {
                s = IProviderService::Probe(o);
                s->SetClassName(value);
            }
            else {
                AutoPtr<ICollections> collections;
                CCollections::AcquireSingleton((ICollections**)&collections);
                AutoPtr<IList> emptyList;
                collections->GetEmptyList((IList**)&emptyList);
                AutoPtr<IMap> emptyMap;
                collections->GetEmptyMap((IMap**)&emptyMap);
                s = NULL;
                CProviderService::New(this, serviceName, algorithm,
                        value, emptyList, emptyMap, (IProviderService**)&s);
                if (mPropertyServiceTable == NULL) {
                    CLinkedHashMap::New(128, (ILinkedHashMap**)&mPropertyServiceTable);
                }
                AutoPtr<ICharSequence> keyObj;
                CString::New(propertyServiceTableKey, (ICharSequence**)&keyObj);
                mPropertyServiceTable->Put(keyObj, s);
            }
        }
        else {
            // <crypto_service>.<algorithm_or_type> <attribute_name>=<attrValue>
            serviceName = key.Substring(0, j);
            algorithm = key.Substring(j + 1, i);
            String attribute = key.Substring(i + 1);
            String propertyServiceTableKey = Key(serviceName, algorithm);
            AutoPtr<IInterface> o;
            if (mPropertyServiceTable != NULL) {
                AutoPtr<ICharSequence> keyObj;
                CString::New(propertyServiceTableKey, (ICharSequence**)&keyObj);
                mPropertyServiceTable->Get(keyObj, (IInterface**)&o);
            }
            if (o != NULL) {
                s = IProviderService::Probe(o);
                s->PutAttribute(attribute, value);
            }
            else {
                AutoPtr<ICharSequence> keyObj;
                CString::New(serviceName + "." + algorithm, (ICharSequence**)&keyObj);
                AutoPtr<IInterface> valueObj;
                mChangedProperties->Get(keyObj, (IInterface**)&valueObj);
                String className;
                ICharSequence::Probe(valueObj)->ToString(&className);
                if (!className.IsNull()) {
                    AutoPtr<IMap> m;
                    CHashMap::New((IMap**)&m);
                    AutoPtr<ICharSequence> keyObj, valueObj;
                    CString::New(attribute, (ICharSequence**)&keyObj);
                    CString::New(value, (ICharSequence**)&valueObj);
                    m->Put(keyObj, valueObj);
                    AutoPtr<IList> l;
                    CArrayList::New((IList**)&l);
                    s = NULL;
                    CProviderService::New(this, serviceName, algorithm, className,
                            l, m, (IProviderService**)&s);
                    if (mPropertyServiceTable == NULL) {
                        CLinkedHashMap::New(128, (ILinkedHashMap**)&mPropertyServiceTable);
                    }
                    keyObj = NULL;
                    CString::New(propertyServiceTableKey, (ICharSequence**)&keyObj);
                    mPropertyServiceTable->Put(keyObj, s);
                }
            }
        }
    }
    ServicesChanged();
    mChangedProperties = NULL;
}

void Provider::ServicesChanged()
{
    mLastServicesByType = NULL;
    mLastServiceName = NULL;
    mLastServicesSet = NULL;
}

void Provider::PutProviderInfo()
{
    AutoPtr<ICharSequence> keyObj, valueObj;
    CString::New(String("Provider.id name"), (ICharSequence**)&keyObj);
    if (!mName.IsNull()) {
        CString::New(mName, (ICharSequence**)&valueObj);
    }
    else {
        CString::New(String("null"), (ICharSequence**)&valueObj);
    }
    Properties::Put(keyObj, valueObj);
    keyObj = NULL;
    CString::New(String("Provider.id version"), (ICharSequence**)&keyObj);
    valueObj = NULL;
    CString::New(mVersionString, (ICharSequence**)&valueObj);
    Properties::Put(keyObj, valueObj);
    keyObj = NULL;
    CString::New(String("Provider.id info"), (ICharSequence**)&keyObj);
    valueObj = NULL;
    if (!mInfo.IsNull()) {
        CString::New(mInfo, (ICharSequence**)&valueObj);
    }
    else {
        CString::New(String("null"), (ICharSequence**)&valueObj);
    }
    Properties::Put(keyObj, valueObj);
    keyObj = NULL;
    CString::New(String("Provider.id className"), (ICharSequence**)&keyObj);
    valueObj = NULL;
    AutoPtr<IClassInfo> clsInfo;
    CObject::ReflectClassInfo(Probe(EIID_IInterface), (IClassInfo**)&clsInfo);
    String name, ns;
    clsInfo->GetName(&name);
    clsInfo->GetNamespace(&ns);
    CString::New(name + ns, (ICharSequence**)&valueObj);
    Properties::Put(keyObj, valueObj);
}

String Provider::GetPropertyIgnoreCase(
    /* [in] */ const String& key)
{
    String res;
    GetProperty(key, &res);
    if (!res.IsNull()) {
        return res;
    }
    AutoPtr<IEnumeration> names;
    PropertyNames((IEnumeration**)&names);
    Boolean hasMore;
    while (names->HasMoreElements(&hasMore), hasMore) {
        AutoPtr<IInterface> nameObj;
        names->GetNextElement((IInterface**)&nameObj);
        String propertyName;
        ICharSequence::Probe(nameObj)->ToString(&propertyName);
        if (key.EqualsIgnoreCase(propertyName)) {
            GetProperty(propertyName, &res);
            return res;
        }
    }
    return String(NULL);
}

String Provider::Key(
    /* [in] */ const String& type,
    /* [in] */ const String& algorithm)
{
    return type + String(".") + algorithm.ToUpperCase(/*Locale.US*/);
}

ECode Provider::ReadObject(
    /* [in] */ IObjectInputStream* in)
{
    FAIL_RETURN(in->DefaultReadObject())
    mVersionString = StringUtils::ToString(mVersion);
    mProviderNumber = -1;
    return NOERROR;
}

} // namespace Security
} // namespace Elastos
