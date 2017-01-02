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

#ifndef __ELASTOSX_XML_XPATH_XPATHFACTORY_H__
#define __ELASTOSX_XML_XPATH_XPATHFACTORY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

namespace Elastosx {
namespace Xml {
namespace XPath {

EXTERN const _ELASTOS ClassID ECLSID_XPathFactory;

class XPathFactory
    : public Object
    , public IXPathFactory
{
protected:
    XPathFactory(){}

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    static CARAPI NewInstance(
        /* [out] */ IXPathFactory** instance);

    static CARAPI NewInstance(
        /* [in] */ const String& uri,
        /* [out] */ IXPathFactory** instance);

    virtual CARAPI IsObjectModelSupported(
        /* [in] */ const String& objectModel,
        /* [out] */ Boolean* value) = 0;

    virtual CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value) = 0;

    virtual CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* value) = 0;

    virtual CARAPI SetXPathVariableResolver(
        /* [in] */ IXPathVariableResolver* resolver) = 0;

    virtual CARAPI NewXPath(
        /* [out] */IXPath** xpath) = 0;
public:
    /**
     * <p>The default property name according to the JAXP spec.</p>
     */
    const static String DEFAULT_PROPERTY_NAME;

    /**
     * <p>Default Object Model URI.</p>
     */
    const static String DEFAULT_OBJECT_MODEL_URI;

};

}// namespace XPath
}// namespace Xml
}// namespace Elastosx

#endif //__ELASTOSX_XML_XPATH_XPATHFACTORY_H__