
#ifndef __ORG_APACHE_HTTP_IMPL_IO_SOCKETOUTPUTBUFFER_H__
#define __ORG_APACHE_HTTP_IMPL_IO_SOCKETOUTPUTBUFFER_H__

#include "org/apache/http/impl/io/AbstractSessionOutputBuffer.h"

using Elastos::Net::ISocket;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

/**
 * {@link Socket} bound session output buffer.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 560358 $
 *
 * @since 4.0
 */
class SocketOutputBuffer : public AbstractSessionOutputBuffer
{
public:
    SocketOutputBuffer(
        /* [in] */ ISocket* socket,
        /* [in] */ Int32 buffersize,
        /* [in] */ IHttpParams* params);
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_SOCKETOUTPUTBUFFER_H__
