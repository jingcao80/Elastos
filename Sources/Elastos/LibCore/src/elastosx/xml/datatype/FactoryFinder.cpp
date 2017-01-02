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

#include "FactoryFinder.h"
#include "CProperties.h"
#include "CSystem.h"
#include "utility/logging/Slogger.h"
#include "CPathClassLoader.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::CSystem;
using Elastos::Core::CPathClassLoader;
using Elastos::Utility::IProperties;
using Elastos::Utility::CProperties;

namespace Elastosx {
namespace Xml {
namespace Datatype {

// static AutoPtr<IProperties> InitProps()
// {
//     AutoPtr<CProperties> p;
//     CProperties::NewByFriend((CProperties**)&p);
//     return p;
// }

// AutoPtr<IProperties> FactoryFinder::sCacheProps = InitProps();
Boolean FactoryFinder::sFirstTime = TRUE;
const Int32 FactoryFinder::DEFAULT_LINE_LENGTH = 80;

ECode FactoryFinder::NewInstance(
    /* [in] */ const String& clsName,
    /* [in] */ IClassLoader* loader,
    /* [out] */ IInterface** obj)
{
    *obj = NULL;
    AutoPtr<IClassLoader> pLoader = loader;
    if (pLoader == NULL) {
        FAIL_RETURN(FindClassLoader(clsName, (IClassLoader**)&pLoader))
    }

    AutoPtr<IClassInfo> clsInfo;
    FAIL_RETURN(pLoader->LoadClass(clsName, (IClassInfo**)&clsInfo))
    if (clsInfo == NULL) {
        return E_CLASS_NOT_FOUND_EXCEPTION;
    }
    ClassID id;
    clsInfo->GetId(&id);
    return CObject::CreateInstance(id, RGM_SAME_DOMAIN, EIID_IInterface, obj);
}

ECode FactoryFinder::Find(
    /* [in] */ const String& factoryId,
    /* [in] */ const String& fallbackClassName,
    /* [out] */ IInterface** obj)
{
    AutoPtr<IClassLoader> pLoader;
    AutoPtr<CSystem> sys;
    CSystem::AcquireSingletonByFriend((CSystem**)&sys);
    String systemProp;
    sys->GetProperty(factoryId, &systemProp);
    if (systemProp != NULL && systemProp.GetLength() > 0) {
        FAIL_RETURN(FindClassLoader(systemProp, (IClassLoader**)&pLoader))
        return NewInstance(systemProp, pLoader, obj);
    }

    // // try to read from $java.home/lib/jaxp.properties
    // try {
    //     String javah = System.getProperty("java.home");
    //     String configFile = javah + File.separator + "lib" + File.separator + "jaxp.properties";
    //     String factoryClassName = null;
    //     if (firstTime) {
    //         {    AutoLock syncLock(cacheProps);
    //             if (firstTime) {
    //                 File f = new File(configFile);
    //                 firstTime = false;
    //                 if (f.exists()) {
    //                     if (debug) debugPrintln("Read properties file " + f);
    //                     cacheProps.load(new FileInputStream(f));
    //                 }
    //             }
    //         }
    //     }
    //     factoryClassName = cacheProps.getProperty(factoryId);
    //     if (debug) debugPrintln("found " + factoryClassName + " in $java.home/jaxp.properties");

    //     if (factoryClassName != null) {
    //         return newInstance(factoryClassName, classLoader);
    //     }
    // } catch (Exception ex) {
    //     if (debug) {
    //         ex.printStackTrace();
    //     }
    // }

    // // Try Jar Service Provider Mechanism
    // Object provider = findJarServiceProvider(factoryId);
    // if (provider != null) {
    //     return provider;
    // }

    if (fallbackClassName == NULL) {
        // throw new ConfigurationError(
        SLOGGERD("FactoryFinder", String("Provider for ") + factoryId + " cannot be found")
        return E_CLASS_NOT_FOUND_EXCEPTION;
    }

    // if (debug) debugPrintln("FactoryFinder","loaded from fallback value: " + fallbackClassName);
    return NewInstance(fallbackClassName, NULL, obj);
}

FactoryFinder::FactoryFinder(){}

ECode FactoryFinder::FindClassLoader(
    /* [in] */ const String& clsName,
    /* [out] */ IClassLoader** pLoader)
{
    *pLoader = NULL;
    String path = clsName.Substring(0, clsName.LastIndexOf('.'));
    return CPathClassLoader::New(path, NULL, pLoader);
}

} // namespace Datatype
} // namespace Xml
} // namespace Elastosx