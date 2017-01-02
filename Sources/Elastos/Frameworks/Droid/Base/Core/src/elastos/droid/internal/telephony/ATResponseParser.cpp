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

#include "elastos/droid/internal/telephony/ATResponseParser.h"
#include <elastos/core/Character.h>

using Elastos::Core::Character;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(ATResponseParser, Object, IATResponseParser)
ATResponseParser::ATResponseParser(
    /* [in] */ const String& line)
    : mNext(0)
    , mTokStart(0)
    , mTokEnd(0)
{
    mLine = line;
}

ECode ATResponseParser::NextBoolean(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    // "\s*(\d)(,|$)"
    // \d is '0' or '1'

    FAIL_RETURN(NextTok());

    if (mTokEnd - mTokStart > 1) {
        // throw new ATParseEx();
        return E_TELEPHONEY_AT_PARSE_EXCEPTION;
    }
    Char32 c = mLine.GetChar(mTokStart);

    if (c == '0') {
        *value = FALSE;
        return NOERROR;
    }
    if (c ==  '1') {
        *value = TRUE;
        return NOERROR;
    }
    // throw new ATParseEx();
    return E_TELEPHONEY_AT_PARSE_EXCEPTION;
}

/** positive Int32 only */
ECode ATResponseParser::NextInt32(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;
    // "\s*(\d+)(,|$)"
    Int32 ret = 0;

    FAIL_RETURN(NextTok());

    for (Int32 i = mTokStart ; i < mTokEnd ; i++) {
        Char32 c = mLine.GetChar(i);

        // Yes, ASCII decimal digits only
        if (c < '0' || c > '9') {
            // throw new ATParseEx();
            return E_TELEPHONEY_AT_PARSE_EXCEPTION;
        }

        ret *= 10;
        ret += c - '0';
    }

    *value = ret;
    return NOERROR;
}

ECode ATResponseParser::NextString(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);

    FAIL_RETURN(NextTok());

    *value = mLine.Substring(mTokStart, mTokEnd);
    return NOERROR;
}

ECode ATResponseParser::HasMore(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = mNext < mLine.GetLength();
    return NOERROR;
}

ECode ATResponseParser::NextTok()
{
    Int32 len = mLine.GetLength();

    if (mNext == 0) {
        FAIL_RETURN(SkipPrefix());
    }

    if (mNext >= len) {
        // throw new ATParseEx();
        return E_TELEPHONEY_AT_PARSE_EXCEPTION;
    }

    // \s*("([^"]*)"|(.*)\s*)(,|$)
    Char32 c = mLine.GetChar(mNext++);
    Boolean hasQuote = FALSE;

    Char32 _c = c;
    FAIL_RETURN(SkipWhiteSpace(_c, &c));

    if (c == '"') {
        if (mNext >= len) {
            // throw new ATParseEx();
            return E_TELEPHONEY_AT_PARSE_EXCEPTION;
        }
        c = mLine.GetChar(mNext++);
        if (c == String::INVALID_CHAR) {
            return E_TELEPHONEY_AT_PARSE_EXCEPTION;
        }
        mTokStart = mNext - 1;
        while (c != '"' && mNext < len) {
            c = mLine.GetChar(mNext++);
            if (c == String::INVALID_CHAR) {
                return E_TELEPHONEY_AT_PARSE_EXCEPTION;
            }
        }
        if (c != '"') {
            // throw new ATParseEx();
            return E_TELEPHONEY_AT_PARSE_EXCEPTION;
        }
        mTokEnd = mNext - 1;
        if (mNext < len && mLine.GetChar(mNext++) != ',') {
            // throw new ATParseEx();
            return E_TELEPHONEY_AT_PARSE_EXCEPTION;
        }
    }
    else {
        mTokStart = mNext - 1;
        mTokEnd = mTokStart;
        while (c != ',') {
            if (!Character::IsWhitespace(c)) {
                mTokEnd = mNext;
            }
            if (mNext == len) {
                break;
            }
            c = mLine.GetChar(mNext++);
            if (c == String::INVALID_CHAR) {
                return E_TELEPHONEY_AT_PARSE_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

/** Throws ATParseEx if whitespace extends to the end of string */
ECode ATResponseParser::SkipWhiteSpace(
    /* [in] */ Char32 c,
    /* [out] */ Char32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 len;
    len = mLine.GetLength();
    while (mNext < len && Character::IsWhitespace(c)) {
        c = mLine.GetChar(mNext++);
    }

    if (Character::IsWhitespace(c)) {
        // throw new ATParseEx();
        return E_TELEPHONEY_AT_PARSE_EXCEPTION;
    }
    *value = c;
    return NOERROR;
}

ECode ATResponseParser::SkipPrefix()
{
    // consume "^[^:]:"
    mNext = 0;
    Int32 s = mLine.GetLength();
    while (mNext < s){
        Char32 c = mLine.GetChar(mNext++);

        if (c == ':') {
            return NOERROR;
        }
    }

    // throw new ATParseEx("missing prefix");
    return E_TELEPHONEY_AT_PARSE_EXCEPTION;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
