
#include "COpenSSLSignature_SHA1RSA.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(COpenSSLSignature_SHA1RSA)

ECode COpenSSLSignature_SHA1RSA::GetProvider(
    /* [out] */ Elastos::Security::IProvider ** ppProvider)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::GetAlgorithm(
    /* [out] */ String * pAlgo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::InitVerify(
    /* [in] */ Elastos::Security::IPublicKey * pPublicKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::InitVerifyEx(
    /* [in] */ Elastos::Security::ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::InitSign(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::InitSignEx(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey,
    /* [in] */ Elastos::Security::ISecureRandom * pRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::Sign(
    /* [out, callee] */ ArrayOf<Byte> ** ppSign)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::SignEx(
    /* [in] */ const ArrayOf<Byte> & outbuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len,
    /* [out] */ Int32 * pNum)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::Verify(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::VerifyEx(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::Update(
    /* [in] */ Byte b)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::UpdateEx(
    /* [in] */ const ArrayOf<Byte> & data)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::UpdateEx2(
    /* [in] */ const ArrayOf<Byte> & data,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::UpdateEx3(
    /* [in] */ Elastos::IO::IByteBuffer * pData)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::SetParameter(
    /* [in] */ const String& param,
    /* [in] */ IInterface * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::SetParameterEx(
    /* [in] */ Elastos::Security::Spec::IAlgorithmParameterSpec * pParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::GetParameters(
    /* [out] */ Elastos::Security::IAlgorithmParameters ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::Clone(
    /* [out] */ IInterface ** ppClone)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1RSA::constructor()
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

