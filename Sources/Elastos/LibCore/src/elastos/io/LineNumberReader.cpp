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

#include "LineNumberReader.h"
#include "Character.h"
#include "AutoLock.h"

using Elastos::Core::Character;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(LineNumberReader, BufferedReader, ILineNumberReader)

LineNumberReader::LineNumberReader()
    : mLineNumber(0)
    , mMarkedLineNumber(-1)
    , mLastWasCR(FALSE)
    , mMarkedLastWasCR(FALSE)
{
}

LineNumberReader::~LineNumberReader()
{
}

ECode LineNumberReader::constructor(
    /* [in] */ IReader* ir)
{
    return BufferedReader::constructor(ir);
}

ECode LineNumberReader::constructor(
    /* [in] */ IReader* ir,
    /* [in] */ Int32 size)
{
    return BufferedReader::constructor(ir, size);
}

ECode LineNumberReader::GetLineNumber(
    /* [out] */ Int32* lineNumber)
{
    VALIDATE_NOT_NULL(lineNumber)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    *lineNumber = mLineNumber;

    return NOERROR;
}

ECode LineNumberReader::Mark(
    /* [in] */ Int32 readLimit)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    FAIL_RETURN(BufferedReader::Mark(readLimit));
    mMarkedLineNumber = mLineNumber;
    mMarkedLastWasCR = mLastWasCR;

    return NOERROR;
}

ECode LineNumberReader::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    Int32 ch;
    FAIL_RETURN(BufferedReader::Read(&ch));
    if (ch == '\n' && mLastWasCR) {
        FAIL_RETURN(BufferedReader::Read(&ch));
    }
    mLastWasCR = FALSE;
    switch (ch) {
        case '\r':
            ch = '\n';
            mLastWasCR = TRUE;
            // fall through
        case '\n':
            mLineNumber++;
    }
    *value = ch;

    return NOERROR;
}

ECode LineNumberReader::Read(
    /* [out] */ ArrayOf<Char32>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(buffer)
    assert(mLock != NULL);
    AutoLock lock(mLock);

    Int32 read;
    FAIL_RETURN(BufferedReader::Read(buffer, offset, length, &read));
    if (read == -1) {
        *number = -1;
        return NOERROR;
    }

    for (Int32 i = 0; i < read; i++) {
        Char32 ch = (*buffer)[i];
        if (ch == '\r') {
            mLineNumber++;
            mLastWasCR = TRUE;
        }
        else if (ch == '\n') {
            if (!mLastWasCR) {
                mLineNumber++;
            }
            mLastWasCR = FALSE;
        }
        else {
            mLastWasCR = FALSE;
        }
    }
    *number = read;

    return NOERROR;
}

ECode LineNumberReader::ReadLine(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);
    assert(mLock != NULL);
    AutoLock lock(mLock);

    if (mLastWasCR) {
        FAIL_RETURN(ChompNewline());
        mLastWasCR = FALSE;
    }
    String lstr;
    FAIL_RETURN(BufferedReader::ReadLine(&lstr));
    if (!lstr.IsNull()) {
        mLineNumber++;
    }
    *result = lstr;

    return NOERROR;
}

ECode LineNumberReader::Reset()
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    FAIL_RETURN(BufferedReader::Reset());
    mLineNumber = mMarkedLineNumber;
    mLastWasCR = mMarkedLastWasCR;

    return NOERROR;
}

ECode LineNumberReader::SetLineNumber(
    /* [in] */ Int32 lineNumber)
{
    assert(mLock != NULL);
    AutoLock lock(mLock);

    mLineNumber = lineNumber;

    return NOERROR;
}

ECode LineNumberReader::Skip(
    /* [in] */ Int64 count,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (count < 0) {
//      throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    assert(mLock != NULL);
    AutoLock lock(mLock);

    Int32 value;
    for (Int32 i = 0; i < count; i++) {
        Read(&value);
        if (value == -1) {
            *number = i;
            return NOERROR;
        }
    }
    *number = count;

    return NOERROR;
}

} // namespace IO
} // namespace Elastos
