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

#ifndef __ELASTOS_IO_CHARSET_ELASTOS_IO_CHARSET_CHARSETENCODERICU_H__
#define __ELASTOS_IO_CHARSET_ELASTOS_IO_CHARSET_CHARSETENCODERICU_H__

#include "CharsetEncoder.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace IO {
namespace Charset {

class CharsetEncoderICU
    : public CharsetEncoder
{
public:
    CharsetEncoderICU();

    ~CharsetEncoderICU();

    CARAPI Init(
        /* [in] */ ICharset* cs,
        /* [in] */ Float averageBytesPerChar,
        /* [in] */ Float maxBytesPerChar,
        /* [in] */ ArrayOf<Byte>* replacement,
        /* [in] */ Int64 address);

public:
    static CARAPI NewInstance(
        /* [in] */ ICharset* cs,
        /* [in] */ const String& icuCanonicalName,
        /* [out] */ CharsetEncoderICU** encoderICU);

protected:
    CARAPI ImplReplaceWith(
        /* [in] */ ArrayOf<Byte>* newReplacement);

    CARAPI ImplOnMalformedInput(
        /* [in] */ ICodingErrorAction* newAction);

    CARAPI ImplOnUnmappableCharacter(
        /* [in] */ ICodingErrorAction* newAction);

    CARAPI ImplReset();

    CARAPI ImplFlush(
        /* [in] */ IByteBuffer* outBuffer,
        /* [out] */  ICoderResult** result);

    CARAPI EncodeLoop(
        /* [in] */ ICharBuffer* charBuffer,
        /* [in, out] */ IByteBuffer* byteBuffer,
        /* [out] */ ICoderResult** result);

private:
    static CARAPI MakeReplacement(
        /* [in] */ const String& icuCanonicalName,
        /* [in] */ Int64 address,
        /* [out, callee] */ ArrayOf<Byte>** resultArray);

    CARAPI UpdateCallback();

    CARAPI GetArray(
        /* [in] */ IByteBuffer* outBuffer,
        /* [out] */ Int32* result);

    CARAPI GetArray(
        /* [in] */ ICharBuffer* inBuffer,
        /* [out] */ Int32* result);

    CARAPI SetPosition(
        /* [in] */ IByteBuffer* outBuffer);

    CARAPI SetPosition(
        /* [in] */ ICharBuffer* inBuffer);

private:
    static AutoPtr< HashMap<String, AutoPtr<ArrayOf<Byte> > > > DEFAULT_REPLACEMENTS;
    static const Int32 INPUT_OFFSET = 0;
    static const Int32 OUTPUT_OFFSET = 1;
    static const Int32 INVALID_CHAR_COUNT = 2;

    /*
     * data[INPUT_OFFSET]   = on input contains the start of input and on output the number of input chars consumed
     * data[OUTPUT_OFFSET]  = on input contains the start of output and on output the number of output bytes written
     * data[INVALID_CHARS]  = number of invalid chars
     */
    AutoPtr< ArrayOf<Int32> > mData;

    /* handle to the ICU converter that is opened */
    Int64 mConverterHandle;

    AutoPtr< ArrayOf<Char32> > mInput;
    AutoPtr< ArrayOf<Byte> > mOutput;

    AutoPtr< ArrayOf<Char32> > mAllocatedInput;
    AutoPtr< ArrayOf<Byte> > mAllocatedOutput;

    // These instance variables are always assigned in the methods before being used. This class
    // is inherently thread-unsafe so we don't have to worry about synchronization.
    Int32 mInEnd;
    Int32 mOutEnd;
};

} // namespace CharSet
} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CHARSET_ELASTOS_IO_CHARSET_CHARSETENCODERICU_H__
