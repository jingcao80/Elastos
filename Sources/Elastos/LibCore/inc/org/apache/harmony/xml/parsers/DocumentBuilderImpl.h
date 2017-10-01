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

#ifndef _ORG_APACHE_HARMONY_XML_PARSERS_DOCUMENTBUILDERIMPL_H__
#define _ORG_APACHE_HARMONY_XML_PARSERS_DOCUMENTBUILDERIMPL_H__

#include "org/apache/harmony/xml/dom/CDOMImplementationImplHelper.h"
#include "elastosx/xml/parsers/DocumentBuilder.h"

//import java.io.IOException;
//import java.net.URL;
//import java.net.URLConnection;
//import javax.xml.parsers.DocumentBuilder;
using Elastosx::Xml::Parsers::DocumentBuilder;
//import libcore.io.IoUtils;
//import org.apache.harmony.xml.dom.CDATASectionImpl;
//import org.apache.harmony.xml.dom.DOMImplementationImpl;
using Org::W3c::Dom::IDOMImplementation;
using Org::Apache::Harmony::Xml::Dom::CDOMImplementationImplHelper;
//using Org::Apache::Harmony::Xml::Dom::IDocumentImpl;
//import org.apache.harmony.xml.dom.DocumentTypeImpl;
//import org.apache.harmony.xml.dom.TextImpl;
using Org::Kxml2::IO::IKXmlParser;
//import org.w3c.dom.Attr;
//import org.w3c.dom.DOMImplementation;
//import org.w3c.dom.Document;
//import org.w3c.dom.DocumentType;
//import org.w3c.dom.Element;
using Org::W3c::Dom::INode;
//import org.w3c.dom.Text;
//import org.xml.sax.EntityResolver;
using Org::Xml::Sax::IEntityResolver;
//import org.xml.sax.ErrorHandler;
using Org::Xml::Sax::IErrorHandler;
//import org.xml.sax.InputSource;
using Org::Xml::Sax::IInputSource;
//import org.xml.sax.SAXException;
//import org.xml.sax.SAXParseException;
//import org.xml.sax.helpers.LocatorImpl;
//import org.xmlpull.v1.XmlPullParser;
//import org.xmlpull.v1.XmlPullParserException;

/**
 * Builds a DOM using KXmlParser.
 */
namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {
namespace Parsers {

class DocumentBuilderImpl
    : public DocumentBuilder
{
public:
    DocumentBuilderImpl();

    CARAPI Reset();

    CARAPI GetDOMImplementation(
        /* [out] */ IDOMImplementation** impl);

    CARAPI IsNamespaceAware(
        /* [out] */ Boolean* result);

    CARAPI IsValidating(
        /* [out] */ Boolean* result);

    CARAPI NewDocument(
        /* [out] */ IDocument** doc);

    CARAPI Parse(
        /* [in] */ IInputSource* source,
        /* [out] */ IDocument** doc);

    CARAPI SetEntityResolver(
        /* [in] */ IEntityResolver* resolver);

    CARAPI SetErrorHandler(
        /* [in] */ IErrorHandler* handler);

    /**
     * Controls whether this DocumentBuilder ignores comments.
     */
    CARAPI SetIgnoreComments(
        /* [in] */ Boolean value);

    CARAPI SetCoalescing(
        /* [in] */ Boolean value);

    /**
     * Controls whether this DocumentBuilder ignores element content whitespace.
     */
    CARAPI SetIgnoreElementContentWhitespace(
        /* [in] */ Boolean value);

    /**
     * Controls whether this DocumentBuilder is namespace-aware.
     */
    CARAPI SetNamespaceAware(
        /* [in] */ Boolean value);

    using DocumentBuilder::Parse;

private:
    /**
     * Implements the whole parsing of the XML document. The XML pull parser is
     * actually more of a tokenizer, and we are doing a classical recursive
     * descent parsing (the method invokes itself for XML elements). Our
     * approach to parsing does accept some illegal documents (more than one
     * root element, for example). The assumption is that the DOM implementation
     * throws the proper exceptions in these cases.
     *
     * @param parser The XML pull parser we're reading from.
     * @param document The document we're building.
     * @param node The node we're currently on (initially the document itself).
     * @param endToken The token that will end this recursive call. Either
     *        XmlPullParser.END_DOCUMENT or XmlPullParser.END_TAG.
     *
     * @throws XmlPullParserException If a parsing error occurs.
     * @throws IOException If a general IO error occurs.
     */
    CARAPI_(void) Parse(
        /* [in] */ IKXmlParser* parser,
        /* [in] */ IDocument* document,
        /* [in] */ INode* node,
        /* [in] */ Int32 endToken);

    /**
     * @param token the XML pull parser token type, such as XmlPullParser.CDSECT
     *      or XmlPullParser.ENTITY_REF.
     */
    CARAPI_(void) AppendText(
        /* [in] */ IDocument* document,
        /* [in] */ INode* parent,
        /* [in] */ Int32 token,
        /* [in] */ const String& text);

    /**
     * Returns the replacement text or null if {@code entity} isn't predefined.
     */
    CARAPI_(String) ResolvePredefinedOrCharacterEntity(
        /* [in] */ const String& entityName);

    CARAPI_(String) ResolveCharacterReference(
        /* [in] */ const String& value,
        /* [in] */ Int32 base);

private:
    static AutoPtr<IDOMImplementation> dom;// = DOMImplementationImpl.getInstance();

    Boolean coalescing;
    AutoPtr<IEntityResolver> entityResolver;
    AutoPtr<IErrorHandler> errorHandler;
    Boolean ignoreComments;
    Boolean ignoreElementContentWhitespace;
    Boolean namespaceAware;
    // adding a new field? don't forget to update reset().

};

} // Parsers
} // Xml
} // Harmony
} // Apache
} // Org

#endif // _ORG_APACHE_HARMONY_XML_PARSERS_DOCUMENTBUILDERIMPL_H__
