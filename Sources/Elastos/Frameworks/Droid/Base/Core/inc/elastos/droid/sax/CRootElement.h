#ifndef __ELASTOS_DROID_SAX_CROOTELEMENT_H__
#define __ELASTOS_DROID_SAX_CROOTELEMENT_H__

#include <Elastos.CoreLibrary.External.h>
#include "_Elastos_Droid_Sax_CRootElement.h"
#include "elastos/droid/sax/Element.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::IStringBuilder;
using Org::Xml::Sax::IAttributes;
using Org::Xml::Sax::IEntityResolver;
using Org::Xml::Sax::EIID_IEntityResolver;
using Org::Xml::Sax::IDTDHandler;
using Org::Xml::Sax::EIID_IDTDHandler;
using Org::Xml::Sax::IContentHandler;
using Org::Xml::Sax::EIID_IContentHandler;
using Org::Xml::Sax::IErrorHandler;
using Org::Xml::Sax::EIID_IErrorHandler;
using Org::Xml::Sax::IInputSource;

namespace Elastos {
namespace Droid {
namespace Sax {

/**
 * The root XML element. The entry point for this API. Not safe for concurrent
 * use.
 *
 * <p>For example, passing this XML:
 *
 * <pre>
 * &lt;feed xmlns='http://www.w3.org/2005/Atom'>
 *   &lt;entry>
 *     &lt;id>bob&lt;/id>
 *   &lt;/entry>
 * &lt;/feed>
 * </pre>
 *
 * to this code:
 *
 * <pre>
 * static final String ATOM_NAMESPACE = "http://www.w3.org/2005/Atom";
 *
 * ...
 *
 * RootElement root = new RootElement(ATOM_NAMESPACE, "feed");
 * Element entry = root.getChild(ATOM_NAMESPACE, "entry");
 * entry.getChild(ATOM_NAMESPACE, "id").setEndTextElementListener(
 *   new EndTextElementListener() {
 *     public void end(String body) {
 *       System.out.println("Entry ID: " + body);
 *     }
 *   });
 *
 * XMLReader reader = ...;
 * reader.setContentHandler(root.getContentHandler());
 * reader.parse(...);
 * </pre>
 *
 * would output:
 *
 * <pre>
 * Entry ID: bob
 * </pre>
 */
CarClass(CRootElement)
    , public Element
{
public:

    /*
    * in Java, class Handler extends DefaultHandler, the DefaultHandler is defined here:
    * #include "org/xml/sax/helpers/DefaultHandler.h"
    */
    class Handler
        : public Object
        , public IEntityResolver
        , public IDTDHandler
        , public IContentHandler
        , public IErrorHandler
    {
    public:

        CAR_INTERFACE_DECL()

        Handler(
            /* [in] */ CRootElement* host)
            : mDepth(-1)
            , mHost(host)
        {}

        virtual ~Handler();

        //@Override
        CARAPI SetDocumentLocator(
            /* [in] */ ILocator* locator);

        //@Override
        CARAPI StartElement(
            /* [in] */ const String& uri,
            /* [in] */ const String& localName,
            /* [in] */ const String& qName,
            /* [in] */ IAttributes* attributes);

        CARAPI StartRoot(
            /* [in] */ const String& uri,
            /* [in] */ const String& localName,
            /* [in] */ IAttributes* attributes);

        CARAPI Start(
            /* [in] */ IElement* e,
            /* [in] */ IAttributes* attributes);

        //@Override
        CARAPI Characters(
            /* [in] */ const ArrayOf<Char32>& buffer,
            /* [in] */ Int32 start,
            /* [in] */ Int32 length);

        //@Override
        CARAPI EndElement(
            /* [in] */ const String& uri,
            /* [in] */ const String& localName,
            /* [in] */ const String& qName);

        /**
         * Resolve an external entity.
         *
         * <p>Always return null, so that the parser will use the system
         * identifier provided in the XML document.  This method implements
         * the SAX default behaviour: application writers can override it
         * in a subclass to do special translations such as catalog lookups
         * or URI redirection.</p>
         *
         * @param publicId The public identifer, or null if none is
         *                 available.
         * @param systemId The system identifier provided in the XML
         *                 document.
         * @return The new input source, or null to require the
         *         default behaviour.
         * @exception java.io.IOException If there is an error setting
         *            up the new input source.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.EntityResolver#resolveEntity
         */
        CARAPI ResolveEntity(
            /* [in] */ const String& publicId,
            /* [in] */ const String& systemId,
            /* [out] */ IInputSource** src);

        /**
         * Receive notification of a notation declaration.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method in a subclass if they wish to keep track of the notations
         * declared in a document.</p>
         *
         * @param name The notation name.
         * @param publicId The notation public identifier, or null if not
         *                 available.
         * @param systemId The notation system identifier.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.DTDHandler#notationDecl
         */
        CARAPI NotationDecl(
            /* [in] */ const String& name,
            /* [in] */ const String& publicId,
            /* [in] */ const String& systemId);

        /**
         * Receive notification of an unparsed entity declaration.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method in a subclass to keep track of the unparsed entities
         * declared in a document.</p>
         *
         * @param name The entity name.
         * @param publicId The entity public identifier, or null if not
         *                 available.
         * @param systemId The entity system identifier.
         * @param notationName The name of the associated notation.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.DTDHandler#unparsedEntityDecl
         */
        CARAPI UnparsedEntityDecl(
            /* [in] */ const String& name,
            /* [in] */ const String& publicId,
            /* [in] */ const String& systemId,
            /* [in] */ const String& notationName);

        /**
         * Receive notification of the beginning of the document.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method in a subclass to take specific actions at the beginning
         * of a document (such as allocating the root node of a tree or
         * creating an output file).</p>
         *
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ContentHandler#startDocument
         */
        CARAPI StartDocument();

        /**
         * Receive notification of the end of the document.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method in a subclass to take specific actions at the end
         * of a document (such as finalising a tree or closing an output
         * file).</p>
         *
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ContentHandler#endDocument
         */
        CARAPI EndDocument();

        /**
         * Receive notification of the start of a Namespace mapping.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method in a subclass to take specific actions at the start of
         * each Namespace prefix scope (such as storing the prefix mapping).</p>
         *
         * @param prefix The Namespace prefix being declared.
         * @param uri The Namespace URI mapped to the prefix.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ContentHandler#startPrefixMapping
         */
        CARAPI StartPrefixMapping(
            /* [in] */ const String& prefix,
            /* [in] */ const String& uri);

        /**
         * Receive notification of the end of a Namespace mapping.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method in a subclass to take specific actions at the end of
         * each prefix mapping.</p>
         *
         * @param prefix The Namespace prefix being declared.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ContentHandler#endPrefixMapping
         */
        CARAPI EndPrefixMapping(
            /* [in] */ const String& prefix);

        /**
         * Receive notification of character data inside an element.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method to take specific actions for each chunk of character data
         * (such as adding the data to a node or buffer, or printing it to
         * a file).</p>
         *
         * @param ch The characters.
         * @param start The start position in the character array.
         * @param length The number of characters to use from the
         *               character array.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ContentHandler#characters
         */
        CARAPI Characters(
            /* [out] */ ArrayOf<Char32>* ch,
            /* [in] */ Int32 start,
            /* [in] */ Int32 length);

        /**
         * Receive notification of ignorable whitespace in element content.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method to take specific actions for each chunk of ignorable
         * whitespace (such as adding data to a node or buffer, or printing
         * it to a file).</p>
         *
         * @param ch The whitespace characters.
         * @param start The start position in the character array.
         * @param length The number of characters to use from the
         *               character array.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ContentHandler#ignorableWhitespace
         */
        CARAPI IgnorableWhitespace(
            /* [out] */ ArrayOf<Char32>* ch,
            /* [in] */ Int32 start,
            /* [in] */ Int32 length);

        /**
         * Receive notification of a processing instruction.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method in a subclass to take specific actions for each
         * processing instruction, such as setting status variables or
         * invoking other methods.</p>
         *
         * @param target The processing instruction target.
         * @param data The processing instruction data, or null if
         *             none is supplied.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ContentHandler#processingInstruction
         */
        CARAPI ProcessingInstruction(
            /* [in] */ const String& target,
            /* [in] */ const String& data);

        /**
         * Receive notification of a skipped entity.
         *
         * <p>By default, do nothing.  Application writers may override this
         * method in a subclass to take specific actions for each
         * processing instruction, such as setting status variables or
         * invoking other methods.</p>
         *
         * @param name The name of the skipped entity.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ContentHandler#processingInstruction
         */
        CARAPI SkippedEntity(
            /* [in] */ const String& name);

        ////////////////////////////////////////////////////////////////////
        // Default implementation of the ErrorHandler interface.
        ////////////////////////////////////////////////////////////////////

        /**
         * Receive notification of a parser warning.
         *
         * <p>The default implementation does nothing.  Application writers
         * may override this method in a subclass to take specific actions
         * for each warning, such as inserting the message in a log file or
         * printing it to the console.</p>
         *
         * @param e The warning information encoded as an exception.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ErrorHandler#warning
         * @see org.xml.sax.SAXParseException
         */
        CARAPI Warning(
            /* [in] */ ECode exception);

        /**
         * Receive notification of a recoverable parser error.
         *
         * <p>The default implementation does nothing.  Application writers
         * may override this method in a subclass to take specific actions
         * for each error, such as inserting the message in a log file or
         * printing it to the console.</p>
         *
         * @param e The warning information encoded as an exception.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ErrorHandler#warning
         * @see org.xml.sax.SAXParseException
         */
        CARAPI Error(
            /* [in] */ ECode exception);

        /**
         * Report a fatal XML parsing error.
         *
         * <p>The default implementation throws a SAXParseException.
         * Application writers may override this method in a subclass if
         * they need to take specific actions for each fatal error (such as
         * collecting all of the errors into a single report): in any case,
         * the application must stop all regular processing when this
         * method is invoked, since the document is no longer reliable, and
         * the parser may no longer report parsing events.</p>
         *
         * @param e The error information encoded as an exception.
         * @exception org.xml.sax.SAXException Any SAX exception, possibly
         *            wrapping another exception.
         * @see org.xml.sax.ErrorHandler#fatalError
         * @see org.xml.sax.SAXParseException
         */
        CARAPI FatalError(
            /* [in] */ ECode exception);

    private:
        AutoPtr<ILocator> mLocator;
        Int32 mDepth;
        AutoPtr<IElement> mCurrent;
        AutoPtr<IStringBuilder> mBodyBuilder;
        CRootElement* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CRootElement()
    {}

    virtual ~CRootElement();

    /**
     * Constructs a new root element with the given name.
     *
     * @param uri the namespace
     * @param localName the local name
     */
    CARAPI constructor(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName);

    /**
     * Constructs a new root element with the given name. Uses an empty string
     * as the namespace.
     *
     * @param localName the local name
     */
    CARAPI constructor(
        /* [in] */ const String& localName);

    /**
     * Gets the SAX {@code ContentHandler}. Pass this to your SAX parser.
     */
    CARAPI GetContentHandler(
        /* [out] */ IContentHandler** handler);

private:
    AutoPtr<Handler> mHandler;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SAX_CROOTELEMENT_H__
