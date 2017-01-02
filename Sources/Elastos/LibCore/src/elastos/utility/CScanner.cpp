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

#include "CScanner.h"
#include "Pattern.h"
#include "CLocale.h"
#include "Charset.h"
#include "IoUtils.h"
#include "CStringReader.h"
#include "CFileInputStream.h"
#include "CInputStreamReader.h"
#include "CString.h"
#include "CBigDecimal.h"
#include "CBigInteger.h"
#include "CByte.h"
#include "CDouble.h"
#include "CFloat.h"
#include "CInteger16.h"
#include "CInteger32.h"
#include "CInteger64.h"
#include "Character.h"
#include "CharBuffer.h"
#include "NumberFormat.h"
#include <Math.h>
#include <StringUtils.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::IInteger16;
using Elastos::Core::CInteger16;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::Character;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IStringReader;
using Elastos::IO::CStringReader;
using Elastos::IO::CharBuffer;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::Charset;
using Elastos::IO::IBuffer;
using Elastos::Math::CBigDecimal;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigInteger;
using Elastos::Text::INumberFormat;
using Elastos::Text::NumberFormat;
using Elastos::Text::IDecimalFormatSymbols;
using Elastos::Utility::CLocale;
using Elastos::Utility::Regex::Pattern;
using Libcore::IO::IoUtils;

namespace Elastos {
namespace Utility {

static AutoPtr<IPattern> InitPattern(
    /* [in] */ const String str,
    /* [in] */ Int32 flag = 0)
{
    AutoPtr<IPattern> p;
    Pattern::Compile(str, flag, (IPattern**)&p);
    return p;
}

const AutoPtr<IPattern> CScanner::DEFAULT_DELIMITER = InitPattern(String("\\p{javaWhitespace}+"));
const AutoPtr<IPattern> CScanner::BOOLEAN_PATTERN = InitPattern(String("true|false"), Pattern::CASE_INSENSITIVE);
const AutoPtr<IPattern> CScanner::LINE_TERMINATOR = InitPattern(String("\n|\r\n|\r|\u0085|\u2028|\u2029"));
const AutoPtr<IPattern> CScanner::MULTI_LINE_TERMINATOR = InitPattern(String("(\n|\r\n|\r|\u0085|\u2028|\u2029)+"));
const AutoPtr<IPattern> CScanner::LINE_PATTERN = InitPattern(String(".*(\n|\r\n|\r|\u0085|\u2028|\u2029)|.+(\n|\r\n|\r|\u0085|\u2028|\u2029)?"));
const AutoPtr<IPattern> CScanner::ANY_PATTERN = InitPattern(String("(?s).*"));

const Int32 CScanner::DEFAULT_RADIX;
const Int32 CScanner::DEFAULT_TRUNK_SIZE;

CAR_INTERFACE_IMPL_2(CScanner, Object, IScanner, IIterator)

CAR_OBJECT_IMPL(CScanner)

CScanner::CScanner()
    : mDelimiter(DEFAULT_DELIMITER)
    , mCurrentRadix(DEFAULT_RADIX)
    , mFindStartIndex(0)
    , mPreStartIndex(mFindStartIndex)
    , mBufferLength(0)
    , mClosed(FALSE)
    , mLastIOException(NOERROR)
    , mMatchSuccessful(FALSE)
    , mInputExhausted(FALSE)
    , mCachedNextIndex(-1)
    , mCachedIntegerPatternRadix(-1)
{
    mLocale = CLocale::GetDefault();
}

ECode CScanner::constructor(
    /* [in] */ IFile* filesrc)
{
    AutoPtr<ICharset> outset;
    Charset::DefaultCharset((ICharset**)&outset);
    String outname;
    outset->GetName(&outname);
    return this->constructor(filesrc, outname);
}

ECode CScanner::constructor(
    /* [in] */ IFile* filesrc,
    /* [in] */ const String& charsetName)
{
    if (filesrc == NULL) {
        // throw new NullPointerException("filesrc == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IFileInputStream> fis;
    FAIL_RETURN(CFileInputStream::New(filesrc, (IFileInputStream**)&fis));
    if (charsetName.IsNull()) {
        // throw new IllegalArgumentException("charsetName == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
    AutoPtr<IInputStreamReader> isr;
    ECode ec = CInputStreamReader::New((IInputStream::Probe(fis)), charsetName, (IInputStreamReader**)&isr);
    // } catch (UnsupportedEncodingException e) {
    if (ec != NOERROR) {
        IoUtils::CloseQuietly(ICloseable::Probe(fis));
        // throw new IllegalArgumentException(e.getMessage());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // }
    return Initialization(IReadable::Probe(isr));
}

ECode CScanner::constructor(
    /* [in] */ const String& strsrc)
{
    AutoPtr<IStringReader> sr;
    FAIL_RETURN(CStringReader::New(strsrc, (IStringReader**)&sr));
    return Initialization(IReadable::Probe(sr));
}

ECode CScanner::constructor(
    /* [in] */ IInputStream* inputsrc)
{
    AutoPtr<ICharset> outset;
    Charset::DefaultCharset((ICharset**)&outset);
    String outname;
    outset->GetName(&outname);
    return this->constructor(inputsrc, outname);
}

ECode CScanner::constructor(
    /* [in] */ IInputStream* inputsrc,
    /* [in] */ const String& charsetName)
{
    if (inputsrc == NULL) {
        // throw new NullPointerException("inputsrc == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
    AutoPtr<IInputStreamReader> isr;
    FAIL_RETURN(CInputStreamReader::New(inputsrc, charsetName, (IInputStreamReader**)&isr));
    // } catch (UnsupportedEncodingException e) {
        // throw new IllegalArgumentException(e.getMessage());
    // }
    return Initialization(IReadable::Probe(isr));
}

ECode CScanner::constructor(
    /* [in] */ IReadable* readsrc)
{
    if (readsrc == NULL) {
        // throw new NullPointerException("readsrc == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    return Initialization(readsrc);
}

ECode CScanner::constructor(
    /* [in] */ IReadableByteChannel* rbcsrc)
{
    AutoPtr<ICharset> outset;
    Charset::DefaultCharset((ICharset**)&outset);
    String outname;
    outset->GetName(&outname);
    return this->constructor(rbcsrc, outname);
}

ECode CScanner::constructor(
    /* [in] */ IReadableByteChannel* rbcsrc,
    /* [in] */ const String& charsetName)
{
    if (rbcsrc == NULL) {
        // throw new NullPointerException("rbcsrc == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (charsetName.IsNull()) {
        // throw new IllegalArgumentException("charsetName == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    assert(0 && "TODO");
    AutoPtr<IReadable> r;
    //Channels::NewReader(rbcsrc, charsetName, (IReadable**)&r);
    return Initialization(r);
}

ECode CScanner::HasNext(
    /* [out] */ Boolean* result)
{
    return HasNext(ANY_PATTERN, result);
}

ECode CScanner::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    String str;
    FAIL_RETURN(Next(ANY_PATTERN, &str));
    AutoPtr<ICharSequence> sq;
    CString::New(str, (ICharSequence**)&sq);
    *object = sq;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CScanner::Remove()
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CScanner::Close()
{
    if (mClosed) {
        return NOERROR;
    }
    if (ICloseable::Probe(mInput)) {
        // try {
        mLastIOException = ICloseable::Probe(mInput)->Close();
        // } catch (IOException e) {
            // mLastIOException = e;
        // }
    }
    mClosed = TRUE;
    return NOERROR;
}

ECode CScanner::Delimiter(
    /* [out] */ IPattern** outpat)
{
    VALIDATE_NOT_NULL(outpat)

    *outpat = mDelimiter;
    REFCOUNT_ADD(*outpat)
    return NOERROR;
}

ECode CScanner::FindInLine(
    /* [in] */ IPattern* pattern,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);

    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(CheckNotNull(pattern));
    Int32 horizonLineSeparator = 0;

    mMatcher->UsePattern(MULTI_LINE_TERMINATOR);
    mMatcher->Region(mFindStartIndex, mBufferLength);

    Boolean findComplete = FALSE;
    Int32 terminatorLength = 0;
    while (!findComplete) {
        if (mMatcher->Find(&findComplete), findComplete) {
            (IMatchResult::Probe(mMatcher))->Start(&horizonLineSeparator);
            (IMatchResult::Probe(mMatcher))->End(&terminatorLength);
            terminatorLength -= horizonLineSeparator;
            findComplete = TRUE;
        }
        else {
            if (!mInputExhausted) {
                ReadMore();
                ResetMatcher();
            }
            else {
                horizonLineSeparator = mBufferLength;
                findComplete = TRUE;
            }
        }
    }

    mMatcher->UsePattern(pattern);

    /*
     * TODO The following 2 statements are used to deal with regex's bug.
     * java.util.regex.Matcher.region(Int32 start, Int32 end) implementation
     * does not have any effects when called. They will be removed once the
     * bug is fixed.
     */
    Int32 oldLimit = 0;
    (IBuffer::Probe(mBuffer))->GetLimit(&oldLimit);
    // Considering the look ahead feature, the line terminator should be involved as RI
    (IBuffer::Probe(mBuffer))->SetLimit(horizonLineSeparator + terminatorLength);
    // ========== To deal with regex bug ====================

    // Considering the look ahead feature, the line terminator should be involved as RI
    mMatcher->Region(mFindStartIndex, horizonLineSeparator + terminatorLength);
    if (mMatcher->Find(&findComplete), findComplete) {
        // The scanner advances past the input that matched
        (IMatchResult::Probe(mMatcher))->End(&mFindStartIndex);
        // If the matched pattern is immediately followed by line
        // terminator.
        Int32 endlength = 0;
        if (horizonLineSeparator == ((IMatchResult::Probe(mMatcher))->End(&endlength), endlength)) {
            mFindStartIndex += terminatorLength;
        }
        // the line terminator itself should not be a part of
        // the match result according to the Spec

        if (horizonLineSeparator != mBufferLength
                && (horizonLineSeparator + terminatorLength == ((IMatchResult::Probe(mMatcher))->End(&endlength), endlength))) {
            // ========== To deal with regex bug ====================
            (IBuffer::Probe(mBuffer))->SetLimit(oldLimit);
            // ========== To deal with regex bug ====================

            mMatchSuccessful = FALSE;
            *str = String(NULL);
            return NOERROR;
        }
        mMatchSuccessful = TRUE;

        // ========== To deal with regex bug ====================
        (IBuffer::Probe(mBuffer))->SetLimit(oldLimit);
        // ========== To deal with regex bug ====================

        return (IMatchResult::Probe(mMatcher))->Group(str);
    }

    // ========== To deal with regex bug ====================
    (IBuffer::Probe(mBuffer))->SetLimit(oldLimit);
    // ========== To deal with regex bug ====================

    mMatchSuccessful = FALSE;
    *str = String(NULL);
    return NOERROR;
}

ECode CScanner::FindInLine(
    /* [in] */ const String& pattern,
    /* [out] */ String* str)
{
    AutoPtr<IPattern> outpat;
    FAIL_RETURN(Pattern::Compile(pattern, (IPattern**)&outpat));
    return FindInLine(outpat, str);
}

ECode CScanner::FindWithinHorizon(
    /* [in] */ IPattern* pattern,
    /* [in] */ Int32 horizon,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);

    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(CheckNotNull(pattern));
    if (horizon < 0) {
        // throw new IllegalArgumentException("horizon < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMatcher->UsePattern(pattern);

    using Elastos::Core::Math;
    String result;
    Int32 horizonEndIndex = (horizon == 0) ? Math::INT32_MAX_VALUE : mFindStartIndex + horizon;
    Int32 findEndIndex = 0;
    while (TRUE) {
        // If horizon > 0, then search up to
        // min( mBufferLength, mFindStartIndex + horizon).
        // Otherwise search until readable is exhausted.
        findEndIndex = Elastos::Core::Math::Min(horizonEndIndex, mBufferLength);
        // If horizon == 0, consider horizon as always outside buffer.
        Boolean isHorizonInBuffer = (horizonEndIndex <= mBufferLength);
        // First, try to find pattern within buffer. If pattern can not be
        // found in buffer, then expand the buffer and try again,
        // util horizonEndIndex is exceeded or no more input left.
        mMatcher->Region(mFindStartIndex, findEndIndex);
        Boolean isflag, hitEnd;
        if (mMatcher->Find(&isflag), isflag) {
            if (isHorizonInBuffer || mInputExhausted
                || (horizon == 0 && (mMatcher->HitEnd(&hitEnd), !hitEnd))) {
                (IMatchResult::Probe(mMatcher))->Group(&result);
                break;
            }
        }
        else {
            // Pattern is not found in buffer while horizonEndIndex is
            // within buffer, or input is exhausted. Under this situation,
            // it can be judged that find fails.
            if (isHorizonInBuffer || mInputExhausted) {
                break;
            }
        }

        // Expand buffer and reset matcher if needed.
        if (!mInputExhausted) {
            ReadMore();
            ResetMatcher();
        }
    }
    if (!result.IsNull()) {
        (IMatchResult::Probe(mMatcher))->End(&mFindStartIndex);
        mMatchSuccessful = TRUE;
    }
    else {
        mMatchSuccessful = FALSE;
    }
    *str = result;
    return NOERROR;
}

ECode CScanner::FindWithinHorizon(
    /* [in] */ const String& pattern,
    /* [in] */ Int32 horizon,
    /* [out] */ String* str)
{
    AutoPtr<IPattern> outpat;
    FAIL_RETURN(Pattern::Compile(pattern, (IPattern**)&outpat));
    return FindWithinHorizon(outpat, horizon, str);
}

ECode CScanner::HasNext(
    /* [in] */ IPattern* pattern,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(CheckNotNull(pattern));
    mMatchSuccessful = FALSE;
    PrepareForScan();
    // if the next token exists, set the match region, otherwise return
    // FALSE
    if (!SetTokenRegion()) {
        RecoverPreviousStatus();
        *value = FALSE;
        return NOERROR;
    }
    mMatcher->UsePattern(pattern);
    Boolean hasNext = FALSE;
    Boolean isHasNext = FALSE;
    // check whether next token matches the specified pattern
    if (mMatcher->Matches(&isHasNext), isHasNext) {
        mCachedNextIndex = mFindStartIndex;
        mMatchSuccessful = TRUE;
        hasNext = TRUE;
    }
    RecoverPreviousStatus();
    *value = hasNext;
    return NOERROR;
}

ECode CScanner::HasNext(
    /* [in] */ const String& pattern,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    AutoPtr<IPattern> outpat;
    FAIL_RETURN(Pattern::Compile(pattern, (IPattern**)&outpat));
    return HasNext(outpat, value);
}

ECode CScanner::HasNextBigDecimal(
    /* [out] */ Boolean* value)
{
    AutoPtr<IPattern> floatPattern = GetFloatPattern();
    Boolean isBigDecimalValue = FALSE;
    Boolean isHasNext = FALSE;
    if (HasNext(floatPattern, &isHasNext), isHasNext) {
        String floatString;
        (IMatchResult::Probe(mMatcher))->Group(&floatString);
        floatString = RemoveLocaleInfoFromFloat(floatString);
        // try {
        AutoPtr<IBigDecimal> outbd;
        ECode ec = CBigDecimal::New(floatString, (IBigDecimal**)&outbd);
        // } catch (NumberFormatException e) {
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            mMatchSuccessful = FALSE;
        }
        // }
        if (SUCCEEDED(ec)) {
            isBigDecimalValue = TRUE;
            mCachedNextValue = outbd;
        }

    }
    *value = isBigDecimalValue;
    return NOERROR;
}

ECode CScanner::HasNextBigInteger(
    /* [out] */ Boolean* value)
{
    return HasNextBigInteger(mCurrentRadix, value);
}

ECode CScanner::HasNextBigInteger(
    /* [in] */ Int32 radix,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    Boolean isBigIntegerValue = FALSE;
    Boolean isHasNext = FALSE;
    if (HasNext(integerPattern, &isHasNext), isHasNext) {
        String intString;
        (IMatchResult::Probe(mMatcher))->Group(&intString);
        intString = RemoveLocaleInfo(intString, INT);
        // try {
        AutoPtr<IBigInteger> outbi;
        ECode ec = CBigInteger::New(intString, radix, (IBigInteger**)&outbi);
        // } catch (NumberFormatException e) {
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            mMatchSuccessful = FALSE;
        }
        // }
        if (SUCCEEDED(ec)) {
            isBigIntegerValue = TRUE;
            mCachedNextValue = outbi;
        }

    }
    *value = isBigIntegerValue;
    return NOERROR;
}

ECode CScanner::HasNextBoolean(
    /* [out] */ Boolean* value)
{
    return HasNext(BOOLEAN_PATTERN, value);
}

ECode CScanner::HasNextByte(
    /* [out] */ Boolean* value)
{
    return HasNextByte(mCurrentRadix, value);
}

ECode CScanner::HasNextByte(
    /* [in] */ Int32 radix,
    /* [out] */ Boolean* value)
{
    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    Boolean isByteValue = FALSE;
    Boolean isHasNext = FALSE;
    if (HasNext(integerPattern, &isHasNext), isHasNext) {
        String intString;
        (IMatchResult::Probe(mMatcher))->Group(&intString);
        intString = RemoveLocaleInfo(intString, INT);
        // try {
        Byte outbyte;
        ECode ec = StringUtils::Parse(intString, radix, &outbyte);
        if (SUCCEEDED(ec)) {
            AutoPtr<IByte> inbyte;
            CByte::New((Byte)outbyte, (IByte**)&inbyte);
            mCachedNextValue = inbyte;
            isByteValue = TRUE;
        }
        // } catch (NumberFormatException e) {
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            mMatchSuccessful = FALSE;
        }
        // }
    }
    *value = isByteValue;
    return NOERROR;
}

ECode CScanner::HasNextDouble(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IPattern> floatPattern = GetFloatPattern();
    Boolean isDoubleValue = FALSE;
    Boolean isHasNext = FALSE;
    if (HasNext(floatPattern, &isHasNext), isHasNext) {
        String floatString;
        (IMatchResult::Probe(mMatcher))->Group(&floatString);
        floatString = RemoveLocaleInfoFromFloat(floatString);
        // try {
        Double outdouble;
        ECode ec = StringUtils::Parse(floatString, &outdouble);
        if (SUCCEEDED(ec)) {
            AutoPtr<IDouble> indouble;
            CDouble::New(outdouble, (IDouble**)&indouble);
            mCachedNextValue = indouble;
            isDoubleValue = TRUE;
        }
        // } catch (NumberFormatException e) {
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            mMatchSuccessful = FALSE;
        }
        // }
    }
    *value = isDoubleValue;
    return NOERROR;
}

ECode CScanner::HasNextFloat(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IPattern> floatPattern = GetFloatPattern();
    Boolean isFloatValue = FALSE;
    Boolean isHasNext = FALSE;
    if (HasNext(floatPattern, &isHasNext), isHasNext) {
        String floatString;
        (IMatchResult::Probe(mMatcher))->Group(&floatString);
        floatString = RemoveLocaleInfoFromFloat(floatString);
        // try {
        Float outfloat;
        ECode ec = StringUtils::Parse(floatString, &outfloat);
        if (SUCCEEDED(ec)) {
            AutoPtr<IFloat> infloat;
            CFloat::New(outfloat, (IFloat**)&infloat);

            mCachedNextValue = infloat;
            isFloatValue = TRUE;
        }
        // } catch (NumberFormatException e) {
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            mMatchSuccessful = FALSE;
        }
        // }
    }
    *value = isFloatValue;
    return NOERROR;
}

ECode CScanner::HasNextInt32(
    /* [out] */ Boolean* value)
{
    return HasNextInt32(mCurrentRadix, value);
}

ECode CScanner::HasNextInt32(
    /* [in] */ Int32 radix,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    Boolean isIntValue = FALSE;
    Boolean isHasNext = FALSE;
    if (HasNext(integerPattern, &isHasNext), isHasNext) {
        String intString;
        (IMatchResult::Probe(mMatcher))->Group(&intString);
        intString = RemoveLocaleInfo(intString, INT);
        // try {
        Int32 outint;
        ECode ec = StringUtils::Parse(intString, radix, &outint);
        if (SUCCEEDED(ec)) {
            AutoPtr<IInteger32> inint;
            CInteger32::New(outint, (IInteger32**)&inint);
            mCachedNextValue = inint;
            isIntValue = TRUE;
        }
        // } catch (NumberFormatException e) {
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            mMatchSuccessful = FALSE;
        }
        // }
    }
    *value = isIntValue;
    return NOERROR;
}

ECode CScanner::HasNextLine(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    PrepareForScan();
    String result;
    FAIL_RETURN(FindWithinHorizon(LINE_PATTERN, 0, &result));
    RecoverPreviousStatus();
    *value = !result.IsNull();
    return NOERROR;
}

ECode CScanner::HasNextInt64(
    /* [out] */ Boolean* value)
{
    return HasNextInt64(mCurrentRadix, value);
}

ECode CScanner::HasNextInt64(
    /* [in] */ Int32 radix,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    Boolean isLongValue = FALSE;
    Boolean isHasNext = FALSE;
    if (HasNext(integerPattern, &isHasNext), isHasNext) {
        String intString;
        (IMatchResult::Probe(mMatcher))->Group(&intString);
        intString = RemoveLocaleInfo(intString, INT);
        // try {
        Int64 outint;
        ECode ec = StringUtils::Parse(intString, radix, &outint);
        if (SUCCEEDED(ec)) {
            AutoPtr<IInteger64> inint;
            CInteger64::New(outint, (IInteger64**)&inint);
            mCachedNextValue = inint;
            isLongValue = TRUE;
        }
        // } catch (NumberFormatException e) {
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            mMatchSuccessful = FALSE;
        }
        // }
    }
    *value = isLongValue;
    return NOERROR;
}

ECode CScanner::HasNextInt16(
    /* [out] */ Boolean* value)
{
    return HasNextInt16(mCurrentRadix, value);
}

ECode CScanner::HasNextInt16(
    /* [in] */ Int32 radix,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    Boolean isShortValue = FALSE;
    Boolean isHasNext = FALSE;
    if (HasNext(integerPattern, &isHasNext), isHasNext) {
        String intString;
        (IMatchResult::Probe(mMatcher))->Group(&intString);
        intString = RemoveLocaleInfo(intString, INT);
        // try {
        Int16 outint;
        ECode ec = StringUtils::Parse(intString, radix, &outint);
        if (SUCCEEDED(ec)) {
            AutoPtr<IInteger16> inint;
            CInteger16::New(outint, (IInteger16**)&inint);
            mCachedNextValue = inint;
            isShortValue = TRUE;
        }
        // } catch (NumberFormatException e) {
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            mMatchSuccessful = FALSE;
        }
        // }
    }
    *value = isShortValue;
    return NOERROR;
}

ECode CScanner::IoException()
{
    return mLastIOException;
}

ECode CScanner::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)

    *locale = mLocale;
    REFCOUNT_ADD(*locale)
    return NOERROR;
}

void CScanner::SetLocale(
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    mDecimalFormat = NULL;
    mCachedFloatPattern = NULL;
    mCachedIntegerPatternRadix = -1;
    mCachedIntegerPattern = NULL;
}

ECode CScanner::GetMatch(
    /* [out] */ IMatchResult** outmatch)
{
    VALIDATE_NOT_NULL(outmatch)
    *outmatch = NULL;

    if (!mMatchSuccessful) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return mMatcher->ToMatchResult(outmatch);
}

ECode CScanner::Next(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Next(ANY_PATTERN, str);
}

ECode CScanner::Next(
    /* [in] */ IPattern* pattern,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);

    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(CheckNotNull(pattern));
    mMatchSuccessful = FALSE;
    PrepareForScan();
    if (!SetTokenRegion()) {
        RecoverPreviousStatus();
        // if setting match region fails
        // throw new NoSuchElementException();
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    mMatcher->UsePattern(pattern);
    Boolean isflag = FALSE;
    if (!(mMatcher->Matches(&isflag), isflag)) {
        RecoverPreviousStatus();
        // throw new InputMismatchException();
        return E_INPUT_MISMATCH_EXCEPTION;
    }
    mMatchSuccessful = TRUE;
    return (IMatchResult::Probe(mMatcher))->Group(str);
}

ECode CScanner::Next(
    /* [in] */ const String& pattern,
    /* [out] */ String* str)
{
    AutoPtr<IPattern> outpat;
    FAIL_RETURN(Pattern::Compile(pattern, (IPattern**)&outpat));
    return Next(outpat, str);
}

ECode CScanner::NextBigDecimal(
    /* [out] */ IBigDecimal** outbig)
{
    VALIDATE_NOT_NULL(outbig)
    *outbig = NULL;

    FAIL_RETURN(CheckOpen());
    AutoPtr<IInterface> obj = mCachedNextValue;
    mCachedNextValue = NULL;
    if (IBigDecimal::Probe(obj)) {
        mFindStartIndex = mCachedNextIndex;
        *outbig = IBigDecimal::Probe(obj);
        REFCOUNT_ADD(*outbig);
        return NOERROR;
    }
    AutoPtr<IPattern> floatPattern = GetFloatPattern();
    String floatString;
    FAIL_RETURN(Next(floatPattern, &floatString));
    floatString = RemoveLocaleInfoFromFloat(floatString);
    AutoPtr<IBigDecimal> bigDecimalValue;
    // try {
    ECode ec = CBigDecimal::New(floatString, (IBigDecimal**)&bigDecimalValue);
    // } catch (NumberFormatException e) {
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        mMatchSuccessful = FALSE;
        RecoverPreviousStatus();
        // throw new InputMismatchException();
        return E_INPUT_MISMATCH_EXCEPTION;
    }
    // }
    *outbig = bigDecimalValue;
    REFCOUNT_ADD(*outbig)
    return NOERROR;
}

ECode CScanner::NextBigInteger(
    /* [out] */ IBigInteger** outbig)
{
    return NextBigInteger(mCurrentRadix, outbig);
}

ECode CScanner::NextBigInteger(
    /* [in] */ Int32 radix,
    /* [out] */ IBigInteger** outbig)
{
    VALIDATE_NOT_NULL(outbig)
    *outbig = NULL;

    FAIL_RETURN(CheckOpen());
    AutoPtr<IInterface> obj = mCachedNextValue;
    mCachedNextValue = NULL;
    if (IBigInteger::Probe(obj)) {
        mFindStartIndex = mCachedNextIndex;
        *outbig = IBigInteger::Probe(obj);
        REFCOUNT_ADD(*outbig);
        return NOERROR;
    }
    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    String intString;
    FAIL_RETURN(Next(integerPattern, &intString));
    intString = RemoveLocaleInfo(intString, INT);
    AutoPtr<IBigInteger> bigIntegerValue;
    // try {
    ECode ec = CBigInteger::New(intString, radix, (IBigInteger**)&bigIntegerValue);
    // } catch (NumberFormatException e) {
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        mMatchSuccessful = FALSE;
        RecoverPreviousStatus();
        // throw new InputMismatchException();
        return E_INPUT_MISMATCH_EXCEPTION;
    }
    // }
    *outbig = bigIntegerValue;
    REFCOUNT_ADD(*outbig)
    return NOERROR;
}

ECode CScanner::NextBoolean(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    String outstr;
    FAIL_RETURN(Next(BOOLEAN_PATTERN, &outstr));
    *value = outstr.EqualsIgnoreCase("true");
    return NOERROR;
}

ECode CScanner::NextByte(
    /* [out] */ Byte* value)
{
    return NextByte(mCurrentRadix, value);
}

ECode CScanner::NextByte(
    /* [in] */ Int32 radix,
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    FAIL_RETURN(CheckOpen());
    AutoPtr<IInterface> obj = mCachedNextValue;
    mCachedNextValue = NULL;
    if (IByte::Probe(obj)) {
        mFindStartIndex = mCachedNextIndex;
        return IByte::Probe(obj)->GetValue(value);
    }
    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    String intString;
    FAIL_RETURN(Next(integerPattern, &intString));
    intString = RemoveLocaleInfo(intString, INT);
    // try {
    Byte byteValue = 0;
    ECode ec = StringUtils::Parse(intString, radix, &byteValue);
    // } catch (NumberFormatException e) {
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        mMatchSuccessful = FALSE;
        RecoverPreviousStatus();
        // throw new InputMismatchException();
        return E_INPUT_MISMATCH_EXCEPTION;
    }
    // }
    *value = (Byte)byteValue;
    return NOERROR;
}

ECode CScanner::NextDouble(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    FAIL_RETURN(CheckOpen());
    AutoPtr<IInterface> obj = mCachedNextValue;
    mCachedNextValue = NULL;
    if (IDouble::Probe(obj)) {
        mFindStartIndex = mCachedNextIndex;
        return IDouble::Probe(obj)->GetValue(value);
    }
    AutoPtr<IPattern> floatPattern = GetFloatPattern();
    String floatString;
    FAIL_RETURN(Next(floatPattern, &floatString));
    floatString = RemoveLocaleInfoFromFloat(floatString);
    Double doubleValue = 0;
    // try {
    ECode ec = StringUtils::Parse(floatString, &doubleValue);
    // } catch (NumberFormatException e) {
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        mMatchSuccessful = FALSE;
        RecoverPreviousStatus();
        // throw new InputMismatchException();
        return E_INPUT_MISMATCH_EXCEPTION;
    }
    // }
    *value = doubleValue;
    return NOERROR;
}

ECode CScanner::NextFloat(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    FAIL_RETURN(CheckOpen());
    AutoPtr<IInterface> obj = mCachedNextValue;
    mCachedNextValue = NULL;
    if (IFloat::Probe(obj)) {
        mFindStartIndex = mCachedNextIndex;
        return IFloat::Probe(obj)->GetValue(value);
    }
    AutoPtr<IPattern> floatPattern = GetFloatPattern();
    String floatString;
    FAIL_RETURN(Next(floatPattern, &floatString));
    floatString = RemoveLocaleInfoFromFloat(floatString);
    Float floatValue = 0;
    // try {
    ECode ec = StringUtils::Parse(floatString, &floatValue);
    // } catch (NumberFormatException e) {
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        mMatchSuccessful = FALSE;
        RecoverPreviousStatus();
        // throw new InputMismatchException();
        return E_INPUT_MISMATCH_EXCEPTION;
    }
    // }
    *value = floatValue;
    return NOERROR;
}

ECode CScanner::NextInt32(
    /* [out] */ Int32* value)
{
    return NextInt32(mCurrentRadix, value);
}

ECode CScanner::NextInt32(
    /* [in] */ Int32 radix,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    FAIL_RETURN(CheckOpen());
    AutoPtr<IInterface> obj = mCachedNextValue;
    mCachedNextValue = NULL;
    if (IInteger32::Probe(obj)) {
        mFindStartIndex = mCachedNextIndex;
        return IInteger32::Probe(obj)->GetValue(value);
    }
    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    String intString;
    FAIL_RETURN(Next(integerPattern, &intString));
    intString = RemoveLocaleInfo(intString, INT);
    Int32 intValue = 0;
    // try {
    ECode ec = StringUtils::Parse(intString, radix, &intValue);
    // } catch (NumberFormatException e) {
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        mMatchSuccessful = FALSE;
        RecoverPreviousStatus();
        // throw new InputMismatchException();
        return E_INPUT_MISMATCH_EXCEPTION;
    }
    // }
    *value = intValue;
    return NOERROR;
}

ECode CScanner::NextLine(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String(NULL);

    FAIL_RETURN(CheckOpen());

    mMatcher->UsePattern(LINE_PATTERN);
    mMatcher->Region(mFindStartIndex, mBufferLength);
    String result;
    Boolean isflag = FALSE;
    while (TRUE) {
        if (mMatcher->Find(&isflag), isflag) {
            Int32 endvalue = 0;
            if (mInputExhausted || ((IMatchResult::Probe(mMatcher))->End(&endvalue), endvalue) != mBufferLength
                    || mBufferLength < ((IBuffer::Probe(mBuffer))->GetCapacity(&endvalue), endvalue)) {
                mMatchSuccessful = TRUE;
                (IMatchResult::Probe(mMatcher))->End(&mFindStartIndex);
                (IMatchResult::Probe(mMatcher))->Group(&result);
                break;
            }
        }
        else {
            if (mInputExhausted) {
                mMatchSuccessful = FALSE;
                // throw new NoSuchElementException();
                return E_NO_SUCH_ELEMENT_EXCEPTION;
            }
        }
        if (!mInputExhausted) {
            ReadMore();
            ResetMatcher();
        }
    }
    // Find text without line terminator here.
    if (result != NULL) {
        AutoPtr<IMatcher> terminatorMatcher;
        LINE_TERMINATOR->Matcher(result, (IMatcher**)&terminatorMatcher);
        if (terminatorMatcher->Find(&isflag), isflag) {
            Int32 startvalue = 0;
            (IMatchResult::Probe(terminatorMatcher))->Start(&startvalue);
            result = result.Substring(0, startvalue);
        }
    }
    *str = result;
    return NOERROR;
}

ECode CScanner::NextInt64(
    /* [out] */ Int64* value)
{
    return NextInt64(mCurrentRadix, value);
}

ECode CScanner::NextInt64(
    /* [in] */ Int32 radix,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    FAIL_RETURN(CheckOpen());
    AutoPtr<IInterface> obj = mCachedNextValue;
    mCachedNextValue = NULL;
    if (IInteger64::Probe(obj)) {
        mFindStartIndex = mCachedNextIndex;
        return IInteger64::Probe(obj)->GetValue(value);
    }
    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    String intString;
    FAIL_RETURN(Next(integerPattern, &intString));
    intString = RemoveLocaleInfo(intString, INT);
    Int64 longValue = 0;
    // try {
    ECode ec = StringUtils::Parse(intString, radix, &longValue);
    // } catch (NumberFormatException e) {
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        mMatchSuccessful = FALSE;
        RecoverPreviousStatus();
        // throw new InputMismatchException();
        return E_INPUT_MISMATCH_EXCEPTION;
    }
    // }
    *value = longValue;
    return NOERROR;
}

ECode CScanner::NextInt16(
    /* [out] */ Int16* value)
{
    return NextInt16(mCurrentRadix, value);
}

ECode CScanner::NextInt16(
    /* [in] */ Int32 radix,
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value)

    FAIL_RETURN(CheckOpen());
    AutoPtr<IInterface> obj = mCachedNextValue;
    mCachedNextValue = NULL;
    if (IInteger16::Probe(obj)) {
        mFindStartIndex = mCachedNextIndex;
        return IInteger16::Probe(obj)->GetValue(value);
    }
    AutoPtr<IPattern> integerPattern;
    FAIL_RETURN(GetIntegerPattern(radix, (IPattern**)&integerPattern));
    String intString;
    FAIL_RETURN(Next(integerPattern, &intString));
    intString = RemoveLocaleInfo(intString, INT);
    Int16 shortValue = 0;
    // try {
    ECode ec = StringUtils::Parse(intString, radix, &shortValue);
    // } catch (NumberFormatException e) {
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        mMatchSuccessful = FALSE;
        RecoverPreviousStatus();
        // throw new InputMismatchException();
        return E_INPUT_MISMATCH_EXCEPTION;
    }
    // }
    *value = shortValue;
    return NOERROR;
}

ECode CScanner::Radix(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mCurrentRadix;
    return NOERROR;
}

ECode CScanner::Skip(
    /* [in] */ IPattern* pattern,
    /* [out] */ IScanner** outscan)
{
    VALIDATE_NOT_NULL(outscan)

    FAIL_RETURN(CheckOpen());
    FAIL_RETURN(CheckNotNull(pattern));
    mMatcher->UsePattern(pattern);
    mMatcher->Region(mFindStartIndex, mBufferLength);
    while (TRUE) {
        Boolean isflag = FALSE;
        if (mMatcher->LookingAt(&isflag), isflag) {
            Int32 endvalue = 0;
            (IMatchResult::Probe(mMatcher))->End(&endvalue);
            Boolean matchInBuffer = endvalue < mBufferLength
                    || (endvalue == mBufferLength && mInputExhausted);
            if (matchInBuffer) {
                mMatchSuccessful = TRUE;
                (IMatchResult::Probe(mMatcher))->End(&mFindStartIndex);
                break;
            }
        }
        else {
            if (mInputExhausted) {
                mMatchSuccessful = FALSE;
                // throw new NoSuchElementException();
                return E_NO_SUCH_ELEMENT_EXCEPTION;
            }
        }
        if (!mInputExhausted) {
            ReadMore();
            ResetMatcher();
        }
    }
    *outscan = (IScanner*) this->Probe(EIID_IScanner);
    REFCOUNT_ADD(*outscan)
    return NOERROR;
}

ECode CScanner::Skip(
    /* [in] */ const String& pattern,
    /* [out] */ IScanner** outscan)
{
    AutoPtr<IPattern> outpat;
    FAIL_RETURN(Pattern::Compile(pattern, (IPattern**)&outpat));
    return Skip(outpat, outscan);
}

ECode CScanner::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    String name("CScanner");
    String strPattern;
    IObject::Probe(mDelimiter)->ToString(&strPattern);
    *str = name + "[mDelimiter=" + strPattern +
             ",mFindStartIndex=" + StringUtils::ToString(mFindStartIndex, 10) +
             ",mMatchSuccessful=" + StringUtils::BooleanToString(mMatchSuccessful) +
             ",mClosed=" + StringUtils::BooleanToString(mClosed) +
             "]";
    return NOERROR;
}

ECode CScanner::UseDelimiter(
    /* [in] */ IPattern* pattern,
    /* [out] */ IScanner** outscan)
{
    VALIDATE_NOT_NULL(outscan)

    mDelimiter = pattern;
    *outscan = (IScanner*) this->Probe(EIID_IScanner);
    REFCOUNT_ADD(*outscan)
    return NOERROR;
}

ECode CScanner::UseDelimiter(
    /* [in] */ const String& pattern,
    /* [out] */ IScanner** outscan)
{
    AutoPtr<IPattern> outpat;
    FAIL_RETURN(Pattern::Compile(pattern, (IPattern**)&outpat));
    return UseDelimiter(outpat, outscan);
}

ECode CScanner::UseLocale(
    /* [in] */ ILocale* l,
    /* [out] */ IScanner** outscan)
{
    VALIDATE_NOT_NULL(outscan)

    if (l == NULL) {
        // throw new NullPointerException("l == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    SetLocale(l);
    *outscan = (IScanner*) this->Probe(EIID_IScanner);
    REFCOUNT_ADD(*outscan)
    return NOERROR;
}

ECode CScanner::UseRadix(
    /* [in] */ Int32 radix,
    /* [out] */ IScanner** outscan)
{
    VALIDATE_NOT_NULL(outscan)

    FAIL_RETURN(CheckRadix(radix));
    mCurrentRadix = radix;
    *outscan = (IScanner*) this->Probe(EIID_IScanner);
    REFCOUNT_ADD(*outscan)
    return NOERROR;
}

ECode CScanner::Reset()
{
    mDelimiter = DEFAULT_DELIMITER;
    SetLocale(CLocale::GetDefault());
    mCurrentRadix = DEFAULT_RADIX;
    return NOERROR;
}

ECode CScanner::CheckRadix(
    /* [in] */ Int32 radix)
{
    if (radix < Character::MIN_RADIX || radix > Character::MAX_RADIX) {
        // throw new IllegalArgumentException("Invalid radix: " + radix);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CScanner::Initialization(
    /* [in] */ IReadable* reader)
{
    mInput = reader;
    FAIL_RETURN(CharBuffer::Allocate(DEFAULT_TRUNK_SIZE, (ICharBuffer**)&mBuffer));

    mDelimiter->Matcher(String(""), (IMatcher**)&mMatcher);
    mMatcher->UseTransparentBounds(true);
    return mMatcher->UseAnchoringBounds(false);
}

ECode CScanner::CheckOpen()
{
    if (mClosed) {
        // throw new IllegalStateException();
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CScanner::CheckNotNull(
    /* [in] */ IPattern* pattern)
{
    if (pattern == NULL) {
        // throw new NullPointerException("pattern == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

ECode CScanner::ResetMatcher()
{
    mMatcher->Reset(ICharSequence::Probe(mBuffer));
    return mMatcher->Region(mFindStartIndex, mBufferLength);
}

ECode CScanner::PrepareForScan()
{
    // Compacting the buffer recovers space taken by already processed characters. This does not
    // prevent the buffer growing in all situations but keeps the buffer small when delimiters
    // exist regularly.
    Int32 cap;
    IBuffer::Probe(mBuffer)->GetCapacity(&cap);
    if (mFindStartIndex >= cap / 2) {
        // When over half the buffer is filled with characters no longer being considered by the
        // scanner we take the cost of compacting the buffer.

        // Move all characters from [findStartIndex, findStartIndex + remaining()) to
        // [0, remaining()).
        Int32 position;
        IBuffer::Probe(mBuffer)->GetPosition(&position);
        Int32 oldPosition = position;
        IBuffer::Probe(mBuffer)->SetPosition(mFindStartIndex);
        mBuffer->Compact();
        IBuffer::Probe(mBuffer)->SetPosition(oldPosition);

        // Update Scanner state to reflect the new buffer state.
        mBufferLength -= mFindStartIndex;
        mFindStartIndex = 0;
        mPreStartIndex = -1;

        // The matcher must also be informed that the buffer has changed because it operates on
        // a String copy.
        ResetMatcher();
    }

    // Save the matcher's last find position so it can be returned to if the next token cannot
    // be parsed.
    mPreStartIndex = mFindStartIndex;
    return NOERROR;
}

ECode CScanner::RecoverPreviousStatus()
{
    mFindStartIndex = mPreStartIndex;
    return NOERROR;
}

ECode CScanner::GetIntegerPattern(
    /* [in] */ Int32 radix,
    /* [out] */ IPattern** outpat)
{
    VALIDATE_NOT_NULL(outpat)
    *outpat = NULL;
    FAIL_RETURN(CheckRadix(radix));

    if (mDecimalFormat == NULL) {
        AutoPtr<INumberFormat> nf;
        NumberFormat::GetInstance(mLocale, (INumberFormat**)&nf);
        mDecimalFormat = IDecimalFormat::Probe(nf);
    }

    if (mCachedIntegerPatternRadix == radix) {
        *outpat = mCachedIntegerPattern;
        REFCOUNT_ADD(*outpat);
        return NOERROR;
    }

    String digits = String("0123456789abcdefghijklmnopqrstuvwxyz");
    String ASCIIDigit = digits.Substring(0, radix);
    String nonZeroASCIIDigit = digits.Substring(1, radix);

    StringBuilder digit("((?i)[");
    digit.Append(ASCIIDigit);
    digit.Append("]|\\p{javaDigit})");
    StringBuilder nonZeroDigit("((?i)[");
    nonZeroDigit.Append(nonZeroASCIIDigit);
    nonZeroDigit.Append("]|([\\p{javaDigit}&&[^0]]))");
    AutoPtr<StringBuilder> numeral = GetNumeral(&digit, &nonZeroDigit);

    StringBuilder regex("(([-+]?(");
    regex.Append(numeral->ToString());
    regex.Append(")))|(");
    regex.Append(AddPositiveSign(numeral)->ToString());
    regex.Append(")|(");
    regex.Append(AddNegativeSign(numeral)->ToString());
    regex.Append(")");

    mCachedIntegerPatternRadix = radix;
    mCachedIntegerPattern = NULL;
    Pattern::Compile(regex.ToString(), (IPattern**)&mCachedIntegerPattern);
    *outpat = mCachedIntegerPattern;
    REFCOUNT_ADD(*outpat);
    return NOERROR;
}

AutoPtr<IPattern> CScanner::GetFloatPattern()
{
    if (mDecimalFormat == NULL) {
        AutoPtr<INumberFormat> nf;
        NumberFormat::GetInstance(mLocale, (INumberFormat**)&nf);
        mDecimalFormat = IDecimalFormat::Probe(nf);
    }

    if (mCachedIntegerPattern != NULL) {
        return mCachedIntegerPattern;
    }

    AutoPtr<IDecimalFormatSymbols> dfs;
    mDecimalFormat->GetDecimalFormatSymbols((IDecimalFormatSymbols**)&dfs);

    StringBuilder digit("([0-9]|(\\p{javaDigit}))");
    StringBuilder nonZeroDigit("[\\p{javaDigit}&&[^0]]");
    AutoPtr<StringBuilder> numeral = GetNumeral(&digit, &nonZeroDigit);


    Char32 outchar;
    dfs->GetDecimalSeparator(&outchar);
    StringBuilder temp("\\");
    temp.AppendChar(outchar);
    String decimalSeparator = temp.ToString();

    StringBuilder decimalNumeral("(");
    decimalNumeral.Append(numeral->ToString());
    decimalNumeral.Append("|");
    decimalNumeral.Append(numeral->ToString());
    decimalNumeral.Append(decimalSeparator);
    decimalNumeral.Append(digit.ToString());
    decimalNumeral.Append("*+|");
    decimalNumeral.Append(decimalSeparator);
    decimalNumeral.Append(digit.ToString());
    decimalNumeral.Append("++)");

    StringBuilder exponent("([eE][+-]?");
    exponent.Append(digit.ToString());
    exponent.Append("+)?");

    StringBuilder decimal("(([-+]?");
    decimal.Append(decimalNumeral.ToString());
    decimal.Append("(");
    decimal.Append(exponent.ToString());
    decimal.Append("?)");
    decimal.Append(")|(");
    decimal.Append(AddPositiveSign(&decimalNumeral)->ToString());
    decimal.Append("(");
    decimal.Append(exponent.ToString());
    decimal.Append("?)");
    decimal.Append(")|(");
    decimal.Append(AddNegativeSign(&decimalNumeral)->ToString());
    decimal.Append("(");
    decimal.Append(exponent.ToString());
    decimal.Append("?)");
    decimal.Append("))");

    StringBuilder hexFloat("([-+]?0[xX][0-9a-fA-F]*\\.[0-9a-fA-F]+([pP][-+]?[0-9]+)?)");
    // hexFloat.Append("\\.");
    // hexFloat.Append("[0-9a-fA-F]+([pP][-+]?[0-9]+)?)");

    String localNaN;
    dfs->GetNaN(&localNaN);
    String localeInfinity;
    dfs->GetInfinity(&localeInfinity);

    StringBuilder nonNumber("(NaN|\\Q");
    nonNumber.Append(localNaN);
    nonNumber.Append("\\E|Infinity|\\Q");
    nonNumber.Append(localeInfinity);
    nonNumber.Append("\\E)");

    StringBuilder singedNonNumber("((([-+]?(");
    singedNonNumber.Append(nonNumber.ToString());
    singedNonNumber.Append(")))|(");
    singedNonNumber.Append(AddPositiveSign(&nonNumber)->ToString());
    singedNonNumber.Append(")|(");
    singedNonNumber.Append(AddNegativeSign(&nonNumber)->ToString());
    singedNonNumber.Append("))");

    StringBuilder floatString;
    floatString.Append(decimal.ToString());
    floatString.Append("|");
    floatString.Append(hexFloat.ToString());
    floatString.Append("|");
    floatString.Append(singedNonNumber.ToString());
    mCachedFloatPattern = NULL;
    Pattern::Compile(floatString.ToString(), (IPattern**)&mCachedFloatPattern);
    return mCachedFloatPattern;
}

AutoPtr<StringBuilder> CScanner::GetNumeral(
    /* [in] */ StringBuilder* digit,
    /* [in] */ StringBuilder* nonZeroDigit)
{
    AutoPtr<IDecimalFormatSymbols> dfs;
    mDecimalFormat->GetDecimalFormatSymbols((IDecimalFormatSymbols**)&dfs);

    Char32 outchar;
    dfs->GetGroupingSeparator(&outchar);
    StringBuilder groupSeparator("\\");
    groupSeparator.AppendChar(outchar);

    StringBuilder groupedNumeral("(");
    groupedNumeral.Append(nonZeroDigit->ToString());
    groupedNumeral.Append(digit->ToString());
    groupedNumeral.Append("?");
    groupedNumeral.Append(digit->ToString());
    groupedNumeral.Append("?(");
    groupedNumeral.Append(groupSeparator.ToString());
    groupedNumeral.Append(digit->ToString());
    groupedNumeral.Append(digit->ToString());
    groupedNumeral.Append(digit->ToString());
    groupedNumeral.Append(")+)");

    AutoPtr<StringBuilder> numeral = new StringBuilder("((");
    numeral->Append(digit->ToString());
    numeral->Append("++)|");
    numeral->Append(groupedNumeral.ToString());
    numeral->Append(")");
    return numeral;
}

AutoPtr<StringBuilder> CScanner::AddPositiveSign(
    /* [in] */ StringBuilder* unSignNumeral)
{
    String positivePrefix("");
    String positiveSuffix("");
    String gpp;
    mDecimalFormat->GetPositivePrefix(&gpp);
    if (!gpp.IsEmpty()) {
        positivePrefix = String("\\Q") + gpp + "\\E";
    }
    String gps;
    mDecimalFormat->GetPositiveSuffix(&gps);
    if (!gps.IsEmpty()) {
        positiveSuffix = String("\\Q") + gps + "\\E";
    }
    AutoPtr<StringBuilder> signedNumeral = new StringBuilder();
    signedNumeral->Append(positivePrefix);
    signedNumeral->Append(unSignNumeral->ToString());
    signedNumeral->Append(positiveSuffix);
    return signedNumeral;
}

AutoPtr<StringBuilder> CScanner::AddNegativeSign(
    /* [in] */ StringBuilder* unSignNumeral)
{
    String negativePrefix("");
    String negativeSuffix("");
    String gnp;
    mDecimalFormat->GetNegativePrefix(&gnp);
    if (!gnp.IsEmpty()) {
        negativePrefix = String("\\Q") + gnp + "\\E";
    }
    String gns;
    mDecimalFormat->GetNegativeSuffix(&gns);
    if (!gns.IsEmpty()) {
        negativeSuffix = String("\\Q") + gns + "\\E";
    }
    AutoPtr<StringBuilder> signedNumeral = new StringBuilder();
    signedNumeral->Append(negativePrefix);
    signedNumeral->Append(unSignNumeral->ToString());
    signedNumeral->Append(negativeSuffix);
    return signedNumeral;
}

String CScanner::RemoveLocaleInfoFromFloat(
    /* [in] */ const String& floatString)
{
    // If the token is HexFloat
    if (-1 != floatString.IndexOf('x') || -1 != floatString.IndexOf('X')) {
        return floatString;
    }

    Int32 exponentIndex;
    // If the token is scientific notation
    if (-1 != (exponentIndex = floatString.IndexOf('e'))
            || -1 != (exponentIndex = floatString.IndexOf('E'))) {
        String decimalNumeralString = floatString.Substring(0, exponentIndex);
        String exponentString = floatString.Substring(exponentIndex + 1,
                floatString.GetLength());
        decimalNumeralString = RemoveLocaleInfo(decimalNumeralString, FLOAT);
        return decimalNumeralString + String("e") + exponentString;
    }
    return RemoveLocaleInfo(floatString, FLOAT);
}

String CScanner::RemoveLocaleInfo(
    /* [in] */ const String& token,
    /* [in] */ DataType type)
{
    StringBuilder tokenBuilder(token);
    Boolean negative = RemoveLocaleSign(&tokenBuilder);
    // Remove group separator
    AutoPtr<IDecimalFormatSymbols> dfs;
    mDecimalFormat->GetDecimalFormatSymbols((IDecimalFormatSymbols**)&dfs);
    Char32 outchar;
    dfs->GetGroupingSeparator(&outchar);
    StringBuilder groupSeparator;
    groupSeparator.AppendChar(outchar);
    Int32 separatorIndex = -1;
    String gs = groupSeparator.ToString();
    while (-1 != (tokenBuilder.IndexOf(gs, &separatorIndex), separatorIndex)) {
        tokenBuilder.Delete(separatorIndex, separatorIndex + 1);
    }
    // Remove decimal separator
    dfs->GetDecimalSeparator(&outchar);
    StringBuilder decimalSeparator;
    decimalSeparator.AppendChar(outchar);
    tokenBuilder.IndexOf(decimalSeparator.ToString(), &separatorIndex);
    StringBuilder result("");
    if (INT == type) {
        String sbstr = tokenBuilder.ToString();
        AutoPtr<ArrayOf<Char32> > chars = sbstr.GetChars();
        for (Int32 i = 0; i < chars->GetLength(); i++) {
            if (-1 != Character::ToDigit((*chars)[i], Character::MAX_RADIX)) {
                result.AppendChar((*chars)[i]);
            }
        }
    }
    else if (FLOAT == type) {
        String sbstr = tokenBuilder.ToString();
        String strnan;
        if (sbstr.Equals((dfs->GetNaN(&strnan), strnan))) {
            result += "NaN";
        }
        else if (sbstr.Equals((dfs->GetInfinity(&strnan), strnan))) {
            result += "Infinity";
        }
        else {
            AutoPtr<ArrayOf<Char32> > chars = sbstr.GetChars();
            for (Int32 i = 0; i < chars->GetLength(); i++) {
                if (-1 != Character::ToDigit((*chars)[i], 10)) {
                    result += Character::ToDigit((*chars)[i], 10);
                }
            }
        }
    }
    else {
        return String(NULL);//E_ASSERTION_ERROR;
    }

    // Token is NaN or Infinity
    if (result.GetLength() == 0) {
        result += tokenBuilder;
    }
    if (-1 != separatorIndex) {
        result.InsertChar(separatorIndex, '.');
    }
    // If input is negative
    if (negative) {
        result.InsertChar(0, '-');
    }
    return result.ToString();
}

Boolean CScanner::RemoveLocaleSign(
    /* [in] */ StringBuilder* tokenBuilder)
{
    String positivePrefix;
    mDecimalFormat->GetPositivePrefix(&positivePrefix);
    String positiveSuffix;
    mDecimalFormat->GetPositiveSuffix(&positiveSuffix);
    String negativePrefix;
    mDecimalFormat->GetNegativePrefix(&negativePrefix);
    String negativeSuffix;
    mDecimalFormat->GetNegativeSuffix(&negativeSuffix);

    Int32 indexvalue = 0;
    if ((tokenBuilder->IndexOf(String("+"), &indexvalue), indexvalue) == 0) {
        tokenBuilder->Delete(0, 1);
    }
    if (!positivePrefix.IsEmpty()
        && (tokenBuilder->IndexOf(positivePrefix, &indexvalue), indexvalue) == 0) {
        tokenBuilder->Delete(0, positivePrefix.GetLength());
    }
    if (!positiveSuffix.IsEmpty()
            && -1 != (tokenBuilder->IndexOf(positiveSuffix, &indexvalue), indexvalue)) {
        tokenBuilder->Delete(
            tokenBuilder->GetLength() - positiveSuffix.GetLength(),
            tokenBuilder->GetLength());
    }
    Boolean negative = FALSE;
    if (tokenBuilder->IndexOf(String("-"), &indexvalue), indexvalue == 0) {
        tokenBuilder->Delete(0, 1);
        negative = TRUE;
    }
    if (!negativePrefix.IsEmpty()
        && (tokenBuilder->IndexOf(negativePrefix, &indexvalue), indexvalue) == 0) {
        tokenBuilder->Delete(0, negativePrefix.GetLength());
        negative = TRUE;
    }
    if (!negativeSuffix.IsEmpty()
            && -1 != (tokenBuilder->IndexOf(negativeSuffix, &indexvalue), indexvalue)) {
        tokenBuilder->Delete(
            tokenBuilder->GetLength() - negativeSuffix.GetLength(),
            tokenBuilder->GetLength());
        negative = TRUE;
    }
    return negative;
}

Boolean CScanner::SetTokenRegion()
{
    // The position where token begins
    Int32 tokenStartIndex = 0;
    // The position where token ends
    Int32 tokenEndIndex = 0;
    // Use mDelimiter pattern
    mMatcher->UsePattern(mDelimiter);
    mMatcher->Region(mFindStartIndex, mBufferLength);

    tokenStartIndex = FindPreDelimiter();
    if (SetHeadTokenRegion(tokenStartIndex)) {
        return TRUE;
    }
    tokenEndIndex = FindDelimiterAfter();
    // If the second mDelimiter is not found
    if (-1 == tokenEndIndex) {
        // Just first Delimiter Exists
        if (mFindStartIndex == mBufferLength) {
            return FALSE;
        }
        tokenEndIndex = mBufferLength;
        mFindStartIndex = mBufferLength;
    }

    mMatcher->Region(tokenStartIndex, tokenEndIndex);
    return TRUE;
}

Int32 CScanner::FindPreDelimiter()
{
    Int32 tokenStartIndex;
    Boolean findComplete = FALSE;
    while (!findComplete) {
        if (mMatcher->Find(&findComplete), findComplete) {
            findComplete = TRUE;
            // If just mDelimiter remains
            Int32 startvalue =0;
            if (((IMatchResult::Probe(mMatcher))->Start(&startvalue), startvalue) == mFindStartIndex
                    && ((IMatchResult::Probe(mMatcher))->End(&startvalue), startvalue) == mBufferLength) {
                // If more input resource exists
                if (!mInputExhausted) {
                    ReadMore();
                    ResetMatcher();
                    findComplete = FALSE;
                }
            }
        }
        else {
            if (!mInputExhausted) {
                ReadMore();
                ResetMatcher();
            }
            else {
                return -1;
            }
        }
    }
    (IMatchResult::Probe(mMatcher))->End(&tokenStartIndex);
    mFindStartIndex = tokenStartIndex;
    return tokenStartIndex;
}

Boolean CScanner::SetHeadTokenRegion(
    /* [in] */ Int32 findIndex)
{
    Int32 tokenStartIndex;
    Int32 tokenEndIndex;
    Boolean setSuccess = FALSE;
    // If no mDelimiter exists, but something exites in this scanner
    if (-1 == findIndex && mPreStartIndex != mBufferLength) {
        tokenStartIndex = mPreStartIndex;
        tokenEndIndex = mBufferLength;
        mFindStartIndex = mBufferLength;
        mMatcher->Region(tokenStartIndex, tokenEndIndex);
        setSuccess = TRUE;
    }
    Int32 startvalue = 0;
    // If the first mDelimiter of scanner is not at the find start position
    if (-1 != findIndex && mPreStartIndex != ((IMatchResult::Probe(mMatcher))->Start(&startvalue), startvalue)) {
        tokenStartIndex = mPreStartIndex;
        (IMatchResult::Probe(mMatcher))->Start(&tokenEndIndex);
        (IMatchResult::Probe(mMatcher))->Start(&mFindStartIndex);
        // set match region and return
        mMatcher->Region(tokenStartIndex, tokenEndIndex);
        setSuccess = TRUE;
    }
    return setSuccess;
}

Int32 CScanner::FindDelimiterAfter()
{
    Int32 tokenEndIndex = 0;
    Boolean findComplete = FALSE;
    while (!findComplete) {
        if (mMatcher->Find(&findComplete), findComplete) {
            findComplete = TRUE;
            Int32 endvalue = 0;
            if (((IMatchResult::Probe(mMatcher))->Start(&tokenEndIndex), tokenEndIndex) == mFindStartIndex
                    && tokenEndIndex == ((IMatchResult::Probe(mMatcher))->End(&endvalue), endvalue)) {
                findComplete = FALSE;
            }
        }
        else {
            if (!mInputExhausted) {
                ReadMore();
                ResetMatcher();
            }
            else {
                return -1;
            }
        }
    }
    (IMatchResult::Probe(mMatcher))->Start(&tokenEndIndex);
    mFindStartIndex = tokenEndIndex;
    return tokenEndIndex;
}

ECode CScanner::ReadMore()
{
    Int32 oldPosition = 0;
    (IBuffer::Probe(mBuffer))->GetPosition(&oldPosition);
    Int32 oldBufferLength = mBufferLength;
    // Increase capacity if empty space is not enough
    Int32 capacitylen = 0;
    if (mBufferLength >= ((IBuffer::Probe(mBuffer))->GetCapacity(&capacitylen), capacitylen)) {
        ExpandBuffer();
    }

    // Read input resource
    Int32 readCount = 0;
    // try {
    (IBuffer::Probe(mBuffer))->SetLimit(capacitylen);
    (IBuffer::Probe(mBuffer))->SetPosition(oldBufferLength);

    ECode ec = mInput->Read(mBuffer, &readCount);
    FAIL_GOTO(ec, _EXIT_)

    while (readCount == 0) {
        // nothing to do here
        ec = mInput->Read(mBuffer, &readCount);
        FAIL_GOTO(ec, _EXIT_)
    }

_EXIT_:
    // } catch (IOException e) {
    if (ec == (ECode)E_IO_EXCEPTION) {
        // Consider the scenario: readable puts 4 chars into
        // buffer and then an IOException is thrown out. In this case,
        // buffer is
        // actually grown, but readable.read() will never return.
        (IBuffer::Probe(mBuffer))->GetPosition(&mBufferLength);
        /*
         * Uses -1 to record IOException occurring, and no more input can be
         * read.
         */
        readCount = -1;
        mLastIOException = ec;
    }
    // }

    (IBuffer::Probe(mBuffer))->Flip();
    (IBuffer::Probe(mBuffer))->SetPosition(oldPosition);
    if (-1 == readCount) {
        mInputExhausted = TRUE;
    }
    else {
        mBufferLength = readCount + mBufferLength;
    }
    return NOERROR;
}

ECode CScanner::ExpandBuffer()
{
    Int32 oldPosition = 0;
    (IBuffer::Probe(mBuffer))->GetPosition(&oldPosition);
    Int32 oldCapacity = 0;
    (IBuffer::Probe(mBuffer))->GetCapacity(&oldCapacity);
    Int32 oldLimit = 0;
    (IBuffer::Probe(mBuffer))->GetLimit(&oldLimit);
    Int32 newCapacity = oldCapacity * 2;
    AutoPtr< ArrayOf<Char32> > newBuffer = ArrayOf<Char32>::Alloc(newCapacity);
    AutoPtr< ArrayOf<Char32> > outarr;
    mBuffer->GetArray((ArrayOf<Char32>**)&outarr);
    newBuffer->Copy(0, outarr, 0, oldLimit);
    mBuffer = NULL;
    CharBuffer::Wrap(newBuffer, 0, newCapacity, (ICharBuffer**)&mBuffer);
    (IBuffer::Probe(mBuffer))->SetPosition(oldPosition);
    (IBuffer::Probe(mBuffer))->SetLimit(oldLimit);
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos

