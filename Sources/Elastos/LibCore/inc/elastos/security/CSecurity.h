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

#ifndef __ELASTOS_SECURITY_CSECURITY_H__
#define __ELASTOS_SECURITY_CSECURITY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "_Elastos_Security_CSecurity.h"
#include "Object.h"
#include "Singleton.h"

using Elastos::Core::Object;
using Elastos::Core::Singleton;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IProperties;
using Org::Apache::Harmony::Security::Fortress::ISecurityAccess;

namespace Elastos {
namespace Security {

/**
 * {@code Security} is the central class in the Java Security API. It manages
 * the list of security {@code Provider} that have been installed into this
 * runtime environment.
 */
CarClass(CSecurity)
    , public Singleton
    , public ISecurity
{
private:
    class SecurityDoor
        : public Object
        , public ISecurityAccess
    {
    public:
        CAR_INTERFACE_DECL()

        //CAR_OBJECT_DECL()

        /**
         * Access to Security.renumProviders()
         */
        CARAPI RenumProviders();

        /**
         * Access to Service.getAliases()
         */
        CARAPI GetAliases(
            /* [in] */ IProviderService* s,
            /* [out] */ IList** aliases);

        /**
         * Access to Provider.getService(String type)
         */
        CARAPI GetService(
            /* [in] */ IProvider* p,
            /* [in] */ const String& type,
            /* [out] */ IProviderService** service);
    };

    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetAlgorithmProperty(
        /* [in] */ const String& algName,
        /* [in] */ const String& propName,
        /* [out] */ String* algProp);

    CARAPI InsertProviderAt(
        /* [in] */ IProvider* provider,
        /* [in] */ Int32 position,
        /* [out] */ Int32* pos);

    CARAPI AddProvider(
        /* [in] */ IProvider* provider,
        /* [out] */ Int32* pos);

    CARAPI RemoveProvider(
        /* [in] */ const String& name);

    CARAPI GetProviders(
        /* [out, callee] */ ArrayOf<IProvider*>** providers);

    CARAPI GetProvider(
        /* [in] */ const String& name,
        /* [out] */ IProvider** provider);

    CARAPI GetProviders(
        /* [in] */ const String& filter,
        /* [out, callee] */ ArrayOf<IProvider*>** providers);

    CARAPI GetProviders(
        /* [in] */ IMap* filter,
        /* [out, callee] */ ArrayOf<IProvider*>** providers);

    CARAPI GetProperty(
        /* [in] */ const String& key,
        /* [out] */ String* prop);

    CARAPI SetProperty(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

    CARAPI GetAlgorithms(
        /* [in] */ const String& serviceName,
        /* [out] */ ISet** algs);

private:
    static CARAPI_(void) RegisterDefaultProviders();

    static CARAPI_(void) FilterProviders(
        /* [in] */ IArrayList* providers,
        /* [in] */ const String& service,
        /* [in] */ const String& algorithm,
        /* [in] */ const String& attribute,
        /* [in] */ const String& attrValue);

    static CARAPI RenumProviders();

    static CARAPI_(AutoPtr<IProperties>) Init_sSecprops();

private:
    // Security properties
    static const AutoPtr<IProperties> sSecprops;
    static const StaticInitializer sInitializer;
};

}
}

#endif // __ELASTOS_SECURITY_CSECURITY_H__
