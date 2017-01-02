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

#include "CKeyRep.h"
#include "CKeyFactory.h"
#include "spec/CPKCS8EncodedKeySpec.h"
#include "spec/CX509EncodedKeySpec.h"
#include "CSecretKeySpec.h"
#include "Elastos.CoreLibrary.IO.h"

using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastosx::Crypto::Spec::CSecretKeySpec;
using Elastosx::Crypto::Spec::ISecretKeySpec;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyRep)
CAR_INTERFACE_IMPL(CKeyRep, Object, ISerializable)
CKeyRep::CKeyRep()
    : mType(-1)
{}

ECode CKeyRep::constructor(
    /* [in] */ KeyRepType type,
    /* [in] */ const String& algorithm,
    /* [in] */ const String& format,
    /* [in] */ ArrayOf<Byte>* encoded)
{
    mType = type;
    mAlgorithm = algorithm;
    mFormat = format;
    mEncoded = encoded;
    if(mType == -1) {
        // throw new NullPointerException("type == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if(mAlgorithm == NULL) {
        // throw new NullPointerException("algorithm == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if(mFormat == NULL) {
        // throw new NullPointerException("format == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if(mEncoded == NULL) {
        // throw new NullPointerException("encoded == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode CKeyRep::ReadResolve(
    /* [out] */ IInterface** result) /*throws ObjectStreamException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    switch (mType) {
        case KeyRepType_SECRET: {
            if (String("RAW").Equals(mFormat)) {
                AutoPtr<ISecretKeySpec> ss;
                ECode ec = CSecretKeySpec::New(mEncoded, mAlgorithm, (ISecretKeySpec**)&ss);
                if (FAILED(ec)) {
                    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                        return E_NOT_SERIALIZABLE_EXCEPTION;
                    }
                    return ec;
                }

                *result = IInterface::Probe(ss);
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
            // throw new NotSerializableException("unrecognized type/format combination: " + type + "/" + format);
            return E_NOT_SERIALIZABLE_EXCEPTION;
        }
        case KeyRepType_PUBLIC: {
            if (String("X.509").Equals(mFormat)) {
                ECode ec = NOERROR;
                do {
                    AutoPtr<IKeyFactory> kf;
                    AutoPtr<IKeySpec> ks;
                    AutoPtr<IPublicKey> pk;
                    ec = CKeyFactory::GetInstance(mAlgorithm, (IKeyFactory**)&kf);
                    FAIL_GOTO(ec, error1);
                    ec = CX509EncodedKeySpec::New(mEncoded, (IKeySpec**)&ks);
                    FAIL_GOTO(ec, error1);
                    ec = kf->GeneratePublic(ks, (IPublicKey**)&pk);
                    FAIL_GOTO(ec, error1);
                    *result = IInterface::Probe(pk);
                    REFCOUNT_ADD(*result);
                    return NOERROR;
                } while (0);

            error1:
                if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION || ec == (ECode)E_INVALID_KEY_EXCEPTION) {
                    return E_NOT_SERIALIZABLE_EXCEPTION;
                }
                return ec;
            }
            // throw new NotSerializableException("unrecognized type/format combination: " + type + "/" + format);
            return E_NOT_SERIALIZABLE_EXCEPTION;
        }
        case KeyRepType_PRIVATE: {
            if (String("PKCS#8").Equals(mFormat)) {
                ECode ec = NOERROR;
                do {
                    AutoPtr<IKeyFactory> kf;
                    AutoPtr<IKeySpec> ks;
                    AutoPtr<IPrivateKey> pk;
                    ec = CKeyFactory::GetInstance(mAlgorithm, (IKeyFactory**)&kf);
                    FAIL_GOTO(ec, error2);
                    ec = CPKCS8EncodedKeySpec::New(mEncoded, (IKeySpec**)&ks);
                    FAIL_GOTO(ec, error2);
                    kf->GeneratePrivate(ks, (IPrivateKey**)&pk);
                    FAIL_GOTO(ec, error2);
                    *result = IInterface::Probe(pk);
                    REFCOUNT_ADD(*result);
                    return NOERROR;
                } while (0);

            error2:
                if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION || ec == (ECode)E_INVALID_KEY_EXCEPTION) {
                    return E_NOT_SERIALIZABLE_EXCEPTION;
                }
                return ec;
            }
            // throw new NotSerializableException("unrecognized type/format combination: " + type + "/" + format);
            return E_NOT_SERIALIZABLE_EXCEPTION;
        }
    }
    // throw new NotSerializableException("unrecognized key type: " + type);
    return E_NOT_SERIALIZABLE_EXCEPTION;
}

ECode CKeyRep::ReadObject(
    /* [in] */ IObjectInputStream* is) /*throws IOException, ClassNotFoundException*/
{
    FAIL_RETURN(is->DefaultReadObject());
    AutoPtr<ArrayOf<Byte> > new_encoded = ArrayOf<Byte>::Alloc(mEncoded->GetLength());

    // System.arraycopy(encoded, 0, new_encoded, 0, new_encoded.length);
    new_encoded->Copy(0, mEncoded, 0, new_encoded->GetLength());
    mEncoded = new_encoded;
    return NOERROR;
}

}
}
