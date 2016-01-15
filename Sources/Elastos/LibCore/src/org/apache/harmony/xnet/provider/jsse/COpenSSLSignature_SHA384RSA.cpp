
#include "COpenSSLSignature_SHA384RSA.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {


ECode COpenSSLSignature_SHA384RSA::GetProvider(
    /* [out] */ Elastos::Security::IProvider ** ppProvider)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::GetAlgorithm(
    /* [out] */ String * pAlgo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::InitVerify(
    /* [in] */ Elastos::Security::IPublicKey * pPublicKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::InitVerifyEx(
    /* [in] */ Elastos::Security::ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::InitSign(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::InitSignEx(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey,
    /* [in] */ Elastos::Security::ISecureRandom * pRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::Sign(
    /* [out, callee] */ ArrayOf<Byte> ** ppSign)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::SignEx(
    /* [in] */ const ArrayOf<Byte> & outbuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len,
    /* [out] */ Int32 * pNum)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::Verify(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::VerifyEx(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::Update(
    /* [in] */ Byte b)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::UpdateEx(
    /* [in] */ const ArrayOf<Byte> & data)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::UpdateEx2(
    /* [in] */ const ArrayOf<Byte> & data,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::UpdateEx3(
    /* [in] */ Elastos::IO::IByteBuffer * pData)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::SetParameter(
    /* [in] */ const String& param,
    /* [in] */ IInterface * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::SetParameterEx(
    /* [in] */ Elastos::Security::Spec::IAlgorithmParameterSpec * pParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::GetParameters(
    /* [out] */ Elastos::Security::IAlgorithmParameters ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::Clone(
    /* [out] */ IInterface ** ppClone)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA384RSA::constructor()
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

