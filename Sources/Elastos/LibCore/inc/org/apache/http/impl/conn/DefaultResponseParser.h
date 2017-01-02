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

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_DefaultResponseParser_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_DefaultResponseParser_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/impl/io/AbstractMessageParser.h"

using Org::Apache::Http::IHttpResponseFactory;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Impl::IO::AbstractMessageParser;
using Org::Apache::Http::IO::ISessionInputBuffer;
using Org::Apache::Http::Message::ILineParser;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

class DefaultResponseParser : public AbstractMessageParser
{
public:
    DefaultResponseParser(
        /* [in] */ ISessionInputBuffer* buffer,
        /* [in] */ ILineParser* parser,
        /* [in] */ IHttpResponseFactory* responseFactory,
        /* [in] */ IHttpParams* params);

protected:
    CARAPI ParseHead(
        /* [in] */ ISessionInputBuffer* sessionBuffer,
        /* [out] */ IHttpMessage** msg);

private:
    AutoPtr<IHttpResponseFactory> mResponseFactory;
    AutoPtr<ICharArrayBuffer> mLineBuf;
    Int32 mMaxGarbageLines;

};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_DefaultResponseParser_H__
