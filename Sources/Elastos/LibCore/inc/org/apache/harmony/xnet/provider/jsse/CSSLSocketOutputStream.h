
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSOCKETOUTPUTSTREAM_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSOCKETOUTPUTSTREAM_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CSSLSocketOutputStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CSSLSocketOutputStream)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI Close();

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI WriteBytes(
        /* [in] */ const ArrayOf<Byte> & buffer);

    CARAPI WriteBytesEx(
        /* [in] */ const ArrayOf<Byte> & buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI CheckError(
        /* [out] */ Boolean * pHasError);

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

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CSSLSOCKETOUTPUTSTREAM_H__
