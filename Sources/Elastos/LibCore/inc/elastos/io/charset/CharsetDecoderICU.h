#ifndef __ELASTOS_IO_CHARSETDECODERICU_H__
#define __ELASTOS_IO_CHARSETDECODERICU_H__

#include "CharsetDecoder.h"

namespace Elastos {
namespace IO {
namespace Charset {

class CharsetDecoderICU
    : public CharsetDecoder
{
public:
    CharsetDecoderICU();

    ~CharsetDecoderICU();

    CARAPI constructor(
        /* [in] */ ICharset* cs,
        /* [in] */ Float averageCharsPerByte,
        /* [in] */ Int64 address);

public:
    static CARAPI NewInstance(
        /* [in] */ ICharset* cs,
        /* [in] */ const String& icuCanonicalName,
        /* [out] */ CharsetDecoderICU** decoderICU);

protected:
    CARAPI ImplReplaceWith(
        /* [in] */ const String& newReplacement);

    CARAPI ImplOnMalformedInput(
        /* [in] */ ICodingErrorAction* newAction);

    CARAPI ImplOnUnmappableCharacter(
        /* [in] */ ICodingErrorAction* newAction);

    CARAPI ImplReset();

    CARAPI ImplFlush(
        /* [in] */ ICharBuffer* outInput,
        /* [out] */ ICoderResult** result);

    CARAPI DecodeLoop(
        /* [in] */ IByteBuffer* byteBuffer,
        /* [in] */ ICharBuffer* charBuffer,
        /* [out] */ ICoderResult** result);

private:
    CARAPI UpdateCallback();

    CARAPI GetArray(
        /* [in] */ ICharBuffer* outBuffer,
        /* [out] */ Int32* result);

    CARAPI GetArray(
        /* [in] */ IByteBuffer* inBuffer,
        /* [out] */ Int32* result);

    CARAPI SetPosition(
        /* [in] */ ICharBuffer* outBuffer);

    CARAPI SetPosition(
        /* [in] */ IByteBuffer* inBuffer);

private:
    static const Int32 MAX_CHARS_PER_BYTE = 2;

    static const Int32 INPUT_OFFSET = 0;
    static const Int32 OUTPUT_OFFSET = 1;
    static const Int32 INVALID_CHAR_COUNT = 2;

    /*
     * data[INPUT_OFFSET]   = on input contains the start of input and on output the number of input bytes consumed
     * data[OUTPUT_OFFSET]  = on input contains the start of output and on output the number of output chars written
     * data[INVALID_BYTES]  = number of invalid bytes
     */
    AutoPtr< ArrayOf<Int32> > mData;

    /* handle to the ICU converter that is opened */
    Int64 mConverterHandle;

    AutoPtr< ArrayOf<Byte> > mInput;
    AutoPtr< ArrayOf<Char32> > mOutput;

    AutoPtr< ArrayOf<Byte> > mAllocatedInput;
    AutoPtr< ArrayOf<Char32> > mAllocatedOutput;

    // These instance variables are always assigned in the methods before being used. This class
    // is inherently thread-unsafe so we don't have to worry about synchronization.
    Int32 mInEnd;
    Int32 mOutEnd;
};

} // namespace CharSet
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSETDECODERICU_H__
