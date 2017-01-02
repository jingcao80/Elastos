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

#ifndef __ORG_APACHE_HTTP_IMPL_IO_CONTENTLENGTHOUTPUTSTREAM_H__
#define __ORG_APACHE_HTTP_IMPL_IO_CONTENTLENGTHOUTPUTSTREAM_H__

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
 * A stream wrapper that closes itself after a defined number of bytes.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 560343 $
 *
 * @since 4.0
 */
class ContentLengthOutputStream : public OutputStream
{
public:
    /**
     * Creates a new length limited stream
     *
     * @param out The data transmitter to wrap
     * @param contentLength The maximum number of bytes that can be written to
     * the stream. Subsequent write operations will be ignored.
     *
     * @since 4.0
     */
    ContentLengthOutputStream(
        /* [in] */ ISessionOutputBuffer* out,
        /* [in] */ Int64 contentLength);

    /**
     * <p>Does not close the underlying socket output.</p>
     *
     * @throws IOException If an I/O problem occurs.
     */
    CARAPI Close();

    CARAPI Flush();

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI Write(
        /* [in] */ Int32 b);

private:
    /**
     * Wrapped session outbut buffer.
     */
    AutoPtr<ISessionOutputBuffer> mOut;

    /**
     * The maximum number of bytes that can be written the stream. Subsequent
     * write operations will be ignored.
     */
    Int64 mContentLength;

    /** Total bytes written */
    Int64 mTotal;

    /** True if this stream is closed */
    Boolean mClosed;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_CONTENTLENGTHOUTPUTSTREAM_H__
