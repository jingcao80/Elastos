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

#include "org/apache/harmony/security/x509/CSubjectPublicKeyInfo.h"
#include "CX509PublicKey.h"
#include "CBerInputStream.h"
#include "CBitString.h"
#include "AlgorithmIdentifier.h"
#include "ASN1BitString.h"
#include "CKeyFactory.h"
#include "CX509EncodedKeySpec.h"

using Elastos::Core::IArrayOf;
using Elastos::Security::CKeyFactory;
using Elastos::Security::IKeyFactory;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Org::Apache::Harmony::Security::Asn1::ASN1BitString;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Asn1::CBitString;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IBitString;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CSubjectPublicKeyInfo::ASN1SequenceWrapper::ASN1SequenceWrapper()
{}

ECode CSubjectPublicKeyInfo::ASN1SequenceWrapper::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result)
{
    AutoPtr<IArrayOf> values = IArrayOf::Probe(((CBerInputStream*)in)->mContent);
    AutoPtr<IInterface> v0;
    values->Get(0, (IInterface**)&v0);

    AutoPtr<IInterface> v1;
    values->Get(1, (IInterface**)&v1);

    AutoPtr<ArrayOf<Byte> > bytes;
    IBitString::Probe(v1)->GetBytes((ArrayOf<Byte>**)&bytes);
    Int32 unusedBits = 0;
    IBitString::Probe(v1)->GetUnusedBits(&unusedBits);
    AutoPtr<ArrayOf<Byte> > e;
    in->GetEncoded((ArrayOf<Byte>**)&e);

    AutoPtr<ISubjectPublicKeyInfo> pi;
    CSubjectPublicKeyInfo::New(IAlgorithmIdentifier::Probe(v0),
            bytes, unusedBits, e, (ISubjectPublicKeyInfo**)&pi);
    *result = IInterface::Probe(pi);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSubjectPublicKeyInfo::ASN1SequenceWrapper::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CSubjectPublicKeyInfo* spki = (CSubjectPublicKeyInfo*) ISubjectPublicKeyInfo::Probe(object);
    values->Set(0,spki->mAlgorithmID);
    AutoPtr<IBitString> bs;
    values->Set(1,bs);
    return NOERROR;
}

INIT_PROI_5 AutoPtr<IASN1Sequence> CSubjectPublicKeyInfo::ASN1 = InitASN1();
CAR_OBJECT_IMPL(CSubjectPublicKeyInfo)
CAR_INTERFACE_IMPL(CSubjectPublicKeyInfo, Object, ISubjectPublicKeyInfo)
AutoPtr<IASN1Sequence> CSubjectPublicKeyInfo::InitASN1()
{
    AutoPtr<ASN1SequenceWrapper> a = new ASN1SequenceWrapper();
    AutoPtr<ArrayOf<IASN1Type*> > at = ArrayOf<IASN1Type*>::Alloc(2);
    at->Set(0, IASN1Type::Probe(AlgorithmIdentifier::ASN1));

    AutoPtr<IASN1Type> tmp;
    ASN1BitString::GetInstance((IASN1Type**)&tmp);
    at->Set(1, IASN1Type::Probe(tmp));
    a->constructor(at);
    return a;
}

AutoPtr<IPublicKey> CSubjectPublicKeyInfo::GenerateKeyForAlgorithm(
    /* [in] */ IKeySpec* keySpec,
    /* [in] */ const String& algorithm)
{
    // try {
    AutoPtr<IKeyFactory> kf;
    ECode ec = CKeyFactory::GetInstance(algorithm, (IKeyFactory**)&kf);
    if (FAILED(ec)) {
        return NULL;
    }
    AutoPtr<IPublicKey> pk;
    ec = kf->GeneratePublic(keySpec, (IPublicKey**)&pk);
    if (FAILED(ec)) {
        return NULL;
    }
    // return CKeyFactory::GetInstance(algorithm).generatePublic(keySpec);
    // } catch (InvalidKeySpecException ignored) {
    // } catch (NoSuchAlgorithmException ignored) {
    // }
    return pk;
}

CSubjectPublicKeyInfo::CSubjectPublicKeyInfo()
    : mUnusedBits(0)
{}

ECode CSubjectPublicKeyInfo::GetAlgorithmIdentifier(
    /* [out] */ IAlgorithmIdentifier** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAlgorithmID;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSubjectPublicKeyInfo::GetSubjectPublicKey(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSubjectPublicKey;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSubjectPublicKeyInfo::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
    }
    *result = mEncoding;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSubjectPublicKeyInfo::GetPublicKey(
    /* [out] */ IPublicKey** ppPublicKey)
{
    VALIDATE_NOT_NULL(ppPublicKey);
    *ppPublicKey = NULL;
    if (mPublicKey == NULL) {
        AutoPtr<ArrayOf<Byte> > encoded;
        GetEncoded((ArrayOf<Byte>**)&encoded);
        AutoPtr<IKeySpec> keySpec;
        CX509EncodedKeySpec::New(encoded, (IKeySpec**)&keySpec);

        /* Try using the algorithm name first. */
        String algName;
        mAlgorithmID->GetAlgorithmName(&algName);
        mPublicKey = GenerateKeyForAlgorithm(keySpec, algName);

        /*
         * Fall back to using the algorithm OID if it's not the same as the
         * algorithm name.
         */
        String algOid;
        mAlgorithmID->GetAlgorithm(&algOid);
        if (mPublicKey == NULL && !algOid.Equals(algName)) {
            mPublicKey = GenerateKeyForAlgorithm(keySpec, algOid);
        }

        /*
         * Encode this as an X.509 public key since we didn't have any
         * KeyFactory that could handle this algorithm name or OID. Perhaps
         * the thing that's using this can decode it.
         */
        if (mPublicKey == NULL) {
            CX509PublicKey::New(algOid, encoded, mSubjectPublicKey, (IPublicKey**)&mPublicKey);
        }
    }
    *ppPublicKey = mPublicKey;
    REFCOUNT_ADD(*ppPublicKey);
    return NOERROR;
}

ECode CSubjectPublicKeyInfo::constructor(
    /* [in] */ IAlgorithmIdentifier* algID,
    /* [in] */ ArrayOf<Byte>* subjectPublicKey)
{
    return constructor(algID, subjectPublicKey, 0);
}

ECode CSubjectPublicKeyInfo::constructor(
    /* [in] */ IAlgorithmIdentifier* algID,
    /* [in] */ ArrayOf<Byte>* subjectPublicKey,
    /* [in] */ Int32 unused)
{
    return constructor(algID, subjectPublicKey, 0, NULL);
}

ECode CSubjectPublicKeyInfo::constructor(
    /* [in] */ IAlgorithmIdentifier* algID,
    /* [in] */ ArrayOf<Byte>* subjectPublicKey,
    /* [in] */ Int32 unused,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mAlgorithmID = algID;
    mSubjectPublicKey = subjectPublicKey;
    mUnusedBits = unused;
    mEncoding = encoding;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org