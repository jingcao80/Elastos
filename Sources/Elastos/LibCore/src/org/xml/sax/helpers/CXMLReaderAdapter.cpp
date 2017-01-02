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

#include "CXMLReaderAdapter.h"
#include "CXMLReaderFactory.h"
#include "CInputSource.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CAR_INTERFACE_IMPL_2(CXMLReaderAdapter, Object, IParser, IContentHandler)

CAR_OBJECT_IMPL(CXMLReaderAdapter)

ECode CXMLReaderAdapter::SetLocale(
    /* [in] */ ILocale* locale)
{
    // throw new SAXNotSupportedException("setLocale not supported");
    return E_SAX_NOT_SUPPORTED_EXCEPTION;
}

ECode CXMLReaderAdapter::SetEntityResolver(
    /* [in] */ IEntityResolver* resolver)
{
    mXmlReader->SetEntityResolver(resolver);
    return NOERROR;
}

ECode CXMLReaderAdapter::SetDTDHandler(
    /* [in] */ IDTDHandler* handler)
{
    return mXmlReader->SetDTDHandler(handler);
}

ECode CXMLReaderAdapter::SetDocumentHandler(
    /* [in] */ IDocumentHandler* handler)
{
    mDocumentHandler = handler;

    return NOERROR;
}

ECode CXMLReaderAdapter::SetErrorHandler(
    /* [in] */ IErrorHandler* handler)
{
    return mXmlReader->SetErrorHandler(handler);
}

ECode CXMLReaderAdapter::Parse(
    /* [in] */ IInputSource* source)
{
    SetupXMLReader();

    return mXmlReader->Parse(source);
}

ECode CXMLReaderAdapter::Parse(
    /* [in] */ const String& systemId)
{
    AutoPtr<IInputSource> source;

    CInputSource::New(systemId, (IInputSource**)&source);

    return Parse(source);
}

ECode CXMLReaderAdapter::SetDocumentLocator(
    /* [in] */ ILocator* locator)
{
    if (mDocumentHandler != NULL){
        mDocumentHandler->SetDocumentLocator(locator);
    }

    return NOERROR;
}

ECode CXMLReaderAdapter::StartDocument()
{
    if (mDocumentHandler != NULL) {
        mDocumentHandler->StartDocument();
    }

    return NOERROR;
}

ECode CXMLReaderAdapter::EndDocument()
{
    if (mDocumentHandler != NULL) {
        mDocumentHandler->EndDocument();
    }

    return NOERROR;
}

ECode CXMLReaderAdapter::StartPrefixMapping(
    /* [in] */ const String& prefix,
    /* [in] */ const String& uri)
{
    return NOERROR;
}

ECode CXMLReaderAdapter::EndPrefixMapping(
    /* [in] */ const String& prefix)
{
    return NOERROR;
}

ECode CXMLReaderAdapter::StartElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName,
    /* [in] */ IAttributes* atts)
{
    if (mDocumentHandler != NULL) {
        mQAtts->SetAttributes(atts);
        mDocumentHandler->StartElement(qName, mQAtts);
    }

    return NOERROR;
}

ECode CXMLReaderAdapter::EndElement(
    /* [in] */ const String& uri,
    /* [in] */ const String& localName,
    /* [in] */ const String& qName)
{
    if (mDocumentHandler != NULL) {
        mDocumentHandler->EndElement(qName);
    }

    return NOERROR;
}

ECode CXMLReaderAdapter::Characters(
    /* [in] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    VALIDATE_NOT_NULL(ch);

    if (mDocumentHandler != NULL) {
        mDocumentHandler->Characters(ch, start, length);
    }

    return NOERROR;
}

ECode CXMLReaderAdapter::IgnorableWhitespace(
    /* [in] */ ArrayOf<Char32>* ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    VALIDATE_NOT_NULL(ch);

    if (mDocumentHandler != NULL) {
        mDocumentHandler->IgnorableWhitespace(ch, start, length);
    }

    return NOERROR;
}

ECode CXMLReaderAdapter::ProcessingInstruction(
    /* [in] */ const String& target,
    /* [in] */ const String& data)
{
    if (mDocumentHandler != NULL) {
        mDocumentHandler->ProcessingInstruction(target, data);
    }

    return NOERROR;
}

ECode CXMLReaderAdapter::SkippedEntity(
    /* [in] */ const String& name)
{
    return NOERROR;
}

ECode CXMLReaderAdapter::constructor()
{
    AutoPtr<IXMLReader> xmlReader;

    XMLReaderFactory::CreateXMLReader((IXMLReader**)(&xmlReader));

    return Setup(xmlReader);
}

ECode CXMLReaderAdapter::constructor(
    /* [in] */ IXMLReader* xmlReader)
{
    return Setup(xmlReader);
}

ECode CXMLReaderAdapter::Setup(
    /* [in] */ IXMLReader* xmlReader)
{
    if (xmlReader == NULL) {
        //throw new NULLPointerException("XMLReader must not be NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    mXmlReader = xmlReader;
    mQAtts = new AttributesAdapter();
    return NOERROR;
}

ECode CXMLReaderAdapter::SetupXMLReader()
{
    mXmlReader->SetFeature(String("http://xml.org/sax/features/namespace-prefixes"), TRUE);
//    try {
        mXmlReader->SetFeature(String("http://xml.org/sax/features/namespaces"), FALSE);
//    } catch (SAXException e) {
//        // NO OP: it's just extra information, and we can ignore it
//    }

    mXmlReader->SetContentHandler(this);

    return NOERROR;
}

//===================================================================
// AttributesAdapter
//===================================================================

CAR_INTERFACE_IMPL(AttributesAdapter, Object, IAttributeList)

ECode AttributesAdapter::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);

    return mAttributes->GetLength(length);
}

ECode AttributesAdapter::GetName(
    /* [in] */ Int32 i,
    /* [out] */ String* name)
{
    return mAttributes->GetQName(i, name);
}

ECode AttributesAdapter::GetType(
    /* [in] */ Int32 i,
    /* [out] */ String* type)
{
    return mAttributes->GetType(i, type);
}

ECode AttributesAdapter::GetValue(
    /* [in] */ Int32 i,
    /* [out] */ String* value)
{
    return mAttributes->GetValue(i, value);
}

ECode AttributesAdapter::GetType(
    /* [in] */ const String& qName,
    /* [out] */ String* type)
{
    return mAttributes->GetType(qName, type);
}

ECode AttributesAdapter::GetValue(
    /* [in] */ const String& qName,
    /* [out] */ String* value)
{
    return mAttributes->GetValue(qName, value);
}

ECode AttributesAdapter::SetAttributes (
        /* [in] */IAttributes* attributes)
{
    mAttributes = attributes;

    return NOERROR;
}

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org
