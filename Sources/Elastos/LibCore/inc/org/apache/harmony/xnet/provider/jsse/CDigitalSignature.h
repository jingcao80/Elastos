
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CDIGITALSIGNATURE_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CDIGITALSIGNATURE_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CDigitalSignature.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CDigitalSignature)
{
public:
    CARAPI Init(
        /* [in] */ Elastos::Security::IPrivateKey * pKey);

    CARAPI InitEx(
        /* [in] */ Elastos::Security::Cert::ICertificate * pCert);

    CARAPI Update(
        /* [in] */ ArrayOf<Byte> * pData);

    CARAPI SetMD5(
        /* [in] */ ArrayOf<Byte> * pData);

    CARAPI SetSHA(
        /* [in] */ ArrayOf<Byte> * pData);

    CARAPI Sign(
        /* [out, callee] */ ArrayOf<Byte> ** ppSign);

    CARAPI VerifySignature(
        /* [in] */ ArrayOf<Byte> * pData,
        /* [out] */ Boolean * pResult);

    CARAPI constructor(
        /* [in] */ const String& authType);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CDIGITALSIGNATURE_H__
