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

#ifndef __ORG_XML_SAX_HELPERS_CXMLREADERADAPTER_H__
#define __ORG_XML_SAX_HELPERS_CXMLREADERADAPTER_H__

#include "_Org_Xml_Sax_Helpers_CXMLReaderAdapter.h"
#include "Object.h"

using Elastos::Utility::ILocale;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

class AttributesAdapter;

CarClass(CXMLReaderAdapter)
    , public Object
    , public IParser
    , public IContentHandler
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Create a new adapter.
     *
     * <p>Use the "org.xml.sax.driver" property to locate the SAX2
     * driver to embed.</p>
     *
     * @exception org.xml.sax.SAXException If the embedded driver
     *            cannot be instantiated or if the
     *            org.xml.sax.driver property is not specified.
     */
    CARAPI constructor();

    /**
     * Create a new adapter.
     *
     * <p>Create a new adapter, wrapped around a SAX2 XMLReader.
     * The adapter will make the XMLReader act like a SAX1
     * Parser.</p>
     *
     * @param xmlReader The SAX2 XMLReader to wrap.
     * @exception java.lang.NullPointerException If the argument is null.
     */
    CARAPI constructor(
        /* [in] */ IXMLReader* xmlReader);

    /**
     * Set the locale for error reporting.
     *
     * <p>This is not supported in SAX2, and will always throw
     * an exception.</p>
     *
     * @param locale the locale for error reporting.
     * @see org.xml.sax.Parser#setLocale
     * @exception org.xml.sax.SAXException Thrown unless overridden.
     */
    CARAPI SetLocale(
        /* [in] */ ILocale* locale);

    /**
     * Register the entity resolver.
     *
     * @param resolver The new resolver.
     * @see org.xml.sax.Parser#setEntityResolver
     */
    CARAPI SetEntityResolver(
        /* [in] */ IEntityResolver* resolver);

    /**
     * Register the DTD event handler.
     *
     * @param handler The new DTD event handler.
     * @see org.xml.sax.Parser#setDTDHandler
     */
    CARAPI SetDTDHandler(
        /* [in] */ IDTDHandler* handler);

    /**
     * Register the SAX1 document event handler.
     *
     * <p>Note that the SAX1 document handler has no Namespace
     * support.</p>
     *
     * @param handler The new SAX1 document event handler.
     * @see org.xml.sax.Parser#setDocumentHandler
     */
    CARAPI SetDocumentHandler(
        /* [in] */ IDocumentHandler* handler);

    /**
     * Register the error event handler.
     *
     * @param handler The new error event handler.
     * @see org.xml.sax.Parser#setErrorHandler
     */
    CARAPI SetErrorHandler(
        /* [in] */ IErrorHandler* handler);

    /**
     * Parse the document.
     *
     * <p>This method will throw an exception if the embedded
     * XMLReader does not support the
     * http://xml.org/sax/features/namespace-prefixes property.</p>
     *
     * @param input An input source for the document.
     * @exception java.io.IOException If there is a problem reading
     *            the raw content of the document.
     * @exception org.xml.sax.SAXException If there is a problem
     *            processing the document.
     * @see #parse(java.lang.String)
     * @see org.xml.sax.Parser#parse(org.xml.sax.InputSource)
     */
    CARAPI Parse(
        /* [in] */ IInputSource* source);

    /**
     * Parse the document.
     *
     * <p>This method will throw an exception if the embedded
     * XMLReader does not support the
     * http://xml.org/sax/features/namespace-prefixes property.</p>
     *
     * @param systemId The absolute URL of the document.
     * @exception java.io.IOException If there is a problem reading
     *            the raw content of the document.
     * @exception org.xml.sax.SAXException If there is a problem
     *            processing the document.
     * @see #parse(org.xml.sax.InputSource)
     * @see org.xml.sax.Parser#parse(java.lang.String)
     */
    CARAPI Parse(
        /* [in] */ const String& systemId);

    ////////////////////////////////////////////////////////////////////
    // Implementation of org.xml.sax.ContentHandler.
    ////////////////////////////////////////////////////////////////////


    /**
     * Set a document locator.
     *
     * @param locator The document locator.
     * @see org.xml.sax.ContentHandler#setDocumentLocator
     */
    CARAPI SetDocumentLocator(
        /* [in] */ ILocator* locator);

    /**
     * Start document event.
     *
     * @exception org.xml.sax.SAXException The client may raise a
     *            processing exception.
     * @see org.xml.sax.ContentHandler#startDocument
     */
    CARAPI StartDocument();

    /**
     * End document event.
     *
     * @exception org.xml.sax.SAXException The client may raise a
     *            processing exception.
     * @see org.xml.sax.ContentHandler#endDocument
     */
    CARAPI EndDocument();

    /**
     * Adapt a SAX2 start prefix mapping event.
     *
     * @param prefix The prefix being mapped.
     * @param uri The Namespace URI being mapped to.
     * @see org.xml.sax.ContentHandler#startPrefixMapping
     */
    CARAPI StartPrefixMapping(
        /* [in] */ const String& prefix,
        /* [in] */ const String& uri);

    /**
     * Adapt a SAX2 end prefix mapping event.
     *
     * @param prefix The prefix being mapped.
     * @see org.xml.sax.ContentHandler#endPrefixMapping
     */
    CARAPI EndPrefixMapping(
        /* [in] */ const String& prefix);

    /**
     * Adapt a SAX2 start element event.
     *
     * @param uri The Namespace URI.
     * @param localName The Namespace local name.
     * @param qName The qualified (prefixed) name.
     * @param atts The SAX2 attributes.
     * @exception org.xml.sax.SAXException The client may raise a
     *            processing exception.
     * @see org.xml.sax.ContentHandler#endDocument
     */
    CARAPI StartElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName,
        /* [in] */ IAttributes* atts);

    /**
     * Adapt a SAX2 end element event.
     *
     * @param uri The Namespace URI.
     * @param localName The Namespace local name.
     * @param qName The qualified (prefixed) name.
     * @exception org.xml.sax.SAXException The client may raise a
     *            processing exception.
     * @see org.xml.sax.ContentHandler#endElement
     */
    CARAPI EndElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName);

    /**
     * Adapt a SAX2 characters event.
     *
     * @param ch An array of characters.
     * @param start The starting position in the array.
     * @param length The number of characters to use.
     * @exception org.xml.sax.SAXException The client may raise a
     *            processing exception.
     * @see org.xml.sax.ContentHandler#characters
     */
    CARAPI Characters(
        /* [out] */ ArrayOf<Char32>* ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

    /**
     * Adapt a SAX2 ignorable whitespace event.
     *
     * @param ch An array of characters.
     * @param start The starting position in the array.
     * @param length The number of characters to use.
     * @exception org.xml.sax.SAXException The client may raise a
     *            processing exception.
     * @see org.xml.sax.ContentHandler#ignorableWhitespace
     */
    CARAPI IgnorableWhitespace(
        /* [out] */ ArrayOf<Char32>* ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

    /**
     * Adapt a SAX2 processing instruction event.
     *
     * @param target The processing instruction target.
     * @param data The remainder of the processing instruction
     * @exception org.xml.sax.SAXException The client may raise a
     *            processing exception.
     * @see org.xml.sax.ContentHandler#processingInstruction
     */
    CARAPI ProcessingInstruction(
        /* [in] */ const String& target,
        /* [in] */ const String& data);

    /**
     * Adapt a SAX2 skipped entity event.
     *
     * @param name The name of the skipped entity.
     * @see org.xml.sax.ContentHandler#skippedEntity
     * @exception org.xml.sax.SAXException Throwable by subclasses.
     */
    CARAPI SkippedEntity(
        /* [in] */ const String& name);

private:
    /**
     * Internal setup.
     *
     * @param xmlReader The embedded XMLReader.
     */
    CARAPI Setup (
        /* [in] */ IXMLReader* xmlReader);

    /**
     * Set up the XML reader.
     */
    CARAPI SetupXMLReader ();

private:
    AutoPtr<IXMLReader> mXmlReader;
    AutoPtr<IDocumentHandler> mDocumentHandler;
    AutoPtr<AttributesAdapter> mQAtts; // AttributesAdapter
};

class AttributesAdapter
    : public Object
    , public IAttributeList
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Return the number of attributes.
     *
     * @return The length of the attribute list.
     * @see org.xml.sax.AttributeList#getLength
     */
    CARAPI GetLength(
        /* [out] */ Int32* length);

    /**
     * Return the qualified (prefixed) name of an attribute by position.
     *
     * @return The qualified name.
     * @see org.xml.sax.AttributeList#getName
     */
    CARAPI GetName(
        /* [in] */ Int32 i,
        /* [out] */ String* name);

    /**
     * Return the type of an attribute by position.
     *
     * @return The type.
     * @see org.xml.sax.AttributeList#getType(int)
     */
    CARAPI GetType(
        /* [in] */ Int32 i,
        /* [out] */ String* type);

    /**
     * Return the value of an attribute by position.
     *
     * @return The value.
     * @see org.xml.sax.AttributeList#getValue(int)
     */
    CARAPI GetValue(
        /* [in] */ Int32 i,
        /* [out] */ String* value);

    /**
     * Return the type of an attribute by qualified (prefixed) name.
     *
     * @return The type.
     * @see org.xml.sax.AttributeList#getType(java.lang.String)
     */
    CARAPI GetType(
        /* [in] */ const String& name,
        /* [out] */ String* type);

    /**
     * Return the value of an attribute by qualified (prefixed) name.
     *
     * @return The value.
     * @see org.xml.sax.AttributeList#getValue(java.lang.String)
     */
    CARAPI GetValue(
        /* [in] */ const String& name,
        /* [out] */ String* value);

    /**
     * Set the embedded Attributes object.
     *
     * @param The embedded SAX2 Attributes.
     */
    CARAPI SetAttributes (
        /* [in] */IAttributes* attributes);

private:
    AutoPtr<IAttributes> mAttributes;
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_CXMLREADERADAPTER_H__
