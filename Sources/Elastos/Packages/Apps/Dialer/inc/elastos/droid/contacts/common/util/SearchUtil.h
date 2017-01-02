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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_SEARCHUTIL_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_SEARCHUTIL_H__

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/** Static methods that are useful for scheduling actions to occur at a later time. */
class SearchUtil
{
public:
    class MatchedLine : public Object
    {
    public:
        MatchedLine()
            :mStartIndex(-1)
        {}

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Int32 mStartIndex;
        String mLine;
    };

public:
    /**
     * Given a string with lines delimited with '\n', finds the matching line to the given
     * substring.
     *
     * @param contents The string to search.
     * @param substring The substring to search for.
     * @return A MatchedLine object containing the matching line and the startIndex of the substring
     * match within that line.
     */
    static CARAPI_(AutoPtr<MatchedLine>) FindMatchingLine(
        /* [in] */ const String& contents,
        /* [in] */ const String& substring);

    /**
     * Similar to String.contains() with two main differences:
     * <p>
     * 1) Only searches token prefixes.  A token is defined as any combination of letters or
     * numbers.
     * <p>
     * 2) Returns the starting index where the substring is found.
     *
     * @param value The string to search.
     * @param substring The substring to look for.
     * @return The starting index where the substring is found. {@literal -1} if substring is not
     *         found in value.
     */
    // @VisibleForTesting
    static CARAPI_(Int32) Contains(
        /* [in] */ const String& value,
        /* [in] */ const String& substring);

    /**
     * Find the start of the next token.  A token is composed of letters and numbers. Any other
     * character are considered delimiters.
     *
     * @param line The string to search for the next token.
     * @param startIndex The index to start searching.  0 based indexing.
     * @return The index for the start of the next token.  line.length() if next token not found.
     */
    // @VisibleForTestings
    static CARAPI_(Int32) FindNextTokenStart(
        /* [in] */ const String& line,
        /* [in] */ Int32 startIndex);

    /**
     * Anything other than letter and numbers are considered delimiters.  Remove start and end
     * delimiters since they are not relevant to search.
     *
     * @param query The query string to clean.
     * @return The cleaned query. Empty string if all characters are cleaned out.
     */
    static CARAPI_(String) CleanStartAndEndOfSearchQuery(
        /* [in] */ const String& query);
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_SEARCHUTIL_H__
