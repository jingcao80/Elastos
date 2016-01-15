
#ifndef __ORG_APACHE_HTTP_IMPL_IO_HTTPREQUESTPARSER_H__
#define __ORG_APACHE_HTTP_IMPL_IO_HTTPREQUESTPARSER_H__

#include "org/apache/http/impl/io/AbstractMessageParser.h"

using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IHttpRequestFactory;
using Org::Apache::Http::IO::ISessionInputBuffer;
using Org::Apache::Http::Message::ILineParser;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

class HttpRequestParser : public AbstractMessageParser
{
public:
    HttpRequestParser(
        /* [in] */ ISessionInputBuffer* buffer,
        /* [in] */ ILineParser* parser,
        /* [in] */ IHttpRequestFactory* requestFactory,
        /* [in] */ IHttpParams* params);

protected:
    CARAPI ParseHead(
        /* [in] */ ISessionInputBuffer* buffer,
        /* [out] */ IHttpMessage** message);

private:
    AutoPtr<IHttpRequestFactory> mRequestFactory;
    AutoPtr<ICharArrayBuffer> mLineBuf;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_HTTPREQUESTPARSER_H__
