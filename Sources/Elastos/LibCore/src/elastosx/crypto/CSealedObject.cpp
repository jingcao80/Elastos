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

#include "Elastos.CoreLibrary.Security.h"
#include "CSealedObject.h"
#include "IoUtils.h"
#include "CByteArrayOutputStream.h"
#include "CByteArrayInputStream.h"
#include "CCipherHelper.h"
#include "AlgorithmParameters.h"
#include "CObjectOutputStream.h"
//TODO: Need CObjectInputStream
// #include "CObjectInputStream.h"

using Libcore::IO::IoUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::IByte;
using Elastos::Core::IArrayOf;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CObjectOutputStream;
using Elastos::IO::EIID_ISerializable;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IObjectOutput;
using Elastos::IO::IObjectInput;
using Elastos::IO::IOutputStream;
using Elastos::Security::IAlgorithmParameters;
using Elastos::Security::IAlgorithmParametersHelper;
using Elastos::Security::AlgorithmParameters;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CSealedObject)
CAR_INTERFACE_IMPL_2(CSealedObject, Object, ISealedObject, ISerializable)

Int64 CSealedObject::mSerialVersionUID = 4482838265551344752;

CSealedObject::CSealedObject()
{
}

ECode CSealedObject::constructor(
    /* [in] */ ISerializable* object,
    /* [in] */ ICipher* c)
{
    if (c == NULL) {
        // throw new NULLPointerException("c == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IObjectOutputStream> oos;
    // try {
        AutoPtr<IByteArrayOutputStream> bos;
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&bos);
        CObjectOutputStream::New(IOutputStream::Probe(bos), (IObjectOutputStream**)&oos);
        IObjectOutput::Probe(oos)->WriteObject(object);
        IObjectOutput::Probe(oos)->Flush();
        AutoPtr<IAlgorithmParameters> ap;
        c->GetParameters((IAlgorithmParameters**)&ap);
        if(ap == NULL) {
            mEncodedParams = NULL;
            mParamsAlg = String(NULL);
            ap->GetAlgorithm(&mParamsAlg);
        }
        else {
            ap->GetEncoded((ArrayOf<Byte>**)&mEncodedParams);
            ap->GetAlgorithm(&mParamsAlg);
        }
        c->GetAlgorithm(&mSealAlg);
        AutoPtr<ArrayOf<Byte> > bytes;
        bos->ToByteArray((ArrayOf<Byte>**)&bytes);
        c->DoFinal(bytes, (ArrayOf<Byte>**)&mEncryptedContent);
    // } catch (BadPaddingException e) {
    //     // should be never thrown because the cipher
    //     // should be initialized for encryption
    //     throw new IOException(e.toString());
    // } finally {
        IoUtils::CloseQuietly(ICloseable::Probe(oos));
    // }
    return NOERROR;
}

ECode CSealedObject::constructor(
    /* [in] */ ISealedObject* so)
{
    if (so == NULL) {
        // throw new NULLPointerException("so == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    // For safety: clone the mutable arrays so that each object has its own independent copy of
    // the data.
    AutoPtr<CSealedObject> obj = (CSealedObject*)so;
    if(obj->mEncryptedContent != NULL) {
        mEncryptedContent = obj->mEncryptedContent->Clone();
    } else {
        mEncryptedContent = NULL;
    }
    if(obj->mEncodedParams != NULL) {
        mEncodedParams = obj->mEncodedParams->Clone();
    } else {
        mEncodedParams = NULL;
    }
    mSealAlg = obj->mSealAlg;
    mParamsAlg = obj->mParamsAlg;
    return NOERROR;
}

ECode CSealedObject::GetAlgorithm(
    /* [out] */ String * name)
{
    VALIDATE_NOT_NULL(name)
    *name = mSealAlg;
    return NOERROR;
}

ECode CSealedObject::GetObject(
    /* [in] */ IKey * key,
    /* [out] */ IInterface ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (key == NULL) {
        // throw new InvalidKeyException("key == NULL");
        return E_INVALID_KEY_EXCEPTION;
    }
    // try {
        AutoPtr<ICipher> cipher;
        AutoPtr<ICipherHelper> cipherHelper;
        CCipherHelper::AcquireSingleton((ICipherHelper**)&cipherHelper);
        cipherHelper->GetInstance(mSealAlg, (ICipher**)&cipher);
        if ((mParamsAlg != NULL) && (mParamsAlg.GetLength() != 0)) {
            AutoPtr<IAlgorithmParameters> params;
            AutoPtr<IAlgorithmParametersHelper> apHelper;
            AlgorithmParameters::GetInstance(mParamsAlg, (IAlgorithmParameters**)&params);
            params->Init(mEncodedParams);
            cipher->Init(ICipher::DECRYPT_MODE, key, params);
        } else {
            cipher->Init(ICipher::DECRYPT_MODE, key);
        }
        AutoPtr<ArrayOf<Byte> > serialized;
        cipher->DoFinal(mEncryptedContent, (ArrayOf<Byte>**)&serialized);
        ReadSerialized(serialized, result);
        REFCOUNT_ADD(*result)
        return NOERROR;
    // } catch (NoSuchPaddingException e)  {
    //     // should not be thrown because cipher text was made
    //     // with existing padding
    //     throw new NoSuchAlgorithmException(e.toString());
    // } catch (InvalidAlgorithmParameterException e) {
    //     // should not be thrown because cipher text was made
    //     // with correct algorithm parameters
    //     throw new NoSuchAlgorithmException(e.toString());
    // } catch (IllegalBlockSizeException e) {
    //     // should not be thrown because the cipher text
    //     // was correctly made
    //     throw new NoSuchAlgorithmException(e.toString());
    // } catch (BadPaddingException e) {
    //     // should not be thrown because the cipher text
    //     // was correctly made
    //     throw new NoSuchAlgorithmException(e.toString());
    // } catch (IllegalStateException e) {
    //     // should never be thrown because cipher is initialized
    //     throw new NoSuchAlgorithmException(e.toString());
    // }
}

ECode CSealedObject::GetObject(
    /* [in] */ ICipher * c,
    /* [out] */ IInterface ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (c == NULL) {
        // throw new NULLPointerException("c == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ArrayOf<Byte> > serialized;
    c->DoFinal(mEncryptedContent, (ArrayOf<Byte>**)&serialized);
    ReadSerialized(serialized, result);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSealedObject::GetObject(
    /* [in] */ IKey * key,
    /* [in] */ const String& provider,
    /* [out] */ IInterface ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (provider == NULL || provider.IsEmpty()) {
        // throw new IllegalArgumentException("provider name empty or NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
        AutoPtr<ICipher> cipher;
        AutoPtr<ICipherHelper> cipherHelper;
        CCipherHelper::AcquireSingleton((ICipherHelper**)&cipherHelper);
        cipherHelper->GetInstance(mSealAlg, provider, (ICipher**)&cipher);

        if ((mParamsAlg != NULL) && (mParamsAlg.GetLength() != 0)) {
            AutoPtr<IAlgorithmParameters> params;
            AutoPtr<IAlgorithmParametersHelper> apHelper;
            AlgorithmParameters::GetInstance(mParamsAlg, (IAlgorithmParameters**)&params);

            params->Init(mEncodedParams);
            cipher->Init(ICipher::DECRYPT_MODE, key, params);
        }
        else {
            cipher->Init(ICipher::DECRYPT_MODE, key);
        }
        AutoPtr<ArrayOf<Byte> > serialized;
        cipher->DoFinal(mEncryptedContent, (ArrayOf<Byte>**)&serialized);
        ReadSerialized(serialized, result);
        REFCOUNT_ADD(*result)
        return NOERROR;
    // } catch (NoSuchPaddingException e)  {
    //     // should not be thrown because cipher text was made
    //     // with existing padding
    //     throw new NoSuchAlgorithmException(e.toString());
    // } catch (InvalidAlgorithmParameterException e) {
    //     // should not be thrown because cipher text was made
    //     // with correct algorithm parameters
    //     throw new NoSuchAlgorithmException(e.toString());
    // } catch (IllegalBlockSizeException e) {
    //     // should not be thrown because the cipher text
    //     // was correctly made
    //     throw new NoSuchAlgorithmException(e.toString());
    // } catch (BadPaddingException e) {
    //     // should not be thrown because the cipher text
    //     // was correctly made
    //     throw new NoSuchAlgorithmException(e.toString());
    // } catch (IllegalStateException  e) {
    //     // should never be thrown because cipher is initialized
    //     throw new NoSuchAlgorithmException(e.toString());
    // }
}

AutoPtr<ArrayOf<Byte> > CSealedObject::GetSafeCopy(
    /* [in] */ IObjectInputStreamGetField * fields,
    /* [in] */ const String& fieldName)
{
    AutoPtr<ArrayOf<Byte> > fieldValue;
    AutoPtr<IInterface> obj;
    fields->Get(fieldName, NULL, (IInterface**)&obj);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
    if (array != NULL) {
        Int32 len;
        array->GetLength(&len);
        fieldValue = ArrayOf<Byte>::Alloc(len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<IInterface> ib;
            array->Get(i, (IInterface**)&ib);
            Byte b;
            IByte::Probe(ib)->GetValue(&b);
            fieldValue->Set(i, b);
        }
        return fieldValue;
    }
    return NULL;
}

void CSealedObject::ReadObject(
    /* [in] */ IObjectInputStream * s)
{
    // This implementation is based on the latest recommendations for safe deserialization at
    // the time of writing. See the Serialization spec section A.6.
    AutoPtr<IObjectInputStreamGetField> fields;
    s->ReadFields((IObjectInputStreamGetField**)&fields);

    // The mutable byte arrays are cloned and the immutable strings are not.
    mEncodedParams = GetSafeCopy(fields, String("mEncodedParams"));
    mEncryptedContent = GetSafeCopy(fields, String("mEncryptedContent"));
    AutoPtr<IInterface> cs;
    fields->Get(String("mParamsAlg"), NULL, (IInterface**)&cs);
    ICharSequence::Probe(cs)->ToString(&mParamsAlg);
    cs = NULL;
    fields->Get(String("mSealAlg"), NULL, (IInterface**)&cs);
    ICharSequence::Probe(cs)->ToString(&mSealAlg);
}

ECode CSealedObject::ReadSerialized(
    /* [in] */ ArrayOf<Byte> * serialized,
    /* [out] */ IInterface ** result)
{
    AutoPtr<IObjectInputStream> ois;
    // try {
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(serialized, (IByteArrayInputStream**)&stream);
        assert(0 && "TODO");
        // CObjectInputStream::New(stream, (IObjectInputStream**)&ois);
        IObjectInput::Probe(ois)->ReadObject(result);
    // } finally {
        IoUtils::CloseQuietly(ICloseable::Probe(ois));
    // }
        return NOERROR;
}

} // Crypto
} // Elastosx