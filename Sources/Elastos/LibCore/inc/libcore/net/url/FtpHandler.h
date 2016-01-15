
#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_FTPHANDLER_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_FTPHANDLER_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "URLStreamHandler.h"

using Elastos::Net::URLStreamHandler;
using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;

namespace Libcore {
namespace Net {
namespace Url {

class FtpHandler
    : public URLStreamHandler
    , public IFtpHandler
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

protected:
    CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [out] */ IURLConnection** urlConnection);

    CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [in] */ IProxy* proxy,
        /* [out] */ IURLConnection** urlConnection);

    CARAPI GetDefaultPort(
        /* [out] */ Int32* portNum);
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_FTPHANDLER_H__
