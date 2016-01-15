
#ifndef __ELASTOS_NET_URLDECODER_H__
#define __ELASTOS_NET_URLDECODER_H__

#include <eltypes.h>

namespace Elastos {
namespace Net {

class URLDecoder
{
public:
    static CARAPI Decode(
        /* [in] */ const String& s,
        /* [out] */ String* decodedStr);

    static CARAPI Decode(
        /* [in] */ const String& s,
        /* [in] */ const String& encoding,
        /* [out] */ String* decodedStr);

private:
    URLDecoder() {}

};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_URLDECODER_H__
