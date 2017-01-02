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

#ifndef __ORG_XML_SAX_HELPERS_CPARSERADAPTER_H__
#define __ORG_XML_SAX_HELPERS_CPARSERADAPTER_H__

#include "_Org_Xml_Sax_Helpers_CParserAdapter.h"
#include "Object.h"

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

/**
 * Adapt a SAX1 Parser as a SAX2 XMLReader.
 *
 * <blockquote>
 * <em>This module, both source code and documentation, is in the
 * Public Domain, and comes with <strong>NO WARRANTY</strong>.</em>
 * See <a href='http://www.saxproject.org'>http://www.saxproject.org</a>
 * for further information.
 * </blockquote>
 *
 * <p>This class wraps a SAX1 {@link org.xml.sax.Parser Parser}
 * and makes it act as a SAX2 {@link org.xml.sax.XMLReader XMLReader},
 * with feature, property, and Namespace support.  Note
 * that it is not possible to report {@link org.xml.sax.ContentHandler#skippedEntity
 * skippedEntity} events, since SAX1 does not make that information available.</p>
 *
 * <p>This adapter does not test for duplicate Namespace-qualified
 * attribute names.</p>
 *
 * @since SAX 2.0
 * @author David Megginson
 * @version 2.0.1 (sax2r2)
 * @see org.xml.sax.helpers.XMLReaderAdapter
 * @see org.xml.sax.XMLReader
 * @see org.xml.sax.Parser
 */
CarClass(CParserAdapter)
    , public Object
    , public IXMLReader
    , public IDocumentHandler
{
public:
    class AttributeListAdapter;

public:
    CAR_INTERFACE_DECL()
    CAR_OBJECT_DECL()

    CParserAdapter();

    /**
     * Construct a new parser adapter.
     *
     * <p>Use the "org.xml.sax.parser" property to locate the
     * embedded SAX1 driver.</p>
     *
     * @exception SAXException If the embedded driver
     *            cannot be instantiated or if the
     *            org.xml.sax.parser property is not specified.
     */
    CARAPI constructor();

    /**
     * Construct a new parser adapter.
     *
     * <p>Note that the embedded parser cannot be changed once the
     * adapter is created; to embed a different parser, allocate
     * a new ParserAdapter.</p>
     *
     * @param parser The SAX1 parser to embed.
     * @exception java.lang.NullPointerException If the parser parameter
     *            is null.
     */
    CARAPI constructor(
        /* [in] */ IParser * parser);

    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean * feature);

    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface ** value);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface * value);

    CARAPI SetEntityResolver(
        /* [in] */ IEntityResolver * resolver);

    CARAPI GetEntityResolver(
        /* [out] */ IEntityResolver ** resolver);

    CARAPI SetDTDHandler(
        /* [in] */ IDTDHandler * handler);

    CARAPI GetDTDHandler(
        /* [out] */ IDTDHandler ** handler);

    CARAPI SetContentHandler(
        /* [in] */ IContentHandler * handler);

    CARAPI GetContentHandler(
        /* [out] */ IContentHandler ** handler);

    CARAPI SetErrorHandler(
        /* [in] */ IErrorHandler * handler);

    CARAPI GetErrorHandler(
        /* [out] */ IErrorHandler ** handler);

    CARAPI Parse(
        /* [in] */ IInputSource * input);

    CARAPI Parse(
        /* [in] */ const String& systemId);

    CARAPI SetDocumentLocator(
        /* [in] */ ILocator * locator);

    CARAPI StartDocument();

    CARAPI EndDocument();

    CARAPI StartElement(
        /* [in] */ const String& qName,
        /* [in] */ IAttributeList * qAtts);

    CARAPI EndElement(
        /* [in] */ const String& qName);

    CARAPI Characters(
        /* [in] */ ArrayOf<Char32>* ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

    CARAPI IgnorableWhitespace(
        /* [in] */ ArrayOf<Char32>* ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

    CARAPI ProcessingInstruction(
        /* [in] */ const String& target,
        /* [in] */ const String& data);

private:
    /**
     * Internal setup method.
     *
     * @param parser The embedded parser.
     * @exception java.lang.NullPointerException If the parser parameter
     *            is null.
     */
    CARAPI Setup(
        /* [in] */ IParser* parser);

    /**
     * Initialize the parser before each run.
     */
    CARAPI SetupParser();

    /**
     * Process a qualified (prefixed) name.
     *
     * <p>If the name has an undeclared prefix, use only the qname
     * and make an ErrorHandler.error callback in case the app is
     * interested.</p>
     *
     * @param qName The qualified (prefixed) name.
     * @param isAttribute true if this is an attribute name.
     * @return The name split into three parts.
     * @exception SAXException The client may throw
     *            an exception if there is an error callback.
     */
    CARAPI_(AutoPtr<ArrayOf<String> >) ProcessName(
        /* [in] */ const String& qName,
        /* [in] */ Boolean isAttribute,
        /* [in] */ Boolean useException);

    /**
     * Report a non-fatal error.
     *
     * @param message The error message.
     * @exception SAXException The client may throw
     *            an exception.
     */
    CARAPI_(void) ReportError(
        /* [in] */ const String& message);

    /**
     * Construct an exception for the current context.
     *
     * @param message The error message.
     */
    CARAPI MakeException(
        /* [in] */ const String& message);

    /**
     * Throw an exception if we are parsing.
     *
     * <p>Use this method to detect illegal feature or
     * property changes.</p>
     *
     * @param type The type of thing (feature or property).
     * @param name The feature or property name.
     * @exception SAXNotSupportedException If a
     *            document is currently being parsed.
     */
    CARAPI_(void) CheckNotParsing(
        /* [in] */ const String& type,
        /* [in] */ const String& name);

protected:
    // Properties

    // Handlers
    AutoPtr<ILocator> mLocator;

    AutoPtr<IEntityResolver> mEntityResolver;
    AutoPtr<IDTDHandler> mDtdHandler;
    AutoPtr<IContentHandler> mContentHandler;
    AutoPtr<IErrorHandler> mErrorHandler;

private:
    //
    // Internal constants for the sake of convenience.
    //
    const static String FEATURES;// = "http://xml.org/sax/features/";
    const static String NAMESPACES;// = FEATURES + "namespaces";
    const static String NAMESPACE_PREFIXES;// = FEATURES + "namespace-prefixes";
    const static String XMLNS_URIs;// = FEATURES + "xmlns-uris";

    ////////////////////////////////////////////////////////////////////
    // Internal state.
    ////////////////////////////////////////////////////////////////////

    AutoPtr<INamespaceSupport> mNsSupport;
    AutoPtr<AttributeListAdapter> mAttAdapter;

    AutoPtr< ArrayOf<String> > mNameParts; // = new String[3];

    AutoPtr<IParser> mParser;

    AutoPtr<IAttributesImpl> mAtts;

    Boolean mParsing; // = false;

    // Features
    Boolean mNamespaces; // = true;
    Boolean mPrefixes; // = false;
    Boolean mUris; // = false;

public:
    ////////////////////////////////////////////////////////////////////
    // Inner class to wrap an AttributeList when not doing NS proc.
    ////////////////////////////////////////////////////////////////////


    /**
     * Adapt a SAX1 AttributeList as a SAX2 Attributes object.
     *
     * <p>This class is in the Public Domain, and comes with NO
     * WARRANTY of any kind.</p>
     *
     * <p>This wrapper class is used only when Namespace support
     * is disabled -- it provides pretty much a direct mapping
     * from SAX1 to SAX2, except that names and types are
     * interned whenever requested.</p>
     */
    class AttributeListAdapter
        : public Object
        , public IAttributes
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Construct a new adapter.
         */
        AttributeListAdapter(
            /* [in] */ CParserAdapter* host);

        /**
         * Set the embedded AttributeList.
         *
         * <p>This method must be invoked before any of the others
         * can be used.</p>
         *
         * @param The SAX1 attribute list (with qnames).
         */
        CARAPI SetAttributeList(
            /* [in] */ IAttributeList* qAtts);

        /**
         * Return the length of the attribute list.
         *
         * @return The number of attributes in the list.
         * @see org.xml.sax.Attributes#getLength
         */
        CARAPI GetLength(
            /* [out] */ Int32* value);

        /**
         * Return the Namespace URI of the specified attribute.
         *
         * @param The attribute's index.
         * @return Always the empty string.
         * @see org.xml.sax.Attributes#getURI
         */
        CARAPI GetURI(
            /* [in] */ Int32 i,
            /* [out] */ String* str);

        /**
         * Return the local name of the specified attribute.
         *
         * @param The attribute's index.
         * @return Always the empty string.
         * @see org.xml.sax.Attributes#getLocalName
         */
        CARAPI GetLocalName(
            /* [in] */ Int32 i,
            /* [out] */ String* str);

        /**
         * Return the qualified (prefixed) name of the specified attribute.
         *
         * @param The attribute's index.
         * @return The attribute's qualified name, internalized.
         */
        CARAPI GetQName(
            /* [in] */ Int32 i,
            /* [out] */ String* str);

        /**
         * Return the type of the specified attribute.
         *
         * @param The attribute's index.
         * @return The attribute's type as an internalized string.
         */
        CARAPI GetType(
            /* [in] */ Int32 i,
            /* [out] */ String* str);

        /**
         * Return the value of the specified attribute.
         *
         * @param The attribute's index.
         * @return The attribute's value.
         */
        CARAPI GetValue(
            /* [in] */ Int32 i,
            /* [out] */ String* str);

        /**
         * Look up an attribute index by Namespace name.
         *
         * @param uri The Namespace URI or the empty string.
         * @param localName The local name.
         * @return The attributes index, or -1 if none was found.
         * @see org.xml.sax.Attributes#getIndex(java.lang.String,java.lang.String)
         */
        CARAPI GetIndex(
            /* [in] */ const String& uri,
            /* [in] */ const String& localName,
            /* [out] */ Int32* value);

        /**
         * Look up an attribute index by qualified (prefixed) name.
         *
         * @param qName The qualified name.
         * @return The attributes index, or -1 if none was found.
         * @see org.xml.sax.Attributes#getIndex(java.lang.String)
         */
        CARAPI GetIndex(
            /* [in] */ const String& qName,
            /* [out] */ Int32* value);

        /**
         * Look up the type of an attribute by Namespace name.
         *
         * @param uri The Namespace URI
         * @param localName The local name.
         * @return The attribute's type as an internalized string.
         */
        CARAPI GetType(
            /* [in] */ const String& uri,
            /* [in] */ const String& localName,
            /* [out] */ String* str);

        /**
         * Look up the type of an attribute by qualified (prefixed) name.
         *
         * @param qName The qualified name.
         * @return The attribute's type as an internalized string.
         */
        CARAPI GetType(
            /* [in] */ const String& qName,
            /* [out] */ String* str);

        /**
         * Look up the value of an attribute by Namespace name.
         *
         * @param uri The Namespace URI
         * @param localName The local name.
         * @return The attribute's value.
         */
        CARAPI GetValue(
            /* [in] */ const String& uri,
            /* [in] */ const String& localName,
            /* [out] */ String* str);

        /**
         * Look up the value of an attribute by qualified (prefixed) name.
         *
         * @param qName The qualified name.
         * @return The attribute's value.
         */
        CARAPI GetValue(
            /* [in] */ const String& qName,
            /* [out] */ String* str);

    private:
        AutoPtr<IAttributeList> mQAtts;
        AutoPtr<IWeakReference> mWeakHost;
    };
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_CPARSERADAPTER_H__
