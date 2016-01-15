
#ifndef __ORG_APACHE_HTTP_IMPL_IO_ChunkedOutputStream_H__
#define __ORG_APACHE_HTTP_IMPL_IO_ChunkedOutputStream_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/io/OutputStream.h"

using Elastos::IO::OutputStream;
using Org::Apache::Http::IO::ISessionOutputBuffer;

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
 * transfer coding. After the stream is Write to the end, it provides access
 * to the trailers, if any.
 * <p>
 * Note that this class NEVER closes the underlying stream, even when close
 * gets called.  Instead, it will Write until the "end" of its chunking on
 * close, which allows for the seamless execution of subsequent HTTP 1.1
 * requests, while not requiring the client to remember to Write the entire
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
class ChunkedOutputStream : public OutputStream
{
public:
    // ----------------------------------------------------------- Constructors
    /**
     * Wraps a session output buffer and chunks the output.
     * @param out the session output buffer to wrap
     * @param bufferSize minimum chunk size (excluding last chunk)
     * @throws IOException
     */
    ChunkedOutputStream(
        /* [in] */ ISessionOutputBuffer* out,
        /* [in] */ Int32 bufferSize);

    /**
     * Wraps a session output buffer and chunks the output. The default buffer
     * size of 2048 was chosen because the chunk overhead is less than 0.5%
     *
     * @param out       the output buffer to wrap
     * @throws IOException
     */
    ChunkedOutputStream(
        /* [in] */ ISessionOutputBuffer* out);

    /**
     * Must be called to ensure the internal cache is flushed and the closing chunk is written.
     * @throws IOException
     */
    CARAPI Finish();

    CARAPI Write(
        /* [in] */ Int32 b);

    /**
     * Writes the array. If the array does not fit within the buffer, it is
     * not split, but rather written out as one large chunk.
     * @param b
     * @throws IOException
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount);

    /**
     * Flushes the content buffer and the underlying stream.
     * @throws IOException
     */
    CARAPI Flush();

    /**
     * Finishes writing to the underlying stream, but does NOT close the underlying stream.
     * @throws IOException
     */
    CARAPI Close();

protected:
    /**
     * Writes the cache out onto the underlying stream
     * @throws IOException
     */
    CARAPI FlushCache();

    /**
     * Writes the cache and bufferToAppend to the underlying stream
     * as one large chunk
     * @param bufferToAppend
     * @param off
     * @param len
     * @throws IOException
     */
    CARAPI FlushCacheWithAppend(
        /* [in] */ ArrayOf<Byte>* bufferToAppend,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI WriteClosingChunk();

private:
    /** The session input buffer */
    AutoPtr<ISessionOutputBuffer> mOut;

    AutoPtr< ArrayOf<Byte> > mCache;

    Int32 mCachePosition;

    Boolean mWroteLastChunk;

    /** True if this stream is closed */
    Boolean mClosed;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_ChunkedOutputStream_H__
