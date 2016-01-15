
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CHANDSHAKEIODATASTREAM_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CHANDSHAKEIODATASTREAM_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CHandshakeIODataStream.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CHandshakeIODataStream)
{
public:
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

    CARAPI MarkEx();

    CARAPI Write(
        /* [in] */ Byte b);

    CARAPI WriteUint8(
        /* [in] */ Int64 n);

    CARAPI WriteUint16(
        /* [in] */ Int64 n);

    CARAPI WriteUint24(
        /* [in] */ Int64 n);

    CARAPI WriteUint32(
        /* [in] */ Int64 n);

    CARAPI WriteUint64(
        /* [in] */ Int64 n);

    CARAPI WriteEx(
        /* [in] */ ArrayOf<Byte> * pVector);

    CARAPI Append(
        /* [in] */ ArrayOf<Byte> * pSrc);

    CARAPI HasData(
        /* [out] */ Boolean * pHasData);

    CARAPI GetData(
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Byte> ** ppData);

    CARAPI constructor();

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CHANDSHAKEIODATASTREAM_H__
