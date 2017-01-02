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

#ifndef __ORG_XMLPULL_V1_SAX2_CDRIVER_H__
#define __ORG_XMLPULL_V1_SAX2_CDRIVER_H__

#include "_Org_Xmlpull_V1_Sax2_CDriver.h"
#include <elastos/core/Object.h>

using Org::Xml::Sax::ILocator;
using Org::Xml::Sax::IXMLReader;
using Org::Xml::Sax::IEntityResolver;
using Org::Xml::Sax::IDTDHandler;
using Org::Xml::Sax::IContentHandler;
using Org::Xml::Sax::IErrorHandler;
using Org::Xml::Sax::IInputSource;
using Org::Xml::Sax::IAttributes;

namespace Org {
namespace Xmlpull {
namespace V1 {
namespace Sax2 {

/**
 * SAX2 Driver that pulls events from XmlPullParser
 * and comverts them into SAX2 callbacks.
 *
 * @author <a href="http://www.extreme.indiana.edu/~aslom/">Aleksander Slominski</a>
 */
CarClass(CDriver)
    , public Object
    , public ILocator
    , public IXMLReader
    , public IAttributes
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IXmlPullParser* pp);

    CARAPI GetPublicId(
        /* [out] */ String* id);

    CARAPI GetSystemId(
        /* [out] */ String* id);

    CARAPI GetLineNumber(
        /* [out] */ Int32* lineNumber);

    CARAPI GetColumnNumber(
        /* [out] */ Int32* columnNumber);

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* feature);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface** value);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value);

    CARAPI SetEntityResolver(
        /* [in] */ IEntityResolver* resolver);

    CARAPI GetEntityResolver(
        /* [out] */ IEntityResolver** resolver);

    CARAPI SetDTDHandler(
        /* [in] */ IDTDHandler* handler);

    CARAPI GetDTDHandler(
        /* [out] */ IDTDHandler** handler);

    CARAPI SetContentHandler(
        /* [in] */ IContentHandler* handler);

    CARAPI GetContentHandler(
        /* [out] */ IContentHandler** handler);

    CARAPI SetErrorHandler(
        /* [in] */ IErrorHandler* handler);

    CARAPI GetErrorHandler(
        /* [out] */ IErrorHandler** handler);

    CARAPI Parse(
        /* [in] */ IInputSource* source);

    CARAPI Parse(
        /* [in] */ const String& systemId);

    CARAPI GetLength(
        /* [out] */ Int32* length);

    CARAPI GetURI(
        /* [in] */ Int32 index,
        /* [out] */ String* URI);

    CARAPI GetLocalName(
        /* [in] */ Int32 index,
        /* [out] */ String* localName);

    CARAPI GetQName(
        /* [in] */ Int32 index,
        /* [out] */ String* qName);

    CARAPI GetType(
        /* [in] */ Int32 index,
        /* [out] */ String* type);

    CARAPI GetValue(
        /* [in] */ Int32 index,
        /* [out] */ String* value);

    CARAPI GetIndex(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ Int32* index);

    CARAPI GetIndex(
        /* [in] */ const String& qName,
        /* [out] */ Int32* index);

    CARAPI GetType(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ String* type);

    CARAPI GetType(
        /* [in] */ const String& qName,
        /* [out] */ String* type);

    CARAPI GetValue(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ String* value);

    CARAPI GetValue(
        /* [in] */ const String& qName,
        /* [out] */ String* value);

    CARAPI_(void) ParseSubTree(
        /* [in] */ IXmlPullParser* pp);

protected:
    CARAPI_(void) StartElement(
        /* [in] */ const String& namesp,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName);

protected:
    static const String DECLARATION_HANDLER_PROPERTY; // = "http://xml.org/sax/properties/declaration-handler";

    static const String LEXICAL_HANDLER_PROPERTY; // = "http://xml.org/sax/properties/lexical-handler";

    static const String NAMESPACES_FEATURE; // = "http://xml.org/sax/features/namespaces";

    static const String NAMESPACE_PREFIXES_FEATURE; // = "http://xml.org/sax/features/namespace-prefixes";

    static const String VALIDATION_FEATURE; // = "http://xml.org/sax/features/validation";

    static const String APACHE_SCHEMA_VALIDATION_FEATURE; // = "http://apache.org/xml/features/validation/schema";

    static const String APACHE_DYNAMIC_VALIDATION_FEATURE; // = "http://apache.org/xml/features/validation/dynamic";

    AutoPtr<IContentHandler> mContentHandler; // = new DefaultHandler();
    AutoPtr<IErrorHandler> mErrorHandler; // = new DefaultHandler();

    String mSystemId;

    AutoPtr<IXmlPullParser> mPp;
};

} // namespace Sax2
} // namespace V1
} // namespace Xmlpull
} // namespace Org

#endif // __ORG_XMLPULL_V1_SAX2_CDRIVER_H__
