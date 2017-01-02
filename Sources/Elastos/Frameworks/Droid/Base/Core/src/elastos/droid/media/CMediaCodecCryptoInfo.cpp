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

#include "elastos/droid/media/CMediaCodecCryptoInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CMediaCodecCryptoInfo, Object, IMediaCodecCryptoInfo)

CAR_OBJECT_IMPL(CMediaCodecCryptoInfo)

CMediaCodecCryptoInfo::CMediaCodecCryptoInfo()
    : mNumSubSamples(0)
    , mMode(0)
{
}

CMediaCodecCryptoInfo::~CMediaCodecCryptoInfo()
{
}

ECode CMediaCodecCryptoInfo::constructor()
{
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::Set(
    /* [in] */ Int32 newNumSubSamples,
    /* [in] */ ArrayOf<Int32>* newNumBytesOfClearData,
    /* [in] */ ArrayOf<Int32>* newNumBytesOfEncryptedData,
    /* [in] */ ArrayOf<Byte>* newKey,
    /* [in] */ ArrayOf<Byte>* newIV,
    /* [in] */ Int32 newMode)
{
    mNumSubSamples = newNumSubSamples;
    mNumBytesOfClearData = newNumBytesOfClearData ? newNumBytesOfClearData->Clone() : NULL;
    mNumBytesOfEncryptedData = newNumBytesOfEncryptedData ? newNumBytesOfEncryptedData->Clone() : NULL;
    mKey = newKey ? newKey->Clone() : NULL;
    mIv = newIV ? newIV->Clone() : NULL;
    mMode = newMode;
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::GetNumSubSamples(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumSubSamples;
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::GetNumBytesOfClearData(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumBytesOfClearData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::GetNumBytesOfEncryptedData(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumBytesOfEncryptedData;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::GetKey(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mKey;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::GetIv(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIv;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::GetMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMode;
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::SetNumSubSamples(
    /* [in] */ Int32 samples)
{
    mNumSubSamples = samples;
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::SetNumBytesOfClearData(
    /* [in] */ ArrayOf<Int32>* num)
{
    mNumBytesOfClearData = num ? num->Clone() : NULL;
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::SetNumBytesOfEncryptedData(
    /* [in] */ ArrayOf<Int32>* num)
{
    mNumBytesOfEncryptedData = num ? num->Clone() : NULL;
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::SetKey(
    /* [in] */ ArrayOf<Byte>* key)
{
    mKey = key ? key->Clone() : NULL;
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::SetIv(
    /* [in] */ ArrayOf<Byte>* iv)
{
    mIv = iv ? iv->Clone() : NULL;
    return NOERROR;
}

ECode CMediaCodecCryptoInfo::SetMode(
    /* [in] */ Int32 mode)
{
    mMode = mode;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
