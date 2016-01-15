#ifndef __ELASTOS_DROID_SPEECH_SREC_WaveHeader_H__
#define __ELASTOS_DROID_SPEECH_SREC_WaveHeader_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include <elastos/io/InputStream.h>
#include <elastos/io/OutputStream.h>
#include "Elastos.Droid.Speech.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Srec {

/**
 * This class represents the header of a WAVE format audio file, which usually
 * have a .wav suffix.  The following integer valued fields are contained:
 * <ul>
 * <li> format - usually PCM, ALAW or ULAW.
 * <li> numChannels - 1 for mono, 2 for stereo.
 * <li> sampleRate - usually 8000, 11025, 16000, 22050, or 44100 hz.
 * <li> bitsPerSample - usually 16 for PCM, 8 for ALAW, or 8 for ULAW.
 * <li> numBytes - size of audio data after this header, in bytes.
 * </ul>
 *
 * Not yet ready to be supported, so
 * @hide
 */
class WaveHeader
    : public Object
    , public IWaveHeader
{
public:
    CAR_INTERFACE_DECL();

    WaveHeader();

    virtual ~WaveHeader();

    /**
     * Construct a WaveHeader, with all fields defaulting to zero.
     */
    CARAPI constructor();

    /**
     * Construct a WaveHeader, with fields initialized.
     * @param format format of audio data,
     * one of {@link #FORMAT_PCM}, {@link #FORMAT_ULAW}, or {@link #FORMAT_ALAW}.
     * @param numChannels 1 for mono, 2 for stereo.
     * @param sampleRate typically 8000, 11025, 16000, 22050, or 44100 hz.
     * @param bitsPerSample usually 16 for PCM, 8 for ULAW or 8 for ALAW.
     * @param numBytes size of audio data after this header, in bytes.
     */
    CARAPI constructor(
        /* [in] */ Int16 format,
        /* [in] */ Int16 numChannels,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int16 bitsPerSample,
        /* [in] */ Int32 numBytes);

    /**
     * Get the format field.
     * @return format field,
     * one of {@link #FORMAT_PCM}, {@link #FORMAT_ULAW}, or {@link #FORMAT_ALAW}.
     */
    CARAPI GetFormat(
        /* [out] */ Int16* ret);

    /**
     * Set the format field.
     * @param format
     * one of {@link #FORMAT_PCM}, {@link #FORMAT_ULAW}, or {@link #FORMAT_ALAW}.
     * @return reference to this WaveHeader instance.
     */
    CARAPI SetFormat(
        /* [in] */ Int16 format);

    /**
     * Get the number of channels.
     * @return number of channels, 1 for mono, 2 for stereo.
     */
    CARAPI GetNumChannels(
        /* [out] */ Int16* ret);

    /**
     * Set the number of channels.
     * @param numChannels 1 for mono, 2 for stereo.
     * @return reference to this WaveHeader instance.
     */
    CARAPI SetNumChannels(
        /* [in] */ Int16 numChannels);

    /**
     * Get the sample rate.
     * @return sample rate, typically 8000, 11025, 16000, 22050, or 44100 hz.
     */
    CARAPI GetSampleRate(
        /* [out] */ Int32* ret);

    /**
     * Set the sample rate.
     * @param sampleRate sample rate, typically 8000, 11025, 16000, 22050, or 44100 hz.
     * @return reference to this WaveHeader instance.
     */
    CARAPI SetSampleRate(
        /* [in] */ Int32 sampleRate);

    /**
     * Get the number of bits per sample.
     * @return number of bits per sample,
     * usually 16 for PCM, 8 for ULAW or 8 for ALAW.
     */
    CARAPI GetBitsPerSample(
        /* [out] */ Int16* ret);

    /**
     * Set the number of bits per sample.
     * @param bitsPerSample number of bits per sample,
     * usually 16 for PCM, 8 for ULAW or 8 for ALAW.
     * @return reference to this WaveHeader instance.
     */
    CARAPI SetBitsPerSample(
        /* [in] */ Int16 bitsPerSample);

    /**
     * Get the size of audio data after this header, in bytes.
     * @return size of audio data after this header, in bytes.
     */
    CARAPI GetNumBytes(
        /* [out] */ Int32* ret);

    /**
     * Set the size of audio data after this header, in bytes.
     * @param numBytes size of audio data after this header, in bytes.
     * @return reference to this WaveHeader instance.
     */
    CARAPI SetNumBytes(
        /* [in] */ Int32 numBytes);

    /**
     * Read and initialize a WaveHeader.
     * @param in {@link java.io.InputStream} to read from.
     * @return number of bytes consumed.
     * @throws IOException
     */
    CARAPI Read(
        /* [in] */ IInputStream* in,
        /* [out] */ Int32* ret);

    /**
     * Write a WAVE file header.
     * @param out {@link java.io.OutputStream} to receive the header.
     * @return number of bytes written.
     * @throws IOException
     */
    CARAPI Write(
        /* [in] */ IOutputStream* out,
        /* [out] */ Int32* ret);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    static CARAPI ReadId(
        /* [in] */ IInputStream* in,
        /* [in] */ const String& id);

    static CARAPI_(Int32) ReadInt(
        /* [in] */ IInputStream* in);

    static CARAPI_(Int16) ReadShort(
        /* [in] */ IInputStream* in);

    static CARAPI WriteId(
        /* [in] */ IOutputStream* out,
        /* [in] */ const String& id);

    static CARAPI WriteInt(
        /* [in] */ IOutputStream* out,
        /* [in] */ Int32 val);

    static CARAPI WriteShort(
        /* [in] */ IOutputStream* out,
        /* [in] */ Int16 val);

private:
    static const String TAG;            // = "WaveHeader";

    static const Int32 HEADER_LENGTH;   // = 44;

private:
    Int16 mFormat;
    Int16 mNumChannels;
    Int32 mSampleRate;
    Int16 mBitsPerSample;
    Int32 mNumBytes;

};

} // namespace Srec
} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SREC_WaveHeader_H__
