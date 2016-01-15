
#ifndef __ORG_XML_SAX_CINPUTSOURCE_H__
#define __ORG_XML_SAX_CINPUTSOURCE_H__

#include "Elastos.CoreLibrary.IO.h"
#include "_Org_Xml_Sax_CInputSource.h"
#include "Object.h"

using Elastos::IO::IReader;
using Elastos::IO::IInputStream;

namespace Org {
namespace Xml {
namespace Sax {

CarClass(CInputSource)
    , public Object
    , public IInputSource
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Zero-argument default constructor.
     *
     * @see #setPublicId
     * @see #setSystemId
     * @see #setByteStream
     * @see #setCharacterStream
     * @see #setEncoding
     */
    CARAPI constructor();

    /**
     * Create a new input source with a system identifier.
     *
     * <p>Applications may use setPublicId to include a
     * public identifier as well, or setEncoding to specify
     * the character encoding, if known.</p>
     *
     * <p>If the system identifier is a URL, it must be fully
     * resolved (it may not be a relative URL).</p>
     *
     * @param systemId The system identifier (URI).
     * @see #setPublicId
     * @see #setSystemId
     * @see #setByteStream
     * @see #setEncoding
     * @see #setCharacterStream
     */
    CARAPI constructor(
        /* [in] */ const String& systemId);

    /**
     * Create a new input source with a byte stream.
     *
     * <p>Application writers should use setSystemId() to provide a base
     * for resolving relative URIs, may use setPublicId to include a
     * public identifier, and may use setEncoding to specify the object's
     * character encoding.</p>
     *
     * @param byteStream The raw byte stream containing the document.
     * @see #setPublicId
     * @see #setSystemId
     * @see #setEncoding
     * @see #setByteStream
     * @see #setCharacterStream
     */
    CARAPI constructor(
        /* [in] */ IInputStream* byteStream);

    /**
     * Create a new input source with a character stream.
     *
     * <p>Application writers should use setSystemId() to provide a base
     * for resolving relative URIs, and may use setPublicId to include a
     * public identifier.</p>
     *
     * <p>The character stream shall not include a byte order mark.</p>
     *
     * @param characterStream The raw character stream containing the document.
     * @see #setPublicId
     * @see #setSystemId
     * @see #setByteStream
     * @see #setCharacterStream
     */
    CARAPI constructor(
        /* [in] */ IReader* characterStream);

    /**
     * Set the public identifier for this input source.
     *
     * <p>The public identifier is always optional: if the application
     * writer includes one, it will be provided as part of the
     * location information.</p>
     *
     * @param publicId The public identifier as a string.
     * @see #getPublicId
     * @see org.xml.sax.Locator#getPublicId
     * @see org.xml.sax.SAXParseException#getPublicId
     */
    CARAPI SetPublicId(
        /* [in] */ const String& publicId);

    /**
     * Get the public identifier for this input source.
     *
     * @return The public identifier, or null if none was supplied.
     * @see #setPublicId
     */
    CARAPI GetPublicId(
        /* [out] */ String* publicId);

    /**
     * Set the system identifier for this input source.
     *
     * <p>The system identifier is optional if there is a byte stream
     * or a character stream, but it is still useful to provide one,
     * since the application can use it to resolve relative URIs
     * and can include it in error messages and warnings (the parser
     * will attempt to open a connection to the URI only if
     * there is no byte stream or character stream specified).</p>
     *
     * <p>If the application knows the character encoding of the
     * object pointed to by the system identifier, it can register
     * the encoding using the setEncoding method.</p>
     *
     * <p>If the system identifier is a URL, it must be fully
     * resolved (it may not be a relative URL).</p>
     *
     * @param systemId The system identifier as a string.
     * @see #setEncoding
     * @see #getSystemId
     * @see org.xml.sax.Locator#getSystemId
     * @see org.xml.sax.SAXParseException#getSystemId
     */
    CARAPI SetSystemId(
        /* [in] */ const String& systemId);

    /**
     * Get the system identifier for this input source.
     *
     * <p>The getEncoding method will return the character encoding
     * of the object pointed to, or null if unknown.</p>
     *
     * <p>If the system ID is a URL, it will be fully resolved.</p>
     *
     * @return The system identifier, or null if none was supplied.
     * @see #setSystemId
     * @see #getEncoding
     */
    CARAPI GetSystemId(
        /* [out] */ String* systemId);

    /**
     * Set the byte stream for this input source.
     *
     * <p>The SAX parser will ignore this if there is also a character
     * stream specified, but it will use a byte stream in preference
     * to opening a URI connection itself.</p>
     *
     * <p>If the application knows the character encoding of the
     * byte stream, it should set it with the setEncoding method.</p>
     *
     * @param byteStream A byte stream containing an XML document or
     *        other entity.
     * @see #setEncoding
     * @see #getByteStream
     * @see #getEncoding
     * @see java.io.InputStream
     */
    CARAPI SetByteStream(
        /* [in] */ IInputStream* byteStream);

    /**
     * Get the byte stream for this input source.
     *
     * <p>The getEncoding method will return the character
     * encoding for this byte stream, or null if unknown.</p>
     *
     * @return The byte stream, or null if none was supplied.
     * @see #getEncoding
     * @see #setByteStream
     */
    CARAPI GetByteStream(
        /* [out] */ IInputStream** byteStream);

    /**
     * Set the character encoding, if known.
     *
     * <p>The encoding must be a string acceptable for an
     * XML encoding declaration (see section 4.3.3 of the XML 1.0
     * recommendation).</p>
     *
     * <p>This method has no effect when the application provides a
     * character stream.</p>
     *
     * @param encoding A string describing the character encoding.
     * @see #setSystemId
     * @see #setByteStream
     * @see #getEncoding
     */
    CARAPI SetEncoding(
        /* [in] */ const String& encoding);

    /**
     * Get the character encoding for a byte stream or URI.
     * This value will be ignored when the application provides a
     * character stream.
     *
     * @return The encoding, or null if none was supplied.
     * @see #setByteStream
     * @see #getSystemId
     * @see #getByteStream
     */
    CARAPI GetEncoding(
        /* [out] */ String* encoding);

    /**
     * Set the character stream for this input source.
     *
     * <p>If there is a character stream specified, the SAX parser
     * will ignore any byte stream and will not attempt to open
     * a URI connection to the system identifier.</p>
     *
     * @param characterStream The character stream containing the
     *        XML document or other entity.
     * @see #getCharacterStream
     * @see java.io.Reader
     */
    CARAPI SetCharacterStream(
        /* [in] */ IReader* characterStream);

    /**
     * Get the character stream for this input source.
     *
     * @return The character stream, or null if none was supplied.
     * @see #setCharacterStream
     */
    CARAPI GetCharacterStream(
        /* [out] */ IReader** characterStream);

private:
    String mPublicId;
    String mSystemId;
    String mEncoding;
    AutoPtr<IInputStream> mByteStream;
    AutoPtr<IReader> mCharacterStream;
};

} // namespace Sax
} // namespace Xml
} // namespace Org

#endif // __ORG_XML_SAX_CINPUTSOURCE_H__
