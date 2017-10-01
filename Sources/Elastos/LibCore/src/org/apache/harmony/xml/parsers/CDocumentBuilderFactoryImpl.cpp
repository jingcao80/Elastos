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
#include "org/apache/harmony/xml/parsers/CDocumentBuilderFactoryImpl.h"
#include "org/apache/harmony/xml/parsers/DocumentBuilderImpl.h"
#include <elastos/utility/logging/Logger.h>

using Elastosx::Xml::Parsers::IDocumentBuilder;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

const String CDocumentBuilderFactoryImpl::NAMESPACES("http://xml.org/sax/features/namespaces");
const String CDocumentBuilderFactoryImpl::VALIDATION("http://xml.org/sax/features/validation");

ECode CDocumentBuilderFactoryImpl::NewDocumentBuilder(
    /* [out] */ Elastosx::Xml::Parsers::IDocumentBuilder ** ppBuilder)
{
    VALIDATE_NOT_NULL(ppBuilder);
    ppBuilder = NULL;

    Boolean isValidating;
    if (IsValidating(&isValidating), isValidating) {
        //throw new ParserConfigurationException("No validating DocumentBuilder implementation available");
        Logger::E("CDocumentBuilderFactoryImpl", "New ParserConfigurationException");
        assert(0);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    /**
     * TODO If Android is going to support a different DocumentBuilder
     * implementations, this should be wired here. If we wanted to
     * allow different implementations, these could be distinguished by
     * a special feature (like http://www.org.apache.harmony.com/xml/expat)
     * or by throwing the full SPI monty at it.
     */
    AutoPtr<DocumentBuilderImpl> builder = new DocumentBuilderImpl();
    Boolean result;
    builder->SetCoalescing((IsCoalescing(&result), result));
    builder->SetIgnoreComments((IsIgnoringComments(&result), result));
    builder->SetIgnoreElementContentWhitespace((IsIgnoringElementContentWhitespace(&result), result));
    builder->SetNamespaceAware((IsNamespaceAware(&result), result));

    // TODO What about expandEntityReferences?

    *ppBuilder = builder;
    REFCOUNT_ADD(*ppBuilder);
    return NOERROR;
}

ECode CDocumentBuilderFactoryImpl::SetAttribute(
    /* [in] */ const String& name,
    /* [in] */ IObject * pValue)
{
    Logger::E("CDocumentBuilderFactoryImpl", "SetAttribute not support, name:%s", name.string());
    assert(0);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CDocumentBuilderFactoryImpl::GetAttribute(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** ppAttri)
{
    Logger::E("CDocumentBuilderFactoryImpl", "GetAttribute not support, name:%s", name.string());
    assert(0);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CDocumentBuilderFactoryImpl::SetFeature(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    if (name.IsNull()) {
        //throw new NullPointerException("name == null");
        Logger::E("CDocumentBuilderFactoryImpl", "SetFeature,name is null");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    if (NAMESPACES.Equals(name)) {
        SetNamespaceAware(value);
    } else if (VALIDATION.Equals(name)) {
        SetValidating(value);
    } else {
        //throw new ParserConfigurationException(name);
        Logger::E("CDocumentBuilderFactoryImpl", "Set ParserConfigurationException name: %s", name.string());
        assert(0);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode CDocumentBuilderFactoryImpl::GetFeature(
    /* [in] */ const String& name,
    /* [out] */ Boolean * pFeature)
{
    VALIDATE_NOT_NULL(pFeature);
    *pFeature = FALSE;
    if (name.IsNull()) {
        //throw new NullPointerException("name == null");
        Logger::E("CDocumentBuilderFactoryImpl", "name is null");
        assert(0);
        return E_NULL_POINTER_EXCEPTION;
    }

    if (NAMESPACES.Equals(name)) {
        return IsNamespaceAware(pFeature);
    } else if (VALIDATION.Equals(name)) {
        return IsValidating(pFeature);
    } else {
        //throw new ParserConfigurationException(name);
        Logger::E("CDocumentBuilderFactoryImpl", "ParserConfigurationException name: %s", name.string());
        assert(0);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
}

}
}
}
}
}
