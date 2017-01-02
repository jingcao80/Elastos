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

#include "SAXParserFactory.h"
#include "Thread.h"
#include "CPathClassLoader.h"
#include "utility/logging/Slogger.h"
// #include "org/apache/harmony/xml/parsers/CSAXParserFactoryImpl.h"

using Elastos::Core::Thread;
using Elastos::Core::CPathClassLoader;
// using Org::Apache::Harmony::Xml::Parsers::CSAXParserFactoryImpl;

namespace Elastosx {
namespace Xml {
namespace Parsers {

CAR_INTERFACE_IMPL(SAXParserFactory, Object, ISAXParserFactory)

ECode SAXParserFactory::SetNamespaceAware(
    /* [in]*/ Boolean awareness)
{
    mNamespaceAware = awareness;
    return NOERROR;
}

ECode SAXParserFactory::SetValidating(
    /* [in]*/ Boolean validating)
{
    mValidating = validating;
    return NOERROR;
}

ECode SAXParserFactory::IsNamespaceAware(
    /* [out]*/ Boolean* isAware)
{
    *isAware = mNamespaceAware;
    return NOERROR;
}

ECode SAXParserFactory::IsValidating(
    /* [out]*/ Boolean* isValidating)
{
    *isValidating = mValidating;
    return NOERROR;
}

ECode SAXParserFactory::GetSchema(
    /* [out]*/ ISchema** schema)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode SAXParserFactory::SetSchema(
    /* [in]*/ ISchema* schema)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode SAXParserFactory::SetXIncludeAware(
    /* [in]*/ Boolean state)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode SAXParserFactory::IsXIncludeAware(
    /* [out]*/ Boolean* isAware)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode SAXParserFactory::NewInstance(
    /* [out]*/ ISAXParserFactory** instance)
{
    // return CSAXParserFactoryImpl::New(instance);
    return NOERROR;
}

ECode SAXParserFactory::NewInstance(
    /* [in]*/ const String& factoryClassName,
    /* [in]*/ IClassLoader* classLoader,
    /* [out]*/ ISAXParserFactory** instance)
{
    *instance = NULL;
    if (factoryClassName == NULL) {
        SLOGGERD("SAXParserFactory", "factoryClassName == null");
        return E_FACTORY_CONFIGURATION_EXCEPTION;
    }

    AutoPtr<IClassLoader> pLoader = classLoader;
    if (pLoader == NULL) {
        FAIL_RETURN(Thread::GetCurrentThread()->GetContextClassLoader((IClassLoader**)&pLoader))
    }

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
        *instance = ISAXParserFactory::Probe(tmp);
        REFCOUNT_ADD(*instance)
        return NOERROR;
    //     Class<?> type = classLoader != null
    //             ? classLoader.loadClass(factoryClassName)
    //             : Class.forName(factoryClassName);
    //     return (DocumentBuilderFactory) type.newInstance();
    // } catch (ClassNotFoundException e) {
    //     throw new FactoryConfigurationError(e);
    // } catch (InstantiationException e) {
    //     throw new FactoryConfigurationError(e);
    // } catch (IllegalAccessException e) {
    //     throw new FactoryConfigurationError(e);
    // }
}

SAXParserFactory::SAXParserFactory()
    : mValidating(FALSE)
    , mNamespaceAware(FALSE)
{}

} // namespace Parsers
} // namespace Xml
} // namespace Elastosx
