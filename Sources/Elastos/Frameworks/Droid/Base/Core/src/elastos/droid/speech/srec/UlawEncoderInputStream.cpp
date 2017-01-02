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

#include "elastos/droid/speech/srec/UlawEncoderInputStream.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>
//#include "elastos/droid/ext/frameworkext.h"

using Elastos::Utility::Logging::Logger;
using Elastos::IO::EIID_IInputStream;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

const String UlawEncoderInputStream::TAG("UlawEncoderInputStream");
const Int32 UlawEncoderInputStream::MAX_ULAW = 8192;
const Int32 UlawEncoderInputStream::SCALE_BITS = 16;

CAR_INTERFACE_IMPL_2(UlawEncoderInputStream, Object, IInputStream, IUlawEncoderInputStream);

UlawEncoderInputStream::UlawEncoderInputStream()
{}

UlawEncoderInputStream::~UlawEncoderInputStream()
{}

ECode UlawEncoderInputStream::constructor()
{
    return InputStream::constructor();
}

ECode UlawEncoderInputStream::constructor(
    /* [in] */ IInputStream* ins,
    /* [in] */ Int32 max)
{
    mIn = ins;
    mMax = max;

    mBuf = ArrayOf<Byte>::Alloc(1024);
    mBufCount = 0;
    mOneByte = ArrayOf<Byte>::Alloc(1);

    return NOERROR;
}

ECode UlawEncoderInputStream::Encode(
    /* [in] */ ArrayOf<Byte>* pcmBuf,
    /* [in] */ Int32 pcmOffset,
    /* [in] */ ArrayOf<Byte>* ulawBuf,
    /* [in] */ Int32 ulawOffset,
    /* [in] */ Int32 length,
    /* [in] */ Int32 max)
{
    // set scale factors
    if (max <= 0){
        max = MAX_ULAW;
    }

    Int32 coef = MAX_ULAW * (1 << SCALE_BITS) / max;

    for (Int32 i = 0; i < length; i++) {
        Int32 pcm;

        pcm = (0xff & (*pcmBuf)[pcmOffset++]);
        pcm += ((*pcmBuf)[pcmOffset++] << 8);
        pcm = (pcm * coef) >> SCALE_BITS;

        Int32 ulaw;
        if (pcm >= 0) {
            ulaw = pcm <= 0 ? 0xff :
                    pcm <=   30 ? 0xf0 + ((  30 - pcm) >> 1) :
                    pcm <=   94 ? 0xe0 + ((  94 - pcm) >> 2) :
                    pcm <=  222 ? 0xd0 + (( 222 - pcm) >> 3) :
                    pcm <=  478 ? 0xc0 + (( 478 - pcm) >> 4) :
                    pcm <=  990 ? 0xb0 + (( 990 - pcm) >> 5) :
                    pcm <= 2014 ? 0xa0 + ((2014 - pcm) >> 6) :
                    pcm <= 4062 ? 0x90 + ((4062 - pcm) >> 7) :
                    pcm <= 8158 ? 0x80 + ((8158 - pcm) >> 8) :
                    0x80;
        }
        else {
            ulaw = -1 <= pcm ? 0x7f :
                      -31 <= pcm ? 0x70 + ((pcm -   -31) >> 1) :
                      -95 <= pcm ? 0x60 + ((pcm -   -95) >> 2) :
                     -223 <= pcm ? 0x50 + ((pcm -  -223) >> 3) :
                     -479 <= pcm ? 0x40 + ((pcm -  -479) >> 4) :
                     -991 <= pcm ? 0x30 + ((pcm -  -991) >> 5) :
                    -2015 <= pcm ? 0x20 + ((pcm - -2015) >> 6) :
                    -4063 <= pcm ? 0x10 + ((pcm - -4063) >> 7) :
                    -8159 <= pcm ? 0x00 + ((pcm - -8159) >> 8) :
                    0x00;
        }
        (*ulawBuf)[ulawOffset++] = (Byte)ulaw;
    }

    return NOERROR;
}

ECode UlawEncoderInputStream::MaxAbsPcm(
    /* [in] */ ArrayOf<Byte>* pcmBuf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);

    Int32 max = 0;
    for (Int32 i = 0; i < length; i++) {
        Int32 pcm;

        pcm = (0xff & (*pcmBuf)[offset++]);
        pcm += ((*pcmBuf)[offset++] << 8);

        if (pcm < 0){
            pcm = -pcm;
        }
        if (pcm > max){
            max = pcm;
        }
    }
    *ret = max;
    return NOERROR;
}

ECode UlawEncoderInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    if (mIn == NULL){
        //Java:    throw new IllegalStateException("not open");
        Logger::E(TAG, "IllegalStateException:not open\n");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // return at least one byte, but try to fill 'length'
    while (mBufCount < 2) {
        Int32 n;
        mIn->Read(mBuf, mBufCount, Elastos::Core::Math::Min(length * 2, mBuf->GetLength() - mBufCount), &n);
        if (n == -1) {
            *number = -1;
            return NOERROR;
        }
        mBufCount += n;
    }

    // compand data
    Int32 n = Elastos::Core::Math::Min(mBufCount / 2, length);
    Encode(mBuf, 0, buf, offset, n, mMax);

    // move data to bottom of mBuf
    mBufCount -= n * 2;
    for (Int32 i = 0; i < mBufCount; i++){
        (*mBuf)[i] = (*mBuf)[i + n * 2];
    }

    *number = n;
    return NOERROR;
}

ECode UlawEncoderInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buf,
    /* [out] */ Int32* number)
{
    return Read(buf, 0, buf->GetLength(), number);
}

ECode UlawEncoderInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    Int32 n;
    Read(mOneByte, 0, 1, &n);
    if (n == -1){
        *value = -1;
        return NOERROR;
    }
    *value = 0xff & (Int32)(*mOneByte)[0];
    return NOERROR;
}

ECode UlawEncoderInputStream::Close()
{
    if (mIn != NULL) {
        AutoPtr<IInputStream> in = mIn;
        mIn = NULL;
        in->Close();
    }
    return NOERROR;
}

ECode UlawEncoderInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    Int32 numberAvailable;
    *number = ((mIn->Available(&numberAvailable), numberAvailable) + mBufCount) / 2;
    return NOERROR;
}

ECode UlawEncoderInputStream::Mark(
    /* [in]  */ Int32 readLimit)
{
    return NOERROR;
}

ECode UlawEncoderInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    return NOERROR;
}

ECode UlawEncoderInputStream::Reset()
{
    return NOERROR;
}

ECode UlawEncoderInputStream::Skip(
    /* [in]  */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    return NOERROR;
}

ECode UlawEncoderInputStream::GetLock(
    /* [out] */ IInterface** lockobj)
{
    return NOERROR;
}

} // namespace Srec
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
