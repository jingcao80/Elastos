
#ifndef __ORG_APACHE_HTTP_IMPL_IO_SOCKETINPUTBUFFER_H__
#define __ORG_APACHE_HTTP_IMPL_IO_SOCKETINPUTBUFFER_H__

#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/io/AbstractSessionInputBuffer.h"

using Elastos::Net::ISocket;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

extern "C" const InterfaceID EIID_SocketInputBuffer;

class SocketInputBuffer : public AbstractSessionInputBuffer
{
public:
    SocketInputBuffer(
        /* [in] */ ISocket* socket,
        /* [in] */ Int32 buffersize,
        /* [in] */ IHttpParams* params);

    CARAPI IsDataAvailable(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* isAvailable);

    // BEGIN android-added
    /**
     * Returns true if the connection is probably functional. It's insufficient
     * to rely on isDataAvailable() returning normally; that approach cannot
     * distinguish between an exhausted stream and a stream with zero bytes
     * buffered.
     *
     * @hide
     */
    CARAPI IsStale(
        /* [out] */ Boolean* isStale);

private:
    AutoPtr<ISocket> mSocket;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_SOCKETINPUTBUFFER_H__
