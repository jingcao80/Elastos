
#ifndef __ORG_APACHE_HTTP_IMPL_IO_CHUNKEDINPUTSTREAM_H__
#define __ORG_APACHE_HTTP_IMPL_IO_CHUNKEDINPUTSTREAM_H__

#include "elastos/io/InputStream.h"

using Elastos::IO::InputStream;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::IO::ISessionInputBuffer;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

/**
 * Implements chunked transfer coding.
 * See <a href="http://www.w3.org/Protocols/rfc2616/rfc2616.txt">RFC 2616</a>,
 * <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.6">section 3.6.1</a>.
 * It transparently coalesces chunks of a HTTP stream that uses chunked
 * transfer coding. After the stream is read to the end, it provides access
 * to the trailers, if any.
 * <p>
 * Note that this class NEVER closes the underlying stream, even when close
 * gets called.  Instead, it will read until the "end" of its chunking on
 * close, which allows for the seamless execution of subsequent HTTP 1.1
 * requests, while not requiring the client to remember to read the entire
 * contents of the response.
 * </p>
 *
 * @author Ortwin Glueck
 * @author Sean C. Sullivan
 * @author Martin Elwin
 * @author Eric Johnson
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author Michael Becke
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 *
 */
class ChunkedInputStream : public InputStream
{
public:
    ChunkedInputStream(
        /* [in] */ ISessionInputBuffer* in);

    /**
     * <p> Returns all the data in a chunked stream in coalesced form. A chunk
     * is followed by a CRLF. The method returns -1 as soon as a chunksize of 0
     * is detected.</p>
     *
     * <p> Trailer headers are read automcatically at the end of the stream and
     * can be obtained with the getResponseFooters() method.</p>
     *
     * @return -1 of the end of the stream has been reached or the next data
     * byte
     * @throws IOException If an IO problem occurs
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Read some bytes from the stream.
     * @param b The byte array that will hold the contents from the stream.
     * @param off The offset into the byte array at which bytes will start to be
     * placed.
     * @param len the maximum number of bytes that can be returned.
     * @return The number of bytes returned or -1 if the end of stream has been
     * reached.
     * @see java.io.InputStream#read(byte[], int, int)
     * @throws IOException if an IO problem occurs.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    /**
     * Read some bytes from the stream.
     * @param b The byte array that will hold the contents from the stream.
     * @return The number of bytes returned or -1 if the end of stream has been
     * reached.
     * @see java.io.InputStream#read(byte[])
     * @throws IOException if an IO problem occurs.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

    /**
     * Upon close, this reads the remainder of the chunked message,
     * leaving the underlying socket at a position to start reading the
     * next response without scanning.
     * @throws IOException If an IO problem occurs.
     */
    CARAPI Close();

    /**
     * Upon close, this reads the remainder of the chunked message,
     * leaving the underlying socket at a position to start reading the
     * next response without scanning.
     * @throws IOException If an IO problem occurs.
     */
    CARAPI_(AutoPtr< ArrayOf<IHeader*> >) GetFooters();

private:
    /**
     * Read the next chunk.
     * @throws IOException If an IO error occurs.
     */
    CARAPI NextChunk();

    /**
     * Expects the stream to start with a chunksize in hex with optional
     * comments after a semicolon. The line must end with a CRLF: "a3; some
     * comment\r\n" Positions the stream at the start of the next line.
     *
     * @param in The new input stream.
     * @param required <tt>true<tt/> if a valid chunk must be present,
     *                 <tt>false<tt/> otherwise.
     *
     * @return the chunk size as integer
     *
     * @throws IOException when the chunk size could not be parsed
     */
    CARAPI GetChunkSize(
        /* [out] */ Int32* size);

    /**
     * Reads and stores the Trailer headers.
     * @throws IOException If an IO problem occurs
     */
    CARAPI ParseTrailerHeaders();

    /**
     * Exhaust an input stream, reading until EOF has been encountered.
     *
     * <p>Note that this function is intended as a non-public utility.
     * This is a little weird, but it seemed silly to make a utility
     * class for this one function, so instead it is just static and
     * shared that way.</p>
     *
     * @param inStream The {@link InputStream} to exhaust.
     * @throws IOException If an IO problem occurs
     */
    static CARAPI ExhaustInputStream(
        /* [in] */ IInputStream* inStream);

private:
    /** The session input buffer */
    AutoPtr<ISessionInputBuffer> mIn;

    AutoPtr<ICharArrayBuffer> mBuffer;

    /** The chunk size */
    Int32 mChunkSize;

    /** The current position within the current chunk */
    Int32 mPos;

    /** True if we'are at the beginning of stream */
    Boolean mBof;

    /** True if we've reached the end of stream */
    Boolean mEof;

    /** True if this stream is closed */
    Boolean mClosed;

    AutoPtr< ArrayOf<IHeader*> > mFooters;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_CHUNKEDINPUTSTREAM_H__
