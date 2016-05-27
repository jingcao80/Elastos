
#include "CDigitalSignature.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CDigitalSignature)

ECode CDigitalSignature::Init(
    /* [in] */ Elastos::Security::IPrivateKey * pKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDigitalSignature::InitEx(
    /* [in] */ Elastos::Security::Cert::ICertificate * pCert)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDigitalSignature::Update(
    /* [in] */ ArrayOf<Byte> * pData)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDigitalSignature::SetMD5(
    /* [in] */ ArrayOf<Byte> * pData)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDigitalSignature::SetSHA(
    /* [in] */ ArrayOf<Byte> * pData)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDigitalSignature::Sign(
    /* [out, callee] */ ArrayOf<Byte> ** ppSign)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDigitalSignature::VerifySignature(
    /* [in] */ ArrayOf<Byte> * pData,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CDigitalSignature::constructor(
    /* [in] */ const String& authType)
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

