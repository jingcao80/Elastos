
#ifndef __ORG_APACHE_HTTP_IMPL_IO_HTTPREQUESTWRITER_H__
#define __ORG_APACHE_HTTP_IMPL_IO_HTTPREQUESTWRITER_H__

#include "org/apache/http/impl/io/AbstractMessageWriter.h"

using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::IO::ISessionOutputBuffer;
using Org::Apache::Http::Message::ILineFormatter;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

class HttpRequestWriter : public AbstractMessageWriter
{
public:
    HttpRequestWriter(
        /* [in] */ ISessionOutputBuffer* buffer,
        /* [in] */ ILineFormatter* formatter,
        /* [in] */ IHttpParams* params);

protected:
    CARAPI WriteHeadLine(
        /* [in] */ IHttpMessage* message);
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_HTTPREQUESTWRITER_H__
