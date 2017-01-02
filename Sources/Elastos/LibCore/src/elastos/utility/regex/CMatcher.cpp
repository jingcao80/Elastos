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

#include "CMatcher.h"
#include "Pattern.h"
#include "MatchResultImpl.h"
#include "eldebugdef.h"
#include "CString.h"
#include "StringBuilder.h"
#include "unicode/parseerr.h"
#include "unicode/regex.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Utility {
namespace Regex {

/**
* Checks if an error has occurred, throwing a suitable exception if so.
* @param env JNI environment
* @param errorCode code to determine if it is an error
* @return 0 if errorCode is not an error, 1 if errorCode is an error, but the
*         creation of the exception to be thrown fails
 * @exception thrown if errorCode represents an error
*/
static ECode icu4_error(UErrorCode errorCode)
{
//    const char* message = u_errorName(errorCode);
    if (errorCode <= U_ZERO_ERROR || errorCode >= U_ERROR_LIMIT) {
        return NOERROR;
    }

    switch (errorCode) {
    case U_ILLEGAL_ARGUMENT_ERROR:
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    case U_INDEX_OUTOFBOUNDS_ERROR:
    case U_BUFFER_OVERFLOW_ERROR:
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    case U_UNSUPPORTED_ERROR:
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    default:
        return E_RUNTIME_EXCEPTION;
    }
}


/**
 * We use ICU4C's RegexMatcher class, but our input is on the Java heap and potentially moving
 * around between calls. This wrapper class ensures that our RegexMatcher is always pointing at
 * the current location of the char[]. Earlier versions of Android simply copied the data to the
 * native heap, but that's wasteful and hides allocations from the garbage collector.
 */
class MatcherAccessor
{
public:
    MatcherAccessor(
        /* [in] */ RegexMatcher* matcher,
        /* [in] */ const String& input,
        /* [in] */ Boolean reset)
    {
        Init(matcher);

        mInput = input;
        mChars = mInput.string();
        if (mChars == NULL) {
            return;
        }

        mUText = utext_openUTF8(NULL, mChars, mInput.GetByteLength(), &mStatus);
        if (mUText == NULL) {
            return;
        }

        if (reset) {
            mMatcher->reset(mUText);
        }
        else {
            mMatcher->refreshInputText(mUText, mStatus);
        }
    }

    MatcherAccessor(
    /* [in] */ RegexMatcher* matcher)
    {
        Init(matcher);
    }

    ~MatcherAccessor()
    {
        utext_close(mUText);
        if (mInput) {
            mChars = NULL;
        }
        //maybeThrowIcuException(mEnv, "utext_close", mStatus);
    }

    RegexMatcher* operator->()
    {
        return mMatcher;
    }

    UErrorCode& Status()
    {
        return mStatus;
    }

    void UpdateOffsets(
        /* [in/out] */ ArrayOf<Int32>* offsets)
    {
        if (offsets == NULL) {
            return;
        }

        for (size_t i = 0, groupCount = mMatcher->groupCount(); i <= groupCount; ++i) {
            (*offsets)[2 * i + 0] = mMatcher->start(i, mStatus);
            (*offsets)[2 * i + 1] = mMatcher->end(i, mStatus);
        }
    }

private:
    void Init(RegexMatcher* matcher)
    {
        mInput = String(NULL);
        mMatcher = matcher;
        mChars = NULL;
        mStatus = U_ZERO_ERROR;
        mUText = NULL;
    }

    String mInput;
    RegexMatcher* mMatcher;
    const char* mChars;
    UErrorCode mStatus;
    UText* mUText;

    // Disallow copy and assignment.
    MatcherAccessor(const MatcherAccessor&);
    void operator=(const MatcherAccessor&);
};

CAR_INTERFACE_IMPL_2(CMatcher, Object, IMatcher, IMatchResult)

CAR_OBJECT_IMPL(CMatcher)

CMatcher::CMatcher()
    : mNativeMatcher(NULL)
    , mRegionStart(0)
    , mRegionEnd(0)
    , mAppendPos(0)
    , mMatchFound(FALSE)
    , mAnchoringBounds(TRUE)
    , mTransparentBounds(FALSE)
{
}

CMatcher::~CMatcher()
{
    if (mNativeMatcher != NULL) {
        delete mNativeMatcher;
    }
}

ECode CMatcher::constructor(
    /* [in] */ IPattern* pattern,
    /* [in] */ ICharSequence* input)
{
    FAIL_RETURN(UsePattern(pattern));
    return Reset(input);
}

ECode CMatcher::AppendReplacement(
    /* [in] */ IStringBuffer* buffer,
    /* [in] */ const String& replacement)
{
    VALIDATE_NOT_NULL(buffer);
    if (replacement.IsNull()) {
        return NOERROR;
    }

    Int32 index;
    Start(&index);
    String subStr = mInput.Substring(mAppendPos, index);
    buffer->Append(subStr);
    AppendEvaluated((StringBuffer*)buffer, replacement);
    return End(&mAppendPos);
}

ECode CMatcher::AppendEvaluated(
    /* [in] */ StringBuffer* buffer,
    /* [in] */ const String& s)
{
    VALIDATE_NOT_NULL(buffer);
    if (s.IsNull()) {
        return NOERROR;
    }

    Boolean escape = FALSE;
    Boolean dollar = FALSE;

    Char32 c;
    String text;
    AutoPtr<ArrayOf<Char32> > array = s.GetChars();
    for (Int32 i = 0; i < array->GetLength(); i++) {
        c = (*array)[i];
        if (c == '\\' && !escape) {
            escape = TRUE;
        }
        else if (c == '$' && !escape) {
            dollar = TRUE;
        }
        else if (c >= '0' && c <= '9' && dollar) {
            Group((c - '0'), &text);

            buffer->Append(text);
            dollar = FALSE;
        }
        else {
            buffer->AppendChar(c);
            dollar = FALSE;
            escape = FALSE;
        }
    }

    // This seemingly stupid piece of code reproduces a JDK bug.
    if (escape) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//        throw new ArrayIndexOutOfBoundsException(s.length());
    }

    return NOERROR;
}

ECode CMatcher::Reset()
{
    AutoPtr<ICharSequence> charSeq;
    CString::New(mInput, (ICharSequence**)&charSeq);

    return Reset(charSeq, 0, mInput.GetLength());
}

ECode CMatcher::Reset(
    /* [in] */ ICharSequence* input)
{
    VALIDATE_NOT_NULL(input);

    Int32 length;
    input->GetLength(&length);
    return Reset(input, 0, length);
}

ECode CMatcher::Reset(
    /* [in] */ ICharSequence* input,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    VALIDATE_NOT_NULL(input);

    Int32 length;
    input->GetLength(&length);
    if (start < 0 || end < 0 || start > length || end > length || start > end) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//        throw new IndexOutOfBoundsException();
    }

    input->ToString(&mInput);
    mRegionStart = start;
    mRegionEnd = end;

    ResetForInput();

    mMatchFound = FALSE;
    mAppendPos = 0;

    return NOERROR;
}

ECode CMatcher::UsePattern(
    /* [in] */ IPattern* pattern)
{
    VALIDATE_NOT_NULL(pattern);

    mPattern = pattern;

    if (mNativeMatcher != NULL) {
        delete mNativeMatcher;
        mNativeMatcher = NULL;
    }

    using Elastos::Utility::Regex::Pattern;
    RegexPattern* nativePattern = ((Pattern*)pattern)->mNativePattern;
    FAIL_RETURN(OpenImpl(nativePattern, &mNativeMatcher));
    if (!mInput.IsNull()) {
        ResetForInput();
    }

    Int32 count;
    GroupCount(&count);
    mMatchOffsets = ArrayOf<Int32>::Alloc((count + 1) * 2);
    mMatchFound = FALSE;

    return NOERROR;
}

void CMatcher::ResetForInput()
{
    {    AutoLock syncLock(this);
        SetInputImpl(mNativeMatcher, mInput, mRegionStart, mRegionEnd);
        UseAnchoringBoundsImpl(mNativeMatcher, mAnchoringBounds);
        UseTransparentBoundsImpl(mNativeMatcher, mTransparentBounds);
    }
}

ECode CMatcher::Region(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<ICharSequence> charSeq;
    CString::New(mInput, (ICharSequence**)&charSeq);

    return Reset(charSeq, start, end);
}

ECode CMatcher::AppendTail(
    /* [in] */ IStringBuffer* buffer)
{
    VALIDATE_NOT_NULL(buffer);

    if (mAppendPos < mRegionEnd) {
        String subStr = mInput.Substring(mAppendPos, mRegionEnd);
        buffer->Append(subStr);
    }

    return NOERROR;
}

ECode CMatcher::ReplaceFirst(
    /* [in] */ const String& replacement,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    Reset();

    StringBuffer buffer(mInput.GetByteLength());
    Boolean found;
    Find(&found);
    if (found) {
        AppendReplacement(&buffer, replacement);
    }
    AppendTail((IStringBuffer*)&buffer);
    return buffer.ToString(result);
}

ECode CMatcher::ReplaceAll(
    /* [in] */ const String& replacement,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    Reset();

    StringBuffer buffer(mInput.GetByteLength());
    Boolean found;
    while (Find(&found), found) {
        AppendReplacement(&buffer, replacement);
    }
    AppendTail((IStringBuffer*)&buffer);
    return buffer.ToString(result);
}

ECode CMatcher::Pattern(
    /* [out] */ IPattern** pattern)
{
    VALIDATE_NOT_NULL(pattern);
    *pattern = mPattern;
    REFCOUNT_ADD(*pattern);
    return NOERROR;
}

ECode CMatcher::Group(
    /* [out] */ String* text)
{
    return Group(0, text);
}

ECode CMatcher::Group(
    /* [in] */ Int32 group,
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    *text = String(NULL);

    FAIL_RETURN(EnsureMatch());
    Int32 from = (*mMatchOffsets)[group * 2];
    Int32 to = (*mMatchOffsets)[(group * 2) + 1];
    if (from == -1 || to == -1) {
        return NOERROR;
    }
    else {
        *text = mInput.Substring(from, to);
    }

    return NOERROR;
}

ECode CMatcher::Find(
    /* [out] */ Boolean* found)
{
    VALIDATE_NOT_NULL(found);

    {    AutoLock syncLock(this);
        mMatchFound = FindNextImpl(mNativeMatcher, mInput, mMatchOffsets);
    }
    *found = mMatchFound;

    return NOERROR;
}

ECode CMatcher::Find(
    /* [in] */ Int32 start,
    /* [out] */ Boolean* found)
{
    VALIDATE_NOT_NULL(found);

    if (start < 0 || start > (Int32)mInput.GetLength()) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
//        throw new IndexOutOfBoundsException("start=" + start + "; length=" + input.length());
    }
    {    AutoLock syncLock(this);
        mMatchFound = FindImpl(mNativeMatcher, mInput, start, mMatchOffsets);
    }
    *found = mMatchFound;
    return NOERROR;
}

ECode CMatcher::LookingAt(
    /* [out] */ Boolean* matched)
{
    VALIDATE_NOT_NULL(matched);

    {    AutoLock syncLock(this);
        mMatchFound = LookingAtImpl(mNativeMatcher, mInput, mMatchOffsets);
    }
    *matched = mMatchFound;

    return NOERROR;
}

ECode CMatcher::Matches(
    /* [out] */ Boolean* matched)
{
    VALIDATE_NOT_NULL(matched);

    {    AutoLock syncLock(this);
        mMatchFound = MatchesImpl(mNativeMatcher, mInput, mMatchOffsets);
    }
    *matched = mMatchFound;
    return NOERROR;
}

ECode CMatcher::Start(
    /* [in] */ Int32 group,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    FAIL_RETURN(EnsureMatch());
    *index = (*mMatchOffsets)[group * 2];
    return NOERROR;
}

ECode CMatcher::End(
    /* [in] */ Int32 group,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);

    FAIL_RETURN(EnsureMatch());
    *index = (*mMatchOffsets)[(group * 2) + 1];
    return NOERROR;
}

String CMatcher::QuoteReplacement(
    /* [in] */ const String& s)
{
    StringBuilder result(s.GetByteLength());
    AutoPtr<ArrayOf<Char32> > charArray = s.GetChars();
    Char32 c;
    for (Int32 i = 0; i < charArray->GetLength(); i++) {
        c = (*charArray)[i];
        if (c == '\\' || c == '$') {
            result.AppendChar('\\');
        }
        result.AppendChar(c);
    }
    return result.ToString();
}

ECode CMatcher::Start(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    return Start(0, index);
}

ECode CMatcher::GroupCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = GroupCountImpl(mNativeMatcher);
    return NOERROR;
}

ECode CMatcher::End(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    return End(0, index);
}

ECode CMatcher::ToMatchResult(
    /* [out] */ IMatchResult** result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(EnsureMatch());
    *result = (IMatchResult*)new MatchResultImpl(mInput, *mMatchOffsets);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMatcher::UseAnchoringBounds(
    /* [in] */ Boolean value)
{
    {    AutoLock syncLock(this);
        mAnchoringBounds = value;
        UseAnchoringBoundsImpl(mNativeMatcher, value);
    }
    return NOERROR;
}

ECode CMatcher::HasAnchoringBounds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAnchoringBounds;
    return NOERROR;
}

ECode CMatcher::UseTransparentBounds(
    /* [in] */ Boolean value)
{
    {    AutoLock syncLock(this);
        mTransparentBounds = value;
        UseTransparentBoundsImpl(mNativeMatcher, value);
    }
    return NOERROR;
}

ECode CMatcher::EnsureMatch()
{
    if (!mMatchFound) {
        return E_ILLEGAL_STATE_EXCEPTION;
//        throw new IllegalStateException("No successful match so far");
    }
    return NOERROR;
}

ECode CMatcher::HasTransparentBounds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTransparentBounds;
    return NOERROR;
}

ECode CMatcher::RegionStart(
    /* [out] */ Int32* start)
{
    VALIDATE_NOT_NULL(start);
    *start = mRegionStart;
    return NOERROR;
}

ECode CMatcher::RegionEnd(
    /* [out] */ Int32* end)
{
    VALIDATE_NOT_NULL(end);
    *end = mRegionEnd;
    return NOERROR;
}

ECode CMatcher::RequireEnd(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(this);
        *result = RequireEndImpl(mNativeMatcher);
    }
    return NOERROR;
}

ECode CMatcher::HitEnd(
    /* [out] */ Boolean* hit)
{
    VALIDATE_NOT_NULL(hit);
    {    AutoLock syncLock(this);
        *hit = HitEndImpl(mNativeMatcher);
    }
    return NOERROR;
}

Boolean CMatcher::FindImpl(
    /* [in] */ RegexMatcher* matcher,
    /* [in] */ const String& s,
    /* [in] */ Int32 startIndex,
    /* [in] */ ArrayOf<Int32>* offsets)
{
    MatcherAccessor matcherAccessor(matcher, s, FALSE);
    UBool result = matcherAccessor->find(startIndex, matcherAccessor.Status());
    if (result) {
        matcherAccessor.UpdateOffsets(offsets);
    }
    return result;
}

Boolean CMatcher::FindNextImpl(
    /* [in] */ RegexMatcher* matcher,
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<Int32>* offsets)
{
    MatcherAccessor matcherAccessor(matcher, s, FALSE);
    if (matcherAccessor.Status() != U_ZERO_ERROR) {
        return -1;
    }
    UBool result = matcherAccessor->find();
    if (result) {
        matcherAccessor.UpdateOffsets(offsets);
    }
    return result;
}

Int32 CMatcher::GroupCountImpl(
    /* [in] */ RegexMatcher* matcher)
{
    MatcherAccessor matcherAccessor(matcher);
    return matcherAccessor->groupCount();
}

Boolean CMatcher::HitEndImpl(
    /* [in] */ RegexMatcher* matcher)
{
    MatcherAccessor matcherAccessor(matcher);
    return matcherAccessor->hitEnd();
}

Boolean CMatcher::LookingAtImpl(
    /* [in] */ RegexMatcher* matcher,
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<Int32>* offsets)
{
    MatcherAccessor matcherAccessor(matcher, s, FALSE);
    UBool result = matcherAccessor->lookingAt(matcherAccessor.Status());
    if (result) {
        matcherAccessor.UpdateOffsets(offsets);
    }
    return result;
}

Boolean CMatcher::MatchesImpl(
    /* [in] */ RegexMatcher* matcher,
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<Int32>* offsets)
{
    MatcherAccessor matcherAccessor(matcher, s, FALSE);
    UBool result = matcherAccessor->matches(matcherAccessor.Status());
    if (result) {
        matcherAccessor.UpdateOffsets(offsets);
    }
    return result;
}

ECode CMatcher::OpenImpl(
    /* [in] */ RegexPattern* pattern,
    /* [out] */ RegexMatcher** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(pattern);

    UErrorCode status = U_ZERO_ERROR;
    *result = pattern->matcher(status);
    return icu4_error(status);
}

Boolean CMatcher::RequireEndImpl(
    /* [in] */ RegexMatcher* matcher)
{
    assert(matcher != NULL);
    MatcherAccessor matcherAccessor(matcher);
    return matcherAccessor->requireEnd();
}

void CMatcher::SetInputImpl(
    /* [in] */ RegexMatcher* matcher,
    /* [in] */ const String& s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    MatcherAccessor matcherAccessor(matcher, s, TRUE);
    matcherAccessor->region(start, end, matcherAccessor.Status());
}

void CMatcher::UseAnchoringBoundsImpl(
    /* [in] */ RegexMatcher* matcher,
    /* [in] */ Boolean value)
{
    MatcherAccessor matcherAccessor(matcher);
    matcherAccessor->useAnchoringBounds(value);
}

void CMatcher::UseTransparentBoundsImpl(
    /* [in] */ RegexMatcher* matcher,
    /* [in] */ Boolean value)
{
    MatcherAccessor matcherAccessor(matcher);
    matcherAccessor->useTransparentBounds(value);
}

} // namespace Regex
} // namespace Utility
} // namespace Elastos
