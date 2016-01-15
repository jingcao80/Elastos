
#include "org/apache/http/impl/io/HttpResponseWriter.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Utility::CCharArrayBuffer;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

HttpResponseWriter::HttpResponseWriter(
    /* [in] */ ISessionOutputBuffer* buffer,
    /* [in] */ ILineFormatter* formatter,
    /* [in] */ IHttpParams* params)
    : AbstractMessageWriter(buffer, formatter, params)
{}

ECode HttpResponseWriter::WriteHeadLine(
    /* [in] */ IHttpMessage* message)
{
    AutoPtr<IHttpResponse> response = IHttpResponse::Probe(message);
    AutoPtr<IStatusLine> statusLine;
    response->GetStatusLine((IStatusLine**)&statusLine);
    AutoPtr<ICharArrayBuffer> buffer;
    mLineFormatter->FormatStatusLine(mLineBuffer, statusLine, (ICharArrayBuffer**)&buffer);
    return mSessionBuffer->WriteLine(buffer);
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org