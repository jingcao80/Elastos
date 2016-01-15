
#include "COpenSSLDSAKeyFactory.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {


ECode COpenSSLDSAKeyFactory::EngineGeneratePublic(
    /* [in] */ Elastos::Security::Spec::IKeySpec * pKeySpec,
    /* [out] */ Elastos::Security::IPublicKey ** ppPubKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLDSAKeyFactory::EngineGeneratePrivate(
    /* [in] */ Elastos::Security::Spec::IKeySpec * pKeySpec,
    /* [out] */ Elastos::Security::IPrivateKey ** ppPriKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLDSAKeyFactory::EngineGetKeySpec(
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [in] */ const ClassID & keySpec,
    /* [out] */ Elastos::Security::Spec::IKeySpec ** ppRetkeySpec)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLDSAKeyFactory::EngineTranslateKey(
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [out] */ Elastos::Security::IKey ** ppTranslatedKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

