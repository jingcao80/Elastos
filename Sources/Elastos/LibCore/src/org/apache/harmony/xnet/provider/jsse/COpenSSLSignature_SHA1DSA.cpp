
#include "COpenSSLSignature_SHA1DSA.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(COpenSSLSignature_SHA1DSA)

ECode COpenSSLSignature_SHA1DSA::GetProvider(
    /* [out] */ Elastos::Security::IProvider ** ppProvider)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::GetAlgorithm(
    /* [out] */ String * pAlgo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::InitVerify(
    /* [in] */ Elastos::Security::IPublicKey * pPublicKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::InitVerifyEx(
    /* [in] */ Elastos::Security::ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::InitSign(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::InitSignEx(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey,
    /* [in] */ Elastos::Security::ISecureRandom * pRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::Sign(
    /* [out, callee] */ ArrayOf<Byte> ** ppSign)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::SignEx(
    /* [in] */ const ArrayOf<Byte> & outbuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len,
    /* [out] */ Int32 * pNum)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::Verify(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::VerifyEx(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::Update(
    /* [in] */ Byte b)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::UpdateEx(
    /* [in] */ const ArrayOf<Byte> & data)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::UpdateEx2(
    /* [in] */ const ArrayOf<Byte> & data,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::UpdateEx3(
    /* [in] */ Elastos::IO::IByteBuffer * pData)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::SetParameter(
    /* [in] */ const String& param,
    /* [in] */ IInterface * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::SetParameterEx(
    /* [in] */ Elastos::Security::Spec::IAlgorithmParameterSpec * pParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::GetParameters(
    /* [out] */ Elastos::Security::IAlgorithmParameters ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::Clone(
    /* [out] */ IInterface ** ppClone)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLSignature_SHA1DSA::constructor()
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

