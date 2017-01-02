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

#include "CBase64.h"

namespace Libcore {
namespace IO {

const Byte CBase64::sMap[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
        'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
        'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
        'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'};

CAR_SINGLETON_IMPL(CBase64)

CAR_INTERFACE_IMPL(CBase64, Singleton, IBase64)

ECode CBase64::Decode(
    /* [in] */ ArrayOf<Byte>* inData,
    /* [out, callee] */ ArrayOf<Byte>** outData)
{
    return Decode(inData, inData->GetLength(), outData);
}

ECode CBase64::Decode(
    /* [in] */ ArrayOf<Byte>* inData,
    /* [in] */ Int32 len,
    /* [out, callee] */ ArrayOf<Byte>** outData)
{
    VALIDATE_NOT_NULL(outData)
    // approximate output length
    Int32 length = len / 4 * 3;
    // return an empty array on empty or short input without padding
    if (length == 0) {
        //Todo later
        //return EmptyArray.BYTE;
        return NOERROR;
    }
    // temporary array
    AutoPtr<ArrayOf<Byte> > out = ArrayOf<Byte>::Alloc(length);
    // number of padding characters ('=')
    Int32 pad = 0;
    Byte chr;
    // compute the number of the padding characters
    // and adjust the length of the input
    for (;;len--) {
        chr = (*inData)[len-1];
        // skip the neutral characters
        if ((chr == '\n') || (chr == '\r') ||
                (chr == ' ') || (chr == '\t')) {
            continue;
        }
        if (chr == '=') {
            pad++;
        } else {
            break;
        }
    }
    // index in the output array
    Int32 outIndex = 0;
    // index in the input array
    Int32 inIndex = 0;
    // holds the value of the input character
    Int32 bits = 0;
    // holds the value of the input quantum
    Int32 quantum = 0;
    for (Int32 i=0; i<len; i++) {
        chr = (*inData)[i];
        // skip the neutral characters
        if ((chr == '\n') || (chr == '\r') ||
                (chr == ' ') || (chr == '\t')) {
            continue;
        }
        if ((chr >= 'A') && (chr <= 'Z')) {
            // char ASCII value
            //  A    65    0
            //  Z    90    25 (ASCII - 65)
            bits = chr - 65;
        } else if ((chr >= 'a') && (chr <= 'z')) {
            // char ASCII value
            //  a    97    26
            //  z    122   51 (ASCII - 71)
            bits = chr - 71;
        } else if ((chr >= '0') && (chr <= '9')) {
            // char ASCII value
            //  0    48    52
            //  9    57    61 (ASCII + 4)
            bits = chr + 4;
        } else if (chr == '+') {
            bits = 62;
        } else if (chr == '/') {
            bits = 63;
        } else {
            return NOERROR;
        }
        // append the value to the quantum
        quantum = (quantum << 6) | (Byte) bits;
        if (inIndex%4 == 3) {
            // 4 characters were read, so make the output:
            (*out)[outIndex++] = (Byte) (quantum >> 16);
            (*out)[outIndex++] = (Byte) (quantum >> 8);
            (*out)[outIndex++] = (Byte) quantum;
        }
        inIndex++;
    }
    if (pad > 0) {
        // adjust the quantum value according to the padding
        quantum = quantum << (6*pad);
        // make output
        (*out)[outIndex++] = (Byte) (quantum >> 16);
        if (pad == 1) {
            (*out)[outIndex++] = (Byte) (quantum >> 8);
        }
    }
    // create the resulting array
    *outData = ArrayOf<Byte>::Alloc(outIndex);
    (*outData)->Copy(0, out, 0, outIndex);
    REFCOUNT_ADD(*outData)
    return NOERROR;
}

ECode CBase64::Encode(
    /* [in] */ ArrayOf<Byte>* inData,
    /* [out] */ String* outData)
{
    VALIDATE_NOT_NULL(outData)
    Int32 length = (inData->GetLength() + 2) * 4 / 3;
    AutoPtr<ArrayOf<Byte> > out = ArrayOf<Byte>::Alloc(length);
    Int32 index = 0, end = inData->GetLength() - inData->GetLength() % 3;
    for (Int32 i = 0; i < end; i += 3) {
        (*out)[index++] = sMap[((*inData)[i] & 0xff) >> 2];
        (*out)[index++] = sMap[(((*inData)[i] & 0x03) << 4) | (((*inData)[i+1] & 0xff) >> 4)];
        (*out)[index++] = sMap[(((*inData)[i+1] & 0x0f) << 2) | (((*inData)[i+2] & 0xff) >> 6)];
        (*out)[index++] = sMap[((*inData)[i+2] & 0x3f)];
    }
    switch (inData->GetLength() % 3) {
        case 1:
            (*out)[index++] = sMap[((*inData)[end] & 0xff) >> 2];
            (*out)[index++] = sMap[((*inData)[end] & 0x03) << 4];
            (*out)[index++] = '=';
            (*out)[index++] = '=';
            break;
        case 2:
            (*out)[index++] = sMap[((*inData)[end] & 0xff) >> 2];
            (*out)[index++] = sMap[(((*inData)[end] & 0x03) << 4) | (((*inData)[end+1] & 0xff) >> 4)];
            (*out)[index++] = sMap[(((*inData)[end+1] & 0x0f) << 2)];
            (*out)[index++] = '=';
            break;
    } // end switch
    *outData = String((char*)out->GetPayload(), index);
    return NOERROR;
}

} // namespace IO
} // namespace Libcore

