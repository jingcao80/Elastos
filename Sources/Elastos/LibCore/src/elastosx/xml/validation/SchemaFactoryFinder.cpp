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

#include "SchemaFactoryFinder.h"
#include "utility/CProperties.h"
#include "utility/logging/Slogger.h"
#include "CSystem.h"
#include "Thread.h"
#include "CPathClassLoader.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::Thread;
using Elastos::Core::CPathClassLoader;
using Elastos::Core::CSystem;
using Elastos::Utility::CProperties;

namespace Elastosx {
namespace Xml {
namespace Validation {

static AutoPtr<IProperties> InitProperties()
{
    AutoPtr<CProperties> sCacheProps;
    CProperties::NewByFriend((CProperties**)&sCacheProps);
    return sCacheProps;
}

AutoPtr<IProperties> SchemaFactoryFinder::sCacheProps = InitProperties();
Boolean SchemaFactoryFinder::sFirstTime = FALSE;
Int32 SchemaFactoryFinder::DEFAULT_LINE_LENGTH = 80;
String SchemaFactoryFinder::SERVICE_CLASS = String("SchemaFactory");
const String SchemaFactoryFinder::W3C_XML_SCHEMA10_NS_URI = String("http://www.w3.org/XML/XMLSchema/v1.0");
const String SchemaFactoryFinder::W3C_XML_SCHEMA11_NS_URI = String("http://www.w3.org/XML/XMLSchema/v1.1");

CAR_INTERFACE_IMPL(SchemaFactoryFinder, Object, ISchemaFactoryFinder)

SchemaFactoryFinder::SchemaFactoryFinder(
    /* [in] */ IClassLoader* loader)
{
    mClassLoader = loader;
}

ECode SchemaFactoryFinder::NewFactory(
    /* [in] */ const String& schemaLanguage,
    /* [out] */ ISchemaFactory** factory)
{
    if (schemaLanguage == NULL) {
        SLOGGERD("SchemaFactoryFinder", "schemaLanguage == null")
        return E_NULL_POINTER_EXCEPTION;
    }
    return _NewFactory(schemaLanguage, factory);
}

ECode SchemaFactoryFinder::CreateInstance(
    /* [in] */ const String& factoryClassName,
    /* [out] */ ISchemaFactory** factory)
{
    *factory = NULL;
    SLOGGERD("SchemaFactoryFinder", String("instantiating ") + factoryClassName)
    AutoPtr<IClassLoader> pLoader;
    FAIL_RETURN(Thread::GetCurrentThread()->GetContextClassLoader((IClassLoader**)&pLoader))
    // try {
        if (pLoader == NULL) {
            String path = factoryClassName.Substring(0, factoryClassName.LastIndexOf('.'));
            FAIL_RETURN(CPathClassLoader::New(path, NULL, (IClassLoader**)&pLoader))
        }

        AutoPtr<IClassInfo> clsInfo;
        FAIL_RETURN(pLoader->LoadClass(factoryClassName, (IClassInfo**)&clsInfo))
        if (clsInfo == NULL) {
            return E_CLASS_NOT_FOUND_EXCEPTION;
        }
        ClassID id;
        clsInfo->GetId(&id);
        AutoPtr<IInterface> tmp;
        FAIL_RETURN(CObject::CreateInstance(id, RGM_SAME_DOMAIN, EIID_IInterface, (IInterface**)&tmp))
        *factory = ISchemaFactory::Probe(tmp);
        REFCOUNT_ADD(*factory)
        return NOERROR;
    // try {
        // Class clazz;
        // if( classLoader != NULL)
        //     clazz = classLoader.loadClass(className);
        // else
        //     clazz = Class.forName(className);
        // if(debug)       debugPrintln("loaded it from "+which(clazz));
        // Object o = clazz.newInstance();

        // if( o instanceof SchemaFactory )
        //     return (SchemaFactory)o;

        // if (debug) debugPrintln(className+" is not assignable to "+SERVICE_CLASS.getName());
    // }
    // // The VM ran out of memory or there was some other serious problem. Re-throw.
    // catch (VirtualMachineError vme) {
    //     throw vme;
    // }
    // // ThreadDeath should always be re-thrown
    // catch (ThreadDeath td) {
    //     throw td;
    // }
    // catch (Throwable t) {
    //     debugPrintln("failed to instantiate "+className);
    //     if(debug)   t.printStackTrace();
    // }
    // return null;
    return NOERROR;
}

ECode SchemaFactoryFinder::_NewFactory(
    /* [in] */ const String& schemaLanguage,
    /* [out] */ ISchemaFactory** factory)
{
    *factory = NULL;
    AutoPtr<ISchemaFactory> sf;
    String propertyName = SERVICE_CLASS + ":" + schemaLanguage;
    SLOGGERD("SchemaFactoryFinder", String("Looking up system property :") + propertyName)
    AutoPtr<CSystem> sys;
    CSystem::AcquireSingletonByFriend((CSystem**)&sys);
    String r;
    sys->GetProperty(propertyName, &r);

    if (r != NULL && r.GetLength() > 0) {
        SLOGGERD("SchemaFactoryFinder", String("The value is :") + r)
        FAIL_RETURN(CreateInstance(r, factory))
        if (*factory != NULL) {
            return NOERROR;
        }
    } else {
        SLOGGERD("SchemaFactoryFinder", "The property is undefined.")
    }


    // String javah = System.getProperty("java.home");
    // String configFile = javah + File.separator +
    // "lib" + File.separator + "jaxp.properties";

    // String factoryClassName = null ;

    // // try to read from $java.home/lib/jaxp.properties
    // try {
    //     if(firstTime){
    //         {    AutoLock syncLock(cacheProps);
    //             if(firstTime){
    //                 File f=new File( configFile );
    //                 firstTime = false;
    //                 if(f.exists()){
    //                     if (debug) debugPrintln("Read properties file " + f);
    //                     cacheProps.load(new FileInputStream(f));
    //                 }
    //             }
    //         }
    //     }
    //     factoryClassName = cacheProps.getProperty(propertyName);
    //     if (debug) debugPrintln("found " + factoryClassName + " in $java.home/jaxp.properties");

    //     if (factoryClassName != null) {
    //         sf = createInstance(factoryClassName);
    //         if(sf != null){
    //             return sf;
    //         }
    //     }
    // } catch (Exception ex) {
    //     if (debug) {
    //         ex.printStackTrace();
    //     }
    // }

    // // try META-INF/services files
    // for (URL resource : createServiceFileIterator()) {
    //     if (debug) debugPrintln("looking into " + resource);
    //     try {
    //         sf = loadFromServicesFile(schemaLanguage,resource.toExternalForm(),
    //                 resource.openStream());
    //         if(sf!=null)    return sf;
    //     } catch(IOException e) {
    //         if( debug ) {
    //             debugPrintln("failed to read "+resource);
    //             e.printStackTrace();
    //         }
    //     }
    // }

    // platform defaults
    if (schemaLanguage.Equals(IXMLConstants::W3C_XML_SCHEMA_NS_URI) || schemaLanguage.Equals(W3C_XML_SCHEMA10_NS_URI)) {
        SLOGGERD("SchemaFactoryFinder", "attempting to use the platform default XML Schema 1.0 validator");
        // return createInstance("org.apache.xerces.jaxp.validation.XMLSchemaFactory");
        SLOGGERD("SchemaFactoryFinder", "TODO: No class info")
    }
    else if (schemaLanguage.Equals(W3C_XML_SCHEMA11_NS_URI)) {
        SLOGGERD("SchemaFactoryFinder", "attempting to use the platform default XML Schema 1.1 validator");
        // return createInstance("org.apache.xerces.jaxp.validation.XMLSchema11Factory");
        SLOGGERD("SchemaFactoryFinder", "TODO: No class info")
    }

    SLOGGERD("SchemaFactoryFinder", "all things were tried, but none was found. bailing out.");
    return NOERROR;
}

ECode SchemaFactoryFinder::CreateServiceFileIterator(
    /* [out] */ IIterable** it)
{
    return NOERROR;
}

ECode SchemaFactoryFinder::LoadFromServicesFile(
    /* [in] */ const String& schemaLanguage,
    /* [in] */ const String& resourceName,
    /* [in] */ IInputStream* in,
    /* [out] */ ISchemaFactory** factory)
{
    return NOERROR;
}

ECode SchemaFactoryFinder::Which(
    /* [in] */ ClassID clazz,
    /* [out] */ String* str)
{
    return NOERROR;
}

} // namespace Validation
} // namespace Xml
} // namespace Elastosx