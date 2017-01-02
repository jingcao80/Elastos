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

#ifndef __ELASTOSX_XML_XPATH_CXPATHCONSTANTS_H__
#define __ELASTOSX_XML_XPATH_CXPATHCONSTANTS_H__

#include "Object.h"
#include "_Elastosx_Xml_XPath_CXPathConstants.h"

using Elastos::Core::Object;
using Elastosx::Xml::Namespace::IQName;

namespace Elastosx {
namespace Xml {
namespace XPath {

CarClass(CXPathConstants)
    , public Object
    , public IXPathConstants
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetQNameNUMBER(
        /* [out] */ IQName** qName);

    CARAPI GetQNameSTRING(
        /* [out] */ IQName** qName);

    CARAPI GetQNameBOOLEAN(
        /* [out] */ IQName** qName);

    CARAPI GetQNameNODESET(
        /* [out] */ IQName** qName);

    CARAPI GetQNameNODE(
        /* [out] */ IQName** qName);

public:
    static const AutoPtr<IQName> NUMBER;
    static const AutoPtr<IQName> STRING;
    static const AutoPtr<IQName> BOOLEAN;
    static const AutoPtr<IQName> NODESET;
    static const AutoPtr<IQName> NODE;
};

}// namespace XPath
}// namespace Xml
}// namespace Elastosx

#endif // __ELASTOSX_XML_XPATH_CXPATHCONSTANTS_H__
