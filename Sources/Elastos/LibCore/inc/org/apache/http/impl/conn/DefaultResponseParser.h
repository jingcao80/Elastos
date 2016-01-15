
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
