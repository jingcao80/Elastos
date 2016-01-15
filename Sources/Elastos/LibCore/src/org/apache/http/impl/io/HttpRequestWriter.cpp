
#include "org/apache/http/impl/io/HttpRequestWriter.h"
#include "org/apache/http/utility/CCharArrayBuffer.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Utility::CCharArrayBuffer;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

HttpRequestWriter::HttpRequestWriter(
    /* [in] */ ISessionOutputBuffer* buffer,
    /* [in] */ ILineFormatter* formatter,
    /* [in] */ IHttpParams* params)
    : AbstractMessageWriter(buffer, formatter, params)
{}

ECode HttpRequestWriter::WriteHeadLine(
    /* [in] */ IHttpMessage* message)
{
    AutoPtr<IRequestLine> requestLine;
    IHttpRequest::Probe(message)->GetRequestLine((IRequestLine**)&requestLine);
    AutoPtr<ICharArrayBuffer> buffer;
    mLineFormatter->FormatRequestLine(mLineBuffer, requestLine, (ICharArrayBuffer**)&buffer);
    return mSessionBuffer->WriteLine(buffer);
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org