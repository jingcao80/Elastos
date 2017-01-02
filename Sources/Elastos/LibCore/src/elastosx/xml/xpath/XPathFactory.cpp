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

#include "XPathFactory.h"

namespace Elastosx {
namespace Xml {
namespace XPath {

const String XPathFactory::DEFAULT_PROPERTY_NAME = String("javax.xml.xpath.XPathFactory");
const String XPathFactory::DEFAULT_OBJECT_MODEL_URI = String("http://java.sun.com/jaxp/xpath/dom");

// {129b344c-aeac-4f57-936a-e58637cd4052}
extern const _ELASTOS ClassID ECLSID_XPathFactory =
        { 0x129b344c, 0xaeac, 0x4f57, { 0x93, 0x6a, 0xe5, 0x86, 0x37, 0xcd, 0x40, 0x52 } };

CAR_INTERFACE_IMPL(XPathFactory, Object, IXPathFactory)

CAR_OBJECT_IMPL(XPathFactory)

ECode XPathFactory::NewInstance(
    /* [out] */ IXPathFactory** instance)
{
    VALIDATE_NOT_NULL(instance)
    return NewInstance(DEFAULT_OBJECT_MODEL_URI, instance);
}

ECode XPathFactory::NewInstance(
    /* [in] */ const String& uri,
    /* [out] */ IXPathFactory** instance)
{
    VALIDATE_NOT_NULL(instance)
    if (uri.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (uri.GetLength() == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
//    ClassLoader classLoader = Thread.currentThread().getContextClassLoader();
//    if (classLoader == null) {
        //use the current class loader
//        classLoader = XPathFactory.class.getClassLoader();
//    }
//    XPathFactory xpathFactory = new XPathFactoryFinder(classLoader).newFactory(uri);
//    if (xpathFactory == null) {
//        throw new XPathFactoryConfigurationException(
//            "No XPathFactory implementation found for the object model: "
//            + uri
//        );
//    }
//   return xpathFactory;
/*
    AutoPtr<IThread> thread = Thread::GetCurrentThread();
    AutoPtr<IClassLoader> classLoader;
    thread->GetContextClassLoader((IClassLoader**)&classLoader);
    if (classLoader == NULL) {
        //use the current class loader
//        classLoader = XPathFactory.class.getClassLoader();
    }
    AutoPtr<IXPathFactory> XPathFactory;
**/
    return NOERROR;
}

}// namespace XPath
}// namespace Xml
}// namespace Elastosx