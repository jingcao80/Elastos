
#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_CFTPHANDLER_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_CFTPHANDLER_H__

#include "_Libcore_Net_Url_CFtpHandler.h"
#include "FtpHandler.h"

using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;

namespace Libcore {
namespace Net {
namespace Url {

CarClass(CFtpHandler), public FtpHandler
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_CFTPHANDLER_H__
