
#ifndef __LIBCORE_NET_URL_LIBCORE_NET_URI_JARHANDLER_H__
#define __LIBCORE_NET_URL_LIBCORE_NET_URI_JARHANDLER_H__

#include "_Libcore_Net_Url_CJarHandler.h"
#include "URLStreamHandler.h"

using Elastos::Net::URLStreamHandler;
using Elastos::Net::IURLConnection;
using Elastos::Net::IURL;
using Elastos::Net::IInetAddress;

namespace Libcore {
namespace Net {
namespace Url {

CarClass(CJarHandler)
    , public URLStreamHandler
    , public IJarHandler
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();

    virtual CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [out] */ IURLConnection** urlConnection);

    CARAPI ParseURL(
        /* [in] */ IURL* url,
        /* [in] */ const String& spec,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI ToExternalForm(
        /* [in] */ IURL* url,
        /* [out] */ String* s);
};

} // namespace Url
} // namespace Net
} // namespace Libcore

#endif //__LIBCORE_NET_URL_LIBCORE_NET_URI_JARHANDLER_H__
