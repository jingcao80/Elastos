#include "elastos/droid/speech/srec/MicrophoneInputStream.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

const String MicrophoneInputStream::TAG("MicrophoneInputStream");

CAR_INTERFACE_IMPL(MicrophoneInputStream, InputStream, IMicrophoneInputStream)

MicrophoneInputStream::MicrophoneInputStream()
{}

MicrophoneInputStream::~MicrophoneInputStream()
{}

ECode MicrophoneInputStream::constructor()
{
    return InputStream::constructor();
}

ECode MicrophoneInputStream::constructor(
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 fifoDepth)
{
    mOneByte = ArrayOf<Byte>::Alloc(1);
    mAudioRecord = AudioRecordNew(sampleRate, fifoDepth);
    if (mAudioRecord == 0){
        //Java:    throw new IOException("AudioRecord constructor failed - busy?");
        Logger::E(TAG, "IOException:AudioRecord constructor failed - busy?\n");
        return E_IO_EXCEPTION;
    }
    Int32 status = AudioRecordStart(mAudioRecord);
    if (status != 0) {
        Close();
        //Java:    throw new IOException("AudioRecord start failed: " + status);
        Logger::E(TAG, "IOException:AudioRecord start failed: \n", status);
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode MicrophoneInputStream::Read(
    /* [out] */ Int32* value)
{
    if (mAudioRecord == 0){
        //Java:    throw new IllegalStateException("not open");
        Logger::E(TAG, "IllegalStateException:not open\n");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Int32 rtn = AudioRecordRead(mAudioRecord, mOneByte, 0, 1);
    *value = rtn == 1 ? ( (Int32)(*mOneByte)[0] ) & 0xff : -1;
    return NOERROR;
}

ECode MicrophoneInputStream::Read(
    /* [out] */ ArrayOf<Byte>* b,
    /* [out] */ Int32* number)
{
    if (mAudioRecord == 0){
        //Java:    throw new IllegalStateException("not open");
        Logger::E(TAG, "IllegalStateException:not open\n");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    *number = AudioRecordRead(mAudioRecord, b, 0, b->GetLength());
    return NOERROR;
}

ECode MicrophoneInputStream::Read(
        /* [out] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number)
{
    if (mAudioRecord == 0){
        //Java:    throw new IllegalStateException("not open");
        Logger::E(TAG, "IllegalStateException:not open\n");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // TODO: should we force all reads to be a multiple of the sample size?
    *number = AudioRecordRead(mAudioRecord, b, offset, length);
    return NOERROR;
}

ECode MicrophoneInputStream::Close()
{
    if (mAudioRecord != 0) {
        //try {
            AudioRecordStop(mAudioRecord);
        //} finally {
            //try {
                AudioRecordDelete(mAudioRecord);
            //} finally {
                mAudioRecord = 0;
            //}
        //}
    }
    return NOERROR;
}

void MicrophoneInputStream::Finalize()
{
    if (mAudioRecord != 0) {
        Close();
        //throw new IOException("someone forgot to close MicrophoneInputStream");
        Logger::E(TAG, "someone forgot to close MicrophoneInputStream\n");
        return;     //E_IO_EXCEPTION
    }
}

Int32 MicrophoneInputStream::AudioRecordNew(
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 fifoDepth)
{
    return 0;
}

Int32 MicrophoneInputStream::AudioRecordStart(
    /* [in] */ Int32 audioRecord)
{
    return 0;
}

Int32 MicrophoneInputStream::AudioRecordRead(
    /* [in] */ Int32 audioRecord,
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    return 0;
}

void MicrophoneInputStream::AudioRecordStop(
    /* [in] */ Int32 audioRecord)
{}

void MicrophoneInputStream::AudioRecordDelete(
    /* [in] */ Int32 audioRecord)
{}

} // namespace Srec
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
