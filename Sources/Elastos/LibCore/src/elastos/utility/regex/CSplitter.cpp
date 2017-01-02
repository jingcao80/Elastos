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

#include "CSplitter.h"
#include "CMatcher.h"
#include "CString.h"
#include "Math.h"
#include "EmptyArray.h"

using Elastos::Core::CString;
using Elastos::Core::Math;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Utility {
namespace Regex {

CAR_INTERFACE_IMPL(CSplitter, Singleton, ISplitter)

CAR_SINGLETON_IMPL(CSplitter)

const String CSplitter::METACHARACTERS("\\?*+[](){}^$.|");

ECode CSplitter::FastSplit(
    /* [in] */ const String& regularExpression,
    /* [in] */ const String& input,
    /* [in] */ Int32 limit,
    /* [out, callee] */ ArrayOf<String>** array)
{
    VALIDATE_NOT_NULL(array)
    if (input.IsNull()) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    *array = NULL;

    // Can we do it cheaply?
    Int32 len = regularExpression.GetLength();
    if (len == 0) {
        return NOERROR;
    }
    Char32 ch = regularExpression.GetChar(0);
    if (len == 1 && METACHARACTERS.IndexOf(ch) == -1) {
        // We're looking for a single non-metacharacter. Easy.
    }
    else if (len == 2 && ch == '\\') {
        // We're looking for a quoted character.
        // Quoted metacharacters are effectively single non-metacharacters.
        ch = regularExpression.GetChar(1);
        if (METACHARACTERS.IndexOf(ch) == -1) {
            return NOERROR;
        }
    }
    else {
        return NOERROR;
    }

    // We can do this cheaply...

    // Unlike Perl, which considers the result of splitting the empty string to be the empty
    // array, Java returns an array containing the empty string.
    if (input.IsEmpty()) {
        *array = ArrayOf<String>::Alloc(1);
        REFCOUNT_ADD(*array);
        (**array)[0] = String("");
        return NOERROR;
    }

    // Count separators
    Int32 separatorCount = 0;
    Int32 begin = 0;
    Int32 end;
    while (separatorCount + 1 != limit && (end = input.IndexOf(ch, begin)) != -1) {
        ++separatorCount;
        begin = end + 1;
    }
    Int32 lastPartEnd = input.GetLength();
    if (limit == 0 && begin == lastPartEnd) {
        // Last part is empty for limit == 0, remove all trailing empty matches.
        if (separatorCount == lastPartEnd) {
            // Input contains only separators.
            *array = EmptyArray::STRING;
            REFCOUNT_ADD(*array)
            return NOERROR;
        }

        AutoPtr<ArrayOf<Char32> > chars = input.GetChars();
        // Find the beginning of trailing separators.
        do {
            --begin;
        } while ((*chars)[begin - 1] == ch);
        // Reduce separatorCount and fix lastPartEnd.
        separatorCount -= input.GetLength() - begin;
        lastPartEnd = begin;
    }

    // Collect the result parts.
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(separatorCount + 1);
    begin = 0;
    for (Int32 i = 0; i != separatorCount; ++i) {
        end = input.IndexOf(ch, begin);
        result->Set(i, input.Substring(begin, end));
        begin = end + 1;
    }
    // Add last part.
    result->Set(separatorCount, input.Substring(begin, lastPartEnd));
    *array = result;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CSplitter::Split(
    /* [in] */ IPattern* pattern,
    /* [in] */ const String& regularExpression,
    /* [in] */ const String& input,
    /* [in] */ Int32 limit,
    /* [out, callee] */ ArrayOf<String>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    AutoPtr<ArrayOf<String> > splitResult;
    FAIL_RETURN(FastSplit(regularExpression, input, limit, (ArrayOf<String>**)&splitResult));
    if (splitResult) {
        *array = splitResult;
        REFCOUNT_ADD(*array);
        return NOERROR;
    }

    // Unlike Perl, which considers the result of splitting the empty string to be the empty
    // array, Java returns an array containing the empty string.
    if (input.IsEmpty()) {
        *array = ArrayOf<String>::Alloc(1);
        REFCOUNT_ADD(*array);
        (**array)[0] = String("");
        return NOERROR;
    }

    // Collect text preceding each occurrence of the separator, while there's enough space.
    List<String> list;

    AutoPtr<CMatcher> matcher;
    AutoPtr<ICharSequence> inputSeq;
    FAIL_RETURN(CString::New(input, (ICharSequence**)&inputSeq));
    FAIL_RETURN(CMatcher::NewByFriend(pattern, inputSeq, (CMatcher**)&matcher));

    Int32 begin = 0, end;
    Boolean result;
    String subStr;

    while ((list.GetSize() + 1) != limit && (matcher->Find(&result), result)) {
        matcher->Start(&end);
        subStr = input.Substring(begin, end);
        list.PushBack(subStr);
        matcher->End(&begin);
    }
    return FinishSplit(list, input, begin, limit, array);
}

ECode CSplitter::FinishSplit(
    /* [in] */ List<String>& list,
    /* [in] */ const String& input,
    /* [in] */ Int32 begin,
    /* [in] */ Int32 limit,
    /* [out, callee] */ ArrayOf<String>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    // Add trailing text.
    if (begin < input.GetLength()) {
        list.PushBack(input.Substring(begin));
    }
    else if (limit != 0) {
        list.PushBack(String(""));
    }
    else {
        // Remove all trailing empty matches in the limit == 0 case.
        List<String>::ReverseIterator rit = list.RBegin();
        while (rit != list.REnd()) {
            if ((*rit).IsNullOrEmpty()) {
                rit = List<String>::ReverseIterator(list.Erase(--(rit.GetBase())));
            }
            else {
                ++rit;
            }
        }
    }

    if (list.IsEmpty() == FALSE) {
        // Convert to an array.
        *array = ArrayOf<String>::Alloc(list.GetSize());
        List<String>::Iterator it = list.Begin();
        for (Int32 i = 0; it != list.End(); ++i, ++it) {
            (**array)[i] = *it;
        }
        list.Clear();

    }
    REFCOUNT_ADD(*array);
    return NOERROR;
}


} // namespace Regex
} // namespace Utility
} // namespace Elastos
