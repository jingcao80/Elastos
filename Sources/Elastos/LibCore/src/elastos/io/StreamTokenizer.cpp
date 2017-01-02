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

#include "StreamTokenizer.h"
#include "StringBuilder.h"
#include "StringUtils.h"
#include "CLocale.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CLocale;

namespace Elastos {
namespace IO {

const Byte StreamTokenizer::TOKEN_COMMENT = 1;

const Byte StreamTokenizer::TOKEN_QUOTE = 2;

const Byte StreamTokenizer::TOKEN_WHITE = 4;

const Byte StreamTokenizer::TOKEN_WORD = 8;

const Int32 StreamTokenizer::TT_UNKNOWN = -4;

const Byte StreamTokenizer::TOKEN_DIGIT = 16;

CAR_INTERFACE_IMPL(StreamTokenizer, Object, IStreamTokenizer)

StreamTokenizer::StreamTokenizer()
    : mTtype(TT_UNKNOWN)
    , mLineNumber(1)
    , mPeekChar(-2)
{
    mTokenTypes = ArrayOf<Byte>::Alloc(256);
}

ECode StreamTokenizer::constructor(
    /* [in] */ IInputStream* is)
{
    constructor();
    if (NULL == is) {
        // throw new NullPointerException("is == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mInStream = is;
    return NOERROR;
}

ECode StreamTokenizer::constructor(
    /* [in] */ IReader* r)
{
    constructor();
    if (NULL == r) {
        // throw new NullPointerException("r == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mInReader = r;
    return NOERROR;
}

ECode StreamTokenizer::SetCommentChar(
    /* [in] */ Int32 ch)
{
    if (ch >= 0 && ch < mTokenTypes->GetLength()) {
        (*mTokenTypes)[ch] = TOKEN_COMMENT;
    }
    return NOERROR;
}

ECode StreamTokenizer::SetEolIsSignificant(
    /* [in] */ Boolean flag)
{
    mIsEOLSignificant = flag;
    return NOERROR;
}

ECode StreamTokenizer::GetLineno(
    /* [out] */ Int32* rev)
{
    VALIDATE_NOT_NULL(rev)
    *rev = mLineNumber;
    return NOERROR;
}

ECode StreamTokenizer::SetLowerCaseMode(
    /* [in] */ Boolean flag)
{
    mForceLowercase = flag;
    return NOERROR;
}

ECode StreamTokenizer::GetNextToken(
    /* [out] */ Int32* rev)
{
    VALIDATE_NOT_NULL(rev)

    if (mPushBackToken) {
        mPushBackToken = FALSE;
        if (mTtype != TT_UNKNOWN) {
            *rev = mTtype;
            return NOERROR;
        }
    }
    mSval = NULL; // Always reset sval to null
    Int32 currentChar;
    if (mPeekChar == -2) {
        Read(&currentChar);
    } else {
        currentChar = mPeekChar;
    }

    if (mLastCr && '\n' == currentChar) {
        mLastCr = FALSE;
        Read(&currentChar);
    }
    if (-1 == currentChar) {
        *rev = (mTtype = TT_EOF);
        return NOERROR;
    }

    Byte currentType = currentChar > 255 ? TOKEN_WORD
            : (*mTokenTypes)[currentChar];
    while ((currentType & TOKEN_WHITE) != 0) {
        /**
         * Skip over white space until we hit a new line or a real token
         */
        if ('\r' == currentChar) {
            mLineNumber++;
            if (mIsEOLSignificant) {
                mLastCr = TRUE;
                mPeekChar = -2;
                *rev = (mTtype = TT_EOL);
                return NOERROR;
            }
            Read(&currentChar);
            if ('\n' == currentChar) {
                Read(&currentChar);
            }
        } else if ('\n' == currentChar) {
            mLineNumber++;
            if (mIsEOLSignificant) {
                mPeekChar = -2;
                *rev = (mTtype = TT_EOL);
                return NOERROR;
            }
            Read(&currentChar);
        } else {
            // Advance over this white space character and try again.
            Read(&currentChar);
        }
        if (-1 == currentChar) {
            *rev = (mTtype = TT_EOF);
            return NOERROR;
        }
        currentType = currentChar > 255 ? TOKEN_WORD
                : (*mTokenTypes)[currentChar];
    }

    /**
     * Check for digits before checking for words since digits can be
     * contained within words.
     */
    if ((currentType & TOKEN_DIGIT) != 0) {
        AutoPtr<StringBuilder> digits = new StringBuilder(20);
        Boolean haveDecimal = FALSE, checkJustNegative = currentChar == '-';
        while (TRUE) {
            if (currentChar == '.') {
                haveDecimal = TRUE;
            }
            digits->AppendChar(currentChar);
            Read(&currentChar);
            if ((currentChar < '0' || currentChar > '9')
                    && (haveDecimal || currentChar != '.')) {
                break;
            }
        }
        mPeekChar = currentChar;
        if (checkJustNegative && 1 == digits->GetLength()) {
            // Didn't get any other digits other than '-'
            *rev = (mTtype = '-');
            return NOERROR;
        }
        // try {
            mNval = 0;
            String s;
            digits->ToString(&s);
            FAIL_RETURN(StringUtils::Parse(s, &mNval));
        // } catch (NumberFormatException e) {
            // Unsure what to do, will write test.
            // nval = 0;
        // }
        *rev = (mTtype = TT_NUMBER);
        return NOERROR;
    }
    // Check for words
    if ((currentType & TOKEN_WORD) != 0) {
        AutoPtr<StringBuilder> word = new StringBuilder(20);
        while (TRUE) {
            word->AppendChar(currentChar);
            Read(&currentChar);
            if (-1 == currentChar
                    || (currentChar < 256 && ((*mTokenTypes)[currentChar] & (TOKEN_WORD | TOKEN_DIGIT)) == 0)) {
                break;
            }
        }
        mPeekChar = currentChar;
        word->ToString(&mSval);
        if (mForceLowercase) {
            mSval = mSval.ToLowerCase();    // sval = sval.toLowerCase(Locale.getDefault());
        }
        *rev = (mTtype = TT_WORD);
        return NOERROR;
    }
    // Check for quoted character
    if (currentType == TOKEN_QUOTE) {
        Int32 matchQuote = currentChar;
        AutoPtr<StringBuilder> quoteString = new StringBuilder();
        Int32 peekOne;
        Read(&peekOne);
        while (peekOne >= 0 && peekOne != matchQuote && peekOne != '\r'
                && peekOne != '\n') {
            Boolean readPeek = TRUE;
            if (peekOne == '\\') {
                Int32 c1;
                Read(&c1);
                // Check for quoted octal IE: \377
                if (c1 <= '7' && c1 >= '0') {
                    Int32 digitValue = c1 - '0';
                    Read(&c1);
                    if (c1 > '7' || c1 < '0') {
                        readPeek = FALSE;
                    } else {
                        digitValue = digitValue * 8 + (c1 - '0');
                        Read(&c1);
                        // limit the digit value to a byte
                        if (digitValue > 037 || c1 > '7' || c1 < '0') {
                            readPeek = FALSE;
                        } else {
                            digitValue = digitValue * 8 + (c1 - '0');
                        }
                    }
                    if (!readPeek) {
                        // We've consumed one to many
                        quoteString->AppendChar( digitValue);
                        peekOne = c1;
                    } else {
                        peekOne = digitValue;
                    }
                } else {
                    switch (c1) {
                        case 'a':
                            peekOne = 0x7;
                            break;
                        case 'b':
                            peekOne = 0x8;
                            break;
                        case 'f':
                            peekOne = 0xc;
                            break;
                        case 'n':
                            peekOne = 0xA;
                            break;
                        case 'r':
                            peekOne = 0xD;
                            break;
                        case 't':
                            peekOne = 0x9;
                            break;
                        case 'v':
                            peekOne = 0xB;
                            break;
                        default:
                            peekOne = c1;
                    }
                }
            }
            if (readPeek) {
                quoteString->AppendChar( peekOne);
                Read(&peekOne);
            }
        }
        if (peekOne == matchQuote) {
            Read(&peekOne);
        }
        mPeekChar = peekOne;
        mTtype = matchQuote;
        quoteString->ToString(&mSval);
        *rev = mTtype;
        return NOERROR;
    }
    // Do comments, both "//" and "/*stuff*/"
    if (currentChar == '/' && (mSlashSlashComments || mSlashStarComments)) {
        Read(&currentChar);
        if ('*' == currentChar && mSlashStarComments) {
            Int32 peekOne;
            Read(&peekOne);
            while (TRUE) {
                currentChar = peekOne;
                Read(&peekOne);
                if (currentChar == -1) {
                    mPeekChar = -1;
                    *rev = (mTtype = TT_EOF);
                    return NOERROR;
                }
                if (currentChar == '\r') {
                    if (peekOne == '\n') {
                        Read(&peekOne);
                    }
                    mLineNumber++;
                } else if (currentChar == '\n') {
                    mLineNumber++;
                } else if (currentChar == '*' && peekOne == '/') {
                    Read(&mPeekChar);
                    GetNextToken(rev);
                    return NOERROR;
                }
            }
        } else if (currentChar == '/' && mSlashSlashComments) {
            // Skip to EOF or new line then return the next token
            do
            {
                Read(&currentChar);
            }while(currentChar>= 0 && currentChar != '\r' && currentChar != '\n');
            mPeekChar = currentChar;
            GetNextToken(rev);
            return NOERROR;
        } else if (currentType != TOKEN_COMMENT) {
            // Was just a slash by itself
            mPeekChar = currentChar;
            *rev = (mTtype = '/');
            return NOERROR;
        }
    }
    // Check for comment character
    if (currentType == TOKEN_COMMENT) {
        // Skip to EOF or new line then return the next token
        do
        {
            Read(&currentChar);
        }while(currentChar>= 0 && currentChar != '\r' && currentChar != '\n');
        mPeekChar = currentChar;
        GetNextToken(rev);
        return NOERROR;
    }

    Read(&mPeekChar);
    *rev = (mTtype = currentChar);
    return NOERROR;
}

ECode StreamTokenizer::SetOrdinaryChar(
    /* [in] */ Int32 ch)
{
    if (ch >= 0 && ch < mTokenTypes->GetLength()) {
        (*mTokenTypes)[ch] = 0;
    }
    return NOERROR;
}

ECode StreamTokenizer::SetOrdinaryChars(
    /* [in] */ Int32 low,
    /* [in] */ Int32 hi)
{
    if (low < 0) {
        low = 0;
    }
    if (hi > mTokenTypes->GetLength()) {
        hi = mTokenTypes->GetLength() - 1;
    }
    for (Int32 i = low; i <= hi; i++) {
        (*mTokenTypes)[i] = 0;
    }
    return NOERROR;
}

ECode StreamTokenizer::ParseNumbers()
{
    for (Int32 i = '0'; i <= '9'; i++) {
        (*mTokenTypes)[i] |= TOKEN_DIGIT;
    }
    (*mTokenTypes)['.'] |= TOKEN_DIGIT;
    (*mTokenTypes)['-'] |= TOKEN_DIGIT;
    return NOERROR;
}

ECode StreamTokenizer::PushBack()
{
    mPushBackToken = TRUE;
    return NOERROR;
}

ECode StreamTokenizer::SetQuoteChar(
    /* [in] */ Int32 ch)
{
    if (ch >= 0 && ch < mTokenTypes->GetLength()) {
        (*mTokenTypes)[ch] = TOKEN_QUOTE;
    }
    return NOERROR;
}

ECode StreamTokenizer::ResetSyntax()
{
    for (Int32 i = 0; i < 256; i++) {
        (*mTokenTypes)[i] = 0;
    }
    return NOERROR;
}

ECode StreamTokenizer::SetSlashSlashComments(
    /* [in] */ Boolean flag)
{
    mSlashSlashComments = flag;
    return NOERROR;
}

ECode StreamTokenizer::SetSlashStarComments(
    /* [in] */ Boolean flag)
{
    mSlashStarComments = flag;
    return NOERROR;
}

ECode StreamTokenizer::ToString(
    /* [in] */ String* rev)
{
    // Values determined through experimentation
    AutoPtr<StringBuilder> result = new StringBuilder();
    result->Append("Token[");
    switch (mTtype) {
        case TT_EOF:
            result->Append("EOF");
            break;
        case TT_EOL:
            result->Append("EOL");
            break;
        case TT_NUMBER:
            result->Append("n=");
            result->Append(mNval);
            break;
        case TT_WORD:
            result->Append(mSval);
            break;
        default:
            if (mTtype == TT_UNKNOWN || (*mTokenTypes)[mTtype] == TOKEN_QUOTE) {
                result->Append(mSval);
            } else {
                result->AppendChar('\'');
                result->AppendChar( mTtype);
                result->AppendChar('\'');
            }
    }
    result->Append("], line ");
    result->Append(mLineNumber);
    result->ToString(rev);
    return NOERROR;
}

ECode StreamTokenizer::SetWhitespaceChars(
    /* [in] */ Int32 low,
    /* [in] */ Int32 hi)
{
    if (low < 0) {
        low = 0;
    }
    if (hi > mTokenTypes->GetLength()) {
        hi = mTokenTypes->GetLength() - 1;
    }
    for (Int32 i = low; i <= hi; i++) {
        (*mTokenTypes)[i] = TOKEN_WHITE;
    }
    return NOERROR;
}

ECode StreamTokenizer::SetWordChars(
    /* [in] */ Int32 low,
    /* [in] */ Int32 hi)
{
    if (low < 0) {
        low = 0;
    }
    if (hi > mTokenTypes->GetLength()) {
        hi = mTokenTypes->GetLength() - 1;
    }
    for (Int32 i = low; i <= hi; i++) {
        (*mTokenTypes)[i] |= TOKEN_WORD;
    }
    return NOERROR;
}

ECode StreamTokenizer::constructor()
{
    /*
     * Initialize the default state per specification. All byte values 'A'
     * through 'Z', 'a' through 'z', and '\u00A0' through '\u00FF' are
     * considered to be alphabetic.
     */
    SetWordChars('A', 'Z');
    SetWordChars('a', 'z');
    SetWordChars(160, 255);
    /**
     * All byte values '\u0000' through '\u0020' are considered to be white
     * space.
     */
    SetWhitespaceChars(0, 32);
    /**
     * '/' is a comment character. Single quote '\'' and double quote '"'
     * are string quote characters.
     */
    SetCommentChar('/');
    SetQuoteChar('"');
    SetQuoteChar('\'');
    /**
     * Numbers are parsed.
     */
    ParseNumbers();
    /**
     * Ends of lines are treated as white space, not as separate tokens.
     * C-style and C++-style comments are not recognized. These are the
     * defaults and are not needed in constructor.
     */
     return NOERROR;
}

ECode StreamTokenizer::Read(
    /* [out] */ Int32* rev)
{
    VALIDATE_NOT_NULL(rev)
    *rev = -1;

    // Call the read for the appropriate stream
    if (NULL == mInStream) {
        FAIL_RETURN(mInReader->Read(rev));
        return NOERROR;
    }
    FAIL_RETURN(mInStream->Read(rev));
    return NOERROR;
}

ECode StreamTokenizer::GetNval(
    /* [out] */ Double* nval)
{
    VALIDATE_NOT_NULL(nval)
    *nval = mNval;
    return NOERROR;
}

ECode StreamTokenizer::SetNval(
    /* [in] */ Double nval)
{
    mNval = nval;
    return NOERROR;
}

ECode StreamTokenizer::GetSval(
    /* [out] */ String* sval)
{
    VALIDATE_NOT_NULL(sval)
    *sval = mSval;
    return NOERROR;
}

ECode StreamTokenizer::SetSval(
    /* [in] */ const String& sval)
{
    mSval = sval;
    return NOERROR;
}

ECode StreamTokenizer::GetTtype(
    /* [out] */ Int32* ttype)
{
    VALIDATE_NOT_NULL(ttype)
    *ttype = mTtype;
    return NOERROR;
}

ECode StreamTokenizer::SetTtype(
    /* [in] */ Int32 ttype)
{
    mTtype = ttype;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
