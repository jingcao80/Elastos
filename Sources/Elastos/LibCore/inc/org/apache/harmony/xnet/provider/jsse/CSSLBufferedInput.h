
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLBUFFEREDINPUT_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLBUFFEREDINPUT_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CSSLBufferedInput.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CSSLBufferedInput)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI Close();

    CARAPI Available(
        /* [out] */ Int32 * pNumber);

    CARAPI Mark(
        /* [in] */ Int32 readLimit);

    CARAPI IsMarkSupported(
        /* [out] */ Boolean * pSupported);

    CARAPI Read(
        /* [out] */ Int32 * pValue);

    CARAPI ReadBytes(
        /* [out] */ ArrayOf<Byte> * pBuffer,
        /* [out] */ Int32 * pNumber);

    CARAPI ReadBytesEx(
        /* [out] */ ArrayOf<Byte> * pBuffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32 * pNumber);

    CARAPI Reset();

    CARAPI Skip(
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64 * pNumber);

    CARAPI GetLock(
        /* [out] */ IInterface ** ppLockObj);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLBUFFEREDINPUT_H__
