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

#include "elastos/droid/internal/utility/CIndentingPrintWriter.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CIndentingPrintWriter, Object, IIndentingPrintWriter)

CAR_OBJECT_IMPL(CIndentingPrintWriter)

CIndentingPrintWriter::CIndentingPrintWriter()
    : mWrapLength(0)
    , mCurrentLength(0)
    , mEmptyLine(TRUE)
{}

ECode CIndentingPrintWriter::constructor(
    /* [in] */ IWriter* writer,
    /* [in] */ const String& singleIndent)
{
    return constructor(writer, singleIndent, -1);
}

ECode CIndentingPrintWriter::constructor(
    /* [in] */ IWriter* writer,
    /* [in] */ const String& singleIndent,
    /* [in] */ Int32 wrapLength)
{
    FAIL_RETURN(PrintWriter::constructor(writer))
    mSingleIndent = singleIndent;
    mWrapLength = wrapLength;
    return NOERROR;
}

ECode CIndentingPrintWriter::IncreaseIndent()
{
    FAIL_RETURN(mIndentBuilder.Append(mSingleIndent))
    mCurrentIndent = NULL;
    return NOERROR;
}

ECode CIndentingPrintWriter::DecreaseIndent()
{
    FAIL_RETURN(mIndentBuilder.Delete(0, mSingleIndent.GetLength()))
    mCurrentIndent = NULL;
    return NOERROR;
}

ECode CIndentingPrintWriter::PrintPair(
    /* [in] */ const String& key,
    /* [in] */ IInterface* value)
{
    String str;
    FAIL_RETURN(IObject::Probe(value)->ToString(&str))

    StringBuilder sb;
    sb += String(key);
    sb += String("=");
    sb += str;
    sb += String(" ");
    return Print(sb.ToString());
}

ECode CIndentingPrintWriter::PrintHexPair(
    /* [in] */ const String& key,
    /* [in] */ Int32 value)
{
    StringBuilder sb;
    sb += String(key);
    sb += String("=0x");
    sb += StringUtils::ToString(value, 16);
    sb += String(" ");
    return Print(sb.ToString());
}

ECode CIndentingPrintWriter::Write(
    /* [in] */ ArrayOf<Char32>* buf,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    const Int32 indentLength = mIndentBuilder.GetLength();
    const Int32 bufferEnd = offset + count;
    Int32 lineStart = offset;
    Int32 lineEnd = offset;

    // March through incoming buffer looking for newlines
    while (lineEnd < bufferEnd) {
        Char32 ch = (*buf)[lineEnd++];
        mCurrentLength++;
        if (ch == '\n') {
            FAIL_RETURN(MaybeWriteIndent());
            FAIL_RETURN(PrintWriter::Write(buf, lineStart, lineEnd - lineStart));
            lineStart = lineEnd;
            mEmptyLine = TRUE;
            mCurrentLength = 0;
        }

        // Wrap if we've pushed beyond line length
        if (mWrapLength > 0 && mCurrentLength >= mWrapLength - indentLength) {
            if (!mEmptyLine) {
                // Give ourselves a fresh line to work with
                FAIL_RETURN(PrintWriter::Write('\n'));
                mEmptyLine = TRUE;
                mCurrentLength = lineEnd - lineStart;
            }
            else {
                // We need more than a dedicated line, slice it hard
                FAIL_RETURN(MaybeWriteIndent());
                FAIL_RETURN(PrintWriter::Write(buf, lineStart, lineEnd - lineStart));
                FAIL_RETURN(PrintWriter::Write('\n'));
                mEmptyLine = TRUE;
                lineStart = lineEnd;
                mCurrentLength = 0;
            }
        }
    }

    if (lineStart != lineEnd) {
        FAIL_RETURN(MaybeWriteIndent());
        FAIL_RETURN(PrintWriter::Write(buf, lineStart, lineEnd - lineStart));
    }
    return NOERROR;
}

ECode CIndentingPrintWriter::MaybeWriteIndent()
{
    if (mEmptyLine) {
        mEmptyLine = FALSE;
        if (mIndentBuilder.GetLength() != 0) {
            if (mCurrentIndent == NULL) {
                mCurrentIndent = mIndentBuilder.ToString().GetChars();
            }
            FAIL_RETURN(PrintWriter::Write(mCurrentIndent, 0, mCurrentIndent->GetLength()));
        }
    }
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
