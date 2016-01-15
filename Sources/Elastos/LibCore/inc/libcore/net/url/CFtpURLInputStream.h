
#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_CFTPURLINPUTSTREAM_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_CFTPURLINPUTSTREAM_H__

#include "_Libcore_Net_Url_CFtpURLInputStream.h"
#include "FtpURLInputStream.h"

namespace Libcore {
namespace Net {
namespace Url {

CarClass(CFtpURLInputStream), public FtpURLInputStream
{
public:
    CAR_OBJECT_DECL()
};
} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_CFTPURLINPUTSTREAM_H__
