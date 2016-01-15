
#ifndef  __ELASTOSX_XML_TRANSFORM_STREAM_CSTREAMSOURCE_H__
#define  __ELASTOSX_XML_TRANSFORM_STREAM_CSTREAMSOURCE_H__

#include "Elastos.CoreLibrary.IO.h"
#include "_Elastosx_Xml_Transform_Stream_CStreamSource.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
using Elastos::IO::IFile;

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Stream {

/**
 * <p>Acts as an holder for a transformation Source in the form
 * of a stream of XML markup.</p>
 *
 * <p><em>Note:</em> Due to their internal use of either a {@link Reader} or {@link InputStream} instance,
 * <code>StreamSource</code> instances may only be used once.</p>
 *
 * @author <a href="Jeff.Suttor@Sun.com">Jeff Suttor</a>
 * @version $Revision: 829971 $, $Date: 2009-10-26 14:15:39 -0700 (Mon, 26 Oct 2009) $
 */
CarClass(CStreamSource)
    , public Object
    , public IStreamSource
    , public ISource
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * <p>Zero-argument default constructor.  If this constructor is used, and
     * no Stream source is set using
     * {@link #setInputStream(java.io.InputStream inputStream)} or
     * {@link #setReader(java.io.Reader reader)}, then the
     * <code>Transformer</code> will
     * create an empty source {@link java.io.InputStream} using
     * {@link java.io.InputStream#InputStream() new InputStream()}.</p>
     *
     * @see javax.xml.transform.Transformer#transform(Source xmlSource, Result outputTarget)
     */
    CARAPI constructor();

    /**
     * Construct a StreamSource from a byte stream.  Normally,
     * a stream should be used rather than a reader, so
     * the XML parser can resolve character encoding specified
     * by the XML declaration.
     *
     * <p>If this constructor is used to process a stylesheet, normally
     * setSystemId should also be called, so that relative URI references
     * can be resolved.</p>
     *
     * @param inputStream A valid InputStream reference to an XML stream.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* inputStream);

    /**
     * Construct a StreamSource from a byte stream.  Normally,
     * a stream should be used rather than a reader, so that
     * the XML parser can resolve character encoding specified
     * by the XML declaration.
     *
     * <p>This constructor allows the systemID to be set in addition
     * to the input stream, which allows relative URIs
     * to be processed.</p>
     *
     * @param inputStream A valid InputStream reference to an XML stream.
     * @param systemId Must be a String that conforms to the URI syntax.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* inputStream,
        /* [in] */ const String& systemId);

    /**
     * Construct a StreamSource from a character reader.  Normally,
     * a stream should be used rather than a reader, so that
     * the XML parser can resolve character encoding specified
     * by the XML declaration.  However, in many cases the encoding
     * of the input stream is already resolved, as in the case of
     * reading XML from a StringReader.
     *
     * @param reader A valid Reader reference to an XML character stream.
     */
    CARAPI constructor(
        /* [in] */ IReader* reader);

    /**
     * Construct a StreamSource from a character reader.  Normally,
     * a stream should be used rather than a reader, so that
     * the XML parser may resolve character encoding specified
     * by the XML declaration.  However, in many cases the encoding
     * of the input stream is already resolved, as in the case of
     * reading XML from a StringReader.
     *
     * @param reader A valid Reader reference to an XML character stream.
     * @param systemId Must be a String that conforms to the URI syntax.
     */
    CARAPI constructor(
        /* [in] */ IReader* reader,
        /* [in] */ const String& systemId);

    /**
     * Construct a StreamSource from a URL.
     *
     * @param systemId Must be a String that conforms to the URI syntax.
     */
    CARAPI constructor(
        /* [in] */ const String& systemId);

    /**
     * Construct a StreamSource from a File.
     *
     * @param f Must a non-null File reference.
     */
    CARAPI constructor(
        /* [in] */ IFile* f);

    /**
     * Set the byte stream to be used as input.  Normally,
     * a stream should be used rather than a reader, so that
     * the XML parser can resolve character encoding specified
     * by the XML declaration.
     *
     * <p>If this Source object is used to process a stylesheet, normally
     * setSystemId should also be called, so that relative URL references
     * can be resolved.</p>
     *
     * @param inputStream A valid InputStream reference to an XML stream.
     */
    CARAPI SetInputStream(
        /* [in] */ IInputStream* inputStream);

    /**
     * Get the byte stream that was set with setByteStream.
     *
     * @return The byte stream that was set with setByteStream, or null
     * if setByteStream or the ByteStream constructor was not called.
     */
    CARAPI GetInputStream(
        /* [out] */ IInputStream** stream);

    /**
     * Set the input to be a character reader.  Normally,
     * a stream should be used rather than a reader, so that
     * the XML parser can resolve character encoding specified
     * by the XML declaration.  However, in many cases the encoding
     * of the input stream is already resolved, as in the case of
     * reading XML from a StringReader.
     *
     * @param reader A valid Reader reference to an XML CharacterStream.
     */
    CARAPI SetReader(
        /* [in] */ IReader* reader);

    /**
     * Get the character stream that was set with setReader.
     *
     * @return The character stream that was set with setReader, or null
     * if setReader or the Reader constructor was not called.
     */
    CARAPI GetReader(
        /* [out] */ IReader** reader);

    /**
     * Set the public identifier for this Source.
     *
     * <p>The public identifier is always optional: if the application
     * writer includes one, it will be provided as part of the
     * location information.</p>
     *
     * @param publicId The public identifier as a string.
     */
    CARAPI SetPublicId(
        /* [in] */ const String& publicId);

    /**
     * Get the public identifier that was set with setPublicId.
     *
     * @return The public identifier that was set with setPublicId, or null
     * if setPublicId was not called.
     */
    CARAPI GetPublicId(
        /* [out] */ String* id);

    /**
     * Set the system identifier for this Source.
     *
     * <p>The system identifier is optional if there is a byte stream
     * or a character stream, but it is still useful to provide one,
     * since the application can use it to resolve relative URIs
     * and can include it in error messages and warnings (the parser
     * will attempt to open a connection to the URI only if
     * there is no byte stream or character stream specified).</p>
     *
     * @param systemId The system identifier as a URL string.
     */
    CARAPI SetSystemId(
        /* [in] */ const String& systemId);

    /**
     * Get the system identifier that was set with setSystemId.
     *
     * @return The system identifier that was set with setSystemId, or null
     * if setSystemId was not called.
     */
    CARAPI GetSystemId(
        /* [out] */ String* id);

    /**
     * Set the system ID from a File reference.
     *
     * @param f Must a non-null File reference.
     */
    CARAPI SetSystemId(
        /* [in] */ IFile* f);

private:
    //////////////////////////////////////////////////////////////////////
    // Internal state.
    //////////////////////////////////////////////////////////////////////

    /**
     * The public identifier for this input source, or null.
     */
    String mPublicId;

    /**
     * The system identifier as a URL string, or null.
     */
    String mSystemId;

    /**
     * The byte stream for this Source, or null.
     */
    AutoPtr<IInputStream> mInputStream;

    /**
     * The character stream for this Source, or null.
     */
    AutoPtr<IReader> mReader;
};

} // namespace Stream
} // namespace Transform
} // namespace Xml
} // namespace Elastosx

#endif // __ELASTOSX_XML_TRANSFORM_STREAM_CSTREAMSOURCE_H__
