
#include "dialpad/SmartDialNameMatcher.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Dialpad {

AutoPtr<ISmartDialMap> CreateDialMap()
{
    AutoPtr<ILatinSmartDialMap> dialmap;
    CLatinSmartDialMap::New((ILatinSmartDialMap**)&dialmap);
    return ISmartDialMap::Probe(dialmap);
}

const AutoPtr<ISmartDialMap> LATIN_SMART_DIAL_MAP = CreateDialMap();

const Boolean SmartDialNameMatcher::ALLOW_INITIAL_MATCH = TRUE;

const Int32 SmartDialNameMatcher::INITIAL_LENGTH_LIMIT = 1;

CAR_INTERFACE_IMPL(SmartDialNameMatcher, Object, ISmartDialNameMatcher);

ECode SmartDialNameMatcher::constructor(
    /* [in] */ const String& query)
{
    return constructor(query, LATIN_SMART_DIAL_MAP);
}

ECode SmartDialNameMatcher::constructor(
    /* [in] */ const String& query,
    /* [in] */ ISmartDialMap* map)
{
    mQuery = query;
    mMap = map;

    return NOERROR;
}

void SmartDialNameMatcher::ConstructEmptyMask(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ Int32 length)
{
    for (Int32 i = 0; i < length; ++i) {
        builder->Append("0");
    }
}

void SmartDialNameMatcher::ReplaceBitInMask(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ ISmartDialMatchPosition* matchPos)
{
    SmartDialMatchPosition* position = (SmartDialMatchPosition*)matchPos
    for (Int32 i = position->mStart; i < position->mEnd; ++i) {
        builder->Replace(i, i + 1, "1");
    }
}

String SmartDialNameMatcher::NormalizeNumber(
    /* [in] */ const String& number,
    /* [in] */ ISmartDialMap* map)
{
    return NormalizeNumber(number, 0, map);
}

String SmartDialNameMatcher::NormalizeNumber(
    /* [in] */ const String& number,
    /* [in] */ Int32 offset,
    /* [in] */ ISmartDialMap* map)
{
    AutoPtr<StringBuilder> s = new StringBuilder();
    for (Int32 i = offset; i < number.GetLength(); i++) {
        Char32 ch = number.GetChar(i);
        Boolean result;
        if (map->IsValidDialpadNumericChar(ch, &result), result) {
            s->Append(ch);
        }
    }
    return s->ToString();
}

ECode SmartDialNameMatcher::MatchesNumber(
    /* [in] */ String phoneNumber,
    /* [in] */ String query,
    /* [in] */ Boolean useNanp,
    /* [out] */ ISmartDialMatchPosition** position)
{
    VALIDATE_NOT_NULL(position);

    AutoPtr<StringBuilder> builder = new StringBuilder();
    ConstructEmptyMask(builder, phoneNumber.GetLength());
    mPhoneNumberMatchMask = builder->ToString();

    // Try matching the number as is
    AutoPtr<ISmartDialMatchPosition> matchPos = MatchesNumberWithOffset(phoneNumber, query, 0);
    if (matchPos == NULL) {
        AutoPtr<ISmartDialPrefixPhoneNumberTokens> phoneNumberTokens =
                SmartDialPrefix::ParsePhoneNumber(phoneNumber);

        if (phoneNumberTokens == NULL) {
            *position = matchPos;
            REFCOUNT_ADD(*position);
            return NOERROR;
        }
        if (((SmartDialPrefix::PhoneNumberTokens*)phoneNumberTokens)->mCountryCodeOffset != 0) {
            matchPos = matchesNumberWithOffset(phoneNumber, query,
                    ((SmartDialPrefix::PhoneNumberTokens*)phoneNumberTokens)->mCountryCodeOffset);
        }
        if (matchPos == NULL
            && ((SmartDialPrefix::PhoneNumberTokens*)phoneNumberTokens)->mNanpCodeOffset != 0
            && useNanp) {
            matchPos = MatchesNumberWithOffset(phoneNumber, query,
                    ((SmartDialPrefix::PhoneNumberTokens*)phoneNumberTokens)->mNanpCodeOffset);
        }
    }
    if (matchPos != NULL) {
        ReplaceBitInMask(builder, matchPos);
        mPhoneNumberMatchMask = builder->ToString();
    }
    *position = matchPos;
    REFCOUNT_ADD(*position);
    return NOERROR;
}

ECode SmartDialNameMatcher::MatchesNumber(
    /* [in] */ String phoneNumber,
    /* [out] */ ISmartDialMatchPosition** position)
{
    VALIDATE_NOT_NULL(position);

    return MatchesNumber(phoneNumber, mQuery, TRUE, position);
}

ECode SmartDialNameMatcher::MatchesNumber(
    /* [in] */ String phoneNumber,
    /* [in] */ String query,
    /* [out] */ ISmartDialMatchPosition** position)
{
    VALIDATE_NOT_NULL(position);

    return MatchesNumber(phoneNumber, query, TRUE, position);
}

AutoPtr<ISmartDialMatchPosition> SmartDialNameMatcher::MatchesNumberWithOffset(
    /* [in] */ const String& phoneNumber,
    /* [in] */ const String& query,
    /* [in] */ Int32 offset)
{
    if (TextUtils::IsEmpty(phoneNumber) || TextUtils::IsEmpty(query)) {
        return NULL;
    }
    Int32 queryAt = 0;
    Int32 numberAt = offset;
    for (Int32 i = offset; i < phoneNumber.GetLength(); i++) {
        if (queryAt == query.GetLength()) {
            break;
        }
        Char32 ch = phoneNumber.GetChar(i);
        Boolean result;
        if (mMap->IsValidDialpadNumericChar(ch, &result), result) {
            if (ch != query.GetChar(queryAt)) {
                return NULL;
            }
            queryAt++;
        }
        else {
            if (queryAt == 0) {
                // Found a separator before any part of the query was matched, so advance the
                // offset to avoid prematurely highlighting separators before the rest of the
                // query.
                // E.g. don't highlight the first '-' if we're matching 1-510-111-1111 with
                // '510'.
                // However, if the current offset is 0, just include the beginning separators
                // anyway, otherwise the highlighting ends up looking weird.
                // E.g. if we're matching (510)-111-1111 with '510', we should include the
                // first '('.
                if (offset != 0) {
                    offset++;
                }
            }
        }
        numberAt++;
    }
    AutoPtr<ISmartDialMatchPosition> position;
    CSmartDialMatchPosition::New(0 + offset,
            numberAt, (ISmartDialMatchPosition**)&position);
    return position;
}

Boolean SmartDialNameMatcher::MatchesCombination(
    /* [in] */ const String& displayName,
    /* [in] */ const String& query,
    /* [in] */ IArrayList* matchList)
{
    AutoPtr<StringBuilder> builder = new StringBuilder();
    ConstructEmptyMask(builder, displayName.GetLength());
    mNameMatchMask = builder->ToString();
    const Int32 nameLength = displayName.GetLength();
    const Int32 queryLength = query.GetLength();

    if (nameLength < queryLength) {
        return FALSE;
    }

    if (queryLength == 0) {
        return FALSE;
    }

    // The current character index in displayName
    // E.g. 3 corresponds to 'd' in "Fred Smith"
    Int32 nameStart = 0;

    // The current character in the query we are trying to match the displayName against
    Int32 queryStart = 0;

    // The start position of the current token we are inspecting
    Int32 tokenStart = 0;

    // The number of non-alphabetic characters we've encountered so far in the current match.
    // E.g. if we've currently matched 3733764849 to (Fred Smith W)illiam, then the
    // seperatorCount should be 2. This allows us to correctly calculate offsets for the match
    // positions
    Int32 seperatorCount = 0;

    AutoPtr<IArrayList> partial;
    CArrayList::New((IArrayList**)&partial);
    // Keep going until we reach the end of displayName
    while (nameStart < nameLength && queryStart < queryLength) {
        Char32 ch = displayName.GetChar(nameStart);
        // Strip diacritics from accented characters if any
        mMap->NormalizeCharacter(ch, &ch);
        Boolean result;
        if (mMap->IsValidDialpadCharacter(ch, &result), result) {
            if (mMap->IsValidDialpadAlphabeticChar(ch, &result), result) {
                mMap->GetDialpadNumericCharacter(ch, &ch);
            }
            if (ch != query.GetChar(queryStart)) {
                // Failed to match the current character in the query.

                // Case 1: Failed to match the first character in the query. Skip to the next
                // token since there is no chance of this token matching the query.

                // Case 2: Previous characters in the query matched, but the current character
                // failed to match. This happened in the middle of a token. Skip to the next
                // token since there is no chance of this token matching the query.

                // Case 3: Previous characters in the query matched, but the current character
                // failed to match. This happened right at the start of the current token. In
                // this case, we should restart the query and try again with the current token.
                // Otherwise, we would fail to match a query like "964"(yog) against a name
                // Yo-Yoghurt because the query match would fail on the 3rd character, and
                // then skip to the end of the "Yoghurt" token.

                Char32 ch1;
                mMap->NormalizeCharacter(displayName.GetChar(nameStart - 1), &ch1);
                if (queryStart == 0 || mMap->IsValidDialpadCharacter(ch1, &result), result) {
                    // skip to the next token, in the case of 1 or 2.
                    mMap->NormalizeCharacter(displayName.GetChar(nameStart), &ch1);
                    while (nameStart < nameLength &&
                            mMap->IsValidDialpadCharacter(ch1, &result), result) {
                        nameStart++;
                        mMap->NormalizeCharacter(displayName.GetChar(nameStart), &ch1);
                    }
                    nameStart++;
                }

                // Restart the query and set the correct token position
                queryStart = 0;
                seperatorCount = 0;
                tokenStart = nameStart;
            }
            else {
                if (queryStart == queryLength - 1) {

                    // As much as possible, we prioritize a full token match over a sub token
                    // one so if we find a full token match, we can return right away
                    AutoPtr<ISmartDialMatchPosition> position;
                    CSmartDialMatchPosition::New(
                            tokenStart, queryLength + tokenStart + seperatorCount,
                            (ISmartDialMatchPosition**)&position);
                    matchList->Add(position);
                    Int32 size;
                    matchList->GetSize(&size);
                    for (Int32 i = 0; i < size; i++) {
                        AutoPtr<IInterface> match;
                        matchList->Get(i, (IInterface**)&match);
                        ReplaceBitInMask(builder, ISmartDialMatchPosition::Probe(match));
                    }
                    mNameMatchMask = builder->ToString();
                    return TRUE;
                }
                else if (ALLOW_INITIAL_MATCH && queryStart < INITIAL_LENGTH_LIMIT) {
                    // we matched the first character.
                    // branch off and see if we can find another match with the remaining
                    // characters in the query string and the remaining tokens
                    // find the next separator in the query string
                    Int32 j;
                    Boolean result;
                    for (j = nameStart; j < nameLength; j++) {
                        Char32 ch;
                        mMap->NormalizeCharacter(displayName.GetChar(j), &ch);
                        if (mMap->IsValidDialpadCharacter(ch, &result), !result) {
                            break;
                        }
                    }
                    // this means there is at least one character left after the separator
                    if (j < nameLength - 1) {
                        const String remainder = displayName.Substring(j + 1);
                        AutoPtr<IArrayList> partialTemp;
                        CArrayList::New((IArrayList**)&partialTemp);
                        if (MatchesCombination(
                                remainder, query.Substring(queryStart + 1), partialTemp)) {

                            // store the list of possible match positions
                            SmartDialMatchPosition::AdvanceMatchPositions(partialTemp, j + 1);
                            AutoPtr<ISmartDialMatchPosition> position;
                            CSmartDialMatchPosition::New(
                                    nameStart, nameStart + 1,
                                    (ISmartDialMatchPosition**)&position);
                            partialTemp->Add(0, position);
                            // we found a partial token match, store the data in a
                            // temp buffer and return it if we end up not finding a full
                            // token match
                            partial = partialTemp;
                        }
                    }
                }
                nameStart++;
                queryStart++;
                // we matched the current character in the name against one in the query,
                // continue and see if the rest of the characters match
            }
        }
        else {
            // found a separator, we skip this character and continue to the next one
            nameStart++;
            if (queryStart == 0) {
                // This means we found a separator before the start of a token,
                // so we should increment the token's start position to reflect its true
                // start position
                tokenStart = nameStart;
            }
            else {
                // Otherwise this separator was found in the middle of a token being matched,
                // so increase the separator count
                seperatorCount++;
            }
        }
    }
    // if we have no complete match at this point, then we attempt to fall back to the partial
    // token match(if any). If we don't allow initial matching (ALLOW_INITIAL_MATCH = false)
    // then partial will always be empty.
    Boolean empty;
    if (partial->IsEmpty(&empty), !empty) {
        matchList->AddAll(partial);

        Int32 size;
        matchList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> match;
            matchList->Get(i, (IInterface**)&match);
            ReplaceBitInMask(builder, ISmartDialMatchPosition::Probe(match));
        }
        mNameMatchMask = builder.ToString();
        return TRUE;
    }
    return FALSE;
}

ECode SmartDialNameMatcher::Matches(
    /* [in] */ const String& displayName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    mMatchPositions->Clear();
    *result = MatchesCombination(displayName, mQuery, mMatchPositions);

    return NOERROR;
}

ECode SmartDialNameMatcher::GetMatchPositions(
    /* [out] */ IArrayList** positions)
{
    VALIDATE_NOT_NULL(positions);

    return CArrayList::New(mMatchPositions, positions);
}

ECode SmartDialNameMatcher::SetQuery(
    /* [in] */ const String& query)
{
    mQuery = query;
    return NOERROR;
}

ECode SmartDialNameMatcher::GetNameMatchPositionsInString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNameMatchMask
    return NOERROR;
}

ECode SmartDialNameMatcher::GetNumberMatchPositionsInString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPhoneNumberMatchMask
    return NOERROR;
}

ECode SmartDialNameMatcher::GetQuery(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mQuery
    return NOERROR;
}

} // Dialpad
} // Dialer
} // Apps
} // Elastos
