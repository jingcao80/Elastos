
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLDSAKEYPAIRGENERATOR_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLDSAKEYPAIRGENERATOR_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLDSAKeyPairGenerator.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLDSAKeyPairGenerator)
{
public:
    CARAPI GenerateKeyPair(
        /* [out] */ Elastos::Security::IKeyPair ** ppKeyPair);

    CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ Elastos::Security::ISecureRandom * pRandom);

    CARAPI InitializeEx(
        /* [in] */ Elastos::Security::Spec::IAlgorithmParameterSpec * pParams,
        /* [in] */ Elastos::Security::ISecureRandom * pRandom);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLDSAKEYPAIRGENERATOR_H__
