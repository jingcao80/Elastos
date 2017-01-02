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

#ifndef __ELASTOS_UTILITY_REGEX_REGEX_CMATCHER_H__
#define __ELASTOS_UTILITY_REGEX_REGEX_CMATCHER_H__

#include "_Elastos_Utility_Regex_CMatcher.h"
#include "Object.h"
#include "StringBuffer.h"
#include <unicode/regex.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::IStringBuffer;
using Elastos::Core::StringBuffer;
using Elastos::Core::Object;
using U_ICU_NAMESPACE::RegexMatcher;

namespace Elastos {
namespace Utility {
namespace Regex {

/**
 * The result of applying a {@code Pattern} to a given input. See {@link Pattern} for
 * example uses.
 */
CarClass(CMatcher)
    , public Object
    , public IMatcher
    , public IMatchResult
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CMatcher();

    ~CMatcher();

    /**
     * Appends a literal part of the input plus a replacement for the current
     * match to a given {@link StringBuffer}. The literal part is exactly the
     * part of the input between the previous match and the current match. The
     * method can be used in conjunction with {@link #find()} and
     * {@link #appendTail(StringBuffer)} to walk through the input and replace
     * all occurrences of the {@code Pattern} with something else.
     *
     * @param replacement
     *            the replacement text.
     * @return the {@code Matcher} itself.
     * @throws IllegalStateException
     *             if no successful match has been made.
     */
    CARAPI AppendReplacement(
        /* [in] */ IStringBuffer* buffer,
        /* [in] */ const String& replacement);

    /**
     * Resets the {@code Matcher}. This results in the region being set to the
     * whole input. Results of a previous find get lost. The next attempt to
     * find an occurrence of the {@link Pattern} in the string will start at the
     * beginning of the input.
     *
     * @return the {@code Matcher} itself.
     */
    CARAPI Reset();

    /**
     * Provides a new input and resets the {@code Matcher}. This results in the
     * region being set to the whole input. Results of a previous find get lost.
     * The next attempt to find an occurrence of the {@link Pattern} in the
     * string will start at the beginning of the input.
     *
     * @param input
     *            the new input sequence.
     *
     * @return the {@code Matcher} itself.
     */
    CARAPI Reset(
        /* [in] */ ICharSequence* input);

    /**
     * Sets a new pattern for the {@code Matcher}. Results of a previous find
     * get lost. The next attempt to find an occurrence of the {@link Pattern}
     * in the string will start at the beginning of the input.
     *
     * @param pattern
     *            the new {@code Pattern}.
     *
     * @return the {@code Matcher} itself.
     */
    CARAPI UsePattern(
        /* [in] */ IPattern* pattern);

    /**
     * Resets this matcher and sets a region. Only characters inside the region
     * are considered for a match.
     *
     * @param start
     *            the first character of the region.
     * @param end
     *            the first character after the end of the region.
     * @return the {@code Matcher} itself.
     */
    CARAPI Region(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Appends the (unmatched) remainder of the input to the given
     * {@link StringBuffer}. The method can be used in conjunction with
     * {@link #find()} and {@link #appendReplacement(StringBuffer, String)} to
     * walk through the input and replace all matches of the {@code Pattern}
     * with something else.
     *
     * @param buffer
     *            the {@code StringBuffer} to append to.
     * @return the {@code StringBuffer}.
     * @throws IllegalStateException
     *             if no successful match has been made.
     */
    CARAPI AppendTail(
        /* [in] */ IStringBuffer* result);

    /**
     * Replaces the first occurrence of this matcher's pattern in the input with
     * a given string.
     *
     * @param replacement
     *            the replacement text.
     * @return the modified input string.
     */
    CARAPI ReplaceFirst(
        /* [in] */ const String& replacement,
        /* [out] */ String* result);

    /**
     * Replaces all occurrences of this matcher's pattern in the input with a
     * given string.
     *
     * @param replacement
     *            the replacement text.
     * @return the modified input string.
     */
    CARAPI ReplaceAll(
        /* [in] */ const String& replacement,
        /* [out] */ String* result);

    /**
     * Returns the {@link Pattern} instance used inside this matcher.
     *
     * @return the {@code Pattern} instance.
     */
    CARAPI Pattern(
        /* [out] */ IPattern** pattern);

    /**
     * Returns the text that matched the whole regular expression.
     *
     * @return the text.
     * @throws IllegalStateException
     *             if no successful match has been made.
     */
    CARAPI Group(
        /* [out] */ String* text);

    /**
     * Returns the text that matched a given group of the regular expression.
     * Explicit capturing groups in the pattern are numbered left to right in order
     * of their <i>opening</i> parenthesis, starting at 1.
     * The special group 0 represents the entire match (as if the entire pattern is surrounded
     * by an implicit capturing group).
     * For example, "a((b)c)" matching "abc" would give the following groups:
     * <pre>
     * 0 "abc"
     * 1 "bc"
     * 2 "b"
     * </pre>
     *
     * <p>An optional capturing group that failed to match as part of an overall
     * successful match (for example, "a(b)?c" matching "ac") returns null.
     * A capturing group that matched the empty string (for example, "a(b?)c" matching "ac")
     * returns the empty string.
     *
     * @throws IllegalStateException
     *             if no successful match has been made.
     */
    CARAPI Group(
        /* [in] */ Int32 group,
        /* [out] */ String* text);

    /**
     * Returns the next occurrence of the {@link Pattern} in the input. If a
     * previous match was successful, the method continues the search from the
     * first character following that match in the input. Otherwise it searches
     * either from the region start (if one has been set), or from position 0.
     *
     * @return true if (and only if) a match has been found.
     */
    CARAPI Find(
        /* [out] */ Boolean* found);

    /**
     * Returns true if there is another match in the input, starting
     * from the given position. The region is ignored.
     *
     * @throws IndexOutOfBoundsException if {@code start < 0 || start > input.length()}
     */
    CARAPI Find(
        /* [in] */ Int32 start,
        /* [out] */ Boolean* found);

    /**
     * Tries to match the {@link Pattern}, starting from the beginning of the
     * region (or the beginning of the input, if no region has been set).
     * Doesn't require the {@code Pattern} to match against the whole region.
     *
     * @return true if (and only if) the {@code Pattern} matches.
     */
    CARAPI LookingAt(
        /* [out] */ Boolean* matched);

    /**
     * Tries to match the {@link Pattern} against the entire region (or the
     * entire input, if no region has been set).
     *
     * @return true if (and only if) the {@code Pattern} matches the entire
     *         region.
     */
    CARAPI Matches(
        /* [out] */ Boolean* matched);

    /**
     * Returns the index of the first character of the text that matched a given
     * group.
     *
     * @param group
     *            the group, ranging from 0 to groupCount() - 1, with 0
     *            representing the whole pattern.
     *
     * @return the character index.
     */
    CARAPI Start(
        /* [in] */ Int32 group,
        /* [out] */ Int32* index);

    /**
     * Returns the index of the first character following the text that matched
     * a given group.
     *
     * @param group
     *            the group, ranging from 0 to groupCount() - 1, with 0
     *            representing the whole pattern.
     * @return the character index.
     * @throws IllegalStateException
     *             if no successful match has been made.
     */
    CARAPI End(
        /* [in] */ Int32 group,
        /* [out] */ Int32* index);

    /**
     * Returns a replacement string for the given one that has all backslashes
     * and dollar signs escaped.
     *
     * @param s
     *            the input string.
     * @return the input string, with all backslashes and dollar signs having
     *         been escaped.
     */
    static CARAPI_(String) QuoteReplacement(
        /* [in] */ const String& s);

    /**
     * Returns the index of the first character of the text that matched the
     * whole regular expression.
     *
     * @return the character index.
     * @throws IllegalStateException
     *             if no successful match has been made.
     */
    CARAPI Start(
        /* [out] */ Int32* index);

    /**
     * Returns the number of groups in the result, which is always equal to
     * the number of groups in the original regular expression.
     *
     * @return the number of groups.
     */
    CARAPI GroupCount(
        /* [out] */ Int32* count);

    /**
     * Returns the index of the first character following the text that matched
     * the whole regular expression.
     *
     * @return the character index.
     * @throws IllegalStateException
     *             if no successful match has been made.
     */
    CARAPI End(
        /* [out] */ Int32* index);

    /**
     * Converts the current match into a separate {@link MatchResult} instance
     * that is independent from this matcher. The new object is unaffected when
     * the state of this matcher changes.
     *
     * @return the new {@code MatchResult}.
     * @throws IllegalStateException
     *             if no successful match has been made.
     */
    CARAPI ToMatchResult(
        /* [out] */ IMatchResult** result);

    /**
     * Determines whether this matcher has anchoring bounds enabled or not. When
     * anchoring bounds are enabled, the start and end of the input match the
     * '^' and '$' meta-characters, otherwise not. Anchoring bounds are enabled
     * by default.
     *
     * @param value
     *            the new value for anchoring bounds.
     * @return the {@code Matcher} itself.
     */
    CARAPI UseAnchoringBounds(
        /* [in] */ Boolean value);

    /**
     * Indicates whether this matcher has anchoring bounds enabled. When
     * anchoring bounds are enabled, the start and end of the input match the
     * '^' and '$' meta-characters, otherwise not. Anchoring bounds are enabled
     * by default.
     *
     * @return true if (and only if) the {@code Matcher} uses anchoring bounds.
     */
    CARAPI HasAnchoringBounds(
        /* [out] */ Boolean* result);

    /**
     * Determines whether this matcher has transparent bounds enabled or not.
     * When transparent bounds are enabled, the parts of the input outside the
     * region are subject to lookahead and lookbehind, otherwise they are not.
     * Transparent bounds are disabled by default.
     *
     * @param value
     *            the new value for transparent bounds.
     * @return the {@code Matcher} itself.
     */
    CARAPI UseTransparentBounds(
        /* [in] */ Boolean value);

    /**
     * Indicates whether this matcher has transparent bounds enabled. When
     * transparent bounds are enabled, the parts of the input outside the region
     * are subject to lookahead and lookbehind, otherwise they are not.
     * Transparent bounds are disabled by default.
     *
     * @return true if (and only if) the {@code Matcher} uses anchoring bounds.
     */
    CARAPI HasTransparentBounds(
        /* [out] */ Boolean* result);

    /**
     * Returns this matcher's region start, that is, the first character that is
     * considered for a match.
     *
     * @return the start of the region.
     */
    CARAPI RegionStart(
        /* [out] */ Int32* start);

    /**
     * Returns this matcher's region end, that is, the first character that is
     * not considered for a match.
     *
     * @return the end of the region.
     */
    CARAPI RegionEnd(
        /* [out] */ Int32* end);

    /**
     * Indicates whether more input might change a successful match into an
     * unsuccessful one.
     *
     * @return true if (and only if) more input might change a successful match
     *         into an unsuccessful one.
     */
    CARAPI RequireEnd(
        /* [out] */ Boolean* result);

    /**
     * Indicates whether the last match hit the end of the input.
     *
     * @return true if (and only if) the last match hit the end of the input.
     */
    CARAPI HitEnd(
        /* [out] */ Boolean* hit);

    /**
     * Creates a matcher for a given combination of pattern and input. Both
     * elements can be changed later on.
     *
     * @param pattern
     *            the pattern to use.
     * @param input
     *            the input to use.
     */
    CARAPI constructor(
        /* [in] */ IPattern* pattern,
        /* [in] */ ICharSequence* input);

private:
    /**
     * Internal helper method to append a given string to a given string buffer.
     * If the string contains any references to groups, these are replaced by
     * the corresponding group's contents.
     *
     * @param s
     *            the string to append.
     */
    CARAPI AppendEvaluated(
        /* [in] */ StringBuffer* s,
        /* [in] */ const String& result);

    /**
     * Resets the Matcher. A new input sequence and a new region can be
     * specified. Results of a previous find get lost. The next attempt to find
     * an occurrence of the Pattern in the string will start at the beginning of
     * the region. This is the internal version of reset() to which the several
     * public versions delegate.
     *
     * @param input
     *            the input sequence.
     * @param start
     *            the start of the region.
     * @param end
     *            the end of the region.
     *
     * @return the matcher itself.
     */
    CARAPI Reset(
        /* [in] */ ICharSequence* input,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI_(void) ResetForInput();

    /**
     * Makes sure that a successful match has been made. Is invoked internally
     * from various places in the class.
     *
     * @throws IllegalStateException
     *             if no successful match has been made.
     */
    CARAPI EnsureMatch();

    static CARAPI_(Boolean) FindImpl(
        /* [in] */ RegexMatcher* matcher,
        /* [in] */ const String& s,
        /* [in] */ Int32 startIndex,
        /* [in] */ ArrayOf<Int32>* offsets);

    static CARAPI_(Boolean) FindNextImpl(
        /* [in] */ RegexMatcher* matcher,
        /* [in] */ const String& s,
        /* [in] */ ArrayOf<Int32>* offsets);

    static CARAPI_(Int32) GroupCountImpl(
        /* [in] */ RegexMatcher* matcher);

    static CARAPI_(Boolean) HitEndImpl(
        /* [in] */ RegexMatcher* matcher);

    static CARAPI_(Boolean) LookingAtImpl(
        /* [in] */ RegexMatcher* matcher,
        /* [in] */ const String& s,
        /* [in] */ ArrayOf<Int32>* offsets);

    static CARAPI_(Boolean) MatchesImpl(
        /* [in] */ RegexMatcher* matcher,
        /* [in] */ const String& s,
        /* [in] */ ArrayOf<Int32>* offsets);

    static CARAPI OpenImpl(
        /* [in] */ RegexPattern* pattern,
        /* [out] */ RegexMatcher** result);

    static CARAPI_(Boolean) RequireEndImpl(
        /* [in] */ RegexMatcher* matcher);

    static CARAPI_(void) SetInputImpl(
        /* [in] */ RegexMatcher* matcher,
        /* [in] */ const String& s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    static CARAPI_(void) UseAnchoringBoundsImpl(
        /* [in] */ RegexMatcher* matcher,
        /* [in] */ Boolean value);

    static CARAPI_(void) UseTransparentBoundsImpl(
        /* [in] */ RegexMatcher* matcher,
        /* [in] */ Boolean value);

private:
    /**
     * Holds the pattern, that is, the compiled regular expression.
     */
    AutoPtr<IPattern> mPattern;

    /**
     * Holds the handle for the native version of the pattern.
     */
    RegexMatcher* mNativeMatcher;

    /**
     * Holds the input text.
     */
    String mInput;

    /**
     * Holds the start of the region, or 0 if the matching should start at the
     * beginning of the text.
     */
    Int32 mRegionStart;

    /**
     * Holds the end of the region, or input.length() if the matching should
     * go until the end of the input.
     */
    Int32 mRegionEnd;

    /**
     * Holds the position where the next append operation will take place.
     */
    Int32 mAppendPos;

    /**
     * Reflects whether a match has been found during the most recent find
     * operation.
     */
    Boolean mMatchFound;

    /**
     * Holds the offsets for the most recent match.
     */
    AutoPtr<ArrayOf<Int32> > mMatchOffsets;

    /**
     * Reflects whether the bounds of the region are anchoring.
     */
    Boolean mAnchoringBounds;

    /**
     * Reflects whether the bounds of the region are transparent.
     */
    Boolean mTransparentBounds;
};

} // namespace Regex
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_REGEX_REGEX_CMATCHER_H__
