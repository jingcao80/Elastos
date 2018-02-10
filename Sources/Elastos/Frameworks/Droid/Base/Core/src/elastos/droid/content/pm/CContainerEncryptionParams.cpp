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

#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Security.h>
#include "elastos/droid/content/pm/CContainerEncryptionParams.h"
#include <elastos/utility/Arrays.h>

using Elastos::Utility::Arrays;
using Elastos::Security::IKey;
using Elastosx::Crypto::Spec::CIvParameterSpec;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

const String CContainerEncryptionParams::TO_STRING_PREFIX("ContainerEncryptionParams{");

const Int32 CContainerEncryptionParams::ENC_PARAMS_IV_PARAMETERS = 1;
const Int32 CContainerEncryptionParams::MAC_PARAMS_NONE = 1;

CAR_INTERFACE_IMPL_2(CContainerEncryptionParams, Object, IContainerEncryptionParams, IParcelable)

CAR_OBJECT_IMPL(CContainerEncryptionParams)

ECode CContainerEncryptionParams::constructor()
{
    return NOERROR;
}

ECode CContainerEncryptionParams::constructor(
    /* [in] */ const String& encryptionAlgorithm,
    /* [in] */ IAlgorithmParameterSpec* encryptionSpec,
    /* [in] */ ISecretKey* encryptionKey)
{
    return constructor(encryptionAlgorithm, encryptionSpec, encryptionKey,
                 String(NULL), NULL, NULL, NULL, -1, -1, -1);
}

ECode CContainerEncryptionParams::constructor(
    /* [in] */ const String& encryptionAlgorithm,
    /* [in] */ IAlgorithmParameterSpec* encryptionSpec,
    /* [in] */ ISecretKey* encryptionKey,
    /* [in] */ const String& macAlgorithm,
    /* [in] */ IAlgorithmParameterSpec* macSpec,
    /* [in] */ ISecretKey* macKey,
    /* [in] */ ArrayOf<Byte>* macTag,
    /* [in] */ Int64 authenticatedDataStart,
    /* [in] */ Int64 encryptedDataStart,
    /* [in] */ Int64 dataEnd)
{
    if (encryptionAlgorithm.IsEmpty()) {
        // throw new NullPointerException("algorithm == null");
        return E_NULL_POINTER_EXCEPTION;
    } else if (encryptionSpec == NULL) {
        // throw new NullPointerException("encryptionSpec == null");
        return E_NULL_POINTER_EXCEPTION;
    } else if (encryptionKey == NULL) {
        // throw new NullPointerException("encryptionKey == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (!macAlgorithm.IsEmpty()) {
        if (macKey == NULL) {
            // throw new NullPointerException("macKey == null");
            return E_NULL_POINTER_EXCEPTION;
        }
    }

    if (IIvParameterSpec::Probe(encryptionSpec) == NULL) {
        // throw new InvalidAlgorithmParameterException(
        //         "Unknown parameter spec class; must be IvParameters");
        return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
    }

    mEncryptionAlgorithm = encryptionAlgorithm;
    mEncryptionSpec = IIvParameterSpec::Probe(encryptionSpec);
    mEncryptionKey = encryptionKey;

    mMacAlgorithm = macAlgorithm;
    mMacSpec = macSpec;
    mMacKey = macKey;
    mMacTag = macTag;

    mAuthenticatedDataStart = authenticatedDataStart;
    mEncryptedDataStart = encryptedDataStart;
    mDataEnd = dataEnd;

    return NOERROR;
}

ECode CContainerEncryptionParams::GetEncryptionAlgorithm(
    /* [out] */ String* ealgo)
{
    VALIDATE_NOT_NULL(ealgo);
    *ealgo = mEncryptionAlgorithm;
    return NOERROR;
}

ECode CContainerEncryptionParams::GetEncryptionSpec(
    /* [out] */ IAlgorithmParameterSpec** spec)
{
    VALIDATE_NOT_NULL(spec);
    *spec = IAlgorithmParameterSpec::Probe(mEncryptionSpec);
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

ECode CContainerEncryptionParams::GetEncryptionKey(
    /* [out] */ ISecretKey** key)
{
    VALIDATE_NOT_NULL(key);
    *key = mEncryptionKey;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode CContainerEncryptionParams::GetMacAlgorithm(
    /* [out] */ String* malgo)
{
    VALIDATE_NOT_NULL(malgo);
    *malgo = mMacAlgorithm;
    return NOERROR;
}

ECode CContainerEncryptionParams::GetMacSpec(
    /* [out] */ IAlgorithmParameterSpec** spec)
{
    VALIDATE_NOT_NULL(spec);
    *spec = mMacSpec;
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

ECode CContainerEncryptionParams::GetMacKey(
    /* [out] */ ISecretKey** key)
{
    VALIDATE_NOT_NULL(key);
    *key = mMacKey;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode CContainerEncryptionParams::GetMacTag(
    /* [out, callee] */ ArrayOf<Byte>** tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = mMacTag;
    REFCOUNT_ADD(*tag);
    return NOERROR;
}

ECode CContainerEncryptionParams::GetAuthenticatedDataStart(
    /* [out] */ Int64* aDataStart)
{
    VALIDATE_NOT_NULL(aDataStart);
    *aDataStart = mAuthenticatedDataStart;
    return NOERROR;
}

ECode CContainerEncryptionParams::GetEncryptedDataStart(
    /* [out] */ Int64* eDataStart)
{
    VALIDATE_NOT_NULL(eDataStart);
    *eDataStart = mEncryptedDataStart;
    return NOERROR;
}

ECode CContainerEncryptionParams::GetDataEnd(
    /* [out] */ Int64* dataEnd)
{
    VALIDATE_NOT_NULL(dataEnd);
    *dataEnd = mDataEnd;
    return NOERROR;
}

ECode CContainerEncryptionParams::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* isEquals)
{
    VALIDATE_NOT_NULL(isEquals);
    *isEquals = FALSE;
    VALIDATE_NOT_NULL(o);

    if (TO_IINTERFACE(this) == o) {
        *isEquals = TRUE;
        return NOERROR;
    }

    if (IContainerEncryptionParams::Probe(o) == NULL) {
        return NOERROR;
    }

    CContainerEncryptionParams* other = (CContainerEncryptionParams*)IContainerEncryptionParams::Probe(o);

    // Primitive comparison
    if ((mAuthenticatedDataStart != other->mAuthenticatedDataStart)
            || (mEncryptedDataStart != other->mEncryptedDataStart)
            || (mDataEnd != other->mDataEnd)) {
        return NOERROR;
    }

    // String comparison
    if (!mEncryptionAlgorithm.Equals(other->mEncryptionAlgorithm)
            || !mMacAlgorithm.Equals(other->mMacAlgorithm)) {
        return NOERROR;
    }

    // Object comparison
    if (!IsSecretKeyEqual(mEncryptionKey, other->mEncryptionKey)
            || !IsSecretKeyEqual(mMacKey, other->mMacKey)) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > iv1, iv2;
    mEncryptionSpec->GetIV((ArrayOf<Byte>**)&iv1);
    other->mEncryptionSpec->GetIV((ArrayOf<Byte>**)&iv2);
    if (!Arrays::Equals(iv1, iv2)
            || !Arrays::Equals(mMacTag, other->mMacTag) || (mMacSpec != other->mMacSpec)) {
        return NOERROR;
    }

    *isEquals = TRUE;
    return NOERROR;
}

Boolean CContainerEncryptionParams::IsSecretKeyEqual(
    /* [in] */ ISecretKey* key1,
    /* [in] */ ISecretKey* key2)
{
    String keyFormat, otherKeyFormat;
    IKey::Probe(key1)->GetFormat(&keyFormat);
    IKey::Probe(key2)->GetFormat(&otherKeyFormat);

    if (keyFormat.IsNull()) {
        if (!otherKeyFormat.IsNull()) {
            return FALSE;
        }

        AutoPtr<ArrayOf<Byte> > e1, e2;
        IKey::Probe(key1)->GetEncoded((ArrayOf<Byte>**)&e1);
        IKey::Probe(key2)->GetEncoded((ArrayOf<Byte>**)&e2);
        if (e1.Get() != e2.Get()) {
            return FALSE;
        }
    }
    else {
        if (!keyFormat.Equals(otherKeyFormat)) {
            return FALSE;
        }

        AutoPtr<ArrayOf<Byte> > e1, e2;
        IKey::Probe(key1)->GetEncoded((ArrayOf<Byte>**)&e1);
        IKey::Probe(key2)->GetEncoded((ArrayOf<Byte>**)&e2);
        if (!Arrays::Equals(e1, e2)) {
            return FALSE;
        }
    }

    return TRUE;
}

ECode CContainerEncryptionParams::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    Int32 hash = 3;
    hash += 5 * mEncryptionAlgorithm.GetHashCode();

    AutoPtr<ArrayOf<Byte> > ivs;
    mEncryptionSpec->GetIV((ArrayOf<Byte>**)&ivs);
    hash += 7 * Arrays::GetHashCode(ivs);
    hash += 11 * Object::GetHashCode(mEncryptionKey);
    hash += 13 * mMacAlgorithm.GetHashCode();
    hash += 17 * Object::GetHashCode(mMacKey);
    hash += 19 * Arrays::GetHashCode(mMacTag);
    hash += 23 * mAuthenticatedDataStart;
    hash += 29 * mEncryptedDataStart;
    hash += 31 * mDataEnd;

    *code = hash;
    return NOERROR;
}

ECode CContainerEncryptionParams::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb(TO_STRING_PREFIX);

    sb.Append("mEncryptionAlgorithm=\"");
    sb.Append(mEncryptionAlgorithm);
    sb.Append("\",");
    sb.Append("mEncryptionSpec=");
    sb.Append(Object::ToString(mEncryptionSpec));
    sb.Append("mEncryptionKey=");
    sb.Append(Object::ToString(mEncryptionKey));

    sb.Append("mMacAlgorithm=\"");
    sb.Append(mMacAlgorithm);
    sb.Append("\",");
    sb.Append("mMacSpec=");
    sb.Append(Object::ToString(mMacSpec));
    sb.Append("mMacKey=");
    sb.Append(Object::ToString(mMacKey));

    sb.Append(",mAuthenticatedDataStart=");
    sb.Append(mAuthenticatedDataStart);
    sb.Append(",mEncryptedDataStart=");
    sb.Append(mEncryptedDataStart);
    sb.Append(",mDataEnd=");
    sb.Append(mDataEnd);
    sb.AppendChar('}');

    *str = sb.ToString();
    return NOERROR;
}

ECode CContainerEncryptionParams::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    source->ReadString(&mEncryptionAlgorithm);
    Int32 encParamType;
    source->ReadInt32(&encParamType);

    AutoPtr<ArrayOf<Byte> > encParamsEncoded;
    source->ReadArrayOf((HANDLE*)(&encParamsEncoded));

    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mEncryptionKey = ISecretKey::Probe(obj);

    source->ReadString(&mMacAlgorithm);
    Int32 macParamType;
    source->ReadInt32(&macParamType);
    AutoPtr<ArrayOf<Byte> > macParamsEncoded;
    source->ReadArrayOf((HANDLE*)(&macParamsEncoded));

    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mMacKey = ISecretKey::Probe(obj);

    mMacTag = NULL;
    source->ReadArrayOf((HANDLE*)(&mMacTag));

    source->ReadInt64(&mAuthenticatedDataStart);
    source->ReadInt64(&mDataEnd);

    switch (encParamType) {
        case ENC_PARAMS_IV_PARAMETERS:
            CIvParameterSpec::New(encParamsEncoded, (IIvParameterSpec**)&mEncryptionSpec);
            break;
        default:
            return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
            // throw new InvalidAlgorithmParameterException("Unknown parameter type "
            //         + encParamType);
    }

    switch (macParamType) {
        case MAC_PARAMS_NONE:
            mMacSpec = NULL;
            break;
        default:
            return E_INVALID_ALGORITHM_PARAMETER_EXCEPTION;
            // throw new InvalidAlgorithmParameterException("Unknown parameter type "
            //         + macParamType);
    }

    if (mEncryptionKey == NULL) {
        // throw new NullPointerException("encryptionKey == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode CContainerEncryptionParams::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    dest->WriteString(mEncryptionAlgorithm);
    dest->WriteInt32(ENC_PARAMS_IV_PARAMETERS);
    AutoPtr<ArrayOf<Byte> > ivs;
    mEncryptionSpec->GetIV((ArrayOf<Byte>**)&ivs);
    dest->WriteArrayOf((HANDLE)ivs.Get());
    dest->WriteInterfacePtr(mEncryptionKey);

    dest->WriteString(mMacAlgorithm);
    dest->WriteInt32(MAC_PARAMS_NONE);
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(0);
    dest->WriteArrayOf((HANDLE)array.Get());
    dest->WriteInterfacePtr(mMacKey);

    dest->WriteArrayOf((HANDLE)mMacTag.Get());

    dest->WriteInt64(mAuthenticatedDataStart);
    dest->WriteInt64(mEncryptedDataStart);
    dest->WriteInt64(mDataEnd);

    return NOERROR;
}

}
}
}
}

