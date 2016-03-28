
#ifndef __ELASTOS_NET_URLSTREAMHANDLER_H__
#define __ELASTOS_NET_URLSTREAMHANDLER_H__

#include "Elastos.CoreLibrary.Net.h"
#include "Object.h"

namespace Elastos {
namespace Net {

class URLStreamHandler
    : public Object
    , public IURLStreamHandler
{
public:
    CAR_INTERFACE_DECL()

    URLStreamHandler();

    virtual ~URLStreamHandler();

    virtual CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [out] */ IURLConnection** urlConnection) = 0;

    CARAPI OpenConnection(
        /* [in] */ IURL* u,
        /* [in] */ IProxy* proxy,
        /* [out] */ IURLConnection** urlConnection);

    CARAPI ParseURL(
        /* [in] */ IURL* url,
        /* [in] */ const String& spec,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI SetURL(
        /* [in] */ IURL* u,
        /* [in] */ const String& protocol,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& file,
        /* [in] */ const String& ref);

    CARAPI SetURL(
        /* [in] */ IURL* u,
        /* [in] */ const String& protocol,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& authority,
        /* [in] */ const String& userInfo,
        /* [in] */ const String& file,
        /* [in] */ const String& query,
        /* [in] */ const String& ref);

    CARAPI ToExternalForm(
        /* [in] */ IURL* url,
        /* [in] */ Boolean escapeIllegalCharacters,
        /* [out] */ String* s);

    CARAPI ToExternalForm(
        /* [in] */ IURL* url,
        /* [out] */ String* s);

    CARAPI Equals(
        /* [in] */ IURL* url1,
        /* [in] */ IURL* url2,
        /* [out] */ Boolean* isEquals);

    CARAPI GetDefaultPort(
        /* [out] */ Int32* portNum);

    CARAPI GetHostAddress(
        /* [in] */ IURL* url,
        /* [out] */ IInetAddress** hostAddress);

    CARAPI GetHashCode(
        /* [in] */ IURL* url,
        /* [out] */ Int32* code);

    CARAPI HostsEqual(
        /* [in] */ IURL* a,
        /* [in] */ IURL* b,
        /* [out] */ Boolean* isEqual);

    CARAPI SameFile(
        /* [in] */ IURL* url1,
        /* [in] */ IURL* url2,
        /* [out] */ Boolean* isSame);

private:
    static CARAPI_(String) RelativePath(
        /* [in] */ const String& base,
        /* [in] */ const String& path);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_URLSTREAMHANDLER_H__
