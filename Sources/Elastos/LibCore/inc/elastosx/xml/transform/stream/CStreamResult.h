
#ifndef  __ELASTOSX_XML_TRANSFORM_STREAM_CSTREAMRESULT_H__
#define  __ELASTOSX_XML_TRANSFORM_STREAM_CSTREAMRESULT_H__

#include "Elastos.CoreLibrary.IO.h"
#include "_Elastosx_Xml_Transform_Stream_CStreamResult.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::IOutputStream;
using Elastos::IO::IWriter;
using Elastos::IO::IFile;

namespace Elastosx {
namespace Xml {
namespace Transform {
namespace Stream {

/**
 * <p>Acts as an holder for a transformation result,
 * which may be XML, plain Text, HTML, or some other form of markup.</p>
 *
 * @author <a href="Jeff.Suttor@Sun.com">Jeff Suttor</a>
 */
CarClass(CStreamResult)
    , public Object
    , public IStreamResult
    , public IResult
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Zero-argument default constructor.
     */
    CARAPI constructor();

    /**
     * Construct a StreamResult from a byte stream.  Normally,
     * a stream should be used rather than a reader, so that
     * the transformer may use instructions contained in the
     * transformation instructions to control the encoding.
     *
     * @param outputStream A valid OutputStream reference.
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* outputStream);

    /**
     * Construct a StreamResult from a character stream.  Normally,
     * a stream should be used rather than a reader, so that
     * the transformer may use instructions contained in the
     * transformation instructions to control the encoding.  However,
     * there are times when it is useful to write to a character
     * stream, such as when using a StringWriter.
     *
     * @param writer  A valid Writer reference.
     */
    CARAPI constructor(
        /* [in] */ IWriter* writer);

    /**
     * Construct a StreamResult from a URL.
     *
     * @param systemId Must be a String that conforms to the URI syntax.
     */
    CARAPI constructor(
        /* [in] */ const String& systemId);

    /**
     * Construct a StreamResult from a File.
     *
     * @param f Must a non-null File reference.
     */
    CARAPI constructor(
        /* [in] */ IFile* f);

    /**
     * Set the ByteStream that is to be written to.  Normally,
     * a stream should be used rather than a reader, so that
     * the transformer may use instructions contained in the
     * transformation instructions to control the encoding.
     *
     * @param outputStream A valid OutputStream reference.
     */
    CARAPI SetOutputStream(
        /* [in] */ IOutputStream* outputStream);

    /**
     * Get the byte stream that was set with setOutputStream.
     *
     * @return The byte stream that was set with setOutputStream, or null
     * if setOutputStream or the ByteStream constructor was not called.
     */
    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** stream);

    /**
     * Set the writer that is to receive the result.  Normally,
     * a stream should be used rather than a writer, so that
     * the transformer may use instructions contained in the
     * transformation instructions to control the encoding.  However,
     * there are times when it is useful to write to a writer,
     * such as when using a StringWriter.
     *
     * @param writer  A valid Writer reference.
     */
    CARAPI SetWriter(
        /* [in] */ IWriter* writer);

    /**
     * Get the character stream that was set with setWriter.
     *
     * @return The character stream that was set with setWriter, or null
     * if setWriter or the Writer constructor was not called.
     */
    CARAPI GetWriter(
        /* [out] */ IWriter** writer);

    /**
     * Set the systemID that may be used in association
     * with the byte or character stream, or, if neither is set, use
     * this value as a writeable URI (probably a file name).
     *
     * @param systemId The system identifier as a URI string.
     */
    CARAPI SetSystemId(
        /* [in] */ const String& systemId);

    /**
     * <p>Set the system ID from a <code>File</code> reference.</p>
     *
     * <p>Note the use of {@link File#toURI()} and {@link File#toURL()}.
     * <code>toURI()</code> is preferred and used if possible.
     * To allow JAXP 1.3 to run on J2SE 1.3, <code>toURL()</code>
     * is used if a {@link NoSuchMethodException} is thrown by the attempt
     * to use <code>toURI()</code>.</p>
     *
     * @param f Must a non-null File reference.
     */
    CARAPI SetSystemId(
        /* [in] */ IFile* f);

    /**
     * Get the system identifier that was set with setSystemId.
     *
     * @return The system identifier that was set with setSystemId, or null
     * if setSystemId was not called.
     */
    CARAPI GetSystemId(
        /* [out] */ String* id);

private:
    //////////////////////////////////////////////////////////////////////
    // Internal state.
    //////////////////////////////////////////////////////////////////////

    /**
     * The systemID that may be used in association
     * with the byte or character stream, or, if neither is set, use
     * this value as a writeable URI (probably a file name).
     */
    String mSystemId;

    /**
     * The byte stream that is to be written to.
     */
    AutoPtr<IOutputStream> mOutputStream;

    /**
     * The character stream that is to be written to.
     */
    AutoPtr<IWriter> mWriter;
};

} // namespace Stream
} // namespace Transform
} // namespace Xml
} // namespace Elastosx

#endif // __ELASTOSX_XML_TRANSFORM_STREAM_CSTREAMRESULT_H__
