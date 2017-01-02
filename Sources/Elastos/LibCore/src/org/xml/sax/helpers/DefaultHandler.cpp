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

#include "DefaultHandler.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CAR_INTERFACE_IMPL_4(DefaultHandler, Object, IEntityResolver, IDTDHandler, IContentHandler, IErrorHandler)

ECode DefaultHandler::ResolveEntity(
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [out] */ IInputSource** src)
{
    VALIDATE_NOT_NULL(src);

    *src = NULL;

    return NOERROR;
}

ECode DefaultHandler::NotationDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId)
{
    return NOERROR;
}

ECode DefaultHandler::UnparsedEntityDecl(
    /* [in] */ const String& name,
    /* [in] */ const String& publicId,
    /* [in] */ const String& systemId,
    /* [in] */ const String& notationName)
{
    return NOERROR;
}

ECode DefaultHandler::SetDocumentLocator(
    /* [in] */ ILocator* locator)
{
    return NOERROR;
}

ECode DefaultHandler::StartDocument()
{
    return NOERROR;
}

ECode DefaultHandler::EndDocument()
{
    return NOERROR;
}

ECode DefaultHandler::StartPrefixMapping(
    /* [in] */ const String& prefix,
    /* [in] */ const String& uri)
{
    return NOERROR;
}

ECode DefaultHandler::EndPrefixMapping(
    /* [in] */ const String& prefix)
{
    return NOERROR;
}

ECode DefaultHandler::StartElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ IAttributes* atts)
{
    return NOERROR;
}

ECode DefaultHandler::EndElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName)
{
    return NOERROR;
}

ECode DefaultHandler::Characters(
    /* [in] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    return NOERROR;
}

ECode DefaultHandler::IgnorableWhitespace(
    /* [out] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    return NOERROR;
}

ECode DefaultHandler::ProcessingInstruction(
    /* [in] */ const String& target,
    /* [in] */ const String& data)
{
    return NOERROR;
}

ECode DefaultHandler::SkippedEntity(
    /* [in] */ const String& name)
{
    return NOERROR;
}

ECode DefaultHandler::Warning(
    /* [in] */ ECode exception)
{
    return NOERROR;
}

ECode DefaultHandler::Error(
    /* [in] */ ECode exception)
{
    return NOERROR;
}

ECode DefaultHandler::FatalError(
    /* [in] */ ECode exception)
{
    return E_SAX_PARSE_EXCEPTION;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
