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

#include <elastos/core/Object.h>
#include "org/apache/harmony/xml/ExpatAttributes.h"

//import java.io.IOException;
using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
//import java.net.URI;
//import java.net.URL;
//import java.net.URLConnection;
//import libcore.io.IoUtils;
using Org::Xml::Sax::IAttributes;
//import org.xml.sax.ContentHandler;
//import org.xml.sax.DTDHandler;
//import org.xml.sax.EntityResolver;
using Org::Xml::Sax::IInputSource;
using Org::Xml::Sax::ILocator;
//import org.xml.sax.SAXException;
//import org.xml.sax.SAXParseException;
//import org.xml.sax.ext.LexicalHandler;

/**
 * Adapts SAX API to the Expat native XML parser. Not intended for reuse
 * across documents.
 *
 * @see org.apache.harmony.xml.ExpatReader
 */
namespace Org {
namespace Apache {
namespace Harmony {
namespace Xml {

class ExpatParser
    : public Object
{
private:
    /**
     * Used for cloned attributes.
     */
    class ClonedAttributes
        : public ExpatAttributes
    {
        friend class ExpatParser;
    public:
        ~ClonedAttributes();

        //@Override
        CARAPI GetParserPointer(
            /* [out] */ Int64* pointer);

        //@Override
        CARAPI GetPointer(
            /* [out] */ Int64* pointer);

        //@Override
        CARAPI GetLength(
            /* [out] */ Int32* length);

        //@Override protected synchronized
        CARAPI_(void) Finalize();

    private:
        /**
         * Constructs a Java wrapper for native attributes.
         *
         * @param parserPointer pointer to the parse, can be 0 if length is 0.
         * @param pointer pointer to the attributes array, can be 0 if the
         *  length is 0.
         * @param length number of attributes
         */
        ClonedAttributes(
            /* [in] */ Int64 parserPointer,
            /* [in] */ Int64 pointer,
            /* [in] */ Int32 length);

    private:

        static AutoPtr<IAttributes> EMPTY;// = new ClonedAttributes(0, 0, 0);

        Int64 parserPointer;
        Int64 pointer;
        Int32 length;

    };

    class ExpatLocator
        : public Object
        , public ILocator
    {
    public:
        CAR_INTERFACE_DECL();

        ExpatLocator(
            /* [in] */ ExpatParser* host);

        CARAPI GetPublicId(
            /* [out] */ String* id);

        CARAPI GetSystemId(
            /* [out] */ String* id);

        CARAPI GetLineNumber(
            /* [out] */ Int32* lineNumber);

        CARAPI GetColumnNumber(
            /* [out] */ Int32* columnNumber);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        ExpatParser* mHost;
    };

    /**
     * Attributes that are only valid during startElement().
     */
    class CurrentAttributes
        : public ExpatAttributes
    {
    public:
        CurrentAttributes(
            /* [in] */ ExpatParser* host);

        CARAPI GetParserPointer(
            /* [out] */ Int64* pointer);

        CARAPI GetPointer(
            /* [out] */ Int64* pointer);

        CARAPI GetLength(
            /* [out] */ Int32* len);

    private:
        ExpatParser* mHost;
    };

    /**
     * Includes line and column in the message.
     */
    //class ParseException extends SAXParseException {

    //    private ParseException(String message, Locator locator) {
    //        super(makeMessage(message, locator), locator);
    //    }

    //    private static String makeMessage(String message, Locator locator) {
    //        return makeMessage(message, locator.getLineNumber(),
    //                locator.getColumnNumber());
    //    }

    //    private static String makeMessage(
    //            String message, int line, int column) {
    //        return "At line " + line + ", column "
    //                + column + ": " + message;
    //    }
    //}

public:
    /**
     * Constructs a new parser with the specified encoding.
     */
    /*package*/
    ExpatParser(
        /* [in] */ const String& encoding,
        /* [in] */ IExpatReader* xmlReader,
        /* [in] */ Boolean processNamespaces,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    ~ExpatParser();

    /**
     * Called at the start of an element.
     *
     * @param uri namespace URI of element or "" if namespace processing is
     *  disabled
     * @param localName local name of element or "" if namespace processing is
     *  disabled
     * @param qName qualified name or "" if namespace processing is enabled
     * @param attributePointer pointer to native attribute char*--we keep
     *  a separate pointer so we can detach it from the parser instance
     * @param attributeCount number of attributes
     */
    CARAPI_(void) StartElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName,
        /* [in] */ Int64 attributePointer,
        /* [in] */ Int32 attributeCount);

    CARAPI_(void) EndElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName);

    CARAPI_(void) Text(
        /* [in] */ ArrayOf<Char32>* text,// char[]
        /* [in] */ Int32 length);

    CARAPI_(void) Comment(
        /* [in] */ ArrayOf<Char32>* text,// char[]
        /* [in] */ Int32 length);

    CARAPI_(void) StartCdata();

    CARAPI_(void) EndCdata();

    CARAPI_(void) StartNamespace(
        /* [in] */ const String& prefix,
        /* [in] */ const String& uri);

    CARAPI_(void) EndNamespace(
        /* [in] */ const String& prefix);

    CARAPI_(void) StartDtd(
        /* [in] */ const String& name,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    CARAPI_(void) EndDtd();

    CARAPI_(void) ProcessingInstruction(
        /* [in] */ const String& target,
        /* [in] */ const String& data);

    CARAPI_(void) NotationDecl(
        /* [in] */ const String& name,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    CARAPI_(void) UnparsedEntityDecl(
        /* [in] */ const String& name,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId,
        /* [in] */ const String& notationName);

    /**
     * Handles an external entity.
     *
     * @param context to be passed back to Expat when we parse the entity
     * @param publicId the publicId of the entity
     * @param systemId the systemId of the entity
     */
    CARAPI_(void) HandleExternalEntity(
        /* [in] */ const String& context,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    /**
     * Appends part of an XML document. This parser will parse the given XML to
     * the extent possible and dispatch to the appropriate methods.
     *
     * @param xml a whole or partial snippet of XML
     * @throws SAXException if an error occurs during parsing
     */
    CARAPI_(void) Append(
        /* [in] */ const String& xml);

    /**
     * Appends part of an XML document. This parser will parse the given XML to
     * the extent possible and dispatch to the appropriate methods.
     *
     * @param xml a whole or partial snippet of XML
     * @param offset into the char[]
     * @param length of characters to use
     * @throws SAXException if an error occurs during parsing
     */
    CARAPI_(void) Append(
        /* [in] */ ArrayOf<Char32>* xml, //char[]
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /**
     * Appends part of an XML document. This parser will parse the given XML to
     * the extent possible and dispatch to the appropriate methods.
     *
     * @param xml a whole or partial snippet of XML
     * @throws SAXException if an error occurs during parsing
     */
    CARAPI_(void) Append(
        /* [in] */ ArrayOf<Byte>* xml);

    /**
     * Appends part of an XML document. This parser will parse the given XML to
     * the extent possible and dispatch to the appropriate methods.
     *
     * @param xml a whole or partial snippet of XML
     * @param offset into the byte[]
     * @param length of bytes to use
     * @throws SAXException if an error occurs during parsing
     */
    CARAPI_(void) Append(
        /* [in] */ ArrayOf<Byte>* xml,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /**
     * Parses an XML document from the given input stream.
     */
    CARAPI_(void) ParseDocument(
        /* [in] */ IInputStream* in);

    /**
     * Parses an XML Document from the given reader.
     */
    CARAPI_(void) ParseDocument(
        /* [in] */ IReader* in);

    /**
     * Indicate that we're finished parsing.
     *
     * @throws SAXException if the xml is incomplete
     */
    CARAPI_(void) Finish();

    //@Override protected synchronized
    CARAPI_(void) Finalize();

    /**
     * Clones the current attributes so they can be used outside of
     * startElement().
     */
    CARAPI CloneAttributes(
        /* [out] */ IAttributes** result);

    /**
     * Opens an InputStream for the given URL.
     */
    static CARAPI_(AutoPtr<IInputStream>) OpenUrl(
        /* [in] */ const String& url);

private:
    /**
     * Used by {@link EntityParser}.
     */
    ExpatParser(
        /* [in] */ const String& encoding,
        /* [in] */ IExpatReader* xmlReader,
        /* [in] */ Int64 pointer,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    /**
     * Initializes native resources.
     *
     * @return the pointer to the native parser
     */
    CARAPI_(Int64) Initialize(
        /* [in] */ const String& encoding,
        /* [in] */ Boolean namespacesEnabled);

    /**
     * Picks an encoding for an external entity. Defaults to UTF-8.
     */
    CARAPI_(String) PickEncoding(
        /* [in] */ IInputSource* inputSource);

    /**
     * Parses the the external entity provided by the input source.
     */
    CARAPI_(void) ParseExternalEntity(
        /* [in] */ ExpatParser* entityParser,
        /* [in] */ IInputSource* inputSource);

    /**
     * Creates a native entity parser.
     *
     * @param parentPointer pointer to parent Expat parser
     * @param context passed to {@link #handleExternalEntity}
     * @return pointer to native parser
     */
    static CARAPI_(Int64) CreateEntityParser(
        /* [in] */ Int64 parentPointer,
        /* [in] */ const String& context);

    CARAPI_(void) AppendString(
        /* [in] */ Int64 pointer,
        /* [in] */ const String& xml,
        /* [in] */ Boolean isFinal);

    CARAPI_(void) AppendChars(
        /* [in] */ Int64 pointer,
        /* [in] */ ArrayOf<Char32>* xml, // char[]
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    CARAPI_(void) AppendBytes(
        /* [in] */ Int64 pointer,
        /* [in] */ ArrayOf<Byte>* xml,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    /**
     * Parses XML from the given Reader.
     */
    CARAPI_(void) ParseFragment(
        /* [in] */ IReader* in);

    /**
     * Parses XML from the given input stream.
     */
    CARAPI_(void) ParseFragment(
        /* [in] */ IInputStream* in);

    CARAPI_(void) StartDocument();

    CARAPI_(void) EndDocument();

    /**
     * Releases all native objects.
     */
    CARAPI_(void) ReleasePointer(
        /* [in] */ Int64 pointer);

    /**
     * Releases native parser only.
     */
    static CARAPI_(void) ReleaseParser(
        /* [in] */ Int64 pointer);

    /**
     * Initialize static resources.
     */
    static CARAPI_(Int32) StaticInitialize(
        /* [in] */ const String& emptyString);

    //static {
    //    staticInitialize("");
    //}

    /**
     * Gets the current line number within the XML file.
     */
    CARAPI_(Int32) Line();

    static CARAPI_(Int32) Line(
        /* [in] */ Int64 pointer);

    /**
     * Gets the current column number within the XML file.
     */
    CARAPI_(Int32) Column();

    static CARAPI_(Int32) Column(
        /* [in] */ Int64 pointer);

    static CARAPI_(Int64) CloneAttributes(
        /* [in] */ Int64 pointer,
        /* [in] */ Int32 attributeCount);

public:
    /** Encoding used for Java chars, used to parse Readers and Strings */
    /*package*/ static const String CHARACTER_ENCODING;// = "UTF-16";

private:
    static const int BUFFER_SIZE = 8096; // in bytes

    /** Pointer to XML_Parser instance. */
    Int64 pointer;

    Boolean inStartElement;// = false;
    Int32 attributeCount;// = -1;
    Int64 attributePointer;// = 0;

    AutoPtr<ILocator> locator;// = new ExpatLocator();
    AutoPtr<IExpatReader> xmlReader;

    String publicId;
    String systemId;
    String encoding;

    AutoPtr<ExpatAttributes> attributes;// = new CurrentAttributes();

    static const String OUTSIDE_START_ELEMENT;// = "Attributes can only be used within the scope of startElement().";

    /** We default to UTF-8 when the user doesn't specify an encoding. */
    static const String DEFAULT_ENCODING;// = "UTF-8";

    /** Timeout for HTTP connections (in ms) */
    static const int TIMEOUT = 20 * 1000;
    Object mLock;
    static Int32 dummyCode;

    friend class EntityParser;
};

/**
 * Parses an external entity.
 */
class EntityParser
    : public ExpatParser
{
public:
    EntityParser(
        /* [in] */ const String& encoding,
        /* [in] */ IExpatReader* xmlReader,
        /* [in] */ Int64 pointer,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    CARAPI_(void) StartElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName,
        /* [in] */ Int64 attributePointer,
        /* [in] */ Int32 attributeCount);

    CARAPI_(void) EndElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName);

    //@Override
    //@SuppressWarnings("FinalizeDoesntCallSuperFinalize")
    //synchronized
    CARAPI_(void) Finalize();

private:

    Int32 depth;// = 0;

};

} // namespace Xml
} // namespace Harmony
} // namespace Apache
} // namespace Org
