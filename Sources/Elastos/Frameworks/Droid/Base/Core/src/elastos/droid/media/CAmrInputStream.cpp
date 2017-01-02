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

#include "elastos/droid/media/CAmrInputStream.h"

#include <media/stagefright/codecs/amrnb/gsmamr_enc.h>

namespace Elastos {
namespace Droid {
namespace Media {

// Corresponds to max bit rate of 12.2 kbps.
static const Int32 MAX_OUTPUT_BUFFER_SIZE = 32;
static const Int32 FRAME_DURATION_MS = 20;
static const Int32 SAMPLING_RATE_HZ = 8000;
static const Int32 SAMPLES_PER_FRAME = ((SAMPLING_RATE_HZ * FRAME_DURATION_MS) / 1000);
static const Int32 BYTES_PER_SAMPLE = 2;  // Assume 16-bit PCM samples
static const Int32 BYTES_PER_FRAME = (SAMPLES_PER_FRAME * BYTES_PER_SAMPLE);

struct GsmAmrEncoderState
{
    GsmAmrEncoderState()
        : mEncState(NULL)
        , mSidState(NULL)
        , mLastModeUsed(0)
    {}

    ~GsmAmrEncoderState() {}

    void*   mEncState;
    void*   mSidState;
    int32_t mLastModeUsed;
};

String CAmrInputStream::TAG("AmrInputStream");

CAR_INTERFACE_IMPL(CAmrInputStream, InputStream, IAmrInputStream)

CAR_OBJECT_IMPL(CAmrInputStream)

CAmrInputStream::CAmrInputStream()
    : mGae(0)
    , mBufIn(0)
    , mBufOut(0)
{
    mOneByte = ArrayOf<Byte>::Alloc(1);
    mBuf = ArrayOf<Byte>::Alloc(SAMPLES_PER_FRAME * 2);
}

CAmrInputStream::~CAmrInputStream()
{
    if (mGae != 0) {
        Close();
        //throw new IllegalStateException("someone forgot to close AmrInputStream");
    }
}

ECode CAmrInputStream::constructor(
    /* [in] */ IInputStream* inputStream)
{
    mInputStream = inputStream;
    FAIL_RETURN(GsmAmrEncoderNew(&mGae));
    return GsmAmrEncoderInitialize(mGae);
}

ECode CAmrInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = -1;

    FAIL_RETURN(Read(mOneByte, 0, 1, value));
    *value = *value == 1 ? (0xff & (*mOneByte)[0]) : -1;
    return NOERROR;
}

ECode CAmrInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    return Read(buffer,0, buffer->GetLength(), number);
}

ECode CAmrInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    if (mGae == 0) {
        // throw new IllegalStateException("not open");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // local buffer of amr encoded audio empty
    if (mBufOut >= mBufIn) {
        // reset the buffer
        mBufOut = 0;
        mBufIn = 0;

        // fetch a 20 msec frame of pcm
        for (Int32 i = 0; i < SAMPLES_PER_FRAME * 2; ) {
            Int32 n;
            mInputStream->Read(mBuf, i, SAMPLES_PER_FRAME * 2 - i, &n);
            if (n == -1) return NOERROR;
            i += n;
        }

        // encode it
        FAIL_RETURN(GsmAmrEncoderEncode(mGae, mBuf, 0, mBuf, 0, &mBufIn));
    }

    // return encoded audio to user
    if (length > mBufIn - mBufOut) length = mBufIn - mBufOut;
    memcpy(buffer + offset, mBuf + mBufOut, length);
    mBufOut += length;

    *number = length;
    return NOERROR;
}

ECode CAmrInputStream::Close()
{
    if (mInputStream != NULL) {
        mInputStream->Close();
        mInputStream = NULL;
    }

    if (mGae != 0) {
        GsmAmrEncoderCleanup(mGae);
        GsmAmrEncoderDelete(mGae);
        mGae = 0;
    }

    return NOERROR;
}

ECode CAmrInputStream::GsmAmrEncoderNew(
    /* [out] */ Int64* gae)
{
    VALIDATE_NOT_NULL(gae);

    GsmAmrEncoderState* gaeObj = new GsmAmrEncoderState();
    if (gaeObj == NULL) {
        // throwException(env, "java/lang/RuntimeException",
        //         "Out of memory", 0);
        *gae = 0;
        return E_RUNTIME_EXCEPTION;
    }
    *gae = (Int32)gaeObj;
    return NOERROR;
}

ECode CAmrInputStream::GsmAmrEncoderInitialize(
    /* [in] */ Int64 gae)
{
    GsmAmrEncoderState *state = (GsmAmrEncoderState *)gae;
    int32_t nResult = AMREncodeInit(&state->mEncState, &state->mSidState, false);
    if (nResult != 0) {
        // throwException(env, "java/lang/IllegalArgumentException",
        //     "GsmAmrEncoder initialization failed %d", nResult);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CAmrInputStream::GsmAmrEncoderEncode(
    /* [in] */ Int64 gae,
    /* [in] */ ArrayOf<Byte>* pcm,
    /* [in] */ Int32 pcmOffset,
    /* [in] */ ArrayOf<Byte>* amr,
    /* [in] */ Int32 amrOffset,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    Byte inBuf[BYTES_PER_FRAME];
    Byte outBuf[MAX_OUTPUT_BUFFER_SIZE];

    memcpy(inBuf, pcm->GetPayload() + pcmOffset, sizeof(inBuf));
    GsmAmrEncoderState *state = (GsmAmrEncoderState *)gae;
    int32_t length;
    length = AMREncode(state->mEncState, state->mSidState,
                                (Mode)MR122,
                                (int16_t *)inBuf,
                                (unsigned char *)outBuf,
                                (Frame_Type_3GPP*)&state->mLastModeUsed,
                                AMR_TX_WMF);
    if (length < 0) {
        // throwException(env, "java/io/IOException",
        //         "Failed to encode a frame with error code: %d", length);
        *number = -1;
        return E_IO_EXCEPTION;
    }

    // The 1st byte of PV AMR frames are WMF (Wireless Multimedia Forum)
    // bitpacked, i.e.;
    //    [P(4) + FT(4)]. Q=1 for good frame, P=padding bit, 0
    // Here we are converting the header to be as specified in Section 5.3 of
    // RFC 3267 (AMR storage format) i.e.
    //    [P(1) + FT(4) + Q(1) + P(2)].
    if (length > 0) {
        outBuf[0] = (outBuf[0] << 3) | 0x4;
    }

    memcpy(amr + amrOffset, outBuf, length);

    *number = length;
    return NOERROR;
}

void CAmrInputStream::GsmAmrEncoderCleanup(
    /* [in] */ Int64 gae)
{
    GsmAmrEncoderState *state = (GsmAmrEncoderState *)gae;
    if (state) {
        AMREncodeExit(&state->mEncState, &state->mSidState);
        state->mEncState = NULL;
        state->mSidState = NULL;
    }
}

void CAmrInputStream::GsmAmrEncoderDelete(
    /* [in] */ Int64 gae)
{
    GsmAmrEncoderState *state = (GsmAmrEncoderState *)gae;
    if (state)
        delete state;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
