
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLMESSAGEDIGESTJDK_MD5_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLMESSAGEDIGESTJDK_MD5_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLMessageDigestJDK_MD5.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLMessageDigestJDK_MD5)
{
public:
    CARAPI Clone(
        /* [out] */ IInterface ** ppObject);

    CARAPI EngineGetDigestLength(
        /* [out] */ Int32 * pLength);

    CARAPI EngineUpdate(
        /* [in] */ Byte input);

    CARAPI EngineUpdateEx(
        /* [in] */ ArrayOf<Byte> * pInput,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI EngineUpdateEx2(
        /* [in] */ Elastos::IO::IByteBuffer * pInput);

    CARAPI EngineDigest(
        /* [out, callee] */ ArrayOf<Byte> ** ppDigest);

    CARAPI EngineDigestEx(
        /* [in] */ ArrayOf<Byte> * pBuf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [out] */ Int32 * pDigest);

    CARAPI EngineReset();

    CARAPI Reset();

    CARAPI Update(
        /* [in] */ Byte arg0);

    CARAPI UpdateEx(
        /* [in] */ ArrayOf<Byte> * pInput,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI UpdateEx2(
        /* [in] */ ArrayOf<Byte> * pInput);

    CARAPI Digest(
        /* [out, callee] */ ArrayOf<Byte> ** ppHashValue);

    CARAPI DigestEx(
        /* [in] */ ArrayOf<Byte> * pBuf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [out] */ Int32 * pNumber);

    CARAPI DigestEx2(
        /* [in] */ ArrayOf<Byte> * pInput,
        /* [out, callee] */ ArrayOf<Byte> ** ppHashValue);

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetAlgorithm(
        /* [out] */ String * pAlgorithm);

    CARAPI GetProvider(
        /* [out] */ Elastos::Security::IProvider ** ppProvider);

    CARAPI GetDigestLength(
        /* [out] */ Int32 * pLength);

    CARAPI UpdateEx3(
        /* [in] */ Elastos::IO::IByteBuffer * pInput);

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

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLMESSAGEDIGESTJDK_MD5_H__
