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

#include "Elastos.CoreLibrary.External.h"
#include "org/apache/harmony/xml/parsers/CSAXParserFactoryImpl.h"
#include "org/apache/harmony/xml/parsers/CSAXParserImpl.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/CHashMap.h"
#include <elastos/utility/logging/Logger.h>

using Elastosx::Xml::Parsers::ISAXParser;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

const String CSAXParserFactoryImpl::NAMESPACES("http://xml.org/sax/features/namespaces");
const String CSAXParserFactoryImpl::VALIDATION("http://xml.org/sax/features/validation");

CSAXParserFactoryImpl::CSAXParserFactoryImpl()
{
    CHashMap::New((IMap**)&features);
}

ECode CSAXParserFactoryImpl::NewSAXParser(
    /* [out] */ Elastosx::Xml::Parsers::ISAXParser ** ppParser)
{
    VALIDATE_NOT_NULL(ppParser);
    *ppParser = NULL;

    Boolean isValidating;
    if (IsValidating(&isValidating), isValidating) {
        //throw new ParserConfigurationException("No validating SAXParser implementation available");
        Logger::E("CSAXParserFactoryImpl", "No validating SAXParser implementation available");
        assert(0);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    AutoPtr<ISAXParser> saxParser;
    //try {
    CSAXParserImpl::New(features, (ISAXParser**)&saxParser);
    *ppParser = saxParser;
    REFCOUNT_ADD(*ppParser);
    //} catch (Exception ex) {
    //    throw new ParserConfigurationException(ex.toString());
    //}
    return NOERROR;
}

ECode CSAXParserFactoryImpl::SetNamespaceAware(
    /* [in] */ Boolean awareness)
{
    //try {
    return SetFeature(NAMESPACES, awareness);
    //} catch (SAXNotRecognizedException ex) {
    //    throw new AssertionError(ex);
    //}
}

ECode CSAXParserFactoryImpl::SetValidating(
    /* [in] */ Boolean validating)
{
    //try {
    return SetFeature(VALIDATION, validating);
    //} catch (SAXNotRecognizedException ex) {
    //    throw new AssertionError(ex);
    //}
}

ECode CSAXParserFactoryImpl::IsNamespaceAware(
    /* [out] */ Boolean * pIsAware)
{
    //try {
    return GetFeature(NAMESPACES, pIsAware);
    //} catch (SAXNotRecognizedException ex) {
    //    throw new AssertionError(ex);
    //}
}

ECode CSAXParserFactoryImpl::IsValidating(
    /* [out] */ Boolean * pIsValidating)
{
    //try {
    return GetFeature(VALIDATION, pIsValidating);
    //} catch (SAXNotRecognizedException ex) {
    //    throw new AssertionError(ex);
    //}
}

ECode CSAXParserFactoryImpl::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    if (name.IsNull()) {
        //throw new NullPointerException("name == null");
        Logger::E("CSAXParserFactoryImpl", "name == null");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    if (!name.StartWith("http://xml.org/sax/features/")) {
        //throw new SAXNotRecognizedException(name);
        Logger::E("CSAXParserFactoryImpl", "SAXNotRecognizedException, name:%s", name.string());
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    if (value) {
        features->Put(CoreUtils::Convert(name), CoreUtils::Convert(TRUE));
    } else {
        // This is needed to disable features that are enabled by default.
        features->Put(CoreUtils::Convert(name), CoreUtils::Convert(FALSE));
    }
    return NOERROR;
}

ECode CSAXParserFactoryImpl::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean * pFeature)
{
    VALIDATE_NOT_NULL(pFeature);
    *pFeature = FALSE;

    if (name.IsNull()) {
        //throw new NullPointerException("name == null");
        Logger::E("CSAXParserFactoryImpl", "name == null");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    if (!name.StartWith("http://xml.org/sax/features/")) {
        //throw new SAXNotRecognizedException(name);
        Logger::E("CSAXParserFactoryImpl", "Get SAXNotRecognizedException, name:%s", name.string());
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IInterface> value;
    features->Get(CoreUtils::Convert(name), (IInterface**)&value);
    IBoolean* result = IBoolean::Probe(value);
    if (result == NULL) {
        Logger::E("CSAXParserFactoryImpl", "Get result is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    else {
        result->GetValue(pFeature);
    }
    return NOERROR;
}

}
}
}
}
}

