#ifndef __ELASTOS_NET_CURLDCODER_H__
#define __ELASTOS_NET_CURLDCODER_H__

#include "_Elastos_Net_CURLDecoder.h"
#include "Singleton.h"

namespace Elastos {
namespace Net {

CarClass(CURLDecoder)
    , public Singleton
    , public IURLDecoder
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI Decode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    CARAPI Decode(
        /* [in] */ const String& s,
        /* [in] */ const String& charsetName,
        /* [out] */ String* result);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_URLENCODER_H__
