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

#ifndef __ORG_APACHE_HTTP_IMPL_IO_IDENTITYOUTPUTSTREAM_H__
#define __ORG_APACHE_HTTP_IMPL_IO_IDENTITYOUTPUTSTREAM_H__

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
 * A stream for writing with an "identity" transport encoding.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 560343 $
 *
 * @since 4.0
 */
class IdentityOutputStream : public OutputStream
{
public:
    IdentityOutputStream(
        /* [in] */ ISessionOutputBuffer* out);

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
    /** The session input buffer */
    AutoPtr<ISessionOutputBuffer> mOut;

    /** True if this stream is closed */
    Boolean mClosed;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_IDENTITYOUTPUTSTREAM_H__
