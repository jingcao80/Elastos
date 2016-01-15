
#ifndef __ORG_XML_SAX_HELPERS_ATTRIBUTESIMPL_H__
#define __ORG_XML_SAX_HELPERS_ATTRIBUTESIMPL_H__

#include "Elastos.CoreLibrary.External.h"
#include "Object.h"

using Org::Xml::Sax::IAttributes;

namespace Org {
namespace Xml {
namespace Sax {
namespace Helpers {

/**
 * Default implementation of the Attributes interface.
 *
 * <blockquote>
 * <em>This module, both source code and documentation, is in the
 * Public Domain, and comes with <strong>NO WARRANTY</strong>.</em>
 * See <a href='http://www.saxproject.org'>http://www.saxproject.org</a>
 * for further information.
 * </blockquote>
 *
 * <p>This class provides a default implementation of the SAX2
 * {@link org.xml.sax.Attributes Attributes} interface, with the
 * addition of manipulators so that the list can be modified or
 * reused.</p>
 *
 * <p>There are two typical uses of this class:</p>
 *
 * <ol>
 * <li>to take a persistent snapshot of an Attributes object
 *  in a {@link org.xml.sax.ContentHandler#startElement startElement} event; or</li>
 * <li>to construct or modify an Attributes object in a SAX2 driver or filter.</li>
 * </ol>
 *
 * <p>This class replaces the now-deprecated SAX1 {@link
 * org.xml.sax.helpers.AttributeListImpl AttributeListImpl}
 * class; in addition to supporting the updated Attributes
 * interface rather than the deprecated {@link org.xml.sax.AttributeList
 * AttributeList} interface, it also includes a much more efficient
 * implementation using a single array rather than a set of Vectors.</p>
 *
 * @since SAX 2.0
 * @author David Megginson
 * @version 2.0.1 (sax2r2)
 */
class AttributesImpl
    : public Object
    , public IAttributes
    , public IAttributesImpl
{
public:
    CAR_INTERFACE_DECL()

    AttributesImpl();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IAttributes* atts);

    /**
     * Return the number of attributes in the list.
     *
     * @return The number of attributes in the list.
     * @see org.xml.sax.Attributes#getLength
     */
    CARAPI GetLength(
        /* [out] */ Int32* length);

    /**
     * Return an attribute's Namespace URI.
     *
     * @param index The attribute's index (zero-based).
     * @return The Namespace URI, the empty string if none is
     *         available, or null if the index is out of range.
     * @see org.xml.sax.Attributes#getURI
     */
    CARAPI GetURI(
        /* [in] */ Int32 index,
        /* [out] */ String* URI);

    /**
     * Return an attribute's local name.
     *
     * @param index The attribute's index (zero-based).
     * @return The attribute's local name, the empty string if
     *         none is available, or null if the index if out of range.
     * @see org.xml.sax.Attributes#getLocalName
     */
    CARAPI GetLocalName(
        /* [in] */ Int32 index,
        /* [out] */ String* localName);

    /**
     * Return an attribute's qualified (prefixed) name.
     *
     * @param index The attribute's index (zero-based).
     * @return The attribute's qualified name, the empty string if
     *         none is available, or null if the index is out of bounds.
     * @see org.xml.sax.Attributes#getQName
     */
    CARAPI GetQName(
        /* [in] */ Int32 index,
        /* [out] */ String* qName);

    /**
     * Return an attribute's type by index.
     *
     * @param index The attribute's index (zero-based).
     * @return The attribute's type, "CDATA" if the type is unknown, or null
     *         if the index is out of bounds.
     * @see org.xml.sax.Attributes#getType(int)
     */
    CARAPI GetType(
        /* [in] */ Int32 index,
        /* [out] */ String* type);

    /**
     * Return an attribute's value by index.
     *
     * @param index The attribute's index (zero-based).
     * @return The attribute's value or null if the index is out of bounds.
     * @see org.xml.sax.Attributes#getValue(int)
     */
    CARAPI GetValue(
        /* [in] */ Int32 index,
        /* [out] */ String* value);

    /**
     * Look up an attribute's index by Namespace name.
     *
     * <p>In many cases, it will be more efficient to look up the name once and
     * use the index query methods rather than using the name query methods
     * repeatedly.</p>
     *
     * @param uri The attribute's Namespace URI, or the empty
     *        string if none is available.
     * @param localName The attribute's local name.
     * @return The attribute's index, or -1 if none matches.
     * @see org.xml.sax.Attributes#getIndex(java.lang.String,java.lang.String)
     */
    CARAPI GetIndex(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ Int32* index);

    /**
     * Look up an attribute's index by qualified (prefixed) name.
     *
     * @param qName The qualified name.
     * @return The attribute's index, or -1 if none matches.
     * @see org.xml.sax.Attributes#getIndex(java.lang.String)
     */
    CARAPI GetIndex(
        /* [in] */ const String& qName,
        /* [out] */ Int32* index);

    /**
     * Look up an attribute's type by Namespace-qualified name.
     *
     * @param uri The Namespace URI, or the empty string for a name
     *        with no explicit Namespace URI.
     * @param localName The local name.
     * @return The attribute's type, or null if there is no
     *         matching attribute.
     * @see org.xml.sax.Attributes#getType(java.lang.String,java.lang.String)
     */
    CARAPI GetType(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ String* type);

    /**
     * Look up an attribute's type by qualified (prefixed) name.
     *
     * @param qName The qualified name.
     * @return The attribute's type, or null if there is no
     *         matching attribute.
     * @see org.xml.sax.Attributes#getType(java.lang.String)
     */
    CARAPI GetType(
        /* [in] */ const String& qName,
        /* [out] */ String* type);

    /**
     * Look up an attribute's value by Namespace-qualified name.
     *
     * @param uri The Namespace URI, or the empty string for a name
     *        with no explicit Namespace URI.
     * @param localName The local name.
     * @return The attribute's value, or null if there is no
     *         matching attribute.
     * @see org.xml.sax.Attributes#getValue(java.lang.String,java.lang.String)
     */
    CARAPI GetValue(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [out] */ String* value);

    /**
     * Look up an attribute's value by qualified (prefixed) name.
     *
     * @param qName The qualified name.
     * @return The attribute's value, or null if there is no
     *         matching attribute.
     * @see org.xml.sax.Attributes#getValue(java.lang.String)
     */
    CARAPI GetValue(
        /* [in] */ const String& qName,
        /* [out] */ String* value);

    /**
     * Clear the attribute list for reuse.
     *
     * <p>Note that little memory is freed by this call:
     * the current array is kept so it can be
     * reused.</p>
     */
    CARAPI Clear();

    /**
     * Copy an entire Attributes object.
     *
     * <p>It may be more efficient to reuse an existing object
     * rather than constantly allocating new ones.</p>
     *
     * @param atts The attributes to copy.
     */
    CARAPI SetAttributes(
        /* [in] */ IAttributes* atts);

    /**
     * Add an attribute to the end of the list.
     *
     * <p>For the sake of speed, this method does no checking
     * to see if the attribute is already in the list: that is
     * the responsibility of the application.</p>
     *
     * @param uri The Namespace URI, or the empty string if
     *        none is available or Namespace processing is not
     *        being performed.
     * @param localName The local name, or the empty string if
     *        Namespace processing is not being performed.
     * @param qName The qualified (prefixed) name, or the empty string
     *        if qualified names are not available.
     * @param type The attribute type as a string.
     * @param value The attribute value.
     */
    CARAPI AddAttribute(
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName,
        /* [in] */ const String& type,
        /* [in] */ const String& value);

    /**
     * Set an attribute in the list.
     *
     * <p>For the sake of speed, this method does no checking
     * for name conflicts or well-formedness: such checks are the
     * responsibility of the application.</p>
     *
     * @param index The index of the attribute (zero-based).
     * @param uri The Namespace URI, or the empty string if
     *        none is available or Namespace processing is not
     *        being performed.
     * @param localName The local name, or the empty string if
     *        Namespace processing is not being performed.
     * @param qName The qualified name, or the empty string
     *        if qualified names are not available.
     * @param type The attribute type as a string.
     * @param value The attribute value.
     * @exception java.lang.ArrayIndexOutOfBoundsException When the
     *            supplied index does not point to an attribute
     *            in the list.
     */
    CARAPI SetAttribute(
        /* [in] */ Int32 index,
        /* [in] */ const String& uri,
        /* [in] */ const String& localName,
        /* [in] */ const String& qName,
        /* [in] */ const String& type,
        /* [in] */ const String& value);

    /**
     * Remove an attribute from the list.
     *
     * @param index The index of the attribute (zero-based).
     * @exception java.lang.ArrayIndexOutOfBoundsException When the
     *            supplied index does not point to an attribute
     *            in the list.
     */
    CARAPI RemoveAttribute(
        /* [in] */ Int32 index);

    /**
     * Set the Namespace URI of a specific attribute.
     *
     * @param index The index of the attribute (zero-based).
     * @param uri The attribute's Namespace URI, or the empty
     *        string for none.
     * @exception java.lang.ArrayIndexOutOfBoundsException When the
     *            supplied index does not point to an attribute
     *            in the list.
     */
    CARAPI SetURI(
        /* [in] */ Int32 index,
        /* [in] */ const String& uri);

    /**
     * Set the local name of a specific attribute.
     *
     * @param index The index of the attribute (zero-based).
     * @param localName The attribute's local name, or the empty
     *        string for none.
     * @exception java.lang.ArrayIndexOutOfBoundsException When the
     *            supplied index does not point to an attribute
     *            in the list.
     */
    CARAPI SetLocalName(
        /* [in] */ Int32 index,
        /* [in] */ const String& localName);

    /**
     * Set the qualified name of a specific attribute.
     *
     * @param index The index of the attribute (zero-based).
     * @param qName The attribute's qualified name, or the empty
     *        string for none.
     * @exception java.lang.ArrayIndexOutOfBoundsException When the
     *            supplied index does not point to an attribute
     *            in the list.
     */
    CARAPI SetQName(
        /* [in] */ Int32 index,
        /* [in] */ const String& qName);

    /**
     * Set the type of a specific attribute.
     *
     * @param index The index of the attribute (zero-based).
     * @param type The attribute's type.
     * @exception java.lang.ArrayIndexOutOfBoundsException When the
     *            supplied index does not point to an attribute
     *            in the list.
     */
    CARAPI SetType(
        /* [in] */ Int32 index,
        /* [in] */ const String& type);

    /**
     * Set the value of a specific attribute.
     *
     * @param index The index of the attribute (zero-based).
     * @param value The attribute's value.
     * @exception java.lang.ArrayIndexOutOfBoundsException When the
     *            supplied index does not point to an attribute
     *            in the list.
     */
    CARAPI SetValue(
        /* [in] */ Int32 index,
        /* [in] */ const String& value);

private:
    /**
     * Ensure the internal array's capacity.
     *
     * @param n The minimum number of attributes that the array must
     *        be able to hold.
     */
    CARAPI_(void) EnsureCapacity (
        /* [in] */ Int32 n);

    /**
     * Report a bad array index in a manipulator.
     *
     * @param index The index to report.
     * @exception java.lang.ArrayIndexOutOfBoundsException Always.
     */
    CARAPI BadIndex (
        /* [in] */ Int32 index);

private:
    Int32 mLength;
    AutoPtr<ArrayOf<String> > mData;
};

} // namespace Helpers
} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_HELPERS_ATTRIBUTESIMPL_H__
