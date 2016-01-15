#ifndef __ELASTOS_NET_CURLENCODER_H__
#define __ELASTOS_NET_CURLENCODER_H__

#include "_Elastos_Net_CURLEncoder.h"
#include "libcore/net/UriCodec.h"
#include "elastos/core/Singleton.h"

using Libcore::Net::UriCodec;

namespace Elastos {
namespace Net {

CarClass(CURLEncoder)
    , public Singleton
    , public IURLEncoder
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Encode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    CARAPI Encode(
        /* [in] */ const String& s,
        /* [in] */ const String& charsetName,
        /* [out] */ String* result);
private:
    class MyUriCodec
        : public UriCodec
    {
        Boolean IsRetained(
            /* [in] */ Char32 c)
        {
            String str(" .-*_");
            return str.IndexOf(c) != -1;
        }
    };

    static MyUriCodec ENCODER;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_URLENCODER_H__
