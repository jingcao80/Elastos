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

#include "org/apache/http/impl/io/SocketInputBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

extern "C" const InterfaceID EIID_SocketInputBuffer =
        { 0x3a5b9442, 0x2015, 0x4783, { 0x9d, 0xf0, 0xd4, 0x79, 0x62, 0x39, 0x2c, 0xe5 } };

SocketInputBuffer::SocketInputBuffer(
    /* [in] */ ISocket* socket,
    /* [in] */ Int32 buffersize,
    /* [in] */ IHttpParams* params)
    : AbstractSessionInputBuffer()
{
    if (socket == NULL) {
        Logger::E("SocketInputBuffer", "Socket may not be null");
        assert(0);
        // throw new IllegalArgumentException("Socket may not be null");
    }
    mSocket = socket;
    // BEGIN android-changed
    // Workaround for http://b/3514259. We take 'buffersize' as a hint in
    // the weakest sense, and always use an 8KiB heap buffer and leave the
    // kernel buffer size alone, trusting the system to have set a
    // network-appropriate default.
    AutoPtr<IInputStream> in;
    socket->GetInputStream((IInputStream**)&in);
    Init(in, 8192, params);
    // END android-changed
}

ECode SocketInputBuffer::IsDataAvailable(
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean* isAvailable)
{
    VALIDATE_NOT_NULL(isAvailable)
    Boolean result = HasBufferedData();
    if (!result) {
        Int32 oldtimeout;
        mSocket->GetSoTimeout(&oldtimeout);
        // try {
        mSocket->SetSoTimeout(timeout);
        FillBuffer();
        result = HasBufferedData();
        // } catch (InterruptedIOException e) {
        //     if (!(e instanceof SocketTimeoutException)) {
        //         throw e;
        //     }
        // } finally {
        //     socket.setSoTimeout(oldtimeout);
        // }
        mSocket->SetSoTimeout(oldtimeout);
    }
    *isAvailable = result;
    return NOERROR;
}

ECode SocketInputBuffer::IsStale(
    /* [out] */ Boolean* isStale)
{
    VALIDATE_NOT_NULL(isStale)
    if (HasBufferedData()) {
        *isStale = FALSE;
        return NOERROR;
    }
    Int32 oldtimeout;
    mSocket->GetSoTimeout(&oldtimeout);
    // try {
    mSocket->SetSoTimeout(1);
    *isStale = FillBuffer() == -1;
    // } catch (SocketTimeoutException e) {
    //     return false; // the connection is not stale; hooray
    // } catch (IOException e) {
    //     return true; // the connection is stale, the read or soTimeout failed.
    // } finally {
    //     this.socket.setSoTimeout(oldTimeout);
    // }
    mSocket->SetSoTimeout(oldtimeout);
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org