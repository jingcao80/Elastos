
#include "COpenSSLSignature_MD5RSA.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(COpenSSLSignature_MD5RSA)

ECode COpenSSLSignature_MD5RSA::GetProvider(
    /* [out] */ Elastos::Security::IProvider ** ppProvider)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::GetAlgorithm(
    /* [out] */ String * pAlgo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::InitVerify(
    /* [in] */ Elastos::Security::IPublicKey * pPublicKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::InitVerifyEx(
    /* [in] */ Elastos::Security::ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::InitSign(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::InitSignEx(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey,
    /* [in] */ Elastos::Security::ISecureRandom * pRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::Sign(
    /* [out, callee] */ ArrayOf<Byte> ** ppSign)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::SignEx(
    /* [in] */ const ArrayOf<Byte> & outbuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len,
    /* [out] */ Int32 * pNum)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::Verify(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::VerifyEx(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::Update(
    /* [in] */ Byte b)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::UpdateEx(
    /* [in] */ const ArrayOf<Byte> & data)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::UpdateEx2(
    /* [in] */ const ArrayOf<Byte> & data,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::UpdateEx3(
    /* [in] */ Elastos::IO::IByteBuffer * pData)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::SetParameter(
    /* [in] */ const String& param,
    /* [in] */ IInterface * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::SetParameterEx(
    /* [in] */ Elastos::Security::Spec::IAlgorithmParameterSpec * pParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::GetParameters(
    /* [out] */ Elastos::Security::IAlgorithmParameters ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::Clone(
    /* [out] */ IInterface ** ppClone)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_MD5RSA::constructor()
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

