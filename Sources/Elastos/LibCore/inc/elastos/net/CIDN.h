
#ifndef __ELASTOS_NET_CIDN_H__
#define __ELASTOS_NET_CIDN_H__

#include "_Elastos_Net_CIDN.h"
#include "Singleton.h"


namespace Elastos {
namespace Net {

CarClass(CIDN)
    , public Singleton
    , public IIDN
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ToASCII(
        /* [in] */ const String& input,
        /* [in] */ Int32 flags,
        /* [out] */ String* result);

    CARAPI ToASCII(
        /* [in] */ const String& input,
        /* [out] */ String* result);

    CARAPI ToUnicode(
        /* [in] */ const String& input,
        /* [in] */ Int32 flags,
        /* [out] */ String* result);

    CARAPI ToUnicode(
        /* [in] */ const String& input,
        /* [out] */ String* result);

private:
    static CARAPI Convert(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean toAscii,
        /* [out] */ String* result);

    static CARAPI NativeConvert(
        /* [in] */ const String& s,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean toAscii,
        /* [out] */ String* result);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CIDN_H__
