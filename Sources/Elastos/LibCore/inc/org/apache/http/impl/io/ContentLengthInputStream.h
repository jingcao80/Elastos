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

#ifndef __ORG_APACHE_HTTP_IMPL_IO_CONTENTLENGHTINPUTSTREAM_H__
#define __ORG_APACHE_HTTP_IMPL_IO_CONTENTLENGHTINPUTSTREAM_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/io/InputStream.h"

using Elastos::IO::InputStream;
using Org::Apache::Http::IO::ISessionInputBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

/**
 * Stream that cuts off after a specified number of bytes.
 * Note that this class NEVER closes the underlying stream, even when close
 * gets called.  Instead, it will read until the "end" of its chunking on
 * close, which allows for the seamless execution of subsequent HTTP 1.1
 * requests, while not requiring the client to remember to read the entire
 * contents of the response.
 *
 * <p>Implementation note: Choices abound. One approach would pass
 * through the {@link InputStream#mark} and {@link InputStream#reset} calls to
 * the underlying stream.  That's tricky, though, because you then have to
 * start duplicating the work of keeping track of how much a reset rewinds.
 * Further, you have to watch out for the "readLimit", and since the semantics
 * for the readLimit leave room for differing implementations, you might get
 * into a lot of trouble.</p>
 *
 * <p>Alternatively, you could make this class extend
 * {@link java.io.BufferedInputStream}
 * and then use the protected members of that class to avoid duplicated effort.
 * That solution has the side effect of adding yet another possible layer of
 * buffering.</p>
 *
 * <p>Then, there is the simple choice, which this takes - simply don't
 * support {@link InputStream#mark} and {@link InputStream#reset}.  That choice
 * has the added benefit of keeping this class very simple.</p>
 *
 * @author Ortwin Glueck
 * @author Eric Johnson
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 *
 * @since 4.0
 */
class ContentLengthInputStream : public InputStream
{
public:
    /**
     * Creates a new length limited stream
     *
     * @param in The session input buffer to wrap
     * @param contentLength The maximum number of bytes that can be read from
     * the stream. Subsequent read operations will return -1.
     */
    ContentLengthInputStream(
        /* [in] */ ISessionInputBuffer* in,
        /* [in] */ Int64 contentLength);

    /**
     * <p>Reads until the end of the known length of content.</p>
     *
     * <p>Does not close the underlying socket input, but instead leaves it
     * primed to parse the next response.</p>
     * @throws IOException If an IO problem occurs.
     */
    CARAPI Close();

    using InputStream::Read;

    /**
     * Read the next byte from the stream
     * @return The next byte or -1 if the end of stream has been reached.
     * @throws IOException If an IO problem occurs
     * @see java.io.InputStream#read()
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Does standard {@link InputStream#read(byte[], int, int)} behavior, but
     * also notifies the watcher when the contents have been consumed.
     *
     * @param b     The byte array to fill.
     * @param off   Start filling at this position.
     * @param len   The number of bytes to attempt to read.
     * @return The number of bytes read, or -1 if the end of content has been
     *  reached.
     *
     * @throws java.io.IOException Should an error occur on the wrapped stream.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);

    /**
     * Read more bytes from the stream.
     * @param b The byte array to put the new data in.
     * @return The number of bytes read into the buffer.
     * @throws IOException If an IO problem occurs
     * @see java.io.InputStream#read(byte[])
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [out] */ Int32* number);

    /**
     * Skips and discards a number of bytes from the input stream.
     * @param n The number of bytes to skip.
     * @return The actual number of bytes skipped. <= 0 if no bytes
     * are skipped.
     * @throws IOException If an error occurs while skipping bytes.
     * @see InputStream#skip(long)
     */
    CARAPI Skip(
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* number);

private:
    static const Int32 BUFFER_SIZE = 2048;

    /**
     * The maximum number of bytes that can be read from the stream. Subsequent
     * read operations will return -1.
     */
    Int64 mContentLength;

    /** The current position */
    Int64 mPos;

    /** True if this stream is closed */
    Boolean mClosed;

    /**
     * Wrapped input stream that all calls are delegated to.
     */
    AutoPtr<ISessionInputBuffer> mIn;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_CONTENTLENGHTINPUTSTREAM_H__
