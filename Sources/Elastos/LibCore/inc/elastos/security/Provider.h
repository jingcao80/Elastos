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

#ifndef __ELASTOS_SECURITY_PROVIDER_H__
#define __ELASTOS_SECURITY_PROVIDER_H__

#include "Elastos.CoreLibrary.Security.h"
#include "elastos/utility/Properties.h"

using Elastos::IO::IObjectInputStream;
using Elastos::Utility::ICollection;
using Elastos::Utility::ILinkedHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::Properties;

namespace Elastos {
namespace Security {

/**
 * {@code Provider} is the abstract superclass for all security providers in the
 * Java security infrastructure.
 */
class ECO_PUBLIC Provider
    : public Properties
    , public IProvider
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the name of this provider.
     *
     * @return the name of this provider.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the version number for the services being provided.
     *
     * @return the version number for the services being provided.
     */
    CARAPI GetVersion(
        /* [out] */ Double* version);

    /**
     * Returns a description of the services being provided.
     *
     * @return a description of the services being provided.
     */
    CARAPI GetInfo(
        /* [out] */ String* info);

    /**
     * Returns a string containing a concise, human-readable description of
     * this {@code Provider} including its name and its version.
     *
     * @return a printable representation for this {@code Provider}.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Clears all properties used to look up services implemented by this
     * {@code Provider}.
     */
    // @Override
    CARAPI Clear();

    // @Override
    CARAPI Load(
        /* [in] */ IInputStream* inStream);

    /**
     * Copies all from the provided map to this {@code Provider}.
     * @param t
     *            the mappings to copy to this provider.
     */
    // @Override
    CARAPI PutAll(
        /* [in] */ IMap* t);

    // @Override
    CARAPI GetEntrySet(
        /* [out] */ ISet** entries);

    // @Override
    CARAPI GetKeySet(
        /* [out] */ ISet** keys);

    // @Override
    CARAPI GetValues(
        /* [out] */ ICollection** values);

    /**
     * Maps the specified {@code key} property name to the specified {@code
     * value}.
     *
     * @param key
     *            the name of the property.
     * @param value
     *            the value of the property.
     * @return the value that was previously mapped to the specified {@code key}
     *         ,or {@code null} if it did not have one.
     */
    // @Override
    CARAPI Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** oldValue);

    using Properties::Put;

    /**
     * Removes the specified {@code key} and its associated value from this
     * {@code Provider}.
     *
     * @param key
     *            the name of the property
     * @return the value that was mapped to the specified {@code key} ,or
     *         {@code null} if no mapping was present
     */
    // @Override
    CARAPI Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value);

    /**
     * Returns true if this provider implements the given algorithm. Caller
     * must specify the cryptographic service and specify constraints via the
     * attribute name and value.
     *
     * @param serv
     *            Crypto service.
     * @param alg
     *            Algorithm or type.
     * @param attribute
     *            The attribute name or {@code null}.
     * @param val
     *            The attribute value.
     * @return
     */
    CARAPI ImplementsAlg(
        /* [in] */ const String& serv,
        /* [in] */ const String& alg,
        /* [in] */ const String& attribute,
        /* [in] */ const String& val,
        /* [out] */ Boolean* result);

    /**
     *
     * Set the provider preference order number.
     *
     * @param n
     */
    CARAPI SetProviderNumber(
        /* [in] */ Int32 n);

    /**
     *
     * Get the provider preference order number.
     *
     * @return
     */
    CARAPI GetProviderNumber(
        /* [out] */ Int32* n);

    /**
     * Get the service of the specified type
     *
     */
    CARAPI GetService(
        /* [in] */ const String& type,
        /* [out] */ IProviderService** service);

    /**
     * Returns the service with the specified {@code type} implementing the
     * specified {@code algorithm}, or {@code null} if no such implementation
     * exists.
     * <p>
     * If two services match the requested type and algorithm, the one added
     * with the {@link #putService(Service)} is returned (as opposed to the one
     * added via {@link #put(Object, Object)}.
     *
     * @param type
     *            the type of the service (for example {@code KeyPairGenerator})
     * @param algorithm
     *            the algorithm name (case insensitive)
     * @return the requested service, or {@code null} if no such implementation
     *         exists
     */
    CARAPI GetService(
        /* [in] */ const String& type,
        /* [in] */ const String& algorithm,
        /* [out] */ IProviderService** service);

    /**
     * Returns an unmodifiable {@code Set} of all services registered by this
     * provider.
     *
     * @return an unmodifiable {@code Set} of all services registered by this
     *         provider
     */
    CARAPI GetServices(
        /* [out] */ ISet** services);

protected:
    /**
     * Constructs a new instance of {@code Provider} with its name, version and
     * description.
     *
     * @param name
     *            the name of the provider.
     * @param version
     *            the version of the provider.
     * @param info
     *            a description of the provider.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Double version,
        /* [in] */ const String& info);

    /**
     * Adds a {@code Service} to this {@code Provider}. If a service with the
     * same name was registered via this method, it is replace.
     *
     * @param s
     *            the {@code Service} to register
     */
    virtual CARAPI PutService(
        /* [in] */ IProviderService* s);

    /**
     * Removes a previously registered {@code Service} from this {@code
     * Provider}.
     *
     * @param s
     *            the {@code Service} to remove
     * @throws NullPointerException
     *             if {@code s} is {@code null}
     */
    virtual CARAPI RemoveService(
        /* [in] */ IProviderService* s);

private:
    ECO_LOCAL CARAPI_(void) MyPutAll(
        /* [in] */ IMap* t);

    /**
     * Returns true if this provider has the same value as is given for the
     * given attribute
     */
    ECO_LOCAL CARAPI_(Boolean) CheckAttribute(
        /* [in] */ const String& servAlg,
        /* [in] */ const String& attribute,
        /* [in] */ const String& val);

    /**
     * Add Service information to the provider's properties.
     */
    ECO_LOCAL CARAPI_(void) ServiceInfoToProperties(
        /* [in] */ IProviderService* s);

    /**
     * Remove Service information from the provider's properties.
     */
    ECO_LOCAL CARAPI_(void) ServiceInfoFromProperties(
        /* [in] */ IProviderService* s);

    // Remove property information from provider Services
    ECO_LOCAL CARAPI_(void) RemoveFromPropertyServiceTable(
        /* [in] */ IInterface* key);

    // Update provider Services if the properties was changed
    ECO_LOCAL CARAPI_(void) UpdatePropertyServiceTable();

    ECO_LOCAL CARAPI_(void) ServicesChanged();

    /**
     * These attributes should be placed in each Provider object:
     * Provider.id name, Provider.id version, Provider.id info,
     * Provider.id className
     */
    ECO_LOCAL CARAPI_(void) PutProviderInfo();

    /**
     * Returns the property with the specified key in the provider properties.
     * The name is not case-sensitive.
     */
    ECO_LOCAL CARAPI_(String) GetPropertyIgnoreCase(
        /* [in] */ const String& key);

    ECO_LOCAL static CARAPI_(String) Key(
        /* [in] */ const String& type,
        /* [in] */ const String& algorithm);

    ECO_LOCAL CARAPI ReadObject(
        /* [in] */ IObjectInputStream* in);

private:
    String mName;

    Double mVersion;

    // String representation of the provider version number.
    String mVersionString;

    String mInfo;

    //The provider preference order number.
    // Equals -1 for non registered provider.
    Int32 mProviderNumber;

    // Contains "Service.Algorithm" and Provider.Service classes added using
    // putService()
    AutoPtr<ILinkedHashMap> mServiceTable;

    // Contains "Service.Alias" and Provider.Service classes added using
    // putService()
    AutoPtr<ILinkedHashMap> mAliasTable;

    // Contains "Service.Algorithm" and Provider.Service classes added using
    // put()
    AutoPtr<ILinkedHashMap> mPropertyServiceTable;

    // Contains "Service.Alias" and Provider.Service classes added using put()
    AutoPtr<ILinkedHashMap> mPropertyAliasTable;

    // The properties changed via put()
    AutoPtr<ILinkedHashMap> mChangedProperties;

    // For getService(String type, String algorithm) optimization:
    // previous result
    AutoPtr<IProviderService> mReturnedService;
    // previous parameters
    String mLastAlgorithm;
    // last name
    String mLastServiceName;

    // For getServices() optimization:
    AutoPtr<ISet> mLastServicesSet;

    // For getService(String type) optimization:
    String mLastType;
    // last Service found by type
    AutoPtr<IProviderService> mLastServicesByType;
};

} // namespace Security
} // namespace Elastos

#endif //__ELASTOS_SECURITY_PROVIDER_H__
