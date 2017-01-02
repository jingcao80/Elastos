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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECCRYPTOINFO_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECCRYPTOINFO_H__

#include "_Elastos_Droid_Media_CMediaCodecCryptoInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecCryptoInfo)
    , public Object
    , public IMediaCodecCryptoInfo
{
public:
    CMediaCodecCryptoInfo();

    virtual ~CMediaCodecCryptoInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI Set(
        /* [in] */ Int32 newNumSubSamples,
        /* [in] */ ArrayOf<Int32>* newNumBytesOfClearData,
        /* [in] */ ArrayOf<Int32>* newNumBytesOfEncryptedData,
        /* [in] */ ArrayOf<Byte>* newKey,
        /* [in] */ ArrayOf<Byte>* newIV,
        /* [in] */ Int32 newMode);

    CARAPI GetNumSubSamples(
        /* [out] */ Int32* result);

    CARAPI GetNumBytesOfClearData(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI GetNumBytesOfEncryptedData(
        /* [out, callee] */ ArrayOf<Int32>** result);

    CARAPI GetKey(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI GetIv(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI GetMode(
        /* [out] */ Int32* result);

    CARAPI SetNumSubSamples(
        /* [in] */ Int32 samples);

    CARAPI SetNumBytesOfClearData(
        /* [in] */ ArrayOf<Int32>* num);

    CARAPI SetNumBytesOfEncryptedData(
        /* [in] */ ArrayOf<Int32>* num);

    CARAPI SetKey(
        /* [in] */ ArrayOf<Byte>* key);

    CARAPI SetIv(
        /* [in] */ ArrayOf<Byte>* iv);

    CARAPI SetMode(
        /* [in] */ Int32 mode);

private:
    /**
     * The number of subSamples that make up the buffer's contents.
     */
    Int32 mNumSubSamples;
    /**
     * The number of leading unencrypted bytes in each subSample.
     */
    AutoPtr<ArrayOf<Int32> > mNumBytesOfClearData;
    /**
     * The number of trailing encrypted bytes in each subSample.
     */
    AutoPtr<ArrayOf<Int32> > mNumBytesOfEncryptedData;
    /**
     * A 16-byte opaque key
     */
    AutoPtr<ArrayOf<Byte> > mKey;
    /**
     * A 16-byte initialization vector
     */
    AutoPtr<ArrayOf<Byte> > mIv;
    /**
     * The type of encryption that has been applied,
     * see {@link #CRYPTO_MODE_UNENCRYPTED} and {@link #CRYPTO_MODE_AES_CTR}.
     */
    Int32 mMode;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECCRYPTOINFO_H__
