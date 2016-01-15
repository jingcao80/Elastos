
#ifndef __ORG_XML_SAX_HELPERS_CXMLFILTERIMPL_H__
#define __ORG_XML_SAX_HELPERS_CXMLFILTERIMPL_H__

#include "_Org_Xml_Sax_Helpers_CXMLFilterImpl.h"
#include "Object.h"


namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

CarClass(CXMLFilterImpl)
    , public Object
    , public IXMLReader
    , public IXMLFilter
    , public IEntityResolver
    , public IDTDHandler
    , public IContentHandler
    , public IErrorHandler
{
public:
    CAR_INTERFACE_DECL();
    CAR_OBJECT_DECL()

    /**
     * Construct an empty XML filter, with no parent.
     *
     * <p>This filter will have no parent: you must assign a parent
     * before you start a parse or do any configuration with
     * setFeature or setProperty, unless you use this as a pure event
     * consumer rather than as an {@link XMLReader}.</p>
     *
     * @see org.xml.sax.XMLReader#setFeature
     * @see org.xml.sax.XMLReader#setProperty
     * @see #setParent
     */
    CARAPI constructor();

    /**
     * Construct an XML filter with the specified parent.
     *
     * @param parent the XML reader from which this filter receives its events.
     *
     * @see #setParent
     * @see #getParent
     */
    CARAPI constructor(
        /* [in] */ IXMLReader* parent);

    /**
     * Look up the value of a feature.
     *
     * <p>This will always fail if the parent is null.</p>
     *
     * @param name The feature name.
     * @return The current value of the feature.
     * @exception org.xml.sax.SAXNotRecognizedException If the feature
     *            value can't be assigned or retrieved from the parent.
     * @exception org.xml.sax.SAXNotSupportedException When the
     *            parent recognizes the feature name but
     *            cannot determine its value at this time.
     */
    CARAPI GetFeature(
        /* [in] */ const String& name,
        /* [out] */ Boolean* feature);

    /**
     * Set the value of a feature.
     *
     * <p>This will always fail if the parent is null.</p>
     *
     * @param name The feature name.
     * @param value The requested feature value.
     * @exception org.xml.sax.SAXNotRecognizedException If the feature
     *            value can't be assigned or retrieved from the parent.
     * @exception org.xml.sax.SAXNotSupportedException When the
     *            parent recognizes the feature name but
     *            cannot set the requested value.
     */
    CARAPI SetFeature(
        /* [in] */ const String& name,
        /* [in] */ Boolean value);

    /**
     * Look up the value of a property.
     *
     * @param name The property name.
     * @return The current value of the property.
     * @exception org.xml.sax.SAXNotRecognizedException If the property
     *            value can't be assigned or retrieved from the parent.
     * @exception org.xml.sax.SAXNotSupportedException When the
     *            parent recognizes the property name but
     *            cannot determine its value at this time.
     */
    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ IInterface** value);

    /**
     * Set the value of a property.
     *
     * <p>This will always fail if the parent is null.</p>
     *
     * @param name The property name.
     * @param value The requested property value.
     * @exception org.xml.sax.SAXNotRecognizedException If the property
     *            value can't be assigned or retrieved from the parent.
     * @exception org.xml.sax.SAXNotSupportedException When the
     *            parent recognizes the property name but
     *            cannot set the requested value.
     */
    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value);

    /**
     * Set the entity resolver.
     *
     * @param resolver The new entity resolver.
     */
    CARAPI SetEntityResolver(
        /* [in] */ IEntityResolver* resolver);

    /**
     * Get the current entity resolver.
     *
     * @return The current entity resolver, or null if none was set.
     */
    CARAPI GetEntityResolver(
        /* [out] */ IEntityResolver** resolver);

    /**
     * Set the DTD event handler.
     *
     * @param handler the new DTD handler
     */
    CARAPI SetDTDHandler(
        /* [in] */ IDTDHandler* handler);

    /**
     * Get the current DTD event handler.
     *
     * @return The current DTD handler, or null if none was set.
     */
    CARAPI GetDTDHandler(
        /* [out] */ IDTDHandler** handler);

    /**
     * Set the content event handler.
     *
     * @param handler the new content handler
     */
    CARAPI SetContentHandler(
        /* [in] */ IContentHandler* handler);

    /**
     * Get the content event handler.
     *
     * @return The current content handler, or null if none was set.
     */
    CARAPI GetContentHandler(
        /* [out] */ IContentHandler** handler);

    /**
     * Set the error event handler.
     *
     * @param handler the new error handler
     */
    CARAPI SetErrorHandler(
        /* [in] */ IErrorHandler* handler);

    /**
     * Get the current error event handler.
     *
     * @return The current error handler, or null if none was set.
     */
    CARAPI GetErrorHandler(
        /* [out] */ IErrorHandler** handler);

    /**
     * Parse a document.
     *
     * @param input The input source for the document entity.
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException An IO exception from the parser,
     *            possibly from a byte stream or character stream
     *            supplied by the application.
     */
    CARAPI Parse(
        /* [in] */ IInputSource* input);

    /**
     * Parse a document.
     *
     * @param systemId The system identifier as a fully-qualified URI.
     * @exception org.xml.sax.SAXException Any SAX exception, possibly
     *            wrapping another exception.
     * @exception java.io.IOException An IO exception from the parser,
     *            possibly from a byte stream or character stream
     *            supplied by the application.
     */
    CARAPI Parse(
        /* [in] */ const String& systemId);

    /**
     * Set the parent reader.
     *
     * <p>This is the {@link org.xml.sax.XMLReader XMLReader} from which
     * this filter will obtain its events and to which it will pass its
     * configuration requests.  The parent may itself be another filter.</p>
     *
     * <p>If there is no parent reader set, any attempt to parse
     * or to set or get a feature or property will fail.</p>
     *
     * @param parent The parent XML reader.
     * @see #getParent
     */
    CARAPI SetParent(
        /* [in] */ IXMLReader* parent);

    /**
     * Get the parent reader.
     *
     * @return The parent XML reader, or null if none is set.
     * @see #setParent
     */
    CARAPI GetParent(
        /* [out] */ IXMLReader** parent);


    /**
     * Filter an external entity resolution.
     *
     * @param publicId The entity's public identifier, or null.
     * @param systemId The entity's system identifier.
     * @return A new InputSource or null for the default.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     * @exception java.io.IOException The client may throw an
     *            I/O-related exception while obtaining the
     *            new InputSource.
     */
    CARAPI ResolveEntity(
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId,
        /* [out] */ IInputSource** src);

    /**
     * Filter a notation declaration event.
     *
     * @param name The notation name.
     * @param publicId The notation's public identifier, or null.
     * @param systemId The notation's system identifier, or null.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI NotationDecl(
        /* [in] */ const String& name,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId);

    /**
     * Filter an unparsed entity declaration event.
     *
     * @param name The entity name.
     * @param publicId The entity's public identifier, or null.
     * @param systemId The entity's system identifier, or null.
     * @param notationName The name of the associated notation.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI UnparsedEntityDecl(
        /* [in] */ const String& name,
        /* [in] */ const String& publicId,
        /* [in] */ const String& systemId,
        /* [in] */ const String& notationName);

    /**
     * Filter a new document locator event.
     *
     * @param locator The document locator.
     */
    CARAPI SetDocumentLocator(
        /* [in] */ ILocator* locator);

    /**
     * Filter a start document event.
     *
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI StartDocument();

    /**
     * Filter an end document event.
     *
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI EndDocument();

    /**
     * Filter a start Namespace prefix mapping event.
     *
     * @param prefix The Namespace prefix.
     * @param uri The Namespace URI.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI StartPrefixMapping(
        /* [in] */ const String& prefix,
        /* [in] */ const String& uri);

    /**
     * Filter an end Namespace prefix mapping event.
     *
     * @param prefix The Namespace prefix.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI EndPrefixMapping(
        /* [in] */ const String& prefix);

    /**
     * Filter a start element event.
     *
     * @param uri The element's Namespace URI, or the empty string.
     * @param localName The element's local name, or the empty string.
     * @param qName The element's qualified (prefixed) name, or the empty
     *        string.
     * @param atts The element's attributes.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI StartElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName,
        /* [in] */ IAttributes* atts);

    /**
     * Filter an end element event.
     *
     * @param uri The element's Namespace URI, or the empty string.
     * @param localName The element's local name, or the empty string.
     * @param qName The element's qualified (prefixed) name, or the empty
     *        string.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI EndElement(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName);

    /**
     * Filter a character data event.
     *
     * @param ch An array of characters.
     * @param start The starting position in the array.
     * @param length The number of characters to use from the array.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI Characters(
        /* [out] */ ArrayOf<Char32>* ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

    /**
     * Filter an ignorable whitespace event.
     *
     * @param ch An array of characters.
     * @param start The starting position in the array.
     * @param length The number of characters to use from the array.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI IgnorableWhitespace(
        /* [out] */ ArrayOf<Char32>* ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 length);

    /**
     * Filter a processing instruction event.
     *
     * @param target The processing instruction target.
     * @param data The text following the target.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI ProcessingInstruction(
        /* [in] */ const String& target,
        /* [in] */ const String& data);

    /**
     * Filter a skipped entity event.
     *
     * @param name The name of the skipped entity.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI SkippedEntity(
        /* [in] */ const String& name);

    /**
     * Filter a warning event.
     *
     * @param e The warning as an exception.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI Warning(
        /* [in] */ ECode exception);

    /**
     * Filter an error event.
     *
     * @param e The error as an exception.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI Error(
        /* [in] */ ECode exception);

    /**
     * Filter a fatal error event.
     *
     * @param e The error as an exception.
     * @exception org.xml.sax.SAXException The client may throw
     *            an exception during processing.
     */
    CARAPI FatalError(
        /* [in] */ ECode exception);

private:
    /**
     * Set up before a parse.
     *
     * <p>Before every parse, check whether the parent is
     * non-null, and re-register the filter for all of the
     * events.</p>
     */
    CARAPI SetupParse();

private:
    AutoPtr<IXMLReader> mParent;
    AutoPtr<ILocator> mLocator;
    AutoPtr<IEntityResolver> mEntityResolver;
    AutoPtr<IDTDHandler> mDtdHandler;
    AutoPtr<IContentHandler> mContentHandler;
    AutoPtr<IErrorHandler> mErrorHandler;
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_CXMLFILTERIMPL_H__
