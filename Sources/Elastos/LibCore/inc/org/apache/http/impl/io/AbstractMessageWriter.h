
#ifndef __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTMESSAGEWRITER_H__
#define __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTMESSAGEWRITER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Message::ILineFormatter;
using Org::Apache::Http::IO::IHttpMessageWriter;
using Org::Apache::Http::IO::ISessionOutputBuffer;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

class AbstractMessageWriter
    : public Object
    , public IHttpMessageWriter
{
public:
    AbstractMessageWriter(
        /* [in] */ ISessionOutputBuffer* buffer,
        /* [in] */ ILineFormatter* formatter,
        /* [in] */ IHttpParams* params);

    CAR_INTERFACE_DECL()

    CARAPI Write(
        /* [in] */ IHttpMessage* message);

protected:
    virtual CARAPI WriteHeadLine(
        /* [in] */ IHttpMessage* message) = 0;

protected:
    AutoPtr<ISessionOutputBuffer> mSessionBuffer;
    AutoPtr<ICharArrayBuffer> mLineBuffer;
    AutoPtr<ILineFormatter> mLineFormatter;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTMESSAGEWRITER_H__
