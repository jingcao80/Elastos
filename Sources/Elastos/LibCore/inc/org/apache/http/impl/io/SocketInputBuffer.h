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

#ifndef __ORG_APACHE_HTTP_IMPL_IO_SOCKETINPUTBUFFER_H__
#define __ORG_APACHE_HTTP_IMPL_IO_SOCKETINPUTBUFFER_H__

#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/io/AbstractSessionInputBuffer.h"

using Elastos::Net::ISocket;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

extern "C" const InterfaceID EIID_SocketInputBuffer;

class SocketInputBuffer : public AbstractSessionInputBuffer
{
public:
    SocketInputBuffer(
        /* [in] */ ISocket* socket,
        /* [in] */ Int32 buffersize,
        /* [in] */ IHttpParams* params);

    CARAPI IsDataAvailable(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* isAvailable);

    // BEGIN android-added
    /**
     * Returns true if the connection is probably functional. It's insufficient
     * to rely on isDataAvailable() returning normally; that approach cannot
     * distinguish between an exhausted stream and a stream with zero bytes
     * buffered.
     *
     * @hide
     */
    CARAPI IsStale(
        /* [out] */ Boolean* isStale);

private:
    AutoPtr<ISocket> mSocket;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_SOCKETINPUTBUFFER_H__
