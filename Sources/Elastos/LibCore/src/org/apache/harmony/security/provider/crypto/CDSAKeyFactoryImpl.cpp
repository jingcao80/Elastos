
#include "CDSAKeyFactoryImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {


ECode CDSAKeyFactoryImpl::EngineGeneratePublic(
    /* [in] */ Elastos::Security::Spec::IKeySpec * pKeySpec,
    /* [out] */ Elastos::Security::IPublicKey ** ppPubKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAKeyFactoryImpl::EngineGeneratePrivate(
    /* [in] */ Elastos::Security::Spec::IKeySpec * pKeySpec,
    /* [out] */ Elastos::Security::IPrivateKey ** ppPriKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAKeyFactoryImpl::EngineGetKeySpec(
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [in] */ const ClassID & keySpec,
    /* [out] */ Elastos::Security::Spec::IKeySpec ** ppRetkeySpec)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDSAKeyFactoryImpl::EngineTranslateKey(
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

