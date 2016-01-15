
#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_FILEHANDLER_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_FILEHANDLER_H__

#include "Elastos.CoreLibrary.Libcore.h"
#include "URLStreamHandler.h"

using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;
using Elastos::Net::URLStreamHandler;

namespace Libcore {
namespace Net {
namespace Url {

class FileHandler
    : public URLStreamHandler
    , public IFileHandler
{
public:
    CAR_INTERFACE_DECL()

    constructor();

    CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [out] */ IURLConnection** urlConnection);

    CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [in] */ Elastos::Net::IProxy* proxy,
        /* [out] */ IURLConnection** urlConnection);

    CARAPI ParseURL(
        /* [in] */ IURL* url,
        /* [in] */ const String& spec,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_FILEHANDLER_H__
