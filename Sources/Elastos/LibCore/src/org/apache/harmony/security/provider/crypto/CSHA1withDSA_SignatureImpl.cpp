//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CSHA1withDSA_SignatureImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CAR_OBJECT_IMPL(CSHA1withDSA_SignatureImpl)

ECode CSHA1withDSA_SignatureImpl::GetProvider(
    /* [out] */ Elastos::Security::IProvider ** ppProvider)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::GetAlgorithm(
    /* [out] */ String * pAlgo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::InitVerify(
    /* [in] */ Elastos::Security::IPublicKey * pPublicKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::InitVerifyEx(
    /* [in] */ Elastos::Security::ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::InitSign(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::InitSignEx(
    /* [in] */ Elastos::Security::IPrivateKey * pPrivateKey,
    /* [in] */ Elastos::Security::ISecureRandom * pRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::Sign(
    /* [out, callee] */ ArrayOf<Byte> ** ppSign)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::SignEx(
    /* [in] */ const ArrayOf<Byte> & outbuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len,
    /* [out] */ Int32 * pNum)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::Verify(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::VerifyEx(
    /* [in] */ const ArrayOf<Byte> & signature,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Boolean * pIsVerified)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::Update(
    /* [in] */ Byte b)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::UpdateEx(
    /* [in] */ const ArrayOf<Byte> & data)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::UpdateEx2(
    /* [in] */ const ArrayOf<Byte> & data,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::UpdateEx3(
    /* [in] */ Elastos::IO::IByteBuffer * pData)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::ToString(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::SetParameter(
    /* [in] */ const String& param,
    /* [in] */ IInterface * pValue)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::SetParameterEx(
    /* [in] */ Elastos::Security::Spec::IAlgorithmParameterSpec * pParams)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::GetParameters(
    /* [out] */ Elastos::Security::IAlgorithmParameters ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::GetParameter(
    /* [in] */ const String& name,
    /* [out] */ IInterface ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::Clone(
    /* [out] */ IInterface ** ppClone)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1withDSA_SignatureImpl::constructor()
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

