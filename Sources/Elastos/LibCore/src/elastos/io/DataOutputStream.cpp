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

#include "DataOutputStream.h"
#include "Math.h"
#include "Character.h"

using Elastos::Core::Character;
using Elastos::Core::Math;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_2(DataOutputStream, FilterOutputStream, IDataOutputStream, IDataOutput)

static const Char32 kByteMask = 0x000000BF;
static const Char32 kByteMark = 0x00000080;

// Mask used to set appropriate bits in first byte of UTF-8 sequence,
// indexed by number of bytes in the sequence.
// 0xxxxxxx
// -> (00-7f) 7bit. Bit mask for the first byte is 0x00000000
// 110yyyyx 10xxxxxx
// -> (c0-df)(80-bf) 11bit. Bit mask is 0x000000C0
// 1110yyyy 10yxxxxx 10xxxxxx
// -> (e0-ef)(80-bf)(80-bf) 16bit. Bit mask is 0x000000E0
// 11110yyy 10yyxxxx 10xxxxxx 10xxxxxx
// -> (f0-f7)(80-bf)(80-bf)(80-bf) 21bit. Bit mask is 0x000000F0
static const Char32 kFirstByteMark[] = {
    0x00000000, 0x00000000, 0x000000C0, 0x000000E0, 0x000000F0
};

DataOutputStream::DataOutputStream()
    : mWritten(0)
{
    mBuff = ArrayOf<Byte>::Alloc(8);
    assert(mBuff);
}

DataOutputStream::~DataOutputStream()
{
}

ECode DataOutputStream::constructor(
    /* [in] */ IOutputStream* outs)
{
    return FilterOutputStream::constructor(outs);
}

ECode DataOutputStream::Flush()
{
    return FilterOutputStream::Flush();
}

ECode DataOutputStream::GetSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (mWritten < 0) {
        mWritten = Elastos::Core::Math::INT32_MAX_VALUE;
    }
    *value = mWritten;
    return NOERROR;
}

ECode DataOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    FAIL_RETURN(mOut->Write(oneByte));
    mWritten++;
    return NOERROR;
}

ECode DataOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    // BEGIN android-note
    // changed array notation to be consistent with the rest of harmony
    // END android-note
    FAIL_RETURN(mOut->Write(buffer, offset, count));
    mWritten += count;
    return NOERROR;
}

ECode DataOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    VALIDATE_NOT_NULL(buffer)
    return Write(buffer, 0, buffer->GetLength());
}

ECode DataOutputStream::WriteBoolean(
    /* [in] */ Boolean val)
{
    FAIL_RETURN(mOut->Write(val? 1 : 0));
    mWritten++;
    return NOERROR;
}

ECode DataOutputStream::WriteByte(
    /* [in] */ Int32 val)
{
    FAIL_RETURN(mOut->Write(val));
    mWritten++;
    return NOERROR;
}

ECode DataOutputStream::WriteBytes(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    FAIL_RETURN(mOut->Write(buffer));
    mWritten += buffer->GetLength();
    return NOERROR;
}

ECode DataOutputStream::WriteBytes(
    /* [in] */ const String& str)
{
    if (str.IsNullOrEmpty())
        return NOERROR;
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(str.GetByteLength());
    memcpy(bytes->GetPayload(), (const char*)str, str.GetByteLength());

    return mOut->Write(bytes);
}

ECode DataOutputStream::WriteChar(
    /* [in] */ Int32 val)
{
    Int32 len = CountUTFBytes((Char32)val);
    WriteUTFBytesToBuffer(mBuff->GetPayload(), (Char32)val, len);
    FAIL_RETURN(mOut->Write(mBuff, 0, len));
    mWritten += len;
    return NOERROR;
}

ECode DataOutputStream::WriteChars(
    /* [in] */ const String& str)
{
    if (str.IsNullOrEmpty())
        return NOERROR;

    AutoPtr<ArrayOf<Char32> > chs = str.GetChars();
    return mOut->Write((ArrayOf<Byte>*)chs.Get());
}

ECode DataOutputStream::WriteDouble(
    /* [in] */ Double val)
{
    return WriteInt64(Elastos::Core::Math::DoubleToInt64Bits(val));
}

ECode DataOutputStream::WriteFloat(
    /* [in] */ Float val)
{
    return WriteInt32(Elastos::Core::Math::FloatToInt32Bits(val));
}

ECode DataOutputStream::WriteInt32(
    /* [in] */ Int32 val)
{
    (*mBuff)[0] = (Byte)(val >> 24);
    (*mBuff)[1] = (Byte)(val >> 16);
    (*mBuff)[2] = (Byte)(val >> 8);
    (*mBuff)[3] = (Byte)val;
    FAIL_RETURN(mOut->Write(mBuff, 0, 4));
    mWritten += 4;
    return NOERROR;
}

ECode DataOutputStream::WriteInt64(
    /* [in] */ Int64 val)
{
    (*mBuff)[0] = (Byte)(val >> 56);
    (*mBuff)[1] = (Byte)(val >> 48);
    (*mBuff)[2] = (Byte)(val >> 40);
    (*mBuff)[3] = (Byte)(val >> 32);
    (*mBuff)[4] = (Byte)(val >> 24);
    (*mBuff)[5] = (Byte)(val >> 16);
    (*mBuff)[6] = (Byte)(val >> 8);
    (*mBuff)[7] = (Byte)val;
    FAIL_RETURN(mOut->Write(mBuff, 0, 8));
    mWritten += 8;
    return NOERROR;
}

ECode DataOutputStream::WriteInt16(
    /* [in] */ Int32 val)
{
    (*mBuff)[0] = (Byte)(val >> 8);
    (*mBuff)[1] = (Byte)val;
    FAIL_RETURN(mOut->Write(mBuff, 0, 2));
    mWritten += 2;
    return NOERROR;
}

ECode DataOutputStream::WriteUTF(
    /* [in] */ const String& str)
{
    return WriteBytes(str);
}

// Return number of utf8 bytes required for the character.
Int32 DataOutputStream::CountUTFBytes(
    /* [in] */ Char32 srcChar)
{
    Int32 bytesToWrite;

    // Figure out how many bytes the result will require.
    if (srcChar < 0x00000080) {
        bytesToWrite = 1;
    }
    else if (srcChar < 0x00000800) {
        bytesToWrite = 2;
    }
    else if (srcChar < 0x00010000) {
        if ((srcChar < Character::MIN_HIGH_SURROGATE)
            || (srcChar > Character::MAX_LOW_SURROGATE)) {
            bytesToWrite = 3;
        }
        else {
            // Surrogates are invalid UTF-32 characters.
            return 0;
        }
    }
    // Max code point for Unicode is 0x0010FFFF.
    else if (srcChar <= Character::MAX_CODE_POINT) {
        bytesToWrite = 4;
    }
    else {
        // Invalid UTF-32 character.
        return 0;
    }

    return bytesToWrite;
}

// Write out the source character to <dstP>.
void DataOutputStream::WriteUTFBytesToBuffer(
    /* [in] */ Byte* dstP,
    /* [in] */ Char32 srcChar,
    /* [in] */ Int32 bytes)
{
    dstP += bytes;
    switch (bytes)
    {   /* note: everything falls through. */
        case 4: *--dstP = (Byte)((srcChar | kByteMark) & kByteMask); srcChar >>= 6;
        case 3: *--dstP = (Byte)((srcChar | kByteMark) & kByteMask); srcChar >>= 6;
        case 2: *--dstP = (Byte)((srcChar | kByteMark) & kByteMask); srcChar >>= 6;
        case 1: *--dstP = (Byte)(srcChar | kFirstByteMark[bytes]);
    }
}

} // namespace IO
} // namespace Elastos
