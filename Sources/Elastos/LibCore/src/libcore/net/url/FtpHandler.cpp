
#include "FtpHandler.h"

using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;

namespace Libcore {
namespace Net {
namespace Url {

CAR_INTERFACE_IMPL(FtpHandler, URLStreamHandler, IFtpHandler)

ECode FtpHandler::constructor()
{
    return NOERROR;
}
ECode FtpHandler::OpenConnection(
    /* [in] */ IURL* u,
    /* [out] */ IURLConnection** urlConnection)
{
    // return new FtpURLConnection(u);
    return NOERROR;
}

ECode FtpHandler::OpenConnection(
    /* [in] */ IURL* u,
    /* [in] */ IProxy* proxy,
    /* [out] */ IURLConnection** urlConnection)
{
    // if (url == null || proxy == null) {
    //     throw new IllegalArgumentException("url == null || proxy == null");
    // }
    // return new FtpURLConnection(url, proxy);
    return NOERROR;
}

ECode FtpHandler::GetDefaultPort(
    /* [out] */ Int32* portNum)
{
    VALIDATE_NOT_NULL(portNum)

    *portNum = 21;
    return NOERROR;
}

} // namespace Url
} // namespace Net
} // namespace Libcore
