#ifndef __ELASTOS_DROID_SPEECH_SREC_MicrophoneInputStream_H__
#define __ELASTOS_DROID_SPEECH_SREC_MicrophoneInputStream_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include <elastos/io/InputStream.h>
#include "Elastos.Droid.Speech.h"

using Elastos::IO::IInputStream;
using Elastos::IO::InputStream;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

/**
 * PCM input stream from the microphone, 16 bits per sample.
 */
class MicrophoneInputStream
    : public InputStream
    , public IMicrophoneInputStream
{
public:
    CAR_INTERFACE_DECL();

    MicrophoneInputStream();

    virtual ~MicrophoneInputStream();

    CARAPI constructor();

    /**
     * MicrophoneInputStream constructor.
     * @param sampleRate sample rate of the microphone, typically 11025 or 8000.
     * @param fifoDepth depth of the real time fifo, measured in sampleRate clock ticks.
     * This determines how long an application may delay before losing data.
     */
    CARAPI constructor(
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 fifoDepth);

    //@Override
    CARAPI Read(
        /* [out] */ Int32* value);

    //@Override
    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* b,
        /* [out] */ Int32* number);

    //@Override
    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

    /**
     * Closes this stream.
     */
    //@Override
    CARAPI Close();

protected:
    //@Override
    CARAPI_(void) Finalize();

private:

    //
    // AudioRecord JNI interface
    //
    //native
    static CARAPI_(Int32) AudioRecordNew(
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 fifoDepth);

    //native
    static CARAPI_(Int32) AudioRecordStart(
        /* [in] */ Int32 audioRecord);

    //native
    static CARAPI_(Int32) AudioRecordRead(
        /* [in] */ Int32 audioRecord,
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length);

    //native
    static CARAPI_(void) AudioRecordStop(
        /* [in] */ Int32 audioRecord);

    //native
    static CARAPI_(void) AudioRecordDelete(
        /* [in] */ Int32 audioRecord);

private:
    static const String TAG;                // = "MicrophoneInputStream";
    Int32 mAudioRecord;
    AutoPtr<ArrayOf<Byte> > mOneByte;
};

} // namespace Srec
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_MicrophoneInputStream_H__
