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

#ifndef __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTSESSIONINPUTBUFFER_H__
#define __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTSESSIONINPUTBUFFER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.IO.h"
#include "org/apache/http/impl/io/HttpTransportMetricsImpl.h"
#include "elastos/core/Object.h"

using Elastos::IO::IInputStream;
using Org::Apache::Http::IO::ISessionInputBuffer;
using Org::Apache::Http::IO::IHttpTransportMetrics;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Utility::IByteArrayBuffer;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

class AbstractSessionInputBuffer
    : public Object
    , public ISessionInputBuffer
{
public:
    AbstractSessionInputBuffer();

    CAR_INTERFACE_DECL()

    CARAPI Read(
        /* [out] */ Int32* count);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [out] */ Int32* count);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [out] */ Int32* count);

    CARAPI ReadLine(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [out] */ Int32* count);

    CARAPI ReadLine(
        /* [out] */ String* line);

    CARAPI GetMetrics(
        /* [out] */ IHttpTransportMetrics** metrics);

protected:
    CARAPI Init(
        /* [in] */ IInputStream* instream,
        /* [in] */ Int32 buffersize,
        /* [in] */ IHttpParams* params);

    CARAPI_(Int32) FillBuffer();

    CARAPI_(Boolean) HasBufferedData();

    CARAPI_(Int32) LocateLF();

    CARAPI LineFromLineBuffer(
        /* [in] */ ICharArrayBuffer* charbuffer,
        /* [out] */ Int32* len);

    CARAPI LineFromReadBuffer(
        /* [in] */ ICharArrayBuffer* charbuffer,
        /* [in] */ Int32 pos,
        /* [out] */ Int32* len);

private:
    AutoPtr<IInputStream> mInstream;
    AutoPtr< ArrayOf<Byte> > mBuffer;
    Int32 mBufferpos;
    Int32 mBufferlen;
    AutoPtr<IByteArrayBuffer> mLinebuffer;
    String mCharset;
    Boolean mAscii;
    Int32 mMaxLineLen;
    AutoPtr<HttpTransportMetricsImpl> mMetrics;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTSESSIONINPUTBUFFER_H__
