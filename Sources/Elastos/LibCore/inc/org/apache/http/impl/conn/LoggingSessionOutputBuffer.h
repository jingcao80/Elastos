
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_LOGGINGSESSIONOUTPUTBUFFER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_LOGGINGSESSIONOUTPUTBUFFER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/impl/conn/Wire.h"

using Org::Apache::Http::IO::ISessionOutputBuffer;
using Org::Apache::Http::IO::IHttpTransportMetrics;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Logs all data written to the wire LOG.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class LoggingSessionOutputBuffer
    : public Object
    , public ISessionOutputBuffer
{
public:
    LoggingSessionOutputBuffer(
        /* [in] */ ISessionOutputBuffer* out,
        /* [in] */ Wire* wire);

    CAR_INTERFACE_DECL()

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Write(
        /* [in] */ Int32 b);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* b);

    CARAPI Flush();

    CARAPI WriteLine(
        /* [in] */ ICharArrayBuffer* buffer);

    CARAPI WriteLine(
        /* [in] */ const String& s);

    CARAPI GetMetrics(
        /* [out] */ IHttpTransportMetrics** metrics);

private:
    /** Original data transmitter. */
    AutoPtr<ISessionOutputBuffer> mOut;

    /** The wire log to use. */
    AutoPtr<Wire> mWire;
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_LOGGINGSESSIONOUTPUTBUFFER_H__
